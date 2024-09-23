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

#ifndef LENSLongDetectorConstruction_h
#define LENSLongDetectorConstruction_h 1


#include "globals.hh"
#include "G4RotationMatrix.hh"
#include "G4ThreeVector.hh"
#include "G4VUserDetectorConstruction.hh"

class G4LogicalVolume;
class G4VPhysicalVolume;
class G4Material;
class G4MaterialPropertiesTable;
// For some reason it does not like this forward declaration, 
// but it does like the #include "G4RotationMatrix.hh".
// In G4RotationMatrix.hh it is a typdef, here it is a class. 
// From "G4RotationMatrix.hh" : typedef HepRotation G4RotationMatrix;
//class G4RotationMatrix;
class G4VisAttributes;
class G4VSensitiveDetector;

class LENSLongPhotomultiplierTube;
class LENSLongLiquidScintillator;
class LENSPixelatedScintillator;

class LENSLongDetectorConstruction : public G4VUserDetectorConstruction
{
public:
	LENSLongDetectorConstruction();
	~LENSLongDetectorConstruction();
	
	G4VPhysicalVolume* Construct();

// All of the Get...() functions
	G4LogicalVolume* GetWorldLogicalVolume() const { return m_pWorld_logVol; }
	void GetHallRotation( G4RotationMatrix& rotation ) const;
	
	LENSLongLiquidScintillator* GetLENSLongDetector( void ) { return m_LENSLongBoxScintillatorSD; }
	
	void GetBeamPipeLaunchCoordinateGlobal( G4ThreeVector& center ) const;
	void GetHitModuleCenterGlobal( G4ThreeVector& center ) const;
	
	const G4double GetCellX() const;
	const G4double GetCellY() const;
	const G4double GetCellZ() const;

	const G4double GetCellXOffset() const;
	const G4double GetCellYOffset() const;
	const G4double GetCellZOffset() const;

	const G4double GetFullDetectorWidth() const;
	const G4double GetFullDetectorHeight() const;
	const G4double GetFullDetectorDepth() const;
	
	/*G4double GetCellX() const;
	G4double GetCellY() const;
	G4double GetCellZ() const;

	G4double GetCellXOffset() const;
	G4double GetCellYOffset() const;
	G4double GetCellZOffset() const;

	G4double GetFullDetectorWidth() const;
	G4double GetFullDetectorHeight() const;
	G4double GetFullDetectorDepth() const;*/

	const G4double GetFloorThickness() const { return m_FloorThickness; }

	// G4double GetFloorThickness() const { return m_FloorThickness; }
	
	G4Material* GetVacuum( ) const { return m_Vacuum; }
	G4Material* GetAir( ) const { return m_Air; }
	G4Material* GetLead( ) const { return m_Pb; }
	G4Material* GetIron( ) const { return m_Fe; }
	G4Material* GetCopper( ) const { return m_Cu; }
	G4Material* GetMineralOil( ) const { return m_MineralOil; }
	G4Material* GetWater( ) const { return m_Water; }
	G4Material* GetPMTGlass( ) const { return m_PMTGlass; }
	G4Material* GetCarbonFiber( ) const { return m_CarbonFiber; }
	G4Material* GetSilicon( ) const { return m_Silicon; }
	G4Material* GetSiliconPutty( ) const { return m_SiliconPutty; }
	G4Material* GetCadmium( ) const { return m_Cd; }
	G4Material* GetHDPE( ) const { return m_HDPE; }
	G4Material* GetDelrin( ) const { return m_Delrin; }

	G4Material* GetPolyStyrene( ) const { return m_Polystyrene; }
	G4Material* GetVinyl( ) const { return m_Vinyl; }
	G4Material* GetNylon( ) const { return m_Nylon; }
	G4Material* GetTeflon( ) const { return m_Teflon; }
	G4Material* GetPTFETape( ) const { return m_PTFETape; }
	G4Material* GetBC408( ) const { return m_BC408Material; }
//	G4Material* GetScFreeScintillator( ) const { return m_IndiumFreeScintillatorMaterial; }
	G4Material* GetTin( ) const { return m_Tin; }
	G4Material* GetAluminium( ) const { return m_Aluminium; }
	G4Material* GetLowDensityAluminium( ) const { return m_LowDensityAluminium; }
	G4Material* GetSodiumIodide( ) const { return m_NaI; }
	G4Material* GetSodiumIodideWithThalium( ) const { return m_NaIWithTl; }
	G4Material* GetAluminiumOxide( ) const { return m_AluminiumOxide; }
	G4Material* GetTextolite( ) const { return m_Textolite; }
	G4Material* GetGermaniumCrystal( ) const { return m_GermaniumCrystal; }
	G4Material* GetBariumFlorideCrystal( ) const { return m_BariumFlorideCrystal; }
	G4Material* GetStainlessSteel( ) const { return m_StainlessSteel; }
	G4Material* GetPCB( ) const { return m_PCB; }
	G4Material* GetYSO( ) const { return m_YSO; }
// END Get...() functions

// All of the Set...() functions
	void SetFloorThickness( G4double newThickness ) { m_FloorThickness = newThickness; }

