CONFIG += debug testcase C++11
TEMPLATE = app
win32 { TEMPLATE = vcapp } #For stupid systems
QT += gui widgets testlib

#location for all objects and MOC files
MOC_DIR=build
OBJECTS_DIR=build

#name and dir of target exe
DESTDIR = .
TARGET=hellogl

#all source/header files
INCLUDEPATH += /usr/include/glm/
INCLUDEPATH += $$PWD/include
SOURCES += $$PWD/src/*.cpp

#HEADERS += $$PWD/test/*.h #tests have to be included as headers
#SOURCES += $$PWD/test/*.cpp
