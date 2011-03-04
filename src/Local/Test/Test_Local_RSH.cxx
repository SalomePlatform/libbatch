//  Copyright (C) 2007-2010  CEA/DEN, EDF R&D, OPEN CASCADE
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
 * Test_Local_RSH.cxx :
 *
 * Author : Renaud BARATE - EDF R&D
 * Date   : April 2009
 *
 */

#include <iostream>
#include <fstream>

#include <Batch_Constants.hxx>
#include <Batch_Job.hxx>
#include <Batch_BatchManagerCatalog.hxx>
#include <Batch_FactBatchManager.hxx>
#include <Batch_BatchManager.hxx>

#include <SimpleParser.hxx>

using namespace std;
using namespace Batch;

int main(int argc, char** argv)
{
  cout << "*******************************************************************************************" << endl;
  cout << "This program tests the local batch submission based on RSH. Passwordless RSH authentication" << endl;
  cout << "must be used for this test to pass (this can be configured with the .rhosts file)." << endl;
  cout << "*******************************************************************************************" << endl;

  // eventually remove any previous result
  remove("result.txt");

  try {
    // Parse the test configuration file
    SimpleParser parser;
    parser.parseTestConfigFile();
    const string & workdir = parser.getValue("TEST_LOCAL_RSH_WORK_DIR");
    const string & exechost = parser.getValue("TEST_LOCAL_RSH_EXECUTION_HOST");
    const string & user = parser.getValue("TEST_LOCAL_RSH_USER");
    int timeout = parser.getValueAsInt("TEST_LOCAL_RSH_TIMEOUT");

    // Define the job...
    Job job;
    // ... and its parameters ...
    Parametre p;
    p[EXECUTABLE]    = "source copied-test-script.sh";
    p[NAME]          = "Test_Local_RSH";
    p[WORKDIR]       = workdir;
    p[INFILE]        = Couple("seta.sh", "copied-seta.sh");
    p[INFILE]       += Couple("setb.sh", "copied-setb.sh");
    p[INFILE]       += Couple("test-script.sh", "copied-test-script.sh");
    p[OUTFILE]       = Couple("result.txt", "orig-result.txt");
    p[EXECUTIONHOST] = exechost;
    p[USER]          = user;
    job.setParametre(p);
    // ... and its environment
    Environnement e;
    job.setEnvironnement(e);
    cout << job << endl;

    // Get the catalog
    BatchManagerCatalog& c = BatchManagerCatalog::getInstance();

    // Create a BatchManager of type Local_RSH on localhost
    FactBatchManager * fbm = c("RSH");
    if (fbm == NULL) {
      cerr << "Can't get RSH batch manager factory" << endl;
      return 1;
    }
    BatchManager * bm = (*fbm)("localhost");

    // Submit the job to the BatchManager
    JobId jobid = bm->submitJob(job);
    cout << jobid.__repr__() << endl;

    // Wait for the end of the job
    string state = bm->waitForJobEnd(jobid, timeout);

    if (state != FINISHED && state != FAILED) {
      cerr << "Error: Job not finished after timeout" << endl;
      return 1;
    }

    cout << "Job " << jobid.__repr__() << " is done" << endl;

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
