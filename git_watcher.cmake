# git_watcher.cmake
#
# This file defines the functions and targets needed to keep a
# watch on the state of the git repo. If the state changes, a
# header is reconfigured.
#
# Customization tip:
#   - You should only need to edit the paths to the pre and
#     post configure file. The rest should be plug-and-play.
#     See below for where those variables are defined.
#
# Script design:
#   - This script was designed similar to a Python application
#     with a Main() function. I wanted to keep it compact to
#     simplify "copy + paste" usage.
# 
#   - This script is made to operate in two scopes:
#       1. Configure time scope (when build files are created).
#       2. Build time scope (called via CMake -P)
#     If you see something odd (e.g. the NOT DEFINED clauses),
#     consider that it can run in one of two scopes.
cmake_minimum_required(VERSION 3.1)

if (DEFINED JSonParserGuard)
    return()
endif()

set(JSonParserGuard yes)

macro(sbeParseJson prefix jsonString)
    cmake_policy(PUSH)

    set(json_string "${${jsonString}}")
    string(LENGTH "${json_string}" json_jsonLen)
    set(json_index 0)
    set(json_AllVariables ${prefix})
    set(json_ArrayNestingLevel 0)
    set(json_MaxArrayNestingLevel 0)

    _sbeParse(${prefix})
    
    unset(json_index)
    unset(json_AllVariables)
    unset(json_jsonLen)
    unset(json_string)
    unset(json_value)
    unset(json_inValue)    
    unset(json_name)
    unset(json_inName)
    unset(json_newPrefix)
    unset(json_reservedWord)
    unset(json_arrayIndex)
    unset(json_char)
    unset(json_end)
    unset(json_ArrayNestingLevel)
    foreach(json_nestingLevel RANGE ${json_MaxArrayNestingLevel})
        unset(json_${json_nestingLevel}_arrayIndex)
    endforeach()
    unset(json_nestingLevel)
    unset(json_MaxArrayNestingLevel)
    
    cmake_policy(POP)
endmacro()

macro(sbeClearJson prefix)
    foreach(json_var ${${prefix}})
        unset(${json_var})
    endforeach()
    
    unset(${prefix})
    unset(json_var)
endmacro()

macro(sbePrintJson prefix)
    foreach(json_var ${${prefix}})
        message("${json_var} = ${${json_var}}")
    endforeach()
endmacro()

macro(_sbeParse prefix)

    while(${json_index} LESS ${json_jsonLen})
        string(SUBSTRING "${json_string}" ${json_index} 1 json_char)
        
        if("\"" STREQUAL "${json_char}")    
            _sbeParseNameValue(${prefix})
        elseif("{" STREQUAL "${json_char}")
            _sbeMoveToNextNonEmptyCharacter()
            _sbeParseObject(${prefix})
        elseif("[" STREQUAL "${json_char}")
            _sbeMoveToNextNonEmptyCharacter()
            _sbeParseArray(${prefix})
        endif()

        if(${json_index} LESS ${json_jsonLen})
            string(SUBSTRING "${json_string}" ${json_index} 1 json_char)
        else()
            break()
        endif()

        if ("}" STREQUAL "${json_char}" OR "]" STREQUAL "${json_char}")
            break()
        endif()
        
        _sbeMoveToNextNonEmptyCharacter()
    endwhile()    
endmacro()

