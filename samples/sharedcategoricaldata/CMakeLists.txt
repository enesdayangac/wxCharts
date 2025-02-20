cmake_minimum_required(VERSION 3.20)
project(sharedcategoricaldata VERSION 0.1.0 DESCRIPTION "wxSharedCategoricalData sample")

# Allows to use virtual folders in visual studio, 
#  it helps to organize larger projects.
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

find_package(wxWidgets COMPONENTS core base REQUIRED)
if(wxWidgets_USE_FILE)
    include(${wxWidgets_USE_FILE})
endif()

find_package(wxcharts CONFIG REQUIRED)

add_executable(sharedcategoricaldata 
  WIN32 MACOSX_BUNDLE
    src/app.h
    src/app.cpp
    src/frame.h
    src/frame.cpp
)

target_link_libraries(sharedcategoricaldata
  PRIVATE
    ${wxWidgets_LIBRARIES}
    wxcharts::wxcharts)

set_target_properties(sharedcategoricaldata
  PROPERTIES
    DEBUG_POSTFIX "d"                     # Appends the letter "d" to the target's output name.
    FOLDER        "Samples")              # In visual studio puts this target's project inside a virtual folder (requires setting USE_FOLDERS = ON)
