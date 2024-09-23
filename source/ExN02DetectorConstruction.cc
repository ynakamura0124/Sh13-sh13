// #include "Sh13DetectorConstruction.hh"
#include "globals.hh"
#include "G4Material.hh"
#include "G4MaterialTable.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "ExN02TrackerSD.hh"
#include "ExN02DetectorConstruction.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4SDManager.hh"
#include "G4VisAttributes.hh"
#include "YamlReader.hpp"

G4material* Plascin = new G4material("Plascin", density = 50*,)


using namespace CLHEP;
/*G4VPhysicalVolume *Sh13DetectorConstruction::Construct()
{
   ConstructMaterials();
   ConstructVisibilityAttributes();

   // Construct the WORLD Volume
   G4double worldHalf_x = 10.0 * m;
   G4double worldHalf_y = 10.0 * m;
   G4double worldHalf_z = 10.0 * m;

   G4Box *world_box = new G4Box("World_box", worldHalf_x, worldHalf_y, worldHalf_z);
   m_pWorld_logVol = new G4LogicalVolume(world_box, Vacuum, "World_logical", 0, 0, 0);
   m_pWorld_phyVol = new G4PVPlacement(0, G4ThreeVector(0.0, 0.0, 0.0), m_pWorld_logVol, "World_physical", 0, 0, 0);

   //  ConstructLENSLongHall();
   //	if( m_UsePixelatedLattice )	ConstructLENSPixelatedLatticeScintillator();
   ConstructDetector();

   //  DumpGeometricalTree(m_pWorld_phyVol);// Get too long of an output and hence is confusing.

   return m_pWorld_phyVol;
}
*/




/* Detector Solid */
void Sh13DetectorConstruction::ConstructDetector()
{
   // Detector
   G4Tubs *sensitiveCylinder = new G4Tubs("sensitiveCylinder",  * cm,  * mm,   * mm,  * deg,  * deg);
   
   return;
}


/* Detector Material */
void Sh13DetectorConstruction::ConstructMaterials()
{
   // Germanium
   G4Material Ge = new G4Material("Germanium", 32, 72.64 * g / mole, 5.323 * g / cm3, kStateSolid,  * kelvin, * pascal);

   /* Vacuum
   Vacuum = new G4Material("Vacuum", 1., 1.01 * g / mole, universe_mean_density,
                           kStateGas, 2.73 * kelvin, 3.e-18 * pascal);*/

   return;
}


/* Logical Volume & Physical Volume */
G4VPhysicalVolume *Sh13DetectorConstruction::Construct()
{
   ConstructMaterials();
   ConstructVisibilityAttributes();

   // Construct the WORLD Volume
   G4double worldHalf_x = 10.0 * m;
   G4double worldHalf_y = 10.0 * m;
   G4double worldHalf_z = 10.0 * m;

   G4Box *world_box = new G4Box("World_box", worldHalf_x, worldHalf_y, worldHalf_z);
   m_pWorld_logVol = new G4LogicalVolume(world_box, Vacuum, "World_logical", 0, 0, 0);
   m_pWorld_phyVol = new G4PVPlacement(0, G4ThreeVector(0.0, 0.0, 0.0), m_pWorld_logVol, "World_physical", 0, 0, 0);

   //  ConstructLENSLongHall();
   //	if( m_UsePixelatedLattice )	ConstructLENSPixelatedLatticeScintillator();
   ConstructDetector();

   //  DumpGeometricalTree(m_pWorld_phyVol);// Get too long of an output and hence is confusing.

   return m_pWorld_phyVol;
G4LogicalVolume *layerLogicTube =
       new G4LogicalVolume(sensitiveCylinder, Ge, "sensitiveLV", 0, 0, 0);
   G4RotationMatrix *angle = new G4RotationMatrix(0. * deg, 0. * deg, 0. * deg);
   G4ThreeVector sensitivePosit1 = G4ThreeVector(0, 0, 100.);
   G4ThreeVector sensitivePosit2 = G4ThreeVector(0, 0, -100.);

   new G4PVPlacement(angle, sensitivePosit1, layerLogicTube,
                     "Sensitive", m_pWorld_logVol, false, 0);
   new G4PVPlacement(angle, sensitivePosit2, layerLogicTube,
                     "Sensitive", m_pWorld_logVol, false, 1);
   // define sensitive detector
   G4SDManager *SDman = G4SDManager::GetSDMpointer();

   G4String trackerChamberSDname = "ExN02/TrackerChamberSD";
   ExN02TrackerSD *aTrackerSD = new ExN02TrackerSD(trackerChamberSDname);
   SDman->AddNewDetector(aTrackerSD);
   layerLogicTube->SetSensitiveDetector(aTrackerSD);

   G4VisAttributes *layerVisAtt = new G4VisAttributes(G4Colour(1., 1., 1.));
   layerLogicTube->SetVisAttributes(layerVisAtt);


}

   new G4PVPlacement(angle, sensitivePosit1, layerLogicTube,
                     "Sensitive", m_pWorld_logVol, false, 0);
   new G4PVPlacement(angle, sensitivePosit2, layerLogicTube,
                     "Sensitive", m_pWorld_logVol, false, 1);










