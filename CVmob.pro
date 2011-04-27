#-------------------------------------------------
#
# Project created by QtCreator 2010-08-10T16:54:41
#
#-------------------------------------------------
TARGET = CVmob
TEMPLATE = app
QT += core gui

HEADERS += src/model/OpenCV/Angle.h \
    src/model/exporter/ExportText.h \
    src/model/exporter/IExportStrategy.h \
    src/model/PhisicsCalc.h \
    src/view/graphs/EnumGraphs.h \
    src/view/graphs/Plot.h \
    src/view/graphs/cvZoomer.h \
    src/model/OpenCV/Point.h \
    src/model/OpenCV/ProxyOpenCv.h \
    src/controller/FacadeController.h \
    src/view/CvMobMainWindow.h \
    src/view/imageviewer.h \
    src/view/options.h \
    src/view/aboutdialog.h \
    src/view/reportdialog.h
SOURCES += src/model/exporter/ExportText.cpp \
    src/model/exporter/IExportStrategy.cpp \
    src/model/PhisicsCalc.cpp \
    src/view/graphs/Plot.cpp \
    src/model/OpenCV/ProxyOpenCv.cpp \
    src/controller/FacadeController.cpp \
    src/view/CvMobMainWindow.cpp \
    src/main.cpp \
    src/view/imageviewer.cpp \
    src/view/options.cpp \
    src/view/aboutdialog.cpp \
    src/model/OpenCV/Angle.cpp \
    src/view/reportdialog.cpp

FORMS    += \
    src/view/reportdialog.ui \
    src/view/FormCvMob.ui \
    src/view/DialogCalibration.ui \
    src/view/dialog.ui \
    src/view/aboutDialog.ui

TRANSLATIONS += pt_BR_cvmob.ts

win32 {
    LIBS += \
        -LC:\OpenCV2.2\lib \
        -lopencv_core220 \
        -lopencv_highgui220 \
        -lopencv_imgproc220 \
        -lopencv_video220 \
        -LC:\Qwt-6.0.0-rc5\lib \

    INCLUDEPATH += C:\Qwt-6.0.0\include
    INCLUDEPATH += C:\OpenCV2.2\include
}
linux-g++-64 {
    LIBS += -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_ml \
        -lopencv_video -lopencv_features2d -lopencv_calib3d -lopencv_objdetect \
        -lopencv_contrib -lopencv_legacy -lopencv_flann \
        -lQtSvg
    INCLUDEPATH += qwt
    LIBS += -lqwt
}

OTHER_FILES += \
    pt_BR_cvmob.ts \
    pt_BR_cvmob.qm

RESOURCES += \
    cvmob_resource.qrc
