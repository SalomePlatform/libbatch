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
 * Test_PBS.cxx :
 *
 * Author : Renaud BARATE - EDF R&D
 * Date   : September 2009
 *
 */

#include <iostream>
#include <fstream>

#include "Batch_Constants.hxx"
#include "Batch_Job.hxx"
#include "Batch_BatchManagerCatalog.hxx"
#include "Batch_FactBatchManager.hxx"
#include "Batch_BatchManager.hxx"

#include "SimpleParser.hxx"

using namespace std;
using namespace Batch;

int main(int argc, char** argv)
{
  cout << "*******************************************************************************************" << endl;
  cout << "This program tests the batch submission based on PBS." << endl;
  cout << "*******************************************************************************************" << endl;

  // eventually remove any previous result
  remove("result.txt");

  try {
    // Parse the test configuration file
    SimpleParser parser;
    parser.parseTestConfigFile();
    const string & host = parser.getValue("TEST_PBS_HOST");
    const string & user = parser.getValue("TEST_PBS_USER");
    const string & queue = parser.getValue("TEST_PBS_QUEUE");
    int timeout = parser.getValueAsInt("TEST_PBS_TIMEOUT");

    char * cwd =
#ifdef WIN32
      _getcwd(NULL, 0);
#else
      new char [PATH_MAX];
    getcwd(cwd, PATH_MAX);
#endif
    string workdir = cwd;
    delete [] cwd;

    // Define the job...
    Job job;
    // ... and its parameters ...
    Parametre p;
    p[EXECUTABLE]    = "test-script.sh";
    p[NAME]          = "Test_PBS";
    p[INFILE]        = Couple(workdir + "/seta.sh", "seta.sh");
    p[INFILE]       += Couple(workdir + "/setb.sh", "setb.sh");
    p[OUTFILE]       = Couple(workdir + "/result.txt", "result.txt");
    p[USER]          = user;
    p[NBPROC]        = 1;
    p[MAXWALLTIME]   = 1;
    p[MAXRAMSIZE]    = 4;
    p[QUEUE]         = queue;
    job.setParametre(p);
    // ... and its environment
    Environnement e;
    e["MYENVVAR"] = "MYVALUE";
    job.setEnvironnement(e);
    cout << job << endl;

    // Get the catalog
    BatchManagerCatalog& c = BatchManagerCatalog::getInstance();

    // Create a BatchManager of type ePBS on localhost
    FactBatchManager * fbm = c("PBS");
    BatchManager * bm = (*fbm)(host.c_str());

    // Submit the job to the BatchManager
    JobId jobid = bm->submitJob(job);
    cout << jobid.__repr__() << endl;

    // Wait for the end of the job
    string state = bm->waitForJobEnd(jobid, timeout);

    if (state == FINISHED || state == FAILED) {
      cout << "Job " << jobid.__repr__() << " is done" << endl;
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
  try {
    SimpleParser resultParser;
    resultParser.parse("result.txt");
    cout << "Result:" << endl << resultParser;
    const string & envvar = resultParser.getValue("MYENVVAR");
    int result = resultParser.getValueAsInt("c");
    if (envvar == "MYVALUE" && result == 12) {
      cout << "OK, Expected result found." << endl;
      return 0;
    } else {
      cerr << "Error, result is not the expected one (MYENVVAR = MYVALUE, c = 12)." << endl;
      return 1;
    }
  } catch (ParserException e) {
    cerr << "Parser error on result file: " << e.what() << endl;
    return 1;
  }
}
