//  Copyright (C) 2007-2012  CEA/DEN, EDF R&D, OPEN CASCADE
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
* BatchManager_eLSF.cxx : emulation of LSF client
*
* Auteur : Bernard SECHER - CEA DEN
* Mail   : mailto:bernard.secher@cea.fr
* Date   : Thu Apr 24 10:17:22 2008
* Projet : PAL Salome
*
*/

#include <stdlib.h>
#include <string.h>

#include <ctime>
#include <iostream>
#include <fstream>
#include <sstream>

#include <stdlib.h>

#ifdef WIN32
#include <direct.h>
#include <io.h>
#endif

#include <Batch_config.h>

#include "Batch_Constants.hxx"
#include "Batch_BatchManager_eClient.hxx"
#include "Batch_RunTimeException.hxx"

#ifdef MSVC
#define EXISTS(path) _access_s(path, 0) == 0
#else
#define EXISTS(path) access(path, F_OK) == 0
#endif

using namespace std;


namespace Batch {

  BatchManager_eClient::BatchManager_eClient(const Batch::FactBatchManager * parent, const char* host,
                                             const char * username,
                                             CommunicationProtocolType protocolType, const char* mpiImpl)
    : BatchManager(parent, host), _protocol(CommunicationProtocol::getInstance(protocolType)),
      _username(username)
  {
    // instanciation of mpi implementation needed to launch executable in batch script
    _mpiImpl = FactoryMpiImpl(mpiImpl);
  }

  // Destructeur
  BatchManager_eClient::~BatchManager_eClient()
  {
    if (_mpiImpl)
      delete _mpiImpl;
  }

  void BatchManager_eClient::exportInputFiles(const Job& job)
  {
    int status;
    Parametre params = job.getParametre();
    const Versatile & V = params[INFILE];
    Versatile::const_iterator Vit;

    status = _protocol.makeDirectory(string(params[TMPDIR]) + "/logs", _hostname, _username);
    if(status) {
      std::ostringstream oss;
      oss << status;
      std::string ex_mess("Error of connection on remote host ! status = ");
      ex_mess += oss.str();
      throw EmulationException(ex_mess.c_str());
    }

    // Second step : copy fileToExecute into
    // batch tmp files directory
    string executeFile = params[EXECUTABLE];
    if (executeFile.size() != 0) {
      status = _protocol.copyFile(executeFile, "", "",
                                  params[TMPDIR], _hostname, _username);
      if(status) {
        std::ostringstream oss;
        oss << status;
        std::string ex_mess("Error of connection on remote host ! status = ");
        ex_mess += oss.str();
        throw EmulationException(ex_mess.c_str());
      }

#ifdef WIN32
      // On Windows, we make the remote file executable afterward because
      // pscp does not preserve access permissions on files
      string subCommand = string("chmod u+x ") + string(params[TMPDIR]) + "/" +
                          string(params[EXECUTABLE]);
      string command = _protocol.getExecCommand(subCommand, _hostname, _username);
      cerr << command.c_str() << endl;
      status = system(command.c_str());
      if(status) {
        std::ostringstream oss;
        oss << status;
        std::string ex_mess("Error of connection on remote host ! status = ");
        ex_mess += oss.str();
        throw EmulationException(ex_mess.c_str());
      }
#endif
    }

    // Third step : copy filesToExportList into
    // batch tmp files directory
    for(Vit=V.begin(); Vit!=V.end(); Vit++) {
      CoupleType cpt  = *static_cast< CoupleType * >(*Vit);
      Couple inputFile = cpt;
      status = _protocol.copyFile(inputFile.getLocal(), "", "",
                                  inputFile.getRemote(), _hostname, _username);
      if(status) {
        std::ostringstream oss;
        oss << status;
        std::string ex_mess("Error of connection on remote host ! status = ");
        ex_mess += oss.str();
        throw EmulationException(ex_mess.c_str());
      }
    }

  }

