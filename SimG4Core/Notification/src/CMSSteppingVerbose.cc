
#include "SimG4Core/Notification/interface/CMSSteppingVerbose.h"
#include "G4Event.hh"
#include "G4Track.hh"
#include "G4TrackStatus.hh"
#include "G4Step.hh"
#include "G4SteppingManager.hh"
#include "G4ParticleDefinition.hh"
#include "G4VProcess.hh"
#include "G4SystemOfUnits.hh"

CMSSteppingVerbose::CMSSteppingVerbose(G4int verb, G4double ekin,
				       std::vector<G4int>& evtNum,
				       std::vector<G4int>& trNum)
  : m_PrintEvent(false),m_PrintTrack(false),m_verbose(verb),
    m_EventNumbers(evtNum),m_TrackNumbers(trNum),m_EkinThreshold(ekin)
{
  m_nEvents = m_EventNumbers.size();
  m_nTracks = m_TrackNumbers.size();
}

CMSSteppingVerbose::~CMSSteppingVerbose()
{}

void CMSSteppingVerbose::BeginOfEvent(const G4Event* evt)
{
  m_PrintEvent = false;
  if(0 >= m_verbose) { return; }
  if(m_nEvents == 0) { m_PrintEvent = true; }
  else {
    for(G4int i=0; i<m_nEvents; ++i) {
      if(evt->GetEventID() == m_EventNumbers[i]) {
	m_PrintEvent = true;
	break;
      }
    }
  }
  if(!m_PrintEvent) { return; }
  G4cout << "========== Event #" << evt->GetEventID() << " =============" << G4endl;
  G4cout << G4endl;
}

void CMSSteppingVerbose::TrackStarted(const G4Track* track, bool isKilled)
{
  m_PrintTrack = false;
  if(!m_PrintEvent) { return; }

  if(m_nTracks == 0) { 
    if(track->GetKineticEnergy() >= m_EkinThreshold) { m_PrintTrack = true; }
     
  } else {
    for(G4int i=0; i<m_nTracks; ++i) {
      if(track->GetTrackID() == m_TrackNumbers[i]) {
	m_PrintTrack = true;
	break;
      }
    }
  }
  if(!m_PrintTrack) { return; }

  G4cout << "*********************************************************************************************************" << G4endl;
  const G4ParticleDefinition* pd = track->GetDefinition();
  G4cout << "* G4Track Information:   Particle = ";
  if(pd) { G4cout << pd->GetParticleName(); }
  G4cout << ",   Track ID = " << track->GetTrackID() 
	 << ",   Parent ID = " << track->GetParentID() << G4endl;
  G4cout << "*********************************************************************************************************" << G4endl;

  G4cout << std::setw( 5) << "Step#"      << " "
	 << std::setw( 8) << "X(cm)"      << " "
	 << std::setw( 8) << "Y(cm)"      << " "  
	 << std::setw( 8) << "Z(cm)"      << " "
	 << std::setw( 9) << "KinE(GeV)"  << " "
	 << std::setw( 8) << "dE(MeV)"    << " "  
	 << std::setw( 8) << "Step(mm)"   << " "  
	 << std::setw( 9) << "TrackL(cm)"  << " "
	 << std::setw(30) << "PhysVolume" << " "
	 << std::setw( 8) << "ProcName"   << G4endl;	     

  G4int prec = G4cout.precision(4);

  G4cout << std::setw( 5) << track->GetCurrentStepNumber() << " "
	 << std::setw( 8) << track->GetPosition().x()/CLHEP::cm << " "
	 << std::setw( 8) << track->GetPosition().y()/CLHEP::cm << " "
	 << std::setw( 8) << track->GetPosition().z()/CLHEP::cm << " "
	 << std::setw( 9) << track->GetKineticEnergy()/CLHEP::GeV << " "
	 << std::setw( 8) << "         "
	 << std::setw( 8) << "         "
	 << std::setw( 9) << "          ";
  if( track->GetVolume() != nullptr ) { 
    G4cout << std::setw(30) << track->GetVolume()->GetName() << " ";
  } 
  if(isKilled) { G4cout << "isKilled"; }
  G4cout << G4endl;
  G4cout.precision(prec);
}

