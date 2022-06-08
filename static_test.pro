QT += core gui
QT += network
QT += sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    Common/BaseWindow.cpp \
    Common/TitleBar.cpp \
    Common/basecontroller.cpp \
    Lib/HttpClient.cpp \
    main.cpp \
    dialog.cpp \
    syncwoker.cpp

HEADERS += \
    Common/BaseWindow.h \
    Common/TitleBar.h \
    Common/basecontroller.h \
    Lib/HttpClient.h \
    dialog.h \
    syncwoker.h

FORMS += \
    dialog.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    Resources.qrc
RC_ICONS = mfc.ico
