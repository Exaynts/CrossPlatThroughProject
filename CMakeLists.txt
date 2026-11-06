cmake_minimum_required(VERSION 3.16)

project(QtNutritionDiary VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Указываем CMake использовать Debug по умолчанию, если тип не задан
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)

qt_standard_project_setup()

qt_add_executable(QtNutritionDiary
    main.cpp
    calculator.cpp
    calculator.h
    core/diarymanager.h
    core/diarymanager.cpp
    ui/widgets/dateselectorwidget.h ui/widgets/dateselectorwidget.cpp
)

target_sources(QtNutritionDiary PRIVATE
    core/diarymanager.cpp core/diarymanager.h
    core/meal.cpp core/meal.h
    core/nutritiondiary.cpp core/nutritiondiary.h
    
    data/diarystorage.cpp data/diarystorage.h
    
    ui/mainwindow.cpp ui/mainwindow.h
    
    ui/widgets/mealwidget.cpp ui/widgets/mealwidget.h
    ui/widgets/normwidget.cpp ui/widgets/normwidget.h
    ui/widgets/filecontrolwidget.cpp ui/widgets/filecontrolwidget.h
    ui/widgets/diarysummarywidget.cpp ui/widgets/diarysummarywidget.h
    ui/widgets/mealscontainerwidget.cpp ui/widgets/mealscontainerwidget.h
    ui/widgets/dateselectorwidget.cpp ui/widgets/dateselectorwidget.h
)

target_include_directories(QtNutritionDiary PRIVATE
    core
    data
    ui
    ui/widgets
)

target_link_libraries(QtNutritionDiary PRIVATE 
    Qt6::Core 
    Qt6::Gui 
    Qt6::Widgets
)

include(GNUInstallDirs)
install(TARGETS QtNutritionDiary
    BUNDLE DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
