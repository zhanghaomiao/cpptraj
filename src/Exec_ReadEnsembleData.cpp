#include "Exec_ReadEnsembleData.h"
#include "CpptrajStdio.h"

// Exec_ReadEnsembleData::Help()
void Exec_ReadEnsembleData::Help() const
{
  mprintf("<filename> [filenames <additional files]\n");
}

// Exec_ReadEnsembleData::Execute()
Exec::RetType Exec_ReadEnsembleData::Execute(CpptrajState& State, ArgList& argIn)
{
  std::string additionalNames = argIn.GetStringKey("filenames");
  FileName fname( argIn.GetStringNext() );
  if (fname.empty()) {
    mprinterr("Error: No file name given.\n");
    return CpptrajState::ERR;
  }

  File::NameArray fileNames;
  if (!additionalNames.empty()) {
    ArgList name_list(additionalNames, ",");
    if (name_list.Nargs() < 1) {
      mprinterr("Error: No additional names specified.\n");
      return CpptrajState::ERR;
    }
    fileNames.push_back( fname );
    for (int arg = 0; arg < name_list.Nargs(); arg++)
      fileNames.push_back( name_list[arg] );
  } else {
    fileNames = File::SearchForReplicas( fname, State.Debug() );
  }
  mprintf("\t%zu files.\n", fileNames.size());
  for (File::NameArray::const_iterator it = fileNames.begin(); it != fileNames.end(); ++it)
    mprintf("\t  %s\n", it->full());


  // Execute a data read on all files.
  unsigned int min_file = 0;
  unsigned int max_file = fileNames.size();

  int err = 0;
  for (unsigned int nfile = min_file; nfile < max_file; nfile++)
  {
    DataFile dataIn;
    dataIn.SetDebug( State.DFL().Debug() );
    // TODO ExpandToFilenames?
    if (dataIn.ReadDataIn( fileNames[nfile], argIn, State.DSL(), nfile, max_file ) != 0)
    {
      mprinterr("Error: Could not read data file '%s'\n", fileNames[nfile].full());
      err = 1;
      break;
    }
  }
  if (err != 0) return CpptrajState::ERR;
  return CpptrajState::OK;
}
