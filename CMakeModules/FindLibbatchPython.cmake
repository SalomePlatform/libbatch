# Copyright (C) 2013-2021  CEA/DEN, EDF R&D, OPEN CASCADE
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
#
# See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
#
# Author: Adrien Bruneton
#

# Python libraries and interpreter detection for libBatch
# The interpreter is found first, and if OK, the corresponding libraries are searched.
# We ensure the version of the libraries matches the one of the interpreter. 
#

# 1. Load environment or any previously detected Python
IF(DEFINED ENV{PYTHON_ROOT_DIR})
  FILE(TO_CMAKE_PATH "$ENV{PYTHON_ROOT_DIR}" _PYTHON_ROOT_DIR_ENV)
  IF(APPLE)
    SET(_dflt_value "${_PYTHON_ROOT_DIR_ENV}/Python.framework/Versions/Current")
    SET(PYTHON_LIBRARY "${_PYTHON_ROOT_DIR_ENV}/Python.framework/Versions/Current/lib/libpython2.7.dylib")
    SET(PYTHON_INCLUDE_DIR "${_PYTHON_ROOT_DIR_ENV}/Python.framework/Versions/Current/Headers")
    LIST(APPEND CMAKE_PREFIX_PATH "${_dflt_value}")
    FIND_PACKAGE(PythonLibs)
    MESSAGE(STATUS "Mac Python: ${_dflt_value}")
    MESSAGE(STATUS "Python libraries: ${PYTHON_LIBRARY}")
    MESSAGE(STATUS "Python include dir: ${PYTHON_INCLUDE_DIR}")
  ELSE(APPLE)
    SET(_dflt_value "${_PYTHON_ROOT_DIR_ENV}")
  ENDIF(APPLE)
ELSE()
  # will be blank if no Python was previously loaded
  SET(_dflt_value "${PYTHON_ROOT_DIR_EXP}")
ENDIF()

#   Make cache entry 
SET(PYTHON_ROOT_DIR "${_dflt_value}" CACHE PATH "Path to Python directory (interpreter and libs)")

# python 3
SET(PythonInterp_FIND_VERSION 3)
SET(PythonInterp_FIND_VERSION_MAJOR 3)

# 2. Find package - config mode first (i.e. looking for XYZ-config.cmake)
IF(WIN32)
 IF(CMAKE_BUILD_TYPE STREQUAL Debug)
  SET(PythonInterp_FIND_VERSION _d)
  SET(PYTHON_DEFINITIONS "-DHAVE_DEBUG_PYTHON")
 ENDIF(CMAKE_BUILD_TYPE STREQUAL Debug)
ENDIF(WIN32)
IF(EXISTS "${PYTHON_ROOT_DIR}")
  # Hope to find direclty a CMake config file there
  SET(_CONF_DIR "${PYTHON_ROOT_DIR}/share/cmake") 

  # Try find_package in config mode with a hard-coded guess. This
  # has the priority.
  FIND_PACKAGE(Python CONFIG QUIET PATHS "${_CONF_DIR}")
  MARK_AS_ADVANCED(Python_DIR)
    
  IF (NOT PYTHON_FOUND)  
    LIST(APPEND CMAKE_PREFIX_PATH "${PYTHON_ROOT_DIR}")
  ELSE()
    MESSAGE(STATUS "Found Python in CONFIG mode!")
  ENDIF()
ENDIF()

# Otherwise try the standard way (module mode, with the standard CMake Find*** macro):
SALOME_FIND_PACKAGE(LibbatchPython PythonInterp MODULE)
SET(_found1 ${PYTHONINTERP_FOUND})

