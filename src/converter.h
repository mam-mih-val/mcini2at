//
// Created by mikhail on 6/15/21.
//

#ifndef BOTVINA2HGEANT_SRC_CONVERTER_H_
#define BOTVINA2HGEANT_SRC_CONVERTER_H_

#include <fstream>

#include <TChain.h>
#include <TFile.h>
#include <TDatabasePDG.h>

#include <AnalysisTree/DataHeader.hpp>
#include <AnalysisTree/Detector.hpp>
#include <AnalysisTree/EventHeader.hpp>
#include <AnalysisTree/Particle.hpp>
#include <AnalysisTree/Configuration.hpp>

#include <EventInitialState.h>
#include <TH1F.h>
#include <UEvent.h>
#include <UParticle.h>
#include <URun.h>

class Converter {
public:
  Converter() = default;
  ~Converter() = default;
  void Init( const std::string&in_file_list, const std::string& out_file_name);
  void Run();

private:
  void Exec();
  void Finish();
  void InitEventHeader();
  void InitParticles();
  void InitDataHeader();
  void AddFilesFromList(const std::string& list_name);
  void InitCentralityHisto();
  // output tree fields
  TFile* out_file_;
  TTree* out_tree_;
  AnalysisTree::Configuration at_configuration_;
  AnalysisTree::DataHeader at_data_header_;
  AnalysisTree::EventHeader* at_event_header_;
  AnalysisTree::Particles* at_particles_;
  AnalysisTree::BranchConfig at_event_header_config_;
  AnalysisTree::BranchConfig at_particles_config_;

  // input tree fields
  TChain* mcini_chain_{nullptr};
  TFile* mcini_file_{ nullptr};
  UEvent *mcini_event_{nullptr};
  URun *mcini_run_info{nullptr};
  EventInitialState *mcini_initial_state_{nullptr};
  double beta_cm_;

  TH1F* centrality_classes_{nullptr};

};

#endif // BOTVINA2HGEANT_SRC_CONVERTER_H_
