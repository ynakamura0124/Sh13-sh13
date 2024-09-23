#ifndef SH13_SIM_DETECTOR_CONSTRUCTION_HH
#define SH13_SIM_DETECTOR_CONSTRUCTION_HH

#include "LENSLongDetectorConstruction.hh"
#include "Sh13DetectorConstruction.hh"

class Sh13DetectorConstruction : public LENSLongDetectorConstruction
{
public:
	Sh13DetectorConstruction() {}
	~Sh13DetectorConstruction() {}

	G4VPhysicalVolume *Construct();
	void ConstructDetector();
	void ConstructMaterials();
	G4Material *GetPLA() const { return m_PLA; }

protected:
	// materials
	G4Material *m_PLA;
	G4Material *Ge;
	G4Material *Vacuum;
	G4Material *Eu2;
	G4Material *Ba3;
};

#endif