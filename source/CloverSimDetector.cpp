#include "CloverSimDetector.hh"

#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4SubtractionSolid.hh"
#include "G4PVPlacement.hh"
#include "G4ThreeVector.hh"
#include "G4RotationMatrix.hh"

#include "YamlReader.hpp"

CloverSimDetector::~CloverSimDetector() {
	if (yso_vis_att_)
		delete yso_vis_att_;
	if (si_vis_att_)
		delete si_vis_att_;
	if (board_vis_att_)
		delete board_vis_att_;
}

void CloverSimDetector::ConstructDetector(LENSLongDetectorConstruction* mainDetectorConstruction) {

	/* call LENSLongLiquidScintillator::ConstructLiquidDetector */
	LENSLongLiquidScintillator::ConstructLiquidDetector(mainDetectorConstruction);

	if (!yso_vis_att_) {
		yso_vis_att_ = new G4VisAttributes(G4Colour(0.7,0.0,0.2,0.5));
		yso_vis_att_->SetForceSolid(true);
	}
	if (!si_vis_att_) {
		si_vis_att_ = new G4VisAttributes(G4Colour(0.2,0.0,0.8,0.5));
		si_vis_att_->SetForceSolid(true);
	}
	if (!board_vis_att_) {
		board_vis_att_ = new G4VisAttributes(G4Colour(0.7,0.7,0.0,0.5));
		//board_vis_att_->SetForceSolid(true);
	}

	/* placement of the other clover */
	{
		/* defines the rotation of the clover (180 deg around the beam axis) */
		G4RotationMatrix* rot = new G4RotationMatrix();
		rot->rotateX(180. * CLHEP::deg);

		/* set placement (positive x) */
		const G4double dist = 3.5 * CLHEP::cm;
		const G4double clv_offset = 0.07 * dist;
		G4ThreeVector placement( 2.25 * CLHEP::cm, 0.0 * CLHEP::cm, dist + 0.5 * (297.0) * CLHEP::mm + clv_offset );

		/* adjust distance between clovers */
		for (size_t i = 0; i < m_Gas_LogicalVolumes[0]->GetNoDaughters(); ++i) {
			auto pvp = m_Gas_LogicalVolumes[0]->GetDaughter(i);
			if(pvp->GetName() == "SingleModule_Volume")
				pvp->SetTranslation(G4ThreeVector( 2.25 * CLHEP::cm, 0.0 * CLHEP::cm, -(dist + 0.5 * (297.0) * CLHEP::mm + clv_offset )));
		}
		/* find a G4LogicalVolume whose name is "SingleModule_Volume" (the first clover) */
		/* and place another one on the other side */
		auto it = std::find_if(
			m_ModuleTotal_LogicalVolumes.begin(),
			m_ModuleTotal_LogicalVolumes.end(),
			[](G4LogicalVolume* const &x)->bool {return x->GetName()=="Al_Can_Logical"; }
		);
		if (it != m_ModuleTotal_LogicalVolumes.end()) {
			new G4PVPlacement(rot, placement, m_ModuleTotal_LogicalVolumes[1], "SingleModule_Volume2", m_Gas_LogicalVolumes[0], 0, 1);
		}
	}

	auto half = [](const G4double& x) { return 0.5 * x; };
	/* modify YSO placement */
	{
		const G4double offset = -0.5 * CLHEP::cm;
		/* change the VisAtt of YSO*/
		auto it = std::find_if(
			m_ModuleTotal_LogicalVolumes.begin(),
			m_ModuleTotal_LogicalVolumes.end(),
			[](G4LogicalVolume* const &x)->bool {return x->GetName() == "YSO_Logical"; }
		);
		if (it != m_ModuleTotal_LogicalVolumes.end())
			(*it)->SetVisAttributes(yso_vis_att_); 

		/* change the placement position of "YSO_Volume" */
		//const G4double dist = 3.5 * CLHEP::cm;
		for (size_t i = 0; i < m_Gas_LogicalVolumes[0]->GetNoDaughters(); ++i) {
			auto pvp = m_Gas_LogicalVolumes[0]->GetDaughter(i);
			if(pvp->GetName() == "YSO_Volume")
				//pvp->SetTranslation(G4ThreeVector(-dist, 0.0 * CLHEP::cm, 0.0 * CLHEP::cm));
				pvp->SetTranslation(G4ThreeVector(offset, 0.0 * CLHEP::cm, 0.0 * CLHEP::cm));
		}

		/* placement of the light guide */
		auto lgMaterial = mainDetectorConstruction->GetPMTGlass();

		const G4double lgX = 2.0 * CLHEP::mm;
		const G4double lgY = 50.0 * CLHEP::mm;
		const G4double lgZ = 50.0 * CLHEP::mm;

		G4VSolid* lgSolid = new G4Box("LG_Solid", half(lgX), half(lgY), half(lgZ));
		G4LogicalVolume* lg_LV = new G4LogicalVolume(lgSolid, lgMaterial, "LG_Logical");
		m_ModuleTotal_LogicalVolumes.push_back(lg_LV);
		new G4PVPlacement(nullptr, G4ThreeVector(-3.5 * CLHEP::mm + offset, 0., 0.), lg_LV, "LG_Volume", m_Gas_LogicalVolumes[0], 0, 1);

		/* placement of the box */
		auto boxMaterial = ((CloverSimDetectorConstruction*)mainDetectorConstruction)->GetPLA();
		const G4double boxX = 50.0 * CLHEP::mm;
		const G4double boxTh = 2.5 * CLHEP::mm;
		const G4double boxZ = 2.5 * CLHEP::mm;
		G4VSolid* boxSolid = new G4Box("Box_Solid", half(boxX), half(lgY)+boxTh, half(lgZ)+boxTh);
		G4VSolid* boxHoleSolid = new G4Box("BoxHole_Solid", half(boxX)+boxZ, half(lgY), half(lgZ));
		G4VSolid* boxSubSolid = new G4SubtractionSolid("BoxSubtracted_Solid", boxSolid, boxHoleSolid);
		G4LogicalVolume* box_LV = new G4LogicalVolume(boxSubSolid, boxMaterial, "Box_Logical");
		m_ModuleTotal_LogicalVolumes.push_back(box_LV);
		new G4PVPlacement(nullptr, G4ThreeVector(-half(boxX) + boxZ + boxZ + offset, 0, 0), box_LV,"Box_Volume",m_Gas_LogicalVolumes[0], false, 0);

		/* placement of the box base */
		const G4double baseY = 2.5 * CLHEP::cm;
		G4VSolid* baseSolid = new G4Box("BoxBase_Solid", half(boxX), half(baseY), half(lgZ)+boxTh);
		G4VSolid* baseHoleSolid = new G4Box("BoxBaseHole_Solid", half(boxX)-boxTh, half(baseY)+boxTh, half(lgZ));
		G4VSolid* baseSubSolid = new G4SubtractionSolid("BoxBaseSub_Solid", baseSolid, baseHoleSolid);
		G4LogicalVolume* boxBase_LV = new G4LogicalVolume(baseSubSolid, boxMaterial, "Base_Logical");
		m_ModuleTotal_LogicalVolumes.push_back(boxBase_LV);
		new G4PVPlacement(nullptr, G4ThreeVector(-half(boxX) + boxZ + boxZ + offset, -half(baseY)-boxTh-half(lgY)-boxTh, 0), boxBase_LV,"BoxBase_Volume",m_Gas_LogicalVolumes[0], false, 0);
		G4VSolid* baseTopSolid = new G4Box("BoxBaseTop_Solid", half(boxX), half(boxTh), half(lgZ)+boxTh);
		G4LogicalVolume* boxBaseTop_LV = new G4LogicalVolume(baseTopSolid, boxMaterial, "BaseTop_Logical");
		m_ModuleTotal_LogicalVolumes.push_back(boxBaseTop_LV);
		new G4PVPlacement(nullptr, G4ThreeVector(-half(boxX) + boxZ + boxZ + offset, -half(lgY)-half(boxTh)-boxTh, 0), boxBaseTop_LV,"BoxBaseTop_Volume",m_Gas_LogicalVolumes[0], false, 0);
	}

	/* placement of WAS3ABi */
	const G4double dist = 1.675 * CLHEP::cm;
	/* placement of WAS3ABi Si */
	{
		auto* siMaterial = mainDetectorConstruction->GetSilicon();
		
		const G4double siX = 1.0 * CLHEP::mm;
		const G4double siY = 56.0 * CLHEP::mm;
		const G4double siZ = 56.0 * CLHEP::mm;

		G4VSolid* siSolid = new G4Box("Si_Solid", half(siX), half(siY), half(siZ));
		G4LogicalVolume* siLV = new G4LogicalVolume(siSolid, siMaterial, "Si_Logical");
		siLV->SetVisAttributes(si_vis_att_);
		m_ModuleTotal_LogicalVolumes.push_back(siLV);

		const std::vector<G4double> dist_vec = {
			dist - half(siX), // 4th layer
			dist + 4.5 * CLHEP::mm - half(siX), // 3rd layer
			dist + 6.3 * CLHEP::mm + half(siX), // 2nd layer
			dist + 10.8 * CLHEP::mm + half(siX), // 1st layer
		};
		for (const auto d : dist_vec) {
			G4ThreeVector placement(d + half(siX), 0.0 * CLHEP::cm, 0.0 * CLHEP::cm);
			new G4PVPlacement(0, placement, siLV, "Si_Volume", m_Gas_LogicalVolumes[0], 0, 1);
		}
	}
	/* placement of WAS3ABi boards */
	{
		auto* boardMaterial = mainDetectorConstruction->GetCarbonFiber();
		auto* alMaterial = mainDetectorConstruction->GetAluminium();

		const G4double boardX = 3.1 * CLHEP::mm;
		const G4double boardY = 71.1 * CLHEP::mm;
		const G4double boardZ = 61.6 * CLHEP::mm;
		const G4double siholeZ = 56 * CLHEP::mm;

		const G4double alX = 1.0 * CLHEP::mm;
		const G4double alY = 93.5 * CLHEP::mm;
		const G4double alZ = 65.5 * CLHEP::mm;
		const G4double holeY = 28.0 * CLHEP::mm;
		const G4double holeZ = 49.5 * CLHEP::mm;

		G4VSolid* boardSolid = new G4Box("Board_Solid", half(boardX), half(boardY), half(boardZ));
		G4VSolid* siholeSolid = new G4Box("Si_Hole_Solid", half(boardX)+1.0*CLHEP::mm, half(siholeZ), half(siholeZ));
		G4VSolid* boardSubSolid = new G4SubtractionSolid("Board_Sub_Solid1",boardSolid, siholeSolid, nullptr, G4ThreeVector(0.0,3.425,0.0));
		G4LogicalVolume* boardLV = new G4LogicalVolume(boardSubSolid, boardMaterial, "Board_Logical");
		boardLV->SetVisAttributes(board_vis_att_);
		m_ModuleTotal_LogicalVolumes.push_back(boardLV);

		G4VSolid* alFrameSolid = new G4Box("AlFrame_Solid", half(alX), half(alY), half(alZ));
		G4VSolid* holeSolid = new G4Box("AlFrame_Hole_Solid", half(alX)+1.0*CLHEP::mm, half(holeY), half(holeZ));
		G4VSolid* alFrameSubSolid1 = new G4SubtractionSolid("AlFrame_Sub_Solid1",alFrameSolid, boardSolid);
		G4VSolid* alFrameSubSolid2 = new G4SubtractionSolid("AlFrame_Sub_Solid2",alFrameSubSolid1, holeSolid, nullptr, G4ThreeVector(0.,half(alY),0.0));
		G4VSolid* alFrameSubSolid3 = new G4SubtractionSolid("AlFrame_Sub_Solid3",alFrameSubSolid2, holeSolid, nullptr, G4ThreeVector(0.,-half(alY),0.0));
		G4LogicalVolume* alFrameLV = new G4LogicalVolume(alFrameSubSolid3, alMaterial, "AlFrame_Logical");
		m_ModuleTotal_LogicalVolumes.push_back(alFrameLV);

		const std::vector<G4double> dist_vec = {
			dist - boardX + half(alX), // 4th layer
			dist + 4.5 * CLHEP::mm - boardX + half(alX), // 3rd layer
			dist + 6.3 * CLHEP::mm + half(boardX) - alX, // 2nd layer
			dist + 10.8 * CLHEP::mm + half(boardX) - alX, // 1st layer
		};
		for (const auto d : dist_vec) {
			G4ThreeVector placement(d + half(boardX), 4.0-half(14.85) * CLHEP::mm, 0.0 * CLHEP::mm);
			new G4PVPlacement(0, placement, boardLV, "Bord_Volume", m_Gas_LogicalVolumes[0], 0, 1);
		}
		const std::vector<G4double> al_dist_vec = {
			dist - half(alX), // 4th layer
			dist + 4.5 * CLHEP::mm - half(alX), // 3rd layer
			dist + 6.3 * CLHEP::mm + half(alX), // 2nd layer
			dist + 10.8 * CLHEP::mm + half(alX), // 1st layer
		};
		for (const auto d : al_dist_vec) {
			G4ThreeVector placement(d + half(alX), 0.0 * CLHEP::mm, 0.0 * CLHEP::mm);
			new G4PVPlacement(0, placement, alFrameLV, "AlFrame_Volume", m_Gas_LogicalVolumes[0], 0, 1);
		}
	}
	/* placement of WAS3ABi frames */
	{
		auto* alMaterial = mainDetectorConstruction->GetAluminium();

		/* alminum rods */
		const G4double rodR = 3.0 / 2.0 * CLHEP::mm;
		const G4double rodL = 58.0 * CLHEP::mm;
		const G4double rodX = dist - 12. * CLHEP::mm;
		const G4double rodY = 85.5 * CLHEP::mm;
		const G4double rodZ = 57.5 * CLHEP::mm;
		const std::vector<G4ThreeVector> pos_vec = {
			G4ThreeVector(half(rodL)+rodX,half(rodY),half(rodZ)),
			G4ThreeVector(half(rodL)+rodX,-half(rodY),half(rodZ)),
			G4ThreeVector(half(rodL)+rodX,half(rodY),-half(rodZ)),
			G4ThreeVector(half(rodL)+rodX,-half(rodY),-half(rodZ))
		};
		G4RotationMatrix *rot = new G4RotationMatrix();
		rot->rotateY(90. * CLHEP::deg);

		G4VSolid* rodSolid = new G4Tubs("Rod_Solid", 0, rodR, half(rodL), 0, 360 * CLHEP::deg);
		G4LogicalVolume* rodLV = new G4LogicalVolume(rodSolid, alMaterial, "Rod_Logical");
		m_ModuleTotal_LogicalVolumes.push_back(rodLV);
		
		for (auto pos : pos_vec) {
			new G4PVPlacement(rot, pos, rodLV, "Rod_Volume", m_Gas_LogicalVolumes[0], 0, 1);
		}
		/* alminum extra frames */
		const G4double alX = 5.0 * CLHEP::mm;
		const G4double alY = 105.0 * CLHEP::mm;
		const G4double alZ = 65.5 * CLHEP::mm;
		const G4double cHole = 56.0 * CLHEP::mm;
		const G4double tbHoleY = 33.0 * CLHEP::mm;
		const G4double tbHoleZ = 50.0 * CLHEP::mm;
		const G4double holeR = 3.1 / 2.0 * CLHEP::mm;
		const G4double hY = 63.0 * CLHEP::mm;
		const G4double hZ = 40.0 * CLHEP::mm;

		G4VSolid* plateSolid = new G4Box("Plate_Solid", half(alX), half(alY), half(alZ));
		G4VSolid* centerHoleSolid = new G4Box("CenterHole_Solid", half(alX)+1.0*CLHEP::mm, half(cHole), half(cHole));
		G4VSolid* rodholeSolid = new G4Tubs("RodHole_Solid", 0, holeR, half(alX)+1.0*CLHEP::mm, 0, 360*CLHEP::deg);
		G4VSolid* tbHoleSolid = new G4Box("TBHole_Solid", half(alX)+1.0*CLHEP::mm, half(tbHoleY), half(tbHoleZ));
		G4VSolid* subSolid1 = (G4VSolid*)new G4SubtractionSolid("Sub_Solid1", plateSolid, centerHoleSolid);
		G4VSolid* subSolid2 = (G4VSolid*)new G4SubtractionSolid("Sub_Solid2", subSolid1, tbHoleSolid, nullptr, G4ThreeVector(0.0,half(alY),0.0));
		G4VSolid* subSolid3 = (G4VSolid*)new G4SubtractionSolid("Sub_Solid3", subSolid2, tbHoleSolid, nullptr, G4ThreeVector(0.0,-half(alY),0.0));
		G4VSolid* subSolid4 = (G4VSolid*)new G4SubtractionSolid("Sub_Solid4", subSolid3, rodholeSolid, rot, G4ThreeVector(0.0,half(hY),half(hZ)));
		G4VSolid* subSolid5 = (G4VSolid*)new G4SubtractionSolid("Sub_Solid5", subSolid4, rodholeSolid, rot, G4ThreeVector(0.0,-half(hY),half(hZ)));
		G4VSolid* subSolid6 = (G4VSolid*)new G4SubtractionSolid("Sub_Solid6", subSolid5, rodholeSolid, rot, G4ThreeVector(0.0,half(hY),-half(hZ)));
		G4VSolid* subSolid7 = (G4VSolid*)new G4SubtractionSolid("Sub_Solid7", subSolid6, rodholeSolid, rot, G4ThreeVector(0.0,-half(hY),-half(hZ)));
		G4VSolid* subSolid8 = (G4VSolid*)new G4SubtractionSolid("Sub_Solid8", subSolid7, rodholeSolid, rot, G4ThreeVector(0.0,half(hY),0.0));
		G4VSolid* alSolid = (G4VSolid*)new G4SubtractionSolid("Al_Solid", subSolid8, rodholeSolid, rot, G4ThreeVector(0.0,-half(hY),0.0));
		G4LogicalVolume* alLV = new G4LogicalVolume(alSolid, alMaterial, "Al_Logical");
		m_ModuleTotal_LogicalVolumes.push_back(alLV);

		const std::vector<G4double> dist_vec = {
			dist - 1.0 * CLHEP::cm, // downstream
			dist + 19.8 * CLHEP::mm - half(alX), // upstream 
		};

		for (const auto d : dist_vec) {
			G4ThreeVector placement(d + half(alX), 0.0 * CLHEP::cm, 0.0 * CLHEP::cm);
			new G4PVPlacement(nullptr, placement, alLV, "Al_Volume", m_Gas_LogicalVolumes[0], 0, 1);
		}
		
		/* alminum windows */
		const G4double windowZ = 0.2 * CLHEP::mm;
		G4VSolid* windowSolid = new G4Box("AlWindow_Solid",half(rodL),half(alY)+windowZ,half(alZ)+windowZ);
		G4VSolid* windowHoleSolid = new G4Box("AlWindowHole_Solid",half(rodL)+windowZ,half(alY),half(alZ));
		G4VSolid* windowSubSolid = new G4SubtractionSolid("AlWindowSub_Solid", windowSolid, windowHoleSolid);
		G4LogicalVolume* windowLV = new G4LogicalVolume(windowSubSolid, alMaterial, "AlWindow_Logical");
		m_ModuleTotal_LogicalVolumes.push_back(windowLV);
		new G4PVPlacement(nullptr, G4ThreeVector(half(rodL)+rodX,0,0), windowLV, "AlWindow_Volume", m_Gas_LogicalVolumes[0], 0, 0);

	}
	/* placement of the source container if PointGammaSourceGeneratorAction will be used */
	{
		YamlReader yaml_reader("RunConfigurator");
		G4double pos_x = 0, pos_y = 0, pos_z = 0;
		if (yaml_reader.GetString("PrimaryGeneratorAction") == "PointGammaSourceGeneratorAction") {
			YamlReader pg_reader("PointGammaSourceGeneratorAction");
			pos_z = pg_reader.GetDouble("PositionX") * CLHEP::mm;
			pos_y = pg_reader.GetDouble("PositionY") * CLHEP::mm;
			pos_x = pg_reader.GetDouble("PositionZ") * CLHEP::mm;
			std::cout << "[CloverSimDetector.cpp]: Placement of source: (" << pos_x << "," << pos_y << "," << pos_z << ")" << std::endl;

			const G4double srcX = 1.0 * CLHEP::mm;
			const G4double srcY = 2.0 * CLHEP::cm;
			const G4double srcZ = 1.0 * CLHEP::cm;

			auto container_material = mainDetectorConstruction->GetVinyl();
			G4VSolid* sourceSolid = new G4Box("Source_Solid", half(srcX), half(srcY), half(srcZ));
			G4LogicalVolume* sourceLV = new G4LogicalVolume(sourceSolid, container_material, "Source_Logical");
			m_ModuleTotal_LogicalVolumes.push_back(sourceLV);
			new G4PVPlacement(nullptr, G4ThreeVector(pos_x, pos_y, pos_z), sourceLV, "Source_Volume", m_Gas_LogicalVolumes[0], 0, 0);
		}
	}
	return;
}
