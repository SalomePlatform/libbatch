#  Copyright (C) 2007-2013  CEA/DEN, EDF R&D, OPEN CASCADE
#
#  Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
#  CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
#
#  This library is free software; you can redistribute it and/or
#  modify it under the terms of the GNU Lesser General Public
#  License as published by the Free Software Foundation; either
#  version 2.1 of the License.
#
#  This library is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#  Lesser General Public License for more details.
#
#  You should have received a copy of the GNU Lesser General Public
#  License along with this library; if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
#
#  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
#

project (libbatch)

cmake_minimum_required (VERSION 2.8)
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/CMakeModules/)

find_package (PThread REQUIRED)

SET (BUILD_LOCAL_SUBMISSION TRUE CACHE BOOL "Build classes for local submission")
SET (BUILD_PYTHON_WRAPPING TRUE CACHE BOOL "Generate Python wrapping")
SET (TEST_ENABLED FALSE CACHE BOOL "Enable testing")

IF (TEST_ENABLED)
    ENABLE_TESTING()
ENDIF (TEST_ENABLED)

IF (BUILD_LOCAL_SUBMISSION)
    find_package (Local)
ENDIF (BUILD_LOCAL_SUBMISSION)
IF (BUILD_PYTHON_WRAPPING)
    FIND_PACKAGE(PythonInterp)
    IF (PYTHONINTERP_FOUND)
        get_filename_component(PYTHON_BIN_DIR "${PYTHON_EXECUTABLE}" PATH)
        get_filename_component(CMAKE_INCLUDE_PATH "${PYTHON_BIN_DIR}/../include/python${PYTHON_VERSION_MAJOR}.${PYTHON_VERSION_MINOR}" ABSOLUTE)
        get_filename_component(CMAKE_LIBRARY_PATH "${PYTHON_BIN_DIR}/../lib" ABSOLUTE)
    ENDIF (PYTHONINTERP_FOUND)
    FIND_PACKAGE(PythonLibs ${PYTHON_VERSION_STRING} EXACT)
    FIND_PACKAGE(SWIG)
ENDIF (BUILD_PYTHON_WRAPPING)

SET(CPACK_GENERATOR TGZ)
SET(CPACK_SOURCE_GENERATOR TGZ ZIP)
SET(CPACK_PACKAGE_VERSION_MAJOR 2)
SET(CPACK_PACKAGE_VERSION_MINOR 0)
SET(CPACK_PACKAGE_VERSION_PATCH 0)
SET(CPACK_SOURCE_IGNORE_FILES /\\\\.;/CVS/;~)
SET(CPACK_SOURCE_PACKAGE_FILE_NAME
    ${PROJECT_NAME}-${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH})

configure_file (${CMAKE_CURRENT_SOURCE_DIR}/config.h.in ${CMAKE_CURRENT_BINARY_DIR}/config.h)

INCLUDE(Summary)

add_subdirectory (src)
add_subdirectory (misc)

INCLUDE(CPack)

MARK_AS_ADVANCED(CMAKE_BACKWARDS_COMPATIBILITY EXECUTABLE_OUTPUT_PATH)
