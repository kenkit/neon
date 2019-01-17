cmake_minimum_required (VERSION 2.8.12)
project(OgreImgui)

# specify which version you need
find_package(OGRE 1.10 REQUIRED)

find_package(Freetype QUIET)

message("XYXYXYXYXYX ${CMAKE_BUILD_TYPE} mode XYXYXYXYX")
# the search paths
include_directories(${OGRE_INCLUDE_DIRS})
link_directories(${OGRE_LIBRARY_DIRS})

# copy essential config files next to our binary where OGRE autodiscovers them
file(COPY ${OGRE_CONFIG_DIR}/plugins.cfg ${OGRE_CONFIG_DIR}/resources.cfg
     DESTINATION ${CMAKE_BINARY_DIR})

set(IMGUI_SRCS ${PROJECT_SOURCE_DIR}/imgui/imgui.cpp ${PROJECT_SOURCE_DIR}/imgui/imgui_draw.cpp  ${PROJECT_SOURCE_DIR}/imgui/addons/imguidock/imgui_dock.cpp)
set(OGRE_IMGUI_SRCS ${PROJECT_SOURCE_DIR}/ImguiManager.cpp ${PROJECT_SOURCE_DIR}/ImguiRenderable.cpp)
if(FREETYPE_FOUND)
    include_directories(${PROJECT_SOURCE_DIR}/imgui/misc/freetype/)
    list(APPEND IMGUI_SRCS ${PROJECT_SOURCE_DIR}/imgui/misc/freetype/imgui_freetype.cpp)
endif()

include_directories(${PROJECT_SOURCE_DIR} ${PROJECT_SOURCE_DIR}/imgui/ ${PROJECT_SOURCE_DIR}/imgui/addons/imguidock/)
add_library(OgreImgui STATIC ${IMGUI_SRCS} ${OGRE_IMGUI_SRCS})
target_link_libraries(OgreImgui PUBLIC ${OGRE_LIBRARIES})
if(FREETYPE_FOUND)
    target_compile_definitions(OgreImgui PRIVATE -DUSE_FREETYPE)
    target_include_directories(OgreImgui PRIVATE ${FREETYPE_INCLUDE_DIRS})
    target_link_libraries(OgreImgui PRIVATE ${FREETYPE_LIBRARIES})
endif()

add_executable(Example Example.cpp ${PROJECT_SOURCE_DIR}/imgui/imgui_demo.cpp)
target_link_libraries(Example OgreImgui)

if(NOT FREETYPE_FOUND)
    set(FREETYPE_FOUND "FALSE")
endif()
message(STATUS "using freetype: ${FREETYPE_FOUND}")