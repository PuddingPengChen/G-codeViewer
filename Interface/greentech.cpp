#include "greentech.h"
#include "ui_greentech.h"
#include "../ModelData/layoutprojectdata.h"
#include "loadingbar.h"
#include <QFileDialog>
#include <QFile>
#include <QMessageBox>
#include <QVector3D>
#include <QGLWidget>
#include <QDebug>
#include <QGraphicsView>
#include <QGraphicsProxyWidget>
#include "../ModelData/modeldata.h"
#include "../ModelData/modelinstance.h"
#include "OS_Function.h"
#include "../ModelData/modelwriter.h"


GreenTech::GreenTech(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::GreenTech)
{
    ui->setupUi(this);

    //Initialize project data
    project = new LayoutProjectData();
    fileType = "STL";
    gcodeModel = new Model();
    project->pMain = this;
//    m_ModelList = m_ModelEdit->GetModelList();
    m_ModelList = new QListWidget();
    processor = new fffProcessor(config);
    this->setWindowTitle(tr("三维科技"));
    setWindowIcon(QIcon(":/image/logo2.ico"));
    pWorldView = new WorldView(0,this);             //WorldVew is a graphicsSence
    QGraphicsView *view = ui->graphicsView;         //操控ui上的graphicsView


    view->setViewport(new QGLWidget(QGLFormat(QGL::SampleBuffers)));

    view->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    view->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    view->setScene(pWorldView);
    view->show();


    //build the interface initially - different than update interface..
    BuildInterface();
//    UpdateInterface();
}

GreenTech::~GreenTech()
{
    unsigned int m;
    for(m=0;m<ModelDataList.size();m++)
    {
        delete ModelDataList[m];
    }
    delete project;
    delete pWorldView;
    delete ui;
}


//returns a list of the currently selected instances
std::vector<ModelInstance*> GreenTech::GetSelectedInstances()
{
    std::vector<ModelInstance*> insts;
    int i;
    for(i=0;i<m_ModelList->selectedItems().size();i++)
    {
        insts.push_back(FindInstance(m_ModelList->selectedItems()[i]));
    }
    return insts;
}

std::vector<ModelInstance*> GreenTech::GetAllInstances()
{
    unsigned int d, i;
    std::vector<ModelInstance*> allInsts;

    for(d = 0; d < ModelDataList.size(); d++)
    {
        for(i = 0; i < ModelDataList[d]->instList.size(); i++)
        {
            allInsts.push_back(ModelDataList[d]->instList[i]);
        }
    }

    return allInsts;
}

//////////////////////////////////////////////////////
//Public Slots
//////////////////////////////////////////////////////
//interface
void GreenTech::BuildInterface()
{

    connect(ui->actionOpen,SIGNAL(triggered()),this,SLOT(AddModel()));


//    connect(ui->actionSave_2,SIGNAL(triggered()),this,SLOT(PromptExportToSTL()));


}
//selection
void GreenTech::RefreshSelectionsFromList()
{
    int i;
    for(i=0;i<m_ModelList->count();i++)
    {
        ModelInstance* inst = FindInstance(m_ModelList->item(i));
        if(inst == NULL)
            return;

        if(!m_ModelList->item(i)->isSelected())
        {
            DeSelect(FindInstance(m_ModelList->item(i)));
        }
        else if(m_ModelList->item(i)->isSelected())
        {
            Select(FindInstance(m_ModelList->item(i)));
        }
    }
}
void GreenTech::Select(ModelInstance* inst)
{
    //qDebug() << inst << "added to selection";
    inst->SetSelected(true);

}
void GreenTech::DeSelect(ModelInstance* inst)
{
    //qDebug() << inst << "removed from selection";
    inst->SetSelected(false);

}
void GreenTech::SelectOnly(ModelInstance* inst)
{
    DeSelectAll();
    Select(inst);
}


void GreenTech::DeSelectAll()
{
    unsigned int m;
    unsigned int i;
    for(m=0;m<ModelDataList.size();m++)
    {
        for(i=0;i<ModelDataList[m]->instList.size();i++)
        {
            DeSelect(ModelDataList[m]->instList[i]);
        }
    }
}

