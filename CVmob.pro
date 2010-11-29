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

#OTHER_FILES += \
#    src/model/exporter/exporter.event

LIBS += -lcxcore \
    -lcv \
    -lhighgui \
    -lcvaux \
    -lqwt \
    -lQtSvg
INCLUDEPATH += qwt
