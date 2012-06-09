#-------------------------------------------------
#
# Project created by QtCreator 2010-08-10T16:54:41
#
#-------------------------------------------------
TARGET = CVmob
TEMPLATE = app
QT += core gui

HEADERS += \
    view/aboutdialog.h \
    view/cvmobmainwindow.h \
    view/videoview.h \
    model/cvmobvideomodel.hpp \
    model/distancesproxymodel.hpp \
    view/playbar.hpp
SOURCES += \
    main.cpp \
    view/aboutdialog.cpp \
    view/cvmobmainwindow.cpp \
    view/videoview.cpp \
    model/cvmobvideomodel.cpp \
    model/distancesproxymodel.cpp \
    view/playbar.cpp

FORMS    += \
    view/aboutDialog.ui \
    view/cvmobmainwindow.ui \
    view/playbar.ui

TRANSLATIONS += pt_BR_cvmob.ts

# The link libraries vary a lot, depending on the system,
# so you'll have to modify this part of the .pro in order
# to compile CVmob.

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
    LIBS += -L/usr/lib/libopencv_core.so -L/usr/lib/libopencv_imgproc.so\
        -L/usr/lib/libopencv_highgui.so -L/usr/lib/libopencv_ml.so \
        -L/usr/lib/libopencv_video.so -L/usr/lib/libopencv_features2d.so \
        -L/usr/lib/libopencv_calib3d.so -L/usr/lib/libopencv_objdetect.so \
        -L/usr/lib/libopencv_contrib.so -L/usr/lib/libopencv_legacy.so \
        -L/usr/lib/libopencv_flann.so \
        -lQtSvg
    INCLUDEPATH += qwt
    LIBS += -L/usr/lib/libqwt.so -L/usr/lib/libqwtmathml.so
}

OTHER_FILES += \
    pt_BR_cvmob.ts \
    pt_BR_cvmob.qm

RESOURCES += \
    cvmob_resource.qrc
