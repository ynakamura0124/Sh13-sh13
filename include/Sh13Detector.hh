#ifndef CLOVER_SIM_DETECTOR_HH
#define CLOVER_SIM_DETECTOR_HH

#include "LENSLongLiquidScintillator.hh"
#include "LENSLongDetectorConstruction.hh"
#include "Sh13DetectorConstruction.hh"
#include "G4VisAttributes.hh"

class Sh13Detector : public LENSLongLiquidScintillator {
public:
	Sh13Detector( G4String name ) : LENSLongLiquidScintillator( name ) {};
	virtual ~Sh13Detector();

	void ConstructDetector(LENSLongDetectorConstruction* mainDetectorConstructor);
protected:
	G4VisAttributes* yso_vis_att_ = nullptr;
	G4VisAttributes* si_vis_att_ = nullptr;
	G4VisAttributes* board_vis_att_ = nullptr;

};

#endif