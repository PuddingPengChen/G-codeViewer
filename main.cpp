#include <QtGui/QApplication>
#include "./Interface/greentech.h"
#include "OS_Function.h"
#include <QSplashScreen>
#include <QElapsedTimer>
#include <QTextCodec>

int main(int argc, char *argv[])
{
    QApplication::setStyle(QStyleFactory::create("Fusion"));
    QApplication a(argc, argv);
     QTextCodec::setCodecForTr(QTextCodec::codecForName("utf8"));

    QSplashScreen splash(QPixmap(":/image/splash.png"));
    splash.setDisabled(true);
    splash.show();
    QElapsedTimer t;
    t.start();
    while(t.elapsed()<2000)
    {
        QCoreApplication::processEvents();
    }

    qDebug() << "Program Start";
    qDebug() << "Relevent Used Application Directories";
    qDebug() << "   EXECUTABLE_DIR: " << CROSS_OS_GetDirectoryFromLocationTag("EXECUTABLE_DIR");
    qDebug() << "   APPLICATION_DIR: " << CROSS_OS_GetDirectoryFromLocationTag("APPLICATION_DIR");
    qDebug() << "   TEMP_DIR: " << CROSS_OS_GetDirectoryFromLocationTag("TEMP_DIR");
    qDebug() << "   DOCUMENTS_DIR: " << CROSS_OS_GetDirectoryFromLocationTag("DOCUMENTS_DIR");
    GreenTech w;

//    w.setWindowOpacity(0.7);
    w.show();
    splash.finish(&w);
    return a.exec();
}
