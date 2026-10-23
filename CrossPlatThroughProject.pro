QT       += core gui widgets

CONFIG   += console c++17
CONFIG   -= app_bundle

TARGET = QtNutritionDiary
TEMPLATE = app

SOURCES += \
    diarystorage.cpp \
    filecontrolwidget.cpp \
    main.cpp \
    mainwindow.cpp \
    meal.cpp \
    normwidget.cpp \
    nutritiondiary.cpp \
    mealwidget.cpp \
    calculator.cpp

HEADERS += \
    diarystorage.h \
    filecontrolwidget.h \
    mainwindow.h \
    meal.h \
    normwidget.h \
    nutritiondiary.h \
    mealwidget.h \
    calculator.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
