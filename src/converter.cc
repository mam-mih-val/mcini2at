//
// Created by mikhail on 6/15/21.
//

#include "converter.h"

void Converter::Init(const std::string&in_file_list, const std::string& out_file_name) {
  mcini_chain_ = new TChain("events");
  AddFilesFromList( in_file_list );
  mcini_event_ = new UEvent;
  mcini_chain_->SetBranchAddress("event", &mcini_event_);
  mcini_file_->GetObject( "run", mcini_run_info );

  out_file_ = TFile::Open( out_file_name.c_str(), "recreate" );
  out_tree_ = new TTree( "analysis_tree", "Analysis Tree, unigen data" );

  this->InitDataHeader();
  this->InitEventHeader();
  this->InitParticles();
  at_configuration_.Write("Configuration");
}

void Converter::Exec() {
  int evt_id =  mcini_event_->GetEventNr();
  int n_particles =  mcini_event_->GetNpa();
  float b = mcini_event_->GetB();
  float psi_rp = mcini_event_->GetPhi();

  at_event_header_->SetField( psi_rp, at_event_header_config_.GetFieldId("reaction_plane"));
  at_event_header_->SetField( b, at_event_header_config_.GetFieldId("impact_parameter"));
  at_event_header_->SetField( evt_id, at_event_header_config_.GetFieldId("event_id"));
  at_event_header_->SetField( n_particles, at_event_header_config_.GetFieldId("number_of_particles"));

  at_particles_->ClearChannels();

  auto x_id = at_particles_config_.GetFieldId("x");
  auto y_id = at_particles_config_.GetFieldId("y");
  auto z_id = at_particles_config_.GetFieldId("z");
  auto t_id = at_particles_config_.GetFieldId("t");

  for( int i=0; i<n_particles; ++i ){
    auto in_particle = mcini_event_->GetParticle(i);
    auto out_particle = at_particles_->AddChannel();
    out_particle->Init(at_particles_config_);
    int pdg_id = in_particle->GetPdg();
    auto mom4 = in_particle->GetMomentum();
    auto mass = mom4.M();
    auto particle = TDatabasePDG::Instance()->GetParticle( pdg_id );
    if( particle )
      mass = particle->Mass();
    auto px = mom4.Px();
    auto py = mom4.Py();
    auto pz = mom4.Pz();
    auto pos4 = in_particle->GetPosition();
    out_particle->SetPid(pdg_id);
    out_particle->SetMass(mass);
    out_particle->SetMomentum(px, py, pz);
    out_particle->SetField( (float) pos4.X(), x_id );
    out_particle->SetField( (float) pos4.Y(), y_id );
    out_particle->SetField( (float) pos4.Z(), z_id );
    out_particle->SetField( (float) pos4.T(), t_id );
  }
}

void Converter::InitEventHeader() {
  at_event_header_config_ = AnalysisTree::BranchConfig( "event_header", AnalysisTree::DetType::kEventHeader );
  at_event_header_config_.AddField<int>("event_id");
  at_event_header_config_.AddField<int>("number_of_particles");
  at_event_header_config_.AddField<float>("impact_parameter");
  at_event_header_config_.AddField<float>("reaction_plane");
  at_event_header_config_.AddField<float>("centrality");

  at_configuration_.AddBranchConfig(at_event_header_config_);
  at_event_header_ = new AnalysisTree::EventHeader(at_configuration_.GetNumberOfBranches()-1);
  at_event_header_->Init(at_event_header_config_);
  out_tree_->Branch("event_header", "AnalysisTree::EventHeader", &at_event_header_);
}

void Converter::InitParticles() {
  at_particles_config_ = AnalysisTree::BranchConfig( "particles", AnalysisTree::DetType::kParticle );
  at_particles_config_.AddField<float>("x");
  at_particles_config_.AddField<float>("y");
  at_particles_config_.AddField<float>("z");
  at_particles_config_.AddField<float>("t");

  at_configuration_.AddBranchConfig( at_particles_config_ );
  at_particles_ = new AnalysisTree::Particles( at_configuration_.GetNumberOfBranches()-1 );
  out_tree_->Branch( "particles", "AnalysisTree::Particles", &at_particles_ );
}
void Converter::Finish() {
  out_file_->cd();
  out_tree_->Write();
  out_file_->Close();
}
void Converter::Run() {
  for( int i=0; i<mcini_chain_->GetEntries(); ++i ){
    auto curr_file = out_tree_->GetCurrentFile();
    if( curr_file != out_file_ ){
      out_file_ = curr_file;
      at_configuration_.Write("Configuration");
      at_data_header_.Write("DataHeader");
    }
    mcini_chain_->GetEntry(i);
    Exec();
    out_tree_->Fill();
  }
  this->Finish();
}
void Converter::AddFilesFromList(const std::string& list_name) {
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
void Converter::InitDataHeader() {
  auto a_proj = mcini_run_info->GetAProj();
  auto z_proj = mcini_run_info->GetZProj();
  auto a_targ = mcini_run_info->GetATarg();
  auto z_targ = mcini_run_info->GetZTarg();
  TString t_generator;
  mcini_run_info->GetGenerator(t_generator);
  std::string generator = t_generator.Data();
  std::string system{ generator+" P("+std::to_string( a_proj )+", "+std::to_string(z_proj)+")+T("+std::to_string(a_targ)+" ,"+std::to_string(z_targ)+")" };
  at_data_header_.SetSystem(system);
  auto sqrt_snn = mcini_run_info->GetNNSqrtS();
  auto M = 0.938;
  auto T = sqrt_snn*sqrt_snn/(2*M) - 2*M;
  auto GAMMA = (T + M) / M;
  auto BETA = sqrt(1 - (M * M) / (M + T) / (M + T));
  auto PZ = M * BETA * GAMMA;
  at_data_header_.SetBeamMomentum( PZ );
  at_data_header_.Write("DataHeader");
}
