TARGET = com.app.nativenoname
  
CONFIG += qt
QT += gui qml

SOURCES += main.cpp
  
INSTALL_APPDIR = /usr/palm/applications/com.app.nativeqmlnoname

target.path = $${INSTALL_APPDIR}
  
icon.path = $${INSTALL_APPDIR}
icon.files = icon.png
  
appinfo.path = $${INSTALL_APPDIR}
appinfo.files = appinfo.json
  
INSTALLS += target icon appinfo
