//
// Created by mikhail on 6/15/21.
//

#ifndef BOTVINA2HGEANT_SRC_MULTIPLICITY_H_
#define BOTVINA2HGEANT_SRC_MULTIPLICITY_H_

#include <fstream>
#include <iostream>

#include <TChain.h>
#include <TFile.h>
#include <TDatabasePDG.h>

#include <EventInitialState.h>
#include <TH1F.h>
#include <UEvent.h>
#include <UParticle.h>
#include <URun.h>

class Multiplicity {
public:
  Multiplicity() = default;
  ~Multiplicity() = default;
  void Init( const std::string&in_file_list, const std::string& out_file_name);
  void Run();
  void Save( const std::string& file_name );

private:
  void Exec();
  void AddFilesFromList(const std::string& list_name);
  void InitHistograms();

  // input tree fields
  TChain* mcini_chain_{nullptr};
  TFile* mcini_file_{ nullptr};
  UEvent *mcini_event_{nullptr};
  URun *mcini_run_info{nullptr};
  EventInitialState *mcini_initial_state_{nullptr};

  double beta_cm_;
  std::string file_out_;
  std::array<double, 2> theta_acceptance_;
  TH1F* multiplicity_{nullptr};
  TH1F* multiplicity_in_acceptance_{nullptr};
  TH1F* accepted_theta_distribution_{nullptr};
  TH1F* accepted_charge_distribution_{nullptr};
  TH1F* rapidity_distribution_{nullptr}; // to check if the Lorenz-boost is correct
};

#endif // BOTVINA2HGEANT_SRC_MULTIPLICITY_H_
