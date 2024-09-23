//
// ********************************************************************
// * DISCLAIMER                                                       *
// *                                                                  *
// * The following disclaimer summarizes all the specific disclaimers *
// * of contributors to this software. The specific disclaimers,which *
// * govern, are listed with their locations in:                      *
// *   http://cern.ch/geant4/license                                  *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.                                                             *
// *                                                                  *
// * This  code  implementation is the  intellectual property  of the *
// * GEANT4 collaboration.                                            *
// * By copying,  distributing  or modifying the Program (or any work *
// * based  on  the Program)  you indicate  your  acceptance of  this *
// * statement, and all its terms.                                    *
// ********************************************************************
//
// Alternatively, you can implement an empty function here and just
//   register the systems you want in your main(), e.g.:
//   G4VisManager* myVisManager = new MyVisManager;
//   myVisManager -> RegisterGraphicsSystem (new MyGraphicsSystem);

#ifndef LENSLongVisualizationManager_h
#define LENSLongVisualizationManager_h 1


#include "G4VisManager.hh"

class G4LogicalVolume;
class G4VPhysicalVolume;
class G4Material;


class LENSLongVisualizationManager : public G4VisManager
{
public:
	LENSLongVisualizationManager();
//	~NuSNSVisualizationManager();
	G4bool Visualize(){ return m_Visualize; }

private:
  void RegisterGraphicsSystems ();
  
  G4bool m_Visualize;

};

#endif // LENSLongVisualizationManager_h

