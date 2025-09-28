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
        ZCreditRoll.h
        resizablegraphicsview.h
        files.cpp
        switchstyle.h
        switchbox.h
        ${TS_FILES}
)

if(WIN32)
    set(app_icon_resource_windows "${CMAKE_CURRENT_SOURCE_DIR}/qhhsadv.rc")
    list(APPEND PROJECT_SOURCES "${app_icon_resources_windows}")
endif()

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    if(WIN32)
        qt_add_executable(qhhsadv WIN32
            ${PROJECT_SOURCES}
            qhhsadv.rc
        )
    else()
        qt_add_executable(qhhsadv
            ${PROJECT_SOURCES}
        )
    endif()

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

add_custom_command(TARGET qhhsadv POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/data
            $<TARGET_FILE_DIR:qhhsadv>/data
)

if(WIN32)
    # デプロイ先ディレクトリ
    set(DEPLOY_DIR ${CMAKE_BINARY_DIR}/installer/deploy)

    # コピー対象
    set(APP_EXE $<TARGET_FILE:qhhsadv>)
    set(DATA_DIR ${CMAKE_BINARY_DIR}/data)

    # デプロイ処理
    add_custom_command(
        TARGET qhhsadv POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory ${DEPLOY_DIR}
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:qhhsadv> ${DEPLOY_DIR}
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${DATA_DIR} ${DEPLOY_DIR}/data
        COMMENT "Copying files to installer/deploy..."
    )

    # Qt DLL やプラグインを集める場合（windeployqt）
    # Qt bin ディレクトリを探す
    get_target_property(QT_QMAKE_EXECUTABLE Qt6::qmake IMPORTED_LOCATION)
    get_filename_component(QT_BIN_DIR "${QT_QMAKE_EXECUTABLE}" DIRECTORY)

    set(WINDEPLOYQT_EXECUTABLE "${QT_BIN_DIR}/windeployqt.exe")

    add_custom_command(
        TARGET qhhsadv POST_BUILD
        COMMAND "${WINDEPLOYQT_EXECUTABLE}" --dir ${DEPLOY_DIR} $<TARGET_FILE:qhhsadv>
        COMMENT "Running windeployqt..."
    )

    add_custom_command(
        TARGET qhhsadv POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/installer
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/installer/setup.iss ${CMAKE_BINARY_DIR}/installer
        COMMENT "Copying setup.iss..."
    )
    # Inno Setup コンパイラを探す

    find_program(INNO_SETUP_COMPILER
        NAMES ISCC iscc
        HINTS
            "$ENV{LOCALAPPDATA}/Programs/Inno Setup 6"
    )
    # PATH に入っている場合も探す
    if(NOT INNO_SETUP_COMPILER)
        find_program(INNO_SETUP_COMPILER NAMES ISCC iscc)
    endif()

    if(INNO_SETUP_COMPILER)
        message(STATUS "Inno Setup found: ${INNO_SETUP_COMPILER}")

        # .iss スクリプトの場所
        set(INNO_SCRIPT ${CMAKE_BINARY_DIR}/installer/setup.iss)

        # 出力先（例: build ディレクトリ内）
        set(INSTALLER_OUTPUT ${CMAKE_BINARY_DIR}/installer/qhhsadv_setup.exe)

        add_custom_command(
            OUTPUT ${INSTALLER_OUTPUT}
            COMMAND "${INNO_SETUP_COMPILER}" "${INNO_SCRIPT}"
            DEPENDS qhhsadv  # アプリ本体がビルドされてから実行
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            COMMENT "Building Inno Setup installer..."
        )

        add_custom_target(installer ALL
            DEPENDS ${INSTALLER_OUTPUT}
        )
    else()
        message(WARNING "Inno Setup (ISCC.exe) not found. Skipping installer target.")
    endif()
endif()

target_link_libraries(qhhsadv PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Multimedia)

if(UNIX AND NOT APPLE)
    # パッケージの基本情報
    set(CPACK_PACKAGE_NAME "myapp")
    set(CPACK_PACKAGE_VERSION "1.1.0")
    set(CPACK_PACKAGE_CONTACT "you@example.com")

    # Debian パッケージ用のメタ情報
    set(CPACK_GENERATOR "DEB")
    set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Hiroyuki") # 必須
    set(CPACK_DEBIAN_PACKAGE_SECTION "utils")
    set(CPACK_DEBIAN_PACKAGE_DEPENDS "libqt6core6 (>= 6.4.0), libc6 (>= 2.34), libqt6widget6 (>= 6.4.0)")

    # アーキテクチャ指定（自動判定も可能）
    string(TOLOWER "${CMAKE_SYSTEM_PROCESSOR}" DETECTED_ARCH)

    if(DETECTED_ARCH STREQUAL "x86_64")
        set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "amd64")
    elseif(DETECTED_ARCH STREQUAL "aarch64")
        set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "arm64")
    elseif(DETECTED_ARCH MATCHES "armv7")
        set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "armhf")
    else()
        set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "${DETECTED_ARCH}") # fallback
    endif()

    include(CPack)

    # 実行ファイルを /opt/qhhsadv に配置
    install(TARGETS qhhsadv DESTINATION /opt/qhhsadv)

    # data ディレクトリを /opt/qhhsadv にコピー
    install(DIRECTORY ${CMAKE_BINARY_DIR}/data DESTINATION /opt/qhhsadv)

    # ラッパースクリプトを /usr/bin にインストール
    install(PROGRAMS ${CMAKE_SOURCE_DIR}/scripts/qhhsadv.sh
            DESTINATION /usr/bin
            RENAME qhhsadv)
    # .desktop ファイル
    install(FILES ${CMAKE_SOURCE_DIR}/resources/linux/qhhsadv.desktop
            DESTINATION /usr/share/applications)

    # アイコン（48x48 の例）
    install(FILES ${CMAKE_SOURCE_DIR}/resources/icons/qhhsadv.png
            DESTINATION /usr/share/icons/hicolor/48x48/apps)

endif()

