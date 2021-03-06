/*
 * This file is part of the OpenFSI package.
 *
 * OpenFSI is free package: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Copyright 2019 Huilin Ye University of Connecticut
 *
 * Author: Huilin Ye (huilin.ye@uconn.edu)
 * Note: This file is written based on lattice decomposition from LIGGGHTS parallelization
 */

#ifndef LATTICE_DECOMPOSITION2D_HH
#define LATTICE_DECOMPOSITION2D_HH

 
// LAMMPS
#include "comm.h"
#include "domain.h"


namespace plb {

LatticeDecomposition2D::LatticeDecomposition2D(plb::plint nx_, plb::plint ny_, plb::plint nz_,LAMMPS_NS::LAMMPS *lmp_)
  : nx(nx_),ny(ny_),lmp(*lmp_),
    blockStructure(0),threadAttribution(0)
{
  // Obtain the domain decomposition information from LAMMPS
  
  npx = lmp.comm->procgrid[0];
  npy = lmp.comm->procgrid[1];
  npz = lmp.comm->procgrid[2];

  for(plint i=0;i<=npx;i++)
    xVal.push_back(round( lmp.comm->xsplit[i]*(double)nx ));
  for(plint i=0;i<=npy;i++)
    yVal.push_back(round( lmp.comm->ysplit[i]*(double)ny ));
  for(plint i=0;i<=npz;i++){
    zVal.push_back(round( lmp.comm->zsplit[i]*(double)nz ));
  }

  // Assign the block boundary coordinate
  
  blockStructure = new SparseBlockStructure2D(Box2D(xVal[0], xVal.back()-1, 
                                                    yVal[0], yVal.back()-1) 
                                                    );
    
  threadAttribution = new ExplicitThreadAttribution();

  for (plint iX=0; iX<xVal.size()-1; ++iX) {
    for (plint iY=0; iY<yVal.size()-1; ++iY) {
      for (plint iZ=0; iZ<zVal.size()-1; ++iZ) {
        plint id = blockStructure->nextIncrementalId();
        blockStructure->addBlock (Box2D( xVal[iX], xVal[iX+1]-1, yVal[iY],
                                         yVal[iY+1]-1),
                                  id);
        threadAttribution->addBlock(id,(plint)lmp.comm->grid2proc[iX][iY][iZ]); // Assign the threadID and corresponding blocks
      }
    }
  }
    
}
  
LatticeDecomposition2D::~LatticeDecomposition2D()
{
  if(blockStructure != 0) delete blockStructure;
  if(threadAttribution != 0) delete threadAttribution;
}


SparseBlockStructure2D LatticeDecomposition2D::getBlockDistribution()
{
  return SparseBlockStructure2D(*blockStructure);
}
ExplicitThreadAttribution* LatticeDecomposition2D::getThreadAttribution()
{
  return new ExplicitThreadAttribution(*threadAttribution);
}

};

#endif // LATTICE_DECOMPOSITION2D_HH
