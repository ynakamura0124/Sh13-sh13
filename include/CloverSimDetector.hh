#ifndef CLOVER_SIM_DETECTOR_HH
#define CLOVER_SIM_DETECTOR_HH

#include "LENSLongLiquidScintillator.hh"
#include "LENSLongDetectorConstruction.hh"
#include "CloverSimDetectorConstruction.hh"
#include "G4VisAttributes.hh"

class CloverSimDetector : public LENSLongLiquidScintillator {
public:
	CloverSimDetector( G4String name ) : LENSLongLiquidScintillator( name ) {};
	virtual ~CloverSimDetector();

	void ConstructDetector(LENSLongDetectorConstruction* mainDetectorConstructor);
protected:
	G4VisAttributes* yso_vis_att_ = nullptr;
	G4VisAttributes* si_vis_att_ = nullptr;
	G4VisAttributes* board_vis_att_ = nullptr;

};

#endif