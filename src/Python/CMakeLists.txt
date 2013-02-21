#  Copyright (C) 2007-2012  CEA/DEN, EDF R&D, OPEN CASCADE
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

INCLUDE(UseSWIG)

INCLUDE_DIRECTORIES(${PYTHON_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})

SET(SWIG_SRC_FILE libBatch_Swig.i)
SET_SOURCE_FILES_PROPERTIES(${SWIG_SRC_FILE} PROPERTIES CPLUSPLUS ON
                                                        SWIG_FLAGS "-shadow")
SWIG_ADD_MODULE(libbatch python ${SWIG_SRC_FILE})
SWIG_LINK_LIBRARIES(libbatch batch ${PYTHON_LIBRARIES})

SET(PYTHON_VERSION "${PYTHON_VERSION_MAJOR}.${PYTHON_VERSION_MINOR}")
INSTALL(TARGETS ${SWIG_MODULE_libbatch_REAL_NAME} DESTINATION lib/python${PYTHON_VERSION}/site-packages)
INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/libbatch.py
        DESTINATION lib/python${PYTHON_VERSION}/site-packages)

SET_DIRECTORY_PROPERTIES(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES libbatch.py)

IF (TEST_ENABLED)
    add_subdirectory(Test)
ENDIF (TEST_ENABLED)
