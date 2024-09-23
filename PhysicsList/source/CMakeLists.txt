add_library(PhysicsListLib SHARED
	#PhysicsList.cc
	#PhysicsListMessenger.cc
	#PhysListEmStandard.cc
	#PhysListEmStandardNR.cc
	#PhysListEmStandardSS.cc
	StepMax.cc
	StepMaxMessenger.cc
	#NeutronHPphysics.cc
	#NeutronHPMessenger.cc
	#G4ScreenedNuclearRecoil.cc
	#G4LindhardPartition.cc
)

target_link_libraries(PhysicsListLib ${Geant4_LIBRARIES})

install(TARGETS PhysicsListLib DESTINATION lib)
