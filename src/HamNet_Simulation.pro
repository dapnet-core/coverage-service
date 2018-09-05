#-------------------------------------------------
#
# Project created by QtCreator 2014-08-31T14:42:22
#
#-------------------------------------------------

QT       += core
QT       += widgets
QT       -= gui

TARGET = HamNet_Simulation
CONFIG   += console
CONFIG   -= app_bundle

QMAKE_CXXFLAGS += -std=c++11

TEMPLATE = app


SOURCES += \
    ../create_image.cpp \
    ../Image.cpp \
    ../main.cpp \
    ../Mapping.cpp \
    ../Prediction.cpp \
    ../Values.cpp \
    ../ASTER.c \
    ../Models/ITM_Model.cpp \
    ../Models/ITU_Model.cpp \
    ../Models/TR_Model.cpp

HEADERS += \
    ../create_image.h \
    ../Image.h \
    ../Mapping.h \
    ../Prediction.h \
    ../Values.h \
    ../Models/ITM_Model.h \
    ../Models/ITU_Model.h \
    ../Models/TR_Model.h
