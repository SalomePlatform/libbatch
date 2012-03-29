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

SET(CLASS_LIST LSF/Batch_BatchManager_eLSF
               LSF/Batch_FactBatchManager_eLSF
               LSF/Batch_JobInfo_eLSF
   )

IF (BUILD_LSF_INTERFACE AND LSF_FOUND)
    SET(CLASS_LIST ${CLASS_LIST}
                   LSF/Batch_BatchManager_LSF
                   LSF/Batch_FactBatchManager_LSF
                   LSF/Batch_Job_LSF
                   LSF/Batch_JobInfo_LSF
       )
ENDIF (BUILD_LSF_INTERFACE AND LSF_FOUND)

APPEND_CLASSES_TO_SRC_FILES(${CLASS_LIST})

IF (TEST_ENABLED)
    add_subdirectory(Test)
ENDIF (TEST_ENABLED)
