


void Sh

class Sh13PhysicsList: public G4VUserPhysicsList 
{ 
public: 
Sh13PhysicsList(); 
~Sh13PhysicsList(); 
void ConstructParticle(); 
void ConstructProcess(); 
void SetCuts();
};

void Sh13PhysicsList::ConstructParticle()
{

    /* Used radiation */
    G4Gamma::Definition();
    
}

void Sh13PhysicsList::ConstructProcess()
{

    /*  */
    AddTransportation();
    
}