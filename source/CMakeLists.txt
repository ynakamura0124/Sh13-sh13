add_library(CloverSimLib SHARED
	YamlParameter.cpp
	YamlReader.cpp
	GSStateG4.cpp
	GSource4G4.cpp
	Sh13Detector.cpp
	Sh13DetectorConstruction.cc
	PointGammaSourceGeneratorAction.cpp
	CloverSimAnalysisManager.cpp
	RunConfigurator.cpp
	ExN02EventAction.cc
	ExN02RunAction.cc
	ExN02TrackerHit.cc
	ExN02TrackerSD.cc
	)

target_link_libraries(CloverSimLib ${YAML_CPP_LIBRARIES} ${ROOT_LIBRARIES} ${Geant4_LIBRARIES})

install(TARGETS CloverSimLib
	LIBRARY DESTINATION lib
	PUBLIC_HEADER DESTINATION include
)