//model
ModelInstance* GreenTech::AddModel(QString filepath, bool bypassVisuals)
{
    QSettings settings;
    if(filepath.isEmpty())
    {
        filepath = QFileDialog::getOpenFileName(this,
            tr("Open Model"), settings.value("WorkingDir").toString(), tr("Models (*.stl *.gcode)"));


        //cancel button
        if(filepath.isEmpty())
            return NULL;
    }
    //by this point we should have a valid file path, if we dont - abort.
    if(!QFileInfo(filepath).exists())
    {
        return NULL;
    }
    QFileInfo pi(filepath);
    if(pi.suffix()=="stl")
    {
        fileType = "STL";

        sliceName = filepath;
        //if the file has already been opened and is in the project, we dont want to load in another! instead we want to make a new instance
        for(unsigned int i = 0; i < ModelDataList.size(); i++)
        {
            if(ModelDataList[i]->GetFilePath() == filepath)
            {
                return ModelDataList[i]->AddInstance();//make a new instance
            }
        }

        ModelData* pNewModel = new ModelData(this,bypassVisuals);

        bool success = pNewModel->LoadIn(filepath);
        if(!success)
        {
            delete pNewModel;
            return NULL;
        }

        //update registry
        settings.setValue("WorkingDir",QFileInfo(filepath).absolutePath());

        //add to the list
        ModelDataList.push_back(pNewModel);
        pWorldView->SetModelSelected(true);
        //make an Instance of the model!
        ModelInstance* pNewInst = pNewModel->AddInstance();
        project->UpdateZSpace();

        //select it too
    //    UpdateGreentech_bounding(pNewInst->GetMaxBound(),pNewInst->GetMinBound());
        SelectOnly(pNewInst);
        ui->Slice->setEnabled(true);
        qDebug()<<tr("ModelDataList.size = %1").arg(ModelDataList.size());
        return pNewInst;
    }
    else if(pi.suffix()=="gcode")
    {
        fileType = "GCODE";
        QMessageBox box;
        box.setWindowTitle(tr("提示"));
        box.setText(tr("即将解析G-code并进行渲染，请耐心等候片刻"));
        box.exec();
        Enable_User_Waiting_Cursor();
        gcodeModel->loadFile(filepath);
        Disable_User_Waiting_Cursor();
        QMessageBox box1;
        box1.setWindowTitle(tr("报告"));
        box1.setText(tr("G-code装载解析完毕！"));
        box1.exec();
    }
    pWorldView->setFileType(fileType);
}
void GreenTech::RemoveAllInstances()
{
    unsigned int m;
    unsigned int i;
    ui->Slice->setEnabled(false);
    if(fileType=="STL")
    {
        std::vector<ModelInstance*> allinstlist;
        for(m=0;m<this->ModelDataList.size();m++)
        {
            ModelDataList[m]->loadedcount = 0;//also reset the index counter for instances!
            for(i=0;i<ModelDataList[m]->instList.size();i++)
            {
                allinstlist.push_back(ModelDataList[m]->instList[i]);
            }
        }
        for(i=0;i<allinstlist.size();i++)
        {
            delete allinstlist[i];
        }

        CleanModelData();
    }
    else if(fileType == "GCODE")
    {
        gcodeModel->m_points.clear();
        fileType = "STL";
    }
    pWorldView->setFileType(fileType);

}
void GreenTech::CleanModelData()
{
    unsigned int m;
    std::vector<ModelData*> templist;
    for(m=0;m<ModelDataList.size();m++)
    {
        if(ModelDataList[m]->instList.size() > 0)
        {
            templist.push_back(ModelDataList[m]);
        }
        else
        {
            delete ModelDataList[m];
        }
    }
    ModelDataList.clear();
    pWorldView->AutoSupportFlag = false;
    ModelDataList = templist;
}

void GreenTech::AddTagToModelList(QListWidgetItem* item)
{
   // ui.ModelList->addItem(item);        //very important
    m_ModelList->addItem(item);
}

ModelInstance* GreenTech::FindInstance(QListWidgetItem* item)
{
    unsigned int d;
    unsigned int i;
    for(d=0;d<ModelDataList.size();d++)
    {
        for(i=0;i<ModelDataList[d]->instList.size();i++)
        {
            if(ModelDataList[d]->instList[i]->listItem == item)
            {
                return ModelDataList[d]->instList[i];
            }
        }
    }
    return NULL;
}

