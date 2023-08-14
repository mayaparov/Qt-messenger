QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17
CONFIG += console

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    CColorSet/CColorSet.cpp \
    CNetworkWrapper/CNetworkWrapper.cpp \
    main.cpp \
    CMainWindow.cpp

HEADERS += \
    CClientData.h \
    CColorSet/CColorSet.h \
    CMainWindow.h \
    CNetworkWrapper/CNetworkWrapper.h

# Specify your boost dir here!
INCLUDEPATH += /Users/bloodberry/QT/6.4.2/macos/lib/boost
INCLUDEPATH += /usr/local/opt/openssl/include

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

TARGET = ProSec

# ~ APPLE AREA RL ~
ios {
  QMAKE_INFO_PLIST = Info_IOS.plist

  iosIcon.files = Resources/AppIco.icns
  QMAKE_BUNDLE_DATA += iosIcon
}

# ~ WINDOWS AREA RL ~
win32 {
    ICON += /Resources/AppIco.icns

    LIBS += -llibssl -llibcrypto
}

RESOURCES += \
    ProSec-IOS-Resources.qrc
