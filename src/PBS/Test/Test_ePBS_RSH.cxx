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

#include <SimpleParser.hxx>

#ifdef WIN32
#include <Windows.h>
#define sleep(seconds) Sleep((seconds)*1000)
#define usleep(useconds) Sleep((useconds)/1000)
#endif

using namespace std;
using namespace Batch;

int main(int argc, char** argv)
{
  cout << "*******************************************************************************************" << endl;
  cout << "This program tests the batch submission based on PBS emulation with RSH. Passwordless RSH" << endl;
  cout << "authentication must be used for this test to pass (this can be configured with the .rhosts" << endl;
  cout << "file). You also need to create a directory \"tmp/Batch\" in your home directory on the PBS" << endl;
  cout << "server before running this test." << endl;
  cout << "*******************************************************************************************" << endl;

  // eventually remove any previous result
  remove("result.txt");

  try {
    // Parse the test configuration file
    SimpleParser parser;
    parser.parseTestConfigFile();
    const string & homedir = parser.getValue("TEST_EPBS_HOMEDIR");
    const string & host = parser.getValue("TEST_EPBS_HOST");
    const string & user = parser.getValue("TEST_EPBS_USER");
    const string & queue = parser.getValue("TEST_EPBS_QUEUE");
    int timeout = parser.getValueAsInt("TEST_EPBS_TIMEOUT");

    // Define the job...
    Job job;
    // ... and its parameters ...
    Parametre p;
    p["EXECUTABLE"]    = "./test-script.sh";
    p["NAME"]          = "Test_ePBS_RSH";
    p["WORKDIR"]       = homedir + "/tmp/Batch";
    p["INFILE"]        = Couple("seta.sh", "tmp/Batch/seta.sh");
    p["INFILE"]       += Couple("setb.sh", "tmp/Batch/setb.sh");
    p["OUTFILE"]       = Couple("result.txt", "tmp/Batch/result.txt");
    p["TMPDIR"]        = "tmp/Batch/";
    p["USER"]          = user;
    p["NBPROC"]        = 1;
    p["MAXWALLTIME"]   = 1;
    p["MAXRAMSIZE"]    = 4;
    p["HOMEDIR"]       = homedir;
    p["QUEUE"]         = queue;
    job.setParametre(p);
    // ... and its environment (SSH_AUTH_SOCK env var is important for ssh agent authentication)
    Environnement e;
    const char * sshAuthSock = getenv("SSH_AUTH_SOCK");
    if (sshAuthSock != NULL) e["SSH_AUTH_SOCK"] = sshAuthSock;
    job.setEnvironnement(e);
    cout << job << endl;

    // Get the catalog
    BatchManagerCatalog& c = BatchManagerCatalog::getInstance();

    // Create a BatchManager of type ePBS on localhost
    FactBatchManager_eClient * fbm = (FactBatchManager_eClient *)(c("ePBS"));
    BatchManager_eClient * bm = (*fbm)(host.c_str(), RSH, "lam");

    // Submit the job to the BatchManager
    JobId jobid = bm->submitJob(job);
    cout << jobid.__repr__() << endl;

    // Wait for the end of the job
    string state = "Undefined";
    for (int i=0 ; i<timeout/2 && state != "U"; i++) {
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
    cerr << "Error: " << e << endl;
    return 1;
  } catch (ParserException e) {
    cerr << "Parser error: " << e.what() << endl;
    return 1;
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
