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
 * Test_ePBS.cxx :
 *
 * Author : Renaud BARATE - EDF R&D
 * Date   : April 2009
 *
 */

#include <iostream>
#include <fstream>
#include <cstring>

#include <Batch_Job.hxx>
#include <Batch_BatchManagerCatalog.hxx>
#include <Batch_FactBatchManager.hxx>
#include <Batch_FactBatchManager_eClient.hxx>
#include <Batch_BatchManager.hxx>
#include <Batch_BatchManager_eClient.hxx>

#include <SimpleParser.hxx>

using namespace std;
using namespace Batch;

void print_usage()
{
  cout << "usage: Test_ePBS PROTOCOL" << endl;
  cout << "    PROTOCOL      \"SSH\" or \"RSH\"" << endl;
}

int main(int argc, char** argv)
{
  // Parse argument
  if (argc != 2) {
    print_usage();
    return 1;
  }
  CommunicationProtocolType protocol;
  if (strcmp(argv[1], "SSH") == 0)
    protocol = SSH;
  else if (strcmp(argv[1], "RSH") == 0)
    protocol = RSH;
  else {
    print_usage();
    return 1;
  }

  cout << "*******************************************************************************************" << endl;
  cout << "This program tests the batch submission based on PBS emulation. Passwordless authentication" << endl;
  cout << "must be used for this test to pass. For SSH, this can be configured with ssh-agent for" << endl;
  cout << "instance. For RSH, this can be configured with the .rhosts file." << endl;
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
    p[EXECUTABLE]    = "./test-script.sh";
    p[NAME]          = string("Test_ePBS_") + argv[1];
    p[WORKDIR]       = homedir + "/tmp/Batch";
    p[INFILE]        = Couple("seta.sh", "tmp/Batch/seta.sh");
    p[INFILE]       += Couple("setb.sh", "tmp/Batch/setb.sh");
    p[OUTFILE]       = Couple("result.txt", "tmp/Batch/result.txt");
    p[TMPDIR]        = "tmp/Batch/";
    p[NBPROC]        = 1;
    p[MAXWALLTIME]   = 1;
    p[MAXRAMSIZE]    = 1;
    p[HOMEDIR]       = homedir;
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
    FactBatchManager_eClient * fbm = (FactBatchManager_eClient *)(c("ePBS"));
    BatchManager_eClient * bm = (*fbm)(host.c_str(), user.c_str(), protocol, "lam");

    // Submit the job to the BatchManager
    JobId jobid = bm->submitJob(job);
    cout << jobid.__repr__() << endl;

    // Wait for the end of the job
    string state = bm->waitForJobEnd(jobid, timeout);

    if (state == FINISHED || state == FAILED) {
      cout << "Job " << jobid.__repr__() << " is done" << endl;
      bm->importOutputFiles(job, "resultdir/seconddirname");
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
    resultParser.parse("resultdir/seconddirname/result.txt");
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