  void BatchManager_eClient::importOutputFiles( const Job & job, const string directory )
  {
    Parametre params = job.getParametre();
    const Versatile & V = params[OUTFILE];
    Versatile::const_iterator Vit;

    // Create local result directory
    int status = CommunicationProtocol::getInstance(SH).makeDirectory(directory, "", "");
    if (status) {
      string mess("Directory creation failed. Status is :");
      ostringstream status_str;
      status_str << status;
      mess += status_str.str();
      cerr << mess << endl;
    }

    for(Vit=V.begin(); Vit!=V.end(); Vit++) {
      CoupleType cpt  = *static_cast< CoupleType * >(*Vit);
      Couple outputFile = cpt;
      status = _protocol.copyFile(outputFile.getRemote(), _hostname, _username,
                                  directory, "", "");
      if (status) {
        // Try to get what we can (logs files)
        // throw BatchException("Error of connection on remote host");
        std::string mess("Copy command failed ! status is :");
        ostringstream status_str;
        status_str << status;
        mess += status_str.str();
        cerr << mess << endl;
      }
    }

    // Copy logs
    status = _protocol.copyFile(string(params[TMPDIR]) + string("/logs"), _hostname, _username,
                                directory, "", "");
    if (status) {
      std::string mess("Copy logs directory failed ! status is :");
      ostringstream status_str;
      status_str << status;
      mess += status_str.str();
      cerr << mess << endl;
    }

  }

  bool BatchManager_eClient::importDumpStateFile( const Job & job, const string directory )
  {
    Parametre params = job.getParametre();

    // Create local result directory
    int status = CommunicationProtocol::getInstance(SH).makeDirectory(directory, "", "");
    if (status) {
      string mess("Directory creation failed. Status is :");
      ostringstream status_str;
      status_str << status;
      mess += status_str.str();
      cerr << mess << endl;
    }

    bool ret = true;
    status = _protocol.copyFile(string(params[TMPDIR]) + string("/dumpState*.xml"), _hostname, _username,
                                directory, "", "");
    if (status) {
      // Try to get what we can (logs files)
      // throw BatchException("Error of connection on remote host");
      std::string mess("Copy command failed ! status is :");
      ostringstream status_str;
      status_str << status;
      mess += status_str.str();
      cerr << mess << endl;
      ret = false;
    }
    return ret;
  }

  MpiImpl *BatchManager_eClient::FactoryMpiImpl(string mpiImpl)
  {
    if(mpiImpl == "lam")
      return new MpiImpl_LAM();
    else if(mpiImpl == "mpich1")
      return new MpiImpl_MPICH1();
    else if(mpiImpl == "mpich2")
      return new MpiImpl_MPICH2();
    else if(mpiImpl == "openmpi")
      return new MpiImpl_OPENMPI();
    else if(mpiImpl == "slurm")
      return new MpiImpl_SLURM();
    else if(mpiImpl == "prun")
      return new MpiImpl_PRUN();
    else if(mpiImpl == "nompi")
      return NULL;
    else{
      ostringstream oss;
      oss << mpiImpl << " : not yet implemented";
      throw EmulationException(oss.str().c_str());
    }
  }

  /**
   * This method generates a temporary file name with the pattern "<tmpdir>/<prefix>-XXXXXX" where
   * <tmpdir> is the directory for temporary files (see BatchManager_eClient::getTmpDir()) and the
   * X's are replaced by random characters. Note that this method is less secure than
   * BatchManager_eClient::createAndOpenTemporaryFile, so use the latter whenever possible.
   * \param prefix the prefix to use for the temporary file.
   * \return a name usable for a temporary file.
   */
  string BatchManager_eClient::generateTemporaryFileName(const string & prefix)
  {
    string fileName = getTmpDir() + "/" + prefix + "-XXXXXX";
    char randstr[7];

    do {
      sprintf(randstr, "%06d", rand() % 1000000);
      fileName.replace(fileName.size()-6, 6, randstr);
    } while (EXISTS(fileName.c_str()));

    return fileName;
  }

