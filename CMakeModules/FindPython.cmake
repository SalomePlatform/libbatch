#  Copyright (C) 2007-2010  CEA/DEN, EDF R&D, OPEN CASCADE
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

IF(WIN32 AND NOT CMAKE_BUILD_TYPE STREQUAL Release)
    FIND_PROGRAM(PYTHON_DEBUG python_d)
    mark_as_advanced(PYTHON_DEBUG)
ENDIF(WIN32 AND NOT CMAKE_BUILD_TYPE STREQUAL Release)

IF (PYTHON_DEBUG)
    SET(PYTHON_EXECUTABLE ${PYTHON_DEBUG} CACHE STRING "Python interpreter")
ELSE (PYTHON_DEBUG)
    IF(MSVC AND NOT CMAKE_BUILD_TYPE STREQUAL Release)
        MESSAGE(STATUS "Warning! Python debug executable not found. To build Swig module, you will need to install it or compile in Release mode")
    ELSE(MSVC AND NOT CMAKE_BUILD_TYPE STREQUAL Release)
        FIND_PROGRAM(PYTHON_EXECUTABLE python DOC "Python interpreter")
    ENDIF(MSVC AND NOT CMAKE_BUILD_TYPE STREQUAL Release)
ENDIF (PYTHON_DEBUG)

IF (PYTHON_EXECUTABLE)
    IF (NOT Python_FIND_QUIETLY)
        MESSAGE(STATUS "Python executable: ${PYTHON_EXECUTABLE}")
    ENDIF (NOT Python_FIND_QUIETLY)

    EXECUTE_PROCESS(
        COMMAND ${PYTHON_EXECUTABLE} -c "import sys; sys.stdout.write(sys.version[:3])"
        OUTPUT_VARIABLE PYTHON_VERSION
    )

    IF (NOT Python_FIND_QUIETLY)
        MESSAGE(STATUS "Python version: ${PYTHON_VERSION}")
    ENDIF (NOT Python_FIND_QUIETLY)

    EXECUTE_PROCESS(
        COMMAND ${PYTHON_EXECUTABLE} -c "import sys; import distutils.sysconfig; sys.stdout.write(distutils.sysconfig.get_python_inc())"
        OUTPUT_VARIABLE PYTHON_DETECTED_INCLUDE_DIR
    )

    SET(PYTHON_HEADER Python.h)
    FIND_PATH(PYTHON_INCLUDE_DIRS ${PYTHON_HEADER}
                                  PATHS ${PYTHON_DETECTED_INCLUDE_DIR}
                                  DOC "Python include directories")

    IF (NOT Python_FIND_QUIETLY)
        IF (PYTHON_INCLUDE_DIRS)
            MESSAGE(STATUS "Python include directories: ${PYTHON_INCLUDE_DIRS}")
        ELSE (PYTHON_INCLUDE_DIRS)
            MESSAGE(STATUS "Could not find Python include directories (looked for file ${PYTHON_HEADER} in directory ${PYTHON_DETECTED_INCLUDE_DIR})")
        ENDIF (PYTHON_INCLUDE_DIRS)        
    ENDIF (NOT Python_FIND_QUIETLY)

    FILE(WRITE ${CMAKE_CURRENT_BINARY_DIR}/findpythonlib.py
         "import sys\n"
         "import os\n"
         "import distutils.sysconfig\n"
         "libdir = distutils.sysconfig.get_config_var('LIBDIR')\n"
         "if libdir is None:\n"
         "    prefix = distutils.sysconfig.get_config_var('prefix')\n"
         "    libdir = prefix + ';' + os.path.join(prefix, 'lib') + ';' + os.path.join(prefix, 'libs')\n"
         "sys.stdout.write(libdir)"
    )
    
    EXECUTE_PROCESS(
        COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/findpythonlib.py
        OUTPUT_VARIABLE PYTHON_DETECTED_LIB_DIRS
    )

    IF(WIN32)
        STRING(REPLACE "." "" PYTHON_VERSION_WITHOUT_DOT ${PYTHON_VERSION})
        IF(PYTHON_DEBUG)
            SET(LIB_NAME python${PYTHON_VERSION_WITHOUT_DOT}_d)
        ELSE(PYTHON_DEBUG)
            SET(LIB_NAME python${PYTHON_VERSION_WITHOUT_DOT})
        ENDIF(PYTHON_DEBUG)
    ELSE(WIN32)
        SET(LIB_NAME python${PYTHON_VERSION})
    ENDIF(WIN32)
    
    FIND_LIBRARY(PYTHON_LIBRARIES ${LIB_NAME} ${PYTHON_DETECTED_LIB_DIRS} DOC "Python libraries")

    IF (NOT Python_FIND_QUIETLY)
        IF (PYTHON_LIBRARIES)
            MESSAGE(STATUS "Python libraries: ${PYTHON_LIBRARIES}")
        ELSE (PYTHON_LIBRARIES)
            MESSAGE(STATUS "Could not find Python libraries (looked for library ${LIB_NAME} in directory ${PYTHON_DETECTED_LIB_DIRS})")
        ENDIF (PYTHON_LIBRARIES)
    ENDIF (NOT Python_FIND_QUIETLY)

ENDIF (PYTHON_EXECUTABLE)

IF(PYTHON_EXECUTABLE AND PYTHON_LIBRARIES AND PYTHON_INCLUDE_DIRS)
  SET(Python_FOUND True)
ENDIF(PYTHON_EXECUTABLE AND PYTHON_LIBRARIES AND PYTHON_INCLUDE_DIRS)

IF (Python_FOUND)

    IF (NOT Python_FIND_QUIETLY)
        MESSAGE(STATUS "OK, working Python installation found")
    ENDIF (NOT Python_FIND_QUIETLY)

ELSE (Python_FOUND)

    IF (Python_FIND_REQUIRED)
        MESSAGE(FATAL_ERROR "No working Python installation found")
    ELSE (Python_FIND_REQUIRED)
        IF (NOT Python_FIND_QUIETLY)
            MESSAGE(STATUS "No working Python installation found")
        ENDIF (NOT Python_FIND_QUIETLY)
    ENDIF (Python_FIND_REQUIRED)

ENDIF (Python_FOUND)
