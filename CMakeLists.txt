set(QT5_MINIMUM_VERSION "5.15.0")
set(QT6_MINIMUM_VERSION "6.2.0")

cmake_minimum_required(VERSION 3.16)
project(plain-text-editor-efektivus LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)

set(DESKTOP_FILE plain-text-editor-efektivus.desktop)

find_package(Qt5 COMPONENTS Core)

#
#https://doc.qt.io/qt-6/cmake-qt5-and-qt6-compatibility.html
#
#find_package(Qt6 COMPONENTS Core)
#if(Qt6Core_FOUND)
#
if(ENABLE_QT6 OR Qt5Core_NOT_FOUND)
    set(QT_VERSION_MAJOR 6)
    find_package(Qt6 "${QT6_MINIMUM_VERSION}" REQUIRED COMPONENTS Core Gui Widgets)
else()
    set(QT_VERSION_MAJOR 5)
    find_package(Qt5 "${QT5_MINIMUM_VERSION}" REQUIRED COMPONENTS Core Gui Widgets)
endif()

# optional
find_package(Qt${QT_VERSION_MAJOR} QUIET COMPONENTS PrintSupport LinguistTools)

set(plain-text-editor-efektivus_sources
    main.cpp
    mainwindow.cpp
    mainwindow.h
)

# translations
if(Qt${QT_VERSION_MAJOR}LinguistTools_FOUND)
    file(GLOB TS_FILES translations/plain-text-editor-efektivus_*.ts)
    if(QT_VERSION_MAJOR EQUAL 6)
        qt6_add_translation(QM_FILES ${TS_FILES})
    else()
        qt5_add_translation(QM_FILES ${TS_FILES})
    endif()
    add_executable(plain-text-editor-efektivus ${plain-text-editor-efektivus_sources} ${QM_FILES})
    if(UNIX OR OS2)
        install(FILES ${QM_FILES} DESTINATION "/usr/share/plain-text-editor-efektivus/translations")
    endif()
else(Qt${QT_VERSION_MAJOR}LinguistTools_FOUND)
    add_executable(plain-text-editor-efektivus ${plain-text-editor-efektivus_sources})
endif(Qt${QT_VERSION_MAJOR}LinguistTools_FOUND)

if(QT_VERSION_MAJOR EQUAL 6)
    target_link_libraries(plain-text-editor-efektivus
                          Qt6::Core
                          Qt6::Gui
                          Qt6::Widgets
                          Qt6::PrintSupport)
else()
    target_link_libraries(plain-text-editor-efektivus
                          Qt5::Core
                          Qt5::Gui
                          Qt5::Widgets
                          Qt5::PrintSupport)
endif()

install(TARGETS plain-text-editor-efektivus
    RUNTIME DESTINATION "/usr/local/bin"
)

install(FILES ${DESKTOP_FILE} DESTINATION "/usr/share/applications/")
