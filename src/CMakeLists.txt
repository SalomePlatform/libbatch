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

# These macros are used to create the lists of source files and header files in global variables.
# If we move to cmake 2.6, this can be done more cleanly with the PARENT_SCOPE option.
MACRO(APPEND_TO_GLOBAL_VAR VAR)
    SET(${VAR} ${${VAR}} ${ARGN} CACHE INTERNAL "")
ENDMACRO(APPEND_TO_GLOBAL_VAR)

MACRO(APPEND_CLASSES_TO_SRC_FILES)
    FOREACH(CLASS ${ARGV})
        APPEND_TO_GLOBAL_VAR(SRC_FILES_BUILD ${CLASS}.cxx)
    ENDFOREACH(CLASS ${ARGV})
ENDMACRO(APPEND_CLASSES_TO_SRC_FILES)

MACRO(APPEND_CLASSES_TO_HDR_FILES)
    FOREACH(CLASS ${ARGV})
        APPEND_TO_GLOBAL_VAR(HDR_FILES_BUILD ${CLASS}.hxx)
    ENDFOREACH(CLASS ${ARGV})
ENDMACRO(APPEND_CLASSES_TO_HDR_FILES)



add_subdirectory (Core)

IF (BUILD_LOCAL_SUBMISSION)
    add_subdirectory (Local)
ENDIF (BUILD_LOCAL_SUBMISSION)

add_subdirectory (LSF)
add_subdirectory (PBS)
add_subdirectory (SGE)

# Make a copy of the built value and clear the built value for the next run of cmake
SET(SRC_FILES ${SRC_FILES_BUILD} CACHE INTERNAL "")
SET(SRC_FILES_BUILD CACHE INTERNAL "")

SET(HDR_FILES ${HDR_FILES_BUILD} CACHE INTERNAL "")
SET(HDR_FILES_BUILD CACHE INTERNAL "")


add_library (Batch SHARED ${SRC_FILES})

include_directories(${CMAKE_BINARY_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Core)

target_link_libraries(Batch ${CMAKE_THREAD_LIBS_INIT})

IF (WIN32)
    target_link_libraries(Batch ws2_32)
ENDIF (WIN32)

# If necessary, activate all warnings (useful for debugging and code cleaning)
# add_definitions(-Wall -Werror -ansi -pedantic)

IF (BUILD_LSF_INTERFACE AND LSF_FOUND)
    include_directories(${LSF_INCLUDE_DIR})
    target_link_libraries(Batch ${LSF_LIBRARY} ${LSF_BAT_LIBRARY})
ENDIF (BUILD_LSF_INTERFACE AND LSF_FOUND)

IF (BUILD_PBS_INTERFACE AND PBS_FOUND)
    include_directories(${PBS_INCLUDE_DIR})
    target_link_libraries(Batch ${PBS_LIBRARY})
ENDIF (BUILD_PBS_INTERFACE AND PBS_FOUND)

INSTALL(TARGETS Batch DESTINATION lib)
INSTALL(FILES ${HDR_FILES} DESTINATION include/Batch)

IF (BUILD_PYTHON_WRAPPING AND Python_FOUND AND SWIG_FOUND)
    add_subdirectory (Python)
ENDIF (BUILD_PYTHON_WRAPPING AND Python_FOUND AND SWIG_FOUND)
