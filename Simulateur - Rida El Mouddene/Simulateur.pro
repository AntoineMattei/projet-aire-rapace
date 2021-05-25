#--------------------------------------------------#
#                                                  #
# Project created by QtCreator 2021-01-15T11:59:59 #
#                                                  #
#--------------------------------------------------#

QT       += core gui


greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET = Simulateur
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    qcustomplot.cpp \
    rs232.cpp

HEADERS  += mainwindow.h \
    qcustomplot.h \
    rs232.h

FORMS    += mainwindow.ui
