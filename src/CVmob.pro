#-------------------------------------------------
#
# Project created by QtCreator 2010-08-10T16:54:41
#
#-------------------------------------------------
TARGET = CVmob
TEMPLATE = app
QT += core gui

HEADERS += model/OpenCV/Angle.h \
    model/exporter/ExportText.h \
    model/exporter/IExportStrategy.h \
    model/PhisicsCalc.h \
    view/graphs/EnumGraphs.h \
    view/graphs/Plot.h \
    view/graphs/cvZoomer.h \
    model/OpenCV/Point.h \
    model/OpenCV/ProxyOpenCv.h \
    view/options.h \
    view/aboutdialog.h \
    view/reportdialog.h \
    view/cvmobmainwindow.h \
    view/videoview.h \
    model/cvmobvideomodel.hpp
SOURCES += model/exporter/ExportText.cpp \
    model/exporter/IExportStrategy.cpp \
    model/PhisicsCalc.cpp \
    view/graphs/Plot.cpp \
    model/OpenCV/ProxyOpenCv.cpp \
    main.cpp \
    view/options.cpp \
    view/aboutdialog.cpp \
    model/OpenCV/Angle.cpp \
    view/reportdialog.cpp \
    view/cvmobmainwindow.cpp \
    view/videoview.cpp \
    model/cvmobvideomodel.cpp

FORMS    += \
    view/reportdialog.ui \
    view/DialogCalibration.ui \
    view/aboutDialog.ui \
    view/cvmobmainwindow.ui

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
