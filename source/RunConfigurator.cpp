#include "RunConfigurator.hpp"
#include "QGSP_BERT.hh"

RunConfigurator::~RunConfigurator()
{
	// if (yaml_reader_) {
	//	delete yaml_reader_;
	// }
	// if (detector_construction_)
	//	delete detector_construction_;
}

int RunConfigurator::Configure(G4RunManager *runManager)
{

	// reads the yaml file and selects UserInitializaions and UserActions
	YamlReader yaml_reader("RunConfigurator");

	if (detector_construction_)
	{
		delete detector_construction_;
		detector_construction_ = nullptr;
	}
	if (physics_list_)
	{
		delete physics_list_;
		physics_list_ = nullptr;
	}
	if (run_action_)
	{
		delete run_action_;
		run_action_ = nullptr;
	}
	if (event_action_)
	{
		delete event_action_;
		event_action_ = nullptr;
	}
	if (primary_generator_action_)
	{
		delete primary_generator_action_;
		primary_generator_action_ = nullptr;
	}

	// Selection of the DetectorConstruction from Yaml
	std::map<std::string, std::function<G4VUserDetectorConstruction *()>> dc_map;
	dc_map.emplace("LENSLongDetectorConstruction", []()
				   { return new LENSLongDetectorConstruction(); });
	// dc_map.emplace("CloverSimDetectorConstruction", []()
				 //  { return new CloverSimDetectorConstruction(); });
	dc_map.emplace("Sh13DetectorConstruction", []()
				   { return new Sh13DetectorConstruction(); });
	detector_construction_ = dc_map[yaml_reader.GetString("DetectorConstruction")]();

	// Selection of the PhysicsList from Yaml
	std::map<std::string, std::function<G4VUserPhysicsList *()>> pl_map;
	pl_map.emplace("PhysicsList", []()
				   { return new QGSP_BERT; });
	physics_list_ = pl_map[yaml_reader.GetString("PhysicsList")]();

	if (!(detector_construction_) || !(physics_list_))
	{
		std::cout << "[RunConfigurator]: failed to configure from the yaml file." << std::endl;
		return 1;
	}

	runManager->SetUserInitialization(detector_construction_);
	runManager->SetUserInitialization(physics_list_);
	runManager->Initialize();

	// Selection of the PrimaryGeneratorAction from Yaml
	std::map<std::string, std::function<G4VUserPrimaryGeneratorAction *()>> pga_map;
	pga_map.emplace("LENSLongPrimaryGeneratorAction", []()
					{ return new LENSLongPrimaryGeneratorAction(); });
	pga_map.emplace("PointGammaSourceGeneratorAction", []()
					{ return new PointGammaSourceGeneratorAction(); });
#ifdef USE_BRIKEN
	pga_map.emplace("BRIKENGammaSourceGeneratorAction", []()
					{
		auto ptr = new BRIKENGammaSourceGeneratorAction();
		ptr->Configure();
		return ptr; });
#endif
	primary_generator_action_ = pga_map[yaml_reader.GetString("PrimaryGeneratorAction")]();

	// Selection of the RunAction from Yaml
	std::map<std::string, std::function<G4UserRunAction *()>> ra_map;
	ra_map.emplace("LENSLongRunAction", []()
				   { return new LENSLongRunAction(); });
	ra_map.emplace("ExN02RunAction", []()
				   { return new ExN02RunAction(); });
	run_action_ = ra_map[yaml_reader.GetString("RunAction")]();

	// Selection of the EventAction from Yaml
	std::map<std::string, std::function<G4UserEventAction *()>> ea_map;
	ea_map.emplace("LENSLongEventAction", []()
				   { return new LENSLongEventAction(); });
	ea_map.emplace("CloverSimEventAction", []()
				   { return new CloverSimEventAction(); });
	ea_map.emplace("ExN02EventAction", []()
				   { return new ExN02EventAction(new TTree); });
	event_action_ = ea_map[yaml_reader.GetString("EventAction")]();

	if (!(primary_generator_action_) || !(run_action_) || !(event_action_))
	{
		std::cout << "[RunConfigurator]: failed to configure from the yaml file." << std::endl;
		return 1;
	}

	runManager->SetUserAction(primary_generator_action_);
	runManager->SetUserAction(run_action_);
	runManager->SetUserAction(event_action_);
	runManager->SetUserAction(new G4UserSteppingAction);

	return 0;
}