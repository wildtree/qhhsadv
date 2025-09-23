cmake_minimum_required(VERSION 3.5)

project(qhhsadv LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# QtCreator supports the following variables for Android, which are identical to qmake Android variables.
# Check https://doc.qt.io/qt/deployment-android.html for more information.
# They need to be set before the find_package( ...) calls below.

#if(ANDROID)
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
#    if (ANDROID_ABI STREQUAL "armeabi-v7a")
#        set(ANDROID_EXTRA_LIBS
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libcrypto.so
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libssl.so)
#    endif()
#endif()

find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets LinguistTools Multimedia REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets LinguistTools Multimedia REQUIRED)

set(TS_FILES qhhsadv_ja_JP.ts)

set(PROJECT_SOURCES
        main.cpp
        zsystem.cpp
        zsystem.h
        zsystem.ui
        graph.h
        graph.cpp
        zrule.h
        zrule.cpp
        zmap.h
        zmap.cpp
        zwords.h
        zwords.cpp
        zuserdata.h
        zuserdata.cpp
        zobjdata.h
        zobjdata.cpp
        files.h
        dialog.h
        dialog.cpp
        prefs.h
        prefs.cpp
        ${TS_FILES}
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(qhhsadv
        ${PROJECT_SOURCES}
        ZCreditRoll.h
    )

    qt_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
else()
    if(ANDROID)
        add_library(qhhsadv SHARED
            ${PROJECT_SOURCES}
        )
    else()
        add_executable(qhhsadv
            ${PROJECT_SOURCES}
        )
    endif()

    qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
endif()

target_link_libraries(qhhsadv PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Multimedia)
