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
//


#ifndef LENSLongLiquidScintillator_h
#define LENSLongLiquidScintillator_h 1

#include "G4RotationMatrix.hh"
#include "G4VSensitiveDetector.hh"
#include "LENSLongLiquidScintillatorHit.hh"

#include <vector>


// Forward declarations
class G4LogicalVolume;
class G4VPhysicalVolume;
class G4Step;
class G4HCofThisEvent;
class G4MaterialPropertiesTable;
class G4TouchableHistory;
class G4VisAttributes;


class LENSLongDetectorConstruction;

class LENSLongLiquidScintillator : public G4VSensitiveDetector
{

  public:
    LENSLongLiquidScintillator( G4String name );
    virtual ~LENSLongLiquidScintillator();
			
	void ConstructLiquidDetector( LENSLongDetectorConstruction* mainConstructor );

	void GetBeamPipeLaunchCoordinateGlobal( G4ThreeVector& center ) const { center = m_SourceLocation; }
	void GetHitModuleCenterGlobalCoord( G4ThreeVector& bunkerCenterG ) const;
	inline G4double GetSodiumIodideX() const { return m_SodiumIodideDimensions[0][0]; }// TEMP and BAD TEMP at that.
	inline G4double GetSodiumIodideY() const { return m_SodiumIodideDimensions[0][1]; }// TEMP and BAD TEMP at that.
	inline G4double GetSodiumIodideZ() const { return m_SodiumIodideDimensions[0][2]; }// TEMP and BAD TEMP at that.

	inline G4double GetDetectorWidth() const { return ( m_ModuleTotalDimensions[1][0] ); }// TEMP and BAD TEMP at that.
	inline G4double GetDetectorHeight() const { return ( 7 * m_ModuleTotalDimensions[1][1] ); }// TEMP and BAD TEMP at that.
	inline G4double GetDetectorDepth() const { return ( 7 * m_ModuleTotalDimensions[1][2] ); }// TEMP and BAD TEMP at that.
	
	inline G4double GetModuleX( const int index ) const { return ( m_ModuleTotalDimensions[index][0] ); }// TEMP and BAD TEMP at that.
	inline G4double GetModuleY( const int index ) const { return ( m_ModuleTotalDimensions[index][1] ); }// TEMP and BAD TEMP at that.
	inline G4double GetModuleZ( const int index ) const { return ( m_ModuleTotalDimensions[index][2] ); }// TEMP and BAD TEMP at that.
	
	inline G4double GetSourceRadius() const { return ( m_SourceRadius ); }
	inline G4bool IsSourceAlongFace() const { return m_SourceAlongFace; }
	
	const G4int GetModuleIndex( G4TouchableHistory* thePostTouchable ) const;
		
	G4double GetOpticalPhotonEmissionFraction() const { return m_PhotonEmissionFraction; }

// Inherited SD processing functions
    virtual void Initialize( G4HCofThisEvent*HCE );
    virtual G4bool ProcessHits( G4Step*aStep, G4TouchableHistory*ROhist );
    virtual void EndOfEvent( G4HCofThisEvent*HCE );
	
	G4bool ProcessOpticalPhotonStepHits( const G4Step* aStep, G4TouchableHistory* ROhist );
		
  protected:
// Ouput stuff or not...
		G4bool m_OutputInfo;
		G4bool m_TrackOpticalPhotons;
		G4bool m_UseRoughSurfaces;
		G4bool m_PerfectPlasticWalls;
		
		G4double m_PhotonEmissionFraction;

// Defined in LENSLongLiquidScintillatorHit.hh
    LENSLongLiquidScintillatorOpticalPhotonHitsCollection* m_OpticalPhotonHitsCollection;
    G4int m_OpticalPhotonHitsCollectionID;

// Definition of LENSLongLiquidScintillatorGammaHitsCollection is in LENSLongLiquidScintillatorHit.hh
    LENSLongLiquidScintillatorGammaHitsCollection* m_GammaHitsCollection;
    G4int m_GammaHitsCollectionID;
		
// Optical property table stuffs
		G4MaterialPropertiesTable* m_NaI_mt;
		G4MaterialPropertiesTable* m_Teflon_mt;
		G4MaterialPropertiesTable* m_PMT_mt;
		G4MaterialPropertiesTable* m_Air_mt;
		G4MaterialPropertiesTable* m_Glass_mt;

		G4VisAttributes* m_ReflectiveFoilVisAtt;
		G4VisAttributes* m_GasVisAtt;
		G4VisAttributes* m_CellWallVisAtt;
		G4VisAttributes* m_IndiumCellVisAtt;
		G4VisAttributes* m_PMTGlassVisAtt;
		G4VisAttributes* m_PMTHitGlassVisAtt;
		G4VisAttributes* m_WireHousingGlassVisAtt;
	
