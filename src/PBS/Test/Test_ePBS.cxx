//  Copyright (C) 2007-2008  CEA/DEN, EDF R&D, OPEN CASCADE
//
//  Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
//  CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
/*
 * Test_ePBS.cxx :
 *
 * Author : Renaud BARATE - EDF R&D
 * Date   : April 2009
 *
 */

#include <iostream>
#include <fstream>

#include <Batch_Job.hxx>
#include <Batch_BatchManagerCatalog.hxx>
#include <Batch_FactBatchManager.hxx>
#include <Batch_FactBatchManager_eClient.hxx>
#include <Batch_BatchManager.hxx>
#include <Batch_BatchManager_eClient.hxx>
#include <Test_PBS_config.h>

using namespace std;
using namespace Batch;

int main(int argc, char** argv)
{
  cout << "*******************************************************************************************" << endl;
  cout << "This program tests the batch submission based on PBS emulation. Passwordless SSH" << endl;
  cout << "authentication must be used for this test to pass (this can be configured with ssh-agent" << endl;
  cout << "for instance). You also need to create a directory \"tmp/Batch\" in your home directory on" << endl;
  cout << "the PBS server before running this test." << endl;
  cout << "*******************************************************************************************" << endl;

  // eventually remove any previous result
  remove("result.txt");

  try {
    // Define the job...
    Job job;
    // ... and its parameters ...
    Parametre p;
    p["EXECUTABLE"]    = "./test-script.sh";
    p["NAME"]          = "Test_ePBS";
    p["WORKDIR"]       = string(TEST_PBS_HOMEDIR) + "/tmp/Batch";
    p["INFILE"]        = Couple("seta.sh", "tmp/Batch/seta.sh");
    p["INFILE"]       += Couple("setb.sh", "tmp/Batch/setb.sh");
    p["OUTFILE"]       = Couple("result.txt", "tmp/Batch/result.txt");
    p["TMPDIR"]        = "tmp/Batch/";
    p["USER"]          = TEST_PBS_USER;
    p["NBPROC"]        = 1;
    p["MAXWALLTIME"]   = 1;
    p["MAXRAMSIZE"]    = 4;
    p["HOMEDIR"]       = TEST_PBS_HOMEDIR;
    p["QUEUE"]         = TEST_PBS_QUEUE;
    job.setParametre(p);
    // ... and its environment (SSH_AUTH_SOCK env var is important for ssh agent authentication)
    Environnement e;
    e["SSH_AUTH_SOCK"] = getenv("SSH_AUTH_SOCK");
    job.setEnvironnement(e);
    cout << job << endl;

    // Get the catalog
    BatchManagerCatalog& c = BatchManagerCatalog::getInstance();

    // Create a BatchManager of type ePBS on localhost
    FactBatchManager_eClient * fbm = (FactBatchManager_eClient *)(c("ePBS"));
    BatchManager_eClient * bm = (*fbm)(TEST_PBS_HOST, "ssh", "lam");

    // Submit the job to the BatchManager
    JobId jobid = bm->submitJob(job);
    cout << jobid.__repr__() << endl;

    // Wait for the end of the job
    string state = "Undefined";
    for (int i=0 ; i<10 && state != "U"; i++) {
      sleep(2);
      JobInfo jinfo = jobid.queryJob();
      state = jinfo.getParametre()["STATE"].str();
      cout << "State is \"" << state << "\"" << endl;
    }

    if (state == "U") {
      cout << "Job " << jobid.__repr__() << " is done" << endl;
      bm->importOutputFiles(job, ".");
    } else {
      cerr << "Timeout while executing job" << endl;
      return 1;
    }

  } catch (GenericException e) {
    cerr << "Batch library exception of type " << e.type << ": " << e.message << endl;
  }

  // test the result file
  string exp = "c = 12";
  string res;
  ifstream f("result.txt");
  getline(f, res);
  f.close();

  cout << "result found : " << res << ", expected : " << exp << endl;

  if (res == exp)
    return 0;
  else
    return 1;
}