  /**
   * This method creates a temporary file and opens an output stream to write into this file.
   * The file is created with the pattern "<tmpdir>/<prefix>-XXXXXX" where <tmpdir> is the directory
   * for temporary files (see BatchManager_eClient::getTmpDir()) and the X's are replaced by random
   * characters. The caller is responsible for closing and deleting the file when it is no more used.
   * \param prefix the prefix to use for the temporary file.
   * \param outputStream an output stream that will be opened for writing in the temporary file. If
   * the stream is already open, it will be closed first.
   * \return the name of the created file.
   */
  string BatchManager_eClient::createAndOpenTemporaryFile(const string & prefix, ofstream & outputStream)
  {
    if (outputStream.is_open())
      outputStream.close();

#ifdef WIN32

    string fileName = generateTemporaryFileName(prefix);
    // Open the file as binary to avoid problems with Windows newlines
    outputStream.open(fileName.c_str(), ios_base::binary | ios_base::out);

#else

    string fileName = getTmpDir() + "/" + prefix + "-XXXXXX";
    char * buf = new char[fileName.size()+1];
    fileName.copy(buf, fileName.size());
    buf[fileName.size()] = '\0';

    int fd = mkstemp(buf);
    if (fd == -1) {
      delete[] buf;
      throw RunTimeException(string("Can't create temporary file ") + fileName);
    }
    fileName = buf;
    delete[] buf;

    outputStream.open(fileName.c_str());
    close(fd);  // Close the file descriptor so that the file is not opened twice

#endif

    if (outputStream.fail())
      throw RunTimeException(string("Can't open temporary file ") + fileName);

    return fileName;
  }

  /**
   * This method finds the name of the directory to use for temporary files in libBatch. This name
   * is <tempdir>/libBatch-<username>-XXXXXX. <tempdir> is found by looking for environment
   * variables TEMP, TMP, TEMPDIR, TMPDIR, and defaults to "/tmp" if none of them is defined.
   * <username> is found by looking for environment variables USER and USERNAME, and defaults to
   * "unknown". XXXXXX represents random characters. The directory name is generated only once for
   * each BatchManager_eClient instance, and the directory is created at this moment. Subsequent
   * calls will always return the same path and the existence of the directory will not be
   * rechecked.
   * \return the name of the directory to use for temporary files.
   */
  const std::string & BatchManager_eClient::getTmpDir()
  {
    if (tmpDirName.empty()) {
      const char * baseDir = getenv("TEMP");
      if (baseDir == NULL) baseDir = getenv("TMP");
      if (baseDir == NULL) baseDir = getenv("TEMPDIR");
      if (baseDir == NULL) baseDir = getenv("TMPDIR");
      if (baseDir == NULL) baseDir = "/tmp";

      const char * userName = getenv("USER");
      if (userName == NULL) userName = getenv("USERNAME");
      if (userName == NULL) userName = "unknown";

      string baseName = string(baseDir) + "/libBatch-" + userName + "-XXXXXX";
      srand(time(NULL));

#ifdef WIN32

      char randstr[7];
      do {
        sprintf(randstr, "%06d", rand() % 1000000);
        baseName.replace(baseName.size()-6, 6, randstr);
      } while (EXISTS(baseName.c_str()));
      if (_mkdir(baseName.c_str()) != 0)
        throw RunTimeException(string("Can't create temporary directory ") + baseName);
      tmpDirName = baseName;

#else

      char * buf = new char[baseName.size() + 1];
      baseName.copy(buf, baseName.size());
      buf[baseName.size()] = '\0';
      if (mkdtemp(buf) == NULL) {
        delete[] buf;
        throw RunTimeException(string("Can't create temporary directory ") + baseName);
      }
      tmpDirName = buf;
      delete[] buf;

#endif

    }

    return tmpDirName;
  }

}
