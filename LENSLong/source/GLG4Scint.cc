/** @file GLG4Scint.cc
    For GLG4Scint class, providing advanced scintillation process.
    Distantly based on an extensively modified version of G4Scintillation.cc.
    
    This file is part of the GenericLAND software library.
    $Id: GLG4Scint.cc,v 1.6 2005/05/04 21:08:52 GLG4sim Exp $

    @author Glenn Horton-Smith (Tohoku) 28-Jan-1999
*/

// [see detailed class description in GLG4Scint.hh]

#include "GLG4Scint.hh"
#include "G4ios.hh"
#include "G4Timer.hh"
#include "Randomize.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIdirectory.hh"
#include "G4TrackFastVector.hh"  // for G4TrackFastVectorSize

#include "G4Poisson.hh"

using namespace CLHEP;

/////////////////////////
// Class Implementation  
/////////////////////////

        //////////////
        // Operators
        //////////////

// GLG4Scint::operator=(const GLG4Scint &right)
// {
// }

////////////////
// static data members
////////////////

std::vector<GLG4Scint *> GLG4Scint::masterVectorOfGLG4Scint;
// top level of scintillation command
G4UIdirectory* GLG4Scint::GLG4ScintDir = 0;

// universal maximum number of secondary tracks per step for GLG4Scint
//G4int GLG4Scint::maxTracksPerStep = G4TrackFastVectorSize;
G4int GLG4Scint::maxTracksPerStep = 180000;

// universal mean number of true photons per secondary track in GLG4Scint
//G4double GLG4Scint::meanPhotonsPerSecondary = 3.0; // Default value in GLG4sim
G4double GLG4Scint::meanPhotonsPerSecondary = 1.0; 

// universal on/off flag
G4bool GLG4Scint::doScintillation = true;
G4bool GLG4Scint::doReemission = true;

// energy deposition
G4double GLG4Scint::totEdep = 0.0;
G4double GLG4Scint::totEdep_quenched = 0.0;
G4ThreeVector GLG4Scint::scintCentroidSum = G4ThreeVector(0.0,0.0,0.0);

/////////////////
// Constructors
/////////////////

GLG4Scint::GLG4Scint(const G4String& tablename,
		 G4double lowerMassLimit)
{
    verboseLevel= 1;
    myLowerMassLimit = lowerMassLimit;

    myPhysicsTable = MyPhysicsTable::FindOrBuild( tablename );
    myPhysicsTable -> IncUsedBy();
    if( verboseLevel > 0 ) myPhysicsTable->Dump();

    // add to ordered list
    if (masterVectorOfGLG4Scint.size() == 0
	|| lowerMassLimit >= masterVectorOfGLG4Scint.back()->myLowerMassLimit)
      {
	masterVectorOfGLG4Scint.push_back(this);
      }
    else for (std::vector<GLG4Scint *>::iterator
	              i = masterVectorOfGLG4Scint.begin();
	      i != masterVectorOfGLG4Scint.end();
	      i++)
      {
	if (lowerMassLimit < (*i)->myLowerMassLimit)
	  {
	    masterVectorOfGLG4Scint.insert(i, this);
	    break;
	  }
      }

    // create UI commands if necessary
    if (GLG4ScintDir == NULL) {
      // the scintillation control commands
      GLG4ScintDir = new G4UIdirectory("/glg4scint/");
      GLG4ScintDir->SetGuidance("scintillation process control.");
      G4UIcommand *cmd;
      cmd= new G4UIcommand("/glg4scint/on", this);
      cmd->SetGuidance("Turn on scintillation");
      cmd= new G4UIcommand("/glg4scint/off", this);
      cmd->SetGuidance("Turn off scintillation");
      cmd= new G4UIcommand("/glg4scint/reemission", this);
      cmd->SetGuidance("Turn on/off reemission of absorbed opticalphotons");
      cmd->SetParameter(new G4UIparameter("status", 's', false));
      cmd= new G4UIcommand("/glg4scint/maxTracksPerStep",this);
      cmd->SetGuidance("Set maximum number of opticalphoton tracks per step\n"
		       "(If more real photons are needed, "
		       "weight of tracked particles is increased.)\n" );
      cmd->SetParameter(new G4UIparameter("maxTracksPerStep", 'i', false));
      cmd= new G4UIcommand("/glg4scint/meanPhotonsPerSecondary",this);
      cmd->SetGuidance("Set mean number of \"real\" photons per secondary\n");
      cmd->SetParameter(new G4UIparameter("meanPhotonsPerSecondary",
					  'd', false));
      cmd= new G4UIcommand("/glg4scint/verbose",this);
      cmd->SetGuidance("Set verbose level");
      cmd->SetParameter(new G4UIparameter("level", 'i', false));
      cmd= new G4UIcommand("/glg4scint/dump",this);
      cmd->SetGuidance("Dump tables");
    }
    
#ifdef G4VERBOSE
    G4cout << "GLG4Scint[" << tablename << "]"
	   << " is created " << G4endl;
#endif
}

// GLG4Scint::GLG4Scint(const GLG4Scint &right)
// {
// }

        ////////////////
        // Destructors
        ////////////////

GLG4Scint::~GLG4Scint() 
{
    myPhysicsTable -> DecUsedBy();
    for (std::vector<GLG4Scint *>::iterator i=masterVectorOfGLG4Scint.begin();
	 i != masterVectorOfGLG4Scint.end(); i++)
      if (*i == this)
	masterVectorOfGLG4Scint.erase(i);
}


        ////////////
        // Methods
        ////////////

