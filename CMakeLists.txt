cmake_minimum_required(VERSION 3.16)
project(QuickHubClient LANGUAGES CXX)

find_package(Qt6 6.2 REQUIRED COMPONENTS
    Quick
    Qml
    Gui
    Core
    WebSockets

)

# collect QML files and JS/qmldir
file(GLOB_RECURSE QML_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    "src/qml/*.qml" "src/qml/**/*.qml"
)

# collect asset resources (svg, png, csv, etc.)
file(GLOB_RECURSE QML_RESOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    "src/qml/Assets/*"
    "src/qml/**/*.svg" "src/qml/**/*.png" "src/qml/**/*.jpg" "src/qml/**/*.jpeg" "src/qml/**/*.csv"
)


# collect C++ sources and headers from src/cpp
file(GLOB_RECURSE CPP_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "src/**/*.cpp")
file(GLOB_RECURSE CPP_HEADERS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "src/**/*.h" )
list(REMOVE_ITEM CPP_SOURCES "src/Helpers/FileUploader.cpp")
list(REMOVE_ITEM CPP_HEADERS "src/Helpers/FileUploader.h")


qt_add_qml_module(QuickHubClient
    URI QuickHub
    VERSION 1.0
    SOURCES
        ${CPP_SOURCES}
        ${CPP_HEADERS}
    QML_FILES
        ${QML_FILES}
    RESOURCES
        ${QML_RESOURCES}
)


if(EMSCRIPTEN)
    target_compile_definitions(QuickHubClient PRIVATE
        WEB_ASSEMBLY
    )

endif()

set_target_properties(QuickHubClient PROPERTIES
    QT_QML_IMPORT_PATHS "${CMAKE_CURRENT_SOURCE_DIR}/src/qml"
)

target_link_libraries(QuickHubClient PUBLIC
    Qt6::Quick
    Qt6::Qml
    Qt6::Gui
    Qt6::Core
    Qt6::WebSockets

)
