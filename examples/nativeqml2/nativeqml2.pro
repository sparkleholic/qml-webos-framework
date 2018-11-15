TARGET = com.app.nativeqml2
  
CONFIG += qt
QT += gui qml quick
  
CONFIG += link_pkgconfig
PKGCONFIG += luna-service2 glib-2.0 pbnjson_cpp PmLogLib

SOURCES += main.cpp
  
#INSTALL_APPDIR = $${WEBOS_INSTALL_WEBOS_APPLICATIONSDIR}/com.app.nativeqml2
INSTALL_APPDIR = /usr/palm/applications/com.app.nativeqml2

target.path = $${INSTALL_APPDIR}
  
icon.path = $${INSTALL_APPDIR}
icon.files = icon.png
  
appinfo.path = $${INSTALL_APPDIR}
appinfo.files = appinfo.json
  
INSTALLS += target icon appinfo