// PostStepDoIt
// -------------
//

#ifdef G4DEBUG
G4double GLG4Scint_tottime = 0.0;
G4int GLG4Scint_num_calls= 0;
G4int GLG4Scint_num_phots= 0;
#endif

G4VParticleChange*
GLG4Scint::PostPostStepDoIt(const G4Track& aTrack, const G4Step& aStep)

  // This routine is called for each step of any particle
  // in a scintillator.  For accurate energy deposition, must be called
  // from user-supplied UserSteppingAction, which also must stack
  // any particles created.  A pseudo-Poisson-distributed number of
  // photons is generated according to the scintillation yield formula,
  // distributed evenly along the track segment and uniformly into 4pi.

{
#ifdef G4DEBUG
  G4Timer timer;
  timer.Start();
  GLG4Scint_num_calls ++;
#endif
  {
    // prepare to generate an event, organizing to
    // check for things that cause an early exit.	
    aParticleChange.Initialize(aTrack);

    G4bool flagReemission= false;
    if (aTrack.GetDefinition() == G4OpticalPhoton::OpticalPhoton()) 
		{
      flagReemission = doReemission	&& aTrack.GetTrackStatus() == fStopAndKill	&& aStep.GetPostStepPoint()->GetStepStatus() != fGeomBoundary;
			
      if (!flagReemission) goto PostStepDoIt_DONE;
    }

    G4double TotalEnergyDeposit = aStep.GetTotalEnergyDeposit();
    if (TotalEnergyDeposit <= 0.0 && !flagReemission) goto PostStepDoIt_DONE;
	  

//  USE TO DEBUG	G4cout << "C: " << TotalEnergyDeposit << G4endl; USE TO DEBUG
    const G4Material* aMaterial = aTrack.GetMaterial();

    // get pointer to the physics entry
    const MyPhysicsTable::Entry* physicsEntry = myPhysicsTable->GetEntry( aMaterial->GetIndex() );
		
    if (!physicsEntry) goto PostStepDoIt_DONE;
	
//	G4cout << "C1" << G4endl; USE TO DEBUG
    // Retrieve the Light Yield or Scintillation Integral for this material  

    G4double ScintillationYield=physicsEntry->light_yield;
    G4PhysicsOrderedFreeVector* ScintillationIntegral = physicsEntry->spectrumIntegral;
    G4PhysicsOrderedFreeVector* ReemissionIntegral = physicsEntry->reemissionIntegral;

//	G4cout << "D: "<< ScintillationIntegral << G4endl;
    
    if (!ScintillationIntegral) goto PostStepDoIt_DONE;
    
    // If no LY defined Max Scintillation Integral == ScintillationYield
    if(!ScintillationYield) ScintillationYield= ScintillationIntegral->GetMaxValue();
    
    // set positions, directions, etc.
    G4StepPoint* pPreStepPoint  = aStep.GetPreStepPoint();
    G4StepPoint* pPostStepPoint = aStep.GetPostStepPoint();

    G4ThreeVector x0 = pPreStepPoint->GetPosition();
    G4ThreeVector p0 = pPreStepPoint->GetMomentumDirection();
    G4double t0 = pPreStepPoint->GetGlobalTime();
//		G4double E0 = pPreStepPoint->GetKineticEnergy();
		
    // Finally ready to start generating the event
    // figure out how many photons we want to make
	  
    G4int numSecondaries;
    G4double weight;
		
//		const int maxNumberOutput = 1000;
//		static G4double TotalNumberPhotons = 0.0;
		
	if (flagReemission)
	{
		G4MaterialPropertiesTable* mpt_scint = aMaterial->GetMaterialPropertiesTable();
		G4MaterialPropertyVector* mpv_scint_reemission = mpt_scint->GetProperty("REEMISSION_PROB");
			
		if (mpv_scint_reemission == 0) goto PostStepDoIt_DONE;
			
		G4double p_reemission = mpv_scint_reemission->Value(aTrack.GetKineticEnergy());
		if (G4UniformRand() >= p_reemission) goto PostStepDoIt_DONE;
			
		numSecondaries= 1;
		weight = aTrack.GetWeight();
	}
	else
	{
	// apply Birk's law
		const G4double birksConstant = physicsEntry->birksConstant;
		G4double QuenchedTotalEnergyDeposit = TotalEnergyDeposit;
		
		if ( birksConstant != 0.0 )
		{
			const G4double stepLength = aStep.GetStepLength();
			G4double dE_dx = stepLength != 0.0 ? TotalEnergyDeposit / aStep.GetStepLength() : 0.0;
						
			if( birksConstant == -1.0 && dE_dx != 0.0 )// Handle NaI weird shape
			{
		// THIS IS THE UNITS OF THE curve!
				dE_dx /= keV;
				dE_dx *= nm;
				QuenchedTotalEnergyDeposit *= getCalcedNonLinearLofEPerStep( dE_dx );
				// Assumes NoDeltaElectrons Ionization
//		  static G4double edepSUm = 0.0;
//		  static G4double eQuenchSUm = 0.0;
//			if( birksConstant == -1.0 )// Handle NaI weird shape
//			{
//				QuenchedTotalEnergyDeposit = getExperimentalNaINonlinearity( E0 / keV )* E0 / keV;
//				QuenchedTotalEnergyDeposit -= 
//					getExperimentalNaINonlinearity( ( E0 - TotalEnergyDeposit ) / keV ) * ( E0 - TotalEnergyDeposit ) / keV;
//				QuenchedTotalEnergyDeposit *= keV;
//				edepSUm += TotalEnergyDeposit;
//				eQuenchSUm += QuenchedTotalEnergyDeposit;
//				G4cout << E0 / keV << ", " << TotalEnergyDeposit / keV << ", " 
//							 << QuenchedTotalEnergyDeposit / keV << ", " << edepSUm / keV << ", " << eQuenchSUm / keV << G4endl;
			}
			else
			{
				QuenchedTotalEnergyDeposit /= ( 1.0 + birksConstant * dE_dx );
			}
		}

		// track total edep, quenched edep
		totEdep += TotalEnergyDeposit;
		totEdep_quenched += QuenchedTotalEnergyDeposit;
		scintCentroidSum += QuenchedTotalEnergyDeposit * ( x0 + p0*(0.5*aStep.GetStepLength()) );

	//  USE TO DEBUG	G4cout << "E" << G4endl;
		// now we are done if we are not actually making photons here
		if ( !doScintillation ) goto PostStepDoIt_DONE;
			
	//  USE TO DEBUG	G4cout << "F" << G4endl;
		// calculate MeanNumPhotons
		G4double MeanNumPhotons = ScintillationYield * QuenchedTotalEnergyDeposit * (1.0 + birksConstant * (physicsEntry->ref_dE_dx) );

		if( birksConstant == -1.0 )
		{
			MeanNumPhotons = ScintillationYield * QuenchedTotalEnergyDeposit;
			
//			TotalNumberPhotons += MeanNumPhotons;
			
//			static int used = 0;
//			if( used < maxNumberOutput )
//			{
//				G4cout << used << ": " << TotalNumberPhotons << ", " << MeanNumPhotons <<  G4endl;// USE TO DEBUG
//				used++;
//			}
			
		}
	//  USE TO DEBUG	G4cout << "G" << G4endl;
		if (MeanNumPhotons <= 0.0) goto PostStepDoIt_DONE;
		
	//  USE TO DEBUG	G4cout << "H" << G4endl;
		// randomize number of TRACKS (not photons)
		// this gets statistics right for number of PE after applying
		// boolean random choice to final absorbed track (change from
		// old method of applying binomial random choice to final absorbed
		// track, which did want poissonian number of photons divided
		// as evenly as possible into tracks)
		// Note for weight=1, there's no difference between tracks and photons.
		G4double MeanNumTracks= MeanNumPhotons/meanPhotonsPerSecondary;
		
		G4double resolutionScale= physicsEntry->resolutionScale;
		if (MeanNumTracks > 12.0)
		{
			numSecondaries= (G4int)(G4RandGauss::shoot(MeanNumTracks, resolutionScale * sqrt(MeanNumTracks)));
		}
		else 
		{
			if (resolutionScale > 1.0)
			{
				MeanNumTracks = G4RandGauss::shoot( MeanNumTracks, sqrt( resolutionScale*resolutionScale-1.0 ) * MeanNumTracks);
			}
			numSecondaries= (G4int) G4Poisson(MeanNumTracks);
	// OLD VERSION//  (G4int)( G4RandPoisson::shoot(MeanNumTracks) );
		}
		//numSecondaries=MeanNumTracks;
		
		weight= meanPhotonsPerSecondary;
		if (numSecondaries > maxTracksPerStep)
		{
			// it's probably better to just set meanPhotonsPerSecondary to
			// a big number if you want a small number of secondaries, but
			// this feature is retained for backwards compatibility.
			weight= weight * numSecondaries/maxTracksPerStep;
			numSecondaries= maxTracksPerStep;
		}
	}    
    // if there are no photons, then we're all done now
	if (numSecondaries <= 0)
	{
	// return unchanged particle and no secondaries  
		aParticleChange.SetNumberOfSecondaries(0);
		goto PostStepDoIt_DONE;
	}
	// Okay, we will make at least one secondary.
	// Notify the proper authorities.
	aParticleChange.SetNumberOfSecondaries(numSecondaries);
	if (!flagReemission)
	{
		if (aTrack.GetTrackStatus() == fAlive) aParticleChange.ProposeTrackStatus(fSuspend);
	}
    // now look up waveform information we need to add the secondaries
	G4PhysicsOrderedFreeVector* WaveformIntegral = physicsEntry->timeIntegral;

	for (G4int iSecondary = 0; iSecondary < numSecondaries; iSecondary++)
	{

	// Determine photon momentum

	G4double sampledMomentum;
	if ( !flagReemission )
	{
		// normal scintillation
		G4double CIIvalue = G4UniformRand()*
	                        ScintillationIntegral->GetMaxValue();
	    sampledMomentum =  ScintillationIntegral->GetEnergy(CIIvalue);
			
#ifdef G4DEBUG
	    if (verboseLevel>1) {
	      G4cout << "sampledMomentum = " << sampledMomentum << G4endl;
	      G4cout << "CIIvalue =        " << CIIvalue << G4endl;
	    }
#endif

	}
	else
	{// reemission
		G4bool this_is_REALLY_STUPID;
		G4double CIIvalue = G4UniformRand() * ReemissionIntegral->GetValue(aTrack.GetKineticEnergy(), this_is_REALLY_STUPID );
		if (CIIvalue == 0.0)
		{// return unchanged particle and no secondaries  
			aParticleChange.SetNumberOfSecondaries(0);
			goto PostStepDoIt_DONE;
		}
		sampledMomentum = ReemissionIntegral->GetEnergy(CIIvalue);
		aParticleChange.ProposeLocalEnergyDeposit( aTrack.GetKineticEnergy() - sampledMomentum );
		
#ifdef G4DEBUG
	    if (sampledMomentum > aTrack.GetKineticEnergy()) {
	      G4cerr << "Error in GLG4Scint: sampled reemitted photon momentum "
		     << sampledMomentum
		     << " is greater than track energy "
		     << aTrack.GetKineticEnergy() << G4endl;
	    }
	    if (verboseLevel>1) {
	      G4cout << "oldMomentum = " <<aTrack.GetKineticEnergy() << G4endl;
	      G4cout << "reemittedSampledMomentum = " << sampledMomentum
		     << "\nreemittedCIIvalue =        " << CIIvalue << G4endl;
	    }
#endif

	  }
	
	
	// Generate random photon direction

	G4double cost = 1. - 2.*G4UniformRand();
	G4double sint = sqrt(1.-cost*cost);  // FIXED BUG from G4Scint

	G4double phi = 2*M_PI*G4UniformRand();
	G4double sinp = sin(phi);
	G4double cosp = cos(phi);

	G4double px = sint*cosp;
	G4double py = sint*sinp;
	G4double pz = cost;

	// Create photon momentum direction vector 

	G4ParticleMomentum photonMomentum(px, py, pz);

	// Determine polarization of new photon 

	G4double sx = cost*cosp;
	G4double sy = cost*sinp; 
	G4double sz = -sint;

	G4ThreeVector photonPolarization(sx, sy, sz);

	G4ThreeVector perp = photonMomentum.cross(photonPolarization);

	phi = 2*M_PI*G4UniformRand();
	sinp = sin(phi);
	cosp = cos(phi);

	photonPolarization = cosp * photonPolarization + sinp * perp;

	photonPolarization = photonPolarization.unit();

	// Generate a new photon:

	G4DynamicParticle* aScintillationPhoton = new G4DynamicParticle(G4OpticalPhoton::OpticalPhoton(), photonMomentum);
	aScintillationPhoton->SetPolarization(photonPolarization.x(),photonPolarization.y(),photonPolarization.z());

	aScintillationPhoton->SetKineticEnergy(sampledMomentum);

	// Generate new G4Track object:

	G4ThreeVector aSecondaryPosition;
	G4double deltaTime;
	if (flagReemission)
	{
		deltaTime= pPostStepPoint->GetGlobalTime() - t0;
		aSecondaryPosition= pPostStepPoint->GetPosition();
	}
	else
	{
		G4double delta= G4UniformRand() * aStep.GetStepLength();
		aSecondaryPosition = x0 + delta * p0;

		// start deltaTime based on where on the track it happened
		deltaTime = delta /((pPreStepPoint->GetVelocity()+pPostStepPoint->GetVelocity())/2.);
	}

	// delay for scintillation time

	if ( WaveformIntegral )
	{
		G4double WFvalue = G4UniformRand() * WaveformIntegral->GetMaxValue();
		G4double sampledDelayTime = WaveformIntegral->GetEnergy(WFvalue);
		deltaTime += sampledDelayTime;
	}

	// set secondary time
		
	G4double aSecondaryTime = t0 + deltaTime;

	// create secondary track
		
	G4Track* aSecondaryTrack = new G4Track(aScintillationPhoton, aSecondaryTime, aSecondaryPosition);
	aSecondaryTrack->SetWeight( weight );
	aSecondaryTrack->SetParentID(aTrack.GetTrackID());
	aSecondaryTrack->SetCreatorProcess( NULL );
		
	// add the secondary to the ParticleChange object
	aParticleChange.SetSecondaryWeightByProcess( true ); // recommended
	aParticleChange.AddSecondary(aSecondaryTrack);
	
	aSecondaryTrack->SetWeight( weight );
	// The above line is necessary because AddSecondary() overrides
	// our setting of the secondary track weight, in Geant4.3.1 & earlier.
	// (and also later, at least until Geant4.7 (and beyond?)
	//  -- maybe not required if SetWeightByProcess(true) called,
	//  but we do both, just to be sure)
      }
    // done iSecondary loop
  }
	
	PostStepDoIt_DONE:
	
#ifdef G4DEBUG    
  timer.Stop();
  GLG4Scint_tottime += timer.GetUserElapsed();
  GLG4Scint_num_phots += aParticleChange.GetNumberOfSecondaries();
#endif

#ifdef G4VERBOSE
  if (verboseLevel>1) {
    G4cout << "\n Exiting from GLG4Scint::DoIt -- NumberOfSecondaries = " 
	   << aParticleChange.GetNumberOfSecondaries() << G4endl;
  }
#endif

  return &aParticleChange;
}


