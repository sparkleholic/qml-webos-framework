TARGET = com.app.nativeqml
  
CONFIG += qt
QT += gui-private qml
  
CONFIG += link_pkgconfig
PKGCONFIG += luna-service2 glib-2.0 pbnjson_cpp PmLogLib
RESOURCES += qml.qrc

SOURCES += ServiceRequest.cpp main.cpp
HEADERS += ServiceRequest.h
  
  
#INSTALL_APPDIR = $${WEBOS_INSTALL_WEBOS_APPLICATIONSDIR}/com.app.nativeqml
INSTALL_APPDIR = /usr/palm/applications/com.app.nativeqml

target.path = $${INSTALL_APPDIR}
  
icon.path = $${INSTALL_APPDIR}
icon.files = icon.png
  
appinfo.path = $${INSTALL_APPDIR}
appinfo.files = appinfo.json
  
INSTALLS += target icon appinfo
