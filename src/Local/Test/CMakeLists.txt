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

# Just copy the test scripts to the binary dir
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/seta.sh ${CMAKE_CURRENT_BINARY_DIR}/seta.sh COPYONLY)
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/setb.sh ${CMAKE_CURRENT_BINARY_DIR}/setb.sh COPYONLY)
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/test-script.sh ${CMAKE_CURRENT_BINARY_DIR}/test-script.sh COPYONLY)

# set the include directories
include_directories(${CMAKE_SOURCE_DIR}/src/Core)
include_directories(${CMAKE_SOURCE_DIR}/src/Core/Test)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

# Build the test programs and add the tests
IF (HAS_SH)

    # Build the executable to use for the local test program
    add_executable(Exec_Test Exec_Test.cxx)
    GET_TARGET_PROPERTY(EXEC_TEST_FULL_PATH_TEMP Exec_Test LOCATION)
    GET_FILENAME_COMPONENT(EXEC_TEST_NAME_TEMP ${EXEC_TEST_FULL_PATH_TEMP} NAME)
    SET(EXEC_TEST_FULL_PATH ${EXEC_TEST_FULL_PATH_TEMP} CACHE INTERNAL "")
    SET(EXEC_TEST_NAME ${EXEC_TEST_NAME_TEMP} CACHE INTERNAL "")
    
    # Configure the config file for the test
    CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/Test_Local_config.h.in ${CMAKE_CURRENT_BINARY_DIR}/Test_Local_config.h)

    add_executable(Test_Local_SH Test_Local_SH.cxx)
    target_link_libraries(Test_Local_SH Batch SimpleParser)
    ADD_TEST(Local_SH Test_Local_SH)

ENDIF (HAS_SH)

IF (HAS_SSH)
    add_executable(Test_Local_SSH Test_Local_SSH.cxx)
    target_link_libraries(Test_Local_SSH Batch SimpleParser)
    ADD_TEST(Local_SSH Test_Local_SSH)
ENDIF (HAS_SSH)

IF (HAS_RSH)
    add_executable(Test_Local_RSH Test_Local_RSH.cxx)
    target_link_libraries(Test_Local_RSH Batch SimpleParser)
    ADD_TEST(Local_RSH Test_Local_RSH)
ENDIF (HAS_RSH)