////////////////////////////////////////////////////////////////
// the generic (static) PostPostStepDoIt
G4VParticleChange*
GLG4Scint::
GenericPostPostStepDoIt(const G4Step *pStep)
{
  G4Track *track= pStep->GetTrack();
  G4double mass= track->GetDynamicParticle()->GetMass();
  std::vector<GLG4Scint *>::iterator it = masterVectorOfGLG4Scint.begin();
  for (int i= masterVectorOfGLG4Scint.size(); (i--) > 1; )
	{
		it++;
		if ( mass < (*it)->myLowerMassLimit )
		{
			return (*(--it))->PostPostStepDoIt(*track,*pStep);
		}
	}
	
  return (*it)->PostPostStepDoIt(*track, *pStep);
}



G4double GLG4Scint::getExperimentalNaINonlinearity( G4double E ) const
{// E in keV?
	static const G4double a[] = 
	{
	1.6,
	0.058,
	0.580,
	0.00490,
	0.25,
	0.479,
	0.00494
	};

	G4double nonlinearity = 1.0;// Set to 1.0 for test and indeed the L(E)/E is identically 1.0 in that case.
	nonlinearity = a[0] * ( 1.0 - exp( -a[1] * E) ) + a[2] * E + a[3] * E * E;
	nonlinearity /= (a[4] + a[5] * E + a[6] * E * E);

	if( E < 5.0 && nonlinearity < 1.0 ) nonlinearity = 1.0;

	return nonlinearity;
}

