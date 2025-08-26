cmake_minimum_required(VERSION 3.16)
project(wallpaper_carousel VERSION 0.1 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# Activation d'options Release/Debug
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build (Debug or Release)" FORCE)
endif()

find_package(Qt6 COMPONENTS Quick Core Gui DBus Multimedia Widgets REQUIRED)
qt_policy(SET QTP0001 NEW)
qt_policy(SET QTP0004 NEW)

# Executable principal
qt_add_executable(wallpaper-carousel
    src/main.cpp
    src/wallpapercontroller.cpp
    src/wallpapercontroller.h
    src/imageloader.cpp
    src/imageloader.h
    src/infinitelistmodel.cpp
    src/infinitelistmodel.h
    src/screenhelper.cpp
    src/screenhelper.h
)

# Module QML
qt_add_qml_module(wallpaper-carousel
    URI CarouselUI
    VERSION 1.0
    QML_FILES
        qml/Main.qml
        qml/CarouselItem.qml
)

# Linking : Ajoute Qt6::Widgets et Qt6::Multimedia
target_link_libraries(wallpaper-carousel
    PRIVATE Qt6::Quick Qt6::Core Qt6::Gui Qt6::DBus Qt6::Multimedia Qt6::Widgets
)

# Options de compilation selon le type de build
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    message(STATUS "Configuring Release build with optimizations")
    target_compile_definitions(wallpaper-carousel PRIVATE NDEBUG)
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -march=native")
elseif(CMAKE_BUILD_TYPE STREQUAL "Debug")
    message(STATUS "Configuring Debug build with symbols")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0")
endif()

# Install
install(TARGETS wallpaper-carousel RUNTIME DESTINATION bin)
install(FILES packaging/wallpaper-carousel.desktop DESTINATION share/autostart)

