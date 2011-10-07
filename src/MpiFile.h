#ifndef INC_MPIFILE_H
#define INC_MPIFILE_H
// MPI file IO
#include "BaseFileIO.h" 
#include "MpiRoutines.h"

class MpiFile : public BaseFileIO {
    parallelType pfile; 
  public:
    MpiFile(); 
    ~MpiFile(); 

    int Open(const char *, const char *);    
    int Close();
    int Read(void *, size_t, size_t );
    int Write(void *, size_t, size_t);  
    int Seek(off_t);
    int Rewind();  
    //long int Tell();  // NOTE: Tell may be unnecessary if only for size reporting.
    int Gets(char *, int );
    int SetSize(long int);
};
#endif