G4double GLG4Scint::getCalcedNonLinearLofEPerStep( G4double dEdx )
{
//const int nBins = 49;// Used for usual ionization
//
//static double dEdxBinCuts[nBins] = 
//{
//0.0, 0.0001,0.0002,0.0003,0.0004,0.0005,0.0006,0.0007,0.0008,0.0009,
//0.001, 0.0011,0.0012,0.0013,0.0014,0.0015,0.0016,0.0017,0.0018, 0.0019,
//0.002, 0.0021,0.0022,0.0023,0.0024,0.0025,0.0026,0.0027,0.0028, 0.0029,
//0.003,0.004,0.005,0.006,0.007,0.008,0.009, 
//0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09, 
//0.1, 0.5, 1.0
//};
//static double alpha[nBins] =
//{
//0.89, 0.89, 0.905, 0.92, 0.92, 0.92, 0.92, 0.91, 0.91, 0.91, // 0.0 to 0.0009
//0.95, 1.0, 1.05, 1.10, 1.15, 1.2, 1.25, 1.3, 1.7, 2.1, // 0.0010 to 0.0019
//2.1, 2.0, 1.9, 1.75, 1.6, 1.5, 1.45, 1.4, 1.35, 1.30, // 0.0020 to 0.0029
//1.30, 1.30, 1.25, 1.2, 1.05, 1.01, 1.0, // 0.003 to 0.009
//1.0, 0.95, 0.95, 0.95, 0.95, 0.95, 0.95, 0.95, 0.95, // Up to 0.09 
//0.95, 0.95, 0.95 // 0.1 to 1.0
//};

const G4int nBins = 26;// Used for 100um cuts+ NoDelta Electrons created in ionization

static G4double dEdxBinCuts[nBins] = 
{
0.0001,0.0002,0.0003,0.0004,0.0005,0.0006,0.0007,0.0008,0.0009,
0.001,0.002,0.003,0.004,0.005,0.006,0.007,0.008,0.009,
0.01,0.011,0.012,0.013,0.014,0.015,0.02,0.03
};
static G4double alpha[nBins] =
{
0.93, 0.93, 0.93, 0.94, 1.00, 1.00, 1.00, 1.00, 1.00, 
1.07, 1.14, 1.225, 1.25, 1.265, 1.265, 1.265, 1.265, 1.262, 
1.26, 1.25, 1.2, 1.17, 1.15, 1.1, 1.00, 1.00
};

//static const int nBins = 22;
//
//static G4double dEdxBinCuts[nBins] = 
//{
//0.0001,0.0002,0.0003,0.0004,0.0005,0.0008,0.001,0.002,0.003,0.004,0.005,0.006,0.007,0.008,0.009,0.01,0.015,0.02,0.03,0.04,0.1,1.0,
//};
//static G4double alpha[nBins] =
//{
//0.9, 0.91, 0.925, 0.95, 0.96, 0.97, 1.08, 1.14, 1.22, 1.25, 1.27, 1.276, 1.275, 1.274, 1.272, 1.25, 1.175, 1.15, 1.11, 1.08, 1.04, 1.0
//};

G4double returnE = 0.0;

//if( E <= 0.0 ) return 0.0;
//if( dist == 0.0 ) return dEdxBinCuts[0];// * E;

int i = 0;

while( i < nBins && dEdxBinCuts[i] < dEdx ) 
{
i++;
}

if( i < nBins )
{
G4double y = 0.0;

if( i != 0 )
{
y = dEdx - dEdxBinCuts[i];// -1.0 <= y < 0.0
y /= dEdxBinCuts[i] - dEdxBinCuts[i-1];
y *= alpha[i] - alpha[i-1];
}

returnE = alpha[i] + y;
}
else if( i == nBins )
{
returnE = alpha[nBins-1];
}
else returnE = 1.0;// Something is broken.

//std::cout << dedx << ", " << i << std::endl;
if( returnE < 0.0 ) returnE = 0.0;

//returnE *= E;
//std::cout << returnE << std::endl;

return returnE;

}

