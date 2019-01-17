cmake_minimum_required(VERSION 3.2)
project(CLUI)
include(git_watcher.cmake)
# specify which version you need
# Set a default build type if none was specified

set(default_build_type "Debug")

 
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to '${default_build_type}' as none was specified.")
  set(CMAKE_BUILD_TYPE "${default_build_type}" CACHE
      STRING "Choose the type of build." FORCE)
  # Set the possible values of build type for cmake-gui
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
    "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    link_directories(${CMAKE_SOURCE_DIR}/lib/Debug)
elseif (CMAKE_BUILD_TYPE STREQUAL "Release")
    link_directories(${CMAKE_SOURCE_DIR}/lib/Release)
else()
    link_directories(${CMAKE_SOURCE_DIR}/lib/Debug)
endif()
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    link_directories(${CMAKE_SOURCE_DIR}/lib/Debug)
    set(build_type "_d")
    set(build_type2 "d")
else()
    set(build_type "")
    set(build_type2 "")
endif()

message("XYXYXYXYXYX ${CMAKE_BUILD_TYPE} mode XYXYXYXYX")

set(CRYPTOPP $ENV{CRYPTOPP})                    #internalize
set (OPENSSL_ROOT_DIR $ENV{OPENSSL_ROOT_DIR})   #internalize
set (CURL_INCLUDE_DIR $ENV{CURL}/include)       #internalize
set (CURL_LIBRARY $ENV{CURL})                   #internalize
set(RAPID_XML ${CMAKE_SOURCE_DIR}/rapidxml)
set(JSONPP ${CMAKE_SOURCE_DIR}/json)
set(WSS_SOURCE_DIR ${CMAKE_SOURCE_DIR}/Simple-WebSocket-Server)
#set (SFML_DIR D:/programming/SFML-2.4.2/)
set (OGRE_DIR $ENV{OGRE_DIR})
set (OGRE_HOME $ENV{OGRE_HOME})
#set (FFMPEG2 $ENV{FFMPEG})
#set (OGRE_FFMPEG  ${CMAKE_SOURCE_DIR}/OgreVideoPlugin)
set (OPEN_AL  $ENV{OPEN_AL})
set(OGRE_IMGUI_SRC ${CMAKE_SOURCE_DIR}/ogre-imgui)


find_package(OpenSSL REQUIRED)

set(Boost_USE_STATIC_LIBS ON) 
set(Boost_USE_MULTITHREADED ON)  
set(Boost_USE_STATIC_RUNTIME ON) 
if (USE_BOOST_THREAD)
    find_package(Boost REQUIRED COMPONENTS thread timer regex filesystem date_time) 
else()
    find_package(Boost REQUIRED COMPONENTS  timer regex filesystem date_time) 
endif()
if (EXISTS "C:/Program Files (x86)")
    set(PROGRAM_FILES "C:/Program Files (x86)")
    message("64 BIT MACHINE")
else()
    message("32 BIT MACHINE")
    set(PROGRAM_FILES "C:/Program Files")
endif()

if(EXISTS "C:/Program Files (x86)/7-Zip/7z.exe")
    SET (7ZIP_EXECUTABLE "C:/Program Files (x86)/7-Zip/7z.exe")
elseif(EXISTS "C:/Program Files/7-Zip/7z.exe")
    SET (7ZIP_EXECUTABLE "C:/Program Files/7-Zip/7z.exe")
