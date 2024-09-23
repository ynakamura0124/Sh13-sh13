//
// ********************************************************************
// * DISCLAIMER                                                       *
// *                                                                  *
// * The following disclaimer summarizes all the specific disclaimers *
// * of contributors to this software. The specific disclaimers,which *
// * govern, are listed with their locations in:                      *
// *   http://cern.ch/geant4/license                                  *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.                                                             *
// *                                                                  *
// * This  code  implementation is the  intellectual property  of the *
// * GEANT4 collaboration.                                            *
// * By copying,  distributing  or modifying the Program (or any work *
// * based  on  the Program)  you indicate  your  acceptance of  this *
// * statement, and all its terms.                                    *
// ********************************************************************
//

#include "G4RunManager.hh"
#include "G4UImanager.hh"
#include "G4UItcsh.hh"
#include "G4UIterminal.hh"
#include "G4VisManager.hh"

#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"

#include "RunConfigurator.hpp"
////#include "LENSLongPhysicsList.hh" 
#include "PhysicsList.hh"
#include "LENSLongPrimaryGeneratorAction.hh"
#include "LENSLongVisualizationManager.hh"
#include "LENSLongRunAction.hh"
#include "LENSLongEventAction.hh"
#include "LENSLongSteppingAction.hh"
////#include "LENSLongSteppingVerbose.hh"

//*************************************************************************
void usage(char* argv0) {
   std::cout << "Usage: " << argv0 << "-c [config_file_name]" << std::endl;
}

int main(int argc, char** argv)
{
   std::string config_file_name;
   std::string batch_file_name;
   bool batch_mode = false;
   if (argc < 3) {
      usage(argv[0]);
      return 1;
   }
   int opt = 0;
   while((opt = getopt(argc, argv, "i:o:c:b:")) != -1) {
      switch (opt) {
      case 'i':
         break;
      case 'o':
         break;
      case 'c':
         config_file_name = optarg;
         break;
      case 'b':
         batch_mode = true;
         batch_file_name = optarg;
         break;
      default:
         usage(argv[0]);
         return 1;
         break;
      }
   }

   // Verbose output class
   //  G4VSteppingVerbose::SetInstance(new NuSNSSteppingVerbose);

   // Run manager
   G4RunManager* runManager = new G4RunManager;

   YamlParameter::Create(config_file_name);
   std::cout << "[main()]: opened a yaml file: " << config_file_name << std::endl;
   RunConfigurator configurator;
   if(configurator.Configure(runManager))
      return 1;

   std::cout << "[main()]: deleting the yaml file instance. " << std::endl;
   YamlParameter::Destroy();

   //runManager->SetUserAction( userSteppingAction );// Hmmmmm! What to do?

   //  Initialize G4 kernel, ORIGINAL LOCATION bad because sesitive detectors are not created until here 
   //  but are looked for in NuSNSEventAction. 
   //  runManager->Initialize();

     //get the pointer to the User Interface manager 
   G4UImanager* UI = G4UImanager::GetUIpointer();

   if (!batch_mode) // Define (G)UI terminal for interactive mode  
   {
      G4VisManager* visManager = new G4VisExecutive;
      visManager->Initialize();

      G4UIsession* session = 0;
#ifdef G4UI_USE_TCSH
      session = new G4UIterminal(new G4UItcsh);
#else
      session = new G4UIterminal(); // G4UIterminal is a (dumb) terminal.
#endif

      UI->ApplyCommand("/control/execute vis.mac");
      //UI->ApplyCommand("/control/execute novis.mac");
      session->SessionStart();

      delete session;
      delete visManager;

   }
   else
      // Batch mode
   {
      G4VisManager* visManager = new G4VisExecutive;
      visManager->Initialize();

      G4UIsession* session = 0;
      //#ifdef G4UI_USE_TCSH
      //    session = new G4UIterminal( new G4UItcsh );
      //#else
      session = new G4UIterminal(); // G4UIterminal is a (dumb) terminal.
  //#endif
      G4String command = "/control/execute ";
      G4String totalCmd(command + batch_file_name);
      G4cout << totalCmd << G4endl;
      UI->ApplyCommand(totalCmd);

      //    session->SessionStart();
   }

   // Clean up
    // delete visManager;
   delete runManager;

   return 0;
}


//*************************************************************************

