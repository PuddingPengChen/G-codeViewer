QT       += core gui
QT       += svg
QT       += opengl
QT       += network


RESOURCES += \
    img.qrc

#DISTFILES += \
#    logo2.ico \
#    pynxl.rc \


HEADERS += \
    geometric.h \
    OS_Function.h \
    OS_GL.h \
    Interface/graph.h \
    Interface/greentech.h \
    Interface/loadingbar.h \
    Interface/worldview.h \
    ModelData/layoutprojectdata.h \
    ModelData/modeldata.h \
    ModelData/modelinstance.h \
    ModelData/modelloader.h \
    ModelData/modelwriter.h \
    ModelData/triangle3d.h \
    ModelData/verticaltricontainer.h \
    SupportEngine/supportstructure.h \
    SliceEngine/clipper/clipper.hpp \
    SliceEngine/modelFile/modelFile.h \
    SliceEngine/utils/floatpoint.h \
    SliceEngine/utils/gettime.h \
    SliceEngine/utils/intpoint.h \
    SliceEngine/utils/logoutput.h \
    SliceEngine/utils/polygon.h \
    SliceEngine/utils/polygondebug.h \
    SliceEngine/utils/socket.h \
    SliceEngine/bridge.h \
    SliceEngine/comb.h \
    SliceEngine/fffProcessor.h \
    SliceEngine/gcodeExport.h \
    SliceEngine/includes.h \
    SliceEngine/infill.h \
    SliceEngine/inset.h \
    SliceEngine/layerPart.h \
    SliceEngine/multiVolumes.h \
    SliceEngine/optimizedModel.h \
    SliceEngine/pathOrderOptimizer.h \
    SliceEngine/polygonOptimizer.h \
    SliceEngine/raft.h \
    SliceEngine/settings.h \
    SliceEngine/skin.h \
    SliceEngine/skirt.h \
    SliceEngine/sliceDataStorage.h \
    SliceEngine/slicer.h \
    SliceEngine/support.h \
    SliceEngine/timeEstimate.h \
    ModelData/model.h \
    ModelData/point3d.h


SOURCES += \
    geometric.cpp \
    main.cpp \
    OS_Function.cpp \
    Interface/graph.cpp \
    Interface/greentech.cpp \
    Interface/loadingbar.cpp \
    Interface/worldview.cpp \
    ModelData/layoutprojectdata.cpp \
    ModelData/modeldata.cpp \
    ModelData/modelinstance.cpp \
    ModelData/modelloader.cpp \
    ModelData/modelwriter.cpp \
    ModelData/triangle3d.cpp \
    ModelData/verticaltricontainer.cpp \
    SupportEngine/supportstructure.cpp \
    SliceEngine/clipper/clipper.cpp \
    SliceEngine/modelFile/modelFile.cpp \
    SliceEngine/utils/gettime.cpp \
    SliceEngine/utils/logoutput.cpp \
    SliceEngine/utils/socket.cpp \
    SliceEngine/bridge.cpp \
    SliceEngine/comb.cpp \
    SliceEngine/gcodeExport.cpp \
    SliceEngine/infill.cpp \
    SliceEngine/inset.cpp \
    SliceEngine/layerPart.cpp \
    SliceEngine/optimizedModel.cpp \
    SliceEngine/pathOrderOptimizer.cpp \
    SliceEngine/polygonOptimizer.cpp \
    SliceEngine/raft.cpp \
    SliceEngine/settings.cpp \
    SliceEngine/skin.cpp \
    SliceEngine/skirt.cpp \
    SliceEngine/slicer.cpp \
    SliceEngine/support.cpp \
    SliceEngine/timeEstimate.cpp \
    ModelData/model.cpp

FORMS += \
    Interface/greentech.ui
RC_FILE = pynxl.rc

