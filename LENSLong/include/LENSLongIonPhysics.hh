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
//

#ifndef LENSLongIonPhysics_h
#define LENSLongIonPhysics_h 1

#include "globals.hh"
#include "G4ios.hh"

#include "G4VPhysicsConstructor.hh"

#include "G4HadronElasticProcess.hh"
#include "G4LElastic.hh"

#include "G4DeuteronInelasticProcess.hh"
#include "G4LEDeuteronInelastic.hh"

#include "G4TritonInelasticProcess.hh"
#include "G4LETritonInelastic.hh"

#include "G4AlphaInelasticProcess.hh"
#include "G4LEAlphaInelastic.hh"

#include "G4hIonisation.hh"
#include "G4ionIonisation.hh"
#include "G4MultipleScattering.hh"

class LENSLongIonPhysics : public G4VPhysicsConstructor
{
  public: 
    LENSLongIonPhysics(const G4String& name="ion");
    virtual ~LENSLongIonPhysics();

  public: 
// This method will be invoked in the Construct() method. 
// each particle type will be instantiated
    virtual void ConstructParticle();
 
// This method will be invoked in the Construct() method.
// each physics process will be instantiated and
// registered to the process manager of each particle type 
    virtual void ConstructProcess();

  protected:
// Elastic Process
   G4HadronElasticProcess      theElasticProcess;
   G4LElastic*                 theElasticModel;

// Generic Ion physics
   G4MultipleScattering        fIonMultipleScattering;
   G4ionIonisation             fIonIonisation;

// Deuteron physics
   G4MultipleScattering        fDeuteronMultipleScattering;
   G4hIonisation               fDeuteronIonisation;
   G4DeuteronInelasticProcess  fDeuteronProcess;
   G4LEDeuteronInelastic*      fDeuteronModel;

// Triton physics
   G4MultipleScattering        fTritonMultipleScattering;
   G4hIonisation               fTritonIonisation;
   G4TritonInelasticProcess    fTritonProcess;
   G4LETritonInelastic*        fTritonModel;
  
// Alpha physics
   G4MultipleScattering        fAlphaMultipleScattering;
   G4hIonisation               fAlphaIonisation;
   G4AlphaInelasticProcess     fAlphaProcess;
   G4LEAlphaInelastic*         fAlphaModel;

// He3 physics
   G4MultipleScattering        fHe3MultipleScattering;
   G4hIonisation               fHe3Ionisation;

};


#endif // LENSLongIonPhysics_h