macro(_sbeParseNameValue prefix)
    set(json_name "")
    set(json_inName no)

    while(${json_index} LESS ${json_jsonLen})
        string(SUBSTRING "${json_string}" ${json_index} 1 json_char)
        
        # check if name ends
        if("\"" STREQUAL "${json_char}" AND json_inName)
            set(json_inName no)
            _sbeMoveToNextNonEmptyCharacter()
            if(NOT ${json_index} LESS ${json_jsonLen})
                break()
            endif()                
            string(SUBSTRING "${json_string}" ${json_index} 1 json_char)
            set(json_newPrefix ${prefix}.${json_name})
            set(json_name "")
            
            if(":" STREQUAL "${json_char}")
                _sbeMoveToNextNonEmptyCharacter()
                if(NOT ${json_index} LESS ${json_jsonLen})
                    break()
                endif()                
                string(SUBSTRING "${json_string}" ${json_index} 1 json_char)

                if("\"" STREQUAL "${json_char}")    
                    _sbeParseValue(${json_newPrefix})
                    break()
                elseif("{" STREQUAL "${json_char}")
                    _sbeMoveToNextNonEmptyCharacter()
                    _sbeParseObject(${json_newPrefix})
                    break()
                elseif("[" STREQUAL "${json_char}")
                    _sbeMoveToNextNonEmptyCharacter()
                    _sbeParseArray(${json_newPrefix})
                    break()
                else()
                    # reserved word starts
                    _sbeParseReservedWord(${json_newPrefix})
                    break()
                endif()
            else()
                # name without value
                list(APPEND ${json_AllVariables} ${json_newPrefix})
                set(${json_newPrefix} "")
                break()            
            endif()           
        endif()

        if(json_inName)
            # remove escapes
            if("\\" STREQUAL "${json_char}")
                math(EXPR json_index "${json_index} + 1")
                if(NOT ${json_index} LESS ${json_jsonLen})
                    break()
                endif()                
                string(SUBSTRING "${json_string}" ${json_index} 1 json_char)
            endif()
        
            set(json_name "${json_name}${json_char}")
        endif()
        
        # check if name starts
        if("\"" STREQUAL "${json_char}" AND NOT json_inName)
            set(json_inName yes)
        endif()
       
        _sbeMoveToNextNonEmptyCharacter()
    endwhile()    
endmacro()

macro(_sbeParseReservedWord prefix)
    set(json_reservedWord "")
    set(json_end no)
    while(${json_index} LESS ${json_jsonLen} AND NOT json_end)
        string(SUBSTRING "${json_string}" ${json_index} 1 json_char)
        
        if("," STREQUAL "${json_char}" OR "}" STREQUAL "${json_char}" OR "]" STREQUAL "${json_char}")
            set(json_end yes)
        else()
            set(json_reservedWord "${json_reservedWord}${json_char}")
            math(EXPR json_index "${json_index} + 1")
        endif()
    endwhile()

    list(APPEND ${json_AllVariables} ${prefix})
    string(STRIP  "${json_reservedWord}" json_reservedWord)
    set(${prefix} ${json_reservedWord})
endmacro()

macro(_sbeParseValue prefix)
    cmake_policy(SET CMP0054 NEW) # turn off implicit expansions in if statement
    
    set(json_value "")
    set(json_inValue no)
    
    while(${json_index} LESS ${json_jsonLen})
        # fast path for copying strings
        if (json_inValue)
            # attempt to gobble up to 128 bytes of string
            string(SUBSTRING "${json_string}" ${json_index} 128 try_gobble)
            # consume a piece of string we can just straight copy before encountering \ or "
            string(REGEX MATCH "^[^\"\\\\]+" simple_copy "${try_gobble}")
            string(CONCAT json_value "${json_value}" "${simple_copy}")
            string(LENGTH "${simple_copy}" copy_length)
            math(EXPR json_index "${json_index} + ${copy_length}")
        endif()

        string(SUBSTRING "${json_string}" ${json_index} 1 json_char)

        # check if json_value ends, it is ended by "
        if("\"" STREQUAL "${json_char}" AND json_inValue)
            set(json_inValue no)
            
            set(${prefix} ${json_value})
            list(APPEND ${json_AllVariables} ${prefix})
            _sbeMoveToNextNonEmptyCharacter()
            break()
        endif()
                
        if(json_inValue)
             # if " is escaped consume
            if("\\" STREQUAL "${json_char}")
                math(EXPR json_index "${json_index} + 1")
                if(NOT ${json_index} LESS ${json_jsonLen})
                    break()
                endif()                
                string(SUBSTRING "${json_string}" ${json_index} 1 json_char)
                if(NOT "\"" STREQUAL "${json_char}")
                    # if it is not " then copy also escape character
                    set(json_char "\\${json_char}")
                endif()
            endif()      
              
            _sbeAddEscapedCharacter("${json_char}")
        endif()
        
        # check if value starts
        if("\"" STREQUAL "${json_char}" AND NOT json_inValue)
            set(json_inValue yes)
        endif()
        
        math(EXPR json_index "${json_index} + 1")
    endwhile()
endmacro()

macro(_sbeAddEscapedCharacter char)
    string(CONCAT json_value "${json_value}" "${char}")
