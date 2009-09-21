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

SET(CLASS_LIST Core/Batch_APIInternalFailureException
               Core/Batch_BatchManager_eClient
               Core/Batch_BatchManager
               Core/Batch_BatchManagerCatalog
               Core/Batch_BoolType
               Core/Batch_CharType
               Core/Batch_CommunicationProtocol
               Core/Batch_ConnexionFailureException
               Core/Batch_Couple
               Core/Batch_CoupleType
               Core/Batch_Date
               Core/Batch_DateType
               Core/Batch_EmulationException
               Core/Batch_Environnement
               Core/Batch_FactBatchManager_eClient
               Core/Batch_FactBatchManager
               Core/Batch_GenericException
               Core/Batch_GenericType
               Core/Batch_IntType
               Core/Batch_InvalidArgumentException
               Core/Batch_InvalidKeyException
               Core/Batch_Job
               Core/Batch_JobId
               Core/Batch_JobInfo
               Core/Batch_ListIsFullException
               Core/Batch_LongType
               Core/Batch_MapKey
               Core/Batch_MpiImpl
               Core/Batch_NotYetImplementedException
               Core/Batch_Parametre
               Core/Batch_RunTimeException
               Core/Batch_StringType
               Core/Batch_TypeMismatchException
               Core/Batch_Versatile
   )

IF (HAS_SH)
    SET(CLASS_LIST ${CLASS_LIST}
                   Core/Batch_CommunicationProtocolSH
       )
ENDIF (HAS_SH)

IF (HAS_RSH)
    SET(CLASS_LIST ${CLASS_LIST}
                   Core/Batch_CommunicationProtocolRSH
       )
ENDIF (HAS_RSH)

IF (HAS_SSH)
    SET(CLASS_LIST ${CLASS_LIST}
                   Core/Batch_CommunicationProtocolSSH
       )
ENDIF (HAS_SSH)

APPEND_CLASSES_TO_SRC_FILES(${CLASS_LIST})
APPEND_CLASSES_TO_HDR_FILES(${CLASS_LIST})
APPEND_CLASSES_TO_HDR_FILES(Core/Batch_Defines)

IF (TEST_ENABLED)
    add_subdirectory(Test)
ENDIF (TEST_ENABLED)
