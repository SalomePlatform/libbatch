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
    p['EXECUTABLE'] = './copied-test-script.sh'
    p['NAME'] = 'Test_Local_SH'
    p['WORKDIR'] = '/tmp'
    p['INFILE'] = [('seta.sh', 'copied-seta.sh'), ('setb.sh', 'copied-setb.sh'),
                   ('test-script.sh', 'copied-test-script.sh')]
    p['OUTFILE'] = [('result.txt', 'orig-result.txt')]
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
    state = 'Unknown'
    i=0
    while state != 'Done' and i<10:
        time.sleep(0.1)
        i+=1
        jinfo = jobid.queryJob()
        try:
            state = jinfo.getParametre()['STATE']
        except KeyError:
            pass
        print "State is", state

    if state != "Done":
        print "Error: Job not finished after timeout"
        return 1;

    print "Job", jobid, "is done"

    # wait for 2 more seconds for the copy of output files and the cleanup
    # (there's no cleaner way to do that yet)
    time.sleep(2)

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