endmacro()

macro(_sbeParseObject prefix)
    _sbeParse(${prefix})
    _sbeMoveToNextNonEmptyCharacter()
endmacro()

macro(_sbeParseArray prefix)
    math(EXPR json_ArrayNestingLevel "${json_ArrayNestingLevel} + 1")
    set(json_${json_ArrayNestingLevel}_arrayIndex 0)

    set(${prefix} "")
    list(APPEND ${json_AllVariables} ${prefix})

    while(${json_index} LESS ${json_jsonLen})
        string(SUBSTRING "${json_string}" ${json_index} 1 json_char)

        if("\"" STREQUAL "${json_char}")
            # simple value
            list(APPEND ${prefix} ${json_${json_ArrayNestingLevel}_arrayIndex})
            _sbeParseValue(${prefix}_${json_${json_ArrayNestingLevel}_arrayIndex})
        elseif("{" STREQUAL "${json_char}")
            # object
            _sbeMoveToNextNonEmptyCharacter()
            list(APPEND ${prefix} ${json_${json_ArrayNestingLevel}_arrayIndex})
            _sbeParseObject(${prefix}_${json_${json_ArrayNestingLevel}_arrayIndex})
        else()
            list(APPEND ${prefix} ${json_${json_ArrayNestingLevel}_arrayIndex})
            _sbeParseReservedWord(${prefix}_${json_${json_ArrayNestingLevel}_arrayIndex})
        endif()
        
        if(NOT ${json_index} LESS ${json_jsonLen})
            break()
        endif()
        
        string(SUBSTRING "${json_string}" ${json_index} 1 json_char)
        
        if("]" STREQUAL "${json_char}")
            _sbeMoveToNextNonEmptyCharacter()
            break()
        elseif("," STREQUAL "${json_char}")
            math(EXPR json_${json_ArrayNestingLevel}_arrayIndex "${json_${json_ArrayNestingLevel}_arrayIndex} + 1")            
        endif()
                
        _sbeMoveToNextNonEmptyCharacter()
    endwhile()    
    
    if(${json_MaxArrayNestingLevel} LESS ${json_ArrayNestingLevel})
        set(json_MaxArrayNestingLevel ${json_ArrayNestingLevel})
    endif()
    math(EXPR json_ArrayNestingLevel "${json_ArrayNestingLevel} - 1")
endmacro()

macro(_sbeMoveToNextNonEmptyCharacter)
    math(EXPR json_index "${json_index} + 1")
    if(${json_index} LESS ${json_jsonLen})
        string(SUBSTRING "${json_string}" ${json_index} 1 json_char)
        while(${json_char} MATCHES "[ \t\n\r]" AND ${json_index} LESS ${json_jsonLen})
            math(EXPR json_index "${json_index} + 1")
            string(SUBSTRING "${json_string}" ${json_index} 1 json_char)
        endwhile()
    endif()
endmacro()

set(Downloaded_JS TRUE)

