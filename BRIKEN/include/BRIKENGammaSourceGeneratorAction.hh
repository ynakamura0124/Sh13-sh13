// BRIKENGammaSourceGeneratorAction.hh generated by R. Yokoyama on 1/29/2020
#ifndef BRIKEN_GAMMA_SOURCE_GENERATOR_ACTION_HH
#define BRIKEN_GAMMA_SOURCE_GENERATOR_ACTION_HH

#include "ImplantedGammaSourceGeneratorAction.hh"
#include "data2Tree.hpp"
#include <TRandom3.h>
#include <TROOT.h>

template class ImplantedGammaSourceGeneratorAction<impData2TTree>;

class BRIKENGammaSourceGeneratorAction : public ImplantedGammaSourceGeneratorAction<impData2TTree> {
public:
	BRIKENGammaSourceGeneratorAction();
	virtual ~BRIKENGammaSourceGeneratorAction();

	static const double GetYSOPosZ() { return -5.0 * CLHEP::mm; }
	static const std::vector<G4double> GetWAS3ABiPosZVec() {
		const G4double dist = 1.675 * CLHEP::cm;
		std::vector<G4double> vec = {
			dist - 0.5 * CLHEP::mm,
			dist + (4.5 - 0.5) * CLHEP::mm,
			dist + (6.3 + 0.5) * CLHEP::mm,
			dist + (10.8 + 0.5) * CLHEP::mm
		};
		return vec;
	};
	static const double GetYSOPosXY(const double &x) {
		return 24.0 * ( (x-0.5)/0.3) * CLHEP::mm;
		//return 48.0 * (x / 16.0 - 0.5) * CLHEP::mm;
	}
	static const double GetWAS3ABiPosXY(const double& x) {
		return 46.5 * (x / 16. - 0.5) * CLHEP::mm;
	}

	void SetReader() override;
	const G4ThreeVector ReadPosition() override;

protected:
	bool Next();
	static G4ThreeVector default_pos_;
	TRandom3 rand3;
	static const double GetYSOThickness(){ return 5.0 * CLHEP::mm; }
	static const double GetWAS3ABiThickness(){ return 1.0 * CLHEP::mm; }
};

G4ThreeVector BRIKENGammaSourceGeneratorAction::default_pos_;
#endif
