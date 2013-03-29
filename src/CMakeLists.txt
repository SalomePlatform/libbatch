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

MACRO(APPEND_CLASSES_TO_SRC_FILES)
    FOREACH(CLASS ${ARGV})
        LIST(APPEND SRC_FILES ${CLASS}.cxx)
    ENDFOREACH(CLASS ${ARGV})
    SET(SRC_FILES ${SRC_FILES} PARENT_SCOPE)
ENDMACRO(APPEND_CLASSES_TO_SRC_FILES)

MACRO(APPEND_CLASSES_TO_HDR_FILES)
    FOREACH(CLASS ${ARGV})
        LIST(APPEND HDR_FILES ${CLASS}.hxx)
    ENDFOREACH(CLASS ${ARGV})
    SET(HDR_FILES ${HDR_FILES} PARENT_SCOPE)
ENDMACRO(APPEND_CLASSES_TO_HDR_FILES)

IF (MSVC)
  add_definitions(/wd4251 /wd4290)  # Disable annoying Visual C++ warnings
ENDIF (MSVC)

include_directories(${PTHREAD_INCLUDE_DIR})

add_subdirectory (Core)

IF (BUILD_LOCAL_SUBMISSION)
    add_subdirectory (Local)
ENDIF (BUILD_LOCAL_SUBMISSION)

add_subdirectory (CCC)
add_subdirectory (LSF)
add_subdirectory (PBS)
add_subdirectory (SGE)
add_subdirectory (LoadLeveler)
add_subdirectory (Slurm)

# Vishnu is not available for Windows
IF (NOT WIN32)
    add_subdirectory (Vishnu)
ENDIF (NOT WIN32)

add_library(batch SHARED ${SRC_FILES})

include_directories(${CMAKE_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/Core)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Local)
target_link_libraries(batch ${PTHREAD_LIBRARY})

IF (WIN32)
    target_link_libraries(batch ws2_32)
ENDIF (WIN32)

# If necessary, activate all warnings (useful for debugging and code cleaning)
# add_definitions(-Wall -Werror -ansi -pedantic)

INSTALL(TARGETS batch DESTINATION lib)
INSTALL(FILES ${HDR_FILES} DESTINATION include/libbatch)

IF (BUILD_PYTHON_WRAPPING AND PYTHONINTERP_FOUND AND PYTHONLIBS_FOUND AND SWIG_FOUND)
    add_subdirectory (Python)
ENDIF (BUILD_PYTHON_WRAPPING AND PYTHONINTERP_FOUND AND PYTHONLIBS_FOUND AND SWIG_FOUND)
