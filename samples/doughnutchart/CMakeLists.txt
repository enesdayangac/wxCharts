cmake_minimum_required(VERSION 3.20)
project(doughnutchart VERSION 0.1.0 DESCRIPTION "wxDoughnutChart sample")

# Allows to use virtual folders in visual studio, 
#  it helps to organize larger projects.
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

find_package(wxWidgets COMPONENTS core base REQUIRED)
if(wxWidgets_USE_FILE)
    include(${wxWidgets_USE_FILE})
endif()

find_package(wxcharts CONFIG REQUIRED)

add_executable(doughnutchart 
  WIN32 MACOSX_BUNDLE
    src/wxdoughnutchartapp.h
    src/wxdoughnutchartapp.cpp
    src/wxdoughnutchartframe.h
    src/wxdoughnutchartframe.cpp
)

target_link_libraries(doughnutchart
  PRIVATE
    ${wxWidgets_LIBRARIES}
    wxcharts::wxcharts)

set_target_properties(doughnutchart
  PROPERTIES
    DEBUG_POSTFIX "d"             # Appends the letter "d" to the target's output name.
    FOLDER        "Samples")      # In visual studio puts this target's project inside a virtual folder (requires setting USE_FOLDERS = ON)