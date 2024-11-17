QT       += core gui websockets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    kamera.cpp \
    main.cpp \
    mainwindow.cpp \
    websocketserver.cpp

HEADERS += \
    kamera.h \
    mainwindow.h \
    websocketserver.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/fingerprints_sdk_x86_20241113_v1.0.0.2/lib/ -lZW_Device
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/fingerprints_sdk_x86_20241113_v1.0.0.2/lib/ -lZW_Device

message("Library path: $$PWD/fingerprints_sdk_x86_20241113_v1.0.0.2/lib/")

INCLUDEPATH += $$PWD/fingerprints_sdk_x86_20241113_v1.0.0.2/include
DEPENDPATH += $$PWD/fingerprints_sdk_x86_20241113_v1.0.0.2/include