	// const G4double GetOpticalPhotonEmissionFraction() const;
	const G4double GetOpticalPhotonEmissionFraction() const;
	
// END Set...() functions

protected:
	void ConstructMaterials();
	void DestroyMaterials();
	
	void ConstructLENSLongHall();
	void ConstructLENSLongScintillator();
	
	void ConstructVisibilityAttributes();
	void DestroyVisibilityAttributes();

	void DumpGeometricalTree(G4VPhysicalVolume* aVolume,G4int depth=0); // Output Geometry heirarchy
	
// Materials
	G4Material* m_Vacuum;
	G4Material* m_CarbonFiber;
	G4Material* m_Silicon;
	G4Material* m_SiliconPutty;
	G4Material* m_Fe;
	G4Material* m_Cu;
	G4Material* m_Pb;
	G4Material* m_Cd;
	G4Material* m_HDPE;
  G4Material* m_Delrin;
	G4Material* m_Air;
	G4Material* m_Polystyrene;
	G4Material* m_MineralOil;
	G4Material* m_Water;
	G4Material* m_PMTGlass;
	G4Material* m_Concrete;
	G4Material* m_NaI;
	G4Material* m_NaIWithTl;
	G4Material* m_AluminiumOxide;
	G4Material* m_Textolite;
	G4Material* m_GermaniumCrystal;
	G4Material* m_BariumFlorideCrystal;
	G4Material* m_StainlessSteel;
	G4Material* m_PCB;
  G4Material* m_YSO;

	G4Material* m_Vinyl;
	G4Material* m_Nylon;
	G4Material* m_Teflon;
	G4Material* m_PTFETape;
	G4Material* m_BC408Material;
//	G4Material* m_IndiumFreeScintillatorMaterial;
	G4Material* m_Tin;
	G4Material* m_Aluminium;
  G4Material* m_LowDensityAluminium;
// Material properties for optical photons	
//	G4MaterialPropertiesTable* m_MineralOil_mt;
//	G4MaterialPropertiesTable* m_PMTGlass_mt;
//	G4MaterialPropertiesTable* m_Water_mt;

	G4MaterialPropertiesTable* m_Nylon_mt;
	G4MaterialPropertiesTable* m_Teflon_mt;
	G4MaterialPropertiesTable* m_BC408Material_mt;
//	G4MaterialPropertiesTable* m_IndiumFreeScintillatorMaterial_mt;

// World
	G4LogicalVolume* m_pWorld_logVol;
	G4VPhysicalVolume* m_pWorld_phyVol;

// LENSLongHall
	G4LogicalVolume* m_pLENSLongHall_logVol;
	G4VPhysicalVolume* m_pLENSLongHall_phyVol;
	G4RotationMatrix* m_LENSLongHallRotation;
	
// Floor
	G4LogicalVolume* m_pFloor_logVol;
	G4VPhysicalVolume* m_pFloor_phyVol;

	G4double m_FloorThickness;
	G4bool m_UseFloor;
		
// Liquid scintillator detector
	LENSLongLiquidScintillator* m_LENSLongBoxScintillatorSD;

// Visibility Attributes
//	G4VisAttributes* m_IronVisAtt;
	G4VisAttributes* m_ScintillatorVisAtt;
//	G4VisAttributes* m_PMTGlassVisAtt;
	G4VisAttributes* m_ConcreteVisAtt;
	G4VisAttributes* m_IndiumVisAtt;
	G4VisAttributes* m_TinVisAtt;
};

#endif // LENSLongDetectorConstruction_h

