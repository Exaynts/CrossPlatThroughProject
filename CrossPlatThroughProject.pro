QT       += core gui widgets

CONFIG   += console c++17
CONFIG   -= app_bundle

TARGET = QtNutritionDiary
TEMPLATE = app

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    meal.cpp \
    nutritiondiary.cpp \
    mealwidget.cpp \
    calculator.cpp

HEADERS += \
    mainwindow.h \
    meal.h \
    nutritiondiary.h \
    mealwidget.h \
    calculator.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
