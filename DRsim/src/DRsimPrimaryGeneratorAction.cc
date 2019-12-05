#include "DRsimPrimaryGeneratorAction.hh"

#include "G4Event.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4GenericMessenger.hh"
#include "G4SystemOfUnits.hh"
#include "Randomize.hh"
#include <cmath>

using namespace std;
DRsimPrimaryGeneratorAction::DRsimPrimaryGeneratorAction(G4int seed, G4String hepMCpath)
: G4VUserPrimaryGeneratorAction()
{
  fSeed = seed;
  fHepMCpath = hepMCpath;
  fTheta = -0.01111;
  fPhi = 0.;
  fRandX = 10.*mm;
  fRandY = 10.*mm;
  fY_0 = 0.;
  fZ_0 = 0.;
  fParticleGun = new G4ParticleGun(1);
  fHepMCAscii = new HepMCG4AsciiReader(fSeed,fHepMCpath);

  G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
  G4String particleName;
  fElectron = particleTable->FindParticle(particleName="e-");
  fPositron = particleTable->FindParticle(particleName="e+");
  fMuon = particleTable->FindParticle(particleName="mu+");
  fPion = particleTable->FindParticle(particleName="pi+");
  fKaon = particleTable->FindParticle(particleName="kaon+");
  fProton = particleTable->FindParticle(particleName="proton");
  fOptGamma = particleTable->FindParticle(particleName="opticalphoton");

  // define commands for this class
  DefineCommands();
}

DRsimPrimaryGeneratorAction::~DRsimPrimaryGeneratorAction() {
  delete fParticleGun;
  delete fHepMCAscii;
  delete fMessenger;
}

void DRsimPrimaryGeneratorAction::GeneratePrimaries(G4Event* event) {
  if (fHepMCpath.empty()) {
    y = (G4UniformRand()-0.5)*fRandX + fY_0;//- 3.142*cm;//
    z = (G4UniformRand()-0.5)*fRandY + fZ_0;//- 4.7135*cm;//10x10 mm^2
    org.set(0,y,z);

    fParticleGun->SetParticlePosition(org); // http://www.apc.univ-paris7.fr/~franco/g4doxy/html/classG4VPrimaryGenerator.html

    direction.setREtaPhi(1.,0.,0.);
    direction.rotateY(fTheta);
    direction.rotateZ(fPhi);

    fParticleGun->SetParticleMomentumDirection(direction);
    fParticleGun->GeneratePrimaryVertex(event);
  } else {
    fHepMCAscii->GeneratePrimaryVertex(event);
  }
}

void DRsimPrimaryGeneratorAction::DefineCommands() {
  // Define /DRsim/generator command directory using generic messenger class
  fMessenger = new G4GenericMessenger(this, "/DRsim/generator/", "Primary generator control");

  G4GenericMessenger::Command& etaCmd = fMessenger->DeclareMethodWithUnit("theta","rad",&DRsimPrimaryGeneratorAction::SetTheta,"theta of beam");
  etaCmd.SetParameterName("theta",true);
  etaCmd.SetDefaultValue("-0.01111");

  G4GenericMessenger::Command& phiCmd = fMessenger->DeclareMethodWithUnit("phi","rad",&DRsimPrimaryGeneratorAction::SetPhi,"phi of beam");
  phiCmd.SetParameterName("phi",true);
  phiCmd.SetDefaultValue("0.");

  G4GenericMessenger::Command& y0Cmd = fMessenger->DeclareMethodWithUnit("y0","cm",&DRsimPrimaryGeneratorAction::SetY0,"y_0 of beam");
  y0Cmd.SetParameterName("y0",true);
  y0Cmd.SetDefaultValue("0.");

  G4GenericMessenger::Command& z0Cmd = fMessenger->DeclareMethodWithUnit("z0","cm",&DRsimPrimaryGeneratorAction::SetZ0,"z_0 of beam");
  z0Cmd.SetParameterName("z0",true);
  z0Cmd.SetDefaultValue("0.");

  G4GenericMessenger::Command& randxCmd = fMessenger->DeclareMethodWithUnit("randx","mm",&DRsimPrimaryGeneratorAction::SetRandX,"x width of beam");
  randxCmd.SetParameterName("randx",true);
  randxCmd.SetDefaultValue("10.");

  G4GenericMessenger::Command& randyCmd = fMessenger->DeclareMethodWithUnit("randy","mm",&DRsimPrimaryGeneratorAction::SetRandY,"y width of beam");
  randyCmd.SetParameterName("randy",true);
  randyCmd.SetDefaultValue("10.");
}