////////////////////////////////////////////////////////////////
// build physics tables for the scintillation process
// --------------------------------------------------
//

static G4PhysicsOrderedFreeVector* Integrate_MPV_to_POFV( G4MaterialPropertyVector* inputVector )
{
  G4PhysicsOrderedFreeVector *aPhysicsOrderedFreeVector = new G4PhysicsOrderedFreeVector();
    
    // Retrieve the first intensity point in vector
    // of (photon momentum, intensity) pairs 

//    inputVector->ResetIterator();
//    ++(*inputVector);	// advance to 1st entry 

  G4double currentIN = (*inputVector)[0];

  if( currentIN >= 0.0)
  {
    // Create first (photon momentum, Scintillation
    // Integral pair  

    G4double currentPM = inputVector->GetEnergy(0);

    G4double currentCII = 0.0;

    aPhysicsOrderedFreeVector->InsertValues(currentPM , currentCII);

    // Set previous values to current ones prior to loop

    G4double prevPM  = currentPM;
    G4double prevCII = currentCII;
    G4double prevIN  = currentIN;

    // loop over all (photon momentum, intensity)
    // pairs stored for this material  

 //   while(++(*inputVector))
      for( G4int jj = 0; jj < inputVector->GetVectorLength(); jj++ )
    {
        currentPM = inputVector->GetEnergy(jj);
        currentIN= (*inputVector)[jj];

        currentCII = 0.5 * (prevIN + currentIN);

        currentCII = prevCII +(currentPM - prevPM) * currentCII;

        aPhysicsOrderedFreeVector->InsertValues(currentPM, currentCII);

        prevPM  = currentPM;
        prevCII = currentCII;
        prevIN  = currentIN;
    }

  }

  return aPhysicsOrderedFreeVector;
}



