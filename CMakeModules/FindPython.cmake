#  Copyright (C) 2007-2008  CEA/DEN, EDF R&D, OPEN CASCADE
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

# We use a custom FindPYTHON.cmake because the one shipped with cmake doesn't search
# properly for the version number.

IF (NOT Python_FIND_QUIETLY)
    MESSAGE(STATUS "Looking for Python...")
ENDIF (NOT Python_FIND_QUIETLY)

IF(WINDOWS)
  IF(CMAKE_BUILD_TYPE STREQUAL Release)
    FIND_PROGRAM(PYTHON_EXECUTABLE python)
  ELSE(CMAKE_BUILD_TYPE STREQUAL Release)
    FIND_PROGRAM(PYTHON_EXECUTABLE python_d)
  ENDIF(CMAKE_BUILD_TYPE STREQUAL Release)
ELSE(WINDOWS)
  FIND_PROGRAM(PYTHON_EXECUTABLE python DOC "Python interpreter")
ENDIF(WINDOWS)

EXECUTE_PROCESS(
  COMMAND ${PYTHON_EXECUTABLE} -c "import sys; sys.stdout.write(sys.version[:3])"
  OUTPUT_VARIABLE PYTHON_VERSION
  )

SET(PYTHON_ROOT ${PYTHON_EXECUTABLE})
GET_FILENAME_COMPONENT(PYTHON_ROOT ${PYTHON_ROOT} PATH)
GET_FILENAME_COMPONENT(PYTHON_ROOT ${PYTHON_ROOT} PATH)

FIND_PATH(PYTHON_INCLUDE_PATH Python.h
                              PATHS ${PYTHON_ROOT}/include ${PYTHON_ROOT}/include/python${PYTHON_VERSION}
                              DOC "Python include path")

IF(WINDOWS)
  STRING(REPLACE "." "" PYTHON_VERSION_WITHOUT_DOT ${PYTHON_VERSION})
  IF(CMAKE_BUILD_TYPE STREQUAL Release)
    FIND_LIBRARY(PYTHON_LIBRARIES python${PYTHON_VERSION_WITHOUT_DOT} ${PYTHON_ROOT}/libs DOC "Python libraries")
  ELSE(CMAKE_BUILD_TYPE STREQUAL Release)
    FIND_LIBRARY(PYTHON_LIBRARIES python${PYTHON_VERSION_WITHOUT_DOT}_d ${PYTHON_ROOT}/libs DOC "Python libraries")
  ENDIF(CMAKE_BUILD_TYPE STREQUAL Release)
ELSE(WINDOWS)
  FIND_LIBRARY(PYTHON_LIBRARIES python${PYTHON_VERSION} ${PYTHON_ROOT}/lib DOC "Python libraries")
ENDIF(WINDOWS)

SET(PYTHON_INCLUDES -I${PYTHON_INCLUDE_PATH})
SET(PYTHON_LIBS ${PYTHON_LIBRARIES})

IF(WINDOWS)
  IF(CMAKE_BUILD_TYPE STREQUAL Release)
  ELSE(CMAKE_BUILD_TYPE STREQUAL Release)
    SET(PYTHON_INCLUDES ${PYTHON_INCLUDES} -DHAVE_DEBUG_PYTHON)
  ENDIF(CMAKE_BUILD_TYPE STREQUAL Release)
ENDIF(WINDOWS)

SET(Python_FOUND ${PYTHON_EXECUTABLE} AND  ${PYTHON_LIBRARIES} AND ${PYTHON_INCLUDE_PATH})

IF (Python_FOUND)

    IF (NOT Python_FIND_QUIETLY)
        MESSAGE(STATUS "Found Python: ${PYTHON_EXECUTABLE} (version ${PYTHON_VERSION})")
    ENDIF (NOT Python_FIND_QUIETLY)

ELSE (Python_FOUND)

    IF (Python_FIND_REQUIRED)
        MESSAGE(FATAL_ERROR "Python not found")
    ELSE (Python_FIND_REQUIRED)
        IF (NOT Python_FIND_QUIETLY)
            MESSAGE(STATUS "Python not found")
        ENDIF (NOT Python_FIND_QUIETLY)
    ENDIF (Python_FIND_REQUIRED)

ENDIF (Python_FOUND)
