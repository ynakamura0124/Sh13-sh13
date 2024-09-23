add_library(LENSLongLib SHARED
	G4BetaDecayCorrections.cc
	G4BetaDecayType.cc
	G4BetaFermiFunction.cc
	#G4ScreenedNuclearRecoil.cc
	GLG4OpAttenuation.cc
	GLG4Scint.cc
	MendelejewProvider.cc
	MTASBetaMinus.cc
	MTASBetaPlus.cc
	MTASDecayManager.cc
	MTASGamma.cc
	MTASLevel.cc
	LENSLongAnalysisManager.cc
	LENSLongDetectorConstruction.cc
	LENSLongEventAction.cc
	LENSLongLiquidScintillator.cc
	LENSLongLiquidScintillatorHit.cc
	LENSLongPrimaryGeneratorAction.cc
	LENSLongRunAction.cc
	LENSLongSteppingAction.cc
	LENSLongVisualizationManager.cc
)

target_link_libraries(LENSLongLib ${Geant4_LIBRARIES})

install(TARGETS LENSLongLib DESTINATION lib)
