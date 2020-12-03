# Tries to find the Readline library.
#
# Parameter Variables:
#
# Readline_ROOT_DIR
#   Root directory of Readline installation.
# Readline_USE_STATIC_LIBS
#   Set to TRUE for linking with static library.
#
# Defines Variables:
#
# Readline_FOUND
#   True if Readline was found.
# Readline_INCLUDE_DIRS
#   Directory where readline/readline.h resides.
# Readline_LIBRARIES
#   Path of Library.
# Readline_VERSION
#   Version found.
#
# Author:
# 
# Matthias Walter <matthias@matthiaswalter.org>
#
# Distributed under the Boost Software License, Version 1.0.
# (See http://www.boost.org/LICENSE_1_0.txt)
  
# Handle Readline_ROOT_DIR.
set(_Readline_ROOT_HINTS ${Readline_ROOT_DIR} ENV Readline_ROOT_DIR)

# Handle Readline_USE_STATIC_LIBS.
if(Readline_USE_STATIC_LIBS)
  set(_Readline_ORIG_CMAKE_FIND_LIBRARY_SUFFIXES ${CMAKE_FIND_LIBRARY_SUFFIXES})
  if(WIN32)
    set(CMAKE_FIND_LIBRARY_SUFFIXES .lib .a ${CMAKE_FIND_LIBRARY_SUFFIXES})
  else()
    set(CMAKE_FIND_LIBRARY_SUFFIXES .a )
  endif()
endif()

# Find header.
find_path(Readline_INCLUDE_DIR NAMES readline/readline.h HINTS ${_Readline_ROOT_HINTS})
set(Readline_INCLUDE_DIRS ${Readline_INCLUDE_DIR})

# Extract version number.
file(STRINGS "${Readline_INCLUDE_DIR}/readline/readline.h" _Readline_VERSION_MAJOR REGEX "^#define[\t ]+RL_VERSION_MAJOR[\t ]+[0-9].*")
string(REGEX REPLACE "^.*RL_VERSION_MAJOR[\t ]+([0-9]).*$" "\\1" Readline_VERSION_MAJOR "${_Readline_VERSION_MAJOR}")
file(STRINGS "${Readline_INCLUDE_DIR}/readline/readline.h" _Readline_VERSION_MINOR REGEX "^#define[\t ]+RL_VERSION_MINOR[\t ]+[0-9].*")
string(REGEX REPLACE "^.*RL_VERSION_MINOR[\t ]+([0-9]).*$" "\\1" Readline_VERSION_MINOR "${_Readline_VERSION_MINOR}")
set(Readline_VERSION "${Readline_VERSION_MAJOR}.${Readline_VERSION_MINOR}")

# Find library.
find_library(Readline_LIBRARY NAMES readline HINTS ${_Readline_ROOT_HINTS})
set(Readline_LIBRARIES ${Readline_LIBRARY})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Readline REQUIRED_VARS Readline_INCLUDE_DIRS Readline_LIBRARIES VERSION_VAR Readline_VERSION)

# Restore the original find_library ordering.
if(Readline_USE_STATIC_LIBS)
  set(CMAKE_FIND_LIBRARY_SUFFIXES ${_Readline_ORIG_CMAKE_FIND_LIBRARY_SUFFIXES})
endif()