IF (PYTHONINTERP_FOUND)
  # Now ensure we find the Python libraries matching the interpreter:
  # This uses the variable PYTHON_EXECUTABLE
  GET_FILENAME_COMPONENT(_python_dir "${PYTHON_EXECUTABLE}" PATH)
  GET_FILENAME_COMPONENT(CMAKE_INCLUDE_PATH "${_python_dir}/../include/python${PYTHON_VERSION_MAJOR}.${PYTHON_VERSION_MINOR}" ABSOLUTE)
  GET_FILENAME_COMPONENT(CMAKE_LIBRARY_PATH "${_python_dir}/../lib" ABSOLUTE)
  # For a Windows install, this might look more like this:
  IF(WIN32)
    LIST(APPEND CMAKE_LIBRARY_PATH "${_python_dir}/libs" ABSOLUTE)
    LIST(APPEND CMAKE_INCLUDE_PATH "${_python_dir}/include" ABSOLUTE)
    SET(PYTHON_INCLUDE_DIR "${_python_dir}/include")
  ENDIF()
  # Override the EXACT and VERSION settings of the LibbatchPython module
  # to force the next call to SALOME_FIND_PACKAGE() to find the exact matching
  # version:
  SET(_old_EXACT ${LibbatchPython_FIND_VERSION_EXACT})
  SET(_old_VERSION "${LibbatchPython_FIND_VERSION}")
  SET(LibbatchPython_FIND_VERSION_EXACT TRUE)
  SET(LibbatchPython_FIND_VERSION "${PYTHON_VERSION_STRING}")
  # Prepare call to FIND_PACKAGE(PythonLibs) and ensure priority is given to 
  # the location found for the interpreter:
  GET_FILENAME_COMPONENT(_tmp "${_python_dir}" PATH)
  SALOME_FIND_PACKAGE(LibbatchPython PythonLibs MODULE)
  # Restore variables:
  SET(LibbatchPython_FIND_VERSION_EXACT ${_old_EXACT})
  SET(LibbatchPython_FIND_VERSION "${_old_VERSION}")
ENDIF()

# Set the FOUND flag for LibbatchPython:
SET(LIBBATCHPYTHON_FOUND FALSE)
IF (_found1 AND PYTHONLIBS_FOUND)
  SET(LIBBATCHPYTHON_FOUND TRUE)
ENDIF()

IF (LIBBATCHPYTHON_FOUND)
  MESSAGE(STATUS "Python interpreter and Python libraries found:")
  MESSAGE(STATUS "Python libraries: ${PYTHON_LIBRARY}")
  MESSAGE(STATUS "Python include dir: ${PYTHON_INCLUDE_DIR}")

  # 3. Set the root dir which was finally retained 
  # For Python this is the grand-parent of the
  # include directory:
  GET_FILENAME_COMPONENT(_tmp_ROOT_DIR "${PYTHON_INCLUDE_DIR}" PATH)
  IF(NOT WIN32)
    GET_FILENAME_COMPONENT(_tmp_ROOT_DIR "${_tmp_ROOT_DIR}" PATH)
  ENDIF()

  # 4. Warn if CMake found something not located under ENV(XYZ_ROOT_DIR)
  IF(DEFINED ENV{PYTHON_ROOT_DIR})
    SALOME_CHECK_EQUAL_PATHS(_res "${_tmp_ROOT_DIR}" "${_PYTHON_ROOT_DIR_ENV}")
    IF(NOT _res)
      MESSAGE(WARNING "Python was found, but not a the path given by the "
"environment PYTHON_ROOT_DIR! Is the variable correctly set?")
    ELSE()
      MESSAGE(STATUS "Python found directory matches what was specified in the PYTHON_ROOT_DIR, all good!")    
    ENDIF()
  ENDIF()

  # 5. Conflict detection
  # 5.1  From another prerequisite using Python
  IF(PYTHON_ROOT_DIR_EXP)
      SALOME_CHECK_EQUAL_PATHS(_res "${_tmp_ROOT_DIR}" "${PYTHON_ROOT_DIR_EXP}") 
      IF(NOT _res)
         MESSAGE(WARNING "Warning: Python: detected version conflicts with a previously found Python!"
                          "The two paths are " ${_tmp_ROOT_DIR} " vs " ${PYTHON_ROOT_DIR_EXP})
      ELSE()
          MESSAGE(STATUS "Python directory matches what was previously exposed by another prereq, all good!")
      ENDIF()        
  ENDIF()

  # 6. Finally retained variable:
  SET(PYTHON_ROOT_DIR "${_tmp_ROOT_DIR}")

  # 7. Specifics
  ## None here  
ELSE(LIBBATCHPYTHON_FOUND)
  MESSAGE(STATUS "Python was only partially (or not at all) found .")  
ENDIF(LIBBATCHPYTHON_FOUND)

