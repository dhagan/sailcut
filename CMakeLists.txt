cmake_minimum_required(VERSION 3.16)
project(sailcut VERSION 1.5.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# MSVC specific flags for C++17 support and Qt6 compatibility
if(MSVC)
    add_compile_options(/Zc:__cplusplus /permissive-)
endif()

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Find Qt6 packages
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets OpenGLWidgets PrintSupport Svg Xml)

# Set output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Define version and paths
add_compile_definitions(
    SAILCUT_VERSION="${PROJECT_VERSION}"
    SAILCUT_DATA_PATH="../share/sailcut"
    SAILCUT_DOC_PATH="../share/doc/sailcut/html"
)

# Include directories
include_directories(
    src
    src/geocpp
    src/sailcpp
    icons
)

# UI files
set(UI_FILES
    src/formhulldefbase.ui
    src/formrigdefbase.ui
    src/formsaildefbase.ui
)

# Header files
set(HEADER_FILES
    src/geocpp/geocpp.h
    src/geocpp/matrix.h
    src/geocpp/matrix4x4.h
    src/geocpp/rect.h
    src/geocpp/subspace.h
    src/geocpp/vector.h
    src/sailcpp/boatdef.h
    src/sailcpp/hulldef.h
    src/sailcpp/hullworker.h
    src/sailcpp/panelgroup.h
    src/sailcpp/panel.h
    src/sailcpp/rigdef.h
    src/sailcpp/rigworker.h
    src/sailcpp/sailcalc.h
    src/sailcpp/saildef.h
    src/sailcpp/sailmould.h
    src/sailcpp/sailworker.h
    src/app.h
    src/boatdef-panel.h
    src/filewriter.h
    src/formboat.h
    src/formhulldef.h
    src/formhull.h
    src/formmain.h
    src/formmould.h
    src/formpanelgroup.h
    src/formprint.h
    src/formrigdef.h
    src/formrig.h
    src/formsaildef.h
    src/formsail.h
    src/prefs.h
    src/saildispgl.h
    src/saildisp.h
    src/saildisplabel.h
    src/saildoc.h
    src/sailpainter.h
    src/sailprinter.h
    src/sailtreeitem.h
    src/sailtreemodel.h
    src/sailviewer.h
    src/sailviewer-panel.h
    src/sailviewer-tabs.h
    src/sailwriter-carlson.h
    src/sailwriter-dxf.h
    src/sailwriter-hand.h
    src/sailwriter-svg.h
    src/sailwriter-txt.h
    src/sailwriter-xml.h
    src/widgetprofile.h
    src/widgetprofilevert.h
)

# Source files
set(SOURCE_FILES
    src/geocpp/matrix.cpp
    src/geocpp/matrix4x4.cpp
    src/geocpp/rect.cpp
    src/geocpp/subspace.cpp
    src/geocpp/vector.cpp
    src/sailcpp/boatdef.cpp
    src/sailcpp/hulldef.cpp
    src/sailcpp/hullworker.cpp
    src/sailcpp/panel.cpp
    src/sailcpp/panelgroup.cpp
    src/sailcpp/rigdef.cpp
    src/sailcpp/rigworker.cpp
    src/sailcpp/sailcalc.cpp
    src/sailcpp/saildef.cpp
    src/sailcpp/sailmould.cpp
    src/sailcpp/sailworker.cpp
    src/app.cpp
    src/boatdef-panel.cpp
    src/formboat.cpp
    src/formhull.cpp
    src/formhulldef.cpp
    src/formmain.cpp
    src/formmould.cpp
    src/formpanelgroup.cpp
    src/formprint.cpp
    src/formrig.cpp
    src/formrigdef.cpp
    src/formsail.cpp
    src/formsaildef.cpp
    src/main.cpp
    src/prefs.cpp
    src/saildisp.cpp
    src/saildispgl.cpp
    src/saildisplabel.cpp
    src/saildoc.cpp
    src/sailpainter.cpp
    src/sailprinter.cpp
    src/sailtreeitem.cpp
    src/sailtreemodel.cpp
    src/sailviewer.cpp
    src/sailviewer-panel.cpp
    src/sailviewer-tabs.cpp
    src/sailwriter-carlson.cpp
    src/sailwriter-dxf.cpp
    src/sailwriter-hand.cpp
    src/sailwriter-svg.cpp
    src/sailwriter-txt.cpp
    src/widgetprofile.cpp
    src/widgetprofilevert.cpp
)

# Create executable
if(WIN32)
    add_executable(sailcut WIN32 ${SOURCE_FILES} ${HEADER_FILES} ${UI_FILES})
    if(EXISTS "${CMAKE_SOURCE_DIR}/icons/sailcut.ico")
        target_sources(sailcut PRIVATE icons/sailcut.ico)
    endif()
elseif(APPLE)
    add_executable(sailcut MACOSX_BUNDLE ${SOURCE_FILES} ${HEADER_FILES} ${UI_FILES})
    set_target_properties(sailcut PROPERTIES
        MACOSX_BUNDLE_BUNDLE_NAME "Sailcut CAD"
        MACOSX_BUNDLE_GUI_IDENTIFIER "com.sailcut.cad"
        MACOSX_BUNDLE_INFO_STRING "Sailcut CAD ${PROJECT_VERSION}"
        MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION}
    )
    if(EXISTS "${CMAKE_SOURCE_DIR}/icons/sailcut.icns")
        set_target_properties(sailcut PROPERTIES MACOSX_BUNDLE_ICON_FILE sailcut.icns)
        target_sources(sailcut PRIVATE icons/sailcut.icns)
    endif()
else()
    add_executable(sailcut ${SOURCE_FILES} ${HEADER_FILES} ${UI_FILES})
endif()

# Link Qt libraries
target_link_libraries(sailcut PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::OpenGLWidgets
    Qt6::PrintSupport
    Qt6::Svg
    Qt6::Xml
)

# Installation rules
install(TARGETS sailcut
    BUNDLE DESTINATION .
    RUNTIME DESTINATION bin
)

if(UNIX AND NOT APPLE)
    install(FILES extras/sailcut.desktop DESTINATION share/applications)
    install(FILES icons/sailcut.svg DESTINATION share/icons/hicolor/scalable)
    install(FILES extras/org.sailcut.cad.metainfo.xml DESTINATION share/metainfo)
endif()
