#include "Parm_PDB.h"
#include "PDBfile.h"
#include "CpptrajStdio.h"

void Parm_PDB::ReadHelp() {
  mprintf("\tpqr:     Read atomic charge/radius from occupancy/B-factor columns (PQR).\n"
          "\treadbox: Read unit cell information from CRYST1 record if present.\n");
}

int Parm_PDB::processReadArgs(ArgList& argIn) {
  readAsPQR_ = argIn.hasKey("pqr");
  readBox_ = argIn.hasKey("readbox");
  return 0;
} 

int Parm_PDB::ReadParm(std::string const& fname, Topology &TopIn) {
  PDBfile infile;
  double XYZ[6]; // Hold XYZ/box coords.
  float occupancy, bfactor; // Read in occ/bfac
  std::vector<AtomExtra> extra; // Hold occ/bfac if not PQR
  BondArray bonds;              // Hold bonds
  int barray[5];                // Hold CONECT atom and bonds
  char altLoc = ' ';            // For reading in altLoc.
  if (infile.OpenRead(fname)) return 1;
  // Loop over PDB records
  while ( infile.NextRecord() != PDBfile::END_OF_FILE ) {
    if (readBox_ && infile.RecType() == PDBfile::CRYST1) {
      // Box info from CRYST1 record.
      infile.pdb_Box( XYZ );
      TopIn.SetParmBox( XYZ );
    } else if (infile.RecType() == PDBfile::CONECT) {
      // BOND - first element will be atom, next few are bonded atoms.
      // To avoid duplicates only add the bond if atom2 > atom1
      int nscan = infile.pdb_Bonds(barray);
      if (nscan > 1) {
        for (int i = 1; i != nscan; i++)
          if (barray[i] > barray[0])
            bonds.push_back( BondType(barray[0] - 1, barray[i] - 1, -1) );
      }
    } else if (infile.RecType() == PDBfile::ATOM) {
      // If this is an ATOM / HETATM keyword, add to topology.
      infile.pdb_XYZ( XYZ );
      Atom pdbAtom = infile.pdb_Atom(altLoc);
      infile.pdb_OccupanyAndBfactor(occupancy, bfactor);
      if (readAsPQR_) {
        pdbAtom.SetCharge( occupancy );
        pdbAtom.SetGBradius( bfactor );
      } else
        extra.push_back( AtomExtra(occupancy, bfactor, altLoc) );
      TopIn.AddTopAtom(pdbAtom, infile.pdb_Residue(), XYZ);
    } else if ( infile.RecType() == PDBfile::TER || 
                infile.RecType() == PDBfile::END )
    {
      // Indicate end of molecule for TER/END. Finish if END.
      TopIn.StartNewMol();
      if (infile.RecType() == PDBfile::END) break;
    }
  }
  // Add bonds
  //for (BondArray::const_iterator bnd = bonds.begin(); bnd != bonds.end(); ++bnd)
  //  TopIn.AddBond( bnd->A1(), bnd->A2() ); 
  if (TopIn.SetExtraAtomInfo(0, extra)) return 1;
  // If Topology name not set with TITLE etc, use base filename.
  // TODO: Read in title.
  std::string pdbtitle;
  TopIn.SetParmName( pdbtitle, infile.Filename() );

  infile.CloseFile();
  return 0;
}

bool Parm_PDB::ID_ParmFormat(CpptrajFile& fileIn) {
  return PDBfile::ID_PDB( fileIn );
}