////////////////////////////////////////////////////////////////
// MyPhysicsTable (nested class) definitions
////////////////////////////////////////////////////////////////

////////////////
// "static" members of the class
// [N.B. don't use "static" keyword here, because it means something
//  entirely different in this context.]
////////////////

GLG4Scint::MyPhysicsTable*
GLG4Scint::MyPhysicsTable::head = NULL;


////////////////
// constructor
////////////////

GLG4Scint::MyPhysicsTable::MyPhysicsTable()
{
    name=0; next=0; used_by_count=0; data=0; length=0;
}

////////////////
// destructor
////////////////

GLG4Scint::MyPhysicsTable::~MyPhysicsTable()
{
    if (used_by_count != 0) {
        G4cerr << "Error, GLG4Scint::MyPhysicsTable is being deleted with "
	  "used_by_count=" << used_by_count << G4endl;
	return;
    }
    if (name) delete name;
    if (data) delete[] data;
}


////////////////
// member functions
////////////////

void GLG4Scint::MyPhysicsTable::Dump(void) const
{
    G4cout << " GLG4Scint::MyPhysicsTable {\n"
  	      "  name=" << (*name) << G4endl
  	   << "  length=" << length << G4endl
	   << "  used_by_count=" << used_by_count << G4endl;
    for (G4int i=0; i<length; i++) {
        G4cout << "  data[" << i << "]= { // "
	       << (*G4Material::GetMaterialTable())[i]->GetName() << G4endl;
  	G4cout << "   spectrumIntegral=";
	if (data[i].spectrumIntegral)
	  (data[i].spectrumIntegral)->DumpValues();
	else
	  G4cout << "NULL" << G4endl;
        
  	G4cout << "   reemissionIntegral=";
	if (data[i].reemissionIntegral)
	  (data[i].reemissionIntegral)->DumpValues();
	else
	  G4cout << "NULL" << G4endl;
  	
	G4cout << "   timeIntegral=";
	if (data[i].timeIntegral)
	  (data[i].timeIntegral)->DumpValues();
	else
	  G4cout << "NULL" << G4endl;
  	G4cout << "   resolutionScale=" << data[i].resolutionScale
  	       << "   birksConstant=" << data[i].birksConstant
  	       << "   ref_dE_dx=" << data[i].ref_dE_dx << G4endl
   	       << "   light yield=" << data[i].light_yield << G4endl
 	       << "  }\n";
    }
    G4cout << " }\n";
}

GLG4Scint::MyPhysicsTable *
GLG4Scint::MyPhysicsTable::FindOrBuild(const G4String& name)
{
    // head should always exist and should always be the default (name=="")
    if (head == NULL) {
	head= new MyPhysicsTable;
	head->Build("");
    }

    MyPhysicsTable *rover= head;
    while (rover) {
	if ( name == *(rover->name) )
	  return rover;
	rover= rover->next;
    }

    rover= new MyPhysicsTable;
    rover->Build(name);
    rover->next= head->next;  // always keep head pointing to default
    head->next= rover;

    return rover;
}

void GLG4Scint::MyPhysicsTable::Build(const G4String& newname)
{
    if (name) delete name;
    if (data) delete[] data;

    name= new G4String(newname);
    
    const G4MaterialTable* theMaterialTable = 
      G4Material::GetMaterialTable();

    length = G4Material::GetNumberOfMaterials();

    data = new Entry [length];

    // create new physics tables
    
    for (G4int i=0 ; i < length; i++) {

        // look for material properties table entry.
    
        const G4Material* aMaterial = (*theMaterialTable)[i];

	// ask data[i] to Build itself
	
	data[i].Build(*name, i, aMaterial->GetMaterialPropertiesTable() );

    }
}

////////////////
// constructor for Entry
////////////////

GLG4Scint::MyPhysicsTable::Entry::Entry()
{
    spectrumIntegral= reemissionIntegral = timeIntegral= NULL;
    I_own_spectrumIntegral= I_own_timeIntegral= 0;
    resolutionScale= 1.0;
    light_yield=0.0;
    birksConstant= ref_dE_dx= 0.0;
}

