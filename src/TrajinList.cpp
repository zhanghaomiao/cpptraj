// TrajinList
#include <cstddef> //NULL
#include "TrajinList.h"
//#include "RemdTraj.h"
#include "CpptrajStdio.h"
//#include "PtrajMpi.h" // worldrank,worldsize

// CONSTRUCTOR
TrajinList::TrajinList() {
  fileAccess=READ;
}

// DESTRUCTOR
TrajinList::~TrajinList() { }

/* TrajinList::Add()
 * Add trajectory to the trajectory list as an input trajectory. 
 * Associate the trajectory with one of the parm files in the 
 * ParmFileList. 
 * trajin <filename> [start] [stop] [offset] [parm <parmfile> | parmindex <#>
 *        remdtraj remdtrajtemp <T>]
 */
int TrajinList::Add(char *filename, ArgList *A, AmberParm *parmIn) {
  TrajectoryFile *traj;

  traj = new TrajectoryFile(); 
  if (traj==NULL) {
    mprinterr("Error: TrajinList::Add: Could not allocate memory for traj.\n");
    return 1;
  }
  traj->SetDebug(debug);
  if ( traj->SetupRead(filename,A,parmIn) ) {
    mprinterr("Error: trajin: Could not set up trajectory.\n");
    delete traj;
    return 1;
  }

  // Add to trajectory file list
  this->push_back(traj);

  return 0;
}

/* TrajinList::SetupFrames()
 * Only called for input trajectories.
 * Loop over all trajectories and call their setup frames routine to calc
 * actual start and stop and how many frames total will be processed.
 * Return the number of frames to be processed.
 */
int TrajinList::SetupFrames() {
  std::list<TrajectoryFile*>::iterator traj;
  int maxFrames, trajFrames;

  maxFrames=0;

  for (traj = this->begin(); traj != this->end(); traj++) {
    trajFrames = (*traj)->SetupFrameInfo();
    if (trajFrames==-1) {
      maxFrames=-1;
    }
    if (maxFrames>=0)
      maxFrames+=trajFrames;
    (*traj)->PrintInfo(1);
  }

  return maxFrames;
}