endif()
#/////////////////////////////////////////////////////////////////////////////
if(WIN32)
    set(CEF_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/cef_binary_3.3578.1860.g36610bd_windows32")
    set(CEF_BINARY_LINK "http://opensource.spotify.com/cefbuilds/cef_binary_3.3578.1860.g36610bd_windows32.tar.bz2")
    set(FFMPEG_BINARY_LINK "https://github.com/kenkit/Cloud_Commander_UI/blob/master/ffmpeg_dependancies.zip?raw=true")
else()
    set(CEF_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/cef_binary_3.3578.1863.gbf8cff2_linux32")
    set(CEF_BINARY_LINK "http://opensource.spotify.com/cefbuilds/cef_binary_3.3578.1863.gbf8cff2_linux32.tar.bz2")
endif()

if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/cef_binary.tar.bz)
    message("Found existing binary archive(${CMAKE_CURRENT_SOURCE_DIR}/cef_binary.tar.bz), not downloading" )
else()
    message("Downloading cef binaries from:${CEF_BINARY_LINK}" )
    file(DOWNLOAD "${CEF_BINARY_LINK}" ${CMAKE_CURRENT_SOURCE_DIR}/cef_binary.tar.bz SHOW_PROGRESS STATUS status)
    list(GET status 0 status_code)
    list(GET status 1 status_string)
    if(NOT status_code EQUAL 0)
        message(WARN "error: downloading failed
        status_code: ${status_code}
        status_string: ${status_string}")
    else()
        message("Download succeeded")
    endif()  
endif()
if(WIN32)
    if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg_dependancies.zip)
        message("Found existing binary archive(${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg_dependancies.zip), not downloading" )
    else()
        message("Downloading ffmpeg binaries from:${FFMPEG_BINARY_LINK}" )
        file(DOWNLOAD "${FFMPEG_BINARY_LINK}" ${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg_dependancies.zip SHOW_PROGRESS STATUS status)
        list(GET status 0 status_code)
        list(GET status 1 status_string)
        if(NOT status_code EQUAL 0)
            message(WARN "error: downloading failed
            status_code: ${status_code}
            status_string: ${status_string}")
        else()
            message("Download succeeded")
        endif()  
    endif()
endif()
if(EXISTS ${CEF_ROOT})
    message("Found existing cef binary dir(${CEF_ROOT}), not extracting")
else()
    message("dir(${CEF_ROOT}) is empty extracting..")
    message("Extracting cef binaries")
    if (WIN32)
        execute_process(
            COMMAND ${7ZIP_EXECUTABLE} x -y -r cef_binary.tar.bz
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            )
        execute_process(
                COMMAND ${7ZIP_EXECUTABLE} x  -y -r cef_binary.tar
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                )
        execute_process(
                COMMAND ${7ZIP_EXECUTABLE} x  -y -r ffmpeg_dependancies.zip
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                )
        else()
            execute_process(
            COMMAND ${CMAKE_COMMAND} -E tar xzf cef_binary.tar.bz 
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            )
    endif()
endif()
set(FFMPEG ${CMAKE_SOURCE_DIR}/ffmpeg/ffmpeg)
if (WIN32)
    if(EXISTS ${FFMPEG})
        message("Found existing ffmpeg binary dir(${FFMPEG}), not extracting")
    else()
        message("dir(${FFMPEG}) is empty extracting..")
        message("Extracting FFMPEG binaries")
        execute_process(
                    COMMAND ${7ZIP_EXECUTABLE} x  -y -r ffmpeg_dependancies.zip
                    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        )
    endif()
endif()
#/////////////////////////////////////////////////////////////////////////////

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CEF_ROOT}/cmake")
execute_process(
    COMMAND ${CMAKE_COMMAND} -E copy_if_different  ${CMAKE_SOURCE_DIR}/imgui-files/cef_macros.cmake  ${CEF_ROOT}/cmake/cef_macros.cmake 
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
execute_process(
        COMMAND ${CMAKE_COMMAND} -E copy_if_different  ${CMAKE_SOURCE_DIR}/imgui-files/cef_variables.cmake  ${CEF_ROOT}/cmake/cef_variables.cmake
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
find_package(CEF REQUIRED)



#string(REPLACE "\\" "/" PFILES "\"${PROGRAM_FILES}/Microsoft Visual Studio 14.0/VC/include\", \"${PROGRAM_FILES}/Windows Kits/10/Include/10.0.10240.0/ucrt\", \"${PROGRAM_FILES}/Windows Kits/10/Include/10.0.10150.0/ucrt\"")

#----------------------------------------------------------------------
if(EXISTS ${CMAKE_SOURCE_DIR}/JSONParser.cmake)
    include(JSONParser.cmake)
    set(Downloaded_JS TRUE)
else ()
    file(DOWNLOAD "https://raw.githubusercontent.com/sbellus/json-cmake/master/JSONParser.cmake" "${CMAKE_SOURCE_DIR}/JSONParser.cmake" SHOW_PROGRESS STATUS status)
    #message(STATUS "Download status = ${status}")
    if(EXISTS ${CMAKE_SOURCE_DIR}/JSONParser.cmake)
        include(JSONParser.cmake)
        set(Downloaded_JS TRUE)
    endif()
endif()



if (WIN32)
  set (show_contents_prog type)
endif (WIN32)

if(EXISTS "${OGRE_HOME}/current_ogre")
    file(READ "${OGRE_HOME}/current_ogre" current_version)
else()
    set(current_version 0)
endif()
set(Ogre_zip "${OGRE_HOME}/Latest_ogre.zip")
string(REPLACE "/" "\\"  Ogre_zip  ${Ogre_zip})
string(REPLACE "\\" "/"  OGRE_HOME  ${OGRE_HOME})
if(WIN32)
if(Downloaded_JS)
    file(DOWNLOAD "https://api.github.com/repos/kenkit/ogre/releases/latest" "${OGRE_HOME}/json_release.json" SHOW_PROGRESS STATUS status)
    file(READ "${OGRE_HOME}/json_release.json" JSON_OUTPUT)
    sbeParseJson(parsed_json JSON_OUTPUT)
    set(latest_version ${parsed_json.tag_name})
    set(download_url ${parsed_json.assets_0.browser_download_url}) 

    string(COMPARE EQUAL "${latest_version}" "" result)
    if(result)
        message("No network??")  
       set(latest_version ${current_version})
    endif()
    message("LATEST VERSION OF OGRE:REPO KENIT/OGRE ${latest_version} CURRENTLY INSTALLED ${current_version}")
    string(REPLACE "." "" current_version ${current_version})
    string(REPLACE "." ""  latest_version_2 ${latest_version})
    if(NOT SKIP_DOWNLOADS)
    if((current_version  LESS  latest_version_2 ) OR (current_version LESS_EQUAL "0" ))
        message("DOWNLOADING NEW OGRE FROM KENKITS REPO:${download_url}" )
        file(DOWNLOAD "${download_url}" "${Ogre_zip}" SHOW_PROGRESS STATUS status)

       file(WRITE ${OGRE_HOME}/extract.bat "\"${7ZIP_EXECUTABLE}\" x  -y -r ${Ogre_zip} -o${OGRE_HOME}")

       list(GET status 0 status_code)
       list(GET status 1 status_string)
        if(NOT status_code EQUAL 0)
        message(WARN "error: downloading failed
        status_code: ${status_code}
        status_string: ${status_string}")
   
        else()
            message("Download succeeded")
            message("Removing old ogre in:${OGRE_HOME} ")
            if(EXISTS ${OGRE_HOME}/build/sdk)
                 FILE(REMOVE_RECURSE ${OGRE_HOME}/build)
           endif()
            
        endif()     
    else ()
        message("YOU HAVE THE LATEST OGRE INSTALLED")
    endif()
    else ()
        message("<<<<<<<<<<<<<<<>-SKIPPING DOWNLOADS-<><<<<<<<<<<<<<<<")
    endif()
endif()

file(WRITE "${OGRE_HOME}/current_ogre" ${latest_version})
            execute_process(COMMAND ${OGRE_HOME}/extract.bat RESULT_VARIABLE rv  OUTPUT_QUIET)
            message("Extraction:${rv}")
            file(REMOVE ${OGRE_HOME}/extract.bat)

            message("Setting ogre_dir environment variable")
            file(WRITE ${OGRE_HOME}/env.bat "set OGRE_DIR =\"${OGRE_HOME}/build/sdk/CMake\" && setx OGRE_DIR \"${OGRE_HOME}/build/sdk/CMake\"")
            execute_process(COMMAND ${OGRE_HOME}/env.bat  RESULT_VARIABLE rv)
            file(REMOVE ${OGRE_HOME}/env.bat)
            message("OGRE_DIR $ENV{OGRE_DIR}")

            file(WRITE ${OGRE_HOME}/env.bat  "setx SDL2DIR \"${OGRE_HOME}/build/sdk/ogredeps/cmake\"")
            message("Setting SDL2_DIR environment variable")
            execute_process(COMMAND  ${OGRE_HOME}/env.bat RESULT_VARIABLE rv  OUTPUT_QUIET) 
            file(REMOVE ${OGRE_HOME}/env.bat) 


            message("setx SDL2_DIR:${rv}")

            file(WRITE ${OGRE_HOME}/env.bat  "set SDL2DIR =\"${OGRE_HOME}/build/sdk/ogredeps/cmake\"")
            execute_process(COMMAND   ${OGRE_HOME}/env.bat  RESULT_VARIABLE rv  OUTPUT_QUIET) 
            file(REMOVE ${OGRE_HOME}/env.bat) 
            message("set SDL2DIR:${rv}")

            file(WRITE ${OGRE_HOME}/env.bat "setx SDL2_DIR \"${OGRE_HOME}/build/sdk/ogredeps/cmake\"")
            execute_process(COMMAND  ${OGRE_HOME}/env.bat  RESULT_VARIABLE rv  OUTPUT_QUIET) 
            file(REMOVE ${OGRE_HOME}/env.bat)
            message("setx SDL2_DIR:${rv}") 

            set(OGRE_DEPENDENCIES_DIR ${OGRE_HOME}/build/sdk/ogredeps )
            file(WRITE ${OGRE_HOME}/env.bat "set SDL2_DIR =\"${OGRE_HOME}/build/sdk/ogredeps/cmake\"")
            execute_process(COMMAND ${OGRE_HOME}/env.bat  RESULT_VARIABLE rv  OUTPUT_QUIET)
            file(REMOVE ${OGRE_HOME}/env.bat)
            message("set SDL2_DIR:${rv}")
#----------------------------------------------------------------------
endif()
find_package(OGRE 1.10 REQUIRED)
link_directories(${FFMPEG}/Install/lib ${OGRE_DEPENDENCIES_DIR}/lib)

#string(REPLACE "\\" "/" FFMPEG ${FFMPEG2})
string(REPLACE "\\" "/" JSONPP ${JSONPP})
string(REPLACE "\\" "/" RAPID_XML ${RAPID_XML})
string(REPLACE "\\" "/" WSS_SOURCE_DIR ${WSS_SOURCE_DIR})
#string(REPLACE "\\" "/" CRYPTOPP ${CRYPTOPP})
string(REPLACE "\\" "/" OGRE_DEPENDENCIES_DIR  ${OGRE_DEPENDENCIES_DIR})

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin/)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin/)


link_directories(${FFMPEG}/Install/lib)
# the search paths

link_directories(${OGRE_LIBRARY_DIRS})

# copy essential config files next to our binary where OGRE autodiscovers them
find_package(SDL2 REQUIRED)
#file(GLOB FF_SFML ${CMAKE_SOURCE_DIR}/sfml/*.cpp)
#file(GLOB SDL_AL ${CMAKE_SOURCE_DIR}/sdl_al/*.cpp)
#file(GLOB SFML_DLL ${SFML_DIR}/bin/*.dll ${OPEN_AL}/build/Debug/OpenAL32.dll)
#file(GLOB FF_OGRE ${OGRE_FFMPEG}/example/*.cpp)
#string(REPLACE "\\" "/" FF_OGRE ${FF_OGRE})
#set(OPEN_AL_LIB ${OPEN_AL}/build/Debug/OpenAL32.lib)
file(GLOB FF_LIBS ${FFMPEG}/Install/lib/*.lib)
message("FF OGRE:${FF_OGRE}")

#add_library(ffplay  ${SDL_AL} )

add_custom_target(
    config_h
    COMMENT "FFPLAY:Coping ${FFMPEG}/config.h to ${FFMPEG}/config_ff.h"
    COMMAND ${CMAKE_COMMAND} -E
    copy_if_different  ${FFMPEG}/config.h ${FFMPEG}/config_ff.h
    )
 
    set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CEF_ROOT}/cmake")
    find_package(CEF REQUIRED)

add_library(ffplay STATIC ${CMAKE_SOURCE_DIR}/ffplay/ffplay.c ${CMAKE_SOURCE_DIR}/ffplay/cmdutils.c)
add_dependencies(ffplay  config_h )
#set_target_properties(ffplay PROPERTIES COMPILE_DEFINITIONS "_NO_CRT_STDIO_INLINE")


file(GLOB other_deps
${CMAKE_SOURCE_DIR}/utilities/*.cpp 
${CMAKE_SOURCE_DIR}/web_sockets/*.cpp 
${CMAKE_SOURCE_DIR}/gui-interface/*.cpp 
) 
if(NOT CONSOLE_APP)
add_executable(cloud_commander_ui  WIN32  cloud_commander_ui.cpp ${CMAKE_SOURCE_DIR}/resource.rc ${other_deps} ${CMAKE_SOURCE_DIR}/imgui-files/boost_fix.cpp #[[${CMAKE_SOURCE_DIR}/utilities/utilities.cpp 
${CMAKE_SOURCE_DIR}/web_sockets/web_socket.cpp 
${CMAKE_SOURCE_DIR}/gui-interface/gui_interface.cpp]] )
message("BUILDING WIN32 GUI APPLICATION")
else()
add_executable(cloud_commander_ui  cloud_commander_ui.cpp ${CMAKE_SOURCE_DIR}/resource.rc ${other_deps} #[[${CMAKE_SOURCE_DIR}/utilities/utilities.cpp 
${CMAKE_SOURCE_DIR}/web_sockets/web_socket.cpp 
${CMAKE_SOURCE_DIR}/gui-interface/gui_interface.cpp]] )
message("BUILDING CONSOLE APPLICATION")
endif()
SET_EXECUTABLE_TARGET_PROPERTIES(cloud_commander_ui)

#set_target_properties(cloud_commander_ui PROPERTIES COMPILE_DEFINITIONS "_WIN32_WINNT=${ver}")
if(USE_BOOST_THREAD)
    #set_target_properties(cloud_commander_ui PROPERTIES COMPILE_DEFINITIONS "CONSOLE_APP=1 USE_BOOST_THREAD=1")
    target_compile_definitions(cloud_commander_ui PUBLIC -D_WIN32_WINNT=0x0A00 -DCONSOLE_APP=1 -DUSE_BOOST_THREAD=1)
    message("|||||||||||| USING BOOST:THREAD ||||||||||||")
else()
    target_compile_definitions(cloud_commander_ui PUBLIC -D_WIN32_WINNT=0x0A00 -DCONSOLE_APP=1)
    message("|||||||||||| USING STD:THREAD ||||||||||||")
endif()
if(ORIG_FFPLAY)
    message("Building original ffplay")
    set(FFPLAY_SOURCE ${FFMPEG}/doc/examples/metadata.c )
else()
    message("Building projects ffplay")
    set(FFPLAY_SOURCE ${CMAKE_SOURCE_DIR}/ffplay/ffplay.c ${CMAKE_SOURCE_DIR}/ffplay/cmdutils.c) 
endif()
add_executable(ffmpeg_ffplay ${FFPLAY_SOURCE} )
set_target_properties(ffmpeg_ffplay PROPERTIES COMPILE_DEFINITIONS "TEST=1")
#add_executable(sfml_test ${FF_SFML} )

#link_directories(${SFML_DIR}/lib)
#include_directories(${SFML_DIR}/include)
include_directories(${OPEN_AL}/include)
#file(GLOB SFML_LIBS ${SFML_DIR}/lib/*-d.lib)

target_link_libraries(ffplay
${OPEN_AL_LIB}
${OGRE_DEPENDENCIES_DIR}/lib/SDL2.lib ${OGRE_DEPENDENCIES_DIR}/lib/SDL2main.lib
winmm
)

set(TOTAL_INCLUDES
#${OGRE_FFMPEG}/example
#${OGRE_FFMPEG}/include
${SDL2_INCLUDE_DIRS}
${CMAKE_SOURCE_DIR}/ogre-imgui
${CMAKE_SOURCE_DIR}/ogre-imgui/imgui/
${CMAKE_SOURCE_DIR}/ogre-imgui/imgui/addons/imguidock/
#${CMAKE_SOURCE_DIR}/sdl_al/
${CMAKE_SOURCE_DIR}/ffplay/ 
${FFMPEG}/include 
${FFMPEG}/fftools 
${FFMPEG}/ 
${OGRE_DEPENDENCIES_DIR}/include/SDL2
${OGRE_INCLUDE_DIRS} 
${OGRE_INCLUDE_DIRS}/Bites
${OGRE_INCLUDE_DIRS}/RTShaderSystem
${CRYPTOPP} 
${OPENSSL_INCLUDE_DIR}
${CRYPTOPP}/cryptopp 
${WSS_SOURCE_DIR} 
${RAPID_XML}
${JSONPP}/src 
${CMAKE_SOURCE_DIR} 
${CMAKE_SOURCE_DIR}/utilities
${CMAKE_SOURCE_DIR}/web_sockets
${CMAKE_SOURCE_DIR}/gui-interface
${CMAKE_SOURCE_DIR}/ffplay
${Boost_INCLUDE_DIRS}
)
include_directories(${TOTAL_INCLUDES})
set(new_includes " ")
foreach(dir ${TOTAL_INCLUDES})
    if (new_includes STREQUAL " ")
        set(new_includes "\"${dir}\"" )
    else()
        set(new_includes "\"${dir}\",${new_includes}" )
    endif()
endforeach() 


set(new_includes  "${PFILES},${new_includes}" ) 

file(WRITE "${CMAKE_SOURCE_DIR}/.vscode/c_cpp_properties.json" "{\n\"configurations\": [\n{\n            \"name\": \"Win32\",\n            \"browse\": {\n                \"path\": [\n                   ${new_includes}\n                 ],\n                \"limitSymbolsToIncludedHeaders\": true\n            },\n            \"includePath\": [\n                ${new_includes} \n            ],\n            \"defines\": [\n                \"_DEBUG\",\n                \"UNICODE\",\n                \"_UNICODE\"\n            ],\n            \"cStandard\": \"c11\",\n            \"cppStandard\": \"c++11\",\n            \"intelliSenseMode\": \"msvc-x64\"\n        }\n    ],    \"version\": 4\n}")

file(GLOB CFGFiles ${CMAKE_SOURCE_DIR}/cfg/*.cfg ${FFMPEG}/Install/bin/*.dll )
     foreach(CFGFile ${CFGFiles})
         add_custom_command(TARGET cloud_commander_ui POST_BUILD
         COMMAND ${CMAKE_COMMAND} -E
         copy_if_different  ${CFGFile} $<TARGET_FILE_DIR:cloud_commander_ui>
                 COMMENT "copying ${CFGFile}\n"
         )
endforeach()  
find_package(Git)
#[[add_custom_command(TARGET cloud_commander_ui PRE_BUILD
COMMAND ${CMAKE_COMMAND} -E make_directory  ${CMAKE_CURRENT_SOURCE_DIR}/Cloud_Commander_UI
COMMAND "${GIT_EXECUTABLE}" log -10  > Cloud_Commander_UI/Latest_Version_Info.txt
WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                 COMMENT "Appending latest version info to secondary repo"
)]]
string(REPLACE "\\" "/" OGRE_HOME_E ${OGRE_DIR})
string(REPLACE "/CMake" ""  OGRE_HOME_E ${OGRE_HOME_E})
string(REPLACE "/Cmake" ""  OGRE_HOME_E ${OGRE_HOME_E})
message("OGRE_HOME:${OGRE_HOME_E}")

set(DLLFiles  
#${OGRE_FFMPEG}/build/${CMAKE_BUILD_TYPE}/OgreVideoPlugin.dll
${CEF_ROOT}/LICENSE.txt
${OPENSSL_ROOT_DIR}/libeay32.dll
${OPENSSL_ROOT_DIR}/ssleay32.dll
${OGRE_HOME_E}/bin/RenderSystem_Direct3D11${build_type}.dll 
${OGRE_HOME_E}/bin/RenderSystem_Direct3D9${build_type}.dll 
${OGRE_HOME_E}/bin/OgreMain${build_type}.dll 
${OGRE_HOME_E}/bin/OgreOverlay${build_type}.dll  
${OGRE_HOME_E}/bin/OgreBites${build_type}.dll 
${OGRE_HOME_E}/bin/OgreRTShaderSystem${build_type}.dll
${OGRE_HOME_E}/bin/SDL2.dll
${OGRE_HOME_E}/bin/zlib.dll
)
foreach(DLLFile ${DLLFiles})
         add_custom_command(TARGET cloud_commander_ui POST_BUILD
         COMMENT ">>>>>>>>>>>>>>>>>>copying ${DLLFile}<<<<<<<<<<<<<<<<<<<<<<<<"
         COMMAND ${CMAKE_COMMAND} -E
         copy_if_different  ${DLLFile} $<TARGET_FILE_DIR:cloud_commander_ui> 
         )
endforeach() 
#[[
foreach(DLLFile ${SFML_DLL})
add_custom_command(TARGET sfml_test POST_BUILD
COMMENT ">>>>>>>>>>>>>>>>>>copying ${DLLFile}<<<<<<<<<<<<<<<<<<<<<<<<"
COMMAND ${CMAKE_COMMAND} -E
copy_if_different  ${SFML_DLL} $<TARGET_FILE_DIR:sfml_test> 
)
endforeach() ]]   
            execute_process(
            COMMAND ${CMAKE_COMMAND} -E make_directory  ${CMAKE_SOURCE_DIR}/ogre-imgui/build
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            )
            execute_process(
            COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_SOURCE_DIR}/ogre-imgui/imgui/addons/
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            )
            execute_process(
            COMMAND ${CMAKE_COMMAND} -E make_directory  ${CMAKE_SOURCE_DIR}/ogre-imgui/imgui/addons/imguidock/
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

            execute_process(
            COMMAND ${CMAKE_COMMAND} -E copy_if_different  ${CMAKE_SOURCE_DIR}/imgui-files/CMakeLists.txt ${CMAKE_SOURCE_DIR}/ogre-imgui/CMakeLists.txt
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

            execute_process(
            COMMAND ${CMAKE_COMMAND} -E copy_if_different  ${CMAKE_SOURCE_DIR}/imgui-files/Example.cpp ${CMAKE_SOURCE_DIR}/ogre-imgui/Example.cpp 
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
            
            execute_process(
            COMMAND ${CMAKE_COMMAND} -E copy_if_different  ${CMAKE_SOURCE_DIR}/imguiDock/imgui_dock.cpp ${CMAKE_SOURCE_DIR}/ogre-imgui/imgui/addons/imguidock/
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
            
            execute_process(
            COMMAND ${CMAKE_COMMAND} -E copy_if_different  ${CMAKE_SOURCE_DIR}/imguiDock/imgui_dock.h ${CMAKE_SOURCE_DIR}/ogre-imgui/imgui/addons/imguidock/
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
            
            execute_process(
            COMMAND ${CMAKE_COMMAND} -E copy_if_different  ${CMAKE_SOURCE_DIR}/imguiDock/imgui_dock.h ${CMAKE_SOURCE_DIR}/ogre-imgui/imgui/addons/imguidock/
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})       

        add_subdirectory(${CMAKE_SOURCE_DIR}/ogre-imgui OgreImgui) 
        SET_EXECUTABLE_TARGET_PROPERTIES(cloud_commander_ui)
        SET_EXECUTABLE_TARGET_PROPERTIES(OgreImgui)
        SET_EXECUTABLE_TARGET_PROPERTIES(Example)
  #[[      file(MAKE_DIRECTORY ${OGRE_FFMPEG}/build)
        add_custom_target(ff_ogre_pre ALL        
        COMMENT "PREPARING OGRE_FFMPEG SOURCES IN:${OGRE_FFMPEG}"
        COMMAND ${CMAKE_COMMAND} ../ -G "${CMAKE_GENERATOR}" -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
        WORKING_DIRECTORY ${OGRE_FFMPEG}/build)


        add_custom_target(ff_ogre_lib ALL  
        COMMENT "BUILDIN GOGRE_FFMPEG SOURCES IN:${CMAKE_SOURCE_DIR}/imgui/build"
        COMMAND ${CMAKE_COMMAND} --build ../build --config ${CMAKE_BUILD_TYPE}
        WORKING_DIRECTORY ${OGRE_FFMPEG}/build
        )
        add_dependencies(ogre_imgui_pre  imgui_addons)    
        add_dependencies(ogre_imgui_lib  ogre_imgui_pre)
        
           
        add_dependencies(ff_ogre_lib  ff_ogre_pre)        
]]


    add_subdirectory(${CEF_ROOT}/libcef_dll libcef_dll_wrapper)
    add_dependencies(cloud_commander_ui  ffplay OgreImgui AlwaysCheckGit libcef_dll_wrapper)   
    ADD_LOGICAL_TARGET(libcef_lib ${CEF_LIB_DEBUG} ${CEF_LIB_RELEASE})

    set_target_properties(ffplay PROPERTIES COMPILE_FLAGS  "${CMAKE_CXX_FLAGS_RELEASE} /MT")
    set_target_properties(ffplay PROPERTIES COMPILE_FLAGS  "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
    #set_target_properties(cloud_commander_ui PROPERTIES COMPILE_FLAGS  "${CMAKE_CXX_FLAGS_RELEASE} /MT")
    #set_target_properties(cloud_commander_ui PROPERTIES COMPILE_FLAGS  "${CMAKE_CXX_FLAGS_DEBUG} /MTd")

        target_link_libraries(cloud_commander_ui ffplay libcef_lib libcef_dll_wrapper  ${CEF_STANDARD_LIBS})
     if( MSVC )
            #ADD_DEFINITIONS("/EHsc")
           #set_target_properties( cloud_commander_ui PROPERTIES LINK_FLAGS "/NODEFAULTLIB:LIBCPMT" )
            #set_target_properties( ffplay PROPERTIES LINK_FLAGS "/NODEFAULTLIB:LIBCPMT" )
	    endif()
        if(USE_SANDBOX)
        # Logical target used to link the cef_sandbox library.
        ADD_LOGICAL_TARGET("cef_sandbox_lib" "${CEF_SANDBOX_LIB_DEBUG}" "${CEF_SANDBOX_LIB_RELEASE}")
        target_link_libraries(cloud_commander_ui cef_sandbox_lib ${CEF_SANDBOX_STANDARD_LIBS} )
      endif()
        set_target_properties(ffplay PROPERTIES ARCHIVE_OUTPUT_DIRECTORY  ${CMAKE_SOURCE_DIR}/lib)
#[[
        target_link_libraries(sfml_test
        ${SFML_LIBS}
        ${FF_LIBS}
        ${Boost_LIBRARIES}
        winmm
        )
   ]]      
        
        if(Boost_FOUND)
            target_link_libraries(cloud_commander_ui  OgreImgui ${SDL2_LIBRARIES} ${OGRE_LIBRARIES} ${OPENSSL_LIBRARIES} ws2_32 wsock32 ${Boost_LIBRARIES} 
            ${FF_LIBS}
            winmm
            )
            target_link_libraries(ffmpeg_ffplay ${SDL2_LIBRARIES}  ${OGRE_DEPENDENCIES_DIR}/lib/SDL2.lib ${OGRE_DEPENDENCIES_DIR}/lib/SDL2main.lib ${FF_LIBS})
        else()
            message("Could not find boost!")
        endif()

file(GLOB rtshaders ${OGRE_HOME_E}/media/RTShaderLib/*.)
foreach(shader ${rtshaders})
add_custom_command(TARGET cloud_commander_ui POST_BUILD
COMMAND ${CMAKE_COMMAND} -E
copy_if_different  ${shader} $<TARGET_FILE_DIR:cloud_commander_ui>/RTShaderLib
        COMMENT "copying ${shader}/RTShaderLib\n"
)
endforeach()  
COPY_FILES(cloud_commander_ui "${CEF_BINARY_FILES}" "${CEF_BINARY_DIR}" $<TARGET_FILE_DIR:cloud_commander_ui>)
COPY_FILES(cloud_commander_ui "${CEF_RESOURCE_FILES}" "${CEF_RESOURCE_DIR}" $<TARGET_FILE_DIR:cloud_commander_ui>)
PRINT_CEF_CONFIG()