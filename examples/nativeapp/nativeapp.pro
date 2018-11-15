TARGET = com.app.qt
  
CONFIG += qt
QT += widgets gui-private
  
CONFIG += link_pkgconfig
PKGCONFIG += luna-service2 glib-2.0 pbnjson_cpp PmLogLib
  
SOURCES += ServiceRequest.cpp main.cpp
HEADERS += ServiceRequest.h
  
  
#INSTALL_APPDIR = $${WEBOS_INSTALL_WEBOS_APPLICATIONSDIR}/com.app.qt
INSTALL_APPDIR = /usr/palm/applications/com.app.qt

target.path = $${INSTALL_APPDIR}
  
icon.path = $${INSTALL_APPDIR}
icon.files = icon.png
  
appinfo.path = $${INSTALL_APPDIR}
appinfo.files = appinfo.json
  
INSTALLS += target icon appinfo
