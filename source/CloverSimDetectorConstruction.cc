#include "Sh13DetectorConstruction.hh"
#include "globals.hh"

#include "G4Material.hh"
#include "G4MaterialTable.hh"

#include "G4Box.hh"
#include "G4LogicalVolume.hh"

#include "G4PVPlacement.hh"
#include "G4SDManager.hh"

#include "G4VisAttributes.hh"
#include "YamlReader.hpp"


using namespace CLHEP;
G4VPhysicalVolume* Sh13DetectorConstruction::Construct()
{
	ConstructMaterials();
	ConstructVisibilityAttributes();

// Construct the WORLD Volume
  G4double worldHalf_x = 10.0*m;
  G4double worldHalf_y = 10.0*m;
  G4double worldHalf_z = 10.0*m;
	
  G4Box* world_box = new G4Box( "World_box", worldHalf_x, worldHalf_y, worldHalf_z );
  m_pWorld_logVol = new G4LogicalVolume( world_box,  m_Vacuum, "World_logical", 0, 0, 0 );
  m_pWorld_phyVol = new G4PVPlacement( 0, G4ThreeVector(0.0,0.0,0.0), m_pWorld_logVol, "World_physical", 0, 0, 0 );
	
//  ConstructLENSLongHall();
//	if( m_UsePixelatedLattice )	ConstructLENSPixelatedLatticeScintillator();
	ConstructDetector();

//  DumpGeometricalTree(m_pWorld_phyVol);// Get too long of an output and hence is confusing.

  return m_pWorld_phyVol;
}

void Sh13DetectorConstruction::ConstructMaterials() {
   LENSLongDetectorConstruction::ConstructMaterials();
//   if (m_PLA)
//      delete m_PLA;

   auto GetElement = [](const std::string e_name)->G4Element* {
      G4ElementTable* elemTable = G4Element::GetElementTable();
      auto itr = std::find_if(
         elemTable->begin(),
         elemTable->end(),
         [&e_name](G4Element* const &x)->bool {return e_name == x->GetName(); }
      );
      if (itr == elemTable->end())
         std::cout << "Cannot find the element named: " << e_name << std::endl;
      return *itr;
   };
   
   m_PLA = new G4Material("PLA", 1.24 * mg / cm3, 3);
   m_PLA->AddElement(GetElement("Hydrogen"), 1);
   m_PLA->AddElement(GetElement("Oxygen"), 3);
   m_PLA->AddElement(GetElement("Carbon"), 3);

   return;
}

void Sh13DetectorConstruction::ConstructDetector() {
   
   G4String SDname;
   m_LENSLongBoxScintillatorSD = new Sh13Detector( SDname = "/LiquidScintillator" );
// The "this" pointer alows the LENSLongLiquidScintillator to get things such as BunkerWidth(), Materials, etc...
	((Sh13Detector*)m_LENSLongBoxScintillatorSD)->ConstructDetector( this ); 

}