void CMSSteppingVerbose::TrackEnded(const G4Track* track) const
{
  if(!m_PrintTrack || 1 == m_verbose) { return; }
}

void CMSSteppingVerbose::StackFilled(const G4Track* track, bool isKilled) const
{
  if(2 >= m_verbose || !m_PrintTrack || 
     track->GetKineticEnergy() <  m_EkinThreshold ) { return; }
  G4int prec = G4cout.precision(4);

  G4cout << std::setw(10) << track->GetTrackID() << " "
	 << std::setw( 8) << track->GetPosition().x()/CLHEP::cm << " "
	 << std::setw( 8) << track->GetPosition().y()/CLHEP::cm << " "
	 << std::setw( 8) << track->GetPosition().z()/CLHEP::cm << " "
	 << std::setw( 9) << track->GetKineticEnergy()/CLHEP::GeV << " ";
  if( track->GetVolume() != nullptr ) { 
    G4cout << std::setw(24) << track->GetVolume()->GetName() << " ";
  } 
  if(isKilled) { G4cout << "isKilled"; }
  G4cout << G4endl;
  G4cout.precision(prec);  
}

void CMSSteppingVerbose::NextStep(const G4Step* step, 
				  const G4SteppingManager* sManager,
				  bool isKilled) const 
{
  if(!m_PrintTrack) { return; }

  G4int prec;
  const G4Track* track = step->GetTrack();
  const G4StepPoint* preStep = step->GetPreStepPoint();
  const G4StepPoint* postStep = step->GetPostStepPoint();
  
  if(3 <= m_verbose) {

    prec = G4cout.precision(16);

    G4cout << G4endl;
    G4cout << "    ++G4Step Information " << G4endl;
    G4cout << "      Address of G4Track    : " << track << G4endl;
    G4cout << "      Step Length (mm)      : " << track->GetStepLength() 
	   << G4endl;
    G4cout << "      Energy Deposit (MeV)  : " << step->GetTotalEnergyDeposit() 
	   << G4endl;

    G4cout << "   -------------------------------------------------------" 
	   << "-------------------------------" <<  G4endl;
    G4cout << "  StepPoint Information  " << std::setw(30) << "PreStep" 
	   << std::setw(30) << "PostStep" << G4endl;
    G4cout << "   -------------------------------------------------------" 
	   << "-------------------------------" <<  G4endl;
    G4cout << "      Position - x (cm)   : " 
	   << std::setw(30) << preStep->GetPosition().x()/CLHEP::cm 
	   << std::setw(30) << postStep->GetPosition().x()/CLHEP::cm << G4endl;
    G4cout << "      Position - y (cm)   : " 
	   << std::setw(30) << preStep->GetPosition().y()/CLHEP::cm 
	   << std::setw(30) << postStep->GetPosition().y()/CLHEP::cm << G4endl;
    G4cout << "      Position - z (cm)   : " 
	   << std::setw(30) << preStep->GetPosition().z()/CLHEP::cm 
	   << std::setw(30) << postStep->GetPosition().z()/CLHEP::cm << G4endl;
    G4cout << "      Global Time (ns)    : " 
	   << std::setw(30) << preStep->GetGlobalTime()/CLHEP::ns
	   << std::setw(30) << postStep->GetGlobalTime()/CLHEP::ns << G4endl;
    G4cout << "      Local Time (ns)     : " 
	   << std::setw(30) << preStep->GetLocalTime()/CLHEP::ns 
	   << std::setw(30) << postStep->GetLocalTime()/CLHEP::ns << G4endl;
    G4cout << "      Proper Time (ns)    : " 
	   << std::setw(30) << preStep->GetProperTime()/CLHEP::ns
	   << std::setw(30) << postStep->GetProperTime()/CLHEP::ns << G4endl;
    G4cout << "      Momentum Direct - x : " 
	   << std::setw(30) << preStep->GetMomentumDirection().x()
	   << std::setw(30) << postStep->GetMomentumDirection().x() 
	   << G4endl;
    G4cout << "      Momentum Direct - y : " 
	   << std::setw(30) << preStep->GetMomentumDirection().y()
	   << std::setw(30) << postStep->GetMomentumDirection().y() 
	   << G4endl;
    G4cout << "      Momentum Direct - z : " 
	   << std::setw(30) << preStep->GetMomentumDirection().z()
	   << std::setw(30) << postStep->GetMomentumDirection().z() 
	   << G4endl;
    G4cout << "      Momentum - x (GeV/c): " 
	   << std::setw(30) << preStep->GetMomentum().x()/CLHEP::GeV
	   << std::setw(30) << postStep->GetMomentum().x()/CLHEP::GeV << G4endl;
    G4cout << "      Momentum - y (GeV/c): " 
	   << std::setw(30) << preStep->GetMomentum().y()/CLHEP::GeV
	   << std::setw(30) << postStep->GetMomentum().y()/CLHEP::GeV << G4endl;
    G4cout << "      Momentum - z (GeV/c): " 
	   << std::setw(30) << preStep->GetMomentum().z()/CLHEP::GeV
	   << std::setw(30) << postStep->GetMomentum().z()/CLHEP::GeV << G4endl;
    G4cout << "      Total Energy (GeV)  : " 
	   << std::setw(30) << preStep->GetTotalEnergy()/CLHEP::GeV
	   << std::setw(30) << postStep->GetTotalEnergy()/CLHEP::GeV << G4endl;
    G4cout << "      Kinetic Energy (GeV): " 
	   << std::setw(30) << preStep->GetKineticEnergy()/CLHEP::GeV
	   << std::setw(30) << postStep->GetKineticEnergy()/CLHEP::GeV << G4endl;
    G4cout << "      Velocity (mm/ns)    : " 
	   << std::setw(30) << preStep->GetVelocity()
	   << std::setw(30) << postStep->GetVelocity() << G4endl;
    G4cout << "      Volume Name         : "
	   << std::setw(30) << preStep->GetPhysicalVolume()->GetName();
    G4String volName = (postStep->GetPhysicalVolume()) ?
      postStep->GetPhysicalVolume()->GetName() : "OutOfWorld";
      
    G4cout << std::setw(30) << volName << G4endl;
    G4cout << "      Safety (mm)         : " 
	   << std::setw(30) << preStep->GetSafety()
	   << std::setw(30) << postStep->GetSafety() << G4endl;
    G4cout << "      Polarization - x    : " 
	   << std::setw(30) << preStep->GetPolarization().x()
	   << std::setw(30) << postStep->GetPolarization().x() 
	   << G4endl;
    G4cout << "      Polarization - y    : " 
	   << std::setw(30) << preStep->GetPolarization().y()
	   << std::setw(30) << postStep->GetPolarization().y() 
	   << G4endl;
    G4cout << "      Polarization - Z    : " 
	   << std::setw(30) << preStep->GetPolarization().z()
	   << std::setw(30) << postStep->GetPolarization().z() 
	   << G4endl;
    G4cout << "      Weight              : " 
	   << std::setw(30) << preStep->GetWeight()
	   << std::setw(30) << postStep->GetWeight() << G4endl;
    G4cout << "      Step Status         : " ;
    G4StepStatus  tStepStatus = preStep->GetStepStatus();
    if( tStepStatus == fGeomBoundary ){
      G4cout << std::setw(30) << "Geom Limit";
    } else if ( tStepStatus == fAlongStepDoItProc ){
      G4cout << std::setw(30) << "AlongStep Proc.";
    } else if ( tStepStatus == fPostStepDoItProc ){
      G4cout << std::setw(30) << "PostStep Proc";
    } else if ( tStepStatus == fAtRestDoItProc ){
      G4cout << std::setw(30) << "AtRest Proc";
    } else if ( tStepStatus == fUndefined ){
      G4cout << std::setw(30) << "Undefined";
    }

    tStepStatus = postStep->GetStepStatus();
    if( tStepStatus == fGeomBoundary ){
      G4cout << std::setw(30) << "Geom Limit";
    } else if ( tStepStatus == fAlongStepDoItProc ){
      G4cout << std::setw(30) << "AlongStep Proc.";
    } else if ( tStepStatus == fPostStepDoItProc ){
      G4cout << std::setw(30) << "PostStep Proc";
    } else if ( tStepStatus == fAtRestDoItProc ){
      G4cout << std::setw(30) << "AtRest Proc";
    } else if ( tStepStatus == fUndefined ){
      G4cout << std::setw(30) << "Undefined";
    }

    G4cout << G4endl;
    G4cout << "      Process defined Step: " ;
    if( preStep->GetProcessDefinedStep() == 0 ){
      G4cout << std::setw(30) << "Undefined";
    } else {
      G4cout << std::setw(30) 
	     << preStep->GetProcessDefinedStep()->GetProcessName();
    }
    if( postStep->GetProcessDefinedStep() == 0){
      G4cout << std::setw(30) << "Undefined";
    } else {
      G4cout << std::setw(30) 
	     << postStep->GetProcessDefinedStep()->GetProcessName(); 
    }
    G4cout.precision(prec);

    G4cout << G4endl;
    G4cout << "   -------------------------------------------------------" 
	   << "-------------------------------" << G4endl;
  }

  // verbose == 1
  prec = G4cout.precision(4);
  G4cout << std::setw( 5) << track->GetCurrentStepNumber() << " "
	 << std::setw( 8) << track->GetPosition().x()/CLHEP::cm << " "
	 << std::setw( 8) << track->GetPosition().y()/CLHEP::cm << " "
	 << std::setw( 8) << track->GetPosition().z()/CLHEP::cm << " "
	 << std::setw( 9) << track->GetKineticEnergy()/CLHEP::GeV << " ";
  G4cout.precision(prec);
  prec = G4cout.precision(3);
  G4cout << std::setw( 8) << step->GetTotalEnergyDeposit()/CLHEP::MeV << " "
	 << std::setw( 8) << step->GetStepLength()/CLHEP::mm << " "
	 << std::setw( 9) << track->GetTrackLength()/CLHEP::cm << " ";

  G4bool endTracking = false;
  if( track->GetNextVolume() != nullptr ) { 
    G4cout << std::setw(30) << track->GetVolume()->GetName() << " ";
  } else {
    G4cout << std::setw(30) << "OutOfWorld" << " "; 
    endTracking = true;
  }
  if(isKilled) { 
    G4cout << "isKilled"; 
    endTracking = true;
  } else if(postStep->GetProcessDefinedStep() != nullptr){
    G4cout << postStep->GetProcessDefinedStep()->GetProcessName();
  } 
  G4cout << G4endl;
  G4cout.precision(prec);

  if(1 >= m_verbose) { return; }
  // verbose > 1
  if(!endTracking && fStopAndKill != track->GetTrackStatus()) { return; }

  prec = G4cout.precision(4);
  G4cout << "    ++List of " << step->GetSecondary()->size()
	 << " secondaries generated " << G4endl;
  const G4TrackVector* tv = sManager->GetSecondary();
  G4int n = tv->size();
  for(G4int i=0; i<n; ++i) {
    if((*tv)[i]->GetKineticEnergy() < m_EkinThreshold) { continue; }
    G4cout << "      ("
	   << std::setw( 9)
	   << (*tv)[i]->GetPosition().x()/CLHEP::cm << " "
	   << std::setw( 9)
	   << (*tv)[i]->GetPosition().y()/CLHEP::cm << " "
	   << std::setw( 9)
	   << (*tv)[i]->GetPosition().z()/CLHEP::cm << ") cm, "
	   << std::setw( 9)
	   << (*tv)[i]->GetKineticEnergy()/CLHEP::GeV << " GeV, "
	   << std::setw( 9)
	   << (*tv)[i]->GetGlobalTime()/CLHEP::ns << " ns, "
	   << std::setw(18)
	   << (*tv)[i]->GetDefinition()->GetParticleName() << G4endl;
  }
}