		std::vector<G4VisAttributes*> m_vVisAttributes;
		
		void makeBasicModules();
		
		void makeWall( const G4int pmtIndex, const G4int numberBars,
		const G4ThreeVector& centerPosition, const G4ThreeVector& moduleDirection, const G4ThreeVector& wallNormal );
		
		void makeAirAroundDetector();
		void makeTable();
		void makePipe();
		
	// These set up the basic module into the actual detectors...
		void makeDefaultSingleModule();

		void makeSingleFourLayer();
		void makeTwoFourLayer();
		void makeThreeFourLayer();

		void makeSingleEightLayer();
		void makeTwoEightLayer();

		void makeBEGeCube();
		void makeCloverCube();
		void makeSquareOrthocupola();
		void makeCloverSquareOrthocupola();
		void makeCloverRhombicuboctahedron();

		void makeGRIFFINCube();
		void makeGRIFFINSquareOrthocupola();
		void makeGRIFFINRhombicuboctahedron();

// These are the same for all, change to vectors (like below) if needed.
		G4double m_PMTLength;
		G4double m_PMTSmallLength;
		G4double m_Layer0Thickness;// Outside
		G4double m_Layer1Thickness;
		G4double m_Layer2Thickness;
		G4double m_Layer3Thickness;// touches NaI

		G4double m_Layer0InCenterHoleThickness;// Outside
		G4double m_Layer1InCenterHoleThickness;
		G4double m_Layer2InCenterHoleThickness;
		G4double m_Layer3InCenterHoleThickness;// touches NaI
		
		G4double m_GlassWindowThickness;
		G4double m_AluminumEndPieceThickness;

		G4bool m_UseSingleHoledCrystal;// If false calls makeMTASDefault, otherwise uses makeMTASSingleHoledCenter
		G4bool m_UseNeutronAbsorber;// Whether to use the Cd and HDPE around the beam pipe
		G4double m_CadmiumThickness;
		G4double m_HDPEThickness;

		G4double m_NaIHoleRadius;
		G4double m_PipeOuterRadius;
		G4double m_PipeThickness;
	
		G4double m_RollerRadius;
		G4double m_RollerLength;

		G4bool m_SingleModuleOnly;
		G4bool m_UsePipe;
		
		G4ThreeVector m_SourceLocation;
		G4double m_SourceRadius;
		G4bool m_SourceAlongFace;
		G4bool m_ShieldSource;
		
		std::vector<G4double> m_PMTRadii;// Fill this, the above variables, and the m_BC408Lengths for each module.
// Bar dimensions
		std::vector<G4ThreeVector> m_SodiumIodideDimensions;// Dimensions of SodiumIodide crystal
		std::vector<G4ThreeVector> m_ModuleTotalDimensions;// Dimensions of a bar with PMT and AluminimOxide inlcuded.
		std::vector<G4LogicalVolume*> m_ModuleTotal_LogicalVolumes;

		std::vector<G4LogicalVolume*> m_Gas_LogicalVolumes;
		std::vector<G4LogicalVolume*> m_Layer0_LogicalVolumes;// Starts from the outside layer and goes to...
		std::vector<G4LogicalVolume*> m_Layer1_LogicalVolumes;//...
		std::vector<G4LogicalVolume*> m_Layer2_LogicalVolumes;//...
		std::vector<G4LogicalVolume*> m_Layer3_LogicalVolumes;// The inside layer that touches the NaI
		std::vector<G4LogicalVolume*> m_NaI_LogicalVolumes;
		std::vector<G4LogicalVolume*> m_PMT_LogicalVolumes;
		std::vector<G4LogicalVolume*> m_Glass_LogicalVolumes;
		std::vector<G4LogicalVolume*> m_PMT_Shield_LogicalVolumes;
		std::vector<G4LogicalVolume*> m_Si_LogicalVolumes;
		
		std::vector<G4LogicalVolume*> m_Shield_LogicalVolumes;
		std::vector<G4LogicalVolume*> m_Cd_LogicalVolumes;
		std::vector<G4LogicalVolume*> m_HDPE_LogicalVolumes;
	
		std::vector<G4LogicalVolume*> m_Pipe_LogicalVolumes;
		std::vector<G4LogicalVolume*> m_Wire_LogicalVolumes;
		std::vector<G4LogicalVolume*> m_Roller_LogicalVolumes;
		
		std::vector<G4RotationMatrix*> m_Rotations;
		
		LENSLongDetectorConstruction* m_MainDetectorConstructor;
};




#endif // LENSLongLiquidScintillator_h