void Sh13DetectorConstruction::ConstructMaterials()
{

    /*Active Stopper*/
    G4Box* solidstopper;
    G4double stopperthick = 36.*mm;

    /* Core point */

    
    solidstopper = new G4Boxs("stopper",160/2.*mm,120/2.*mm,stopperthick/2);
    
    G4LogicalVolume* logicstopper;

    logicstopper= new G4LogicalVolume(solidstopper, GSO, "Stopper", 0, 0, 0); 
    // stoper made of GSO
    G4ThreeVector stopperPosit = G4ThreeVector(0.,0.,0.);

    /* LogicVolume status (shape,place,arg,name,option) */
    new G4PVPlacement(G4Transform3D(angle, stopperPosit), logicstopper ,"Stopper", logicWorld, false,0,true);
}





void Sh13DetectorConstruction::ConstructDetector()
{
    G4TwistedTubs dph1,dph2;

  /*Ge1*/
  const G4double dph1= -90.*deg;
  G4ThreeVector Geposition1 = G4ThreeVector(0, 0.,Ge1pos);
  G4RotationMatrix Geangle1 = G4RotationMatrix();
  Geposition1.rotateX(dph1);
  Geangle1.rotateX(dph1);

  /*Ge2*/
  const G4double dph2= 90.*deg;
  G4ThreeVector Geposition2 = G4ThreeVector(0, 0.,Ge2pos);
  G4RotationMatrix Geangle2 = G4RotationMatrix();
  Geposition2.rotateX(dph2);
  Geangle2.rotateX(dph2);

  new G4PVPlacement(G4Transform3D(Geangle1,Geposition1),sensitiveLogicTube1,"Sensi\
tive", logicWorld, false, 0, true);

 new G4PVPlacement(G4Transform3D(Geangle2,Geposition2),sensitiveLogicTube2,"Sensi\
tive", logicWorld, false, 1, true);

}


void Sh13DetectorConstruction::ConstructMaterials()
{
    G4double a,z;
    G4double density;
    G4int nel;


    G4Element* N = new G4Element;

    /* Beryllium */
    G4Isotope* Be9 = new G4Material(name = "Be9", z = 4, n = 9, a = 9.012*g/mole);

    /* Europium */
    G4Isotope* Eu2 = new G4Material(name = "Eu152", z = 63, n = 152, a = 151.922*g/mole);

    /* Barium */
    G4Isotope* Ba3 = new G4Material(name = "Ba133", z = 56, n = 133, a = 132.91*g/mole);
}



void Sh13DetectorConstruction::ConstructDetector()
{
const G4double dph1= 45.*deg;
  G4ThreeVector Geposition1 = G4ThreeVector(0, 0.,Ge1pos);
  G4RotationMatrix Geangle1 = G4RotationMatrix();
  Geposition1.rotateY(dph1);
  Geangle1.rotateY(dph1);

  new G4PVPlacement(G4Transform3D(Geangle1,Geposition1),sensitiveLogicTube1,"SensitiveGe1", logicWorld, false, 0, true);

}