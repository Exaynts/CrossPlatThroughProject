QT       += core gui widgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG   += console c++17
CONFIG   -= app_bundle

TARGET = QtCalculatorProject
TEMPLATE = app

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    calculator.cpp

HEADERS += \
    mainwindow.h \
    calculator.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