void GreenTech::SetSelectionScale(double x, double y, double z, int axis)
{
    int i;
    for(i=0;i<m_ModelList->selectedItems().size();i++)
    {
        ModelInstance* inst = FindInstance(m_ModelList->selectedItems()[i]);
        if(axis==0)
            inst->SetScale(QVector3D(x,y,z));
        else if(axis==1)
            inst->SetScale(QVector3D(x,inst->GetScale().y(),inst->GetScale().z()));
        else if(axis==2)
            inst->SetScale(QVector3D(inst->GetScale().x(),y,inst->GetScale().z()));
        else if(axis==3)
            inst->SetScale(QVector3D(inst->GetScale().x(),inst->GetScale().y(),z));
    }
}
//修改模型的大小
void GreenTech::ChangeModelScale(double scale)
{

    SetSelectionScale(scale,0,0,1);
    SetSelectionScale(0,scale,0,2);
    SetSelectionScale(0,0,scale,3);

    for( int i = 0; i < GetSelectedInstances().size(); i++)
    {
        GetSelectedInstances()[i]->UpdateBounds();
        GetSelectedInstances()[i]->RestOnBuildSpace();
    }
//    UpdateInterface();
}
int GreenTech::PrintExportToSTL()
{
    QString filename;
//    QSettings settings;
    filename = "./print.stl";
//    qDebug()<<filename;
    QMessageBox msgBox;
    msgBox.setWindowTitle(tr("提示"));
    msgBox.setText("注意");
     msgBox.setInformativeText("即将进入打印控制界面");
     msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
     msgBox.setDefaultButton(QMessageBox::Yes);
    int ret = msgBox.exec();
    switch (ret)
    {
      case QMessageBox::Yes:
           ExportToSTL(filename);
           return 1;
          break;
      case QMessageBox::No:
           msgBox.close();
           return 0;
          break;
      default:
          return 0;
          break;
    }
    return 0;
}
 void GreenTech::PromptExportToSTL()
 {
     QString filename;
     QSettings settings;
     filename = CROSS_OS_GetSaveFileName(this, tr("Export To STL"),
                                         settings.value("WorkingDir").toString() + "/" + ProjectData()->GetFileName(),
                                         tr("stl (*.stl)"));

     if(filename.isEmpty())
         return;

     if(ExportToSTL(filename))
     {
         QMessageBox msgBox;
         msgBox.setText("Stl Export Complete");
         msgBox.exec();
     }
     else
     {
         QMessageBox msgBox;
         msgBox.setIcon(QMessageBox::Warning);
         msgBox.setText("Unable To Export stl File");
         msgBox.exec();
     }
 }
 bool GreenTech::ExportToSTL(QString filename)
 {
     unsigned int i;
     unsigned long int t;
     ModelInstance* pInst = NULL;
     Triangle3D* pTri = NULL;
     bool fileOpened;

     ModelWriter exporter(filename, fileOpened);
     if(!fileOpened)
         return false;

//     SetSupportMode(false);
     for(i = 0; i < GetAllInstances().size(); i++)
     {
         pInst = GetAllInstances().at(i);
         pInst->BakeGeometry(true);

         for(t = 0; t < pInst->triList.size(); t++)
         {
             pTri = pInst->triList[t];
             exporter.WriteNextTri(pTri);
         }

         pInst->UnBakeGeometry();
     }
     exporter.Finalize();
     return true;
 }


//////////////////////////////////////////////////////
//Private
//////////////////////////////////////////////////////


///////////////////////////////////////////////////
//Events 传递到pWorldaiew类里面去了
///////////////////////////////////////////////////
void GreenTech::keyPressEvent(QKeyEvent * event )
{

    pWorldView->keyPressEvent(event);
}
void GreenTech::keyReleaseEvent(QKeyEvent * event )
{
    pWorldView->keyReleaseEvent(event);
}
void GreenTech::mousePressEvent(QMouseEvent *event)
{
  //  pWorldView->mousePressEvent(event);
   event->accept();
}
void GreenTech::mouseReleaseEvent(QMouseEvent *event)
{
//    pWorldView->mouseReleaseEvent(event);
    event->accept();
}

void GreenTech::hideEvent(QHideEvent *event)
{
    emit eventHiding();

 //   pWorldView->pDrawTimer->stop();
    event->accept();
}
void GreenTech::showEvent(QShowEvent *event)
{

 //   pWorldView->pDrawTimer->start();
    showMaximized();
    event->accept();
}

void GreenTech::closeEvent ( QCloseEvent * event )
{
    //if th GreenTech is dirty - ask the user if they want to save.
    if(project->IsDirtied())
    {
        QMessageBox msgBox;
        msgBox.setWindowTitle(tr("提示"));
        msgBox.setText(tr("模型还没有保存"));
         msgBox.setInformativeText(tr("您确定要退出么？"));
         msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Ignore | QMessageBox::Cancel);
         msgBox.setDefaultButton(QMessageBox::Save);
        int ret = msgBox.exec();
        switch (ret)
        {
          case QMessageBox::Ignore:
                //nothing
              break;
          case QMessageBox::Cancel:
                event->ignore();
                return;
              break;
          default:
              break;
        }
    }
    //when we close the window - we want to make a new project.
    //because we might open the window again and we want a fresh start.
    event->accept();

}


void GreenTech::on_Slice_clicked()
{
    QString filename = QFileDialog::getSaveFileName(this,
        tr("保存切片"),
        "",
        tr("*.gcode"));
    if(filename.isEmpty())
    {
        return;
    }
    if(sliceName=="")
    {
        qDebug()<<"You didn't load any file";
    }
    else
    {
        char*  ch;
        QByteArray ba = sliceName.toLatin1();
        ch=ba.data();
        qDebug()<<sliceName;

        QByteArray ta = filename.toLatin1();
        char* target = ta.data();
        QMessageBox box1;
        box1.setWindowTitle(tr("提示栏"));
        box1.setText(tr("正在执行切片任务，请耐心等待。切片完毕后会有相关提示"));
        box1.exec();
        Enable_User_Waiting_Cursor();
        processor->setTargetFile(target);
        processor->processFile(ch);
        processor->finalize();
        Disable_User_Waiting_Cursor();
        QMessageBox box2;
        box2.setWindowTitle(tr("提示栏"));
        box2.setText(tr("  恭喜您，切片完成   "));
        box2.exec();
    }
}

void GreenTech::on_loadfle_clicked()
{
    AddModel();
}

void GreenTech::on_remove_clicked()
{
    RemoveAllInstances();
}
