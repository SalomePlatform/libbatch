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
 * Test_Local_SH.cxx :
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
#include <Batch_BatchManager.hxx>

#include <Test_Local_config.h>
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
  cout << "This program tests the local batch submission based on SH. No specific configuration is" << endl;
  cout << "needed for this test." << endl;
  cout << "*******************************************************************************************" << endl;

  // eventually remove any previous result
  remove("result.txt");

  try {
    // Parse the test configuration file
    SimpleParser parser;
    parser.parseTestConfigFile();
    const string & workdir = parser.getValue("TEST_LOCAL_SH_WORK_DIR");
    int timeout = parser.getValueAsInt("TEST_LOCAL_SH_TIMEOUT");
    int finalizationTime = parser.getValueAsInt("TEST_LOCAL_SH_FINALIZATION_TIME");

    // Define the job...
    Job job;
    // ... and its parameters ...
    Parametre p;
    p["EXECUTABLE"] = string("./copied-") + EXEC_TEST_NAME;
    p["ARGUMENTS"]  = "copied-seta.sh";
    p["ARGUMENTS"] += "copied-setb.sh";
    p["ARGUMENTS"] += "orig-result.txt";
    p["NAME"]       = "Test_Local_SH";
    p["WORKDIR"]    = workdir;
    p["INFILE"]     = Couple("seta.sh", "copied-seta.sh");
    p["INFILE"]    += Couple("setb.sh", "copied-setb.sh");
    p["INFILE"]    += Couple(EXEC_TEST_NAME, string("copied-") + EXEC_TEST_NAME);
    p["OUTFILE"]    = Couple("result.txt", "orig-result.txt");
    job.setParametre(p);
    // ... and its environment
    Environnement e;
    job.setEnvironnement(e);
    cout << job << endl;

    // Get the catalog
    BatchManagerCatalog& c = BatchManagerCatalog::getInstance();

    // Create a BatchManager of type Local_SH on localhost
    FactBatchManager * fbm = c("SH");
    if (fbm == NULL) {
      cerr << "Can't get SH batch manager factory" << endl;
      return 1;
    }
    BatchManager * bm = (*fbm)("localhost");

    // Submit the job to the BatchManager
    JobId jobid = bm->submitJob(job);
    cout << jobid.__repr__() << endl;

    // Wait for the end of the job
    string state = "Unknown";
    for (int i=0 ; i<timeout*10 && state != FINISHED && state != FAILED ; i++) {
      usleep(100000);
      Versatile paramState = jobid.queryJob().getParametre()["STATE"];
      state = (paramState.size() > 0) ? paramState.str() : "Unknown";
      cout << "Job state is: " << state << endl;
    }

    if (state != FINISHED && state != FAILED) {
      cerr << "Error: Job not finished after timeout" << endl;
      return 1;
    }

    cout << "Job " << jobid.__repr__() << " is done" << endl;

    // wait for the copy of output files and the cleanup
    // (there's no cleaner way to do that yet)
    sleep(finalizationTime);

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
