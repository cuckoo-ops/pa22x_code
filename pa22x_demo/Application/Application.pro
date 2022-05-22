#-------------------------------------------------
#
# Project created by QtCreator 2016-06-27T07:53:07
#
#-------------------------------------------------

QT += core gui widgets network

TARGET = Application
TEMPLATE = app
CONFIG += static

FORMS += MainWindow.ui

INCLUDEPATH += $$PWD \
        $$PWD/mtld

SOURCES += main.cpp \
    fcmp.c \
    MainWindow.cpp \
    mtld/mtld.cpp \
    PA22XClient.cpp \
    ImageView.cpp \
    PAImageView.cpp \
    PEImageView.cpp \
    Ruler.cpp \
    SmartRuler.cpp

HEADERS += NDTMath.h \
    fcmp.h \
    MainWindow.h \
    PA22XClient.h \
    mtld/mtld.h \
    ImageView.h \
    PAImageView.h \
    PEImageView.h \
    Ruler.h \
    SmartRuler.h


RESOURCES += Application.qrc

*g++* {
    QMAKE_CXXFLAGS += -std=c++11 -Wno-unused-variable -Wno-unused-parameter -Wno-unused-but-set-variable
}

*msvc* {
    QMAKE_CXXFLAGS_WARN_ON -= -w34100
}
