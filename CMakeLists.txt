# Copyright (c) 2014 The Chromium Embedded Framework Authors. All rights
# reserved. Use of this source code is governed by a BSD-style license that
# can be found in the LICENSE file.

#
# Source files.
#
# Target executable names.
cmake_minimum_required(VERSION 3.5 FATAL_ERROR)
set(CEF_TARGET "neon")
project(${CEF_TARGET})
find_package(Hg)
find_package(Git)
set(PRE_CONFIGURE_FILE "${CMAKE_SOURCE_DIR}/version.h.in")
set(POST_CONFIGURE_FILE "${CMAKE_SOURCE_DIR}/include/version.h")
include(${CMAKE_SOURCE_DIR}/git_watcher.cmake)
if (WIN32)
    if(EXISTS "C:/Program Files (x86)/7-Zip/7z.exe")
        SET (7ZIP_EXECUTABLE "C:/Program Files (x86)/7-Zip/7z.exe")
    elseif(EXISTS "C:/Program Files/7-Zip/7z.exe")
        SET (7ZIP_EXECUTABLE "C:/Program Files/7-Zip/7z.exe")
    endif()
else()
    SET (7ZIP_EXECUTABLE "7z")
endif()


# specify which version you need
# Set a default build type if none was specified
macro(use_cxx11)
  if (CMAKE_VERSION VERSION_LESS "3.1")
    if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
      set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11")
    endif ()
  else ()
    set (CMAKE_CXX_STANDARD 11)
  endif ()
endmacro(use_cxx11)
#use_cxx11()
set(default_build_type "Debug")

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    link_directories(${CMAKE_SOURCE_DIR}/lib/Debug)
elseif (CMAKE_BUILD_TYPE STREQUAL "Release")
    link_directories(${CMAKE_SOURCE_DIR}/lib/Release)