////////////////
// destructor for Entry
////////////////

GLG4Scint::MyPhysicsTable::Entry::~Entry()
{
    if (spectrumIntegral && I_own_spectrumIntegral) {
     delete spectrumIntegral;
     delete reemissionIntegral;
    }
    if (timeIntegral && I_own_timeIntegral) delete timeIntegral;
}

////////////////
// Build for Entry
////////////////

void GLG4Scint::MyPhysicsTable::Entry::Build(const G4String& name,
						int i,
						G4MaterialPropertiesTable
						*aMaterialPropertiesTable)
{
    // delete old data, if any
    
    if (spectrumIntegral && I_own_spectrumIntegral) {
     delete spectrumIntegral;
     delete reemissionIntegral;
    } 
    if (timeIntegral && I_own_timeIntegral) delete timeIntegral;

    // set defaults
    spectrumIntegral= reemissionIntegral= timeIntegral= NULL;
    resolutionScale= 1.0;
    birksConstant= ref_dE_dx= 0.0;    
    light_yield=0.0;    
    // exit, leaving default values, if no material properties

    if (!aMaterialPropertiesTable)
      return;
    
    // Retrieve vector of scintillation wavelength intensity
    // for the material from the material's optical
    // properties table ("SCINTILLATION")
    char property_string[80];

    // strncpy(property_string, ("SCINTILLATION"+name).c_str(),
    // 	       sizeof(property_string));
    sprintf(property_string, "SCINTILLATION%s", name.c_str());
    G4MaterialPropertyVector* theScintillationLightVector = aMaterialPropertiesTable->GetProperty(property_string);

    sprintf(property_string, "SCINTILLATION_WLS%s", name.c_str());   
    G4MaterialPropertyVector* theReemissionLightVector = aMaterialPropertiesTable->GetProperty(property_string);

    if (theScintillationLightVector && !theReemissionLightVector) {
     G4cout<<"\nWarning! Found a scintillator without Re-emission spectrum";
     G4cout<<" (probably a scintillator without WLS)"<<G4endl;
     G4cout<<"I will assume that for this material this spectrum is equal ";
     G4cout<<"to the primary scintillation spectrum..."<<G4endl;
     theReemissionLightVector=theScintillationLightVector;
    }   
       
    if (theScintillationLightVector) {
      if(aMaterialPropertiesTable->ConstPropertyExists("LIGHT_YIELD"))
       light_yield=aMaterialPropertiesTable->GetConstProperty("LIGHT_YIELD");
      else {
       G4cout<<"\nWarning! Found a scintillator without LIGHT_YIELD parameter.";
       G4cout<<"\nI will assume that for this material this parameter is ";
       G4cout<<"implicit in the scintillation integral..."<<G4endl;
      }
	// find the integral

	spectrumIntegral =
	  Integrate_MPV_to_POFV( theScintillationLightVector );
        reemissionIntegral = 
          Integrate_MPV_to_POFV( theReemissionLightVector );   
	I_own_spectrumIntegral= 1;
	
    }
    else {
	// use default integral (possibly null)
	spectrumIntegral =
	  MyPhysicsTable::GetDefault()->GetEntry(i)->spectrumIntegral;
	reemissionIntegral = spectrumIntegral;
	I_own_spectrumIntegral = 0;
    }

    // Retrieve vector of scintillation time profile
    // for the material from the material's optical
    // properties table ("SCINTWAVEFORM")
    
    //strncpy(property_string, ("SCINTWAVEFORM"+name).c_str(),
    //	      sizeof(property_string));
	
    sprintf(property_string, "SCINTWAVEFORM%s", name.c_str());
    G4MaterialPropertyVector* theWaveForm = aMaterialPropertiesTable->GetProperty(property_string);
    
    if (theWaveForm) {

        // do we have time-series or decay-time data?
      
        if ( theWaveForm->GetMinLowEdgeEnergy() >= 0.0 ) {

	  // we have digitized waveform (time-series) data
	  // find the integral

	  timeIntegral =
	    Integrate_MPV_to_POFV( theWaveForm );
	  I_own_timeIntegral= 1;
	}
	else
  {

	  // we have decay-time data.
	  
	  // sanity-check user's values:
	  // issue a warning if they are nonsense, but continue
	  if ( theWaveForm->GetMaxLowEdgeEnergy() > 0.0 ) {
	      G4cerr << "GLG4Scint::MyPhysicsTable::Entry::Build():  "
		     << "SCINTWAVEFORM" << name
		     << " has both positive and negative X values.  "
		" Undefined results will ensue!\n";
	  }

	  G4double maxtime = -10.0 * ( theWaveForm->GetMinLowEdgeEnergy() );
	  int nbins = 100;
	  G4double* tval= new G4double[nbins];
	  G4double* ival= new G4double[nbins];
	  for( int i = 0; i < nbins; i++ )
	  {
	    tval[i] = i*maxtime/nbins;
	    ival[i] = 0.0;
	  }
	  
//	  theWaveForm->ResetIterator();
    G4int numberWaveforms = theWaveForm->GetVectorLength();
    for( G4int ii = 0; ii < numberWaveforms; ii++ )
	  {
	    G4double ampl = (*theWaveForm)(ii);
	    G4double decy = theWaveForm->Energy(ii);
      for (int i=0; i<nbins; i++)
      {
	      ival[i] += ampl*(1.0-exp(tval[i]/decy));
	    }
	  }

	  for( int i = 0; i < nbins; i++ )
	  {
	    ival[i]/= ival[nbins-1];
	  }

	  timeIntegral= new G4PhysicsOrderedFreeVector(tval, ival, nbins);
	  I_own_timeIntegral= 1;

	  // in Geant4.0.0, G4PhysicsOrderedFreeVector makes its own copy
	  // of any array passed to its constructor, so ...
	  delete [] tval;
	  delete [] ival;
	}
	
  }
  else
	{
	// use default integral (possibly null)
	timeIntegral = MyPhysicsTable::GetDefault()->GetEntry(i)->timeIntegral;
	I_own_timeIntegral= 0;

  }

    
    // Retrieve vector of scintillation "modifications"
    // for the material from the material's optical
    // properties table ("SCINTMOD")    
    //strncpy(property_string, ("SCINTMOD"+name).c_str(),
    //	      sizeof(property_string));

    sprintf(property_string, "SCINTMOD%s", name.c_str());
    G4MaterialPropertyVector* theScintModVector = aMaterialPropertiesTable->GetProperty(property_string);

    if (theScintModVector == NULL)
	{
      // use default if not particle-specific value given
      theScintModVector = aMaterialPropertiesTable->GetProperty("SCINTMOD");
    }
    
  if (theScintModVector)
  {

	// parse the entries in ScintMod
	//  ResolutionScale= ScintMod(0);
	//  BirksConstant= ScintMod(1);
	//  Ref_dE_dx= ScintMod(2);
	
    G4int numberScints = theScintModVector->GetVectorLength();
    for( G4int ii = 0; ii < numberScints; ii++ )
    {
      G4double value = (*theScintModVector)(ii);
      G4double key = theScintModVector->Energy(ii);

      if (key == 0.0)
      {
        resolutionScale= value;
      }
      else if (key == 1.0)
      {
        birksConstant= value;
      }
      else if (key == 2.0)
      {
        ref_dE_dx= value;
      }
      else
      {
              G4cerr
        << "GLG4Scint::MyPhysicsTable::Entry::Build"
          ":  Warning, unknown key " << key
        << "in SCINTMOD" << name << G4endl;
      }
    }
	
  }

}


