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

import config
import os
import sys
import time

# Import libBatch library
from libBatch_Swig import *

def work():
    print "*******************************************************************************************"
    print "This script tests the local batch submission based on SH. No specific configuration is"
    print "needed for this test."
    print "*******************************************************************************************"

    # eventually remove any previous result
    if (os.path.exists('result.txt')):
        os.remove('result.txt')

    # Define the job...
    job = Job()
    # ... and its parameters ...
    p = {}
    p[EXECUTABLE] = './copied-' + config.EXEC_TEST_NAME
    p[ARGUMENTS]  = ["copied-seta.sh", "copied-setb.sh", "orig-result.txt"];
    p[NAME] = 'Test_Python_Local_SH'
    p[WORKDIR] = config.TEST_LOCAL_SH_WORK_DIR
    p[INFILE] = [('seta.sh', 'copied-seta.sh'), ('setb.sh', 'copied-setb.sh'),
                   (config.EXEC_TEST_FULL_PATH, 'copied-' + config.EXEC_TEST_NAME)]
    p[OUTFILE] = [('result.txt', 'orig-result.txt')]
    job.setParametre(p)
    # ... and its environment
    e = {}
    job.setEnvironnement(e)
    print job

    # Get the catalog
    c = BatchManagerCatalog.getInstance()

    # Create a BatchManager of type Local_SSH on localhost
    bm = c('SH')('localhost')

    # Submit the job to the BatchManager
    jobid = bm.submitJob(job)
    print jobid

    # Query the job
    jobid.queryJob()

    # Wait for the end of the job
    state = bm.waitForJobEnd(jobid, config.TEST_LOCAL_SH_TIMEOUT);

    if state != FINISHED and state != FAILED:
        print "Error: Job not finished after timeout"
        return 1;

    print "Job", jobid, "is done"

    # test the result file
    exp = "c = 12"
    f = open('result.txt')
    res = f.read().strip()
    print "result found : %s, expected : %s" % (res, exp)

    if (res == exp):
        return 0
    else:
        return 1

if __name__ == "__main__":
    retcode = work()
    sys.exit(retcode)