if(Downloaded_JS)
        file(DOWNLOAD "https://api.github.com/repos/kenkit/Cloud_Commander_UI/releases/latest" "${CMAKE_SOURCE_DIR}/build/json_release.json" SHOW_PROGRESS STATUS status)
        file(READ "${CMAKE_SOURCE_DIR}/build/json_release.json" JSON_OUTPUT)
        sbeParseJson(parsed_json JSON_OUTPUT)
       
    if(NOT status EQUAL 0)
        message(WARN "error: downloading failed
        status_code: ${status_code}
        status_string: ${status_string}")
        set(project_version "Unknown [offline build]")
    else()
       set(project_version ${parsed_json.tag_name})
       string(REPLACE "." ";" VERSION_LIST ${project_version})
        list(GET VERSION_LIST 0 MY_PROGRAM_VERSION_MAJOR)
        list(GET VERSION_LIST 1 MY_PROGRAM_VERSION_MINOR)
        list(GET VERSION_LIST 2 MY_PROGRAM_VERSION_PATCH)
        MATH(EXPR MY_PROGRAM_VERSION_PATCH "${MY_PROGRAM_VERSION_PATCH}+1")
        message("Version major:${MY_PROGRAM_VERSION_MAJOR} minor:${MY_PROGRAM_VERSION_MINOR} patch:${MY_PROGRAM_VERSION_PATCH}")
        set(project_version "${MY_PROGRAM_VERSION_MAJOR}.${MY_PROGRAM_VERSION_MINOR}.${MY_PROGRAM_VERSION_PATCH}")
        message("Current build:${project_version}")
    endif()     
endif()
if(NOT DEFINED post_configure_file)
    set(post_configure_file "${CMAKE_CURRENT_SOURCE_DIR}/version.h")
endif()
if(NOT DEFINED pre_configure_file)
    set(pre_configure_file "${post_configure_file}.in")
endif()
if(NOT EXISTS "${pre_configure_file}")
    message(FATAL_ERROR "Runtime error: the preconfigure file doesn't exist.")
endif()


# This variable describes where we record the state of the git repo.
set(git_state_file "${CMAKE_CURRENT_BINARY_DIR}/git-state")



# Function: GitStateChangedAction
# Description: this action is executed when the state of the git
#              repo changes (e.g. a commit is made).
function(GitStateChangedAction)
    # Read the git state file
    file(STRINGS "${git_state_file}" CONTENT)
    LIST(GET CONTENT 0 HELP_STRING)
    LIST(GET CONTENT 1 GIT_RETRIEVED_STATE)
    LIST(GET CONTENT 2 GIT_HEAD_SHA1)
    LIST(GET CONTENT 3 COMMIT_AUTHOR)
    LIST(GET CONTENT 4 COMMIT_MESSAGE)
    LIST(GET CONTENT 5 COMMIT_DATE)
    LIST(GET CONTENT 6 GIT_IS_DIRTY)
    LIST(GET CONTENT 7 PROJECT_VERSION)
 
    # Configure the file.
    configure_file("${pre_configure_file}" "${post_configure_file}" @ONLY)
endfunction()


###################################################
# There be dragons below here...                  #
###################################################


# Function: GetGitState
# Description: gets the current state of the git repo.
# Args:
#   _working_dir (in)  string; the directory from which git commands will be ran.
#   _hashvar     (out) string; the SHA1 hash for HEAD.
#   _dirty       (out) boolean; whether or not there are uncommitted changes.
#   _success     (out) boolean; whether or not both 
function(GetGitState _working_dir _hashvar _dirty _success)

	# Initialize our returns.
	set(${_hashvar} "GIT-NOTFOUND" PARENT_SCOPE)
	set(${_dirty} "false" PARENT_SCOPE)
	set(${_success} "false" PARENT_SCOPE)

	# Find git.
	if(NOT GIT_FOUND)
		find_package(Git QUIET)
	endif()
	if(NOT GIT_FOUND)
		return()
	endif()

	# Get the hash for HEAD.
	execute_process(COMMAND
		"${GIT_EXECUTABLE}" rev-parse --verify HEAD
		WORKING_DIRECTORY "${_working_dir}"
		RESULT_VARIABLE res
		OUTPUT_VARIABLE hash
		ERROR_QUIET
		OUTPUT_STRIP_TRAILING_WHITESPACE)
	if(NOT res EQUAL 0)
        # The git command failed.
		return()
	endif()

    # Record the SHA1 hash for HEAD.
    set(${_hashvar} "${hash}" PARENT_SCOPE)

	# Get whether or not the working tree is dirty.
	execute_process(COMMAND
		"${GIT_EXECUTABLE}" status --porcelain
		WORKING_DIRECTORY "${_working_dir}"
		RESULT_VARIABLE res
		OUTPUT_VARIABLE out
		ERROR_QUIET
		OUTPUT_STRIP_TRAILING_WHITESPACE)
	if(NOT res EQUAL 0)
		# The git command failed.
		return()
	endif()

	# If there were uncommitted changes, mark it as dirty.
	if (NOT "${out}" STREQUAL "")
		set(${_dirty} "true" PARENT_SCOPE)
	else()
		set(${_dirty} "false" PARENT_SCOPE)
	endif()

    # We got this far, so git must have cooperated.
	set(${_success} "true" PARENT_SCOPE)
endfunction()



# Function: GetGitStateSimple
# Description: gets the current state of the git repo and represent it with a string.
# Args:
#   _working_dir (in)  string; the directory from which git commands will be ran.
#   _state       (out) string; describes the current state of the repo.
function(GetGitStateSimple _working_dir _state)

    # Get the current state of the repo where the current list resides.
    GetGitState("${_working_dir}" hash dirty success)
    # We're going to construct a variable that represents the state of the repo.
   
    
	execute_process(COMMAND
		"${GIT_EXECUTABLE}"  log  --format=%ci -n1
		WORKING_DIRECTORY "${_working_dir}"
		RESULT_VARIABLE res
		OUTPUT_VARIABLE commit_date
		ERROR_QUIET
		OUTPUT_STRIP_TRAILING_WHITESPACE)
	if(NOT res EQUAL 0)
        message("Failed to get commit date")
		return()
	endif()
	execute_process(COMMAND
		"${GIT_EXECUTABLE}" log --format=%B -n1
		WORKING_DIRECTORY "${_working_dir}"
		RESULT_VARIABLE res
		OUTPUT_VARIABLE commit_message
		ERROR_QUIET
		OUTPUT_STRIP_TRAILING_WHITESPACE)
	if(NOT res EQUAL 0)
        message("Failed to get commit message")
		return()
	endif()

	execute_process(COMMAND
		"${GIT_EXECUTABLE}" log --format=%an -n1
		WORKING_DIRECTORY "${_working_dir}"
		RESULT_VARIABLE res
		OUTPUT_VARIABLE commit_author
		ERROR_QUIET
		OUTPUT_STRIP_TRAILING_WHITESPACE)
	if(NOT res EQUAL 0)
        message("Failed to get commit author")
		return()
	endif()

    set(help_string "\
This is a git state file. \
The next three lines are a success code, SHA1 hash, \
and whether or not there were uncommitted changes.")
    set(${_state} "${help_string}\n${success}\n${hash}\n${commit_author}\n${commit_message}\n${commit_date}\n${dirty}\n${project_version}" PARENT_SCOPE)
endfunction()



# Function: MonitorGit
# Description: this function sets up custom commands that make the build system
#              check the state of git before every build. If the state has
#              changed, then a file is configured.
function(MonitorGit)
    add_custom_target(AlwaysCheckGit
        DEPENDS ${pre_configure_file}
        BYPRODUCTS ${post_configure_file}
        COMMAND 
            ${CMAKE_COMMAND}
            -DGIT_FUNCTION=DoMonitoring
            -DGIT_WORKING_DIR=${CMAKE_CURRENT_SOURCE_DIR}
            -Dpre_configure_file=${pre_configure_file}
            -Dpost_configure_file=${post_configure_file}
            -P "${CMAKE_CURRENT_LIST_FILE}")
endfunction()



# Function: CheckGit
# Description: check if the git repo has changed. If so, update the state file.
# Args:
#   _working_dir    (in)  string; the directory from which git commands will be ran.
#   _state_changed (out)    bool; whether or no the state of the repo has changed.
function(CheckGit _working_dir _state_changed)

    # Get the state of the repo where the current list resides.
    GetGitStateSimple(${_working_dir} current_state)


    # Check if the state has changed compared to the backup.
    if(EXISTS "${git_state_file}")
        file(READ "${git_state_file}" OLD_HEAD_CONTENTS)
        if(OLD_HEAD_CONTENTS STREQUAL current_state)
            set(${_state_changed} "false" PARENT_SCOPE)
            return()
        endif()
    endif()

    # The state has changed.
    # We need to update the state file.
    file(WRITE "${git_state_file}" "${current_state}")
    set(${_state_changed} "true" PARENT_SCOPE)

endfunction()



# Function: Main
# Description: primary entry-point to the script. Functions are selected based
#              on the GIT_FUNCTION variable.
function(Main)
    if(GIT_FUNCTION STREQUAL DoMonitoring)
        # Check if the repo has changed.
        # If so, run the change action.
        CheckGit("${GIT_WORKING_DIR}" changed)
        if(changed)
            message(STATUS "Checking git... changed!")
            GitStateChangedAction()
        else()
            message(STATUS "Checking git... no change.")
        endif()
    else()
        # Start monitoring git.
        # This should only ever be run once when the module is imported.
        # Behind the scenes, all this does is setup a custom target.
        MonitorGit()
    endif()
endfunction()

# And off we go...
Main()
