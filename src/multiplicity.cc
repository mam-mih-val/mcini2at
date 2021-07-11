//
// Created by mikhail on 6/15/21.
//

#include "multiplicity.h"

void Multiplicity::Init(const std::string&in_file_list, const std::string& out_file_name) {
  mcini_chain_ = new TChain("events");
  AddFilesFromList( in_file_list );
  mcini_event_ = new UEvent;
  mcini_chain_->SetBranchAddress("event", &mcini_event_);
  mcini_chain_->SetBranchAddress("iniState", &mcini_initial_state_);
  mcini_file_->GetObject( "run", mcini_run_info );
  file_out_ = out_file_name;
  InitHistograms();
  auto sqrt_snn = mcini_run_info->GetNNSqrtS();
  auto M = 0.938;
  beta_cm_ = sqrt( 1.0 - pow( 2*M/sqrt_snn, 2 ) );
}

void Multiplicity::Run() {
  for( int i=0; i<mcini_chain_->GetEntries(); ++i ){
    mcini_chain_->GetEntry(i);
    Exec();
  }
  Save(file_out_);
}

void Multiplicity::Exec() {
  int n_particles =  mcini_event_->GetNpa();
  multiplicity_->Fill( n_particles );
  int n_accepted=0;
  for( int i=0; i<n_particles; ++i ){
    auto in_particle = mcini_event_->GetParticle(i);
    auto mom4_lab = in_particle->GetMomentum();
    mom4_lab.Boost({0.0, 0.0, beta_cm_});
    auto theta = mom4_lab.Theta();
    rapidity_distribution_->Fill(mom4_lab.Rapidity());
    if( theta < TMath::DegToRad()*18.0 )
      continue;
    if( theta > TMath::DegToRad()*85.0 )
      continue;
    auto pid = in_particle->GetPdg();
    int charge=0;
    if(TDatabasePDG::Instance()->GetParticle(pid))
      charge = (int) TDatabasePDG::Instance()->GetParticle(pid)->Charge() / 3;
    if( charge != 0 ) {
      n_accepted++;
      accepted_theta_distribution_->Fill( theta*TMath::RadToDeg() );
      accepted_charge_distribution_->Fill( charge );
    }
  }
  multiplicity_in_acceptance_->Fill( n_accepted );
}

void Multiplicity::AddFilesFromList(const std::string& list_name) {
  std::ifstream file_list;
  file_list.open( list_name );
  std::string file_name;
  short line{0};
  while (std::getline( file_list, file_name)){
    if( !mcini_file_ )
      mcini_file_ = TFile::Open(file_name.c_str() );
    mcini_chain_->Add(file_name.c_str() );
    std::cout << line << " " << file_name
              << " has been added to a files sequence" << std::endl;
    line++;
  }
  std::cout << line << " files have been successfully added to a chain" << std::endl;
  std::cout << mcini_chain_->GetEntries() << " events in a chain" << std::endl;
  file_list.close();
}
void Multiplicity::InitHistograms() {
  multiplicity_ = new TH1F("produced_particles_total", "", 150,390., 540.);
  multiplicity_in_acceptance_ = new TH1F("accepted_charged", "", 300, 0., 300.);
  accepted_theta_distribution_ = new TH1F("accepted_theta_distribution", "#theta_{lab}", 180, 0.0, 180.0);; // to check if the Lorenz-boost is correct
  accepted_charge_distribution_ = new TH1F("accepted_charge_distribution", "", 20, -10.0, 10.0);; // to check if the Lorenz-boost is correct
  rapidity_distribution_ = new TH1F("rapidity", "", 150, -1.0, 3.0);; // to check if the Lorenz-boost is correct
}

void Multiplicity::Save(const std::string& file_name) {
  auto file = TFile::Open( file_name.c_str(), "recreate" );
  file->cd();
  multiplicity_->Write();
  multiplicity_in_acceptance_->Write();
  rapidity_distribution_->Write();
  accepted_theta_distribution_->Write();
  accepted_charge_distribution_->Write();
  file->Close();
}
