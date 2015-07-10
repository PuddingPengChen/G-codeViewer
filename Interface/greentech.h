/*******************************************************************************
*                                                                              *
* Author    :  Peng Chen                                                       *
* Version   :  0.0.1                                                           *
* Date      :  12 July 2014                                                    *
* Website   :  http://www.3green-tech.com                                      *
* Copyright :  3green-tech 2010-2014                                           *
*                                                                              *
* Attributions:                                                                *
* This class was the Mainwindows,which include the QGraphicsCiew,and include   *
* the modeledit,supportedit,and printedit interface.All the big data struct    *
* are be used in this class                                                                             *
*******************************************************************************/
#ifndef GREENTECH_H
#define GREENTECH_H

#include <QMainWindow>
#include <QtGui>
#include "worldview.h"
#include "../SupportEngine/supportstructure.h"
#include "../SliceEngine/fffProcessor.h"
#include "./ModelData/model.h"

class LayoutProjectData;
class WorldView;
class ModelData;
class ModelInstance;
class SliceDebugger;
class SupportStructure;
class PrintEdit;

namespace Ui {
class GreenTech;
}

class GreenTech : public QMainWindow
{
    Q_OBJECT
public:
    explicit GreenTech(QWidget *parent = 0);
    ~GreenTech();
    WorldView* pWorldView;

    std::vector<ModelInstance*> GetAllInstances();
    std::vector<ModelInstance*> GetSelectedInstances();
    std::vector<ModelData*> GetAllModelData(){return ModelDataList;}
    LayoutProjectData* ProjectData(){return project;}
    QString getFileType(){return fileType;}

    //int GetLayerThick(){return LayThickness;}
    QVector3D minB;
    QVector3D maxB;

    Model* gcodeModel;

signals:
    void eventHiding();


public slots:


    //selection
    void RefreshSelectionsFromList();//searches through all active listitems and selects their corresponding instance;
    void Select(ModelInstance* inst);//selects the given instance
    void DeSelect(ModelInstance* inst);//de-selects the given instance
    void SelectOnly(ModelInstance* inst);//deselects eveything and selects only the instance
    void DeSelectAll();//de-selects all instances

    //interface
    void BuildInterface();


    //model
    ModelInstance* AddModel(QString filepath = "", bool bypassVisuals = false);
    void RemoveAllInstances();
    void CleanModelData();//cleans andy modeldata that does not have a instance!
    void AddTagToModelList(QListWidgetItem* item);

    ModelInstance* FindInstance(QListWidgetItem* item);//given a item you can find the connected instance



    //exporting
    void PromptExportToSTL();//export the whole GreenTech to a stl file.
    int PrintExportToSTL();
    bool ExportToSTL(QString filename);



    //events
    void keyPressEvent(QKeyEvent * event );
    void keyReleaseEvent(QKeyEvent * event );
    void mouseReleaseEvent(QMouseEvent * event);
    void mousePressEvent(QMouseEvent * event);

private slots:
//    void on_actionBack_View_triggered();

    void on_Slice_clicked();

    void on_loadfle_clicked();

    void on_remove_clicked();

private:
    Ui::GreenTech *ui;
    ConfigSettings config;
    fffProcessor *processor;

    LayoutProjectData* project;
    QString sliceName;

    QListWidget *m_ModelList;

    std::vector<ModelData*> ModelDataList;

    void SetSelectionScale(double x, double y, double z, int axis);
    void ChangeModelScale(double scale);
    //support mode
    ModelInstance* currInstanceInSupportMode;
    bool oldModelConstricted;//for raising models that are too close to the ground in support mode.
    QVector3D oldPan;
    QVector3D oldRot;
    float oldZoom;
    QString oldTool;


    QString fileType;

    std::vector<SupportStructure*> currSelectedSupports;//what supports are currently in selection.
    void hideEvent(QHideEvent *event);
    void showEvent(QShowEvent *event);
    void closeEvent(QCloseEvent * event );

};

#endif // GREENTECH_H
