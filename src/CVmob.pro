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
    controller/FacadeController.h \
    view/imageviewer.h \
    view/options.h \
    view/aboutdialog.h \
    view/reportdialog.h \
    view/cvmobmainwindow.h \
    model/fixedpointstablemodel.h
SOURCES += model/exporter/ExportText.cpp \
    model/exporter/IExportStrategy.cpp \
    model/PhisicsCalc.cpp \
    view/graphs/Plot.cpp \
    model/OpenCV/ProxyOpenCv.cpp \
    controller/FacadeController.cpp \
    main.cpp \
    view/imageviewer.cpp \
    view/options.cpp \
    view/aboutdialog.cpp \
    model/OpenCV/Angle.cpp \
    view/reportdialog.cpp \
    view/cvmobmainwindow.cpp \
    model/fixedpointstablemodel.cpp

FORMS    += \
    view/reportdialog.ui \
    view/DialogCalibration.ui \
    view/dialog.ui \
    view/aboutDialog.ui \
    view/cvmobmainwindow.ui

TRANSLATIONS += pt_BR_cvmob.ts

#OTHER_FILES += \
#    model/exporter/exporter.event

LIBS += -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_ml \
    -lopencv_video -lopencv_features2d -lopencv_calib3d -lopencv_objdetect \
    -lopencv_contrib -lopencv_legacy -lopencv_flann \
    -lqwt \
    -lQtSvg
INCLUDEPATH += qwt

OTHER_FILES += \
    pt_BR_cvmob.ts \
    pt_BR_cvmob.qm

RESOURCES += \
    cvmob_resource.qrc