else()
     set(CMAKE_BUILD_TYPE "Debug")
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

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    # 64 bits
    if(WIN32)
        set(CEF_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/cef_binary_3.3578.1860.g36610bd_windows32")
        set(CEF_BINARY_LINK "http://opensource.spotify.com/cefbuilds/cef_binary_3.3578.1860.g36610bd_windows32.tar.bz2")
        set(FFMPEG_BINARY_LINK "https://github.com/kenkit/Cloud_Commander_UI/blob/master/ffmpeg_dependancies.zip?raw=true")
        set(Ogre_zip ${CMAKE_CURRENT_SOURCE_DIR}/Latest_ogre.zip)
    else()
        set(CEF_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/cef_binary_3.3578.1864.gfa073f3_linux64")
        set(CEF_BINARY_LINK "http://opensource.spotify.com/cefbuilds/cef_binary_3.3578.1864.gfa073f3_linux64.tar.bz2")
        set(FFMPEG_BINARY_LINK "https://github.com/kenkit/Cloud_Commander_UI/blob/master/FFmpeg_linux.zip?raw=true")
        set(Ogre_zip ${CMAKE_CURRENT_SOURCE_DIR}/Latest_ogre.zip)
        set(OGRE_ZIP_URL "https://github.com/kenkit/Cloud_Commander_UI/blob/master/ogre_linux.zip?raw=true")
    endif()
    message("64 BIT MACHINE")
elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
    if(WIN32)
        set(CEF_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/cef_binary_3.3578.1860.g36610bd_windows32")
        set(CEF_BINARY_LINK "http://opensource.spotify.com/cefbuilds/cef_binary_3.3578.1860.g36610bd_windows32.tar.bz2")
        set(FFMPEG_BINARY_LINK "https://github.com/kenkit/Cloud_Commander_UI/blob/master/ffmpeg_dependancies.zip?raw=true")
        set(Ogre_zip ${CMAKE_CURRENT_SOURCE_DIR}/Latest_ogre.zip)
    else()
        set(CEF_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/cef_binary_3.3578.1864.gfa073f3_linux64")
        set(CEF_BINARY_LINK "http://opensource.spotify.com/cefbuilds/cef_binary_3.3578.1864.gfa073f3_linux64.tar.bz2")
        set(FFMPEG_BINARY_LINK "https://github.com/kenkit/Cloud_Commander_UI/blob/master/FFmpeg_linux.zip?raw=true")
        set(Ogre_zip ${CMAKE_CURRENT_SOURCE_DIR}/Latest_ogre.zip)
        set(OGRE_ZIP_URL "https://github.com/kenkit/Cloud_Commander_UI/blob/master/ogre_linux.zip?raw=true")
    endif()
    message("32 BIT MACHINE")
endif()
message("CEF ROOT IS:${CEF_ROOT}")
if(EXISTS "${CEF_ROOT}"  AND NOT FORCE_OPERATIONS)
    message("Found existing cef binary dir(${CEF_ROOT}), not extracting")
else()
    if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/cef_binary.tar.bz AND NOT FORCE_OPERATIONS)
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
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/cef_binary.tar.bz" OR FORCE_OPERATIONS)
        if (WIN32)
                message("Extracting cef_binary.tar.bz")
                execute_process(
                    COMMAND ${7ZIP_EXECUTABLE} x -y -r cef_binary.tar.bz
                    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                    )
                message("Extracting cef_binary.tar")
                execute_process(
                        COMMAND ${7ZIP_EXECUTABLE} x  -y -r cef_binary.tar
                        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                        )
        else()
                message("Extracting cef_binary.tar.bz")
                    execute_process(
                    COMMAND ${CMAKE_COMMAND} -E tar xzf cef_binary.tar.bz 
                    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                    )
        endif()
    else()
        message("Could not find:${CMAKE_CURRENT_SOURCE_DIR}/cef_binary.tar.bz")
    endif()
endif()


#/////////////////////////////////////////////////////////////////////////////
message("Setting CMAKE_MODULE_PATH TO :${CEF_ROOT}/cmake")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CEF_ROOT}/cmake")
execute_process(
    COMMAND ${CMAKE_COMMAND} -E copy_if_different  ${CMAKE_SOURCE_DIR}/patch_files/cef_macros.cmake  ${CEF_ROOT}/cmake/cef_macros.cmake 
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
execute_process(
        COMMAND ${CMAKE_COMMAND} -E copy_if_different  ${CMAKE_SOURCE_DIR}/patch_files/cef_variables.cmake  ${CEF_ROOT}/cmake/cef_variables.cmake
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
find_package(CEF REQUIRED)


if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/SDL" AND NOT FORCE_OPERATIONS)
        message("Found SDL ${CMAKE_CURRENT_SOURCE_DIR}/SDL")
        message("Pulling SDL")
        execute_process(
            COMMAND ${HG_EXECUTABLE} pull
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/SDL)
else()
        message("no sdl found")
        message("cloning a fresh....")
        if(DOWNLOAD_SDL2)
            set(SLD_DEP_LINK "https://github.com/kenkit/Cloud_Commander_UI/blob/master/sdl_2.tar.gz?raw=true")
            message("Downloading sdl from:${SLD_DEP_LINK}" )
            file(DOWNLOAD "${SLD_DEP_LINK}" ${CMAKE_CURRENT_SOURCE_DIR}/sdl_2.tar.gz  SHOW_PROGRESS STATUS status)
            list(GET status 0 status_code)
            list(GET status 1 status_string)
            if(NOT status_code EQUAL 0)
                message(WARN "error: downloading failed
                status_code: ${status_code}
                status_string: ${status_string}")
            else()
                message("Download succeeded")
                execute_process(
                    COMMAND ${CMAKE_COMMAND} -E tar xzf sdl_2.tar.gz 
                    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                    )
            endif()  
        else()
        execute_process(
            COMMAND ${HG_EXECUTABLE} clone https://hg.libsdl.org/SDL SDL
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
        endif()
endif()

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/SDL" AND NOT FORCE_OPERATIONS)
    if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/SDL/build" AND NOT FORCE_OPERATIONS)
        message("Creating SDL2 build directory:${CMAKE_CURRENT_SOURCE_DIR}/SDL/build")
        execute_process(
            COMMAND ${CMAKE_COMMAND} -E make_directory  ${CMAKE_CURRENT_SOURCE_DIR}/SDL/build
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/SDL/
            )
    endif()
    message("Generating sdl2 sources")
    execute_process(
        COMMAND ${CMAKE_COMMAND} -G ${CMAKE_GENERATOR} ../ -DCMAKE_INSTALL_PREFIX=${CMAKE_CURRENT_SOURCE_DIR}/SDL/build/install -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} 
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/SDL/build)
        message("building sdl2 from sources")    
    execute_process(
        COMMAND ${CMAKE_COMMAND} --build build
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/SDL)
    if(NOT WIN32)
        message("installing sdl2 to:${CMAKE_CURRENT_SOURCE_DIR}/SDL/build/")    
        execute_process(
            COMMAND make install
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/SDL/build/)
    else()
        message("generating sources") 
            execute_process(
                COMMAND ${CMAKE_COMMAND} -G ${CMAKE_GENERATOR} ../ -DCMAKE_INSTALL_PREFIX=${CMAKE_CURRENT_SOURCE_DIR}/SDL/build/install -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} 
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/SDL/build) 
        message("building sdl2 from sources")   
            execute_process(
                COMMAND ${CMAKE_COMMAND} --build build --target Install
                    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/SDL)
    endif()
    if(WIN32)
        set(SDL2_DIR ${CMAKE_CURRENT_SOURCE_DIR}/SDL/build/install/cmake)
    else()
        set(SDL2_DIR ${CMAKE_CURRENT_SOURCE_DIR}/SDL/build/install/lib/cmake/SDL2)
    endif()
endif()

find_package(SDL2 REQUIRED)

set(CEFSIMPLE_SRCS
 ${CMAKE_SOURCE_DIR}/src/main.cpp
  )

include_directories(
    include
    ${CMAKE_CURRENT_SOURCE_DIR}/SDL/build/install/include
)
#
# Windows configuration.
#

  # Executable target.
  add_subdirectory(${CEF_ROOT}/libcef_dll libcef_dll_wrapper)

  add_executable(${CEF_TARGET} WIN32 ${CEFSIMPLE_SRCS} ${CMAKE_SOURCE_DIR}/resource.rc )
  add_dependencies(${CEF_TARGET} libcef_dll_wrapper AlwaysCheckGit )
  ADD_LOGICAL_TARGET(libcef_lib ${CEF_LIB_DEBUG} ${CEF_LIB_RELEASE})
  SET_EXECUTABLE_TARGET_PROPERTIES(${CEF_TARGET})
  #include_directories(D:/Programming/cef)
set(USE_SANDBOX FALSE)
if(USE_SANDBOX)
    # Logical target used to link the cef_sandbox library.
    ADD_LOGICAL_TARGET("cef_sandbox_lib" "${CEF_SANDBOX_LIB_DEBUG}" "${CEF_SANDBOX_LIB_RELEASE}")
    target_link_libraries(${CEF_TARGET} cef_sandbox_lib ${CEF_SANDBOX_STANDARD_LIBS} )
else()
    target_link_libraries(${CEF_TARGET} libcef_lib libcef_dll_wrapper ${CEF_STANDARD_LIBS} SDL2::SDL2 SDL2::SDL2main )
endif()


  # Add the custom manifest files to the executable.
 # ADD_WINDOWS_MANIFEST("${CMAKE_CURRENT_SOURCE_DIR}" "${CEF_TARGET}" "exe")

  # Copy binary and resource files to the target output directory.
  COPY_FILES("${CEF_TARGET}" "${CEF_BINARY_FILES}" "${CEF_BINARY_DIR}" $<TARGET_FILE_DIR:${CEF_TARGET}>)
  COPY_FILES("${CEF_TARGET}" "${CEF_RESOURCE_FILES}" "${CEF_RESOURCE_DIR}" $<TARGET_FILE_DIR:${CEF_TARGET}>)
if(WIN32)
  set(DLLFiles ${CMAKE_CURRENT_SOURCE_DIR}/SDL/build/install/bin/SDL2${build_type2}.dll)
  foreach(DLLFile ${DLLFiles})
           add_custom_command(TARGET ${CEF_TARGET} POST_BUILD
           COMMAND ${CMAKE_COMMAND} -E
           copy_if_different  ${DLLFile}  $<TARGET_FILE_DIR:${CEF_TARGET}>
           )
  endforeach() 
endif()
  PRINT_CEF_CONFIG()
  

if(WIN32)
    add_custom_target(
        package_project  ALL  
            COMMENT "packaging project into zip file with:${7ZIP_EXECUTABLE} a -bb3  ${CMAKE_CURRENT_SOURCE_DIR}/NeonTech.zip *"
            COMMAND ${7ZIP_EXECUTABLE} a -bb3  ${CMAKE_CURRENT_SOURCE_DIR}/NeonTech.zip *
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE}
            )
else()
    add_custom_target(
        package_project  ALL  
            COMMENT "packaging project into zip file with:tar -czvf ${CMAKE_CURRENT_SOURCE_DIR}/NeonTech.zip *"
        COMMAND tar -czvf ${CMAKE_CURRENT_SOURCE_DIR}/NeonTech.zip *
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE}
        )
endif()
add_dependencies(package_project ${CEF_TARGET})