void
GLG4Scint::SetNewValue(G4UIcommand * command, G4String newValues)
{
   G4String commandName= command -> GetCommandName();
   if (commandName == "on") {
     doScintillation= true;
   }
   else if (commandName == "off") {
     doScintillation= false;
   }
   else if (commandName == "reemission") {
     char *endptr;
     G4int i= strtol((const char *)newValues, &endptr, 0);
     if (*endptr != '\0')  // non-numerical argument
       {
	 if ( !(i = strcmp((const char *)newValues, "on"))) {
	   doReemission = true;
	 }
	 else if ( !(i = strcmp((const char *)newValues, "off"))) {
	   doReemission = false;
	 }
	 else {
	   G4cerr << "Command /glg4scint/reemission given unknown parameter "
		  << '\"' << newValues << '\"' << G4endl
		  << "  old value unchanged: "
		  << ( doReemission ? "on" : "off" ) << G4endl;
	 }
       }
     else {
       doReemission= (i != 0);
     }
   }
   else if (commandName == "maxTracksPerStep") {
     G4int i= strtol((const char *)newValues, NULL, 0);
     if (i > 0) {
       maxTracksPerStep= i;
     }
     else {
       G4cerr << "Value must be greater than 0, old value unchanged" << G4endl;
     }
   }
   else if (commandName == "meanPhotonsPerSecondary") {
     G4double d= strtod((const char *)newValues, NULL);
     if (d >= 1.0) {
       meanPhotonsPerSecondary= d;
     }
     else {
       G4cerr << "Value must be >= 1.0, old value unchanged" << G4endl;
     }
   }
   else if (commandName == "verbose") {
     verboseLevel= strtol((const char *)newValues, NULL, 0);
   }
   else if (commandName == "dump") {
     std::vector<GLG4Scint *>::iterator it = masterVectorOfGLG4Scint.begin();
     for (; it != masterVectorOfGLG4Scint.end(); it++) {
       (*it)->DumpInfo();
     }
   }
   else {
     G4cerr << "No GLG4Scint command named " << commandName << G4endl;
   }
   return;
}

G4String
GLG4Scint::GetCurrentValue(G4UIcommand * command)
{
   G4String commandName= command -> GetCommandName();
   if (commandName == "on" || commandName == "off") {
     return doScintillation ? "on" : "off";
   }
   else if (commandName == "reemission") {
     return doReemission ? "1" : "0";
   }
   else if (commandName == "maxTracksPerStep") {
     char outbuff[64];
     sprintf(outbuff, "%d", maxTracksPerStep);
     return G4String(outbuff);
   }
   else if (commandName == "meanPhotonsPerSecondary") {
     char outbuff[64];
     sprintf(outbuff, "%g", meanPhotonsPerSecondary);
     return G4String(outbuff);
   }
   else if (commandName == "verbose") {
     char outbuff[64];
     sprintf(outbuff, "%d", verboseLevel);
     return G4String(outbuff);
   }
   else if (commandName == "dump") {
     return "?/glg4scint/dump not supported";
   }
   else {
     return (commandName+" is not a valid GLG4Scint command");
   }
}
