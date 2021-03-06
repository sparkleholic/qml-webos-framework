# Copyright (c) 2014-2018 LG Electronics, Inc.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
# SPDX-License-Identifier: Apache-2.0

TEMPLATE = app
TARGET = qml-runner

QT += qml quick gui-private network
CONFIG += c++11
CONFIG -= app_bundle

include(../../src/common/common.pri)

CONFIG += webos webos-service
WEBOS_SYSBUS_DIR = $$PWD/../../sysbus

SOURCES += main.cpp apploader.cpp ipcclient.cpp
HEADERS += apploader.h ipcclient.h


qml-profiler {
    DEFINES += QMLJSDEBUGGER
}

!no_webos_platform {
    CONFIG += link_pkgconfig
    PKGCONFIG += PmLogLib
    DEFINES += USE_PMLOGLIB
}

target.path = $$WEBOS_INSTALL_BINS

INSTALLS += target
