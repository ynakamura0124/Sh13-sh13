#ifndef CLOVER_SIM_DETECTOR_CONSTRUCTION_HH
#define CLOVER_SIM_DETECTOR_CONSTRUCTION_HH

#include "LENSLongDetectorConstruction.hh"
#include "CloverSimDetector.hh"

class CloverSimDetectorConstruction : public LENSLongDetectorConstruction {
public:
	CloverSimDetectorConstruction() {}
	~CloverSimDetectorConstruction() {}

	G4VPhysicalVolume* Construct();
	void ConstructDetector();
	void ConstructMaterials();
	G4Material* GetPLA() const { return m_PLA; }

protected:
	// materials
	G4Material* m_PLA;
};

#endif