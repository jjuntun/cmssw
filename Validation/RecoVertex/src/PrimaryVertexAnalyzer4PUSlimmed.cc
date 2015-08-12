#include "Validation/RecoVertex/interface/PrimaryVertexAnalyzer4PUSlimmed.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"

// reco track and vertex
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "RecoVertex/VertexPrimitives/interface/TransientVertex.h"

// TrackingParticle
#include "SimDataFormats/TrackingAnalysis/interface/TrackingParticleFwd.h"
#include "SimDataFormats/TrackingAnalysis/interface/TrackingVertexContainer.h"

// associator
#include "SimTracker/Records/interface/TrackAssociatorRecord.h"
#include "SimTracker/VertexAssociation/interface/calculateVertexSharedTracks.h"

// DQM
#include "DQMServices/Core/interface/MonitorElement.h"

#include <numeric>


#include <string>
#include <sstream>

//
// constructors and destructor
//
PrimaryVertexAnalyzer4PUSlimmed::PrimaryVertexAnalyzer4PUSlimmed(
    const edm::ParameterSet& iConfig)
    : verbose_(iConfig.getUntrackedParameter<bool>("verbose", false)),
      use_only_charged_tracks_(iConfig.getUntrackedParameter<bool>(
          "use_only_charged_tracks", true)),
      sigma_z_match_(
          iConfig.getUntrackedParameter<double>("sigma_z_match", 3.0)),
      abs_z_match_(
          iConfig.getUntrackedParameter<double>("abs_z_match", 0.1)),
      root_folder_(
          iConfig.getUntrackedParameter<std::string>("root_folder",
                                                "Validation/Vertices")),
      doDetailedHistograms_(iConfig.getUntrackedParameter<bool>("doDetailedHistograms", false)),
      vecPileupSummaryInfoToken_(consumes<std::vector<PileupSummaryInfo> >(
          edm::InputTag(std::string("addPileupInfo")))),
      recoTrackCollectionToken_(consumes<reco::TrackCollection>(
          iConfig.getUntrackedParameter<edm::InputTag>("recoTrackProducer"))),
      edmView_recoTrack_Token_(consumes<edm::View<reco::Track> >(
          iConfig.getUntrackedParameter<edm::InputTag>("recoTrackProducer"))),
      trackingParticleCollectionToken_(consumes<TrackingParticleCollection>(
          iConfig.getUntrackedParameter<edm::InputTag>("trackingParticleCollection"))),
      trackingVertexCollectionToken_(consumes<TrackingVertexCollection>(
          iConfig.getUntrackedParameter<edm::InputTag>("trackingVertexCollection"))),
      simToRecoAssociationToken_(consumes<reco::SimToRecoCollection>(
          iConfig.getUntrackedParameter<edm::InputTag>("trackAssociatorMap"))),
      recoToSimAssociationToken_(consumes<reco::RecoToSimCollection>(
          iConfig.getUntrackedParameter<edm::InputTag>("trackAssociatorMap"))),
      vertexAssociatorToken_(consumes<reco::VertexToTrackingVertexAssociator>(
          iConfig.getUntrackedParameter<edm::InputTag>("vertexAssociator"))) {
  reco_vertex_collections_ = iConfig.getParameter<std::vector<edm::InputTag> >(
      "vertexRecoCollections");
  for (auto const& l : reco_vertex_collections_) {
    reco_vertex_collection_tokens_.push_back(
        edm::EDGetTokenT<edm::View<reco::Vertex>>(
            consumes<edm::View<reco::Vertex>>(l)));
  }
}

PrimaryVertexAnalyzer4PUSlimmed::~PrimaryVertexAnalyzer4PUSlimmed() {}

//
// member functions
//
void PrimaryVertexAnalyzer4PUSlimmed::bookHistograms(
    DQMStore::IBooker& i, edm::Run const& iRun, edm::EventSetup const& iSetup) {
  // TODO(rovere) make this booking method shorter and smarter,
  // factorizing similar histograms with different prefix in a single
  // method call.
  float log_bins[31] = {
    0.0, 0.0002, 0.0004, 0.0006, 0.0008, 0.001, 0.002,
    0.004, 0.006, 0.008, 0.01, 0.02,
    0.04, 0.06, 0.08, 0.1, 0.2,
    0.4, 0.6, 0.8, 1.0, 2.0,
    4.0, 6.0, 8.0, 10.0, 20.0,
    40.0, 60.0, 80.0, 100.0
  };
  float log_mergez_bins[18] = {
    0.0, 0.0025, 0.005, 0.0075, 0.01, 0.025, 0.05, 0.075, 0.1,
    0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0
  };
  float log_pt2_bins[16] = {
    0.0, 0.1, 0.5, 
    1.0, 2.0, 5.0,
    10.0, 20.0, 50.0,
    100.0, 200.0, 500.0, 
    1000.0, 2000.0, 5000.0,10000.0
  };
  float log_ntrk_bins[25] = {
    0., 2.0,  4.0,  6.0,  8.0,  10.,
    12.0, 14.0, 16.0, 18.0, 22.0,
    26.0, 30.0, 35.0, 40.0,
    45.0, 50.0, 55.0, 60.0, 70.0,
    80.0, 90.0, 100.0, 150.0, 200.0
  };
  // TODO(rovere) Possibly change or add the main DQMStore booking
  // interface to allow booking a TProfile with variable bin-width
  // using an array of floats, as done for the TH1F case, not of
  // doubles.
  double log_pt2_bins_double[16] = {
    0.0, 0.1, 0.5, 
    1.0, 2.0, 5.0,
    10.0, 20.0, 50.0,
    100.0, 200.0, 500.0, 
    1000.0, 2000.0, 5000.0,10000.0
  };

  i.setCurrentFolder(root_folder_);
  mes_["root_folder"]["GenVtx_vs_BX"] =
      i.book2D("GenVtx_vs_BX", "GenVtx_vs_BX", 16, -12.5, 3.5, 200, 0., 200.);
  // Generated Primary Vertex Plots
  mes_["root_folder"]["GenPV_X"] =
      i.book1D("GenPV_X", "GeneratedPV_X", 120, -0.6, 0.6);
  mes_["root_folder"]["GenPV_Y"] =
      i.book1D("GenPV_Y", "GeneratedPV_Y", 120, -0.6, 0.6);
  mes_["root_folder"]["GenPV_Z"] =
      i.book1D("GenPV_Z", "GeneratedPV_Z", 120, -60., 60.);
  mes_["root_folder"]["GenPV_R"] =
      i.book1D("GenPV_R", "GeneratedPV_R", 120, 0, 0.6);
  mes_["root_folder"]["GenPV_Pt2"] =
      i.book1D("GenPV_Pt2", "GeneratedPV_Sum-pt2", 15, &log_pt2_bins[0]);
  mes_["root_folder"]["GenPV_NumTracks"] =
      i.book1D("GenPV_NumTracks", "GeneratedPV_NumTracks", 24, &log_ntrk_bins[0]);
  mes_["root_folder"]["GenPV_ClosestDistanceZ"] =
      i.book1D("GenPV_ClosestDistanceZ", "GeneratedPV_ClosestDistanceZ", 30,
               &log_bins[0]);

  // All Generated Vertices, used for efficiency plots
  mes_["root_folder"]["GenAllV_NumVertices"] = i.book1D(
      "GenAllV_NumVertices", "GeneratedAllV_NumVertices", 100, 0., 200.);
  mes_["root_folder"]["GenAllV_X"] =
      i.book1D("GenAllV_X", "GeneratedAllV_X", 120, -0.6, 0.6);
  mes_["root_folder"]["GenAllV_Y"] =
      i.book1D("GenAllV_Y", "GeneratedAllV_Y", 120, -0.6, 0.6);
  mes_["root_folder"]["GenAllV_Z"] =
      i.book1D("GenAllV_Z", "GeneratedAllV_Z", 120, -60, 60);
  mes_["root_folder"]["GenAllV_R"] =
      i.book1D("GenAllV_R", "GeneratedAllV_R", 120, 0, 0.6);
  mes_["root_folder"]["GenAllV_Pt2"] =
      i.book1D("GenAllV_Pt2", "GeneratedAllV_Sum-pt2", 15, &log_pt2_bins[0]);
  mes_["root_folder"]["GenAllV_NumTracks"] =
      i.book1D("GenAllV_NumTracks", "GeneratedAllV_NumTracks", 24, &log_ntrk_bins[0]);
  mes_["root_folder"]["GenAllV_ClosestDistanceZ"] =
      i.book1D("GenAllV_ClosestDistanceZ", "GeneratedAllV_ClosestDistanceZ", 30,
               &log_bins[0]);
  mes_["root_folder"]["GenAllV_PairDistanceZ"] =
      i.book1D("GenAllV_PairDistanceZ", "GeneratedAllV_PairDistanceZ",
               1000, 0, 20);
  mes_["root_folder"]["SignalIsHighestPt2"] =
        i.book1D("SignalIsHighestPt2", "SignalIsHighestPt2", 2, -0.5, 1.5);

  for (auto const& l : reco_vertex_collections_) {
    std::string label = l.label();
    std::string current_folder = root_folder_ + "/" + label;
    i.setCurrentFolder(current_folder);

    mes_[label]["RecoVtx_vs_GenVtx"] = i.bookProfile(
        "RecoVtx_vs_GenVtx", "RecoVtx_vs_GenVtx", 125, 0., 250., 250, 0., 250.);
    mes_[label]["MatchedRecoVtx_vs_GenVtx"] =
        i.bookProfile("MatchedRecoVtx_vs_GenVtx", "MatchedRecoVtx_vs_GenVtx",
                      125, 0., 250., 250, 0., 250.);
    mes_[label]["KindOfSignalPV"] =
        i.book1D("KindOfSignalPV", "KindOfSignalPV", 9, -0.5, 8.5);
    mes_[label]["KindOfSignalPV"]->getTH1()->GetXaxis()->SetBinLabel(1, "!Highest!Assoc2Any");
    mes_[label]["KindOfSignalPV"]->getTH1()->GetXaxis()->SetBinLabel(2, "Highest!Assoc2Any");
    mes_[label]["KindOfSignalPV"]->getTH1()->GetXaxis()->SetBinLabel(3, "!HighestAssoc2First");
    mes_[label]["KindOfSignalPV"]->getTH1()->GetXaxis()->SetBinLabel(4, "HighestAssoc2First");
    mes_[label]["KindOfSignalPV"]->getTH1()->GetXaxis()->SetBinLabel(5, "!HighestAssoc2!First");
    mes_[label]["KindOfSignalPV"]->getTH1()->GetXaxis()->SetBinLabel(6, "HighestAssoc2!First");
    mes_[label]["KindOfSignalPV"]->getTH1()->GetXaxis()->SetBinLabel(7, "!HighestAssoc2First");
    mes_[label]["KindOfSignalPV"]->getTH1()->GetXaxis()->SetBinLabel(8, "HighestAssoc2First");
    mes_[label]["MisTagRate"] =
        i.book1D("MisTagRate", "MisTagRate", 2, -0.5, 1.5);
    mes_[label]["MisTagRate_vs_PU"] =
        i.bookProfile("MisTagRate_vs_PU", "MisTagRate_vs_PU", 125, 0., 250.,
                      2, 0., 1.);
    mes_[label]["MisTagRate_vs_sum-pt2"] =
        i.bookProfile("MisTagRate_vs_sum-pt2", "MisTagRate_vs_sum-pt2",
                      15, &log_pt2_bins_double[0], 2, 0., 1.);
    mes_[label]["MisTagRate_vs_Z"] =
        i.bookProfile("MisTagRate_vs_Z", "MisTagRate_vs_Z",
                      120, -60., 60., 2, 0., 1.);
    mes_[label]["MisTagRate_vs_R"] =
        i.bookProfile("MisTagRate_vs_R", "MisTagRate_vs_R",
                      120, 0., 0.6, 2, 0., 1.);
    mes_[label]["MisTagRate_vs_NumTracks"] =
        i.bookProfile("MisTagRate_vs_NumTracks", "MisTagRate_vs_NumTracks",
                      100, 0., 200, 2, 0., 1.);
    mes_[label]["MisTagRateSignalIsHighest"] =
        i.book1D("MisTagRateSignalIsHighest",
                 "MisTagRateSignalIsHighest", 2, -0.5, 1.5);
    mes_[label]["MisTagRateSignalIsHighest_vs_PU"] =
        i.bookProfile("MisTagRateSignalIsHighest_vs_PU",
                      "MisTagRateSignalIsHighest_vs_PU", 125, 0., 250.,
                      2, 0., 1.);
    mes_[label]["MisTagRateSignalIsHighest_vs_sum-pt2"] =
        i.bookProfile("MisTagRateSignalIsHighest_vs_sum-pt2",
                      "MisTagRateSignalIsHighest_vs_sum-pt2",
                      15, &log_pt2_bins_double[0], 2, 0., 1.);
    mes_[label]["MisTagRateSignalIsHighest_vs_Z"] =
        i.bookProfile("MisTagRateSignalIsHighest_vs_Z",
                      "MisTagRateSignalIsHighest_vs_Z",
                      120, -60., 60., 2, 0., 1.);
    mes_[label]["MisTagRateSignalIsHighest_vs_R"] =
        i.bookProfile("MisTagRateSignalIsHighest_vs_R",
                      "MisTagRateSignalIsHighest_vs_R",
                      120, 0., 0.6, 2, 0., 1.);
    mes_[label]["MisTagRateSignalIsHighest_vs_NumTracks"] =
        i.bookProfile("MisTagRateSignalIsHighest_vs_NumTracks",
                      "MisTagRateSignalIsHighest_vs_NumTracks",
                      100, 0., 200, 2, 0., 1.);
    mes_[label]["MisTagRateSignalIsNotHighest"] =
        i.book1D("MisTagRateSignalIsNotHighest",
                 "MisTagRateSignalIsNotHighest", 2, -0.5, 1.5);
    mes_[label]["MisTagRateSignalIsNotHighest_vs_PU"] =
        i.bookProfile("MisTagRateSignalIsNotHighest_vs_PU",
                      "MisTagRateSignalIsNotHighest_vs_PU", 125, 0., 250.,
                      2, 0., 1.);
    mes_[label]["MisTagRateSignalIsNotHighest_vs_sum-pt2"] =
        i.bookProfile("MisTagRateSignalIsNotHighest_vs_sum-pt2",
                      "MisTagRateSignalIsNotHighest_vs_sum-pt2",
                      15, &log_pt2_bins_double[0], 2, 0., 1.);
    mes_[label]["MisTagRateSignalIsNotHighest_vs_Z"] =
        i.bookProfile("MisTagRateSignalIsNotHighest_vs_Z",
                      "MisTagRateSignalIsNotHighest_vs_Z",
                      120, -60., 60., 2, 0., 1.);
    mes_[label]["MisTagRateSignalIsNotHighest_vs_R"] =
        i.bookProfile("MisTagRateSignalIsNotHighest_vs_R",
                      "MisTagRateSignalIsNotHighest_vs_R",
                      120, 0., 0.6, 2, 0., 1.);
    mes_[label]["MisTagRateSignalIsNotHighest_vs_NumTracks"] =
        i.bookProfile("MisTagRateSignalIsNotHighest_vs_NumTracks",
                      "MisTagRateSignalIsNotHighest_vs_NumTracks",
                      100, 0., 200, 2, 0., 1.);
    mes_[label]["TruePVLocationIndex"] =
        i.book1D("TruePVLocationIndex",
                 "TruePVLocationIndexInRecoVertexCollection", 12, -1.5, 10.5);
    mes_[label]["TruePVLocationIndexCumulative"] =
        i.book1D("TruePVLocationIndexCumulative",
                 "TruePVLocationIndexInRecoVertexCollectionCumulative",
                 3, -1.5, 1.5);
    mes_[label]["TruePVLocationIndexSignalIsHighest"] =
        i.book1D("TruePVLocationIndexSignalIsHighest",
                 "TruePVLocationIndexSignalIsHighestInRecoVertexCollection",
                 12, -1.5, 10.5);
    mes_[label]["TruePVLocationIndexSignalIsNotHighest"] =
        i.book1D("TruePVLocationIndexSignalIsNotHighest",
                 "TruePVLocationIndexSignalIsNotHighestInRecoVertexCollection",
                 12, -1.5, 10.5);


    // All Generated Vertices. Used for Efficiency plots We kind of
    // duplicate plots here in case we want to perform more detailed
    // studies on a selection of generated vertices, not on all of them.
    mes_[label]["GenAllAssoc2Reco_NumVertices"] =
        i.book1D("GenAllAssoc2Reco_NumVertices",
                 "GeneratedAllAssoc2Reco_NumVertices", 100, 0., 200.);
    mes_[label]["GenAllAssoc2Reco_X"] = i.book1D(
        "GenAllAssoc2Reco_X", "GeneratedAllAssoc2Reco_X", 120, -0.6, 0.6);
    mes_[label]["GenAllAssoc2Reco_Y"] = i.book1D(
        "GenAllAssoc2Reco_Y", "GeneratedAllAssoc2Reco_Y", 120, -0.6, 0.6);
    mes_[label]["GenAllAssoc2Reco_Z"] = i.book1D(
        "GenAllAssoc2Reco_Z", "GeneratedAllAssoc2Reco_Z", 120, -60, 60);
    mes_[label]["GenAllAssoc2Reco_R"] =
        i.book1D("GenAllAssoc2Reco_R", "GeneratedAllAssoc2Reco_R", 120, 0, 0.6);
    mes_[label]["GenAllAssoc2Reco_Pt2"] =
        i.book1D("GenAllAssoc2Reco_Pt2", "GeneratedAllAssoc2Reco_Sum-pt2", 15,
                 &log_pt2_bins[0]);
    mes_[label]["GenAllAssoc2Reco_NumTracks"] =
        i.book1D("GenAllAssoc2Reco_NumTracks",
                 "GeneratedAllAssoc2Reco_NumTracks", 24, &log_ntrk_bins[0]);
    mes_[label]["GenAllAssoc2Reco_ClosestDistanceZ"] =
        i.book1D("GenAllAssoc2Reco_ClosestDistanceZ",
                 "GeneratedAllAssoc2Reco_ClosestDistanceZ", 30, &log_bins[0]);

    // All Generated Vertices Matched to a Reconstructed vertex. Used
    // for Efficiency plots
    mes_[label]["GenAllAssoc2RecoMatched_NumVertices"] =
        i.book1D("GenAllAssoc2RecoMatched_NumVertices",
                 "GeneratedAllAssoc2RecoMatched_NumVertices", 100, 0., 200.);
    mes_[label]["GenAllAssoc2RecoMatched_X"] =
        i.book1D("GenAllAssoc2RecoMatched_X", "GeneratedAllAssoc2RecoMatched_X",
                 120, -0.6, 0.6);
    mes_[label]["GenAllAssoc2RecoMatched_Y"] =
        i.book1D("GenAllAssoc2RecoMatched_Y", "GeneratedAllAssoc2RecoMatched_Y",
                 120, -0.6, 0.6);
    mes_[label]["GenAllAssoc2RecoMatched_Z"] =
        i.book1D("GenAllAssoc2RecoMatched_Z", "GeneratedAllAssoc2RecoMatched_Z",
                 120, -60, 60);
    mes_[label]["GenAllAssoc2RecoMatched_R"] =
        i.book1D("GenAllAssoc2RecoMatched_R", "GeneratedAllAssoc2RecoMatched_R",
                 120, 0, 0.6);
    mes_[label]["GenAllAssoc2RecoMatched_Pt2"] =
        i.book1D("GenAllAssoc2RecoMatched_Pt2",
                 "GeneratedAllAssoc2RecoMatched_Sum-pt2", 15, &log_pt2_bins[0]);
    mes_[label]["GenAllAssoc2RecoMatched_NumTracks"] =
        i.book1D("GenAllAssoc2RecoMatched_NumTracks",
                 "GeneratedAllAssoc2RecoMatched_NumTracks", 24, &log_ntrk_bins[0]);
    mes_[label]["GenAllAssoc2RecoMatched_ClosestDistanceZ"] = i.book1D(
        "GenAllAssoc2RecoMatched_ClosestDistanceZ",
        "GeneratedAllAssoc2RecoMatched_ClosestDistanceZ", 30, &log_bins[0]);

    // All Generated Vertices Multi-Matched to a Reconstructed vertex. Used
    // for Duplicate rate plots
    mes_[label]["GenAllAssoc2RecoMultiMatched_NumVertices"] = i.book1D(
        "GenAllAssoc2RecoMultiMatched_NumVertices",
        "GeneratedAllAssoc2RecoMultiMatched_NumVertices", 100, 0., 200.);
    mes_[label]["GenAllAssoc2RecoMultiMatched_X"] =
        i.book1D("GenAllAssoc2RecoMultiMatched_X",
                 "GeneratedAllAssoc2RecoMultiMatched_X", 120, -0.6, 0.6);
    mes_[label]["GenAllAssoc2RecoMultiMatched_Y"] =
        i.book1D("GenAllAssoc2RecoMultiMatched_Y",
                 "GeneratedAllAssoc2RecoMultiMatched_Y", 120, -0.6, 0.6);
    mes_[label]["GenAllAssoc2RecoMultiMatched_Z"] =
        i.book1D("GenAllAssoc2RecoMultiMatched_Z",
                 "GeneratedAllAssoc2RecoMultiMatched_Z", 120, -60, 60);
    mes_[label]["GenAllAssoc2RecoMultiMatched_R"] =
        i.book1D("GenAllAssoc2RecoMultiMatched_R",
                 "GeneratedAllAssoc2RecoMultiMatched_R", 120, 0, 0.6);
    mes_[label]["GenAllAssoc2RecoMultiMatched_Pt2"] =
        i.book1D("GenAllAssoc2RecoMultiMatched_Pt2",
                 "GeneratedAllAssoc2RecoMultiMatched_Sum-pt2",
                 15, &log_pt2_bins[0]);
    mes_[label]["GenAllAssoc2RecoMultiMatched_NumTracks"] =
        i.book1D("GenAllAssoc2RecoMultiMatched_NumTracks",
                 "GeneratedAllAssoc2RecoMultiMatched_NumTracks", 24, &log_ntrk_bins[0]);
    mes_[label]["GenAllAssoc2RecoMultiMatched_ClosestDistanceZ"] = i.book1D(
        "GenAllAssoc2RecoMultiMatched_ClosestDistanceZ",
        "GeneratedAllAssoc2RecoMultiMatched_ClosestDistanceZ",
        30, &log_bins[0]);

    // All Reco Vertices. Used for {Fake,Duplicate}-Rate plots
    
    mes_[label]["RecoAllAssoc2Gen_NumVertices"] =
        i.book1D("RecoAllAssoc2Gen_NumVertices",
                 "ReconstructedAllAssoc2Gen_NumVertices", 100, 0., 200.);
    mes_[label]["RecoAllAssoc2Gen_X"] = i.book1D(
        "RecoAllAssoc2Gen_X", "ReconstructedAllAssoc2Gen_X", 120, -0.6, 0.6);
    mes_[label]["RecoAllAssoc2Gen_Y"] = i.book1D(
        "RecoAllAssoc2Gen_Y", "ReconstructedAllAssoc2Gen_Y", 120, -0.6, 0.6);
    mes_[label]["RecoAllAssoc2Gen_Z"] = i.book1D(
        "RecoAllAssoc2Gen_Z", "ReconstructedAllAssoc2Gen_Z", 120, -60, 60);
    mes_[label]["RecoAllAssoc2Gen_R"] = i.book1D(
        "RecoAllAssoc2Gen_R", "ReconstructedAllAssoc2Gen_R", 120, 0, 0.6);
    mes_[label]["RecoAllAssoc2Gen_Pt2"] =
        i.book1D("RecoAllAssoc2Gen_Pt2", "ReconstructedAllAssoc2Gen_Sum-pt2",
                 15, &log_pt2_bins[0]);
    mes_[label]["RecoAllAssoc2Gen_Ndof"] =
        i.book1D("RecoAllAssoc2Gen_Ndof",
                 "ReconstructedAllAssoc2Gen_Ndof", 120, 0., 240.);
    mes_[label]["RecoAllAssoc2Gen_NumTracks"] =
        i.book1D("RecoAllAssoc2Gen_NumTracks",
                 "ReconstructedAllAssoc2Gen_NumTracks", 24, &log_ntrk_bins[0]);
    mes_[label]["RecoAllAssoc2Gen_PU"] =
        i.book1D("RecoAllAssoc2Gen_PU",
                 "ReconstructedAllAssoc2Gen_PU", 125, 0., 250.);
    mes_[label]["RecoAllAssoc2Gen_ClosestDistanceZ"] =
        i.book1D("RecoAllAssoc2Gen_ClosestDistanceZ",
                 "ReconstructedAllAssoc2Gen_ClosestDistanceZ",
                 30, &log_bins[0]);
    mes_[label]["RecoAllAssoc2GenProperties"] =
        i.book1D("RecoAllAssoc2GenProperties",
                 "ReconstructedAllAssoc2Gen_Properties", 8, -0.5, 7.5);
    mes_[label]["RecoAllAssoc2Gen_PairDistanceZ"] =
        i.book1D("RecoAllAssoc2Gen_PairDistanceZ",
                 "RecoAllAssoc2Gen_PairDistanceZ", 1000, 0, 20);

    // All Reconstructed Vertices Matched to a Generated vertex. Used
    // for Fake-Rate plots
    mes_[label]["RecoAllAssoc2GenMatched_NumVertices"] =
        i.book1D("RecoAllAssoc2GenMatched_NumVertices",
                 "ReconstructedAllAssoc2GenMatched_NumVertices", 100, 0., 200.);
    mes_[label]["RecoAllAssoc2GenMatched_X"] =
        i.book1D("RecoAllAssoc2GenMatched_X",
                 "ReconstructedAllAssoc2GenMatched_X", 120, -0.6, 0.6);
    mes_[label]["RecoAllAssoc2GenMatched_Y"] =
        i.book1D("RecoAllAssoc2GenMatched_Y",
                 "ReconstructedAllAssoc2GenMatched_Y", 120, -0.6, 0.6);
    mes_[label]["RecoAllAssoc2GenMatched_Z"] =
        i.book1D("RecoAllAssoc2GenMatched_Z",
                 "ReconstructedAllAssoc2GenMatched_Z", 120, -60, 60);
    mes_[label]["RecoAllAssoc2GenMatched_R"] =
        i.book1D("RecoAllAssoc2GenMatched_R",
                 "ReconstructedAllAssoc2GenMatched_R", 120, 0, 0.6);
    mes_[label]["RecoAllAssoc2GenMatched_Pt2"] =
        i.book1D("RecoAllAssoc2GenMatched_Pt2",
                 "ReconstructedAllAssoc2GenMatched_Sum-pt2",
                 15, &log_pt2_bins[0]);
    mes_[label]["RecoAllAssoc2GenMatched_Ndof"] =
        i.book1D("RecoAllAssoc2GenMatched_Ndof",
                 "ReconstructedAllAssoc2GenMatched_Ndof", 120, 0., 240.);
    mes_[label]["RecoAllAssoc2GenMatched_NumTracks"] =
        i.book1D("RecoAllAssoc2GenMatched_NumTracks",
                 "ReconstructedAllAssoc2GenMatched_NumTracks", 24, &log_ntrk_bins[0]);
    mes_[label]["RecoAllAssoc2GenMatched_PU"] =
        i.book1D("RecoAllAssoc2GenMatched_PU",
                 "ReconstructedAllAssoc2GenMatched_PU", 125, 0., 250.);
    mes_[label]["RecoAllAssoc2GenMatched_ClosestDistanceZ"] = i.book1D(
        "RecoAllAssoc2GenMatched_ClosestDistanceZ",
        "ReconstructedAllAssoc2GenMatched_ClosestDistanceZ", 30, &log_bins[0]);

    // All Reconstructed Vertices  Multi-Matched to a Generated vertex. Used
    // for Merge-Rate plots
    mes_[label]["RecoAllAssoc2GenMultiMatched_NumVertices"] = i.book1D(
        "RecoAllAssoc2GenMultiMatched_NumVertices",
        "ReconstructedAllAssoc2GenMultiMatched_NumVertices", 100, 0., 200.);
    mes_[label]["RecoAllAssoc2GenMultiMatched_X"] =
        i.book1D("RecoAllAssoc2GenMultiMatched_X",
                 "ReconstructedAllAssoc2GenMultiMatched_X", 120, -0.6, 0.6);
    mes_[label]["RecoAllAssoc2GenMultiMatched_Y"] =
        i.book1D("RecoAllAssoc2GenMultiMatched_Y",
                 "ReconstructedAllAssoc2GenMultiMatched_Y", 120, -0.6, 0.6);
    mes_[label]["RecoAllAssoc2GenMultiMatched_Z"] =
        i.book1D("RecoAllAssoc2GenMultiMatched_Z",
                 "ReconstructedAllAssoc2GenMultiMatched_Z", 120, -60, 60);
    mes_[label]["RecoAllAssoc2GenMultiMatched_R"] =
        i.book1D("RecoAllAssoc2GenMultiMatched_R",
                 "ReconstructedAllAssoc2GenMultiMatched_R", 120, 0, 0.6);
    mes_[label]["RecoAllAssoc2GenMultiMatched_Pt2"] = i.book1D(
        "RecoAllAssoc2GenMultiMatched_Pt2",
        "ReconstructedAllAssoc2GenMultiMatched_Sum-pt2", 15, &log_pt2_bins[0]);
    mes_[label]["RecoAllAssoc2GenMultiMatched_NumTracks"] = i.book1D(
        "RecoAllAssoc2GenMultiMatched_NumTracks",
        "ReconstructedAllAssoc2GenMultiMatched_NumTracks", 24, &log_ntrk_bins[0]);
    mes_[label]["RecoAllAssoc2GenMultiMatched_PU"] =
        i.book1D("RecoAllAssoc2GenMultiMatched_PU",
                 "ReconstructedAllAssoc2GenMultiMatched_PU", 125, 0., 250.);
    mes_[label]["RecoAllAssoc2GenMultiMatched_ClosestDistanceZ"] =
        i.book1D("RecoAllAssoc2GenMultiMatched_ClosestDistanceZ",
                 "ReconstructedAllAssoc2GenMultiMatched_ClosestDistanceZ",
                 17, &log_mergez_bins[0]);

    // All Reconstructed Vertices Matched to a Multi-Matched Gen
    // Vertex. Used for Duplicate rate plots done w.r.t. Reco
    // Quantities. We basically want to ask how many times a RecoVTX
    // has been reconstructed and associated to a SimulatedVTX that
    // has been linked to at least another RecoVTX. In this sense this
    // RecoVTX is a duplicate of the same, real GenVTX.
    mes_[label]["RecoAllAssoc2MultiMatchedGen_NumVertices"] = i.book1D(
        "RecoAllAssoc2MultiMatchedGen_NumVertices",
        "RecoAllAssoc2MultiMatchedGen_NumVertices", 100, 0., 200.);
    mes_[label]["RecoAllAssoc2MultiMatchedGen_X"] =
        i.book1D("RecoAllAssoc2MultiMatchedGen_X",
                 "RecoAllAssoc2MultiMatchedGen_X", 120, -0.6, 0.6);
    mes_[label]["RecoAllAssoc2MultiMatchedGen_Y"] =
        i.book1D("RecoAllAssoc2MultiMatchedGen_Y",
                 "RecoAllAssoc2MultiMatchedGen_Y", 120, -0.6, 0.6);
    mes_[label]["RecoAllAssoc2MultiMatchedGen_Z"] =
        i.book1D("RecoAllAssoc2MultiMatchedGen_Z",
                 "RecoAllAssoc2MultiMatchedGen_Z", 120, -60, 60);
    mes_[label]["RecoAllAssoc2MultiMatchedGen_R"] =
        i.book1D("RecoAllAssoc2MultiMatchedGen_R",
                 "RecoAllAssoc2MultiMatchedGen_R", 120, 0, 0.6);
    mes_[label]["RecoAllAssoc2MultiMatchedGen_Pt2"] =
        i.book1D("RecoAllAssoc2MultiMatchedGen_Pt2",
                 "RecoAllAssoc2MultiMatchedGen_Sum-pt2", 15, &log_pt2_bins[0]);
    mes_[label]["RecoAllAssoc2MultiMatchedGen_NumTracks"] =
        i.book1D("RecoAllAssoc2MultiMatchedGen_NumTracks",
                 "RecoAllAssoc2MultiMatchedGen_NumTracks", 24, &log_ntrk_bins[0]);
    mes_[label]["RecoAllAssoc2MultiMatchedGen_PU"] =
        i.book1D("RecoAllAssoc2MultiMatchedGen_PU",
                 "RecoAllAssoc2MultiMatchedGen_PU", 125, 0., 250.);
    mes_[label]["RecoAllAssoc2MultiMatchedGen_ClosestDistanceZ"] = i.book1D(
        "RecoAllAssoc2MultiMatchedGen_ClosestDistanceZ",
        "RecoAllAssoc2MultiMatchedGen_ClosestDistanceZ", 30, &log_bins[0]);
    mes_[label]["RecoAllAssoc2GenSimForMerge_ClosestDistanceZ"] = i.book1D(
        "RecoAllAssoc2GenSimForMerge_ClosestDistanceZ",
        "RecoAllAssoc2GenSimForMerge_ClosestDistanceZ",
        17, &log_mergez_bins[0]);


    // Resolution and pull histograms
    auto book1d = [&](const char *name, int bins, double min, double max) {
      mes_[label][name] = i.book1D(name, name, bins, min, max);
    };
    auto book1dlogx = [&](const char *name, int bins, float *xbinedges) {
      mes_[label][name] = i.book1D(name, name, bins, xbinedges);
    };
    auto book2d = [&](const char *name,
                      int xbins, double xmin, double xmax,
                      int ybins, double ymin, double ymax) {
      mes_[label][name] = i.book2D(name, name, xbins,xmin,xmax, ybins,ymin,ymax);
    };
    auto book2dlogx = [&](const char *name,
                          int xbins, float *xbinedges,
                          int ybins, double ymin, double ymax) {
      auto me = i.book2D(name, name, xbins,xbinedges[0],xbinedges[xbins], ybins,ymin,ymax);
      me->getTH2F()->GetXaxis()->Set(xbins, xbinedges);
      mes_[label][name] = me;
    };

    const double resolx = 0.1;
    const double resoly = 0.1;
    const double resolz = 0.1;
    const double resolpt2 = 10;

    // Non-merged vertices
    book1d("RecoAllAssoc2GenMatched_ResolX",   100,-resolx,resolx);
    book1d("RecoAllAssoc2GenMatched_ResolY",   100,-resoly,resoly);
    book1d("RecoAllAssoc2GenMatched_ResolZ",   100,-resolz,resolz);
    book1d("RecoAllAssoc2GenMatched_ResolPt2", 100,-resolpt2,resolpt2);

    book2d("RecoAllAssoc2GenMatched_ResolX_vs_PU",   125,0.,250., 100,-resolx,resolx);
    book2d("RecoAllAssoc2GenMatched_ResolY_vs_PU",   125,0.,250., 100,-resoly,resoly);
    book2d("RecoAllAssoc2GenMatched_ResolZ_vs_PU",   125,0.,250., 100,-resolz,resolz);
    book2d("RecoAllAssoc2GenMatched_ResolPt2_vs_PU", 125,0.,250., 100,-resolpt2,resolpt2);

    book2dlogx("RecoAllAssoc2GenMatched_ResolX_vs_NumTracks",   24,&log_ntrk_bins[0], 100,-resolx,resolx);
    book2dlogx("RecoAllAssoc2GenMatched_ResolY_vs_NumTracks",   24,&log_ntrk_bins[0], 100,-resoly,resoly);
    book2dlogx("RecoAllAssoc2GenMatched_ResolZ_vs_NumTracks",   24,&log_ntrk_bins[0], 100,-resolz,resolz);
    book2dlogx("RecoAllAssoc2GenMatched_ResolPt2_vs_NumTracks", 24,&log_ntrk_bins[0], 100,-resolpt2,resolpt2);

    book1d("RecoAllAssoc2GenMatched_PullX", 250,-25,25);
    book1d("RecoAllAssoc2GenMatched_PullY", 250,-25,25);
    book1d("RecoAllAssoc2GenMatched_PullZ", 250,-25,25);

    // Merged vertices
    book1d("RecoAllAssoc2GenMatchedMerged_ResolX",   100,-resolx,resolx);
    book1d("RecoAllAssoc2GenMatchedMerged_ResolY",   100,-resoly,resoly);
    book1d("RecoAllAssoc2GenMatchedMerged_ResolZ",   100,-resolz,resolz);
    book1d("RecoAllAssoc2GenMatchedMerged_ResolPt2", 100,-resolpt2,resolpt2);

    book2d("RecoAllAssoc2GenMatchedMerged_ResolX_vs_PU",   125,0.,250., 100,-resolx,resolx);
    book2d("RecoAllAssoc2GenMatchedMerged_ResolY_vs_PU",   125,0.,250., 100,-resoly,resoly);
    book2d("RecoAllAssoc2GenMatchedMerged_ResolZ_vs_PU",   125,0.,250., 100,-resolz,resolz);
    book2d("RecoAllAssoc2GenMatchedMerged_ResolPt2_vs_PU", 125,0.,250., 100,-resolpt2,resolpt2);

    book2dlogx("RecoAllAssoc2GenMatchedMerged_ResolX_vs_NumTracks",   24,&log_ntrk_bins[0], 100,-resolx,resolx);
    book2dlogx("RecoAllAssoc2GenMatchedMerged_ResolY_vs_NumTracks",   24,&log_ntrk_bins[0], 100,-resoly,resoly);
    book2dlogx("RecoAllAssoc2GenMatchedMerged_ResolZ_vs_NumTracks",   24,&log_ntrk_bins[0], 100,-resolz,resolz);
    book2dlogx("RecoAllAssoc2GenMatchedMerged_ResolPt2_vs_NumTracks", 24,&log_ntrk_bins[0], 100,-resolpt2,resolpt2);

    book1d("RecoAllAssoc2GenMatchedMerged_PullX", 250,-25,25);
    book1d("RecoAllAssoc2GenMatchedMerged_PullY", 250,-25,25);
    book1d("RecoAllAssoc2GenMatchedMerged_PullZ", 250,-25,25);


    // Purity histograms
    // Reco PV (vtx0) matched to hard-scatter gen vertex
    book1d("RecoPVAssoc2GenPVMatched_Purity", 50, 0, 1);
    book1d("RecoPVAssoc2GenPVMatched_Missing", 50, 0, 1);
    book2d("RecoPVAssoc2GenPVMatched_Purity_vs_Index", 100,0,100, 50,0,1);

    // RECO PV (vtx0) not matched to hard-scatter gen vertex
    book1d("RecoPVAssoc2GenPVNotMatched_Purity", 50, 0, 1);
    book1d("RecoPVAssoc2GenPVNotMatched_Missing", 50, 0, 1);
    book2d("RecoPVAssoc2GenPVNotMatched_Purity_vs_Index", 100,0,100, 50,0,1);

    // Purity vs. fake rate
    book1d("RecoAllAssoc2Gen_Purity", 50, 0, 1); // denominator
    book1d("RecoAllAssoc2GenMatched_Purity", 50, 0, 1); // 1-numerator

    // Vertex sum(pt2)
    // The first two are orthogonal (i.e. their sum includes all reco vertices)
    book1dlogx("RecoAssoc2GenPVMatched_Pt2", 15, &log_pt2_bins[0]);
    book1dlogx("RecoAssoc2GenPVNotMatched_Pt2", 15, &log_pt2_bins[0]);

    book1dlogx("RecoAssoc2GenPVMatchedNotHighest_Pt2", 15, &log_pt2_bins[0]);
    book1dlogx("RecoAssoc2GenPVNotMatched_GenPVTracksRemoved_Pt2", 15, &log_pt2_bins[0]);

    // Shared tracks
    book1d("RecoAllAssoc2GenSingleMatched_SharedTrackFractionReco", 50, 0, 1);
    book1d("RecoAllAssoc2GenMultiMatched_SharedTrackFractionReco", 50, 0, 1);
    book1d("RecoAllAssoc2GenSingleMatched_SharedTrackFractionRecoMatched", 50, 0, 1);
    book1d("RecoAllAssoc2GenMultiMatched_SharedTrackFractionRecoMatched", 50, 0, 1);
    book1d("RecoAllAssoc2GenSingleMatched_SharedTrackFractionSim", 50, 0, 1);
    book1d("RecoAllAssoc2GenMultiMatched_SharedTrackFractionSim", 50, 0, 1);
    book1d("RecoAllAssoc2GenSingleMatched_SharedTrackFractionSimMatched", 50, 0, 1);
    book1d("RecoAllAssoc2GenMultiMatched_SharedTrackFractionSimMatched", 50, 0, 1);

    mes_[label]["RecoAllAssoc2GenSingleMatched_SharedTrackFractionReco"] =
      i.book1D("RecoAllAssoc2GenSingleMatched_SharedTrackFractionReco",
               "ReconstructedAllAssoc2GenSingleMatched_SharedTrackFractionReco",
               50, 0, 1);
    mes_[label]["RecoAllAssoc2GenMultiMatched_SharedTrackFractionReco"] =
      i.book1D("RecoAllAssoc2GenMultiMatched_SharedTrackFractionReco",
               "ReconstructedAllAssoc2GenMultiMatched_SharedTrackFractionReco",
               50, 0, 1);                 
    mes_[label]["RecoAllAssoc2GenSingleMatched_SharedTrackFractionRecoMatched"] =
      i.book1D("RecoAllAssoc2GenSingleMatched_SharedTrackFractionRecoMatched",
               "ReconstructedAllAssoc2GenSingleMatched_SharedTrackFractionRecoMatched",
               50, 0, 1);    
    mes_[label]["RecoAllAssoc2GenMultiMatched_SharedTrackFractionRecoMatched"] =
      i.book1D("RecoAllAssoc2GenMultiMatched_SharedTrackFractionRecoMatched",
               "ReconstructedAllAssoc2GenMultiMatched_SharedTrackFractionRecoMatched",
               50, 0, 1); 
                                
    mes_[label]["RecoAllAssoc2GenSingleMatched_SharedTrackFractionSim"] =
      i.book1D("RecoAllAssoc2GenSingleMatched_SharedTrackFractionSim",
               "ReconstructedAllAssoc2GenSingleMatched_SharedTrackFractionSim",
               50, 0, 1);
    mes_[label]["RecoAllAssoc2GenMultiMatched_SharedTrackFractionSim"] =
      i.book1D("RecoAllAssoc2GenMultiMatched_SharedTrackFractionSim",
               "ReconstructedAllAssoc2GenMultiMatched_SharedTrackFractionSim",
               50, 0, 1);
                                
    mes_[label]["RecoAllAssoc2GenSingleMatched_SharedTrackFractionSimMatched"] =
      i.book1D("RecoAllAssoc2GenSingleMatched_SharedTrackFractionSimMatched",
               "ReconstructedAllAssoc2GenSingleMatched_SharedTrackFractionSimMatched",
               50, 0, 1);    
    mes_[label]["RecoAllAssoc2GenMultiMatched_SharedTrackFractionSimMatched"] =
      i.book1D("RecoAllAssoc2GenMultiMatched_SharedTrackFractionSimMatched",
               "ReconstructedAllAssoc2GenMultiMatched_SharedTrackFractionSimMatched",
               50, 0, 1);                 


    book1d("GenAllAssoc2RecoSingleMatched_SharedTrackFractionReco", 50, 0, 1);
    book1d("GenAllAssoc2RecoMultiMatched_SharedTrackFractionReco", 50, 0, 1);
    book1d("GenAllAssoc2RecoSingleMatched_SharedTrackFractionRecoMatched", 50, 0, 1);
    book1d("GenAllAssoc2RecoMultiMatched_SharedTrackFractionRecoMatched", 50, 0, 1);
    book1d("GenAllAssoc2RecoSingleMatched_SharedTrackFractionSim", 50, 0, 1);
    book1d("GenAllAssoc2RecoMultiMatched_SharedTrackFractionSim", 50, 0, 1);
    book1d("GenAllAssoc2RecoSingleMatched_SharedTrackFractionSimMatched", 50, 0, 1);
    book1d("GenAllAssoc2RecoMultiMatched_SharedTrackFractionSimMatched", 50, 0, 1);


    mes_[label]["GenAllAssoc2RecoSingleMatched_SharedTrackFractionReco"] =
        i.book1D("GenAllAssoc2RecoSingleMatched_SharedTrackFractionReco",
                 "GeneratedAllAssoc2RecoSingleMatched_SharedTrackFractionReco",
                 50, 0, 1);                  
    mes_[label]["GenAllAssoc2RecoMultiMatched_SharedTrackFractionReco"] =
      i.book1D("GenAllAssoc2RecoMultiMatched_SharedTrackFractionReco",
               "GeneratedAllAssoc2RecoMultiMatched_SharedTrackFractionReco",
               50, 0, 1);
    mes_[label]["GenAllAssoc2RecoSingleMatched_SharedTrackFractionRecoMatched"] =
        i.book1D("GenAllAssoc2RecoSingleMatched_SharedTrackFractionRecoMatched",
                 "GeneratedAllAssoc2RecoSingleMatched_SharedTrackFractionRecoMatched",
                 50, 0, 1);    
    mes_[label]["GenAllAssoc2RecoMultiMatched_SharedTrackFractionRecoMatched"] =
      i.book1D("GenAllAssoc2RecoMultiMatched_SharedTrackFractionRecoMatched",
               "GeneratedAllAssoc2RecoMultiMatched_SharedTrackFractionRecoMatched",
               50, 0, 1);    
                                  
    mes_[label]["GenAllAssoc2RecoSingleMatched_SharedTrackFractionSim"] =
        i.book1D("GenAllAssoc2RecoSingleMatched_SharedTrackFractionSim",
                 "GeneratedAllAssoc2RecoSingleMatched_SharedTrackFractionSim",
                 50, 0, 1);
    mes_[label]["GenAllAssoc2RecoMultiMatched_SharedTrackFractionSim"] =
      i.book1D("GenAllAssoc2RecoMultiMatched_SharedTrackFractionSim",
               "GeneratedAllAssoc2RecoMultiMatched_SharedTrackFractionSim",
               50, 0, 1);                   
    mes_[label]["GenAllAssoc2RecoSingleMatched_SharedTrackFractionSimMatched"] =
        i.book1D("GenAllAssoc2RecoSingleMatched_SharedTrackFractionSimMatched",
                 "GeneratedAllAssoc2RecoSingleMatched_SharedTrackFractionSimMatched",
                 50, 0, 1);    
    mes_[label]["GenAllAssoc2RecoMultiMatched_SharedTrackFractionSimMatched"] =
      i.book1D("GenAllAssoc2RecoMultiMatched_SharedTrackFractionSimMatched",
               "GeneratedAllAssoc2RecoMultiMatched_SharedTrackFractionSimMatched",
               50, 0, 1);    


    
    if(doDetailedHistograms_)
    {
      //Gen
      book1d("GenAllAssoc2RecoSingleMatched_SharedTrackWFractionReco", 50, 0, 1);
      book1d("GenAllAssoc2RecoSingleMatched_SharedTrackPtFractionReco", 50, 0, 1);
      book1d("GenAllAssoc2RecoSingleMatched_SharedTrackWPtFractionReco", 50, 0, 1);
      book1d("GenAllAssoc2RecoSingleMatched_SharedTrackPt2FractionReco", 50, 0, 1);
      book1d("GenAllAssoc2RecoSingleMatched_SharedTrackWPt2FractionReco", 50, 0, 1);
      book1d("GenAllAssoc2RecoSingleMatched_SharedTrackHarmPtFractionReco", 50, 0, 1);
      book1d("GenAllAssoc2RecoSingleMatched_SharedTrackWHarmPtFractionReco", 50, 0, 1);
      book1d("GenAllAssoc2RecoSingleMatched_SharedTrackHarmWPtFractionReco", 50, 0, 1);
      book1d("GenAllAssoc2RecoSingleMatched_SharedTrackHarmPtAvgFractionReco", 50, 0, 1);
      book1d("GenAllAssoc2RecoSingleMatched_SharedTrackWHarmPtAvgFractionReco", 50, 0, 1);
      book1d("GenAllAssoc2RecoSingleMatched_SharedTrackHarmWPtAvgFractionReco", 50, 0, 1);

      book1d("GenAllAssoc2RecoMultiMatched_SharedTrackWFractionReco", 50, 0, 1);
      book1d("GenAllAssoc2RecoMultiMatched_SharedTrackPtFractionReco", 50, 0, 1);
      book1d("GenAllAssoc2RecoMultiMatched_SharedTrackWPtFractionReco", 50, 0, 1);
      book1d("GenAllAssoc2RecoMultiMatched_SharedTrackPt2FractionReco", 50, 0, 1);
      book1d("GenAllAssoc2RecoMultiMatched_SharedTrackWPt2FractionReco", 50, 0, 1);
      book1d("GenAllAssoc2RecoMultiMatched_SharedTrackHarmPtFractionReco", 50, 0, 1);
      book1d("GenAllAssoc2RecoMultiMatched_SharedTrackWHarmPtFractionReco", 50, 0, 1);
      book1d("GenAllAssoc2RecoMultiMatched_SharedTrackHarmWPtFractionReco", 50, 0, 1);
      book1d("GenAllAssoc2RecoMultiMatched_SharedTrackHarmPtAvgFractionReco", 50, 0, 1);
      book1d("GenAllAssoc2RecoMultiMatched_SharedTrackWHarmPtAvgFractionReco", 50, 0, 1);
      book1d("GenAllAssoc2RecoMultiMatched_SharedTrackHarmWPtAvgFractionReco", 50, 0, 1);

      book1d("GenAllAssoc2RecoSingleMatched_SharedTrackWFractionRecoMatched", 50, 0, 1);
      book1d("GenAllAssoc2RecoSingleMatched_SharedTrackPtFractionRecoMatched", 50, 0, 1);
      book1d("GenAllAssoc2RecoSingleMatched_SharedTrackWPtFractionRecoMatched", 50, 0, 1);
      book1d("GenAllAssoc2RecoSingleMatched_SharedTrackPt2FractionRecoMatched", 50, 0, 1);
      book1d("GenAllAssoc2RecoSingleMatched_SharedTrackWPt2FractionRecoMatched", 50, 0, 1);
      book1d("GenAllAssoc2RecoSingleMatched_SharedTrackHarmPtFractionRecoMatched", 50, 0, 1);
      book1d("GenAllAssoc2RecoSingleMatched_SharedTrackWHarmPtFractionRecoMatched", 50, 0, 1);
      book1d("GenAllAssoc2RecoSingleMatched_SharedTrackHarmWPtFractionRecoMatched", 50, 0, 1);
      book1d("GenAllAssoc2RecoSingleMatched_SharedTrackHarmPtAvgFractionRecoMatched", 50, 0, 1);
      book1d("GenAllAssoc2RecoSingleMatched_SharedTrackWHarmPtAvgFractionRecoMatched", 50, 0, 1);
      book1d("GenAllAssoc2RecoSingleMatched_SharedTrackHarmWPtAvgFractionRecoMatched", 50, 0, 1);

      book1d("GenAllAssoc2RecoMultiMatched_SharedTrackWFractionRecoMatched", 50, 0, 1);
      book1d("GenAllAssoc2RecoMultiMatched_SharedTrackPtFractionRecoMatched", 50, 0, 1);
      book1d("GenAllAssoc2RecoMultiMatched_SharedTrackWPtFractionRecoMatched", 50, 0, 1);
      book1d("GenAllAssoc2RecoMultiMatched_SharedTrackPt2FractionRecoMatched", 50, 0, 1);
      book1d("GenAllAssoc2RecoMultiMatched_SharedTrackWPt2FractionRecoMatched", 50, 0, 1);
      book1d("GenAllAssoc2RecoMultiMatched_SharedTrackHarmPtFractionRecoMatched", 50, 0, 1);
      book1d("GenAllAssoc2RecoMultiMatched_SharedTrackWHarmPtFractionRecoMatched", 50, 0, 1);
      book1d("GenAllAssoc2RecoMultiMatched_SharedTrackHarmWPtFractionRecoMatched", 50, 0, 1);
      book1d("GenAllAssoc2RecoMultiMatched_SharedTrackHarmPtAvgFractionRecoMatched", 50, 0, 1);
      book1d("GenAllAssoc2RecoMultiMatched_SharedTrackWHarmPtAvgFractionRecoMatched", 50, 0, 1);
      book1d("GenAllAssoc2RecoMultiMatched_SharedTrackHarmWPtAvgFractionRecoMatched", 50, 0, 1);


      book1d("GenAllAssoc2RecoSingleMatched_SharedTrackPtFractionSim", 50, 0, 1);
      book1d("GenAllAssoc2RecoSingleMatched_SharedTrackPt2FractionSim", 50, 0, 1); 

      book1d("GenAllAssoc2RecoMultiMatched_SharedTrackPtFractionSim", 50, 0, 1);
      book1d("GenAllAssoc2RecoMultiMatched_SharedTrackPt2FractionSim", 50, 0, 1);

      book1d("GenAllAssoc2RecoSingleMatched_SharedTrackPtFractionSimMatched", 50, 0, 1);
      book1d("GenAllAssoc2RecoSingleMatched_SharedTrackPt2FractionSimMatched", 50, 0, 1);    

      book1d("GenAllAssoc2RecoMultiMatched_SharedTrackPtFractionSimMatched", 50, 0, 1);
      book1d("GenAllAssoc2RecoMultiMatched_SharedTrackPt2FractionSimMatched", 50, 0, 1);

      // Separated sharedTrack 1

      book1d("GenAllAssoc2RecoSingleMatched_SharedTrackFractionReco_separated1", 50, 0, 1);
      book1d("GenAllAssoc2RecoSingleMatched_SharedTrackWFractionReco_separated1", 50, 0, 1);
      book1d("GenAllAssoc2RecoSingleMatched_SharedTrackPtFractionReco_separated1", 50, 0, 1);
      book1d("GenAllAssoc2RecoSingleMatched_SharedTrackWPtFractionReco_separated1", 50, 0, 1);
      book1d("GenAllAssoc2RecoSingleMatched_SharedTrackPt2FractionReco_separated1", 50, 0, 1);
      book1d("GenAllAssoc2RecoSingleMatched_SharedTrackWPt2FractionReco_separated1", 50, 0, 1);
      book1d("GenAllAssoc2RecoSingleMatched_SharedTrackHarmPtFractionReco_separated1", 50, 0, 1);
      book1d("GenAllAssoc2RecoSingleMatched_SharedTrackWHarmPtFractionReco_separated1", 50, 0, 1);
      book1d("GenAllAssoc2RecoSingleMatched_SharedTrackHarmWPtFractionReco_separated1", 50, 0, 1);
      book1d("GenAllAssoc2RecoSingleMatched_SharedTrackHarmPtAvgFractionReco_separated1", 50, 0, 1);
      book1d("GenAllAssoc2RecoSingleMatched_SharedTrackWHarmPtAvgFractionReco_separated1", 50, 0, 1);
      book1d("GenAllAssoc2RecoSingleMatched_SharedTrackHarmWPtAvgFractionReco_separated1", 50, 0, 1);

      book1d("GenAllAssoc2RecoMultiMatched_SharedTrackFractionReco_separated1", 50, 0, 1);
      book1d("GenAllAssoc2RecoMultiMatched_SharedTrackWFractionReco_separated1", 50, 0, 1);
      book1d("GenAllAssoc2RecoMultiMatched_SharedTrackPtFractionReco_separated1", 50, 0, 1);
      book1d("GenAllAssoc2RecoMultiMatched_SharedTrackWPtFractionReco_separated1", 50, 0, 1);
      book1d("GenAllAssoc2RecoMultiMatched_SharedTrackPt2FractionReco_separated1", 50, 0, 1);
      book1d("GenAllAssoc2RecoMultiMatched_SharedTrackWPt2FractionReco_separated1", 50, 0, 1);
      book1d("GenAllAssoc2RecoMultiMatched_SharedTrackHarmPtFractionReco_separated1", 50, 0, 1);
      book1d("GenAllAssoc2RecoMultiMatched_SharedTrackWHarmPtFractionReco_separated1", 50, 0, 1);
      book1d("GenAllAssoc2RecoMultiMatched_SharedTrackHarmWPtFractionReco_separated1", 50, 0, 1);
      book1d("GenAllAssoc2RecoMultiMatched_SharedTrackHarmPtAvgFractionReco_separated1", 50, 0, 1);
      book1d("GenAllAssoc2RecoMultiMatched_SharedTrackWHarmPtAvgFractionReco_separated1", 50, 0, 1);
      book1d("GenAllAssoc2RecoMultiMatched_SharedTrackHarmWPtAvgFractionReco_separated1", 50, 0, 1);

      book1d("GenAllAssoc2RecoSingleMatched_SharedTrackFractionRecoMatched_separated1", 50, 0, 1);
      book1d("GenAllAssoc2RecoSingleMatched_SharedTrackWFractionRecoMatched_separated1", 50, 0, 1);
      book1d("GenAllAssoc2RecoSingleMatched_SharedTrackPtFractionRecoMatched_separated1", 50, 0, 1);
      book1d("GenAllAssoc2RecoSingleMatched_SharedTrackWPtFractionRecoMatched_separated1", 50, 0, 1);
      book1d("GenAllAssoc2RecoSingleMatched_SharedTrackPt2FractionRecoMatched_separated1", 50, 0, 1);
      book1d("GenAllAssoc2RecoSingleMatched_SharedTrackWPt2FractionRecoMatched_separated1", 50, 0, 1);
      book1d("GenAllAssoc2RecoSingleMatched_SharedTrackHarmPtFractionRecoMatched_separated1", 50, 0, 1);
      book1d("GenAllAssoc2RecoSingleMatched_SharedTrackWHarmPtFractionRecoMatched_separated1", 50, 0, 1);
      book1d("GenAllAssoc2RecoSingleMatched_SharedTrackHarmWPtFractionRecoMatched_separated1", 50, 0, 1);
      book1d("GenAllAssoc2RecoSingleMatched_SharedTrackHarmPtAvgFractionRecoMatched_separated1", 50, 0, 1);
      book1d("GenAllAssoc2RecoSingleMatched_SharedTrackWHarmPtAvgFractionRecoMatched_separated1", 50, 0, 1);
      book1d("GenAllAssoc2RecoSingleMatched_SharedTrackHarmWPtAvgFractionRecoMatched_separated1", 50, 0, 1);

      book1d("GenAllAssoc2RecoMultiMatched_SharedTrackFractionRecoMatched_separated1", 50, 0, 1);
      book1d("GenAllAssoc2RecoMultiMatched_SharedTrackWFractionRecoMatched_separated1", 50, 0, 1);
      book1d("GenAllAssoc2RecoMultiMatched_SharedTrackPtFractionRecoMatched_separated1", 50, 0, 1);
      book1d("GenAllAssoc2RecoMultiMatched_SharedTrackWPtFractionRecoMatched_separated1", 50, 0, 1);
      book1d("GenAllAssoc2RecoMultiMatched_SharedTrackPt2FractionRecoMatched_separated1", 50, 0, 1);
      book1d("GenAllAssoc2RecoMultiMatched_SharedTrackWPt2FractionRecoMatched_separated1", 50, 0, 1);
      book1d("GenAllAssoc2RecoMultiMatched_SharedTrackHarmPtFractionRecoMatched_separated1", 50, 0, 1);
      book1d("GenAllAssoc2RecoMultiMatched_SharedTrackWHarmPtFractionRecoMatched_separated1", 50, 0, 1);
      book1d("GenAllAssoc2RecoMultiMatched_SharedTrackHarmWPtFractionRecoMatched_separated1", 50, 0, 1);
      book1d("GenAllAssoc2RecoMultiMatched_SharedTrackHarmPtAvgFractionRecoMatched_separated1", 50, 0, 1);
      book1d("GenAllAssoc2RecoMultiMatched_SharedTrackWHarmPtAvgFractionRecoMatched_separated1", 50, 0, 1);
      book1d("GenAllAssoc2RecoMultiMatched_SharedTrackHarmWPtAvgFractionRecoMatched_separated1", 50, 0, 1);

      book1d("GenAllAssoc2RecoSingleMatched_SharedTrackFractionSim_separated1", 50, 0, 1);
      book1d("GenAllAssoc2RecoSingleMatched_SharedTrackPtFractionSim_separated1", 50, 0, 1);
      book1d("GenAllAssoc2RecoSingleMatched_SharedTrackPt2FractionSim_separated1", 50, 0, 1); 
      book1d("GenAllAssoc2RecoMultiMatched_SharedTrackFractionSim_separated1", 50, 0, 1);
      book1d("GenAllAssoc2RecoMultiMatched_SharedTrackPtFractionSim_separated1", 50, 0, 1);
      book1d("GenAllAssoc2RecoMultiMatched_SharedTrackPt2FractionSim_separated1", 50, 0, 1);
      book1d("GenAllAssoc2RecoSingleMatched_SharedTrackFractionSimMatched_separated1", 50, 0, 1);
      book1d("GenAllAssoc2RecoSingleMatched_SharedTrackPtFractionSimMatched_separated1", 50, 0, 1);
      book1d("GenAllAssoc2RecoSingleMatched_SharedTrackPt2FractionSimMatched_separated1", 50, 0, 1);
      book1d("GenAllAssoc2RecoMultiMatched_SharedTrackFractionSimMatched_separated1", 50, 0, 1);
      book1d("GenAllAssoc2RecoMultiMatched_SharedTrackPtFractionSimMatched_separated1", 50, 0, 1);
      book1d("GenAllAssoc2RecoMultiMatched_SharedTrackPt2FractionSimMatched_separated1", 50, 0, 1);

      // Separated sharedTrack 2

      book1d("GenAllAssoc2RecoSingleMatched_SharedTrackFractionReco_separated2", 50, 0, 1);
      book1d("GenAllAssoc2RecoSingleMatched_SharedTrackWFractionReco_separated2", 50, 0, 1);
      book1d("GenAllAssoc2RecoSingleMatched_SharedTrackPtFractionReco_separated2", 50, 0, 1);
      book1d("GenAllAssoc2RecoSingleMatched_SharedTrackWPtFractionReco_separated2", 50, 0, 1);
      book1d("GenAllAssoc2RecoSingleMatched_SharedTrackPt2FractionReco_separated2", 50, 0, 1);
      book1d("GenAllAssoc2RecoSingleMatched_SharedTrackWPt2FractionReco_separated2", 50, 0, 1);
      book1d("GenAllAssoc2RecoSingleMatched_SharedTrackHarmPtFractionReco_separated2", 50, 0, 1);
      book1d("GenAllAssoc2RecoSingleMatched_SharedTrackWHarmPtFractionReco_separated2", 50, 0, 1);
      book1d("GenAllAssoc2RecoSingleMatched_SharedTrackHarmWPtFractionReco_separated2", 50, 0, 1);
      book1d("GenAllAssoc2RecoSingleMatched_SharedTrackHarmPtAvgFractionReco_separated2", 50, 0, 1);
      book1d("GenAllAssoc2RecoSingleMatched_SharedTrackWHarmPtAvgFractionReco_separated2", 50, 0, 1);
      book1d("GenAllAssoc2RecoSingleMatched_SharedTrackHarmWPtAvgFractionReco_separated2", 50, 0, 1);

      book1d("GenAllAssoc2RecoMultiMatched_SharedTrackFractionReco_separated2", 50, 0, 1);
      book1d("GenAllAssoc2RecoMultiMatched_SharedTrackWFractionReco_separated2", 50, 0, 1);
      book1d("GenAllAssoc2RecoMultiMatched_SharedTrackPtFractionReco_separated2", 50, 0, 1);
      book1d("GenAllAssoc2RecoMultiMatched_SharedTrackWPtFractionReco_separated2", 50, 0, 1);
      book1d("GenAllAssoc2RecoMultiMatched_SharedTrackPt2FractionReco_separated2", 50, 0, 1);
      book1d("GenAllAssoc2RecoMultiMatched_SharedTrackWPt2FractionReco_separated2", 50, 0, 1);
      book1d("GenAllAssoc2RecoMultiMatched_SharedTrackHarmPtFractionReco_separated2", 50, 0, 1);
      book1d("GenAllAssoc2RecoMultiMatched_SharedTrackWHarmPtFractionReco_separated2", 50, 0, 1);
      book1d("GenAllAssoc2RecoMultiMatched_SharedTrackHarmWPtFractionReco_separated2", 50, 0, 1);
      book1d("GenAllAssoc2RecoMultiMatched_SharedTrackHarmPtAvgFractionReco_separated2", 50, 0, 1);
      book1d("GenAllAssoc2RecoMultiMatched_SharedTrackWHarmPtAvgFractionReco_separated2", 50, 0, 1);
      book1d("GenAllAssoc2RecoMultiMatched_SharedTrackHarmWPtAvgFractionReco_separated2", 50, 0, 1);

      book1d("GenAllAssoc2RecoSingleMatched_SharedTrackFractionRecoMatched_separated2", 50, 0, 1);
      book1d("GenAllAssoc2RecoSingleMatched_SharedTrackWFractionRecoMatched_separated2", 50, 0, 1);
      book1d("GenAllAssoc2RecoSingleMatched_SharedTrackPtFractionRecoMatched_separated2", 50, 0, 1);
      book1d("GenAllAssoc2RecoSingleMatched_SharedTrackWPtFractionRecoMatched_separated2", 50, 0, 1);
      book1d("GenAllAssoc2RecoSingleMatched_SharedTrackPt2FractionRecoMatched_separated2", 50, 0, 1);
      book1d("GenAllAssoc2RecoSingleMatched_SharedTrackWPt2FractionRecoMatched_separated2", 50, 0, 1);
      book1d("GenAllAssoc2RecoSingleMatched_SharedTrackHarmPtFractionRecoMatched_separated2", 50, 0, 1);
      book1d("GenAllAssoc2RecoSingleMatched_SharedTrackWHarmPtFractionRecoMatched_separated2", 50, 0, 1);
      book1d("GenAllAssoc2RecoSingleMatched_SharedTrackHarmWPtFractionRecoMatched_separated2", 50, 0, 1);
      book1d("GenAllAssoc2RecoSingleMatched_SharedTrackHarmPtAvgFractionRecoMatched_separated2", 50, 0, 1);
      book1d("GenAllAssoc2RecoSingleMatched_SharedTrackWHarmPtAvgFractionRecoMatched_separated2", 50, 0, 1);
      book1d("GenAllAssoc2RecoSingleMatched_SharedTrackHarmWPtAvgFractionRecoMatched_separated2", 50, 0, 1);

      book1d("GenAllAssoc2RecoMultiMatched_SharedTrackFractionRecoMatched_separated2", 50, 0, 1);
      book1d("GenAllAssoc2RecoMultiMatched_SharedTrackWFractionRecoMatched_separated2", 50, 0, 1);
      book1d("GenAllAssoc2RecoMultiMatched_SharedTrackPtFractionRecoMatched_separated2", 50, 0, 1);
      book1d("GenAllAssoc2RecoMultiMatched_SharedTrackWPtFractionRecoMatched_separated2", 50, 0, 1);
      book1d("GenAllAssoc2RecoMultiMatched_SharedTrackPt2FractionRecoMatched_separated2", 50, 0, 1);
      book1d("GenAllAssoc2RecoMultiMatched_SharedTrackWPt2FractionRecoMatched_separated2", 50, 0, 1);
      book1d("GenAllAssoc2RecoMultiMatched_SharedTrackHarmPtFractionRecoMatched_separated2", 50, 0, 1);
      book1d("GenAllAssoc2RecoMultiMatched_SharedTrackWHarmPtFractionRecoMatched_separated2", 50, 0, 1);
      book1d("GenAllAssoc2RecoMultiMatched_SharedTrackHarmWPtFractionRecoMatched_separated2", 50, 0, 1);
      book1d("GenAllAssoc2RecoMultiMatched_SharedTrackHarmPtAvgFractionRecoMatched_separated2", 50, 0, 1);
      book1d("GenAllAssoc2RecoMultiMatched_SharedTrackWHarmPtAvgFractionRecoMatched_separated2", 50, 0, 1);
      book1d("GenAllAssoc2RecoMultiMatched_SharedTrackHarmWPtAvgFractionRecoMatched_separated2", 50, 0, 1);

      book1d("GenAllAssoc2RecoSingleMatched_SharedTrackFractionSim_separated2", 50, 0, 1);
      book1d("GenAllAssoc2RecoSingleMatched_SharedTrackPtFractionSim_separated2", 50, 0, 1);
      book1d("GenAllAssoc2RecoSingleMatched_SharedTrackPt2FractionSim_separated2", 50, 0, 1);
      book1d("GenAllAssoc2RecoMultiMatched_SharedTrackFractionSim_separated2", 50, 0, 1);
      book1d("GenAllAssoc2RecoMultiMatched_SharedTrackPtFractionSim_separated2", 50, 0, 1);
      book1d("GenAllAssoc2RecoMultiMatched_SharedTrackPt2FractionSim_separated2", 50, 0, 1);
      book1d("GenAllAssoc2RecoSingleMatched_SharedTrackFractionSimMatched_separated2", 50, 0, 1);
      book1d("GenAllAssoc2RecoSingleMatched_SharedTrackPtFractionSimMatched_separated2", 50, 0, 1);
      book1d("GenAllAssoc2RecoSingleMatched_SharedTrackPt2FractionSimMatched_separated2", 50, 0, 1); 
      book1d("GenAllAssoc2RecoMultiMatched_SharedTrackFractionSimMatched_separated2", 50, 0, 1);
      book1d("GenAllAssoc2RecoMultiMatched_SharedTrackPtFractionSimMatched_separated2", 50, 0, 1);
      book1d("GenAllAssoc2RecoMultiMatched_SharedTrackPt2FractionSimMatched_separated2", 50, 0, 1);


      // Separated sharedTrack 3

      book1d("GenAllAssoc2RecoSingleMatched_SharedTrackFractionReco_separated3", 50, 0, 1);
      book1d("GenAllAssoc2RecoSingleMatched_SharedTrackWFractionReco_separated3", 50, 0, 1);
      book1d("GenAllAssoc2RecoSingleMatched_SharedTrackPtFractionReco_separated3", 50, 0, 1);
      book1d("GenAllAssoc2RecoSingleMatched_SharedTrackWPtFractionReco_separated3", 50, 0, 1);
      book1d("GenAllAssoc2RecoSingleMatched_SharedTrackPt2FractionReco_separated3", 50, 0, 1);
      book1d("GenAllAssoc2RecoSingleMatched_SharedTrackWPt2FractionReco_separated3", 50, 0, 1);
      book1d("GenAllAssoc2RecoSingleMatched_SharedTrackHarmPtFractionReco_separated3", 50, 0, 1);
      book1d("GenAllAssoc2RecoSingleMatched_SharedTrackWHarmPtFractionReco_separated3", 50, 0, 1);
      book1d("GenAllAssoc2RecoSingleMatched_SharedTrackHarmWPtFractionReco_separated3", 50, 0, 1);
      book1d("GenAllAssoc2RecoSingleMatched_SharedTrackHarmPtAvgFractionReco_separated3", 50, 0, 1);
      book1d("GenAllAssoc2RecoSingleMatched_SharedTrackWHarmPtAvgFractionReco_separated3", 50, 0, 1);
      book1d("GenAllAssoc2RecoSingleMatched_SharedTrackHarmWPtAvgFractionReco_separated3", 50, 0, 1);

      book1d("GenAllAssoc2RecoMultiMatched_SharedTrackFractionReco_separated3", 50, 0, 1);
      book1d("GenAllAssoc2RecoMultiMatched_SharedTrackWFractionReco_separated3", 50, 0, 1);
      book1d("GenAllAssoc2RecoMultiMatched_SharedTrackPtFractionReco_separated3", 50, 0, 1);
      book1d("GenAllAssoc2RecoMultiMatched_SharedTrackWPtFractionReco_separated3", 50, 0, 1);
      book1d("GenAllAssoc2RecoMultiMatched_SharedTrackPt2FractionReco_separated3", 50, 0, 1);
      book1d("GenAllAssoc2RecoMultiMatched_SharedTrackWPt2FractionReco_separated3", 50, 0, 1);
      book1d("GenAllAssoc2RecoMultiMatched_SharedTrackHarmPtFractionReco_separated3", 50, 0, 1);
      book1d("GenAllAssoc2RecoMultiMatched_SharedTrackWHarmPtFractionReco_separated3", 50, 0, 1);
      book1d("GenAllAssoc2RecoMultiMatched_SharedTrackHarmWPtFractionReco_separated3", 50, 0, 1);
      book1d("GenAllAssoc2RecoMultiMatched_SharedTrackHarmPtAvgFractionReco_separated3", 50, 0, 1);
      book1d("GenAllAssoc2RecoMultiMatched_SharedTrackWHarmPtAvgFractionReco_separated3", 50, 0, 1);
      book1d("GenAllAssoc2RecoMultiMatched_SharedTrackHarmWPtAvgFractionReco_separated3", 50, 0, 1);

      book1d("GenAllAssoc2RecoSingleMatched_SharedTrackFractionRecoMatched_separated3", 50, 0, 1);
      book1d("GenAllAssoc2RecoSingleMatched_SharedTrackWFractionRecoMatched_separated3", 50, 0, 1);
      book1d("GenAllAssoc2RecoSingleMatched_SharedTrackPtFractionRecoMatched_separated3", 50, 0, 1);
      book1d("GenAllAssoc2RecoSingleMatched_SharedTrackWPtFractionRecoMatched_separated3", 50, 0, 1);
      book1d("GenAllAssoc2RecoSingleMatched_SharedTrackPt2FractionRecoMatched_separated3", 50, 0, 1);
      book1d("GenAllAssoc2RecoSingleMatched_SharedTrackWPt2FractionRecoMatched_separated3", 50, 0, 1);
      book1d("GenAllAssoc2RecoSingleMatched_SharedTrackHarmPtFractionRecoMatched_separated3", 50, 0, 1);
      book1d("GenAllAssoc2RecoSingleMatched_SharedTrackWHarmPtFractionRecoMatched_separated3", 50, 0, 1);
      book1d("GenAllAssoc2RecoSingleMatched_SharedTrackHarmWPtFractionRecoMatched_separated3", 50, 0, 1);
      book1d("GenAllAssoc2RecoSingleMatched_SharedTrackHarmPtAvgFractionRecoMatched_separated3", 50, 0, 1);
      book1d("GenAllAssoc2RecoSingleMatched_SharedTrackWHarmPtAvgFractionRecoMatched_separated3", 50, 0, 1);
      book1d("GenAllAssoc2RecoSingleMatched_SharedTrackHarmWPtAvgFractionRecoMatched_separated3", 50, 0, 1);

      book1d("GenAllAssoc2RecoMultiMatched_SharedTrackFractionRecoMatched_separated3", 50, 0, 1);
      book1d("GenAllAssoc2RecoMultiMatched_SharedTrackWFractionRecoMatched_separated3", 50, 0, 1);
      book1d("GenAllAssoc2RecoMultiMatched_SharedTrackPtFractionRecoMatched_separated3", 50, 0, 1);
      book1d("GenAllAssoc2RecoMultiMatched_SharedTrackWPtFractionRecoMatched_separated3", 50, 0, 1);
      book1d("GenAllAssoc2RecoMultiMatched_SharedTrackPt2FractionRecoMatched_separated3", 50, 0, 1);
      book1d("GenAllAssoc2RecoMultiMatched_SharedTrackWPt2FractionRecoMatched_separated3", 50, 0, 1);
      book1d("GenAllAssoc2RecoMultiMatched_SharedTrackHarmPtFractionRecoMatched_separated3", 50, 0, 1);
      book1d("GenAllAssoc2RecoMultiMatched_SharedTrackWHarmPtFractionRecoMatched_separated3", 50, 0, 1);
      book1d("GenAllAssoc2RecoMultiMatched_SharedTrackHarmWPtFractionRecoMatched_separated3", 50, 0, 1);
      book1d("GenAllAssoc2RecoMultiMatched_SharedTrackHarmPtAvgFractionRecoMatched_separated3", 50, 0, 1);
      book1d("GenAllAssoc2RecoMultiMatched_SharedTrackWHarmPtAvgFractionRecoMatched_separated3", 50, 0, 1);
      book1d("GenAllAssoc2RecoMultiMatched_SharedTrackHarmWPtAvgFractionRecoMatched_separated3", 50, 0, 1);

      book1d("GenAllAssoc2RecoSingleMatched_SharedTrackFractionSim_separated3", 50, 0, 1);
      book1d("GenAllAssoc2RecoSingleMatched_SharedTrackPtFractionSim_separated3", 50, 0, 1);
      book1d("GenAllAssoc2RecoSingleMatched_SharedTrackPt2FractionSim_separated3", 50, 0, 1);  
      book1d("GenAllAssoc2RecoMultiMatched_SharedTrackFractionSim_separated3", 50, 0, 1);
      book1d("GenAllAssoc2RecoMultiMatched_SharedTrackPtFractionSim_separated3", 50, 0, 1);
      book1d("GenAllAssoc2RecoMultiMatched_SharedTrackPt2FractionSim_separated3", 50, 0, 1);
      book1d("GenAllAssoc2RecoSingleMatched_SharedTrackFractionSimMatched_separated3", 50, 0, 1);
      book1d("GenAllAssoc2RecoSingleMatched_SharedTrackPtFractionSimMatched_separated3", 50, 0, 1);
      book1d("GenAllAssoc2RecoSingleMatched_SharedTrackPt2FractionSimMatched_separated3", 50, 0, 1);
      book1d("GenAllAssoc2RecoMultiMatched_SharedTrackFractionSimMatched_separated3", 50, 0, 1);
      book1d("GenAllAssoc2RecoMultiMatched_SharedTrackPtFractionSimMatched_separated3", 50, 0, 1);
      book1d("GenAllAssoc2RecoMultiMatched_SharedTrackPt2FractionSimMatched_separated3", 50, 0, 1);

      mes_[label]["GenAllAssoc2RecoSingleMatched_SharedTrackWFractionReco"] =
        i.book1D("GenAllAssoc2RecoSingleMatched_SharedTrackWFractionReco",
                 "GeneratedAllAssoc2RecoSingleMatched_SharedTrackWFractionReco",
                 50, 0, 1);
      mes_[label]["GenAllAssoc2RecoSingleMatched_SharedTrackWFractionRecoMatched"] =
        i.book1D("GenAllAssoc2RecoSingleMatched_SharedTrackWFractionRecoMatched",
                 "GeneratedAllAssoc2RecoSingleMatched_SharedTrackWFractionRecoMatched",
                 50, 0, 1);    
      mes_[label]["GenAllAssoc2RecoSingleMatched_SharedTrackPtFractionReco"] =
        i.book1D("GenAllAssoc2RecoSingleMatched_SharedTrackPtFractionReco",
                 "GeneratedAllAssoc2RecoSingleMatched_SharedTrackPtFractionReco",
                 50, 0, 1);
      mes_[label]["GenAllAssoc2RecoSingleMatched_SharedTrackPtFractionRecoMatched"] =
        i.book1D("GenAllAssoc2RecoSingleMatched_SharedTrackPtFractionRecoMatched",
                 "GeneratedAllAssoc2RecoSingleMatched_SharedTrackPtFractionRecoMatched",
                 50, 0, 1);    
      mes_[label]["GenAllAssoc2RecoSingleMatched_SharedTrackWPtFractionReco"] =
        i.book1D("GenAllAssoc2RecoSingleMatched_SharedTrackWPtFractionReco",
                 "GeneratedAllAssoc2RecoSingleMatched_SharedTrackWPtFractionReco",
                 50, 0, 1);
      mes_[label]["GenAllAssoc2RecoSingleMatched_SharedTrackWPtFractionRecoMatched"] =
        i.book1D("GenAllAssoc2RecoSingleMatched_SharedTrackWPtFractionRecoMatched",
                 "GeneratedAllAssoc2RecoSingleMatched_SharedTrackWPtFractionRecoMatched",
                 50, 0, 1);    
      mes_[label]["GenAllAssoc2RecoSingleMatched_SharedTrackPt2FractionReco"] =
        i.book1D("GenAllAssoc2RecoSingleMatched_SharedTrackPt2FractionReco",
                 "GeneratedAllAssoc2RecoSingleMatched_SharedTrackPt2FractionReco",
                 50, 0, 1);
      mes_[label]["GenAllAssoc2RecoSingleMatched_SharedTrackPt2FractionRecoMatched"] =
        i.book1D("GenAllAssoc2RecoSingleMatched_SharedTrackPt2FractionRecoMatched",
                 "GeneratedAllAssoc2RecoSingleMatched_SharedTrackPt2FractionRecoMatched",
                 50, 0, 1);    
      mes_[label]["GenAllAssoc2RecoSingleMatched_SharedTrackWPt2FractionReco"] =
        i.book1D("GenAllAssoc2RecoSingleMatched_SharedTrackWPt2FractionReco",
                 "GeneratedAllAssoc2RecoSingleMatched_SharedTrackWPt2FractionReco",
                 50, 0, 1);
      mes_[label]["GenAllAssoc2RecoSingleMatched_SharedTrackWPt2FractionRecoMatched"] =
        i.book1D("GenAllAssoc2RecoSingleMatched_SharedTrackWPt2FractionRecoMatched",
                 "GeneratedAllAssoc2RecoSingleMatched_SharedTrackWPt2FractionRecoMatched",
                 50, 0, 1);    
 
      mes_[label]["GenAllAssoc2RecoSingleMatched_SharedTrackPtFractionSim"] =
        i.book1D("GenAllAssoc2RecoSingleMatched_SharedTrackPtFractionSim",
                 "GeneratedAllAssoc2RecoSingleMatched_SharedTrackPtFractionSim",
                 50, 0, 1);
      mes_[label]["GenAllAssoc2RecoSingleMatched_SharedTrackPtFractionSimMatched"] =
        i.book1D("GenAllAssoc2RecoSingleMatched_SharedTrackPtFractionSimMatched",
                 "GeneratedAllAssoc2RecoSingleMatched_SharedTrackPtFractionSimMatched",
                 50, 0, 1);    
      mes_[label]["GenAllAssoc2RecoSingleMatched_SharedTrackPtFractionSim"] =
        i.book1D("GenAllAssoc2RecoSingleMatched_SharedTrackPtFractionSim",
                 "GeneratedAllAssoc2RecoSingleMatched_SharedTrackPtFractionSim",
                 50, 0, 1);
      mes_[label]["GenAllAssoc2RecoSingleMatched_SharedTrackPtFractionSimMatched"] =
        i.book1D("GenAllAssoc2RecoSingleMatched_SharedTrackPtFractionSimMatched",
                 "GeneratedAllAssoc2RecoSingleMatched_SharedTrackPtFractionSimMatched",
                 50, 0, 1);    
      mes_[label]["GenAllAssoc2RecoSingleMatched_SharedTrackPt2FractionSim"] =
        i.book1D("GenAllAssoc2RecoSingleMatched_SharedTrackPt2FractionSim",
                 "GeneratedAllAssoc2RecoSingleMatched_SharedTrackPt2FractionSim",
                 50, 0, 1);
      mes_[label]["GenAllAssoc2RecoSingleMatched_SharedTrackPt2FractionSimMatched"] =
        i.book1D("GenAllAssoc2RecoSingleMatched_SharedTrackPt2FractionSimMatched",
                 "GeneratedAllAssoc2RecoSingleMatched_SharedTrackPt2FractionSimMatched",
                 50, 0, 1);    



      mes_[label]["GenAllAssoc2RecoMultiMatched_SharedTrackWFractionReco"] =
        i.book1D("GenAllAssoc2RecoMultiMatched_SharedTrackWFractionReco",
                 "GeneratedAllAssoc2RecoMultiMatched_SharedTrackWFractionReco",
                 50, 0, 1);
      mes_[label]["GenAllAssoc2RecoMultiMatched_SharedTrackWFractionRecoMatched"] =
        i.book1D("GenAllAssoc2RecoMultiMatched_SharedTrackWFractionRecoMatched",
                 "GeneratedAllAssoc2RecoMultiMatched_SharedTrackWFractionRecoMatched",
                 50, 0, 1);    
      mes_[label]["GenAllAssoc2RecoMultiMatched_SharedTrackPtFractionReco"] =
        i.book1D("GenAllAssoc2RecoMultiMatched_SharedTrackPtFractionReco",
                 "GeneratedAllAssoc2RecoMultiMatched_SharedTrackPtFractionReco",
                 50, 0, 1);
      mes_[label]["GenAllAssoc2RecoMultiMatched_SharedTrackPtFractionRecoMatched"] =
        i.book1D("GenAllAssoc2RecoMultiMatched_SharedTrackPtFractionRecoMatched",
                 "GeneratedAllAssoc2RecoMultiMatched_SharedTrackPtFractionRecoMatched",
                 50, 0, 1);    
      mes_[label]["GenAllAssoc2RecoMultiMatched_SharedTrackWPtFractionReco"] =
        i.book1D("GenAllAssoc2RecoMultiMatched_SharedTrackWPtFractionReco",
                 "GeneratedAllAssoc2RecoMultiMatched_SharedTrackWPtFractionReco",
                 50, 0, 1);
      mes_[label]["GenAllAssoc2RecoMultiMatched_SharedTrackWPtFractionRecoMatched"] =
        i.book1D("GenAllAssoc2RecoMultiMatched_SharedTrackWPtFractionRecoMatched",
                 "GeneratedAllAssoc2RecoMultiMatched_SharedTrackWPtFractionRecoMatched",
                 50, 0, 1);    
      mes_[label]["GenAllAssoc2RecoMultiMatched_SharedTrackPt2FractionReco"] =
        i.book1D("GenAllAssoc2RecoMultiMatched_SharedTrackPt2FractionReco",
                 "GeneratedAllAssoc2RecoMultiMatched_SharedTrackPt2FractionReco",
                 50, 0, 1);
      mes_[label]["GenAllAssoc2RecoMultiMatched_SharedTrackPt2FractionRecoMatched"] =
        i.book1D("GenAllAssoc2RecoMultiMatched_SharedTrackPt2FractionRecoMatched",
                 "GeneratedAllAssoc2RecoMultiMatched_SharedTrackPt2FractionRecoMatched",
                 50, 0, 1);    
      mes_[label]["GenAllAssoc2RecoMultiMatched_SharedTrackWPt2FractionReco"] =
        i.book1D("GenAllAssoc2RecoMultiMatched_SharedTrackWPt2FractionReco",
                 "GeneratedAllAssoc2RecoMultiMatched_SharedTrackWPt2FractionReco",
                 50, 0, 1);
      mes_[label]["GenAllAssoc2RecoMultiMatched_SharedTrackWPt2FractionRecoMatched"] =
        i.book1D("GenAllAssoc2RecoMultiMatched_SharedTrackWPt2FractionRecoMatched",
                 "GeneratedAllAssoc2RecoMultiMatched_SharedTrackWPt2FractionRecoMatched",
                 50, 0, 1);    


      mes_[label]["GenAllAssoc2RecoMultiMatched_SharedTrackPtFractionSim"] =
        i.book1D("GenAllAssoc2RecoMultiMatched_SharedTrackPtFractionSim",
                 "GeneratedAllAssoc2RecoMultiMatched_SharedTrackPtFractionSim",
                 50, 0, 1);
      mes_[label]["GenAllAssoc2RecoMultiMatched_SharedTrackPtFractionSimMatched"] =
        i.book1D("GenAllAssoc2RecoMultiMatched_SharedTrackPtFractionSimMatched",
                 "GeneratedAllAssoc2RecoMultiMatched_SharedTrackPtFractionSimMatched",
                 50, 0, 1);    
      mes_[label]["GenAllAssoc2RecoMultiMatched_SharedTrackPt2FractionSim"] =
        i.book1D("GenAllAssoc2RecoMultiMatched_SharedTrackPt2FractionSim",
                 "GeneratedAllAssoc2RecoMultiMatched_SharedTrackPt2FractionSim",
                 50, 0, 1);
      mes_[label]["GenAllAssoc2RecoMultiMatched_SharedTrackPt2FractionSimMatched"] =
        i.book1D("GenAllAssoc2RecoMultiMatched_SharedTrackPt2FractionSimMatched",
                 "GeneratedAllAssoc2RecoMultiMatched_SharedTrackPt2FractionSimMatched",
                 50, 0, 1);  
    
      // Gen separated1
      mes_[label]["GenAllAssoc2RecoSingleMatched_SharedTrackFractionReco_separated1"] =
        i.book1D("GenAllAssoc2RecoSingleMatched_SharedTrackFractionReco_separated1",
                 "GeneratedAllAssoc2RecoSingleMatched_SharedTrackFractionReco_separated1",
                 50, 0, 1);
      mes_[label]["GenAllAssoc2RecoSingleMatched_SharedTrackFractionRecoMatched_separated1"] =
        i.book1D("GenAllAssoc2RecoSingleMatched_SharedTrackFractionRecoMatched_separated1",
                 "GeneratedAllAssoc2RecoSingleMatched_SharedTrackFractionRecoMatched_separated1",
                 50, 0, 1);    
      mes_[label]["GenAllAssoc2RecoSingleMatched_SharedTrackWFractionReco_separated1"] =
        i.book1D("GenAllAssoc2RecoSingleMatched_SharedTrackWFractionReco_separated1",
                 "GeneratedAllAssoc2RecoSingleMatched_SharedTrackWFractionReco_separated1",
                 50, 0, 1);
      mes_[label]["GenAllAssoc2RecoSingleMatched_SharedTrackWFractionRecoMatched_separated1"] =
        i.book1D("GenAllAssoc2RecoSingleMatched_SharedTrackWFractionRecoMatched_separated1",
                 "GeneratedAllAssoc2RecoSingleMatched_SharedTrackWFractionRecoMatched_separated1",
                 50, 0, 1);    
      mes_[label]["GenAllAssoc2RecoSingleMatched_SharedTrackPtFractionReco_separated1"] =
        i.book1D("GenAllAssoc2RecoSingleMatched_SharedTrackPtFractionReco_separated1",
                 "GeneratedAllAssoc2RecoSingleMatched_SharedTrackPtFractionReco_separated1",
                 50, 0, 1);
      mes_[label]["GenAllAssoc2RecoSingleMatched_SharedTrackPtFractionRecoMatched_separated1"] =
        i.book1D("GenAllAssoc2RecoSingleMatched_SharedTrackPtFractionRecoMatched_separated1",
                 "GeneratedAllAssoc2RecoSingleMatched_SharedTrackPtFractionRecoMatched_separated1",
                 50, 0, 1);    
      mes_[label]["GenAllAssoc2RecoSingleMatched_SharedTrackWPtFractionReco_separated1"] =
        i.book1D("GenAllAssoc2RecoSingleMatched_SharedTrackWPtFractionReco_separated1",
                 "GeneratedAllAssoc2RecoSingleMatched_SharedTrackWPtFractionReco_separated1",
                 50, 0, 1);
      mes_[label]["GenAllAssoc2RecoSingleMatched_SharedTrackWPtFractionRecoMatched_separated1"] =
        i.book1D("GenAllAssoc2RecoSingleMatched_SharedTrackWPtFractionRecoMatched_separated1",
                 "GeneratedAllAssoc2RecoSingleMatched_SharedTrackWPtFractionRecoMatched_separated1",
                 50, 0, 1);    
      mes_[label]["GenAllAssoc2RecoSingleMatched_SharedTrackPt2FractionReco_separated1"] =
        i.book1D("GenAllAssoc2RecoSingleMatched_SharedTrackPt2FractionReco_separated1",
                 "GeneratedAllAssoc2RecoSingleMatched_SharedTrackPt2FractionReco_separated1",
                 50, 0, 1);
      mes_[label]["GenAllAssoc2RecoSingleMatched_SharedTrackPt2FractionRecoMatched_separated1"] =
        i.book1D("GenAllAssoc2RecoSingleMatched_SharedTrackPt2FractionRecoMatched_separated1",
                 "GeneratedAllAssoc2RecoSingleMatched_SharedTrackPt2FractionRecoMatched_separated1",
                 50, 0, 1);    
      mes_[label]["GenAllAssoc2RecoSingleMatched_SharedTrackWPt2FractionReco_separated1"] =
        i.book1D("GenAllAssoc2RecoSingleMatched_SharedTrackWPt2FractionReco_separated1",
                 "GeneratedAllAssoc2RecoSingleMatched_SharedTrackWPt2FractionReco_separated1",
                 50, 0, 1);
      mes_[label]["GenAllAssoc2RecoSingleMatched_SharedTrackWPt2FractionRecoMatched_separated1"] =
        i.book1D("GenAllAssoc2RecoSingleMatched_SharedTrackWPt2FractionRecoMatched_separated1",
                 "GeneratedAllAssoc2RecoSingleMatched_SharedTrackWPt2FractionRecoMatched_separated1",
                 50, 0, 1);    
      mes_[label]["GenAllAssoc2RecoSingleMatched_SharedTrackFractionSim_separated1"] =
        i.book1D("GenAllAssoc2RecoSingleMatched_SharedTrackFractionSim_separated1",
                 "GeneratedAllAssoc2RecoSingleMatched_SharedTrackFractionSim_separated1",
                 50, 0, 1);
      mes_[label]["GenAllAssoc2RecoSingleMatched_SharedTrackFractionSimMatched_separated1"] =
        i.book1D("GenAllAssoc2RecoSingleMatched_SharedTrackFractionSimMatched_separated1",
                 "GeneratedAllAssoc2RecoSingleMatched_SharedTrackFractionSimMatched_separated1",
                 50, 0, 1);    
      mes_[label]["GenAllAssoc2RecoSingleMatched_SharedTrackPtFractionSim_separated1"] =
        i.book1D("GenAllAssoc2RecoSingleMatched_SharedTrackPtFractionSim_separated1",
                 "GeneratedAllAssoc2RecoSingleMatched_SharedTrackPtFractionSim_separated1",
                 50, 0, 1);
      mes_[label]["GenAllAssoc2RecoSingleMatched_SharedTrackPtFractionSimMatched_separated1"] =
        i.book1D("GenAllAssoc2RecoSingleMatched_SharedTrackPtFractionSimMatched_separated1",
                 "GeneratedAllAssoc2RecoSingleMatched_SharedTrackPtFractionSimMatched_separated1",
                 50, 0, 1);    
      mes_[label]["GenAllAssoc2RecoSingleMatched_SharedTrackPt2FractionSim_separated1"] =
        i.book1D("GenAllAssoc2RecoSingleMatched_SharedTrackPt2FractionSim_separated1",
                 "GeneratedAllAssoc2RecoSingleMatched_SharedTrackPt2FractionSim_separated1",
                 50, 0, 1);
      mes_[label]["GenAllAssoc2RecoSingleMatched_SharedTrackPt2FractionSimMatched_separated1"] =
        i.book1D("GenAllAssoc2RecoSingleMatched_SharedTrackPt2FractionSimMatched_separated1",
                 "GeneratedAllAssoc2RecoSingleMatched_SharedTrackPt2FractionSimMatched_separated1",
                 50, 0, 1);    


      mes_[label]["GenAllAssoc2RecoMultiMatched_SharedTrackFractionReco_separated1"] =
        i.book1D("GenAllAssoc2RecoMultiMatched_SharedTrackFractionReco_separated1",
                 "GeneratedAllAssoc2RecoMultiMatched_SharedTrackFractionReco_separated1",
                 50, 0, 1);
      mes_[label]["GenAllAssoc2RecoMultiMatched_SharedTrackFractionRecoMatched_separated1"] =
        i.book1D("GenAllAssoc2RecoMultiMatched_SharedTrackFractionRecoMatched_separated1",
                 "GeneratedAllAssoc2RecoMultiMatched_SharedTrackFractionRecoMatched_separated1",
                 50, 0, 1);    
      mes_[label]["GenAllAssoc2RecoMultiMatched_SharedTrackWFractionReco_separated1"] =
        i.book1D("GenAllAssoc2RecoMultiMatched_SharedTrackWFractionReco_separated1",
                 "GeneratedAllAssoc2RecoMultiMatched_SharedTrackWFractionReco_separated1",
                 50, 0, 1);
      mes_[label]["GenAllAssoc2RecoMultiMatched_SharedTrackWFractionRecoMatched_separated1"] =
        i.book1D("GenAllAssoc2RecoMultiMatched_SharedTrackWFractionRecoMatched_separated1",
                 "GeneratedAllAssoc2RecoMultiMatched_SharedTrackWFractionRecoMatched_separated1",
                 50, 0, 1);    
      mes_[label]["GenAllAssoc2RecoMultiMatched_SharedTrackPtFractionReco_separated1"] =
        i.book1D("GenAllAssoc2RecoMultiMatched_SharedTrackPtFractionReco_separated1",
                 "GeneratedAllAssoc2RecoMultiMatched_SharedTrackPtFractionReco_separated1",
                 50, 0, 1);
      mes_[label]["GenAllAssoc2RecoMultiMatched_SharedTrackPtFractionRecoMatched_separated1"] =
        i.book1D("GenAllAssoc2RecoMultiMatched_SharedTrackPtFractionRecoMatched_separated1",
                 "GeneratedAllAssoc2RecoMultiMatched_SharedTrackPtFractionRecoMatched_separated1",
                 50, 0, 1);    
      mes_[label]["GenAllAssoc2RecoMultiMatched_SharedTrackWPtFractionReco_separated1"] =
        i.book1D("GenAllAssoc2RecoMultiMatched_SharedTrackWPtFractionReco_separated1",
                 "GeneratedAllAssoc2RecoMultiMatched_SharedTrackWPtFractionReco_separated1",
                 50, 0, 1);
      mes_[label]["GenAllAssoc2RecoMultiMatched_SharedTrackWPtFractionRecoMatched_separated1"] =
        i.book1D("GenAllAssoc2RecoMultiMatched_SharedTrackWPtFractionRecoMatched_separated1",
                 "GeneratedAllAssoc2RecoMultiMatched_SharedTrackWPtFractionRecoMatched_separated1",
                 50, 0, 1);    
      mes_[label]["GenAllAssoc2RecoMultiMatched_SharedTrackPt2FractionReco_separated1"] =
        i.book1D("GenAllAssoc2RecoMultiMatched_SharedTrackPt2FractionReco_separated1",
                 "GeneratedAllAssoc2RecoMultiMatched_SharedTrackPt2FractionReco_separated1",
                 50, 0, 1);
      mes_[label]["GenAllAssoc2RecoMultiMatched_SharedTrackPt2FractionRecoMatched_separated1"] =
        i.book1D("GenAllAssoc2RecoMultiMatched_SharedTrackPt2FractionRecoMatched_separated1",
                 "GeneratedAllAssoc2RecoMultiMatched_SharedTrackPt2FractionRecoMatched_separated1",
                 50, 0, 1);    
      mes_[label]["GenAllAssoc2RecoMultiMatched_SharedTrackWPt2FractionReco_separated1"] =
        i.book1D("GenAllAssoc2RecoMultiMatched_SharedTrackWPt2FractionReco_separated1",
                 "GeneratedAllAssoc2RecoMultiMatched_SharedTrackWPt2FractionReco_separated1",
                 50, 0, 1);
      mes_[label]["GenAllAssoc2RecoMultiMatched_SharedTrackWPt2FractionRecoMatched_separated1"] =
        i.book1D("GenAllAssoc2RecoMultiMatched_SharedTrackWPt2FractionRecoMatched_separated1",
                 "GeneratedAllAssoc2RecoMultiMatched_SharedTrackWPt2FractionRecoMatched_separated1",
                 50, 0, 1);    
      mes_[label]["GenAllAssoc2RecoMultiMatched_SharedTrackFractionSim_separated1"] =
        i.book1D("GenAllAssoc2RecoMultiMatched_SharedTrackFractionSim_separated1",
                 "GeneratedAllAssoc2RecoMultiMatched_SharedTrackFractionSim_separated1",
                 50, 0, 1);
      mes_[label]["GenAllAssoc2RecoMultiMatched_SharedTrackFractionSimMatched_separated1"] =
        i.book1D("GenAllAssoc2RecoMultiMatched_SharedTrackFractionSimMatched_separated1",
                 "GeneratedAllAssoc2RecoMultiMatched_SharedTrackFractionSimMatched_separated1",
                 50, 0, 1);    
      mes_[label]["GenAllAssoc2RecoMultiMatched_SharedTrackPtFractionSim_separated1"] =
        i.book1D("GenAllAssoc2RecoMultiMatched_SharedTrackPtFractionSim_separated1",
                 "GeneratedAllAssoc2RecoMultiMatched_SharedTrackPtFractionSim_separated1",
                 50, 0, 1);
      mes_[label]["GenAllAssoc2RecoMultiMatched_SharedTrackPtFractionSimMatched_separated1"] =
        i.book1D("GenAllAssoc2RecoMultiMatched_SharedTrackPtFractionSimMatched_separated1",
                 "GeneratedAllAssoc2RecoMultiMatched_SharedTrackPtFractionSimMatched_separated1",
                 50, 0, 1);    
      mes_[label]["GenAllAssoc2RecoMultiMatched_SharedTrackPt2FractionSim_separated1"] =
        i.book1D("GenAllAssoc2RecoMultiMatched_SharedTrackPt2FractionSim_separated1",
                 "GeneratedAllAssoc2RecoMultiMatched_SharedTrackPt2FractionSim_separated1",
                 50, 0, 1);
      mes_[label]["GenAllAssoc2RecoMultiMatched_SharedTrackPt2FractionSimMatched_separated1"] =
        i.book1D("GenAllAssoc2RecoMultiMatched_SharedTrackPt2FractionSimMatched_separated1",
                 "GeneratedAllAssoc2RecoMultiMatched_SharedTrackPt2FractionSimMatched_separated1",
                 50, 0, 1);  

      // Gen separated2
      mes_[label]["GenAllAssoc2RecoSingleMatched_SharedTrackFractionReco_separated2"] =
        i.book1D("GenAllAssoc2RecoSingleMatched_SharedTrackFractionReco_separated2",
                 "GeneratedAllAssoc2RecoSingleMatched_SharedTrackFractionReco_separated2",
                 50, 0, 1);
      mes_[label]["GenAllAssoc2RecoSingleMatched_SharedTrackFractionRecoMatched_separated2"] =
        i.book1D("GenAllAssoc2RecoSingleMatched_SharedTrackFractionRecoMatched_separated2",
                 "GeneratedAllAssoc2RecoSingleMatched_SharedTrackFractionRecoMatched_separated2",
                 50, 0, 1);    
      mes_[label]["GenAllAssoc2RecoSingleMatched_SharedTrackWFractionReco_separated2"] =
        i.book1D("GenAllAssoc2RecoSingleMatched_SharedTrackWFractionReco_separated2",
                 "GeneratedAllAssoc2RecoSingleMatched_SharedTrackWFractionReco_separated2",
                 50, 0, 1);
      mes_[label]["GenAllAssoc2RecoSingleMatched_SharedTrackWFractionRecoMatched_separated2"] =
        i.book1D("GenAllAssoc2RecoSingleMatched_SharedTrackWFractionRecoMatched_separated2",
                 "GeneratedAllAssoc2RecoSingleMatched_SharedTrackWFractionRecoMatched_separated2",
                 50, 0, 1);    
      mes_[label]["GenAllAssoc2RecoSingleMatched_SharedTrackPtFractionReco_separated2"] =
        i.book1D("GenAllAssoc2RecoSingleMatched_SharedTrackPtFractionReco_separated2",
                 "GeneratedAllAssoc2RecoSingleMatched_SharedTrackPtFractionReco_separated2",
                 50, 0, 1);
      mes_[label]["GenAllAssoc2RecoSingleMatched_SharedTrackPtFractionRecoMatched_separated2"] =
        i.book1D("GenAllAssoc2RecoSingleMatched_SharedTrackPtFractionRecoMatched_separated2",
                 "GeneratedAllAssoc2RecoSingleMatched_SharedTrackPtFractionRecoMatched_separated2",
                 50, 0, 1);    
      mes_[label]["GenAllAssoc2RecoSingleMatched_SharedTrackWPtFractionReco_separated2"] =
        i.book1D("GenAllAssoc2RecoSingleMatched_SharedTrackWPtFractionReco_separated2",
                 "GeneratedAllAssoc2RecoSingleMatched_SharedTrackWPtFractionReco_separated2",
                 50, 0, 1);
      mes_[label]["GenAllAssoc2RecoSingleMatched_SharedTrackWPtFractionRecoMatched_separated2"] =
        i.book1D("GenAllAssoc2RecoSingleMatched_SharedTrackWPtFractionRecoMatched_separated2",
                 "GeneratedAllAssoc2RecoSingleMatched_SharedTrackWPtFractionRecoMatched_separated2",
                 50, 0, 1);    
      mes_[label]["GenAllAssoc2RecoSingleMatched_SharedTrackPt2FractionReco_separated2"] =
        i.book1D("GenAllAssoc2RecoSingleMatched_SharedTrackPt2FractionReco_separated2",
                 "GeneratedAllAssoc2RecoSingleMatched_SharedTrackPt2FractionReco_separated2",
                 50, 0, 1);
      mes_[label]["GenAllAssoc2RecoSingleMatched_SharedTrackPt2FractionRecoMatched_separated2"] =
        i.book1D("GenAllAssoc2RecoSingleMatched_SharedTrackPt2FractionRecoMatched_separated2",
                 "GeneratedAllAssoc2RecoSingleMatched_SharedTrackPt2FractionRecoMatched_separated2",
                 50, 0, 1);    
      mes_[label]["GenAllAssoc2RecoSingleMatched_SharedTrackWPt2FractionReco_separated2"] =
        i.book1D("GenAllAssoc2RecoSingleMatched_SharedTrackWPt2FractionReco_separated2",
                 "GeneratedAllAssoc2RecoSingleMatched_SharedTrackWPt2FractionReco_separated2",
                 50, 0, 1);
      mes_[label]["GenAllAssoc2RecoSingleMatched_SharedTrackWPt2FractionRecoMatched_separated2"] =
        i.book1D("GenAllAssoc2RecoSingleMatched_SharedTrackWPt2FractionRecoMatched_separated2",
                 "GeneratedAllAssoc2RecoSingleMatched_SharedTrackWPt2FractionRecoMatched_separated2",
                 50, 0, 1);    
      mes_[label]["GenAllAssoc2RecoSingleMatched_SharedTrackFractionSim_separated2"] =
        i.book1D("GenAllAssoc2RecoSingleMatched_SharedTrackFractionSim_separated2",
                 "GeneratedAllAssoc2RecoSingleMatched_SharedTrackFractionSim_separated2",
                 50, 0, 1);
      mes_[label]["GenAllAssoc2RecoSingleMatched_SharedTrackFractionSimMatched_separated2"] =
        i.book1D("GenAllAssoc2RecoSingleMatched_SharedTrackFractionSimMatched_separated2",
                 "GeneratedAllAssoc2RecoSingleMatched_SharedTrackFractionSimMatched_separated2",
                 50, 0, 1);    
      mes_[label]["GenAllAssoc2RecoSingleMatched_SharedTrackPtFractionSim_separated2"] =
        i.book1D("GenAllAssoc2RecoSingleMatched_SharedTrackPtFractionSim_separated2",
                 "GeneratedAllAssoc2RecoSingleMatched_SharedTrackPtFractionSim_separated2",
                 50, 0, 1);
      mes_[label]["GenAllAssoc2RecoSingleMatched_SharedTrackPtFractionSimMatched_separated2"] =
        i.book1D("GenAllAssoc2RecoSingleMatched_SharedTrackPtFractionSimMatched_separated2",
                 "GeneratedAllAssoc2RecoSingleMatched_SharedTrackPtFractionSimMatched_separated2",
                 50, 0, 1);    
      mes_[label]["GenAllAssoc2RecoSingleMatched_SharedTrackPt2FractionSim_separated2"] =
        i.book1D("GenAllAssoc2RecoSingleMatched_SharedTrackPt2FractionSim_separated2",
                 "GeneratedAllAssoc2RecoSingleMatched_SharedTrackPt2FractionSim_separated2",
                 50, 0, 1);
      mes_[label]["GenAllAssoc2RecoSingleMatched_SharedTrackPt2FractionSimMatched_separated2"] =
        i.book1D("GenAllAssoc2RecoSingleMatched_SharedTrackPt2FractionSimMatched_separated2",
                 "GeneratedAllAssoc2RecoSingleMatched_SharedTrackPt2FractionSimMatched_separated2",
                 50, 0, 1);    


      mes_[label]["GenAllAssoc2RecoMultiMatched_SharedTrackFractionReco_separated2"] =
        i.book1D("GenAllAssoc2RecoMultiMatched_SharedTrackFractionReco_separated2",
                 "GeneratedAllAssoc2RecoMultiMatched_SharedTrackFractionReco_separated2",
                 50, 0, 1);
      mes_[label]["GenAllAssoc2RecoMultiMatched_SharedTrackFractionRecoMatched_separated2"] =
        i.book1D("GenAllAssoc2RecoMultiMatched_SharedTrackFractionRecoMatched_separated2",
                 "GeneratedAllAssoc2RecoMultiMatched_SharedTrackFractionRecoMatched_separated2",
                 50, 0, 1);    
      mes_[label]["GenAllAssoc2RecoMultiMatched_SharedTrackWFractionReco_separated2"] =
        i.book1D("GenAllAssoc2RecoMultiMatched_SharedTrackWFractionReco_separated2",
                 "GeneratedAllAssoc2RecoMultiMatched_SharedTrackWFractionReco_separated2",
                 50, 0, 1);
      mes_[label]["GenAllAssoc2RecoMultiMatched_SharedTrackWFractionRecoMatched_separated2"] =
        i.book1D("GenAllAssoc2RecoMultiMatched_SharedTrackWFractionRecoMatched_separated2",
                 "GeneratedAllAssoc2RecoMultiMatched_SharedTrackWFractionRecoMatched_separated2",
                 50, 0, 1);    
      mes_[label]["GenAllAssoc2RecoMultiMatched_SharedTrackPtFractionReco_separated2"] =
        i.book1D("GenAllAssoc2RecoMultiMatched_SharedTrackPtFractionReco_separated2",
                 "GeneratedAllAssoc2RecoMultiMatched_SharedTrackPtFractionReco_separated2",
                 50, 0, 1);
      mes_[label]["GenAllAssoc2RecoMultiMatched_SharedTrackPtFractionRecoMatched_separated2"] =
        i.book1D("GenAllAssoc2RecoMultiMatched_SharedTrackPtFractionRecoMatched_separated2",
                 "GeneratedAllAssoc2RecoMultiMatched_SharedTrackPtFractionRecoMatched_separated2",
                 50, 0, 1);    
      mes_[label]["GenAllAssoc2RecoMultiMatched_SharedTrackWPtFractionReco_separated2"] =
        i.book1D("GenAllAssoc2RecoMultiMatched_SharedTrackWPtFractionReco_separated2",
                 "GeneratedAllAssoc2RecoMultiMatched_SharedTrackWPtFractionReco_separated2",
                 50, 0, 1);
      mes_[label]["GenAllAssoc2RecoMultiMatched_SharedTrackWPtFractionRecoMatched_separated2"] =
        i.book1D("GenAllAssoc2RecoMultiMatched_SharedTrackWPtFractionRecoMatched_separated2",
                 "GeneratedAllAssoc2RecoMultiMatched_SharedTrackWPtFractionRecoMatched_separated2",
                 50, 0, 1);    
      mes_[label]["GenAllAssoc2RecoMultiMatched_SharedTrackPt2FractionReco_separated2"] =
        i.book1D("GenAllAssoc2RecoMultiMatched_SharedTrackPt2FractionReco_separated2",
                 "GeneratedAllAssoc2RecoMultiMatched_SharedTrackPt2FractionReco_separated2",
                 50, 0, 1);
      mes_[label]["GenAllAssoc2RecoMultiMatched_SharedTrackPt2FractionRecoMatched_separated2"] =
        i.book1D("GenAllAssoc2RecoMultiMatched_SharedTrackPt2FractionRecoMatched_separated2",
                 "GeneratedAllAssoc2RecoMultiMatched_SharedTrackPt2FractionRecoMatched_separated2",
                 50, 0, 1);    
      mes_[label]["GenAllAssoc2RecoMultiMatched_SharedTrackWPt2FractionReco_separated2"] =
        i.book1D("GenAllAssoc2RecoMultiMatched_SharedTrackWPt2FractionReco_separated2",
                 "GeneratedAllAssoc2RecoMultiMatched_SharedTrackWPt2FractionReco_separated2",
                 50, 0, 1);
      mes_[label]["GenAllAssoc2RecoMultiMatched_SharedTrackWPt2FractionRecoMatched_separated2"] =
        i.book1D("GenAllAssoc2RecoMultiMatched_SharedTrackWPt2FractionRecoMatched_separated2",
                 "GeneratedAllAssoc2RecoMultiMatched_SharedTrackWPt2FractionRecoMatched_separated2",
                 50, 0, 1);    
      mes_[label]["GenAllAssoc2RecoMultiMatched_SharedTrackFractionSim_separated2"] =
        i.book1D("GenAllAssoc2RecoMultiMatched_SharedTrackFractionSim_separated2",
                 "GeneratedAllAssoc2RecoMultiMatched_SharedTrackFractionSim_separated2",
                 50, 0, 1);
      mes_[label]["GenAllAssoc2RecoMultiMatched_SharedTrackFractionSimMatched_separated2"] =
        i.book1D("GenAllAssoc2RecoMultiMatched_SharedTrackFractionSimMatched_separated2",
                 "GeneratedAllAssoc2RecoMultiMatched_SharedTrackFractionSimMatched_separated2",
                 50, 0, 1);    
      mes_[label]["GenAllAssoc2RecoMultiMatched_SharedTrackPtFractionSim_separated2"] =
        i.book1D("GenAllAssoc2RecoMultiMatched_SharedTrackPtFractionSim_separated2",
                 "GeneratedAllAssoc2RecoMultiMatched_SharedTrackPtFractionSim_separated2",
                 50, 0, 1);
      mes_[label]["GenAllAssoc2RecoMultiMatched_SharedTrackPtFractionSimMatched_separated2"] =
        i.book1D("GenAllAssoc2RecoMultiMatched_SharedTrackPtFractionSimMatched_separated2",
                 "GeneratedAllAssoc2RecoMultiMatched_SharedTrackPtFractionSimMatched_separated2",
                 50, 0, 1);    
      mes_[label]["GenAllAssoc2RecoMultiMatched_SharedTrackPt2FractionSim_separated2"] =
        i.book1D("GenAllAssoc2RecoMultiMatched_SharedTrackPt2FractionSim_separated2",
                 "GeneratedAllAssoc2RecoMultiMatched_SharedTrackPt2FractionSim_separated2",
                 50, 0, 1);
      mes_[label]["GenAllAssoc2RecoMultiMatched_SharedTrackPt2FractionSimMatched_separated2"] =
        i.book1D("GenAllAssoc2RecoMultiMatched_SharedTrackPt2FractionSimMatched_separated2",
                 "GeneratedAllAssoc2RecoMultiMatched_SharedTrackPt2FractionSimMatched_separated2",
                 50, 0, 1);  

      // Gen separated3
      mes_[label]["GenAllAssoc2RecoSingleMatched_SharedTrackFractionReco_separated3"] =
        i.book1D("GenAllAssoc2RecoSingleMatched_SharedTrackFractionReco_separated3",
                 "GeneratedAllAssoc2RecoSingleMatched_SharedTrackFractionReco_separated3",
                 50, 0, 1);
      mes_[label]["GenAllAssoc2RecoSingleMatched_SharedTrackFractionRecoMatched_separated3"] =
        i.book1D("GenAllAssoc2RecoSingleMatched_SharedTrackFractionRecoMatched_separated3",
                 "GeneratedAllAssoc2RecoSingleMatched_SharedTrackFractionRecoMatched_separated3",
                 50, 0, 1);    
      mes_[label]["GenAllAssoc2RecoSingleMatched_SharedTrackWFractionReco_separated3"] =
        i.book1D("GenAllAssoc2RecoSingleMatched_SharedTrackWFractionReco_separated3",
                 "GeneratedAllAssoc2RecoSingleMatched_SharedTrackWFractionReco_separated3",
                 50, 0, 1);
      mes_[label]["GenAllAssoc2RecoSingleMatched_SharedTrackWFractionRecoMatched_separated3"] =
        i.book1D("GenAllAssoc2RecoSingleMatched_SharedTrackWFractionRecoMatched_separated3",
                 "GeneratedAllAssoc2RecoSingleMatched_SharedTrackWFractionRecoMatched_separated3",
                 50, 0, 1);    
      mes_[label]["GenAllAssoc2RecoSingleMatched_SharedTrackPtFractionReco_separated3"] =
        i.book1D("GenAllAssoc2RecoSingleMatched_SharedTrackPtFractionReco_separated3",
                 "GeneratedAllAssoc2RecoSingleMatched_SharedTrackPtFractionReco_separated3",
                 50, 0, 1);
      mes_[label]["GenAllAssoc2RecoSingleMatched_SharedTrackPtFractionRecoMatched_separated3"] =
        i.book1D("GenAllAssoc2RecoSingleMatched_SharedTrackPtFractionRecoMatched_separated3",
                 "GeneratedAllAssoc2RecoSingleMatched_SharedTrackPtFractionRecoMatched_separated3",
                 50, 0, 1);    
      mes_[label]["GenAllAssoc2RecoSingleMatched_SharedTrackWPtFractionReco_separated3"] =
        i.book1D("GenAllAssoc2RecoSingleMatched_SharedTrackWPtFractionReco_separated3",
                 "GeneratedAllAssoc2RecoSingleMatched_SharedTrackWPtFractionReco_separated3",
                 50, 0, 1);
      mes_[label]["GenAllAssoc2RecoSingleMatched_SharedTrackWPtFractionRecoMatched_separated3"] =
        i.book1D("GenAllAssoc2RecoSingleMatched_SharedTrackWPtFractionRecoMatched_separated3",
                 "GeneratedAllAssoc2RecoSingleMatched_SharedTrackWPtFractionRecoMatched_separated3",
                 50, 0, 1);    
      mes_[label]["GenAllAssoc2RecoSingleMatched_SharedTrackPt2FractionReco_separated3"] =
        i.book1D("GenAllAssoc2RecoSingleMatched_SharedTrackPt2FractionReco_separated3",
                 "GeneratedAllAssoc2RecoSingleMatched_SharedTrackPt2FractionReco_separated3",
                 50, 0, 1);
      mes_[label]["GenAllAssoc2RecoSingleMatched_SharedTrackPt2FractionRecoMatched_separated3"] =
        i.book1D("GenAllAssoc2RecoSingleMatched_SharedTrackPt2FractionRecoMatched_separated3",
                 "GeneratedAllAssoc2RecoSingleMatched_SharedTrackPt2FractionRecoMatched_separated3",
                 50, 0, 1);    
      mes_[label]["GenAllAssoc2RecoSingleMatched_SharedTrackWPt2FractionReco_separated3"] =
        i.book1D("GenAllAssoc2RecoSingleMatched_SharedTrackWPt2FractionReco_separated3",
                 "GeneratedAllAssoc2RecoSingleMatched_SharedTrackWPt2FractionReco_separated3",
                 50, 0, 1);
      mes_[label]["GenAllAssoc2RecoSingleMatched_SharedTrackWPt2FractionRecoMatched_separated3"] =
        i.book1D("GenAllAssoc2RecoSingleMatched_SharedTrackWPt2FractionRecoMatched_separated3",
                 "GeneratedAllAssoc2RecoSingleMatched_SharedTrackWPt2FractionRecoMatched_separated3",
                 50, 0, 1);    
      mes_[label]["GenAllAssoc2RecoSingleMatched_SharedTrackFractionSim_separated3"] =
        i.book1D("GenAllAssoc2RecoSingleMatched_SharedTrackFractionSim_separated3",
                 "GeneratedAllAssoc2RecoSingleMatched_SharedTrackFractionSim_separated3",
                 50, 0, 1);
      mes_[label]["GenAllAssoc2RecoSingleMatched_SharedTrackFractionSimMatched_separated3"] =
        i.book1D("GenAllAssoc2RecoSingleMatched_SharedTrackFractionSimMatched_separated3",
                 "GeneratedAllAssoc2RecoSingleMatched_SharedTrackFractionSimMatched_separated3",
                 50, 0, 1);    
      mes_[label]["GenAllAssoc2RecoSingleMatched_SharedTrackPtFractionSim_separated3"] =
        i.book1D("GenAllAssoc2RecoSingleMatched_SharedTrackPtFractionSim_separated3",
                 "GeneratedAllAssoc2RecoSingleMatched_SharedTrackPtFractionSim_separated3",
                 50, 0, 1);
      mes_[label]["GenAllAssoc2RecoSingleMatched_SharedTrackPtFractionSimMatched_separated3"] =
        i.book1D("GenAllAssoc2RecoSingleMatched_SharedTrackPtFractionSimMatched_separated3",
                 "GeneratedAllAssoc2RecoSingleMatched_SharedTrackPtFractionSimMatched_separated3",
                 50, 0, 1);    
      mes_[label]["GenAllAssoc2RecoSingleMatched_SharedTrackPt2FractionSim_separated3"] =
        i.book1D("GenAllAssoc2RecoSingleMatched_SharedTrackPt2FractionSim_separated3",
                 "GeneratedAllAssoc2RecoSingleMatched_SharedTrackPt2FractionSim_separated3",
                 50, 0, 1);
      mes_[label]["GenAllAssoc2RecoSingleMatched_SharedTrackPt2FractionSimMatched_separated3"] =
        i.book1D("GenAllAssoc2RecoSingleMatched_SharedTrackPt2FractionSimMatched_separated3",
                 "GeneratedAllAssoc2RecoSingleMatched_SharedTrackPt2FractionSimMatched_separated3",
                 50, 0, 1);    


      mes_[label]["GenAllAssoc2RecoMultiMatched_SharedTrackFractionReco_separated3"] =
        i.book1D("GenAllAssoc2RecoMultiMatched_SharedTrackFractionReco_separated3",
                 "GeneratedAllAssoc2RecoMultiMatched_SharedTrackFractionReco_separated3",
                 50, 0, 1);
      mes_[label]["GenAllAssoc2RecoMultiMatched_SharedTrackFractionRecoMatched_separated3"] =
        i.book1D("GenAllAssoc2RecoMultiMatched_SharedTrackFractionRecoMatched_separated3",
                 "GeneratedAllAssoc2RecoMultiMatched_SharedTrackFractionRecoMatched_separated3",
                 50, 0, 1);    
      mes_[label]["GenAllAssoc2RecoMultiMatched_SharedTrackWFractionReco_separated3"] =
        i.book1D("GenAllAssoc2RecoMultiMatched_SharedTrackWFractionReco_separated3",
                 "GeneratedAllAssoc2RecoMultiMatched_SharedTrackWFractionReco_separated3",
                 50, 0, 1);
      mes_[label]["GenAllAssoc2RecoMultiMatched_SharedTrackWFractionRecoMatched_separated3"] =
        i.book1D("GenAllAssoc2RecoMultiMatched_SharedTrackWFractionRecoMatched_separated3",
                 "GeneratedAllAssoc2RecoMultiMatched_SharedTrackWFractionRecoMatched_separated3",
                 50, 0, 1);    
      mes_[label]["GenAllAssoc2RecoMultiMatched_SharedTrackPtFractionReco_separated3"] =
        i.book1D("GenAllAssoc2RecoMultiMatched_SharedTrackPtFractionReco_separated3",
                 "GeneratedAllAssoc2RecoMultiMatched_SharedTrackPtFractionReco_separated3",
                 50, 0, 1);
      mes_[label]["GenAllAssoc2RecoMultiMatched_SharedTrackPtFractionRecoMatched_separated3"] =
        i.book1D("GenAllAssoc2RecoMultiMatched_SharedTrackPtFractionRecoMatched_separated3",
                 "GeneratedAllAssoc2RecoMultiMatched_SharedTrackPtFractionRecoMatched_separated3",
                 50, 0, 1);    
      mes_[label]["GenAllAssoc2RecoMultiMatched_SharedTrackWPtFractionReco_separated3"] =
        i.book1D("GenAllAssoc2RecoMultiMatched_SharedTrackWPtFractionReco_separated3",
                 "GeneratedAllAssoc2RecoMultiMatched_SharedTrackWPtFractionReco_separated3",
                 50, 0, 1);
      mes_[label]["GenAllAssoc2RecoMultiMatched_SharedTrackWPtFractionRecoMatched_separated3"] =
        i.book1D("GenAllAssoc2RecoMultiMatched_SharedTrackWPtFractionRecoMatched_separated3",
                 "GeneratedAllAssoc2RecoMultiMatched_SharedTrackWPtFractionRecoMatched_separated3",
                 50, 0, 1);    
      mes_[label]["GenAllAssoc2RecoMultiMatched_SharedTrackPt2FractionReco_separated3"] =
        i.book1D("GenAllAssoc2RecoMultiMatched_SharedTrackPt2FractionReco_separated3",
                 "GeneratedAllAssoc2RecoMultiMatched_SharedTrackPt2FractionReco_separated3",
                 50, 0, 1);
      mes_[label]["GenAllAssoc2RecoMultiMatched_SharedTrackPt2FractionRecoMatched_separated3"] =
        i.book1D("GenAllAssoc2RecoMultiMatched_SharedTrackPt2FractionRecoMatched_separated3",
                 "GeneratedAllAssoc2RecoMultiMatched_SharedTrackPt2FractionRecoMatched_separated3",
                 50, 0, 1);    
      mes_[label]["GenAllAssoc2RecoMultiMatched_SharedTrackWPt2FractionReco_separated3"] =
        i.book1D("GenAllAssoc2RecoMultiMatched_SharedTrackWPt2FractionReco_separated3",
                 "GeneratedAllAssoc2RecoMultiMatched_SharedTrackWPt2FractionReco_separated3",
                 50, 0, 1);
      mes_[label]["GenAllAssoc2RecoMultiMatched_SharedTrackWPt2FractionRecoMatched_separated3"] =
        i.book1D("GenAllAssoc2RecoMultiMatched_SharedTrackWPt2FractionRecoMatched_separated3",
                 "GeneratedAllAssoc2RecoMultiMatched_SharedTrackWPt2FractionRecoMatched_separated3",
                 50, 0, 1);    
      mes_[label]["GenAllAssoc2RecoMultiMatched_SharedTrackFractionSim_separated3"] =
        i.book1D("GenAllAssoc2RecoMultiMatched_SharedTrackFractionSim_separated3",
                 "GeneratedAllAssoc2RecoMultiMatched_SharedTrackFractionSim_separated3",
                 50, 0, 1);
      mes_[label]["GenAllAssoc2RecoMultiMatched_SharedTrackFractionSimMatched_separated3"] =
        i.book1D("GenAllAssoc2RecoMultiMatched_SharedTrackFractionSimMatched_separated3",
                 "GeneratedAllAssoc2RecoMultiMatched_SharedTrackFractionSimMatched_separated3",
                 50, 0, 1);    
      mes_[label]["GenAllAssoc2RecoMultiMatched_SharedTrackPtFractionSim_separated3"] =
        i.book1D("GenAllAssoc2RecoMultiMatched_SharedTrackPtFractionSim_separated3",
                 "GeneratedAllAssoc2RecoMultiMatched_SharedTrackPtFractionSim_separated3",
                 50, 0, 1);
      mes_[label]["GenAllAssoc2RecoMultiMatched_SharedTrackPtFractionSimMatched_separated3"] =
        i.book1D("GenAllAssoc2RecoMultiMatched_SharedTrackPtFractionSimMatched_separated3",
                 "GeneratedAllAssoc2RecoMultiMatched_SharedTrackPtFractionSimMatched_separated3",
                 50, 0, 1);    
      mes_[label]["GenAllAssoc2RecoMultiMatched_SharedTrackPt2FractionSim_separated3"] =
        i.book1D("GenAllAssoc2RecoMultiMatched_SharedTrackPt2FractionSim_separated3",
                 "GeneratedAllAssoc2RecoMultiMatched_SharedTrackPt2FractionSim_separated3",
                 50, 0, 1);
      mes_[label]["GenAllAssoc2RecoMultiMatched_SharedTrackPt2FractionSimMatched_separated3"] =
        i.book1D("GenAllAssoc2RecoMultiMatched_SharedTrackPt2FractionSimMatched_separated3",
                 "GeneratedAllAssoc2RecoMultiMatched_SharedTrackPt2FractionSimMatched_separated3",
                 50, 0, 1);  


      // Reco
      book1d("RecoAllAssoc2GenSingleMatched_SharedTrackWFractionReco", 50, 0, 1);
      book1d("RecoAllAssoc2GenSingleMatched_SharedTrackPtFractionReco", 50, 0, 1);
      book1d("RecoAllAssoc2GenSingleMatched_SharedTrackWPtFractionReco", 50, 0, 1);
      book1d("RecoAllAssoc2GenSingleMatched_SharedTrackPt2FractionReco", 50, 0, 1);
      book1d("RecoAllAssoc2GenSingleMatched_SharedTrackWPt2FractionReco", 50, 0, 1);
      book1d("RecoAllAssoc2GenSingleMatched_SharedTrackHarmPtFractionReco", 50, 0, 1);
      book1d("RecoAllAssoc2GenSingleMatched_SharedTrackWHarmPtFractionReco", 50, 0, 1);
      book1d("RecoAllAssoc2GenSingleMatched_SharedTrackHarmWPtFractionReco", 50, 0, 1);
      book1d("RecoAllAssoc2GenSingleMatched_SharedTrackHarmPtAvgFractionReco", 50, 0, 1);
      book1d("RecoAllAssoc2GenSingleMatched_SharedTrackWHarmPtAvgFractionReco", 50, 0, 1);
      book1d("RecoAllAssoc2GenSingleMatched_SharedTrackHarmWPtAvgFractionReco", 50, 0, 1);

      book1d("RecoAllAssoc2GenMultiMatched_SharedTrackWFractionReco", 50, 0, 1);
      book1d("RecoAllAssoc2GenMultiMatched_SharedTrackPtFractionReco", 50, 0, 1);
      book1d("RecoAllAssoc2GenMultiMatched_SharedTrackWPtFractionReco", 50, 0, 1);
      book1d("RecoAllAssoc2GenMultiMatched_SharedTrackPt2FractionReco", 50, 0, 1);
      book1d("RecoAllAssoc2GenMultiMatched_SharedTrackWPt2FractionReco", 50, 0, 1);
      book1d("RecoAllAssoc2GenMultiMatched_SharedTrackHarmPtFractionReco", 50, 0, 1);
      book1d("RecoAllAssoc2GenMultiMatched_SharedTrackWHarmPtFractionReco", 50, 0, 1);
      book1d("RecoAllAssoc2GenMultiMatched_SharedTrackHarmWPtFractionReco", 50, 0, 1);
      book1d("RecoAllAssoc2GenMultiMatched_SharedTrackHarmPtAvgFractionReco", 50, 0, 1);
      book1d("RecoAllAssoc2GenMultiMatched_SharedTrackWHarmPtAvgFractionReco", 50, 0, 1);
      book1d("RecoAllAssoc2GenMultiMatched_SharedTrackHarmWPtAvgFractionReco", 50, 0, 1);

      book1d("RecoAllAssoc2GenSingleMatched_SharedTrackWFractionRecoMatched", 50, 0, 1);
      book1d("RecoAllAssoc2GenSingleMatched_SharedTrackPtFractionRecoMatched", 50, 0, 1);
      book1d("RecoAllAssoc2GenSingleMatched_SharedTrackWPtFractionRecoMatched", 50, 0, 1);
      book1d("RecoAllAssoc2GenSingleMatched_SharedTrackPt2FractionRecoMatched", 50, 0, 1);
      book1d("RecoAllAssoc2GenSingleMatched_SharedTrackWPt2FractionRecoMatched", 50, 0, 1);
      book1d("RecoAllAssoc2GenSingleMatched_SharedTrackHarmPtFractionRecoMatched", 50, 0, 1);
      book1d("RecoAllAssoc2GenSingleMatched_SharedTrackWHarmPtFractionRecoMatched", 50, 0, 1);
      book1d("RecoAllAssoc2GenSingleMatched_SharedTrackHarmWPtFractionRecoMatched", 50, 0, 1);
      book1d("RecoAllAssoc2GenSingleMatched_SharedTrackHarmPtAvgFractionRecoMatched", 50, 0, 1);
      book1d("RecoAllAssoc2GenSingleMatched_SharedTrackWHarmPtAvgFractionRecoMatched", 50, 0, 1);
      book1d("RecoAllAssoc2GenSingleMatched_SharedTrackHarmWPtAvgFractionRecoMatched", 50, 0, 1);

      book1d("RecoAllAssoc2GenMultiMatched_SharedTrackWFractionRecoMatched", 50, 0, 1);
      book1d("RecoAllAssoc2GenMultiMatched_SharedTrackPtFractionRecoMatched", 50, 0, 1);
      book1d("RecoAllAssoc2GenMultiMatched_SharedTrackWPtFractionRecoMatched", 50, 0, 1);
      book1d("RecoAllAssoc2GenMultiMatched_SharedTrackPt2FractionRecoMatched", 50, 0, 1);
      book1d("RecoAllAssoc2GenMultiMatched_SharedTrackWPt2FractionRecoMatched", 50, 0, 1);
      book1d("RecoAllAssoc2GenMultiMatched_SharedTrackHarmPtFractionRecoMatched", 50, 0, 1);
      book1d("RecoAllAssoc2GenMultiMatched_SharedTrackWHarmPtFractionRecoMatched", 50, 0, 1);
      book1d("RecoAllAssoc2GenMultiMatched_SharedTrackHarmWPtFractionRecoMatched", 50, 0, 1);
      book1d("RecoAllAssoc2GenMultiMatched_SharedTrackHarmPtAvgFractionRecoMatched", 50, 0, 1);
      book1d("RecoAllAssoc2GenMultiMatched_SharedTrackWHarmPtAvgFractionRecoMatched", 50, 0, 1);
      book1d("RecoAllAssoc2GenMultiMatched_SharedTrackHarmWPtAvgFractionRecoMatched", 50, 0, 1);

      book1d("RecoAllAssoc2GenSingleMatched_SharedTrackWFractionSim", 50, 0, 1);
      book1d("RecoAllAssoc2GenSingleMatched_SharedTrackPtFractionSim", 50, 0, 1);
      book1d("RecoAllAssoc2GenSingleMatched_SharedTrackWPtFractionSim", 50, 0, 1);
      book1d("RecoAllAssoc2GenSingleMatched_SharedTrackPt2FractionSim", 50, 0, 1);
      book1d("RecoAllAssoc2GenSingleMatched_SharedTrackWPt2FractionSim", 50, 0, 1);    
      book1d("RecoAllAssoc2GenMultiMatched_SharedTrackWFractionSim", 50, 0, 1);
      book1d("RecoAllAssoc2GenMultiMatched_SharedTrackPtFractionSim", 50, 0, 1);
      book1d("RecoAllAssoc2GenMultiMatched_SharedTrackWPtFractionSim", 50, 0, 1);
      book1d("RecoAllAssoc2GenMultiMatched_SharedTrackPt2FractionSim", 50, 0, 1);
      book1d("RecoAllAssoc2GenMultiMatched_SharedTrackWPt2FractionSim", 50, 0, 1);
      book1d("RecoAllAssoc2GenSingleMatched_SharedTrackWFractionSimMatched", 50, 0, 1);
      book1d("RecoAllAssoc2GenSingleMatched_SharedTrackPtFractionSimMatched", 50, 0, 1);
      book1d("RecoAllAssoc2GenSingleMatched_SharedTrackWPtFractionSimMatched", 50, 0, 1);
      book1d("RecoAllAssoc2GenSingleMatched_SharedTrackPt2FractionSimMatched", 50, 0, 1);
      book1d("RecoAllAssoc2GenSingleMatched_SharedTrackWPt2FractionSimMatched", 50, 0, 1);    
      book1d("RecoAllAssoc2GenMultiMatched_SharedTrackWFractionSimMatched", 50, 0, 1);
      book1d("RecoAllAssoc2GenMultiMatched_SharedTrackPtFractionSimMatched", 50, 0, 1);
      book1d("RecoAllAssoc2GenMultiMatched_SharedTrackWPtFractionSimMatched", 50, 0, 1);
      book1d("RecoAllAssoc2GenMultiMatched_SharedTrackPt2FractionSimMatched", 50, 0, 1);
      book1d("RecoAllAssoc2GenMultiMatched_SharedTrackWPt2FractionSimMatched", 50, 0, 1);


      // Separated sharedTrack plots 1
        
      book1d("RecoAllAssoc2GenSingleMatched_SharedTrackFractionReco_separated1", 50, 0, 1);
      book1d("RecoAllAssoc2GenSingleMatched_SharedTrackWFractionReco_separated1", 50, 0, 1);
      book1d("RecoAllAssoc2GenSingleMatched_SharedTrackPtFractionReco_separated1", 50, 0, 1);
      book1d("RecoAllAssoc2GenSingleMatched_SharedTrackWPtFractionReco_separated1", 50, 0, 1);
      book1d("RecoAllAssoc2GenSingleMatched_SharedTrackPt2FractionReco_separated1", 50, 0, 1);
      book1d("RecoAllAssoc2GenSingleMatched_SharedTrackWPt2FractionReco_separated1", 50, 0, 1);
      book1d("RecoAllAssoc2GenSingleMatched_SharedTrackHarmPtFractionReco_separated1", 50, 0, 1);
      book1d("RecoAllAssoc2GenSingleMatched_SharedTrackWHarmPtFractionReco_separated1", 50, 0, 1);
      book1d("RecoAllAssoc2GenSingleMatched_SharedTrackHarmWPtFractionReco_separated1", 50, 0, 1);
      book1d("RecoAllAssoc2GenSingleMatched_SharedTrackHarmPtAvgFractionReco_separated1", 50, 0, 1);
      book1d("RecoAllAssoc2GenSingleMatched_SharedTrackWHarmPtAvgFractionReco_separated1", 50, 0, 1);
      book1d("RecoAllAssoc2GenSingleMatched_SharedTrackHarmWPtAvgFractionReco_separated1", 50, 0, 1);

      book1d("RecoAllAssoc2GenMultiMatched_SharedTrackFractionReco_separated1", 50, 0, 1);
      book1d("RecoAllAssoc2GenMultiMatched_SharedTrackWFractionReco_separated1", 50, 0, 1);
      book1d("RecoAllAssoc2GenMultiMatched_SharedTrackPtFractionReco_separated1", 50, 0, 1);
      book1d("RecoAllAssoc2GenMultiMatched_SharedTrackWPtFractionReco_separated1", 50, 0, 1);
      book1d("RecoAllAssoc2GenMultiMatched_SharedTrackPt2FractionReco_separated1", 50, 0, 1);
      book1d("RecoAllAssoc2GenMultiMatched_SharedTrackWPt2FractionReco_separated1", 50, 0, 1);
      book1d("RecoAllAssoc2GenMultiMatched_SharedTrackHarmPtFractionReco_separated1", 50, 0, 1);
      book1d("RecoAllAssoc2GenMultiMatched_SharedTrackWHarmPtFractionReco_separated1", 50, 0, 1);
      book1d("RecoAllAssoc2GenMultiMatched_SharedTrackHarmWPtFractionReco_separated1", 50, 0, 1);
      book1d("RecoAllAssoc2GenMultiMatched_SharedTrackHarmPtAvgFractionReco_separated1", 50, 0, 1);
      book1d("RecoAllAssoc2GenMultiMatched_SharedTrackWHarmPtAvgFractionReco_separated1", 50, 0, 1);
      book1d("RecoAllAssoc2GenMultiMatched_SharedTrackHarmWPtAvgFractionReco_separated1", 50, 0, 1);

      book1d("RecoAllAssoc2GenSingleMatched_SharedTrackFractionRecoMatched_separated1", 50, 0, 1);
      book1d("RecoAllAssoc2GenSingleMatched_SharedTrackWFractionRecoMatched_separated1", 50, 0, 1);
      book1d("RecoAllAssoc2GenSingleMatched_SharedTrackPtFractionRecoMatched_separated1", 50, 0, 1);
      book1d("RecoAllAssoc2GenSingleMatched_SharedTrackWPtFractionRecoMatched_separated1", 50, 0, 1);
      book1d("RecoAllAssoc2GenSingleMatched_SharedTrackPt2FractionRecoMatched_separated1", 50, 0, 1);
      book1d("RecoAllAssoc2GenSingleMatched_SharedTrackWPt2FractionRecoMatched_separated1", 50, 0, 1);
      book1d("RecoAllAssoc2GenSingleMatched_SharedTrackHarmPtFractionRecoMatched_separated1", 50, 0, 1);
      book1d("RecoAllAssoc2GenSingleMatched_SharedTrackWHarmPtFractionRecoMatched_separated1", 50, 0, 1);
      book1d("RecoAllAssoc2GenSingleMatched_SharedTrackHarmWPtFractionRecoMatched_separated1", 50, 0, 1);
      book1d("RecoAllAssoc2GenSingleMatched_SharedTrackHarmPtAvgFractionRecoMatched_separated1", 50, 0, 1);
      book1d("RecoAllAssoc2GenSingleMatched_SharedTrackWHarmPtAvgFractionRecoMatched_separated1", 50, 0, 1);
      book1d("RecoAllAssoc2GenSingleMatched_SharedTrackHarmWPtAvgFractionRecoMatched_separated1", 50, 0, 1);

      book1d("RecoAllAssoc2GenMultiMatched_SharedTrackFractionRecoMatched_separated1", 50, 0, 1);
      book1d("RecoAllAssoc2GenMultiMatched_SharedTrackWFractionRecoMatched_separated1", 50, 0, 1);
      book1d("RecoAllAssoc2GenMultiMatched_SharedTrackPtFractionRecoMatched_separated1", 50, 0, 1);
      book1d("RecoAllAssoc2GenMultiMatched_SharedTrackWPtFractionRecoMatched_separated1", 50, 0, 1);
      book1d("RecoAllAssoc2GenMultiMatched_SharedTrackPt2FractionRecoMatched_separated1", 50, 0, 1);
      book1d("RecoAllAssoc2GenMultiMatched_SharedTrackWPt2FractionRecoMatched_separated1", 50, 0, 1);
      book1d("RecoAllAssoc2GenMultiMatched_SharedTrackHarmPtFractionRecoMatched_separated1", 50, 0, 1);
      book1d("RecoAllAssoc2GenMultiMatched_SharedTrackWHarmPtFractionRecoMatched_separated1", 50, 0, 1);
      book1d("RecoAllAssoc2GenMultiMatched_SharedTrackHarmWPtFractionRecoMatched_separated1", 50, 0, 1);
      book1d("RecoAllAssoc2GenMultiMatched_SharedTrackHarmPtAvgFractionRecoMatched_separated1", 50, 0, 1);
      book1d("RecoAllAssoc2GenMultiMatched_SharedTrackWHarmPtAvgFractionRecoMatched_separated1", 50, 0, 1);
      book1d("RecoAllAssoc2GenMultiMatched_SharedTrackHarmWPtAvgFractionRecoMatched_separated1", 50, 0, 1);

      book1d("RecoAllAssoc2GenSingleMatched_SharedTrackFractionSim_separated1", 50, 0, 1);
      book1d("RecoAllAssoc2GenSingleMatched_SharedTrackWFractionSim_separated1", 50, 0, 1);
      book1d("RecoAllAssoc2GenSingleMatched_SharedTrackPtFractionSim_separated1", 50, 0, 1);
      book1d("RecoAllAssoc2GenSingleMatched_SharedTrackWPtFractionSim_separated1", 50, 0, 1);
      book1d("RecoAllAssoc2GenSingleMatched_SharedTrackPt2FractionSim_separated1", 50, 0, 1);
      book1d("RecoAllAssoc2GenSingleMatched_SharedTrackWPt2FractionSim_separated1", 50, 0, 1);    
      book1d("RecoAllAssoc2GenMultiMatched_SharedTrackFractionSim_separated1", 50, 0, 1);
      book1d("RecoAllAssoc2GenMultiMatched_SharedTrackWFractionSim_separated1", 50, 0, 1);
      book1d("RecoAllAssoc2GenMultiMatched_SharedTrackPtFractionSim_separated1", 50, 0, 1);
      book1d("RecoAllAssoc2GenMultiMatched_SharedTrackWPtFractionSim_separated1", 50, 0, 1);
      book1d("RecoAllAssoc2GenMultiMatched_SharedTrackPt2FractionSim_separated1", 50, 0, 1);
      book1d("RecoAllAssoc2GenMultiMatched_SharedTrackWPt2FractionSim_separated1", 50, 0, 1);
      book1d("RecoAllAssoc2GenSingleMatched_SharedTrackFractionSimMatched_separated1", 50, 0, 1);
      book1d("RecoAllAssoc2GenSingleMatched_SharedTrackWFractionSimMatched_separated1", 50, 0, 1);
      book1d("RecoAllAssoc2GenSingleMatched_SharedTrackPtFractionSimMatched_separated1", 50, 0, 1);
      book1d("RecoAllAssoc2GenSingleMatched_SharedTrackWPtFractionSimMatched_separated1", 50, 0, 1);
      book1d("RecoAllAssoc2GenSingleMatched_SharedTrackPt2FractionSimMatched_separated1", 50, 0, 1);
      book1d("RecoAllAssoc2GenSingleMatched_SharedTrackWPt2FractionSimMatched_separated1", 50, 0, 1);    
      book1d("RecoAllAssoc2GenMultiMatched_SharedTrackFractionSimMatched_separated1", 50, 0, 1);
      book1d("RecoAllAssoc2GenMultiMatched_SharedTrackWFractionSimMatched_separated1", 50, 0, 1);
      book1d("RecoAllAssoc2GenMultiMatched_SharedTrackPtFractionSimMatched_separated1", 50, 0, 1);
      book1d("RecoAllAssoc2GenMultiMatched_SharedTrackWPtFractionSimMatched_separated1", 50, 0, 1);
      book1d("RecoAllAssoc2GenMultiMatched_SharedTrackPt2FractionSimMatched_separated1", 50, 0, 1);
      book1d("RecoAllAssoc2GenMultiMatched_SharedTrackWPt2FractionSimMatched_separated1", 50, 0, 1);
       

      // Separated sharedTrack 2
      
      book1d("RecoAllAssoc2GenSingleMatched_SharedTrackFractionReco_separated2", 50, 0, 1);
      book1d("RecoAllAssoc2GenSingleMatched_SharedTrackWFractionReco_separated2", 50, 0, 1);
      book1d("RecoAllAssoc2GenSingleMatched_SharedTrackPtFractionReco_separated2", 50, 0, 1);
      book1d("RecoAllAssoc2GenSingleMatched_SharedTrackWPtFractionReco_separated2", 50, 0, 1);
      book1d("RecoAllAssoc2GenSingleMatched_SharedTrackPt2FractionReco_separated2", 50, 0, 1);
      book1d("RecoAllAssoc2GenSingleMatched_SharedTrackWPt2FractionReco_separated2", 50, 0, 1);
      book1d("RecoAllAssoc2GenSingleMatched_SharedTrackHarmPtFractionReco_separated2", 50, 0, 1);
      book1d("RecoAllAssoc2GenSingleMatched_SharedTrackWHarmPtFractionReco_separated2", 50, 0, 1);
      book1d("RecoAllAssoc2GenSingleMatched_SharedTrackHarmWPtFractionReco_separated2", 50, 0, 1);
      book1d("RecoAllAssoc2GenSingleMatched_SharedTrackHarmPtAvgFractionReco_separated2", 50, 0, 1);
      book1d("RecoAllAssoc2GenSingleMatched_SharedTrackWHarmPtAvgFractionReco_separated2", 50, 0, 1);
      book1d("RecoAllAssoc2GenSingleMatched_SharedTrackHarmWPtAvgFractionReco_separated2", 50, 0, 1);

      book1d("RecoAllAssoc2GenMultiMatched_SharedTrackFractionReco_separated2", 50, 0, 1);
      book1d("RecoAllAssoc2GenMultiMatched_SharedTrackWFractionReco_separated2", 50, 0, 1);
      book1d("RecoAllAssoc2GenMultiMatched_SharedTrackPtFractionReco_separated2", 50, 0, 1);
      book1d("RecoAllAssoc2GenMultiMatched_SharedTrackWPtFractionReco_separated2", 50, 0, 1);
      book1d("RecoAllAssoc2GenMultiMatched_SharedTrackPt2FractionReco_separated2", 50, 0, 1);
      book1d("RecoAllAssoc2GenMultiMatched_SharedTrackWPt2FractionReco_separated2", 50, 0, 1);
      book1d("RecoAllAssoc2GenMultiMatched_SharedTrackHarmPtFractionReco_separated2", 50, 0, 1);
      book1d("RecoAllAssoc2GenMultiMatched_SharedTrackWHarmPtFractionReco_separated2", 50, 0, 1);
      book1d("RecoAllAssoc2GenMultiMatched_SharedTrackHarmWPtFractionReco_separated2", 50, 0, 1);
      book1d("RecoAllAssoc2GenMultiMatched_SharedTrackHarmPtAvgFractionReco_separated2", 50, 0, 1);
      book1d("RecoAllAssoc2GenMultiMatched_SharedTrackWHarmPtAvgFractionReco_separated2", 50, 0, 1);
      book1d("RecoAllAssoc2GenMultiMatched_SharedTrackHarmWPtAvgFractionReco_separated2", 50, 0, 1);

      book1d("RecoAllAssoc2GenSingleMatched_SharedTrackFractionRecoMatched_separated2", 50, 0, 1);
      book1d("RecoAllAssoc2GenSingleMatched_SharedTrackWFractionRecoMatched_separated2", 50, 0, 1);
      book1d("RecoAllAssoc2GenSingleMatched_SharedTrackPtFractionRecoMatched_separated2", 50, 0, 1);
      book1d("RecoAllAssoc2GenSingleMatched_SharedTrackWPtFractionRecoMatched_separated2", 50, 0, 1);
      book1d("RecoAllAssoc2GenSingleMatched_SharedTrackPt2FractionRecoMatched_separated2", 50, 0, 1);
      book1d("RecoAllAssoc2GenSingleMatched_SharedTrackWPt2FractionRecoMatched_separated2", 50, 0, 1);
      book1d("RecoAllAssoc2GenSingleMatched_SharedTrackHarmPtFractionRecoMatched_separated2", 50, 0, 1);
      book1d("RecoAllAssoc2GenSingleMatched_SharedTrackWHarmPtFractionRecoMatched_separated2", 50, 0, 1);
      book1d("RecoAllAssoc2GenSingleMatched_SharedTrackHarmWPtFractionRecoMatched_separated2", 50, 0, 1);
      book1d("RecoAllAssoc2GenSingleMatched_SharedTrackHarmPtAvgFractionRecoMatched_separated2", 50, 0, 1);
      book1d("RecoAllAssoc2GenSingleMatched_SharedTrackWHarmPtAvgFractionRecoMatched_separated2", 50, 0, 1);
      book1d("RecoAllAssoc2GenSingleMatched_SharedTrackHarmWPtAvgFractionRecoMatched_separated2", 50, 0, 1);

      book1d("RecoAllAssoc2GenMultiMatched_SharedTrackFractionRecoMatched_separated2", 50, 0, 1);
      book1d("RecoAllAssoc2GenMultiMatched_SharedTrackWFractionRecoMatched_separated2", 50, 0, 1);
      book1d("RecoAllAssoc2GenMultiMatched_SharedTrackPtFractionRecoMatched_separated2", 50, 0, 1);
      book1d("RecoAllAssoc2GenMultiMatched_SharedTrackWPtFractionRecoMatched_separated2", 50, 0, 1);
      book1d("RecoAllAssoc2GenMultiMatched_SharedTrackPt2FractionRecoMatched_separated2", 50, 0, 1);
      book1d("RecoAllAssoc2GenMultiMatched_SharedTrackWPt2FractionRecoMatched_separated2", 50, 0, 1);
      book1d("RecoAllAssoc2GenMultiMatched_SharedTrackHarmPtFractionRecoMatched_separated2", 50, 0, 1);
      book1d("RecoAllAssoc2GenMultiMatched_SharedTrackWHarmPtFractionRecoMatched_separated2", 50, 0, 1);
      book1d("RecoAllAssoc2GenMultiMatched_SharedTrackHarmWPtFractionRecoMatched_separated2", 50, 0, 1);
      book1d("RecoAllAssoc2GenMultiMatched_SharedTrackHarmPtAvgFractionRecoMatched_separated2", 50, 0, 1);
      book1d("RecoAllAssoc2GenMultiMatched_SharedTrackWHarmPtAvgFractionRecoMatched_separated2", 50, 0, 1);
      book1d("RecoAllAssoc2GenMultiMatched_SharedTrackHarmWPtAvgFractionRecoMatched_separated2", 50, 0, 1);

      book1d("RecoAllAssoc2GenSingleMatched_SharedTrackFractionSim_separated2", 50, 0, 1);
      book1d("RecoAllAssoc2GenSingleMatched_SharedTrackWFractionSim_separated2", 50, 0, 1);
      book1d("RecoAllAssoc2GenSingleMatched_SharedTrackPtFractionSim_separated2", 50, 0, 1);
      book1d("RecoAllAssoc2GenSingleMatched_SharedTrackWPtFractionSim_separated2", 50, 0, 1);
      book1d("RecoAllAssoc2GenSingleMatched_SharedTrackPt2FractionSim_separated2", 50, 0, 1);
      book1d("RecoAllAssoc2GenSingleMatched_SharedTrackWPt2FractionSim_separated2", 50, 0, 1);    
      book1d("RecoAllAssoc2GenMultiMatched_SharedTrackFractionSim_separated2", 50, 0, 1);
      book1d("RecoAllAssoc2GenMultiMatched_SharedTrackWFractionSim_separated2", 50, 0, 1);
      book1d("RecoAllAssoc2GenMultiMatched_SharedTrackPtFractionSim_separated2", 50, 0, 1);
      book1d("RecoAllAssoc2GenMultiMatched_SharedTrackWPtFractionSim_separated2", 50, 0, 1);
      book1d("RecoAllAssoc2GenMultiMatched_SharedTrackPt2FractionSim_separated2", 50, 0, 1);
      book1d("RecoAllAssoc2GenMultiMatched_SharedTrackWPt2FractionSim_separated2", 50, 0, 1);
      book1d("RecoAllAssoc2GenSingleMatched_SharedTrackFractionSimMatched_separated2", 50, 0, 1);
      book1d("RecoAllAssoc2GenSingleMatched_SharedTrackWFractionSimMatched_separated2", 50, 0, 1);
      book1d("RecoAllAssoc2GenSingleMatched_SharedTrackPtFractionSimMatched_separated2", 50, 0, 1);
      book1d("RecoAllAssoc2GenSingleMatched_SharedTrackWPtFractionSimMatched_separated2", 50, 0, 1);
      book1d("RecoAllAssoc2GenSingleMatched_SharedTrackPt2FractionSimMatched_separated2", 50, 0, 1);
      book1d("RecoAllAssoc2GenSingleMatched_SharedTrackWPt2FractionSimMatched_separated2", 50, 0, 1);    
      book1d("RecoAllAssoc2GenMultiMatched_SharedTrackFractionSimMatched_separated2", 50, 0, 1);
      book1d("RecoAllAssoc2GenMultiMatched_SharedTrackWFractionSimMatched_separated2", 50, 0, 1);
      book1d("RecoAllAssoc2GenMultiMatched_SharedTrackPtFractionSimMatched_separated2", 50, 0, 1);
      book1d("RecoAllAssoc2GenMultiMatched_SharedTrackWPtFractionSimMatched_separated2", 50, 0, 1);
      book1d("RecoAllAssoc2GenMultiMatched_SharedTrackPt2FractionSimMatched_separated2", 50, 0, 1);
      book1d("RecoAllAssoc2GenMultiMatched_SharedTrackWPt2FractionSimMatched_separated2", 50, 0, 1);
      
      // Separated sharedTrack 3
      
      book1d("RecoAllAssoc2GenSingleMatched_SharedTrackFractionReco_separated3", 50, 0, 1);
      book1d("RecoAllAssoc2GenSingleMatched_SharedTrackWFractionReco_separated3", 50, 0, 1);
      book1d("RecoAllAssoc2GenSingleMatched_SharedTrackPtFractionReco_separated3", 50, 0, 1);
      book1d("RecoAllAssoc2GenSingleMatched_SharedTrackWPtFractionReco_separated3", 50, 0, 1);
      book1d("RecoAllAssoc2GenSingleMatched_SharedTrackPt2FractionReco_separated3", 50, 0, 1);
      book1d("RecoAllAssoc2GenSingleMatched_SharedTrackWPt2FractionReco_separated3", 50, 0, 1);
      book1d("RecoAllAssoc2GenSingleMatched_SharedTrackHarmPtFractionReco_separated3", 50, 0, 1);
      book1d("RecoAllAssoc2GenSingleMatched_SharedTrackWHarmPtFractionReco_separated3", 50, 0, 1);
      book1d("RecoAllAssoc2GenSingleMatched_SharedTrackHarmWPtFractionReco_separated3", 50, 0, 1);
      book1d("RecoAllAssoc2GenSingleMatched_SharedTrackHarmPtAvgFractionReco_separated3", 50, 0, 1);
      book1d("RecoAllAssoc2GenSingleMatched_SharedTrackWHarmPtAvgFractionReco_separated3", 50, 0, 1);
      book1d("RecoAllAssoc2GenSingleMatched_SharedTrackHarmWPtAvgFractionReco_separated3", 50, 0, 1);

      book1d("RecoAllAssoc2GenMultiMatched_SharedTrackFractionReco_separated3", 50, 0, 1);
      book1d("RecoAllAssoc2GenMultiMatched_SharedTrackWFractionReco_separated3", 50, 0, 1);
      book1d("RecoAllAssoc2GenMultiMatched_SharedTrackPtFractionReco_separated3", 50, 0, 1);
      book1d("RecoAllAssoc2GenMultiMatched_SharedTrackWPtFractionReco_separated3", 50, 0, 1);
      book1d("RecoAllAssoc2GenMultiMatched_SharedTrackPt2FractionReco_separated3", 50, 0, 1);
      book1d("RecoAllAssoc2GenMultiMatched_SharedTrackWPt2FractionReco_separated3", 50, 0, 1);
      book1d("RecoAllAssoc2GenMultiMatched_SharedTrackHarmPtFractionReco_separated3", 50, 0, 1);
      book1d("RecoAllAssoc2GenMultiMatched_SharedTrackWHarmPtFractionReco_separated3", 50, 0, 1);
      book1d("RecoAllAssoc2GenMultiMatched_SharedTrackHarmWPtFractionReco_separated3", 50, 0, 1);
      book1d("RecoAllAssoc2GenMultiMatched_SharedTrackHarmPtAvgFractionReco_separated3", 50, 0, 1);
      book1d("RecoAllAssoc2GenMultiMatched_SharedTrackWHarmPtAvgFractionReco_separated3", 50, 0, 1);
      book1d("RecoAllAssoc2GenMultiMatched_SharedTrackHarmWPtAvgFractionReco_separated3", 50, 0, 1);

      book1d("RecoAllAssoc2GenSingleMatched_SharedTrackFractionRecoMatched_separated3", 50, 0, 1);
      book1d("RecoAllAssoc2GenSingleMatched_SharedTrackWFractionRecoMatched_separated3", 50, 0, 1);
      book1d("RecoAllAssoc2GenSingleMatched_SharedTrackPtFractionRecoMatched_separated3", 50, 0, 1);
      book1d("RecoAllAssoc2GenSingleMatched_SharedTrackWPtFractionRecoMatched_separated3", 50, 0, 1);
      book1d("RecoAllAssoc2GenSingleMatched_SharedTrackPt2FractionRecoMatched_separated3", 50, 0, 1);
      book1d("RecoAllAssoc2GenSingleMatched_SharedTrackWPt2FractionRecoMatched_separated3", 50, 0, 1);
      book1d("RecoAllAssoc2GenSingleMatched_SharedTrackHarmPtFractionRecoMatched_separated3", 50, 0, 1);
      book1d("RecoAllAssoc2GenSingleMatched_SharedTrackWHarmPtFractionRecoMatched_separated3", 50, 0, 1);
      book1d("RecoAllAssoc2GenSingleMatched_SharedTrackHarmWPtFractionRecoMatched_separated3", 50, 0, 1);
      book1d("RecoAllAssoc2GenSingleMatched_SharedTrackHarmPtAvgFractionRecoMatched_separated3", 50, 0, 1);
      book1d("RecoAllAssoc2GenSingleMatched_SharedTrackWHarmPtAvgFractionRecoMatched_separated3", 50, 0, 1);
      book1d("RecoAllAssoc2GenSingleMatched_SharedTrackHarmWPtAvgFractionRecoMatched_separated3", 50, 0, 1);

      book1d("RecoAllAssoc2GenMultiMatched_SharedTrackFractionRecoMatched_separated3", 50, 0, 1);
      book1d("RecoAllAssoc2GenMultiMatched_SharedTrackWFractionRecoMatched_separated3", 50, 0, 1);
      book1d("RecoAllAssoc2GenMultiMatched_SharedTrackPtFractionRecoMatched_separated3", 50, 0, 1);
      book1d("RecoAllAssoc2GenMultiMatched_SharedTrackWPtFractionRecoMatched_separated3", 50, 0, 1);
      book1d("RecoAllAssoc2GenMultiMatched_SharedTrackPt2FractionRecoMatched_separated3", 50, 0, 1);
      book1d("RecoAllAssoc2GenMultiMatched_SharedTrackWPt2FractionRecoMatched_separated3", 50, 0, 1);
      book1d("RecoAllAssoc2GenMultiMatched_SharedTrackHarmPtFractionRecoMatched_separated3", 50, 0, 1);
      book1d("RecoAllAssoc2GenMultiMatched_SharedTrackWHarmPtFractionRecoMatched_separated3", 50, 0, 1);
      book1d("RecoAllAssoc2GenMultiMatched_SharedTrackHarmWPtFractionRecoMatched_separated3", 50, 0, 1);
      book1d("RecoAllAssoc2GenMultiMatched_SharedTrackHarmPtAvgFractionRecoMatched_separated3", 50, 0, 1);
      book1d("RecoAllAssoc2GenMultiMatched_SharedTrackWHarmPtAvgFractionRecoMatched_separated3", 50, 0, 1);
      book1d("RecoAllAssoc2GenMultiMatched_SharedTrackHarmWPtAvgFractionRecoMatched_separated3", 50, 0, 1);

      book1d("RecoAllAssoc2GenSingleMatched_SharedTrackFractionSim_separated3", 50, 0, 1);
      book1d("RecoAllAssoc2GenSingleMatched_SharedTrackWFractionSim_separated3", 50, 0, 1);
      book1d("RecoAllAssoc2GenSingleMatched_SharedTrackPtFractionSim_separated3", 50, 0, 1);
      book1d("RecoAllAssoc2GenSingleMatched_SharedTrackWPtFractionSim_separated3", 50, 0, 1);
      book1d("RecoAllAssoc2GenSingleMatched_SharedTrackPt2FractionSim_separated3", 50, 0, 1);
      book1d("RecoAllAssoc2GenSingleMatched_SharedTrackWPt2FractionSim_separated3", 50, 0, 1);    
      book1d("RecoAllAssoc2GenMultiMatched_SharedTrackFractionSim_separated3", 50, 0, 1);
      book1d("RecoAllAssoc2GenMultiMatched_SharedTrackWFractionSim_separated3", 50, 0, 1);
      book1d("RecoAllAssoc2GenMultiMatched_SharedTrackPtFractionSim_separated3", 50, 0, 1);
      book1d("RecoAllAssoc2GenMultiMatched_SharedTrackWPtFractionSim_separated3", 50, 0, 1);
      book1d("RecoAllAssoc2GenMultiMatched_SharedTrackPt2FractionSim_separated3", 50, 0, 1);
      book1d("RecoAllAssoc2GenMultiMatched_SharedTrackWPt2FractionSim_separated3", 50, 0, 1);
      book1d("RecoAllAssoc2GenSingleMatched_SharedTrackFractionSimMatched_separated3", 50, 0, 1);
      book1d("RecoAllAssoc2GenSingleMatched_SharedTrackWFractionSimMatched_separated3", 50, 0, 1);
      book1d("RecoAllAssoc2GenSingleMatched_SharedTrackPtFractionSimMatched_separated3", 50, 0, 1);
      book1d("RecoAllAssoc2GenSingleMatched_SharedTrackWPtFractionSimMatched_separated3", 50, 0, 1);
      book1d("RecoAllAssoc2GenSingleMatched_SharedTrackPt2FractionSimMatched_separated3", 50, 0, 1);
      book1d("RecoAllAssoc2GenSingleMatched_SharedTrackWPt2FractionSimMatched_separated3", 50, 0, 1);    
      book1d("RecoAllAssoc2GenMultiMatched_SharedTrackFractionSimMatched_separated3", 50, 0, 1);
      book1d("RecoAllAssoc2GenMultiMatched_SharedTrackWFractionSimMatched_separated3", 50, 0, 1);
      book1d("RecoAllAssoc2GenMultiMatched_SharedTrackPtFractionSimMatched_separated3", 50, 0, 1);
      book1d("RecoAllAssoc2GenMultiMatched_SharedTrackWPtFractionSimMatched_separated3", 50, 0, 1);
      book1d("RecoAllAssoc2GenMultiMatched_SharedTrackPt2FractionSimMatched_separated3", 50, 0, 1);
      book1d("RecoAllAssoc2GenMultiMatched_SharedTrackWPt2FractionSimMatched_separated3", 50, 0, 1);
      


      mes_[label]["RecoAllAssoc2GenSingleMatched_SharedTrackWFractionReco"] =
        i.book1D("RecoAllAssoc2GenSingleMatched_SharedTrackWFractionReco",
                 "ReconstructedAllAssoc2GenSingleMatched_SharedTrackWFractionReco",
                 50, 0, 1);
      mes_[label]["RecoAllAssoc2GenSingleMatched_SharedTrackWFractionRecoMatched"] =
        i.book1D("RecoAllAssoc2GenSingleMatched_SharedTrackWFractionRecoMatched",
                 "ReconstructedAllAssoc2GenSingleMatched_SharedTrackWFractionRecoMatched",
                 50, 0, 1);    
      mes_[label]["RecoAllAssoc2GenSingleMatched_SharedTrackPtFractionReco"] =
        i.book1D("RecoAllAssoc2GenSingleMatched_SharedTrackPtFractionReco",
                 "ReconstructedAllAssoc2GenSingleMatched_SharedTrackPtFractionReco",
                 50, 0, 1);
      mes_[label]["RecoAllAssoc2GenSingleMatched_SharedTrackPtFractionRecoMatched"] =
        i.book1D("RecoAllAssoc2GenSingleMatched_SharedTrackPtFractionRecoMatched",
                 "ReconstructedAllAssoc2GenSingleMatched_SharedTrackPtFractionRecoMatched",
                 50, 0, 1);    
      mes_[label]["RecoAllAssoc2GenSingleMatched_SharedTrackWPtFractionReco"] =
        i.book1D("RecoAllAssoc2GenSingleMatched_SharedTrackWPtFractionReco",
                 "ReconstructedAllAssoc2GenSingleMatched_SharedTrackWPtFractionReco",
                 50, 0, 1);
      mes_[label]["RecoAllAssoc2GenSingleMatched_SharedTrackWPtFractionRecoMatched"] =
        i.book1D("RecoAllAssoc2GenSingleMatched_SharedTrackWPtFractionRecoMatched",
                 "ReconstructedAllAssoc2GenSingleMatched_SharedTrackWPtFractionRecoMatched",
                 50, 0, 1);    
      mes_[label]["RecoAllAssoc2GenSingleMatched_SharedTrackPt2FractionReco"] =
        i.book1D("RecoAllAssoc2GenSingleMatched_SharedTrackPt2FractionReco",
                 "ReconstructedAllAssoc2GenSingleMatched_SharedTrackPt2FractionReco",
                 50, 0, 1);
      mes_[label]["RecoAllAssoc2GenSingleMatched_SharedTrackPt2FractionRecoMatched"] =
        i.book1D("RecoAllAssoc2GenSingleMatched_SharedTrackPt2FractionRecoMatched",
                 "ReconstructedAllAssoc2GenSingleMatched_SharedTrackPt2FractionRecoMatched",
                 50, 0, 1);    
      mes_[label]["RecoAllAssoc2GenSingleMatched_SharedTrackWPt2FractionReco"] =
        i.book1D("RecoAllAssoc2GenSingleMatched_SharedTrackWPt2FractionReco",
                 "ReconstructedAllAssoc2GenSingleMatched_SharedTrackWPt2FractionReco",
                 50, 0, 1);
      mes_[label]["RecoAllAssoc2GenSingleMatched_SharedTrackWPt2FractionRecoMatched"] =
        i.book1D("RecoAllAssoc2GenSingleMatched_SharedTrackWPt2FractionRecoMatched",
                 "ReconstructedAllAssoc2GenSingleMatched_SharedTrackWPt2FractionRecoMatched",
                 50, 0, 1);    

      mes_[label]["RecoAllAssoc2GenSingleMatched_SharedTrackWFractionSim"] =
        i.book1D("RecoAllAssoc2GenSingleMatched_SharedTrackWFractionSim",
                 "ReconstructedAllAssoc2GenSingleMatched_SharedTrackWFractionSim",
                 50, 0, 1);
      mes_[label]["RecoAllAssoc2GenSingleMatched_SharedTrackWFractionSimMatched"] =
        i.book1D("RecoAllAssoc2GenSingleMatched_SharedTrackWFractionSimMatched",
                 "ReconstructedAllAssoc2GenSingleMatched_SharedTrackWFractionSimMatched",
                 50, 0, 1);    
      mes_[label]["RecoAllAssoc2GenSingleMatched_SharedTrackPtFractionSim"] =
        i.book1D("RecoAllAssoc2GenSingleMatched_SharedTrackPtFractionSim",
                 "ReconstructedAllAssoc2GenSingleMatched_SharedTrackPtFractionSim",
                 50, 0, 1);
      mes_[label]["RecoAllAssoc2GenSingleMatched_SharedTrackPtFractionSimMatched"] =
        i.book1D("RecoAllAssoc2GenSingleMatched_SharedTrackPtFractionSimMatched",
                 "ReconstructedAllAssoc2GenSingleMatched_SharedTrackPtFractionSimMatched",
                 50, 0, 1);    
      mes_[label]["RecoAllAssoc2GenSingleMatched_SharedTrackWPtFractionSim"] =
        i.book1D("RecoAllAssoc2GenSingleMatched_SharedTrackWPtFractionSim",
                 "ReconstructedAllAssoc2GenSingleMatched_SharedTrackWPtFractionSim",
                 50, 0, 1);
      mes_[label]["RecoAllAssoc2GenSingleMatched_SharedTrackWPtFractionSimMatched"] =
        i.book1D("RecoAllAssoc2GenSingleMatched_SharedTrackWPtFractionSimMatched",
                 "ReconstructedAllAssoc2GenSingleMatched_SharedTrackWPtFractionSimMatched",
                 50, 0, 1);    
      mes_[label]["RecoAllAssoc2GenSingleMatched_SharedTrackPt2FractionSim"] =
        i.book1D("RecoAllAssoc2GenSingleMatched_SharedTrackPt2FractionSim",
                 "ReconstructedAllAssoc2GenSingleMatched_SharedTrackPt2FractionSim",
                 50, 0, 1);
      mes_[label]["RecoAllAssoc2GenSingleMatched_SharedTrackPt2FractionSimMatched"] =
        i.book1D("RecoAllAssoc2GenSingleMatched_SharedTrackPt2FractionSimMatched",
                 "ReconstructedAllAssoc2GenSingleMatched_SharedTrackPt2FractionSimMatched",
                 50, 0, 1);    
      mes_[label]["RecoAllAssoc2GenSingleMatched_SharedTrackWPt2FractionSim"] =
        i.book1D("RecoAllAssoc2GenSingleMatched_SharedTrackWPt2FractionSim",
                 "ReconstructedAllAssoc2GenSingleMatched_SharedTrackWPt2FractionSim",
                 50, 0, 1);
      mes_[label]["RecoAllAssoc2GenSingleMatched_SharedTrackWPt2FractionSimMatched"] =
        i.book1D("RecoAllAssoc2GenSingleMatched_SharedTrackWPt2FractionSimMatched",
                 "ReconstructedAllAssoc2GenSingleMatched_SharedTrackWPt2FractionSimMatched",
                 50, 0, 1);


   
      mes_[label]["RecoAllAssoc2GenMultiMatched_SharedTrackWFractionReco"] =
        i.book1D("RecoAllAssoc2GenMultiMatched_SharedTrackWFractionReco",
                 "ReconstructedAllAssoc2GenMultiMatched_SharedTrackWFractionReco",
                 50, 0, 1);
      mes_[label]["RecoAllAssoc2GenMultiMatched_SharedTrackWFractionRecoMatched"] =
        i.book1D("RecoAllAssoc2GenMultiMatched_SharedTrackWFractionRecoMatched",
                 "ReconstructedAllAssoc2GenMultiMatched_SharedTrackWFractionRecoMatched",
                 50, 0, 1);    
      mes_[label]["RecoAllAssoc2GenMultiMatched_SharedTrackPtFractionReco"] =
        i.book1D("RecoAllAssoc2GenMultiMatched_SharedTrackPtFractionReco",
                 "ReconstructedAllAssoc2GenMultiMatched_SharedTrackPtFractionReco",
                 50, 0, 1);
      mes_[label]["RecoAllAssoc2GenMultiMatched_SharedTrackPtFractionRecoMatched"] =
        i.book1D("RecoAllAssoc2GenMultiMatched_SharedTrackPtFractionRecoMatched",
                 "ReconstructedAllAssoc2GenMultiMatched_SharedTrackPtFractionRecoMatched",
                 50, 0, 1);    
      mes_[label]["RecoAllAssoc2GenMultiMatched_SharedTrackWPtFractionReco"] =
        i.book1D("RecoAllAssoc2GenMultiMatched_SharedTrackWPtFractionReco",
                 "ReconstructedAllAssoc2GenMultiMatched_SharedTrackWPtFractionReco",
                 50, 0, 1);
      mes_[label]["RecoAllAssoc2GenMultiMatched_SharedTrackWPtFractionRecoMatched"] =
        i.book1D("RecoAllAssoc2GenMultiMatched_SharedTrackWPtFractionRecoMatched",
                 "ReconstructedAllAssoc2GenMultiMatched_SharedTrackWPtFractionRecoMatched",
                 50, 0, 1);    
      mes_[label]["RecoAllAssoc2GenMultiMatched_SharedTrackPt2FractionReco"] =
        i.book1D("RecoAllAssoc2GenMultiMatched_SharedTrackPt2FractionReco",
                 "ReconstructedAllAssoc2GenMultiMatched_SharedTrackPt2FractionReco",
                 50, 0, 1);
      mes_[label]["RecoAllAssoc2GenMultiMatched_SharedTrackPt2FractionRecoMatched"] =
        i.book1D("RecoAllAssoc2GenMultiMatched_SharedTrackPt2FractionRecoMatched",
                 "ReconstructedAllAssoc2GenMultiMatched_SharedTrackPt2FractionRecoMatched",
                 50, 0, 1);    
      mes_[label]["RecoAllAssoc2GenMultiMatched_SharedTrackWPt2FractionReco"] =
        i.book1D("RecoAllAssoc2GenMultiMatched_SharedTrackWPt2FractionReco",
                 "ReconstructedAllAssoc2GenMultiMatched_SharedTrackWPt2FractionReco",
                 50, 0, 1);
      mes_[label]["RecoAllAssoc2GenMultiMatched_SharedTrackWPt2FractionRecoMatched"] =
        i.book1D("RecoAllAssoc2GenMultiMatched_SharedTrackWPt2FractionRecoMatched",
                 "ReconstructedAllAssoc2GenMultiMatched_SharedTrackWPt2FractionRecoMatched",
                 50, 0, 1);    

    
      mes_[label]["RecoAllAssoc2GenMultiMatched_SharedTrackWFractionSim"] =
        i.book1D("RecoAllAssoc2GenMultiMatched_SharedTrackWFractionSim",
                 "ReconstructedAllAssoc2GenMultiMatched_SharedTrackWFractionSim",
                 50, 0, 1);
      mes_[label]["RecoAllAssoc2GenMultiMatched_SharedTrackWFractionSimMatched"] =
        i.book1D("RecoAllAssoc2GenMultiMatched_SharedTrackWFractionSimMatched",
                 "ReconstructedAllAssoc2GenMultiMatched_SharedTrackWFractionSimMatched",
                 50, 0, 1);    
      mes_[label]["RecoAllAssoc2GenMultiMatched_SharedTrackPtFractionSim"] =
        i.book1D("RecoAllAssoc2GenMultiMatched_SharedTrackPtFractionSim",
                 "ReconstructedAllAssoc2GenMultiMatched_SharedTrackPtFractionSim",
                 50, 0, 1);
      mes_[label]["RecoAllAssoc2GenMultiMatched_SharedTrackPtFractionSimMatched"] =
        i.book1D("RecoAllAssoc2GenMultiMatched_SharedTrackPtFractionSimMatched",
                 "ReconstructedAllAssoc2GenMultiMatched_SharedTrackPtFractionSimMatched",
                 50, 0, 1); 
      mes_[label]["RecoAllAssoc2GenMultiMatched_SharedTrackWPtFractionSim"] =
        i.book1D("RecoAllAssoc2GenMultiMatched_SharedTrackWPtFractionSim",
                 "ReconstructedAllAssoc2GenMultiMatched_SharedTrackWPtFractionSim",
                 50, 0, 1);
      mes_[label]["RecoAllAssoc2GenMultiMatched_SharedTrackWPtFractionSimMatched"] =
        i.book1D("RecoAllAssoc2GenMultiMatched_SharedTrackWPtFractionSimMatched",
                 "ReconstructedAllAssoc2GenMultiMatched_SharedTrackWPtFractionSimMatched",
                 50, 0, 1);    
      mes_[label]["RecoAllAssoc2GenMultiMatched_SharedTrackPt2FractionSim"] =
        i.book1D("RecoAllAssoc2GenMultiMatched_SharedTrackPt2FractionSim",
                 "ReconstructedAllAssoc2GenMultiMatched_SharedTrackPt2FractionSim",
                 50, 0, 1);
      mes_[label]["RecoAllAssoc2GenMultiMatched_SharedTrackPt2FractionSimMatched"] =
        i.book1D("RecoAllAssoc2GenMultiMatched_SharedTrackPt2FractionSimMatched",
                 "ReconstructedAllAssoc2GenMultiMatched_SharedTrackPt2FractionSimMatched",
                 50, 0, 1);  
      mes_[label]["RecoAllAssoc2GenMultiMatched_SharedTrackWPt2FractionSim"] =
        i.book1D("RecoAllAssoc2GenMultiMatched_SharedTrackWPt2FractionSim",
                 "ReconstructedAllAssoc2GenMultiMatched_SharedTrackWPt2FractionSim",
                 50, 0, 1);
      mes_[label]["RecoAllAssoc2GenMultiMatched_SharedTrackWPt2FractionSimMatched"] =
        i.book1D("RecoAllAssoc2GenMultiMatched_SharedTrackWPt2FractionSimMatched",
                 "ReconstructedAllAssoc2GenMultiMatched_SharedTrackWPt2FractionSimMatched",
                 50, 0, 1);  
    
      // Reco separated1
      mes_[label]["RecoAllAssoc2GenSingleMatched_SharedTrackFractionReco_separated1"] =
        i.book1D("RecoAllAssoc2GenSingleMatched_SharedTrackFractionReco_separated1",
                 "ReconstructedAllAssoc2GenSingleMatched_SharedTrackFractionReco_separated1",
                 50, 0, 1);
      mes_[label]["RecoAllAssoc2GenSingleMatched_SharedTrackFractionRecoMatched_separated1"] =
        i.book1D("RecoAllAssoc2GenSingleMatched_SharedTrackFractionRecoMatched_separated1",
                 "ReconstructedAllAssoc2GenSingleMatched_SharedTrackFractionRecoMatched_separated1",
                 50, 0, 1);    
      mes_[label]["RecoAllAssoc2GenSingleMatched_SharedTrackWFractionReco_separated1"] =
        i.book1D("RecoAllAssoc2GenSingleMatched_SharedTrackWFractionReco_separated1",
                 "ReconstructedAllAssoc2GenSingleMatched_SharedTrackWFractionReco_separated1",
                 50, 0, 1);
      mes_[label]["RecoAllAssoc2GenSingleMatched_SharedTrackWFractionRecoMatched_separated1"] =
        i.book1D("RecoAllAssoc2GenSingleMatched_SharedTrackWFractionRecoMatched_separated1",
                 "ReconstructedAllAssoc2GenSingleMatched_SharedTrackWFractionRecoMatched_separated1",
                 50, 0, 1);    
      mes_[label]["RecoAllAssoc2GenSingleMatched_SharedTrackPtFractionReco_separated1"] =
        i.book1D("RecoAllAssoc2GenSingleMatched_SharedTrackPtFractionReco_separated1",
                 "ReconstructedAllAssoc2GenSingleMatched_SharedTrackPtFractionReco_separated1",
                 50, 0, 1);
      mes_[label]["RecoAllAssoc2GenSingleMatched_SharedTrackPtFractionRecoMatched_separated1"] =
        i.book1D("RecoAllAssoc2GenSingleMatched_SharedTrackPtFractionRecoMatched_separated1",
                 "ReconstructedAllAssoc2GenSingleMatched_SharedTrackPtFractionRecoMatched_separated1",
                 50, 0, 1);    
      mes_[label]["RecoAllAssoc2GenSingleMatched_SharedTrackWPtFractionReco_separated1"] =
        i.book1D("RecoAllAssoc2GenSingleMatched_SharedTrackWPtFractionReco_separated1",
                 "ReconstructedAllAssoc2GenSingleMatched_SharedTrackWPtFractionReco_separated1",
                 50, 0, 1);
      mes_[label]["RecoAllAssoc2GenSingleMatched_SharedTrackWPtFractionRecoMatched_separated1"] =
        i.book1D("RecoAllAssoc2GenSingleMatched_SharedTrackWPtFractionRecoMatched_separated1",
                 "ReconstructedAllAssoc2GenSingleMatched_SharedTrackWPtFractionRecoMatched_separated1",
                 50, 0, 1);    
      mes_[label]["RecoAllAssoc2GenSingleMatched_SharedTrackPt2FractionReco_separated1"] =
        i.book1D("RecoAllAssoc2GenSingleMatched_SharedTrackPt2FractionReco_separated1",
                 "ReconstructedAllAssoc2GenSingleMatched_SharedTrackPt2FractionReco_separated1",
                 50, 0, 1);
      mes_[label]["RecoAllAssoc2GenSingleMatched_SharedTrackPt2FractionRecoMatched_separated1"] =
        i.book1D("RecoAllAssoc2GenSingleMatched_SharedTrackPt2FractionRecoMatched_separated1",
                 "ReconstructedAllAssoc2GenSingleMatched_SharedTrackPt2FractionRecoMatched_separated1",
                 50, 0, 1);    
      mes_[label]["RecoAllAssoc2GenSingleMatched_SharedTrackWPt2FractionReco_separated1"] =
        i.book1D("RecoAllAssoc2GenSingleMatched_SharedTrackWPt2FractionReco_separated1",
                 "ReconstructedAllAssoc2GenSingleMatched_SharedTrackWPt2FractionReco_separated1",
                 50, 0, 1);
      mes_[label]["RecoAllAssoc2GenSingleMatched_SharedTrackWPt2FractionRecoMatched_separated1"] =
        i.book1D("RecoAllAssoc2GenSingleMatched_SharedTrackWPt2FractionRecoMatched_separated1",
                 "ReconstructedAllAssoc2GenSingleMatched_SharedTrackWPt2FractionRecoMatched_separated1",
                 50, 0, 1);    
      mes_[label]["RecoAllAssoc2GenSingleMatched_SharedTrackFractionSim_separated1"] =
        i.book1D("RecoAllAssoc2GenSingleMatched_SharedTrackFractionSim_separated1",
                 "ReconstructedAllAssoc2GenSingleMatched_SharedTrackFractionSim_separated1",
                 50, 0, 1);
      mes_[label]["RecoAllAssoc2GenSingleMatched_SharedTrackFractionSimMatched_separated1"] =
        i.book1D("RecoAllAssoc2GenSingleMatched_SharedTrackFractionSimMatched_separated1",
                 "ReconstructedAllAssoc2GenSingleMatched_SharedTrackFractionSimMatched_separated1",
                 50, 0, 1);    
      mes_[label]["RecoAllAssoc2GenSingleMatched_SharedTrackWFractionSim_separated1"] =
        i.book1D("RecoAllAssoc2GenSingleMatched_SharedTrackWFractionSim_separated1",
                 "ReconstructedAllAssoc2GenSingleMatched_SharedTrackWFractionSim_separated1",
                 50, 0, 1);
      mes_[label]["RecoAllAssoc2GenSingleMatched_SharedTrackWFractionSimMatched_separated1"] =
        i.book1D("RecoAllAssoc2GenSingleMatched_SharedTrackWFractionSimMatched_separated1",
                 "ReconstructedAllAssoc2GenSingleMatched_SharedTrackWFractionSimMatched_separated1",
                 50, 0, 1);    
      mes_[label]["RecoAllAssoc2GenSingleMatched_SharedTrackPtFractionSim_separated1"] =
        i.book1D("RecoAllAssoc2GenSingleMatched_SharedTrackPtFractionSim_separated1",
                 "ReconstructedAllAssoc2GenSingleMatched_SharedTrackPtFractionSim_separated1",
                 50, 0, 1);
      mes_[label]["RecoAllAssoc2GenSingleMatched_SharedTrackPtFractionSimMatched_separated1"] =
        i.book1D("RecoAllAssoc2GenSingleMatched_SharedTrackPtFractionSimMatched_separated1",
                 "ReconstructedAllAssoc2GenSingleMatched_SharedTrackPtFractionSimMatched_separated1",
                 50, 0, 1);    
      mes_[label]["RecoAllAssoc2GenSingleMatched_SharedTrackWPtFractionSim_separated1"] =
        i.book1D("RecoAllAssoc2GenSingleMatched_SharedTrackWPtFractionSim_separated1",
                 "ReconstructedAllAssoc2GenSingleMatched_SharedTrackWPtFractionSim_separated1",
                 50, 0, 1);
      mes_[label]["RecoAllAssoc2GenSingleMatched_SharedTrackWPtFractionSimMatched_separated1"] =
        i.book1D("RecoAllAssoc2GenSingleMatched_SharedTrackWPtFractionSimMatched_separated1",
                 "ReconstructedAllAssoc2GenSingleMatched_SharedTrackWPtFractionSimMatched_separated1",
                 50, 0, 1);    
      mes_[label]["RecoAllAssoc2GenSingleMatched_SharedTrackPt2FractionSim_separated1"] =
        i.book1D("RecoAllAssoc2GenSingleMatched_SharedTrackPt2FractionSim_separated1",
                 "ReconstructedAllAssoc2GenSingleMatched_SharedTrackPt2FractionSim_separated1",
                 50, 0, 1);
      mes_[label]["RecoAllAssoc2GenSingleMatched_SharedTrackPt2FractionSimMatched_separated1"] =
        i.book1D("RecoAllAssoc2GenSingleMatched_SharedTrackPt2FractionSimMatched_separated1",
                 "ReconstructedAllAssoc2GenSingleMatched_SharedTrackPt2FractionSimMatched_separated1",
                 50, 0, 1); 
      mes_[label]["RecoAllAssoc2GenSingleMatched_SharedTrackWPt2FractionSim_separated1"] =
        i.book1D("RecoAllAssoc2GenSingleMatched_SharedTrackWPt2FractionSim_separated1",
                 "ReconstructedAllAssoc2GenSingleMatched_SharedTrackWPt2FractionSim_separated1",
                 50, 0, 1);
      mes_[label]["RecoAllAssoc2GenSingleMatched_SharedTrackWPt2FractionSimMatched_separated1"] =
        i.book1D("RecoAllAssoc2GenSingleMatched_SharedTrackWPt2FractionSimMatched_separated1",
                 "ReconstructedAllAssoc2GenSingleMatched_SharedTrackWPt2FractionSimMatched_separated1",
                 50, 0, 1);     


      mes_[label]["RecoAllAssoc2GenMultiMatched_SharedTrackFractionReco_separated1"] =
        i.book1D("RecoAllAssoc2GenMultiMatched_SharedTrackFractionReco_separated1",
                 "ReconstructedAllAssoc2GenMultiMatched_SharedTrackFractionReco_separated1",
                 50, 0, 1);
      mes_[label]["RecoAllAssoc2GenMultiMatched_SharedTrackFractionRecoMatched_separated1"] =
        i.book1D("RecoAllAssoc2GenMultiMatched_SharedTrackFractionRecoMatched_separated1",
                 "ReconstructedAllAssoc2GenMultiMatched_SharedTrackFractionRecoMatched_separated1",
                 50, 0, 1);    
      mes_[label]["RecoAllAssoc2GenMultiMatched_SharedTrackWFractionReco_separated1"] =
        i.book1D("RecoAllAssoc2GenMultiMatched_SharedTrackWFractionReco_separated1",
                 "ReconstructedAllAssoc2GenMultiMatched_SharedTrackWFractionReco_separated1",
                 50, 0, 1);
      mes_[label]["RecoAllAssoc2GenMultiMatched_SharedTrackWFractionRecoMatched_separated1"] =
        i.book1D("RecoAllAssoc2GenMultiMatched_SharedTrackWFractionRecoMatched_separated1",
                 "ReconstructedAllAssoc2GenMultiMatched_SharedTrackWFractionRecoMatched_separated1",
                 50, 0, 1);    
      mes_[label]["RecoAllAssoc2GenMultiMatched_SharedTrackPtFractionReco_separated1"] =
        i.book1D("RecoAllAssoc2GenMultiMatched_SharedTrackPtFractionReco_separated1",
                 "ReconstructedAllAssoc2GenMultiMatched_SharedTrackPtFractionReco_separated1",
                 50, 0, 1);
      mes_[label]["RecoAllAssoc2GenMultiMatched_SharedTrackPtFractionRecoMatched_separated1"] =
        i.book1D("RecoAllAssoc2GenMultiMatched_SharedTrackPtFractionRecoMatched_separated1",
                 "ReconstructedAllAssoc2GenMultiMatched_SharedTrackPtFractionRecoMatched_separated1",
                 50, 0, 1);    
      mes_[label]["RecoAllAssoc2GenMultiMatched_SharedTrackWPtFractionReco_separated1"] =
        i.book1D("RecoAllAssoc2GenMultiMatched_SharedTrackWPtFractionReco_separated1",
                 "ReconstructedAllAssoc2GenMultiMatched_SharedTrackWPtFractionReco_separated1",
                 50, 0, 1);
      mes_[label]["RecoAllAssoc2GenMultiMatched_SharedTrackWPtFractionRecoMatched_separated1"] =
        i.book1D("RecoAllAssoc2GenMultiMatched_SharedTrackWPtFractionRecoMatched_separated1",
                 "ReconstructedAllAssoc2GenMultiMatched_SharedTrackWPtFractionRecoMatched_separated1",
                 50, 0, 1);    
      mes_[label]["RecoAllAssoc2GenMultiMatched_SharedTrackPt2FractionReco_separated1"] =
        i.book1D("RecoAllAssoc2GenMultiMatched_SharedTrackPt2FractionReco_separated1",
                 "ReconstructedAllAssoc2GenMultiMatched_SharedTrackPt2FractionReco_separated1",
                 50, 0, 1);
      mes_[label]["RecoAllAssoc2GenMultiMatched_SharedTrackPt2FractionRecoMatched_separated1"] =
        i.book1D("RecoAllAssoc2GenMultiMatched_SharedTrackPt2FractionRecoMatched_separated1",
                 "ReconstructedAllAssoc2GenMultiMatched_SharedTrackPt2FractionRecoMatched_separated1",
                 50, 0, 1);    
      mes_[label]["RecoAllAssoc2GenMultiMatched_SharedTrackWPt2FractionReco_separated1"] =
        i.book1D("RecoAllAssoc2GenMultiMatched_SharedTrackWPt2FractionReco_separated1",
                 "ReconstructedAllAssoc2GenMultiMatched_SharedTrackWPt2FractionReco_separated1",
                 50, 0, 1);
      mes_[label]["RecoAllAssoc2GenMultiMatched_SharedTrackWPt2FractionRecoMatched_separated1"] =
        i.book1D("RecoAllAssoc2GenMultiMatched_SharedTrackWPt2FractionRecoMatched_separated1",
                 "ReconstructedAllAssoc2GenMultiMatched_SharedTrackWPt2FractionRecoMatched_separated1",
                 50, 0, 1);    
      mes_[label]["RecoAllAssoc2GenMultiMatched_SharedTrackFractionSim_separated1"] =
        i.book1D("RecoAllAssoc2GenMultiMatched_SharedTrackFractionSim_separated1",
                 "ReconstructedAllAssoc2GenMultiMatched_SharedTrackFractionSim_separated1",
                 50, 0, 1);
      mes_[label]["RecoAllAssoc2GenMultiMatched_SharedTrackFractionSimMatched_separated1"] =
        i.book1D("RecoAllAssoc2GenMultiMatched_SharedTrackFractionSimMatched_separated1",
                 "ReconstructedAllAssoc2GenMultiMatched_SharedTrackFractionSimMatched_separated1",
                 50, 0, 1);  
      mes_[label]["RecoAllAssoc2GenMultiMatched_SharedTrackWFractionSim_separated1"] =
        i.book1D("RecoAllAssoc2GenMultiMatched_SharedTrackWFractionSim_separated1",
                 "ReconstructedAllAssoc2GenMultiMatched_SharedTrackWFractionSim_separated1",
                 50, 0, 1);
      mes_[label]["RecoAllAssoc2GenMultiMatched_SharedTrackWFractionSimMatched_separated1"] =
        i.book1D("RecoAllAssoc2GenMultiMatched_SharedTrackWFractionSimMatched_separated1",
                 "ReconstructedAllAssoc2GenMultiMatched_SharedTrackWFractionSimMatched_separated1",
                 50, 0, 1);    
      mes_[label]["RecoAllAssoc2GenMultiMatched_SharedTrackPtFractionSim_separated1"] =
        i.book1D("RecoAllAssoc2GenMultiMatched_SharedTrackPtFractionSim_separated1",
                 "ReconstructedAllAssoc2GenMultiMatched_SharedTrackPtFractionSim_separated1",
                 50, 0, 1);
      mes_[label]["RecoAllAssoc2GenMultiMatched_SharedTrackPtFractionSimMatched_separated1"] =
        i.book1D("RecoAllAssoc2GenMultiMatched_SharedTrackPtFractionSimMatched_separated1",
                 "ReconstructedAllAssoc2GenMultiMatched_SharedTrackPtFractionSimMatched_separated1",
                 50, 0, 1);  
      mes_[label]["RecoAllAssoc2GenMultiMatched_SharedTrackWPtFractionSim_separated1"] =
        i.book1D("RecoAllAssoc2GenMultiMatched_SharedTrackWPtFractionSim_separated1",
                 "ReconstructedAllAssoc2GenMultiMatched_SharedTrackWPtFractionSim_separated1",
                 50, 0, 1);
      mes_[label]["RecoAllAssoc2GenMultiMatched_SharedTrackWPtFractionSimMatched_separated1"] =
        i.book1D("RecoAllAssoc2GenMultiMatched_SharedTrackWPtFractionSimMatched_separated1",
                 "ReconstructedAllAssoc2GenMultiMatched_SharedTrackWPtFractionSimMatched_separated1",
                 50, 0, 1);    
      mes_[label]["RecoAllAssoc2GenMultiMatched_SharedTrackPt2FractionSim_separated1"] =
        i.book1D("RecoAllAssoc2GenMultiMatched_SharedTrackPt2FractionSim_separated1",
                 "ReconstructedAllAssoc2GenMultiMatched_SharedTrackPt2FractionSim_separated1",
                 50, 0, 1);
      mes_[label]["RecoAllAssoc2GenMultiMatched_SharedTrackPt2FractionSimMatched_separated1"] =
        i.book1D("RecoAllAssoc2GenMultiMatched_SharedTrackPt2FractionSimMatched_separated1",
                 "ReconstructedAllAssoc2GenMultiMatched_SharedTrackPt2FractionSimMatched_separated1",
                 50, 0, 1);
      mes_[label]["RecoAllAssoc2GenMultiMatched_SharedTrackWPt2FractionSim_separated1"] =
        i.book1D("RecoAllAssoc2GenMultiMatched_SharedTrackWPt2FractionSim_separated1",
                 "ReconstructedAllAssoc2GenMultiMatched_SharedTrackWPt2FractionSim_separated1",
                 50, 0, 1);
      mes_[label]["RecoAllAssoc2GenMultiMatched_SharedTrackWPt2FractionSimMatched_separated1"] =
        i.book1D("RecoAllAssoc2GenMultiMatched_SharedTrackWPt2FractionSimMatched_separated1",
                 "ReconstructedAllAssoc2GenMultiMatched_SharedTrackWPt2FractionSimMatched_separated1",
                 50, 0, 1);  

      // Reco separated2
      mes_[label]["RecoAllAssoc2GenSingleMatched_SharedTrackFractionReco_separated2"] =
        i.book1D("RecoAllAssoc2GenSingleMatched_SharedTrackFractionReco_separated2",
                 "ReconstructedAllAssoc2GenSingleMatched_SharedTrackFractionReco_separated2",
                 50, 0, 1);
      mes_[label]["RecoAllAssoc2GenSingleMatched_SharedTrackFractionRecoMatched_separated2"] =
        i.book1D("RecoAllAssoc2GenSingleMatched_SharedTrackFractionRecoMatched_separated2",
                 "ReconstructedAllAssoc2GenSingleMatched_SharedTrackFractionRecoMatched_separated2",
                 50, 0, 1);    
      mes_[label]["RecoAllAssoc2GenSingleMatched_SharedTrackWFractionReco_separated2"] =
        i.book1D("RecoAllAssoc2GenSingleMatched_SharedTrackWFractionReco_separated2",
                 "ReconstructedAllAssoc2GenSingleMatched_SharedTrackWFractionReco_separated2",
                 50, 0, 1);
      mes_[label]["RecoAllAssoc2GenSingleMatched_SharedTrackWFractionRecoMatched_separated2"] =
        i.book1D("RecoAllAssoc2GenSingleMatched_SharedTrackWFractionRecoMatched_separated2",
                 "ReconstructedAllAssoc2GenSingleMatched_SharedTrackWFractionRecoMatched_separated2",
                 50, 0, 1);    
      mes_[label]["RecoAllAssoc2GenSingleMatched_SharedTrackPtFractionReco_separated2"] =
        i.book1D("RecoAllAssoc2GenSingleMatched_SharedTrackPtFractionReco_separated2",
                 "ReconstructedAllAssoc2GenSingleMatched_SharedTrackPtFractionReco_separated2",
                 50, 0, 1);
      mes_[label]["RecoAllAssoc2GenSingleMatched_SharedTrackPtFractionRecoMatched_separated2"] =
        i.book1D("RecoAllAssoc2GenSingleMatched_SharedTrackPtFractionRecoMatched_separated2",
                 "ReconstructedAllAssoc2GenSingleMatched_SharedTrackPtFractionRecoMatched_separated2",
                 50, 0, 1);    
      mes_[label]["RecoAllAssoc2GenSingleMatched_SharedTrackWPtFractionReco_separated2"] =
        i.book1D("RecoAllAssoc2GenSingleMatched_SharedTrackWPtFractionReco_separated2",
                 "ReconstructedAllAssoc2GenSingleMatched_SharedTrackWPtFractionReco_separated2",
                 50, 0, 1);
      mes_[label]["RecoAllAssoc2GenSingleMatched_SharedTrackWPtFractionRecoMatched_separated2"] =
        i.book1D("RecoAllAssoc2GenSingleMatched_SharedTrackWPtFractionRecoMatched_separated2",
                 "ReconstructedAllAssoc2GenSingleMatched_SharedTrackWPtFractionRecoMatched_separated2",
                 50, 0, 1);    
      mes_[label]["RecoAllAssoc2GenSingleMatched_SharedTrackPt2FractionReco_separated2"] =
        i.book1D("RecoAllAssoc2GenSingleMatched_SharedTrackPt2FractionReco_separated2",
                 "ReconstructedAllAssoc2GenSingleMatched_SharedTrackPt2FractionReco_separated2",
                 50, 0, 1);
      mes_[label]["RecoAllAssoc2GenSingleMatched_SharedTrackPt2FractionRecoMatched_separated2"] =
        i.book1D("RecoAllAssoc2GenSingleMatched_SharedTrackPt2FractionRecoMatched_separated2",
                 "ReconstructedAllAssoc2GenSingleMatched_SharedTrackPt2FractionRecoMatched_separated2",
                 50, 0, 1);    
      mes_[label]["RecoAllAssoc2GenSingleMatched_SharedTrackWPt2FractionReco_separated2"] =
        i.book1D("RecoAllAssoc2GenSingleMatched_SharedTrackWPt2FractionReco_separated2",
                 "ReconstructedAllAssoc2GenSingleMatched_SharedTrackWPt2FractionReco_separated2",
                 50, 0, 1);
      mes_[label]["RecoAllAssoc2GenSingleMatched_SharedTrackWPt2FractionRecoMatched_separated2"] =
        i.book1D("RecoAllAssoc2GenSingleMatched_SharedTrackWPt2FractionRecoMatched_separated2",
                 "ReconstructedAllAssoc2GenSingleMatched_SharedTrackWPt2FractionRecoMatched_separated2",
                 50, 0, 1);    
      mes_[label]["RecoAllAssoc2GenSingleMatched_SharedTrackFractionSim_separated2"] =
        i.book1D("RecoAllAssoc2GenSingleMatched_SharedTrackFractionSim_separated2",
                 "ReconstructedAllAssoc2GenSingleMatched_SharedTrackFractionSim_separated2",
                 50, 0, 1);
      mes_[label]["RecoAllAssoc2GenSingleMatched_SharedTrackFractionSimMatched_separated2"] =
        i.book1D("RecoAllAssoc2GenSingleMatched_SharedTrackFractionSimMatched_separated2",
                 "ReconstructedAllAssoc2GenSingleMatched_SharedTrackFractionSimMatched_separated2",
                 50, 0, 1);    
      mes_[label]["RecoAllAssoc2GenSingleMatched_SharedTrackWFractionSim_separated2"] =
        i.book1D("RecoAllAssoc2GenSingleMatched_SharedTrackWFractionSim_separated2",
                 "ReconstructedAllAssoc2GenSingleMatched_SharedTrackWFractionSim_separated2",
                 50, 0, 1);
      mes_[label]["RecoAllAssoc2GenSingleMatched_SharedTrackWFractionSimMatched_separated2"] =
        i.book1D("RecoAllAssoc2GenSingleMatched_SharedTrackWFractionSimMatched_separated2",
                 "ReconstructedAllAssoc2GenSingleMatched_SharedTrackWFractionSimMatched_separated2",
                 50, 0, 1);    
      mes_[label]["RecoAllAssoc2GenSingleMatched_SharedTrackPtFractionSim_separated2"] =
        i.book1D("RecoAllAssoc2GenSingleMatched_SharedTrackPtFractionSim_separated2",
                 "ReconstructedAllAssoc2GenSingleMatched_SharedTrackPtFractionSim_separated2",
                 50, 0, 1);
      mes_[label]["RecoAllAssoc2GenSingleMatched_SharedTrackPtFractionSimMatched_separated2"] =
        i.book1D("RecoAllAssoc2GenSingleMatched_SharedTrackPtFractionSimMatched_separated2",
                 "ReconstructedAllAssoc2GenSingleMatched_SharedTrackPtFractionSimMatched_separated2",
                 50, 0, 1);   
      mes_[label]["RecoAllAssoc2GenSingleMatched_SharedTrackWPtFractionSim_separated2"] =
        i.book1D("RecoAllAssoc2GenSingleMatched_SharedTrackWPtFractionSim_separated2",
                 "ReconstructedAllAssoc2GenSingleMatched_SharedTrackWPtFractionSim_separated2",
                 50, 0, 1);
      mes_[label]["RecoAllAssoc2GenSingleMatched_SharedTrackWPtFractionSimMatched_separated2"] =
        i.book1D("RecoAllAssoc2GenSingleMatched_SharedTrackWPtFractionSimMatched_separated2",
                 "ReconstructedAllAssoc2GenSingleMatched_SharedTrackWPtFractionSimMatched_separated2",
                 50, 0, 1);    
      mes_[label]["RecoAllAssoc2GenSingleMatched_SharedTrackPt2FractionSim_separated2"] =
        i.book1D("RecoAllAssoc2GenSingleMatched_SharedTrackPt2FractionSim_separated2",
                 "ReconstructedAllAssoc2GenSingleMatched_SharedTrackPt2FractionSim_separated2",
                 50, 0, 1);
      mes_[label]["RecoAllAssoc2GenSingleMatched_SharedTrackPt2FractionSimMatched_separated2"] =
        i.book1D("RecoAllAssoc2GenSingleMatched_SharedTrackPt2FractionSimMatched_separated2",
                 "ReconstructedAllAssoc2GenSingleMatched_SharedTrackPt2FractionSimMatched_separated2",
                 50, 0, 1);    
      mes_[label]["RecoAllAssoc2GenSingleMatched_SharedTrackWPt2FractionSim_separated2"] =
        i.book1D("RecoAllAssoc2GenSingleMatched_SharedTrackWPt2FractionSim_separated2",
                 "ReconstructedAllAssoc2GenSingleMatched_SharedTrackWPt2FractionSim_separated2",
                 50, 0, 1);
      mes_[label]["RecoAllAssoc2GenSingleMatched_SharedTrackWPt2FractionSimMatched_separated2"] =
        i.book1D("RecoAllAssoc2GenSingleMatched_SharedTrackWPt2FractionSimMatched_separated2",
                 "ReconstructedAllAssoc2GenSingleMatched_SharedTrackWPt2FractionSimMatched_separated2",
                 50, 0, 1);    



      mes_[label]["RecoAllAssoc2GenMultiMatched_SharedTrackFractionReco_separated2"] =
        i.book1D("RecoAllAssoc2GenMultiMatched_SharedTrackFractionReco_separated2",
                 "ReconstructedAllAssoc2GenMultiMatched_SharedTrackFractionReco_separated2",
                 50, 0, 1);
      mes_[label]["RecoAllAssoc2GenMultiMatched_SharedTrackFractionRecoMatched_separated2"] =
        i.book1D("RecoAllAssoc2GenMultiMatched_SharedTrackFractionRecoMatched_separated2",
                 "ReconstructedAllAssoc2GenMultiMatched_SharedTrackFractionRecoMatched_separated2",
                 50, 0, 1);    
      mes_[label]["RecoAllAssoc2GenMultiMatched_SharedTrackWFractionReco_separated2"] =
        i.book1D("RecoAllAssoc2GenMultiMatched_SharedTrackWFractionReco_separated2",
                 "ReconstructedAllAssoc2GenMultiMatched_SharedTrackWFractionReco_separated2",
                 50, 0, 1);
      mes_[label]["RecoAllAssoc2GenMultiMatched_SharedTrackWFractionRecoMatched_separated2"] =
        i.book1D("RecoAllAssoc2GenMultiMatched_SharedTrackWFractionRecoMatched_separated2",
                 "ReconstructedAllAssoc2GenMultiMatched_SharedTrackWFractionRecoMatched_separated2",
                 50, 0, 1);    
      mes_[label]["RecoAllAssoc2GenMultiMatched_SharedTrackPtFractionReco_separated2"] =
        i.book1D("RecoAllAssoc2GenMultiMatched_SharedTrackPtFractionReco_separated2",
                 "ReconstructedAllAssoc2GenMultiMatched_SharedTrackPtFractionReco_separated2",
                 50, 0, 1);
      mes_[label]["RecoAllAssoc2GenMultiMatched_SharedTrackPtFractionRecoMatched_separated2"] =
        i.book1D("RecoAllAssoc2GenMultiMatched_SharedTrackPtFractionRecoMatched_separated2",
                 "ReconstructedAllAssoc2GenMultiMatched_SharedTrackPtFractionRecoMatched_separated2",
                 50, 0, 1);    
      mes_[label]["RecoAllAssoc2GenMultiMatched_SharedTrackWPtFractionReco_separated2"] =
        i.book1D("RecoAllAssoc2GenMultiMatched_SharedTrackWPtFractionReco_separated2",
                 "ReconstructedAllAssoc2GenMultiMatched_SharedTrackWPtFractionReco_separated2",
                 50, 0, 1);
      mes_[label]["RecoAllAssoc2GenMultiMatched_SharedTrackWPtFractionRecoMatched_separated2"] =
        i.book1D("RecoAllAssoc2GenMultiMatched_SharedTrackWPtFractionRecoMatched_separated2",
                 "ReconstructedAllAssoc2GenMultiMatched_SharedTrackWPtFractionRecoMatched_separated2",
                 50, 0, 1);    
      mes_[label]["RecoAllAssoc2GenMultiMatched_SharedTrackPt2FractionReco_separated2"] =
        i.book1D("RecoAllAssoc2GenMultiMatched_SharedTrackPt2FractionReco_separated2",
                 "ReconstructedAllAssoc2GenMultiMatched_SharedTrackPt2FractionReco_separated2",
                 50, 0, 1);
      mes_[label]["RecoAllAssoc2GenMultiMatched_SharedTrackPt2FractionRecoMatched_separated2"] =
        i.book1D("RecoAllAssoc2GenMultiMatched_SharedTrackPt2FractionRecoMatched_separated2",
                 "ReconstructedAllAssoc2GenMultiMatched_SharedTrackPt2FractionRecoMatched_separated2",
                 50, 0, 1);    
      mes_[label]["RecoAllAssoc2GenMultiMatched_SharedTrackWPt2FractionReco_separated2"] =
        i.book1D("RecoAllAssoc2GenMultiMatched_SharedTrackWPt2FractionReco_separated2",
                 "ReconstructedAllAssoc2GenMultiMatched_SharedTrackWPt2FractionReco_separated2",
                 50, 0, 1);
      mes_[label]["RecoAllAssoc2GenMultiMatched_SharedTrackWPt2FractionRecoMatched_separated2"] =
        i.book1D("RecoAllAssoc2GenMultiMatched_SharedTrackWPt2FractionRecoMatched_separated2",
                 "ReconstructedAllAssoc2GenMultiMatched_SharedTrackWPt2FractionRecoMatched_separated2",
                 50, 0, 1);    
      mes_[label]["RecoAllAssoc2GenMultiMatched_SharedTrackFractionSim_separated2"] =
        i.book1D("RecoAllAssoc2GenMultiMatched_SharedTrackFractionSim_separated2",
                 "ReconstructedAllAssoc2GenMultiMatched_SharedTrackFractionSim_separated2",
                 50, 0, 1);
      mes_[label]["RecoAllAssoc2GenMultiMatched_SharedTrackFractionSimMatched_separated2"] =
        i.book1D("RecoAllAssoc2GenMultiMatched_SharedTrackFractionSimMatched_separated2",
                 "ReconstructedAllAssoc2GenMultiMatched_SharedTrackFractionSimMatched_separated2",
                 50, 0, 1);    
      mes_[label]["RecoAllAssoc2GenMultiMatched_SharedTrackWFractionSim_separated2"] =
        i.book1D("RecoAllAssoc2GenMultiMatched_SharedTrackWFractionSim_separated2",
                 "ReconstructedAllAssoc2GenMultiMatched_SharedTrackWFractionSim_separated2",
                 50, 0, 1);
      mes_[label]["RecoAllAssoc2GenMultiMatched_SharedTrackWFractionSimMatched_separated2"] =
        i.book1D("RecoAllAssoc2GenMultiMatched_SharedTrackWFractionSimMatched_separated2",
                 "ReconstructedAllAssoc2GenMultiMatched_SharedTrackWFractionSimMatched_separated2",
                 50, 0, 1);    
      mes_[label]["RecoAllAssoc2GenMultiMatched_SharedTrackPtFractionSim_separated2"] =
        i.book1D("RecoAllAssoc2GenMultiMatched_SharedTrackPtFractionSim_separated2",
                 "ReconstructedAllAssoc2GenMultiMatched_SharedTrackPtFractionSim_separated2",
                 50, 0, 1);
      mes_[label]["RecoAllAssoc2GenMultiMatched_SharedTrackPtFractionSimMatched_separated2"] =
        i.book1D("RecoAllAssoc2GenMultiMatched_SharedTrackPtFractionSimMatched_separated2",
                 "ReconstructedAllAssoc2GenMultiMatched_SharedTrackPtFractionSimMatched_separated2",
                 50, 0, 1);  
      mes_[label]["RecoAllAssoc2GenMultiMatched_SharedTrackWPtFractionSim_separated2"] =
        i.book1D("RecoAllAssoc2GenMultiMatched_SharedTrackWPtFractionSim_separated2",
                 "ReconstructedAllAssoc2GenMultiMatched_SharedTrackWPtFractionSim_separated2",
                 50, 0, 1);
      mes_[label]["RecoAllAssoc2GenMultiMatched_SharedTrackWPtFractionSimMatched_separated2"] =
        i.book1D("RecoAllAssoc2GenMultiMatched_SharedTrackWPtFractionSimMatched_separated2",
                 "ReconstructedAllAssoc2GenMultiMatched_SharedTrackWPtFractionSimMatched_separated2",
                 50, 0, 1);    
      mes_[label]["RecoAllAssoc2GenMultiMatched_SharedTrackPt2FractionSim_separated2"] =
        i.book1D("RecoAllAssoc2GenMultiMatched_SharedTrackPt2FractionSim_separated2",
                 "ReconstructedAllAssoc2GenMultiMatched_SharedTrackPt2FractionSim_separated2",
                 50, 0, 1);
      mes_[label]["RecoAllAssoc2GenMultiMatched_SharedTrackPt2FractionSimMatched_separated2"] =
        i.book1D("RecoAllAssoc2GenMultiMatched_SharedTrackPt2FractionSimMatched_separated2",
                 "ReconstructedAllAssoc2GenMultiMatched_SharedTrackPt2FractionSimMatched_separated2",
                 50, 0, 1);  
      mes_[label]["RecoAllAssoc2GenMultiMatched_SharedTrackWPt2FractionSim_separated2"] =
        i.book1D("RecoAllAssoc2GenMultiMatched_SharedTrackWPt2FractionSim_separated2",
                 "ReconstructedAllAssoc2GenMultiMatched_SharedTrackWPt2FractionSim_separated2",
                 50, 0, 1);
      mes_[label]["RecoAllAssoc2GenMultiMatched_SharedTrackWPt2FractionSimMatched_separated2"] =
        i.book1D("RecoAllAssoc2GenMultiMatched_SharedTrackWPt2FractionSimMatched_separated2",
                 "ReconstructedAllAssoc2GenMultiMatched_SharedTrackWPt2FractionSimMatched_separated2",
                 50, 0, 1);  

      // Reco separated3
      mes_[label]["RecoAllAssoc2GenSingleMatched_SharedTrackFractionReco_separated3"] =
        i.book1D("RecoAllAssoc2GenSingleMatched_SharedTrackFractionReco_separated3",
                 "ReconstructedAllAssoc2GenSingleMatched_SharedTrackFractionReco_separated3",
                 50, 0, 1);
      mes_[label]["RecoAllAssoc2GenSingleMatched_SharedTrackFractionRecoMatched_separated3"] =
        i.book1D("RecoAllAssoc2GenSingleMatched_SharedTrackFractionRecoMatched_separated3",
                 "ReconstructedAllAssoc2GenSingleMatched_SharedTrackFractionRecoMatched_separated3",
                 50, 0, 1);    
      mes_[label]["RecoAllAssoc2GenSingleMatched_SharedTrackWFractionReco_separated3"] =
        i.book1D("RecoAllAssoc2GenSingleMatched_SharedTrackWFractionReco_separated3",
                 "ReconstructedAllAssoc2GenSingleMatched_SharedTrackWFractionReco_separated3",
                 50, 0, 1);
      mes_[label]["RecoAllAssoc2GenSingleMatched_SharedTrackWFractionRecoMatched_separated3"] =
        i.book1D("RecoAllAssoc2GenSingleMatched_SharedTrackWFractionRecoMatched_separated3",
                 "ReconstructedAllAssoc2GenSingleMatched_SharedTrackWFractionRecoMatched_separated3",
                 50, 0, 1);    
      mes_[label]["RecoAllAssoc2GenSingleMatched_SharedTrackPtFractionReco_separated3"] =
        i.book1D("RecoAllAssoc2GenSingleMatched_SharedTrackPtFractionReco_separated3",
                 "ReconstructedAllAssoc2GenSingleMatched_SharedTrackPtFractionReco_separated3",
                 50, 0, 1);
      mes_[label]["RecoAllAssoc2GenSingleMatched_SharedTrackPtFractionRecoMatched_separated3"] =
        i.book1D("RecoAllAssoc2GenSingleMatched_SharedTrackPtFractionRecoMatched_separated3",
                 "ReconstructedAllAssoc2GenSingleMatched_SharedTrackPtFractionRecoMatched_separated3",
                 50, 0, 1);    
      mes_[label]["RecoAllAssoc2GenSingleMatched_SharedTrackWPtFractionReco_separated3"] =
        i.book1D("RecoAllAssoc2GenSingleMatched_SharedTrackWPtFractionReco_separated3",
                 "ReconstructedAllAssoc2GenSingleMatched_SharedTrackWPtFractionReco_separated3",
                 50, 0, 1);
      mes_[label]["RecoAllAssoc2GenSingleMatched_SharedTrackWPtFractionRecoMatched_separated3"] =
        i.book1D("RecoAllAssoc2GenSingleMatched_SharedTrackWPtFractionRecoMatched_separated3",
                 "ReconstructedAllAssoc2GenSingleMatched_SharedTrackWPtFractionRecoMatched_separated3",
                 50, 0, 1);    
      mes_[label]["RecoAllAssoc2GenSingleMatched_SharedTrackPt2FractionReco_separated3"] =
        i.book1D("RecoAllAssoc2GenSingleMatched_SharedTrackPt2FractionReco_separated3",
                 "ReconstructedAllAssoc2GenSingleMatched_SharedTrackPt2FractionReco_separated3",
                 50, 0, 1);
      mes_[label]["RecoAllAssoc2GenSingleMatched_SharedTrackPt2FractionRecoMatched_separated3"] =
        i.book1D("RecoAllAssoc2GenSingleMatched_SharedTrackPt2FractionRecoMatched_separated3",
                 "ReconstructedAllAssoc2GenSingleMatched_SharedTrackPt2FractionRecoMatched_separated3",
                 50, 0, 1);    
      mes_[label]["RecoAllAssoc2GenSingleMatched_SharedTrackWPt2FractionReco_separated3"] =
        i.book1D("RecoAllAssoc2GenSingleMatched_SharedTrackWPt2FractionReco_separated3",
                 "ReconstructedAllAssoc2GenSingleMatched_SharedTrackWPt2FractionReco_separated3",
                 50, 0, 1);
      mes_[label]["RecoAllAssoc2GenSingleMatched_SharedTrackWPt2FractionRecoMatched_separated3"] =
        i.book1D("RecoAllAssoc2GenSingleMatched_SharedTrackWPt2FractionRecoMatched_separated3",
                 "ReconstructedAllAssoc2GenSingleMatched_SharedTrackWPt2FractionRecoMatched_separated3",
                 50, 0, 1);    
      mes_[label]["RecoAllAssoc2GenSingleMatched_SharedTrackFractionSim_separated3"] =
        i.book1D("RecoAllAssoc2GenSingleMatched_SharedTrackFractionSim_separated3",
                 "ReconstructedAllAssoc2GenSingleMatched_SharedTrackFractionSim_separated3",
                 50, 0, 1);
      mes_[label]["RecoAllAssoc2GenSingleMatched_SharedTrackFractionSimMatched_separated3"] =
        i.book1D("RecoAllAssoc2GenSingleMatched_SharedTrackFractionSimMatched_separated3",
                 "ReconstructedAllAssoc2GenSingleMatched_SharedTrackFractionSimMatched_separated3",
                 50, 0, 1);    
      mes_[label]["RecoAllAssoc2GenSingleMatched_SharedTrackWFractionSim_separated3"] =
        i.book1D("RecoAllAssoc2GenSingleMatched_SharedTrackWFractionSim_separated3",
                 "ReconstructedAllAssoc2GenSingleMatched_SharedTrackWFractionSim_separated3",
                 50, 0, 1);
      mes_[label]["RecoAllAssoc2GenSingleMatched_SharedTrackWFractionSimMatched_separated3"] =
        i.book1D("RecoAllAssoc2GenSingleMatched_SharedTrackWFractionSimMatched_separated3",
                 "ReconstructedAllAssoc2GenSingleMatched_SharedTrackWFractionSimMatched_separated3",
                 50, 0, 1);    
      mes_[label]["RecoAllAssoc2GenSingleMatched_SharedTrackPtFractionSim_separated3"] =
        i.book1D("RecoAllAssoc2GenSingleMatched_SharedTrackPtFractionSim_separated3",
                 "ReconstructedAllAssoc2GenSingleMatched_SharedTrackPtFractionSim_separated3",
                 50, 0, 1);
      mes_[label]["RecoAllAssoc2GenSingleMatched_SharedTrackPtFractionSimMatched_separated3"] =
        i.book1D("RecoAllAssoc2GenSingleMatched_SharedTrackPtFractionSimMatched_separated3",
                 "ReconstructedAllAssoc2GenSingleMatched_SharedTrackPtFractionSimMatched_separated3",
                 50, 0, 1);    
      mes_[label]["RecoAllAssoc2GenSingleMatched_SharedTrackWPtFractionSim_separated3"] =
        i.book1D("RecoAllAssoc2GenSingleMatched_SharedTrackWPtFractionSim_separated3",
                 "ReconstructedAllAssoc2GenSingleMatched_SharedTrackWPtFractionSim_separated3",
                 50, 0, 1);
      mes_[label]["RecoAllAssoc2GenSingleMatched_SharedTrackWPtFractionSimMatched_separated3"] =
        i.book1D("RecoAllAssoc2GenSingleMatched_SharedTrackWPtFractionSimMatched_separated3",
                 "ReconstructedAllAssoc2GenSingleMatched_SharedTrackWPtFractionSimMatched_separated3",
                 50, 0, 1);    
      mes_[label]["RecoAllAssoc2GenSingleMatched_SharedTrackPt2FractionSim_separated3"] =
        i.book1D("RecoAllAssoc2GenSingleMatched_SharedTrackPt2FractionSim_separated3",
                 "ReconstructedAllAssoc2GenSingleMatched_SharedTrackPt2FractionSim_separated3",
                 50, 0, 1);
      mes_[label]["RecoAllAssoc2GenSingleMatched_SharedTrackPt2FractionSimMatched_separated3"] =
        i.book1D("RecoAllAssoc2GenSingleMatched_SharedTrackPt2FractionSimMatched_separated3",
                 "ReconstructedAllAssoc2GenSingleMatched_SharedTrackPt2FractionSimMatched_separated3",
                 50, 0, 1);   
      mes_[label]["RecoAllAssoc2GenSingleMatched_SharedTrackWPt2FractionSim_separated3"] =
        i.book1D("RecoAllAssoc2GenSingleMatched_SharedTrackWPt2FractionSim_separated3",
                 "ReconstructedAllAssoc2GenSingleMatched_SharedTrackWPt2FractionSim_separated3",
                 50, 0, 1);
      mes_[label]["RecoAllAssoc2GenSingleMatched_SharedTrackWPt2FractionSimMatched_separated3"] =
        i.book1D("RecoAllAssoc2GenSingleMatched_SharedTrackWPt2FractionSimMatched_separated3",
                 "ReconstructedAllAssoc2GenSingleMatched_SharedTrackWPt2FractionSimMatched_separated3",
                 50, 0, 1);    


      mes_[label]["RecoAllAssoc2GenMultiMatched_SharedTrackFractionReco_separated3"] =
        i.book1D("RecoAllAssoc2GenMultiMatched_SharedTrackFractionReco_separated3",
                 "ReconstructedAllAssoc2GenMultiMatched_SharedTrackFractionReco_separated3",
                 50, 0, 1);
      mes_[label]["RecoAllAssoc2GenMultiMatched_SharedTrackFractionRecoMatched_separated3"] =
        i.book1D("RecoAllAssoc2GenMultiMatched_SharedTrackFractionRecoMatched_separated3",
                 "ReconstructedAllAssoc2GenMultiMatched_SharedTrackFractionRecoMatched_separated3",
                 50, 0, 1);    
      mes_[label]["RecoAllAssoc2GenMultiMatched_SharedTrackWFractionReco_separated3"] =
        i.book1D("RecoAllAssoc2GenMultiMatched_SharedTrackWFractionReco_separated3",
                 "ReconstructedAllAssoc2GenMultiMatched_SharedTrackWFractionReco_separated3",
                 50, 0, 1);
      mes_[label]["RecoAllAssoc2GenMultiMatched_SharedTrackWFractionRecoMatched_separated3"] =
        i.book1D("RecoAllAssoc2GenMultiMatched_SharedTrackWFractionRecoMatched_separated3",
                 "ReconstructedAllAssoc2GenMultiMatched_SharedTrackWFractionRecoMatched_separated3",
                 50, 0, 1);    
      mes_[label]["RecoAllAssoc2GenMultiMatched_SharedTrackPtFractionReco_separated3"] =
        i.book1D("RecoAllAssoc2GenMultiMatched_SharedTrackPtFractionReco_separated3",
                 "ReconstructedAllAssoc2GenMultiMatched_SharedTrackPtFractionReco_separated3",
                 50, 0, 1);
      mes_[label]["RecoAllAssoc2GenMultiMatched_SharedTrackPtFractionRecoMatched_separated3"] =
        i.book1D("RecoAllAssoc2GenMultiMatched_SharedTrackPtFractionRecoMatched_separated3",
                 "ReconstructedAllAssoc2GenMultiMatched_SharedTrackPtFractionRecoMatched_separated3",
                 50, 0, 1);    
      mes_[label]["RecoAllAssoc2GenMultiMatched_SharedTrackWPtFractionReco_separated3"] =
        i.book1D("RecoAllAssoc2GenMultiMatched_SharedTrackWPtFractionReco_separated3",
                 "ReconstructedAllAssoc2GenMultiMatched_SharedTrackWPtFractionReco_separated3",
                 50, 0, 1);
      mes_[label]["RecoAllAssoc2GenMultiMatched_SharedTrackWPtFractionRecoMatched_separated3"] =
        i.book1D("RecoAllAssoc2GenMultiMatched_SharedTrackWPtFractionRecoMatched_separated3",
                 "ReconstructedAllAssoc2GenMultiMatched_SharedTrackWPtFractionRecoMatched_separated3",
                 50, 0, 1);    
      mes_[label]["RecoAllAssoc2GenMultiMatched_SharedTrackPt2FractionReco_separated3"] =
        i.book1D("RecoAllAssoc2GenMultiMatched_SharedTrackPt2FractionReco_separated3",
                 "ReconstructedAllAssoc2GenMultiMatched_SharedTrackPt2FractionReco_separated3",
                 50, 0, 1);
      mes_[label]["RecoAllAssoc2GenMultiMatched_SharedTrackPt2FractionRecoMatched_separated3"] =
        i.book1D("RecoAllAssoc2GenMultiMatched_SharedTrackPt2FractionRecoMatched_separated3",
                 "ReconstructedAllAssoc2GenMultiMatched_SharedTrackPt2FractionRecoMatched_separated3",
                 50, 0, 1);    
      mes_[label]["RecoAllAssoc2GenMultiMatched_SharedTrackWPt2FractionReco_separated3"] =
        i.book1D("RecoAllAssoc2GenMultiMatched_SharedTrackWPt2FractionReco_separated3",
                 "ReconstructedAllAssoc2GenMultiMatched_SharedTrackWPt2FractionReco_separated3",
                 50, 0, 1);
      mes_[label]["RecoAllAssoc2GenMultiMatched_SharedTrackWPt2FractionRecoMatched_separated3"] =
        i.book1D("RecoAllAssoc2GenMultiMatched_SharedTrackWPt2FractionRecoMatched_separated3",
                 "ReconstructedAllAssoc2GenMultiMatched_SharedTrackWPt2FractionRecoMatched_separated3",
                 50, 0, 1);    
      mes_[label]["RecoAllAssoc2GenMultiMatched_SharedTrackFractionSim_separated3"] =
        i.book1D("RecoAllAssoc2GenMultiMatched_SharedTrackFractionSim_separated3",
                 "ReconstructedAllAssoc2GenMultiMatched_SharedTrackFractionSim_separated3",
                 50, 0, 1);
      mes_[label]["RecoAllAssoc2GenMultiMatched_SharedTrackFractionSimMatched_separated3"] =
        i.book1D("RecoAllAssoc2GenMultiMatched_SharedTrackFractionSimMatched_separated3",
                 "ReconstructedAllAssoc2GenMultiMatched_SharedTrackFractionSimMatched_separated3",
                 50, 0, 1);
      mes_[label]["RecoAllAssoc2GenMultiMatched_SharedTrackWFractionSim_separated3"] =
        i.book1D("RecoAllAssoc2GenMultiMatched_SharedTrackWFractionSim_separated3",
                 "ReconstructedAllAssoc2GenMultiMatched_SharedTrackWFractionSim_separated3",
                 50, 0, 1);
      mes_[label]["RecoAllAssoc2GenMultiMatched_SharedTrackWFractionSimMatched_separated3"] =
        i.book1D("RecoAllAssoc2GenMultiMatched_SharedTrackWFractionSimMatched_separated3",
                 "ReconstructedAllAssoc2GenMultiMatched_SharedTrackWFractionSimMatched_separated3",
                 50, 0, 1);    
      mes_[label]["RecoAllAssoc2GenMultiMatched_SharedTrackPtFractionSim_separated3"] =
        i.book1D("RecoAllAssoc2GenMultiMatched_SharedTrackPtFractionSim_separated3",
                 "ReconstructedAllAssoc2GenMultiMatched_SharedTrackPtFractionSim_separated3",
                 50, 0, 1);
      mes_[label]["RecoAllAssoc2GenMultiMatched_SharedTrackPtFractionSimMatched_separated3"] =
        i.book1D("RecoAllAssoc2GenMultiMatched_SharedTrackPtFractionSimMatched_separated3",
                 "ReconstructedAllAssoc2GenMultiMatched_SharedTrackPtFractionSimMatched_separated3",
                 50, 0, 1);   
      mes_[label]["RecoAllAssoc2GenMultiMatched_SharedTrackWPtFractionSim_separated3"] =
        i.book1D("RecoAllAssoc2GenMultiMatched_SharedTrackWPtFractionSim_separated3",
                 "ReconstructedAllAssoc2GenMultiMatched_SharedTrackWPtFractionSim_separated3",
                 50, 0, 1);
      mes_[label]["RecoAllAssoc2GenMultiMatched_SharedTrackWPtFractionSimMatched_separated3"] =
        i.book1D("RecoAllAssoc2GenMultiMatched_SharedTrackWPtFractionSimMatched_separated3",
                 "ReconstructedAllAssoc2GenMultiMatched_SharedTrackWPtFractionSimMatched_separated3",
                 50, 0, 1);    
      mes_[label]["RecoAllAssoc2GenMultiMatched_SharedTrackPt2FractionSim_separated3"] =
        i.book1D("RecoAllAssoc2GenMultiMatched_SharedTrackPt2FractionSim_separated3",
                 "ReconstructedAllAssoc2GenMultiMatched_SharedTrackPt2FractionSim_separated3",
                 50, 0, 1);
      mes_[label]["RecoAllAssoc2GenMultiMatched_SharedTrackPt2FractionSimMatched_separated3"] =
        i.book1D("RecoAllAssoc2GenMultiMatched_SharedTrackPt2FractionSimMatched_separated3",
                 "ReconstructedAllAssoc2GenMultiMatched_SharedTrackPt2FractionSimMatched_separated3",
                 50, 0, 1); 
      mes_[label]["RecoAllAssoc2GenMultiMatched_SharedTrackWPt2FractionSim_separated3"] =
        i.book1D("RecoAllAssoc2GenMultiMatched_SharedTrackWPt2FractionSim_separated3",
                 "ReconstructedAllAssoc2GenMultiMatched_SharedTrackWPt2FractionSim_separated3",
                 50, 0, 1);
      mes_[label]["RecoAllAssoc2GenMultiMatched_SharedTrackWPt2FractionSimMatched_separated3"] =
        i.book1D("RecoAllAssoc2GenMultiMatched_SharedTrackWPt2FractionSimMatched_separated3",
                 "ReconstructedAllAssoc2GenMultiMatched_SharedTrackWPt2FractionSimMatched_separated3",
                 50, 0, 1);  

    }
  }
}

void PrimaryVertexAnalyzer4PUSlimmed::fillGenericGenVertexHistograms(
    const simPrimaryVertex& v) {
  if (v.eventId.event() == 0) {
    mes_["root_folder"]["GenPV_X"]->Fill(v.x);
    mes_["root_folder"]["GenPV_Y"]->Fill(v.y);
    mes_["root_folder"]["GenPV_Z"]->Fill(v.z);
    mes_["root_folder"]["GenPV_R"]->Fill(v.r);
    mes_["root_folder"]["GenPV_Pt2"]->Fill(v.ptsq);
    mes_["root_folder"]["GenPV_NumTracks"]->Fill(v.nGenTrk);
    if (v.closest_vertex_distance_z > 0.)
      mes_["root_folder"]["GenPV_ClosestDistanceZ"]
          ->Fill(v.closest_vertex_distance_z);
  }
  mes_["root_folder"]["GenAllV_X"]->Fill(v.x);
  mes_["root_folder"]["GenAllV_Y"]->Fill(v.y);
  mes_["root_folder"]["GenAllV_Z"]->Fill(v.z);
  mes_["root_folder"]["GenAllV_R"]->Fill(v.r);
  mes_["root_folder"]["GenAllV_Pt2"]->Fill(v.ptsq);
  mes_["root_folder"]["GenAllV_NumTracks"]->Fill(v.nGenTrk);
  if (v.closest_vertex_distance_z > 0.)
    mes_["root_folder"]["GenAllV_ClosestDistanceZ"]
        ->Fill(v.closest_vertex_distance_z);
}

void PrimaryVertexAnalyzer4PUSlimmed::fillRecoAssociatedGenVertexHistograms(
    const std::string& label,
    const PrimaryVertexAnalyzer4PUSlimmed::simPrimaryVertex& v) {
  mes_[label]["GenAllAssoc2Reco_X"]->Fill(v.x);
  mes_[label]["GenAllAssoc2Reco_Y"]->Fill(v.y);
  mes_[label]["GenAllAssoc2Reco_Z"]->Fill(v.z);
  mes_[label]["GenAllAssoc2Reco_R"]->Fill(v.r);
  mes_[label]["GenAllAssoc2Reco_Pt2"]->Fill(v.ptsq);
  mes_[label]["GenAllAssoc2Reco_NumTracks"]->Fill(v.nGenTrk);
  if (v.closest_vertex_distance_z > 0.)
    mes_[label]["GenAllAssoc2Reco_ClosestDistanceZ"]
        ->Fill(v.closest_vertex_distance_z);
  if (v.rec_vertices.size()) {
    mes_[label]["GenAllAssoc2RecoMatched_X"]->Fill(v.x);
    mes_[label]["GenAllAssoc2RecoMatched_Y"]->Fill(v.y);
    mes_[label]["GenAllAssoc2RecoMatched_Z"]->Fill(v.z);
    mes_[label]["GenAllAssoc2RecoMatched_R"]->Fill(v.r);
    mes_[label]["GenAllAssoc2RecoMatched_Pt2"]->Fill(v.ptsq);
    mes_[label]["GenAllAssoc2RecoMatched_NumTracks"]->Fill(v.nGenTrk);
    if (v.closest_vertex_distance_z > 0.)
      mes_[label]["GenAllAssoc2RecoMatched_ClosestDistanceZ"]
          ->Fill(v.closest_vertex_distance_z);
  }
  if (v.rec_vertices.size() > 1) {
    mes_[label]["GenAllAssoc2RecoMultiMatched_X"]->Fill(v.x);
    mes_[label]["GenAllAssoc2RecoMultiMatched_Y"]->Fill(v.y);
    mes_[label]["GenAllAssoc2RecoMultiMatched_Z"]->Fill(v.z);
    mes_[label]["GenAllAssoc2RecoMultiMatched_R"]->Fill(v.r);
    mes_[label]["GenAllAssoc2RecoMultiMatched_Pt2"]->Fill(v.ptsq);
    mes_[label]["GenAllAssoc2RecoMultiMatched_NumTracks"]->Fill(v.nGenTrk);
    if (v.closest_vertex_distance_z > 0.)
      mes_[label]["GenAllAssoc2RecoMultiMatched_ClosestDistanceZ"]
          ->Fill(v.closest_vertex_distance_z);
  }

  std::string prefix;
  std::string ptPrefix;
  if(v.rec_vertices.size() == 1) {
    prefix = "GenAllAssoc2RecoSingleMatched_SharedTrackFraction";
    ptPrefix = "GenAllAssoc2RecoSingleMatched_SharedTrack";
  }
  else if(v.rec_vertices.size() > 1) {
    prefix = "GenAllAssoc2RecoMultiMatched_SharedTrackFraction";
    ptPrefix = "GenAllAssoc2RecoMultiMatched_SharedTrack";
  }


  //simVertex-recoVertex fraction values

  std::vector<double> recoFractionValues;
  std::vector<double> wFractionRecoFractionValues;
  std::vector<double> ptFractionRecoFractionValues;
  std::vector<double> wPtFractionRecoFractionValues;
  std::vector<double> pt2FractionRecoFractionValues;
  std::vector<double> wPt2FractionRecoFractionValues;


  std::vector<double> recoMatchedFractionValues;
  std::vector<double> wFractionRecoMatchedFractionValues;
  std::vector<double> ptFractionRecoMatchedFractionValues;
  std::vector<double> wPtFractionRecoMatchedFractionValues;
  std::vector<double> pt2FractionRecoMatchedFractionValues;
  std::vector<double> wPt2FractionRecoMatchedFractionValues;


  std::vector<double> simFractionValues;
  std::vector<double> ptFractionSimFractionValues;
  std::vector<double> pt2FractionSimFractionValues;
  std::vector<double> simMatchedFractionValues;
  std::vector<double> ptFractionSimMatchedFractionValues;
  std::vector<double> pt2FractionSimMatchedFractionValues;


  for(size_t i=0; i<v.rec_vertices.size(); ++i) {
    std::vector<double> numerators = calculateVertexSharedTracksMomentumFractionNumerators(*(v.sim_vertex.get()), *(v.rec_vertices[i]), *s2r_, *r2s_);
    std::vector<std::vector<double>> denominatorGroups = calculateVertexSharedTracksMomentumFractionDenominators(*(v.sim_vertex.get()), *(v.rec_vertices[i]), *s2r_, *r2s_);
    
    double fractionReco = numerators[0]/denominatorGroups[0][0];
    
    double wFractionReco;
    double ptFractionReco;
    double wPtFractionReco;
    double pt2FractionReco;
    double wPt2FractionReco;
    double harmPtFractionReco;
    double wHarmPtFractionReco;
    double harmWPtFractionReco;
    double harmPtAvgFractionReco;
    double wHarmPtAvgFractionReco;
    double harmWPtAvgFractionReco;

    double wFractionRecoMatched;
    double ptFractionRecoMatched;
    double wPtFractionRecoMatched;
    double pt2FractionRecoMatched;
    double wPt2FractionRecoMatched;
    double harmPtFractionRecoMatched;
    double wHarmPtFractionRecoMatched;
    double harmWPtFractionRecoMatched;
    double harmPtAvgFractionRecoMatched;
    double wHarmPtAvgFractionRecoMatched;
    double harmWPtAvgFractionRecoMatched;

    double wFractionSim;
    double ptFractionSim;
    double wPtFractionSim;
    double pt2FractionSim;
    double wPt2FractionSim;
    double ptFractionSimMatched;
    double wPtFractionSimMatched;
    double pt2FractionSimMatched;
    double wPt2FractionSimMatched;

    // avoid unused variable error with this trick
    (void)wFractionReco;
    (void)ptFractionReco;
    (void)wPtFractionReco;
    (void)pt2FractionReco;
    (void)wPt2FractionReco;
    (void)harmPtFractionReco;
    (void)wHarmPtFractionReco;
    (void)harmWPtFractionReco;
    (void)harmPtAvgFractionReco;
    (void)wHarmPtAvgFractionReco;
    (void)harmWPtAvgFractionReco;

    (void)wFractionRecoMatched;
    (void)ptFractionRecoMatched;
    (void)wPtFractionRecoMatched;
    (void)pt2FractionRecoMatched;
    (void)wPt2FractionRecoMatched;
    (void)harmPtFractionRecoMatched;
    (void)wHarmPtFractionRecoMatched;
    (void)harmWPtFractionRecoMatched;
    (void)harmPtAvgFractionRecoMatched;
    (void)wHarmPtAvgFractionRecoMatched;
    (void)harmWPtAvgFractionRecoMatched;

    (void)wFractionSim;
    (void)ptFractionSim;
    (void)wPtFractionSim;
    (void)pt2FractionSim;
    (void)wPt2FractionSim;
    (void)ptFractionSimMatched;
    (void)wPtFractionSimMatched;
    (void)pt2FractionSimMatched;
    (void)wPt2FractionSimMatched;
    
    if(doDetailedHistograms_)
    {
      wFractionReco = numerators[1]/denominatorGroups[0][1];
      ptFractionReco = numerators[2]/denominatorGroups[0][2];
      wPtFractionReco = numerators[3]/denominatorGroups[0][3];
      pt2FractionReco = numerators[4]/denominatorGroups[0][4];
      wPt2FractionReco = numerators[5]/denominatorGroups[0][5];
    }
    
    double fractionRecoMatched = numerators[0]/denominatorGroups[1][0];
    
    if(doDetailedHistograms_)
    {
      wFractionRecoMatched = numerators[1]/denominatorGroups[1][1];
      ptFractionRecoMatched = numerators[2]/denominatorGroups[1][2];
      wPtFractionRecoMatched = numerators[3]/denominatorGroups[1][3];
      pt2FractionRecoMatched = numerators[4]/denominatorGroups[1][4];
      wPt2FractionRecoMatched = numerators[5]/denominatorGroups[1][5];
    }
    
    double fractionSim = numerators[0]/denominatorGroups[2][0];
    
    if(doDetailedHistograms_)
    {
      ptFractionSim = numerators[1]/denominatorGroups[2][1];
      pt2FractionSim = numerators[2]/denominatorGroups[2][2];
    }
  
    double fractionSimMatched = numerators[0]/denominatorGroups[3][0];
  
    if(doDetailedHistograms_)
    {
      ptFractionSimMatched = numerators[1]/denominatorGroups[3][1];
      pt2FractionSimMatched = numerators[2]/denominatorGroups[3][2];
    }
    
    mes_[label][prefix+"Reco"]->Fill(fractionReco);
    
    if(doDetailedHistograms_)
    {
      mes_[label][ptPrefix+"WFractionReco"]->Fill(wFractionReco);
      mes_[label][ptPrefix+"PtFractionReco"]->Fill(ptFractionReco);
      mes_[label][ptPrefix+"WPtFractionReco"]->Fill(wPtFractionReco);
      mes_[label][ptPrefix+"Pt2FractionReco"]->Fill(pt2FractionReco);
      mes_[label][ptPrefix+"WPt2FractionReco"]->Fill(wPt2FractionReco);
    }
    
    mes_[label][prefix+"RecoMatched"]->Fill(fractionRecoMatched);
    
    if(doDetailedHistograms_)
    {
      mes_[label][ptPrefix+"WFractionRecoMatched"]->Fill(wFractionRecoMatched);
      mes_[label][ptPrefix+"PtFractionRecoMatched"]->Fill(ptFractionRecoMatched);
      mes_[label][ptPrefix+"WPtFractionRecoMatched"]->Fill(wPtFractionRecoMatched);
      mes_[label][ptPrefix+"Pt2FractionRecoMatched"]->Fill(pt2FractionRecoMatched);
      mes_[label][ptPrefix+"WPt2FractionRecoMatched"]->Fill(wPt2FractionRecoMatched);
    }
    
    mes_[label][prefix+"Sim"]->Fill(fractionSim);
    
    if(doDetailedHistograms_)
    {
      mes_[label][ptPrefix+"PtFractionSim"]->Fill(ptFractionSim);
      mes_[label][ptPrefix+"Pt2FractionSim"]->Fill(pt2FractionSim);
    }
    
    mes_[label][prefix+"SimMatched"]->Fill(fractionSimMatched);
    
    if(doDetailedHistograms_)
    {
      mes_[label][ptPrefix+"PtFractionSimMatched"]->Fill(ptFractionSimMatched);
      mes_[label][ptPrefix+"Pt2FractionSimMatched"]->Fill(pt2FractionSimMatched);
    }

    if(doDetailedHistograms_)
    {
      recoFractionValues.push_back(fractionReco);
      wFractionRecoFractionValues.push_back(wFractionReco);
      ptFractionRecoFractionValues.push_back(ptFractionReco);
      wPtFractionRecoFractionValues.push_back(wPtFractionReco);
      pt2FractionRecoFractionValues.push_back(pt2FractionReco);
      wPt2FractionRecoFractionValues.push_back(wPt2FractionReco);

      recoMatchedFractionValues.push_back(fractionRecoMatched);
      wFractionRecoMatchedFractionValues.push_back(wFractionRecoMatched);
      ptFractionRecoMatchedFractionValues.push_back(ptFractionRecoMatched);
      wPtFractionRecoMatchedFractionValues.push_back(wPtFractionRecoMatched);
      pt2FractionRecoMatchedFractionValues.push_back(pt2FractionRecoMatched);
      wPt2FractionRecoMatchedFractionValues.push_back(wPt2FractionRecoMatched);

      simFractionValues.push_back(fractionSim);
      ptFractionSimFractionValues.push_back(ptFractionSim);
      pt2FractionSimFractionValues.push_back(pt2FractionSim);
      simMatchedFractionValues.push_back(fractionSimMatched);
      ptFractionSimMatchedFractionValues.push_back(ptFractionSimMatched);
      pt2FractionSimMatchedFractionValues.push_back(pt2FractionSimMatched);
    }
    
  }


  // sort the fraction values (at most 3 for each simVertex, because a simVertex is connected
  // to at most three recoVertices), then Fill three histograms with the fraction values by order of
  // magnitude
  
  if(doDetailedHistograms_)
  {
    std::sort(recoFractionValues.begin(), recoFractionValues.end());
    std::sort(wFractionRecoFractionValues.begin(), wFractionRecoFractionValues.end());
    std::sort(ptFractionRecoFractionValues.begin(), ptFractionRecoFractionValues.end());
    std::sort(wPtFractionRecoFractionValues.begin(), wPtFractionRecoFractionValues.end());
    std::sort(pt2FractionRecoFractionValues.begin(), pt2FractionRecoFractionValues.end());
    std::sort(wPt2FractionRecoFractionValues.begin(), wPt2FractionRecoFractionValues.end());

    std::sort(recoMatchedFractionValues.begin(), recoMatchedFractionValues.end());
    std::sort(wFractionRecoMatchedFractionValues.begin(), wFractionRecoMatchedFractionValues.end());
    std::sort(ptFractionRecoMatchedFractionValues.begin(), ptFractionRecoMatchedFractionValues.end());
    std::sort(wPtFractionRecoMatchedFractionValues.begin(), wPtFractionRecoMatchedFractionValues.end());
    std::sort(pt2FractionRecoMatchedFractionValues.begin(), pt2FractionRecoMatchedFractionValues.end());
    std::sort(wPt2FractionRecoMatchedFractionValues.begin(), wPt2FractionRecoMatchedFractionValues.end());

    std::sort(simFractionValues.begin(), simFractionValues.end());
    std::sort(ptFractionSimFractionValues.begin(), ptFractionSimFractionValues.end());
    std::sort(pt2FractionSimFractionValues.begin(), pt2FractionSimFractionValues.end());
    std::sort(simMatchedFractionValues.begin(), simMatchedFractionValues.end());
    std::sort(ptFractionSimMatchedFractionValues.begin(), ptFractionSimMatchedFractionValues.end());
    std::sort(pt2FractionSimMatchedFractionValues.begin(), pt2FractionSimMatchedFractionValues.end());


    // reverse order, so that the biggest fraction is first

    std::reverse(recoFractionValues.begin(), recoFractionValues.end());
    std::reverse(wFractionRecoFractionValues.begin(), wFractionRecoFractionValues.end());
    std::reverse(ptFractionRecoFractionValues.begin(), ptFractionRecoFractionValues.end());
    std::reverse(wPtFractionRecoFractionValues.begin(), wPtFractionRecoFractionValues.end());
    std::reverse(pt2FractionRecoFractionValues.begin(), pt2FractionRecoFractionValues.end());
    std::reverse(wPt2FractionRecoFractionValues.begin(), wPt2FractionRecoFractionValues.end());

    std::reverse(recoMatchedFractionValues.begin(), recoMatchedFractionValues.end());
    std::reverse(wFractionRecoMatchedFractionValues.begin(), wFractionRecoMatchedFractionValues.end());
    std::reverse(ptFractionRecoMatchedFractionValues.begin(), ptFractionRecoMatchedFractionValues.end());
    std::reverse(wPtFractionRecoMatchedFractionValues.begin(), wPtFractionRecoMatchedFractionValues.end());
    std::reverse(pt2FractionRecoMatchedFractionValues.begin(), pt2FractionRecoMatchedFractionValues.end());
    std::reverse(wPt2FractionRecoMatchedFractionValues.begin(), wPt2FractionRecoMatchedFractionValues.end());

    std::reverse(simFractionValues.begin(), simFractionValues.end());
    std::reverse(ptFractionSimFractionValues.begin(), ptFractionSimFractionValues.end());
    std::reverse(pt2FractionSimFractionValues.begin(), pt2FractionSimFractionValues.end());
    std::reverse(simMatchedFractionValues.begin(), simMatchedFractionValues.end());
    std::reverse(ptFractionSimMatchedFractionValues.begin(), ptFractionSimMatchedFractionValues.end());
    std::reverse(pt2FractionSimMatchedFractionValues.begin(), pt2FractionSimMatchedFractionValues.end());


    // Filling values 1

    if(recoFractionValues.size() >= 1 )
      mes_[label][prefix+"Reco_separated1"]->Fill(recoFractionValues.at(0));
    if(wFractionRecoFractionValues.size() >= 1 )
      mes_[label][ptPrefix+"WFractionReco_separated1"]->Fill(wFractionRecoFractionValues.at(0));
    if(ptFractionRecoFractionValues.size() >= 1 )
      mes_[label][ptPrefix+"PtFractionReco_separated1"]->Fill(ptFractionRecoFractionValues.at(0));
    if(wPtFractionRecoFractionValues.size() >= 1 )
      mes_[label][ptPrefix+"WPtFractionReco_separated1"]->Fill(wPtFractionRecoFractionValues.at(0));
    if(pt2FractionRecoFractionValues.size() >= 1 )
      mes_[label][ptPrefix+"Pt2FractionReco_separated1"]->Fill(pt2FractionRecoFractionValues.at(0));
    if(wPt2FractionRecoFractionValues.size() >= 1 )
      mes_[label][ptPrefix+"WPt2FractionReco_separated1"]->Fill(wPt2FractionRecoFractionValues.at(0));


    if(recoMatchedFractionValues.size() >= 1 )
      mes_[label][prefix+"RecoMatched_separated1"]->Fill(recoMatchedFractionValues.at(0));
    if(wFractionRecoMatchedFractionValues.size() >= 1 )
      mes_[label][ptPrefix+"WFractionRecoMatched_separated1"]->Fill(wFractionRecoMatchedFractionValues.at(0));
    if(ptFractionRecoMatchedFractionValues.size() >= 1 )
      mes_[label][ptPrefix+"PtFractionRecoMatched_separated1"]->Fill(ptFractionRecoMatchedFractionValues.at(0));
    if(wPtFractionRecoMatchedFractionValues.size() >= 1 )
      mes_[label][ptPrefix+"WPtFractionRecoMatched_separated1"]->Fill(wPtFractionRecoMatchedFractionValues.at(0));
    if(pt2FractionRecoMatchedFractionValues.size() >= 1 )
      mes_[label][ptPrefix+"Pt2FractionRecoMatched_separated1"]->Fill(pt2FractionRecoMatchedFractionValues.at(0));
    if(wPt2FractionRecoMatchedFractionValues.size() >= 1 )
      mes_[label][ptPrefix+"WPt2FractionRecoMatched_separated1"]->Fill(wPt2FractionRecoMatchedFractionValues.at(0));


    
    if(simFractionValues.size() >= 1 )
      mes_[label][prefix+"Sim_separated1"]->Fill(simFractionValues.at(0));
    if(ptFractionSimFractionValues.size() >= 1 )
      mes_[label][ptPrefix+"PtFractionSim_separated1"]->Fill(ptFractionSimFractionValues.at(0));
    if(pt2FractionSimFractionValues.size() >= 1 )
      mes_[label][ptPrefix+"Pt2FractionSim_separated1"]->Fill(pt2FractionSimFractionValues.at(0));
    if(simMatchedFractionValues.size() >= 1 )
      mes_[label][prefix+"SimMatched_separated1"]->Fill(simMatchedFractionValues.at(0));
    if(ptFractionSimMatchedFractionValues.size() >= 1 )
      mes_[label][ptPrefix+"PtFractionSimMatched_separated1"]->Fill(ptFractionSimMatchedFractionValues.at(0));
    if(pt2FractionSimMatchedFractionValues.size() >= 1 )
      mes_[label][ptPrefix+"Pt2FractionSimMatched_separated1"]->Fill(pt2FractionSimMatchedFractionValues.at(0));


    // Filling values 2

    if(recoFractionValues.size() >= 2 )
      mes_[label][prefix+"Reco_separated2"]->Fill(recoFractionValues.at(1));
    if(wFractionRecoFractionValues.size() >= 2 )
      mes_[label][ptPrefix+"WFractionReco_separated2"]->Fill(wFractionRecoFractionValues.at(1));
    if(ptFractionRecoFractionValues.size() >= 2 )
      mes_[label][ptPrefix+"PtFractionReco_separated2"]->Fill(ptFractionRecoFractionValues.at(1));
    if(wPtFractionRecoFractionValues.size() >= 2 )
      mes_[label][ptPrefix+"WPtFractionReco_separated2"]->Fill(wPtFractionRecoFractionValues.at(1));
    if(pt2FractionRecoFractionValues.size() >= 2 )
      mes_[label][ptPrefix+"Pt2FractionReco_separated2"]->Fill(pt2FractionRecoFractionValues.at(1));
    if(wPt2FractionRecoFractionValues.size() >= 2 )
      mes_[label][ptPrefix+"WPt2FractionReco_separated2"]->Fill(wPt2FractionRecoFractionValues.at(1));


    if(recoMatchedFractionValues.size() >= 2 )
      mes_[label][prefix+"RecoMatched_separated2"]->Fill(recoMatchedFractionValues.at(1));
    if(wFractionRecoMatchedFractionValues.size() >= 2 )
      mes_[label][ptPrefix+"WFractionRecoMatched_separated2"]->Fill(wFractionRecoMatchedFractionValues.at(1));
    if(ptFractionRecoMatchedFractionValues.size() >= 2 )
      mes_[label][ptPrefix+"PtFractionRecoMatched_separated2"]->Fill(ptFractionRecoMatchedFractionValues.at(1));
    if(wPtFractionRecoMatchedFractionValues.size() >= 2 )
      mes_[label][ptPrefix+"WPtFractionRecoMatched_separated2"]->Fill(wPtFractionRecoMatchedFractionValues.at(1));
    if(pt2FractionRecoMatchedFractionValues.size() >= 2 )
      mes_[label][ptPrefix+"Pt2FractionRecoMatched_separated2"]->Fill(pt2FractionRecoMatchedFractionValues.at(1));
    if(wPt2FractionRecoMatchedFractionValues.size() >= 2 )
      mes_[label][ptPrefix+"WPt2FractionRecoMatched_separated2"]->Fill(wPt2FractionRecoMatchedFractionValues.at(1));


    if(simFractionValues.size() >= 2 )
      mes_[label][prefix+"Sim_separated2"]->Fill(simFractionValues.at(1));
    if(ptFractionSimFractionValues.size() >= 2 )
      mes_[label][ptPrefix+"PtFractionSim_separated2"]->Fill(ptFractionSimFractionValues.at(1));
    if(pt2FractionSimFractionValues.size() >= 2 )
      mes_[label][ptPrefix+"Pt2FractionSim_separated2"]->Fill(pt2FractionSimFractionValues.at(1));
    if(simMatchedFractionValues.size() >= 2 )
      mes_[label][prefix+"SimMatched_separated2"]->Fill(simMatchedFractionValues.at(1));
    if(ptFractionSimMatchedFractionValues.size() >= 2 )
      mes_[label][ptPrefix+"PtFractionSimMatched_separated2"]->Fill(ptFractionSimMatchedFractionValues.at(1));
    if(pt2FractionSimMatchedFractionValues.size() >= 2 )
      mes_[label][ptPrefix+"Pt2FractionSimMatched_separated2"]->Fill(pt2FractionSimMatchedFractionValues.at(1));


    // Filling values 3


    if(recoFractionValues.size() >= 3 )
      mes_[label][prefix+"Reco_separated3"]->Fill(recoFractionValues.at(2));
    if(wFractionRecoFractionValues.size() >= 3 )
      mes_[label][ptPrefix+"WFractionReco_separated3"]->Fill(wFractionRecoFractionValues.at(2));
    if(ptFractionRecoFractionValues.size() >= 3 )
      mes_[label][ptPrefix+"PtFractionReco_separated3"]->Fill(ptFractionRecoFractionValues.at(2));
    if(wPtFractionRecoFractionValues.size() >= 3 )
      mes_[label][ptPrefix+"WPtFractionReco_separated3"]->Fill(wPtFractionRecoFractionValues.at(2));
    if(pt2FractionRecoFractionValues.size() >= 3 )
      mes_[label][ptPrefix+"Pt2FractionReco_separated3"]->Fill(pt2FractionRecoFractionValues.at(2));
    if(wPt2FractionRecoFractionValues.size() >= 3 )
      mes_[label][ptPrefix+"WPt2FractionReco_separated3"]->Fill(wPt2FractionRecoFractionValues.at(2));


    if(recoMatchedFractionValues.size() >= 3 )
      mes_[label][prefix+"RecoMatched_separated3"]->Fill(recoMatchedFractionValues.at(2));
    if(wFractionRecoMatchedFractionValues.size() >= 3 )
      mes_[label][ptPrefix+"WFractionRecoMatched_separated3"]->Fill(wFractionRecoMatchedFractionValues.at(2));
    if(ptFractionRecoMatchedFractionValues.size() >= 3 )
      mes_[label][ptPrefix+"PtFractionRecoMatched_separated3"]->Fill(ptFractionRecoMatchedFractionValues.at(2));
    if(wPtFractionRecoMatchedFractionValues.size() >= 3 )
      mes_[label][ptPrefix+"WPtFractionRecoMatched_separated3"]->Fill(wPtFractionRecoMatchedFractionValues.at(2));
    if(pt2FractionRecoMatchedFractionValues.size() >= 3 )
      mes_[label][ptPrefix+"Pt2FractionRecoMatched_separated3"]->Fill(pt2FractionRecoMatchedFractionValues.at(2));
    if(wPt2FractionRecoMatchedFractionValues.size() >= 3 )
      mes_[label][ptPrefix+"WPt2FractionRecoMatched_separated3"]->Fill(wPt2FractionRecoMatchedFractionValues.at(2));


    if(simFractionValues.size() >= 3 )
      mes_[label][prefix+"Sim_separated3"]->Fill(simFractionValues.at(2));
    if(ptFractionSimFractionValues.size() >= 3 )
      mes_[label][ptPrefix+"PtFractionSim_separated3"]->Fill(ptFractionSimFractionValues.at(2));
    if(pt2FractionSimFractionValues.size() >= 3 )
      mes_[label][ptPrefix+"Pt2FractionSim_separated3"]->Fill(pt2FractionSimFractionValues.at(2));
    if(simMatchedFractionValues.size() >= 3 )
      mes_[label][prefix+"SimMatched_separated3"]->Fill(simMatchedFractionValues.at(2));
    if(ptFractionSimMatchedFractionValues.size() >= 3 )
      mes_[label][ptPrefix+"PtFractionSimMatched_separated3"]->Fill(ptFractionSimMatchedFractionValues.at(2));
    if(pt2FractionSimMatchedFractionValues.size() >= 3 )
      mes_[label][ptPrefix+"Pt2FractionSimMatched_separated3"]->Fill(pt2FractionSimMatchedFractionValues.at(2));
  }
  

}

void PrimaryVertexAnalyzer4PUSlimmed::fillGenAssociatedRecoVertexHistograms(
    const std::string& label,
    int num_pileup_vertices,
    PrimaryVertexAnalyzer4PUSlimmed::recoPrimaryVertex& v) {
  mes_[label]["RecoAllAssoc2Gen_X"]->Fill(v.x);
  mes_[label]["RecoAllAssoc2Gen_Y"]->Fill(v.y);
  mes_[label]["RecoAllAssoc2Gen_Z"]->Fill(v.z);
  mes_[label]["RecoAllAssoc2Gen_R"]->Fill(v.r);
  mes_[label]["RecoAllAssoc2Gen_Pt2"]->Fill(v.ptsq);
  mes_[label]["RecoAllAssoc2Gen_Ndof"]->Fill(v.recVtx->ndof());
  mes_[label]["RecoAllAssoc2Gen_NumTracks"]->Fill(v.nRecoTrk);
  mes_[label]["RecoAllAssoc2Gen_PU"]->Fill(num_pileup_vertices);
  mes_[label]["RecoAllAssoc2Gen_Purity"]->Fill(v.purity);
  if (v.closest_vertex_distance_z > 0.)
    mes_[label]["RecoAllAssoc2Gen_ClosestDistanceZ"]
        ->Fill(v.closest_vertex_distance_z);
  if (v.sim_vertices.size()) {
    v.kind_of_vertex |= recoPrimaryVertex::MATCHED;
    mes_[label]["RecoAllAssoc2GenMatched_X"]->Fill(v.x);
    mes_[label]["RecoAllAssoc2GenMatched_Y"]->Fill(v.y);
    mes_[label]["RecoAllAssoc2GenMatched_Z"]->Fill(v.z);
    mes_[label]["RecoAllAssoc2GenMatched_R"]->Fill(v.r);
    mes_[label]["RecoAllAssoc2GenMatched_Pt2"]->Fill(v.ptsq);
    mes_[label]["RecoAllAssoc2GenMatched_Ndof"]->Fill(v.recVtx->ndof());
    mes_[label]["RecoAllAssoc2GenMatched_NumTracks"]->Fill(v.nRecoTrk);
    mes_[label]["RecoAllAssoc2GenMatched_PU"]->Fill(num_pileup_vertices);
    mes_[label]["RecoAllAssoc2GenMatched_Purity"]->Fill(v.purity);
    if (v.closest_vertex_distance_z > 0.)
      mes_[label]["RecoAllAssoc2GenMatched_ClosestDistanceZ"]
          ->Fill(v.closest_vertex_distance_z);

    // Fill resolution and pull plots here (as in MultiTrackValidator)
    fillResolutionAndPullHistograms(label, num_pileup_vertices, v);

    // Now keep track of all RecoVTX associated to a SimVTX that
    // itself is associated to more than one RecoVTX, for
    // duplicate-rate plots on reco quantities.
    if (v.sim_vertices_internal[0]->rec_vertices.size() > 1) {
      v.kind_of_vertex |= recoPrimaryVertex::DUPLICATE;
      mes_[label]["RecoAllAssoc2MultiMatchedGen_X"]->Fill(v.x);
      mes_[label]["RecoAllAssoc2MultiMatchedGen_Y"]->Fill(v.y);
      mes_[label]["RecoAllAssoc2MultiMatchedGen_Z"]->Fill(v.z);
      mes_[label]["RecoAllAssoc2MultiMatchedGen_R"]->Fill(v.r);
      mes_[label]["RecoAllAssoc2MultiMatchedGen_Pt2"]->Fill(v.ptsq);
      mes_[label]["RecoAllAssoc2MultiMatchedGen_NumTracks"]->Fill(v.nRecoTrk);
      mes_[label]["RecoAllAssoc2MultiMatchedGen_PU"]->Fill(num_pileup_vertices);
      if (v.closest_vertex_distance_z > 0.)
        mes_[label]["RecoAllAssoc2MultiMatchedGen_ClosestDistanceZ"]
            ->Fill(v.closest_vertex_distance_z);
    }
    // This is meant to be used as "denominator" for the merge-rate
    // plots produced starting from reco quantities. We   enter here
    // only if the reco vertex has been associated, since we need info
    // from the SimVTX associated to it. In this regard, the final
    // merge-rate plot coming from reco is not to be intended as a
    // pure efficiency-like plot, since the normalization is biased.
    if (v.sim_vertices_internal[0]->closest_vertex_distance_z > 0.)
      mes_[label]["RecoAllAssoc2GenSimForMerge_ClosestDistanceZ"]
          ->Fill(v.sim_vertices_internal[0]->closest_vertex_distance_z);
  }
  // this plots are meant to be used to compute the merge rate
  if (v.sim_vertices.size() > 1) {
    v.kind_of_vertex |= recoPrimaryVertex::MERGED;
    mes_[label]["RecoAllAssoc2GenMultiMatched_X"]->Fill(v.x);
    mes_[label]["RecoAllAssoc2GenMultiMatched_Y"]->Fill(v.y);
    mes_[label]["RecoAllAssoc2GenMultiMatched_Z"]->Fill(v.z);
    mes_[label]["RecoAllAssoc2GenMultiMatched_R"]->Fill(v.r);
    mes_[label]["RecoAllAssoc2GenMultiMatched_Pt2"]->Fill(v.ptsq);
    mes_[label]["RecoAllAssoc2GenMultiMatched_NumTracks"]->Fill(v.nRecoTrk);
    mes_[label]["RecoAllAssoc2GenMultiMatched_PU"]->Fill(num_pileup_vertices);
    if (v.sim_vertices_internal[0]->closest_vertex_distance_z > 0.)
      mes_[label]["RecoAllAssoc2GenMultiMatched_ClosestDistanceZ"]
          ->Fill(v.sim_vertices_internal[0]->closest_vertex_distance_z);
  }
  mes_[label]["RecoAllAssoc2GenProperties"]->Fill(v.kind_of_vertex);


  std::string prefix;
  std::string ptPrefix;

  if(v.sim_vertices.size() == 1) {
    prefix = "RecoAllAssoc2GenSingleMatched_SharedTrackFraction";
    ptPrefix = "RecoAllAssoc2GenSingleMatched_SharedTrack";
  }
  else if(v.sim_vertices.size() > 1) {
    prefix = "RecoAllAssoc2GenMultiMatched_SharedTrackFraction";
    ptPrefix = "RecoAllAssoc2GenMultiMatched_SharedTrack";
  }

  //recoVertex-simVertex fraction values

  std::vector<double> recoFractionValues;
  std::vector<double> wFractionRecoFractionValues;
  std::vector<double> ptFractionRecoFractionValues;
  std::vector<double> wPtFractionRecoFractionValues;
  std::vector<double> pt2FractionRecoFractionValues;
  std::vector<double> wPt2FractionRecoFractionValues;
  std::vector<double> harmPtFractionRecoFractionValues;
  std::vector<double> wHarmPtFractionRecoFractionValues;
  std::vector<double> harmWPtFractionRecoFractionValues;
  std::vector<double> harmPtAvgFractionRecoFractionValues;
  std::vector<double> wHarmPtAvgFractionRecoFractionValues;
  std::vector<double> harmWPtAvgFractionRecoFractionValues;

  std::vector<double> recoMatchedFractionValues;
  std::vector<double> wFractionRecoMatchedFractionValues;
  std::vector<double> ptFractionRecoMatchedFractionValues;
  std::vector<double> wPtFractionRecoMatchedFractionValues;
  std::vector<double> pt2FractionRecoMatchedFractionValues;
  std::vector<double> wPt2FractionRecoMatchedFractionValues;
  std::vector<double> harmPtFractionRecoMatchedFractionValues;
  std::vector<double> wHarmPtFractionRecoMatchedFractionValues;
  std::vector<double> harmWPtFractionRecoMatchedFractionValues;
  std::vector<double> harmPtAvgFractionRecoMatchedFractionValues;
  std::vector<double> wHarmPtAvgFractionRecoMatchedFractionValues;
  std::vector<double> harmWPtAvgFractionRecoMatchedFractionValues;

  std::vector<double> simFractionValues;
  std::vector<double> wFractionSimFractionValues;
  std::vector<double> ptFractionSimFractionValues;
  std::vector<double> wPtFractionSimFractionValues;
  std::vector<double> pt2FractionSimFractionValues;
  std::vector<double> wPt2FractionSimFractionValues;
  std::vector<double> simMatchedFractionValues;
  std::vector<double> ptFractionSimMatchedFractionValues;
  std::vector<double> wPtFractionSimMatchedFractionValues;
  std::vector<double> pt2FractionSimMatchedFractionValues;
  std::vector<double> wPt2FractionSimMatchedFractionValues;



  for(size_t i=0; i<v.sim_vertices.size(); ++i) {
    const double sharedTracks = v.sim_vertices_num_shared_tracks[i];
    const simPrimaryVertex *simV = v.sim_vertices_internal[i];

    double wFractionReco;
    double ptFractionReco;
    double wPtFractionReco;
    double pt2FractionReco;
    double wPt2FractionReco;
    double harmPtFractionReco;
    double wHarmPtFractionReco;
    double harmWPtFractionReco;
    double harmPtAvgFractionReco;
    double wHarmPtAvgFractionReco;
    double harmWPtAvgFractionReco;

    double wFractionRecoMatched;
    double ptFractionRecoMatched;
    double wPtFractionRecoMatched;
    double pt2FractionRecoMatched;
    double wPt2FractionRecoMatched;
    double harmPtFractionRecoMatched;
    double wHarmPtFractionRecoMatched;
    double harmWPtFractionRecoMatched;
    double harmPtAvgFractionRecoMatched;
    double wHarmPtAvgFractionRecoMatched;
    double harmWPtAvgFractionRecoMatched;

    double wFractionSim;
    double ptFractionSim;
    double wPtFractionSim;
    double pt2FractionSim;
    double wPt2FractionSim;
    double ptFractionSimMatched;
    double wPtFractionSimMatched;
    double pt2FractionSimMatched;
    double wPt2FractionSimMatched;

    // avoid unused variable error with this trick
    (void)wFractionReco;
    (void)ptFractionReco;
    (void)wPtFractionReco;
    (void)pt2FractionReco;
    (void)wPt2FractionReco;
    (void)harmPtFractionReco;
    (void)wHarmPtFractionReco;
    (void)harmWPtFractionReco;
    (void)harmPtAvgFractionReco;
    (void)wHarmPtAvgFractionReco;
    (void)harmWPtAvgFractionReco;

    (void)wFractionRecoMatched;
    (void)ptFractionRecoMatched;
    (void)wPtFractionRecoMatched;
    (void)pt2FractionRecoMatched;
    (void)wPt2FractionRecoMatched;
    (void)harmPtFractionRecoMatched;
    (void)wHarmPtFractionRecoMatched;
    (void)harmWPtFractionRecoMatched;
    (void)harmPtAvgFractionRecoMatched;
    (void)wHarmPtAvgFractionRecoMatched;
    (void)harmWPtAvgFractionRecoMatched;

    (void)wFractionSim;
    (void)ptFractionSim;
    (void)wPtFractionSim;
    (void)pt2FractionSim;
    (void)wPt2FractionSim;
    (void)ptFractionSimMatched;
    (void)wPtFractionSimMatched;
    (void)pt2FractionSimMatched;
    (void)wPt2FractionSimMatched;
    
    if(doDetailedHistograms_)
    {
      std::vector<double> numerators = calculateVertexSharedTracksMomentumFractionNumerators(*(v.recVtx), *(v.sim_vertices[i]), *s2r_, *r2s_);
      std::vector<std::vector<double>> denominatorGroups = calculateVertexSharedTracksMomentumFractionDenominators(*(v.recVtx), *(v.sim_vertices[i]), *s2r_, *r2s_);
      
      wFractionReco = numerators[1]/denominatorGroups[0][1];
      ptFractionReco = numerators[2]/denominatorGroups[0][2];
      wPtFractionReco = numerators[3]/denominatorGroups[0][3];
      pt2FractionReco = numerators[4]/denominatorGroups[0][4];
      wPt2FractionReco = numerators[5]/denominatorGroups[0][5];
      harmPtFractionReco = numerators[6]/denominatorGroups[0][6];
      wHarmPtFractionReco = numerators[7]/denominatorGroups[0][7];
      harmWPtFractionReco = numerators[8]/denominatorGroups[0][8];
      harmPtAvgFractionReco = numerators[9]/denominatorGroups[0][9];
      wHarmPtAvgFractionReco = numerators[10]/denominatorGroups[0][10];
      harmWPtAvgFractionReco = numerators[11]/denominatorGroups[0][11];

      wFractionRecoMatched = numerators[1]/denominatorGroups[1][1];
      ptFractionRecoMatched = numerators[2]/denominatorGroups[1][2];
      wPtFractionRecoMatched = numerators[3]/denominatorGroups[1][3];
      pt2FractionRecoMatched = numerators[4]/denominatorGroups[1][4];
      wPt2FractionRecoMatched = numerators[5]/denominatorGroups[1][5];
      harmPtFractionRecoMatched = numerators[6]/denominatorGroups[1][6];
      wHarmPtFractionRecoMatched = numerators[7]/denominatorGroups[1][7];
      harmWPtFractionRecoMatched = numerators[8]/denominatorGroups[1][8];
      harmPtAvgFractionRecoMatched = numerators[9]/denominatorGroups[1][9];
      wHarmPtAvgFractionRecoMatched = numerators[10]/denominatorGroups[1][10];
      harmWPtAvgFractionRecoMatched = numerators[11]/denominatorGroups[1][11];

      wFractionSim = numerators[1]/denominatorGroups[2][1];
      ptFractionSim = numerators[2]/denominatorGroups[2][2];
      wPtFractionSim = numerators[3]/denominatorGroups[2][3];
      pt2FractionSim = numerators[4]/denominatorGroups[2][4];
      wPt2FractionSim = numerators[5]/denominatorGroups[2][5];
      ptFractionSimMatched = numerators[6]/denominatorGroups[2][6];
      wPtFractionSimMatched = numerators[7]/denominatorGroups[2][7];
      pt2FractionSimMatched = numerators[8]/denominatorGroups[2][8];
      wPt2FractionSimMatched = numerators[9]/denominatorGroups[2][9];

    }
    
    
    mes_[label][prefix+"Reco"]->Fill(sharedTracks/v.nRecoTrk);
    
    if(doDetailedHistograms_)
    {
      mes_[label][ptPrefix+"WFractionReco"]->Fill(wFractionReco);
      mes_[label][ptPrefix+"PtFractionReco"]->Fill(ptFractionReco);
      mes_[label][ptPrefix+"WPtFractionReco"]->Fill(wPtFractionReco);
      mes_[label][ptPrefix+"Pt2FractionReco"]->Fill(pt2FractionReco);
      mes_[label][ptPrefix+"WPt2FractionReco"]->Fill(wPt2FractionReco);
      mes_[label][ptPrefix+"HarmPtFractionReco"]->Fill(harmPtFractionReco);
      mes_[label][ptPrefix+"WHarmPtFractionReco"]->Fill(wHarmPtFractionReco);
      mes_[label][ptPrefix+"HarmWPtFractionReco"]->Fill(harmWPtFractionReco);
      mes_[label][ptPrefix+"HarmPtAvgFractionReco"]->Fill(harmPtAvgFractionReco);
      mes_[label][ptPrefix+"WHarmPtAvgFractionReco"]->Fill(wHarmPtAvgFractionReco);
      mes_[label][ptPrefix+"HarmWPtAvgFractionReco"]->Fill(harmWPtAvgFractionReco);
    }
    
    mes_[label][prefix+"RecoMatched"]->Fill(sharedTracks/v.num_matched_sim_tracks);
    
    if(doDetailedHistograms_)
    {
      mes_[label][ptPrefix+"WFractionRecoMatched"]->Fill(wFractionRecoMatched);
      mes_[label][ptPrefix+"PtFractionRecoMatched"]->Fill(ptFractionRecoMatched);
      mes_[label][ptPrefix+"WPtFractionRecoMatched"]->Fill(wPtFractionRecoMatched);
      mes_[label][ptPrefix+"Pt2FractionRecoMatched"]->Fill(pt2FractionRecoMatched);
      mes_[label][ptPrefix+"WPt2FractionRecoMatched"]->Fill(wPt2FractionRecoMatched);
      mes_[label][ptPrefix+"HarmPtFractionRecoMatched"]->Fill(harmPtFractionRecoMatched);
      mes_[label][ptPrefix+"WHarmPtFractionRecoMatched"]->Fill(wHarmPtFractionRecoMatched);
      mes_[label][ptPrefix+"HarmWPtFractionRecoMatched"]->Fill(harmWPtFractionRecoMatched);
      mes_[label][ptPrefix+"HarmPtAvgFractionRecoMatched"]->Fill(harmPtAvgFractionRecoMatched);
      mes_[label][ptPrefix+"WHarmPtAvgFractionRecoMatched"]->Fill(wHarmPtAvgFractionRecoMatched);
      mes_[label][ptPrefix+"HarmWPtAvgFractionRecoMatched"]->Fill(harmWPtAvgFractionRecoMatched);
    }
    
    mes_[label][prefix+"Sim"]->Fill(sharedTracks/simV->nGenTrk);
    
    if(doDetailedHistograms_)
    {
      mes_[label][ptPrefix+"WFractionSim"]->Fill(wFractionSim);
      mes_[label][ptPrefix+"PtFractionSim"]->Fill(ptFractionSim);
      mes_[label][ptPrefix+"WPtFractionSim"]->Fill(wPtFractionSim);
      mes_[label][ptPrefix+"Pt2FractionSim"]->Fill(pt2FractionSim);
      mes_[label][ptPrefix+"WPt2FractionSim"]->Fill(wPt2FractionSim);
    }
    
    mes_[label][prefix+"SimMatched"]->Fill(sharedTracks/simV->num_matched_reco_tracks);
    
    if(doDetailedHistograms_)
    {
      mes_[label][ptPrefix+"WFractionSimMatched"]->Fill(ptFractionSimMatched);
      mes_[label][ptPrefix+"PtFractionSimMatched"]->Fill(ptFractionSimMatched);
      mes_[label][ptPrefix+"WPtFractionSimMatched"]->Fill(wPtFractionSimMatched);
      mes_[label][ptPrefix+"Pt2FractionSimMatched"]->Fill(pt2FractionSimMatched);
      mes_[label][ptPrefix+"WPt2FractionSimMatched"]->Fill(wPt2FractionSimMatched);
    }
    
    if(doDetailedHistograms_)
    {
      recoFractionValues.push_back(sharedTracks/v.nRecoTrk);
      wFractionRecoFractionValues.push_back(wFractionReco);
      ptFractionRecoFractionValues.push_back(ptFractionReco);
      wPtFractionRecoFractionValues.push_back(wPtFractionReco);
      pt2FractionRecoFractionValues.push_back(pt2FractionReco);
      wPt2FractionRecoFractionValues.push_back(wPt2FractionReco);
      harmPtFractionRecoFractionValues.push_back(harmPtFractionReco);
      wHarmPtFractionRecoFractionValues.push_back(wHarmPtFractionReco);
      harmWPtFractionRecoFractionValues.push_back(harmWPtFractionReco);
      harmPtAvgFractionRecoFractionValues.push_back(harmPtAvgFractionReco);
      wHarmPtAvgFractionRecoFractionValues.push_back(wHarmPtAvgFractionReco);
      harmWPtAvgFractionRecoFractionValues.push_back(harmWPtAvgFractionReco);

      recoMatchedFractionValues.push_back(sharedTracks/v.num_matched_sim_tracks);
      wFractionRecoMatchedFractionValues.push_back(wFractionRecoMatched);
      ptFractionRecoMatchedFractionValues.push_back(ptFractionRecoMatched);
      wPtFractionRecoMatchedFractionValues.push_back(wPtFractionRecoMatched);
      pt2FractionRecoMatchedFractionValues.push_back(pt2FractionRecoMatched);
      wPt2FractionRecoMatchedFractionValues.push_back(wPt2FractionRecoMatched);
      harmPtFractionRecoMatchedFractionValues.push_back(harmPtFractionRecoMatched);
      wHarmPtFractionRecoMatchedFractionValues.push_back(wHarmPtFractionRecoMatched);
      harmWPtFractionRecoMatchedFractionValues.push_back(harmWPtFractionRecoMatched);
      harmPtAvgFractionRecoMatchedFractionValues.push_back(harmPtAvgFractionRecoMatched);
      wHarmPtAvgFractionRecoMatchedFractionValues.push_back(wHarmPtAvgFractionRecoMatched);
      harmWPtAvgFractionRecoMatchedFractionValues.push_back(harmWPtAvgFractionRecoMatched);

      simFractionValues.push_back(sharedTracks/simV->nGenTrk);
      wFractionSimFractionValues.push_back(wFractionSim);
      ptFractionSimFractionValues.push_back(ptFractionSim);
      wPtFractionSimFractionValues.push_back(wPtFractionSim);
      pt2FractionSimFractionValues.push_back(pt2FractionSim);
      wPt2FractionSimFractionValues.push_back(wPt2FractionSim);
      simMatchedFractionValues.push_back(sharedTracks/simV->num_matched_reco_tracks);
      ptFractionSimMatchedFractionValues.push_back(ptFractionSimMatched);
      wPtFractionSimMatchedFractionValues.push_back(wPtFractionSimMatched);
      pt2FractionSimMatchedFractionValues.push_back(pt2FractionSimMatched);
      wPt2FractionSimMatchedFractionValues.push_back(wPt2FractionSimMatched);
    }
    
  }
  
  // sort the fraction values (at most 3 for each recoVertex, because a recoVertex is connected
  // to at most three simVertices), then Fill three histograms with the fraction values by order of
  // magnitude
  if(doDetailedHistograms_)
  {
    std::sort(recoFractionValues.begin(), recoFractionValues.end());
    std::sort(wFractionRecoFractionValues.begin(), wFractionRecoFractionValues.end());
    std::sort(ptFractionRecoFractionValues.begin(), ptFractionRecoFractionValues.end());
    std::sort(wPtFractionRecoFractionValues.begin(), wPtFractionRecoFractionValues.end());
    std::sort(pt2FractionRecoFractionValues.begin(), pt2FractionRecoFractionValues.end());
    std::sort(wPt2FractionRecoFractionValues.begin(), wPt2FractionRecoFractionValues.end());
    std::sort(harmPtFractionRecoFractionValues.begin(), harmPtFractionRecoFractionValues.end());
    std::sort(wHarmPtFractionRecoFractionValues.begin(), wHarmPtFractionRecoFractionValues.end());
    std::sort(harmWPtFractionRecoFractionValues.begin(), harmWPtFractionRecoFractionValues.end());
    std::sort(harmPtAvgFractionRecoFractionValues.begin(), harmPtAvgFractionRecoFractionValues.end());
    std::sort(wHarmPtAvgFractionRecoFractionValues.begin(), wHarmPtAvgFractionRecoFractionValues.end());
    std::sort(harmWPtAvgFractionRecoFractionValues.begin(), harmWPtAvgFractionRecoFractionValues.end());

    std::sort(recoMatchedFractionValues.begin(), recoMatchedFractionValues.end());
    std::sort(wFractionRecoMatchedFractionValues.begin(), wFractionRecoMatchedFractionValues.end());
    std::sort(ptFractionRecoMatchedFractionValues.begin(), ptFractionRecoMatchedFractionValues.end());
    std::sort(wPtFractionRecoMatchedFractionValues.begin(), wPtFractionRecoMatchedFractionValues.end());
    std::sort(pt2FractionRecoMatchedFractionValues.begin(), pt2FractionRecoMatchedFractionValues.end());
    std::sort(wPt2FractionRecoMatchedFractionValues.begin(), wPt2FractionRecoMatchedFractionValues.end());
    std::sort(harmPtFractionRecoMatchedFractionValues.begin(), harmPtFractionRecoMatchedFractionValues.end());
    std::sort(wHarmPtFractionRecoMatchedFractionValues.begin(), wHarmPtFractionRecoMatchedFractionValues.end());
    std::sort(harmWPtFractionRecoMatchedFractionValues.begin(), harmWPtFractionRecoMatchedFractionValues.end());
    std::sort(harmPtAvgFractionRecoMatchedFractionValues.begin(), harmPtAvgFractionRecoMatchedFractionValues.end());
    std::sort(wHarmPtAvgFractionRecoMatchedFractionValues.begin(), wHarmPtAvgFractionRecoMatchedFractionValues.end());
    std::sort(harmWPtAvgFractionRecoMatchedFractionValues.begin(), harmWPtAvgFractionRecoMatchedFractionValues.end());

    std::sort(simFractionValues.begin(), simFractionValues.end());
    std::sort(wFractionSimFractionValues.begin(), wFractionSimFractionValues.end());
    std::sort(ptFractionSimFractionValues.begin(), ptFractionSimFractionValues.end());
    std::sort(wPtFractionSimFractionValues.begin(), wPtFractionSimFractionValues.end());
    std::sort(pt2FractionSimFractionValues.begin(), pt2FractionSimFractionValues.end());
    std::sort(wPt2FractionSimFractionValues.begin(), wPt2FractionSimFractionValues.end());
    std::sort(simMatchedFractionValues.begin(), simMatchedFractionValues.end());
    std::sort(ptFractionSimMatchedFractionValues.begin(), ptFractionSimMatchedFractionValues.end());
    std::sort(wPtFractionSimMatchedFractionValues.begin(), wPtFractionSimMatchedFractionValues.end());
    std::sort(pt2FractionSimMatchedFractionValues.begin(), pt2FractionSimMatchedFractionValues.end());
    std::sort(wPt2FractionSimMatchedFractionValues.begin(), wPt2FractionSimMatchedFractionValues.end());


    // reverse order, so that the biggest fraction is first

    std::reverse(recoFractionValues.begin(), recoFractionValues.end());
    std::reverse(wFractionRecoFractionValues.begin(), wFractionRecoFractionValues.end());
    std::reverse(ptFractionRecoFractionValues.begin(), ptFractionRecoFractionValues.end());
    std::reverse(wPtFractionRecoFractionValues.begin(), wPtFractionRecoFractionValues.end());
    std::reverse(pt2FractionRecoFractionValues.begin(), pt2FractionRecoFractionValues.end());
    std::reverse(wPt2FractionRecoFractionValues.begin(), wPt2FractionRecoFractionValues.end());
    std::reverse(harmPtFractionRecoFractionValues.begin(), harmPtFractionRecoFractionValues.end());
    std::reverse(wHarmPtFractionRecoFractionValues.begin(), wHarmPtFractionRecoFractionValues.end());
    std::reverse(harmWPtFractionRecoFractionValues.begin(), harmWPtFractionRecoFractionValues.end());
    std::reverse(harmPtAvgFractionRecoFractionValues.begin(), harmPtAvgFractionRecoFractionValues.end());
    std::reverse(wHarmPtAvgFractionRecoFractionValues.begin(), wHarmPtAvgFractionRecoFractionValues.end());
    std::reverse(harmWPtAvgFractionRecoFractionValues.begin(), harmWPtAvgFractionRecoFractionValues.end());

    std::reverse(recoMatchedFractionValues.begin(), recoMatchedFractionValues.end());
    std::reverse(wFractionRecoMatchedFractionValues.begin(), wFractionRecoMatchedFractionValues.end());
    std::reverse(ptFractionRecoMatchedFractionValues.begin(), ptFractionRecoMatchedFractionValues.end());
    std::reverse(wPtFractionRecoMatchedFractionValues.begin(), wPtFractionRecoMatchedFractionValues.end());
    std::reverse(pt2FractionRecoMatchedFractionValues.begin(), pt2FractionRecoMatchedFractionValues.end());
    std::reverse(wPt2FractionRecoMatchedFractionValues.begin(), wPt2FractionRecoMatchedFractionValues.end());
    std::reverse(harmPtFractionRecoMatchedFractionValues.begin(), harmPtFractionRecoMatchedFractionValues.end());
    std::reverse(wHarmPtFractionRecoMatchedFractionValues.begin(), wHarmPtFractionRecoMatchedFractionValues.end());
    std::reverse(harmWPtFractionRecoMatchedFractionValues.begin(), harmWPtFractionRecoMatchedFractionValues.end());
    std::reverse(harmPtAvgFractionRecoMatchedFractionValues.begin(), harmPtAvgFractionRecoMatchedFractionValues.end());
    std::reverse(wHarmPtAvgFractionRecoMatchedFractionValues.begin(), wHarmPtAvgFractionRecoMatchedFractionValues.end());
    std::reverse(harmWPtAvgFractionRecoMatchedFractionValues.begin(), harmWPtAvgFractionRecoMatchedFractionValues.end());

    std::reverse(simFractionValues.begin(), simFractionValues.end());
    std::reverse(wFractionSimFractionValues.begin(), wFractionSimFractionValues.end());
    std::reverse(ptFractionSimFractionValues.begin(), ptFractionSimFractionValues.end());
    std::reverse(wPtFractionSimFractionValues.begin(), wPtFractionSimFractionValues.end());
    std::reverse(pt2FractionSimFractionValues.begin(), pt2FractionSimFractionValues.end());
    std::reverse(wPt2FractionSimFractionValues.begin(), wPt2FractionSimFractionValues.end());
    std::reverse(simMatchedFractionValues.begin(), simMatchedFractionValues.end());
    std::reverse(ptFractionSimMatchedFractionValues.begin(), ptFractionSimMatchedFractionValues.end());
    std::reverse(wPtFractionSimMatchedFractionValues.begin(), wPtFractionSimMatchedFractionValues.end());
    std::reverse(pt2FractionSimMatchedFractionValues.begin(), pt2FractionSimMatchedFractionValues.end());
    std::reverse(wPt2FractionSimMatchedFractionValues.begin(), wPt2FractionSimMatchedFractionValues.end());


    // Filling values 1

    if(recoFractionValues.size() >= 1 )
      mes_[label][prefix+"Reco_separated1"]->Fill(recoFractionValues.at(0));
    if(wFractionRecoFractionValues.size() >= 1 )
      mes_[label][ptPrefix+"WFractionReco_separated1"]->Fill(wFractionRecoFractionValues.at(0));
    if(ptFractionRecoFractionValues.size() >= 1 )
      mes_[label][ptPrefix+"PtFractionReco_separated1"]->Fill(ptFractionRecoFractionValues.at(0));
    if(wPtFractionRecoFractionValues.size() >= 1 )
      mes_[label][ptPrefix+"WPtFractionReco_separated1"]->Fill(wPtFractionRecoFractionValues.at(0));
    if(pt2FractionRecoFractionValues.size() >= 1 )
      mes_[label][ptPrefix+"Pt2FractionReco_separated1"]->Fill(pt2FractionRecoFractionValues.at(0));
    if(wPt2FractionRecoFractionValues.size() >= 1 )
      mes_[label][ptPrefix+"WPt2FractionReco_separated1"]->Fill(wPt2FractionRecoFractionValues.at(0));
    if(harmPtFractionRecoFractionValues.size() >= 1 )
      mes_[label][ptPrefix+"HarmPtFractionReco_separated1"]->Fill(harmPtFractionRecoFractionValues.at(0));
    if(wHarmPtFractionRecoFractionValues.size() >= 1 )
      mes_[label][ptPrefix+"WHarmPtFractionReco_separated1"]->Fill(wHarmPtFractionRecoFractionValues.at(0));
    if(harmWPtFractionRecoFractionValues.size() >= 1 )
      mes_[label][ptPrefix+"HarmWPtFractionReco_separated1"]->Fill(harmWPtFractionRecoFractionValues.at(0));
    if(harmPtAvgFractionRecoFractionValues.size() >= 1 )
      mes_[label][ptPrefix+"HarmPtAvgFractionReco_separated1"]->Fill(harmPtAvgFractionRecoFractionValues.at(0));
    if(wHarmPtAvgFractionRecoFractionValues.size() >= 1 )
      mes_[label][ptPrefix+"WHarmPtAvgFractionReco_separated1"]->Fill(wHarmPtAvgFractionRecoFractionValues.at(0));
    if(harmWPtAvgFractionRecoFractionValues.size() >= 1 )
      mes_[label][ptPrefix+"HarmWPtAvgFractionReco_separated1"]->Fill(harmWPtAvgFractionRecoFractionValues.at(0));

    if(recoMatchedFractionValues.size() >= 1 )
      mes_[label][prefix+"RecoMatched_separated1"]->Fill(recoMatchedFractionValues.at(0));
    if(wFractionRecoMatchedFractionValues.size() >= 1 )
      mes_[label][ptPrefix+"WFractionRecoMatched_separated1"]->Fill(wFractionRecoMatchedFractionValues.at(0));
    if(ptFractionRecoMatchedFractionValues.size() >= 1 )
      mes_[label][ptPrefix+"PtFractionRecoMatched_separated1"]->Fill(ptFractionRecoMatchedFractionValues.at(0));
    if(wPtFractionRecoMatchedFractionValues.size() >= 1 )
      mes_[label][ptPrefix+"WPtFractionRecoMatched_separated1"]->Fill(wPtFractionRecoMatchedFractionValues.at(0));
    if(pt2FractionRecoMatchedFractionValues.size() >= 1 )
      mes_[label][ptPrefix+"Pt2FractionRecoMatched_separated1"]->Fill(pt2FractionRecoMatchedFractionValues.at(0));
    if(wPt2FractionRecoMatchedFractionValues.size() >= 1 )
      mes_[label][ptPrefix+"WPt2FractionRecoMatched_separated1"]->Fill(wPt2FractionRecoMatchedFractionValues.at(0));
    if(harmPtFractionRecoMatchedFractionValues.size() >= 1 )
      mes_[label][ptPrefix+"HarmPtFractionRecoMatched_separated1"]->Fill(harmPtFractionRecoMatchedFractionValues.at(0));
    if(wHarmPtFractionRecoMatchedFractionValues.size() >= 1 )
      mes_[label][ptPrefix+"WHarmPtFractionRecoMatched_separated1"]->Fill(wHarmPtFractionRecoMatchedFractionValues.at(0));
    if(harmWPtFractionRecoMatchedFractionValues.size() >= 1 )
      mes_[label][ptPrefix+"HarmWPtFractionRecoMatched_separated1"]->Fill(harmWPtFractionRecoMatchedFractionValues.at(0));
    if(harmPtAvgFractionRecoMatchedFractionValues.size() >= 1 )
      mes_[label][ptPrefix+"HarmPtAvgFractionRecoMatched_separated1"]->Fill(harmPtAvgFractionRecoMatchedFractionValues.at(0));
    if(wHarmPtAvgFractionRecoMatchedFractionValues.size() >= 1 )
      mes_[label][ptPrefix+"WHarmPtAvgFractionRecoMatched_separated1"]->Fill(wHarmPtAvgFractionRecoMatchedFractionValues.at(0));
    if(harmWPtAvgFractionRecoMatchedFractionValues.size() >= 1 )
      mes_[label][ptPrefix+"HarmWPtAvgFractionRecoMatched_separated1"]->Fill(harmWPtAvgFractionRecoMatchedFractionValues.at(0));

    
    if(simFractionValues.size() >= 1 )
      mes_[label][prefix+"Sim_separated1"]->Fill(simFractionValues.at(0));
    if(wFractionSimFractionValues.size() >= 1 )
      mes_[label][ptPrefix+"WFractionSim_separated1"]->Fill(wFractionSimFractionValues.at(0));
    if(ptFractionSimFractionValues.size() >= 1 )
      mes_[label][ptPrefix+"PtFractionSim_separated1"]->Fill(ptFractionSimFractionValues.at(0));
    if(wPtFractionSimFractionValues.size() >= 1 )
      mes_[label][ptPrefix+"WPtFractionSim_separated1"]->Fill(wPtFractionSimFractionValues.at(0));
    if(pt2FractionSimFractionValues.size() >= 1 )
      mes_[label][ptPrefix+"Pt2FractionSim_separated1"]->Fill(pt2FractionSimFractionValues.at(0));
    if(wPt2FractionSimFractionValues.size() >= 1 )
      mes_[label][ptPrefix+"WPt2FractionSim_separated1"]->Fill(wPt2FractionSimFractionValues.at(0));
    if(simMatchedFractionValues.size() >= 1 )
      mes_[label][prefix+"SimMatched_separated1"]->Fill(simMatchedFractionValues.at(0));
    if(ptFractionSimMatchedFractionValues.size() >= 1 )
      mes_[label][ptPrefix+"PtFractionSimMatched_separated1"]->Fill(ptFractionSimMatchedFractionValues.at(0));
    if(wPtFractionSimMatchedFractionValues.size() >= 1 )
      mes_[label][ptPrefix+"WPtFractionSimMatched_separated1"]->Fill(wPtFractionSimMatchedFractionValues.at(0));
    if(pt2FractionSimMatchedFractionValues.size() >= 1 )
      mes_[label][ptPrefix+"Pt2FractionSimMatched_separated1"]->Fill(pt2FractionSimMatchedFractionValues.at(0));
    if(wPt2FractionSimMatchedFractionValues.size() >= 1 )
      mes_[label][ptPrefix+"WPt2FractionSimMatched_separated1"]->Fill(wPt2FractionSimMatchedFractionValues.at(0));


    // Filling values 2

    if(recoFractionValues.size() >= 2 )
      mes_[label][prefix+"Reco_separated2"]->Fill(recoFractionValues.at(1));
    if(wFractionRecoFractionValues.size() >= 2 )
      mes_[label][ptPrefix+"WFractionReco_separated2"]->Fill(wFractionRecoFractionValues.at(1));
    if(ptFractionRecoFractionValues.size() >= 2 )
      mes_[label][ptPrefix+"PtFractionReco_separated2"]->Fill(ptFractionRecoFractionValues.at(1));
    if(wPtFractionRecoFractionValues.size() >= 2 )
      mes_[label][ptPrefix+"WPtFractionReco_separated2"]->Fill(wPtFractionRecoFractionValues.at(1));
    if(pt2FractionRecoFractionValues.size() >= 2 )
      mes_[label][ptPrefix+"Pt2FractionReco_separated2"]->Fill(pt2FractionRecoFractionValues.at(1));
    if(wPt2FractionRecoFractionValues.size() >= 2 )
      mes_[label][ptPrefix+"WPt2FractionReco_separated2"]->Fill(wPt2FractionRecoFractionValues.at(1));
    if(harmPtFractionRecoFractionValues.size() >= 2 )
      mes_[label][ptPrefix+"HarmPtFractionReco_separated2"]->Fill(harmPtFractionRecoFractionValues.at(1));
    if(wHarmPtFractionRecoFractionValues.size() >= 2 )
      mes_[label][ptPrefix+"WHarmPtFractionReco_separated2"]->Fill(wHarmPtFractionRecoFractionValues.at(1));
    if(harmWPtFractionRecoFractionValues.size() >= 2 )
      mes_[label][ptPrefix+"HarmWPtFractionReco_separated2"]->Fill(harmWPtFractionRecoFractionValues.at(1));
    if(harmPtAvgFractionRecoFractionValues.size() >= 2 )
      mes_[label][ptPrefix+"HarmPtAvgFractionReco_separated2"]->Fill(harmPtAvgFractionRecoFractionValues.at(1));
    if(wHarmPtAvgFractionRecoFractionValues.size() >= 2 )
      mes_[label][ptPrefix+"WHarmPtAvgFractionReco_separated2"]->Fill(wHarmPtAvgFractionRecoFractionValues.at(1));
    if(harmWPtAvgFractionRecoFractionValues.size() >= 2 )
      mes_[label][ptPrefix+"HarmWPtAvgFractionReco_separated2"]->Fill(harmWPtAvgFractionRecoFractionValues.at(1));

    if(recoMatchedFractionValues.size() >= 2 )
      mes_[label][prefix+"RecoMatched_separated2"]->Fill(recoMatchedFractionValues.at(1));
    if(wFractionRecoMatchedFractionValues.size() >= 2 )
      mes_[label][ptPrefix+"WFractionRecoMatched_separated2"]->Fill(wFractionRecoMatchedFractionValues.at(1));
    if(ptFractionRecoMatchedFractionValues.size() >= 2 )
      mes_[label][ptPrefix+"PtFractionRecoMatched_separated2"]->Fill(ptFractionRecoMatchedFractionValues.at(1));
    if(wPtFractionRecoMatchedFractionValues.size() >= 2 )
      mes_[label][ptPrefix+"WPtFractionRecoMatched_separated2"]->Fill(wPtFractionRecoMatchedFractionValues.at(1));
    if(pt2FractionRecoMatchedFractionValues.size() >= 2 )
      mes_[label][ptPrefix+"Pt2FractionRecoMatched_separated2"]->Fill(pt2FractionRecoMatchedFractionValues.at(1));
    if(wPt2FractionRecoMatchedFractionValues.size() >= 2 )
      mes_[label][ptPrefix+"WPt2FractionRecoMatched_separated2"]->Fill(wPt2FractionRecoMatchedFractionValues.at(1));
    if(harmPtFractionRecoMatchedFractionValues.size() >= 2 )
      mes_[label][ptPrefix+"HarmPtFractionRecoMatched_separated2"]->Fill(harmPtFractionRecoMatchedFractionValues.at(1));
    if(wHarmPtFractionRecoMatchedFractionValues.size() >= 2 )
      mes_[label][ptPrefix+"WHarmPtFractionRecoMatched_separated2"]->Fill(wHarmPtFractionRecoMatchedFractionValues.at(1));
    if(harmWPtFractionRecoMatchedFractionValues.size() >= 2 )
      mes_[label][ptPrefix+"HarmWPtFractionRecoMatched_separated2"]->Fill(harmWPtFractionRecoMatchedFractionValues.at(1));
    if(harmPtAvgFractionRecoMatchedFractionValues.size() >= 2 )
      mes_[label][ptPrefix+"HarmPtAvgFractionRecoMatched_separated2"]->Fill(harmPtAvgFractionRecoMatchedFractionValues.at(1));
    if(wHarmPtAvgFractionRecoMatchedFractionValues.size() >= 2 )
      mes_[label][ptPrefix+"WHarmPtAvgFractionRecoMatched_separated2"]->Fill(wHarmPtAvgFractionRecoMatchedFractionValues.at(1));
    if(harmWPtAvgFractionRecoMatchedFractionValues.size() >= 2 )
      mes_[label][ptPrefix+"HarmWPtAvgFractionRecoMatched_separated2"]->Fill(harmWPtAvgFractionRecoMatchedFractionValues.at(1));

    if(simFractionValues.size() >= 2 )
      mes_[label][prefix+"Sim_separated2"]->Fill(simFractionValues.at(1));
    if(wFractionSimFractionValues.size() >= 2 )
      mes_[label][ptPrefix+"WFractionSim_separated2"]->Fill(wFractionSimFractionValues.at(1));
    if(ptFractionSimFractionValues.size() >= 2 )
      mes_[label][ptPrefix+"PtFractionSim_separated2"]->Fill(ptFractionSimFractionValues.at(1));
    if(wPtFractionSimFractionValues.size() >= 2 )
      mes_[label][ptPrefix+"WPtFractionSim_separated2"]->Fill(wPtFractionSimFractionValues.at(1));
    if(pt2FractionSimFractionValues.size() >= 2 )
      mes_[label][ptPrefix+"Pt2FractionSim_separated2"]->Fill(pt2FractionSimFractionValues.at(1));
    if(wPt2FractionSimFractionValues.size() >= 2 )
      mes_[label][ptPrefix+"WPt2FractionSim_separated2"]->Fill(wPt2FractionSimFractionValues.at(1));
    if(simMatchedFractionValues.size() >= 2 )
      mes_[label][prefix+"SimMatched_separated2"]->Fill(simMatchedFractionValues.at(1));
    if(ptFractionSimMatchedFractionValues.size() >= 2 )
      mes_[label][ptPrefix+"PtFractionSimMatched_separated2"]->Fill(ptFractionSimMatchedFractionValues.at(1));
    if(wPtFractionSimMatchedFractionValues.size() >= 2 )
      mes_[label][ptPrefix+"WPtFractionSimMatched_separated2"]->Fill(wPtFractionSimMatchedFractionValues.at(1));
    if(pt2FractionSimMatchedFractionValues.size() >= 2 )
      mes_[label][ptPrefix+"Pt2FractionSimMatched_separated2"]->Fill(pt2FractionSimMatchedFractionValues.at(1));
    if(wPt2FractionSimMatchedFractionValues.size() >= 2 )
      mes_[label][ptPrefix+"WPt2FractionSimMatched_separated2"]->Fill(wPt2FractionSimMatchedFractionValues.at(1));



    // Filling values 3


    if(recoFractionValues.size() >= 3 )
      mes_[label][prefix+"Reco_separated3"]->Fill(recoFractionValues.at(2));
    if(wFractionRecoFractionValues.size() >= 3 )
      mes_[label][ptPrefix+"WFractionReco_separated3"]->Fill(wFractionRecoFractionValues.at(2));
    if(ptFractionRecoFractionValues.size() >= 3 )
      mes_[label][ptPrefix+"PtFractionReco_separated3"]->Fill(ptFractionRecoFractionValues.at(2));
    if(wPtFractionRecoFractionValues.size() >= 3 )
      mes_[label][ptPrefix+"WPtFractionReco_separated3"]->Fill(wPtFractionRecoFractionValues.at(2));
    if(pt2FractionRecoFractionValues.size() >= 3 )
      mes_[label][ptPrefix+"Pt2FractionReco_separated3"]->Fill(pt2FractionRecoFractionValues.at(2));
    if(wPt2FractionRecoFractionValues.size() >= 3 )
      mes_[label][ptPrefix+"WPt2FractionReco_separated3"]->Fill(wPt2FractionRecoFractionValues.at(2));
    if(harmPtFractionRecoFractionValues.size() >= 3 )
      mes_[label][ptPrefix+"HarmPtFractionReco_separated3"]->Fill(harmPtFractionRecoFractionValues.at(2));
    if(wHarmPtFractionRecoFractionValues.size() >= 3 )
      mes_[label][ptPrefix+"WHarmPtFractionReco_separated3"]->Fill(wHarmPtFractionRecoFractionValues.at(2));
    if(harmWPtFractionRecoFractionValues.size() >= 3 )
      mes_[label][ptPrefix+"HarmWPtFractionReco_separated3"]->Fill(harmWPtFractionRecoFractionValues.at(2));
    if(harmPtAvgFractionRecoFractionValues.size() >= 3 )
      mes_[label][ptPrefix+"HarmPtAvgFractionReco_separated3"]->Fill(harmPtAvgFractionRecoFractionValues.at(2));
    if(wHarmPtAvgFractionRecoFractionValues.size() >= 3 )
      mes_[label][ptPrefix+"WHarmPtAvgFractionReco_separated3"]->Fill(wHarmPtAvgFractionRecoFractionValues.at(2));
    if(harmWPtAvgFractionRecoFractionValues.size() >= 3 )
      mes_[label][ptPrefix+"HarmWPtAvgFractionReco_separated3"]->Fill(harmWPtAvgFractionRecoFractionValues.at(2));

    if(recoMatchedFractionValues.size() >= 3 )
      mes_[label][prefix+"RecoMatched_separated3"]->Fill(recoMatchedFractionValues.at(2));
    if(wFractionRecoMatchedFractionValues.size() >= 3 )
      mes_[label][ptPrefix+"WFractionRecoMatched_separated3"]->Fill(wFractionRecoMatchedFractionValues.at(2));
    if(ptFractionRecoMatchedFractionValues.size() >= 3 )
      mes_[label][ptPrefix+"PtFractionRecoMatched_separated3"]->Fill(ptFractionRecoMatchedFractionValues.at(2));
    if(wPtFractionRecoMatchedFractionValues.size() >= 3 )
      mes_[label][ptPrefix+"WPtFractionRecoMatched_separated3"]->Fill(wPtFractionRecoMatchedFractionValues.at(2));
    if(pt2FractionRecoMatchedFractionValues.size() >= 3 )
      mes_[label][ptPrefix+"Pt2FractionRecoMatched_separated3"]->Fill(pt2FractionRecoMatchedFractionValues.at(2));
    if(wPt2FractionRecoMatchedFractionValues.size() >= 3 )
      mes_[label][ptPrefix+"WPt2FractionRecoMatched_separated3"]->Fill(wPt2FractionRecoMatchedFractionValues.at(2));
    if(harmPtFractionRecoMatchedFractionValues.size() >= 3 )
      mes_[label][ptPrefix+"HarmPtFractionRecoMatched_separated3"]->Fill(harmPtFractionRecoMatchedFractionValues.at(2));
    if(wHarmPtFractionRecoMatchedFractionValues.size() >= 3 )
      mes_[label][ptPrefix+"WHarmPtFractionRecoMatched_separated3"]->Fill(wHarmPtFractionRecoMatchedFractionValues.at(2));
    if(harmWPtFractionRecoMatchedFractionValues.size() >= 3 )
      mes_[label][ptPrefix+"HarmWPtFractionRecoMatched_separated3"]->Fill(harmWPtFractionRecoMatchedFractionValues.at(2));
    if(harmPtAvgFractionRecoMatchedFractionValues.size() >= 3 )
      mes_[label][ptPrefix+"HarmPtAvgFractionRecoMatched_separated3"]->Fill(harmPtAvgFractionRecoMatchedFractionValues.at(2));
    if(wHarmPtAvgFractionRecoMatchedFractionValues.size() >= 3 )
      mes_[label][ptPrefix+"WHarmPtAvgFractionRecoMatched_separated3"]->Fill(wHarmPtAvgFractionRecoMatchedFractionValues.at(2));
    if(harmWPtAvgFractionRecoMatchedFractionValues.size() >= 3 )
      mes_[label][ptPrefix+"HarmWPtAvgFractionRecoMatched_separated3"]->Fill(harmWPtAvgFractionRecoMatchedFractionValues.at(2));

    if(simFractionValues.size() >= 3 )
      mes_[label][prefix+"Sim_separated3"]->Fill(simFractionValues.at(2));
    if(wFractionSimFractionValues.size() >= 3 )
      mes_[label][ptPrefix+"WFractionSim_separated3"]->Fill(wFractionSimFractionValues.at(2));
    if(ptFractionSimFractionValues.size() >= 3 )
      mes_[label][ptPrefix+"PtFractionSim_separated3"]->Fill(ptFractionSimFractionValues.at(2));
    if(wPtFractionSimFractionValues.size() >= 3 )
      mes_[label][ptPrefix+"WPtFractionSim_separated3"]->Fill(wPtFractionSimFractionValues.at(2));
    if(pt2FractionSimFractionValues.size() >= 3 )
      mes_[label][ptPrefix+"Pt2FractionSim_separated3"]->Fill(pt2FractionSimFractionValues.at(2));
    if(wPt2FractionSimFractionValues.size() >= 3 )
      mes_[label][ptPrefix+"WPt2FractionSim_separated3"]->Fill(wPt2FractionSimFractionValues.at(2));
    if(simMatchedFractionValues.size() >= 3 )
      mes_[label][prefix+"SimMatched_separated3"]->Fill(simMatchedFractionValues.at(2));
    if(ptFractionSimMatchedFractionValues.size() >= 3 )
      mes_[label][ptPrefix+"PtFractionSimMatched_separated3"]->Fill(ptFractionSimMatchedFractionValues.at(2));
    if(wPtFractionSimMatchedFractionValues.size() >= 3 )
      mes_[label][ptPrefix+"WPtFractionSimMatched_separated3"]->Fill(wPtFractionSimMatchedFractionValues.at(2));
    if(pt2FractionSimMatchedFractionValues.size() >= 3 )
      mes_[label][ptPrefix+"Pt2FractionSimMatched_separated3"]->Fill(pt2FractionSimMatchedFractionValues.at(2));
    if(wPt2FractionSimMatchedFractionValues.size() >= 3 )
      mes_[label][ptPrefix+"WPt2FractionSimMatched_separated3"]->Fill(wPt2FractionSimMatchedFractionValues.at(2));
  }
  

}

void PrimaryVertexAnalyzer4PUSlimmed::fillResolutionAndPullHistograms(
    const std::string& label,
    int num_pileup_vertices,
    PrimaryVertexAnalyzer4PUSlimmed::recoPrimaryVertex& v) {

  std::string prefix = "RecoAllAssoc2GenMatched";
  if(v.sim_vertices_internal.size() > 1) {
    prefix += "Merged";
  }

  // Use the best match as defined by the vertex truth associator
  // reco-tracks as the best match
  const simPrimaryVertex& bestMatch = *(v.sim_vertices_internal[0]);
  const double xres = v.x - bestMatch.x;
  const double yres = v.y - bestMatch.y;
  const double zres = v.z - bestMatch.z;
  const double pt2res = v.ptsq - bestMatch.ptsq;

  const double xresol = xres;
  const double yresol = yres;
  const double zresol = zres;
  const double pt2resol = pt2res/v.ptsq;

  mes_[label][prefix+"_ResolX"]->Fill(xresol);
  mes_[label][prefix+"_ResolY"]->Fill(yresol);
  mes_[label][prefix+"_ResolZ"]->Fill(zresol);
  mes_[label][prefix+"_ResolPt2"]->Fill(pt2resol);

  mes_[label][prefix+"_ResolX_vs_PU"]->Fill(num_pileup_vertices, xresol);
  mes_[label][prefix+"_ResolY_vs_PU"]->Fill(num_pileup_vertices, yresol);
  mes_[label][prefix+"_ResolZ_vs_PU"]->Fill(num_pileup_vertices, zresol);
  mes_[label][prefix+"_ResolPt2_vs_PU"]->Fill(num_pileup_vertices, pt2resol);

  mes_[label][prefix+"_ResolX_vs_NumTracks"]->Fill(v.nRecoTrk, xresol);
  mes_[label][prefix+"_ResolY_vs_NumTracks"]->Fill(v.nRecoTrk, yresol);
  mes_[label][prefix+"_ResolZ_vs_NumTracks"]->Fill(v.nRecoTrk, zresol);
  mes_[label][prefix+"_ResolPt2_vs_NumTracks"]->Fill(v.nRecoTrk, pt2resol);

  mes_[label][prefix+"_PullX"]->Fill(xres/v.recVtx->xError());
  mes_[label][prefix+"_PullY"]->Fill(yres/v.recVtx->yError());
  mes_[label][prefix+"_PullZ"]->Fill(zres/v.recVtx->zError());
}

bool PrimaryVertexAnalyzer4PUSlimmed::matchRecoTrack2SimSignal(const reco::TrackBaseRef& recoTrack) {
  auto found = r2s_->find(recoTrack);

  // reco track not matched to any TP
  if(found == r2s_->end())
    return false;

  // reco track matched to some TP from signal vertex
  for(const auto& tp: found->val) {
    if(tp.first->eventId().bunchCrossing() == 0 && tp.first->eventId().event() == 0)
      return true;
  }

  // reco track not matched to any TP from signal vertex
  return false;
}

void PrimaryVertexAnalyzer4PUSlimmed::calculatePurityAndFillHistograms(
    const std::string& label,
    std::vector<recoPrimaryVertex>& recopvs,
    int genpv_position_in_reco_collection,
    bool signal_is_highest_pt) {
  if(recopvs.empty()) return;

  std::vector<double> vtx_sumpt_sigmatched;
  std::vector<double> vtx_sumpt2_sigmatched;

  vtx_sumpt_sigmatched.reserve(recopvs.size());
  vtx_sumpt2_sigmatched.reserve(recopvs.size());

  // Calculate purity
  for(auto& v: recopvs) {
    double sumpt_all = 0;
    double sumpt_sigmatched = 0;
    double sumpt2_sigmatched = 0;
    const reco::Vertex *vertex = v.recVtx;
    for(auto iTrack = vertex->tracks_begin(); iTrack != vertex->tracks_end(); ++iTrack) {
      double pt = (*iTrack)->pt();
      sumpt_all += pt;
      if(matchRecoTrack2SimSignal(*iTrack)) {
        sumpt_sigmatched += pt;
        sumpt2_sigmatched += pt*pt;
      }
    }
    v.purity = sumpt_sigmatched / sumpt_all;

    vtx_sumpt_sigmatched.push_back(sumpt_sigmatched);
    vtx_sumpt2_sigmatched.push_back(sumpt2_sigmatched);
  }

  const double vtxAll_sumpt_sigmatched = std::accumulate(vtx_sumpt_sigmatched.begin(), vtx_sumpt_sigmatched.end(), 0.0);
  const double vtxNot0_sumpt_sigmatched = vtxAll_sumpt_sigmatched - vtx_sumpt_sigmatched[0];
  const double missing = vtxNot0_sumpt_sigmatched / vtxAll_sumpt_sigmatched;

  // Fill purity
  std::string prefix = "RecoPVAssoc2GenPVNotMatched_";
  if(genpv_position_in_reco_collection == 0)
    prefix = "RecoPVAssoc2GenPVMatched_";

  mes_[label][prefix+"Purity"]->Fill(recopvs[0].purity);
  mes_[label][prefix+"Missing"]->Fill(missing);
  auto hpurity = mes_[label][prefix+"Purity_vs_Index"];
  for(size_t i=0; i<recopvs.size(); ++i) {
    hpurity->Fill(i, recopvs[i].purity);
  }

  // Fill sumpt2
  for(size_t i=0; i<recopvs.size(); ++i) {
    if(static_cast<int>(i) == genpv_position_in_reco_collection) {
      mes_[label]["RecoAssoc2GenPVMatched_Pt2"]->Fill(recopvs[i].ptsq);
    }
    else {
      double pt2 = recopvs[i].ptsq;
      mes_[label]["RecoAssoc2GenPVNotMatched_Pt2"]->Fill(pt2);
      // Subtract hard-scatter track pt2 from the pileup pt2
      double pt2_pu = pt2-vtx_sumpt2_sigmatched[i];
      mes_[label]["RecoAssoc2GenPVNotMatched_GenPVTracksRemoved_Pt2"]->Fill(pt2_pu);
    }
  }
  if(!signal_is_highest_pt && genpv_position_in_reco_collection >= 0)
    mes_[label]["RecoAssoc2GenPVMatchedNotHighest_Pt2"]->Fill(recopvs[genpv_position_in_reco_collection].ptsq);
}

/* Extract information form TrackingParticles/TrackingVertex and fill
 * the helper class simPrimaryVertex with proper generation-level
 * information */
std::vector<PrimaryVertexAnalyzer4PUSlimmed::simPrimaryVertex>
PrimaryVertexAnalyzer4PUSlimmed::getSimPVs(
    const edm::Handle<TrackingVertexCollection>& tVC) {
  std::vector<PrimaryVertexAnalyzer4PUSlimmed::simPrimaryVertex> simpv;
  int current_event = -1;

  if (verbose_) {
    std::cout << "getSimPVs TrackingVertexCollection " << std::endl;
  }

  for (TrackingVertexCollection::const_iterator v = tVC->begin();
       v != tVC->end(); ++v) {
    if (verbose_) {
      std::cout << "BunchX.EventId: " << v->eventId().bunchCrossing() << "."
                << (v->eventId()).event() << " Position: " << v->position()
                << " G4/HepMC Vertices: " << v->g4Vertices().size() << "/"
                << v->genVertices().size()
                << "   t = " << v->position().t() * 1.e12
                << "    == 0:" << (v->position().t() > 0) << std::endl;
      for (TrackingVertex::g4v_iterator gv = v->g4Vertices_begin();
           gv != v->g4Vertices_end(); gv++) {
        std::cout << *gv << std::endl;
      }
      std::cout << "----------" << std::endl;
    }  // end of verbose_ session

    // I'd rather change this and select only vertices that come from
    // BX=0.  We should keep only the first vertex from all the events
    // at BX=0.
    if (v->eventId().bunchCrossing() != 0) continue;
    if (v->eventId().event() != current_event) {
      current_event = v->eventId().event();
    } else {
      continue;
    }
    // TODO(rovere) is this really necessary?
    if (fabs(v->position().z()) > 1000) continue;  // skip funny junk vertices

    // could be a new vertex, check  all primaries found so far to avoid
    // multiple entries
    simPrimaryVertex sv(v->position().x(), v->position().y(),
                        v->position().z());
    sv.eventId = v->eventId();
    sv.sim_vertex = TrackingVertexRef(tVC, std::distance(tVC->begin(), v));

    for (TrackingParticleRefVector::iterator iTrack = v->daughterTracks_begin();
         iTrack != v->daughterTracks_end(); ++iTrack) {
      // TODO(rovere) isn't it always the case? Is it really worth
      // checking this out?
      // sv.eventId = (**iTrack).eventId();
      assert((**iTrack).eventId().bunchCrossing() == 0);
    }
    // TODO(rovere) maybe get rid of this old logic completely ... ?
    simPrimaryVertex* vp = NULL;  // will become non-NULL if a vertex
                                  // is found and then point to it
    for (std::vector<simPrimaryVertex>::iterator v0 = simpv.begin();
         v0 != simpv.end(); v0++) {
      if ((sv.eventId == v0->eventId) && (fabs(sv.x - v0->x) < 1e-5) &&
          (fabs(sv.y - v0->y) < 1e-5) && (fabs(sv.z - v0->z) < 1e-5)) {
        vp = &(*v0);
        break;
      }
    }
    if (!vp) {
      // this is a new vertex, add it to the list of sim-vertices
      simpv.push_back(sv);
      vp = &simpv.back();
      if (verbose_) {
        std::cout << "this is a new vertex " << sv.eventId.event() << "   "
                  << sv.x << " " << sv.y << " " << sv.z << std::endl;
      }
    } else {
      if (verbose_) {
        std::cout << "this is not a new vertex" << sv.x << " " << sv.y << " "
                  << sv.z << std::endl;
      }
    }

    // Loop over daughter track(s) as Tracking Particles
    for (TrackingVertex::tp_iterator iTP = v->daughterTracks_begin();
         iTP != v->daughterTracks_end(); ++iTP) {
      auto momentum = (*(*iTP)).momentum();
      const reco::Track* matched_best_reco_track = nullptr;
      double match_quality = -1;
      if (use_only_charged_tracks_ && (**iTP).charge() == 0)
          continue;
      if (s2r_->find(*iTP) != s2r_->end()) {
        matched_best_reco_track = (*s2r_)[*iTP][0].first.get();
        match_quality = (*s2r_)[*iTP][0].second;
      }
      if (verbose_) {
        std::cout << "  Daughter momentum:      " << momentum;
        std::cout << "  Daughter type     " << (*(*iTP)).pdgId();
        std::cout << "  matched: " << (matched_best_reco_track != nullptr);
        std::cout << "  match-quality: " << match_quality;
        std::cout << std::endl;
      }
      vp->ptot.setPx(vp->ptot.x() + momentum.x());
      vp->ptot.setPy(vp->ptot.y() + momentum.y());
      vp->ptot.setPz(vp->ptot.z() + momentum.z());
      vp->ptot.setE(vp->ptot.e() + (**iTP).energy());
      vp->ptsq += ((**iTP).pt() * (**iTP).pt());
      // TODO(rovere) only select charged sim-particles? If so, maybe
      // put it as a configuration parameter?
      if (matched_best_reco_track) {
        vp->num_matched_reco_tracks++;
        vp->average_match_quality += match_quality;
      }
      // TODO(rovere) get rid of cuts on sim-tracks
      // TODO(rovere) be consistent between simulated tracks and
      // reconstructed tracks selection
      // count relevant particles
      if (((**iTP).pt() > 0.2) && (fabs((**iTP).eta()) < 2.5) &&
          (**iTP).charge() != 0) {
        vp->nGenTrk++;
      }
    }  // End of for loop on daughters sim-particles
    if (vp->num_matched_reco_tracks)
      vp->average_match_quality /=
          static_cast<float>(vp->num_matched_reco_tracks);
    if (verbose_) {
      std::cout << "average number of associated tracks: "
                << vp->num_matched_reco_tracks / static_cast<float>(vp->nGenTrk)
                << " with average quality: " << vp->average_match_quality
                << std::endl;
    }
  }  // End of for loop on tracking vertices

  if (verbose_) {
    std::cout << "------- PrimaryVertexAnalyzer4PUSlimmed simPVs from "
                 "TrackingVertices "
                 "-------" << std::endl;
    for (std::vector<simPrimaryVertex>::iterator v0 = simpv.begin();
         v0 != simpv.end(); v0++) {
      std::cout << "z=" << v0->z << "  event=" << v0->eventId.event()
                << std::endl;
    }
    std::cout << "-----------------------------------------------" << std::endl;
  }  // End of for summary on discovered simulated primary vertices.

  // In case of no simulated vertices, break here
  if(simpv.empty())
    return simpv;

  // Now compute the closest distance in z between all simulated vertex
  // first initialize
  auto prev_z = simpv.back().z;
  for(simPrimaryVertex& vsim: simpv) {
    vsim.closest_vertex_distance_z = std::abs(vsim.z - prev_z);
    prev_z = vsim.z;
  }
  // then calculate
  for (std::vector<simPrimaryVertex>::iterator vsim = simpv.begin();
       vsim != simpv.end(); vsim++) {
    std::vector<simPrimaryVertex>::iterator vsim2 = vsim;
    vsim2++;
    for (; vsim2 != simpv.end(); vsim2++) {
      double distance = std::abs(vsim->z - vsim2->z);
      // need both to be complete
      vsim->closest_vertex_distance_z = std::min(vsim->closest_vertex_distance_z, distance);
      vsim2->closest_vertex_distance_z = std::min(vsim2->closest_vertex_distance_z, distance);
    }
  }
  return simpv;
}

/* Extract information form recoVertex and fill the helper class
 * recoPrimaryVertex with proper reco-level information */
std::vector<PrimaryVertexAnalyzer4PUSlimmed::recoPrimaryVertex>
PrimaryVertexAnalyzer4PUSlimmed::getRecoPVs(
    const edm::Handle<edm::View<reco::Vertex>>& tVC) {
  std::vector<PrimaryVertexAnalyzer4PUSlimmed::recoPrimaryVertex> recopv;

  if (verbose_) {
    std::cout << "getRecoPVs TrackingVertexCollection " << std::endl;
  }

  for (auto v = tVC->begin(); v != tVC->end(); ++v) {
    if (verbose_) {
      std::cout << " Position: " << v->position() << std::endl;
    }

    // Skip junk vertices
    if (fabs(v->z()) > 1000) continue;
    if (v->isFake() || !v->isValid()) continue;

    recoPrimaryVertex sv(v->position().x(), v->position().y(),
                         v->position().z());
    sv.recVtx = &(*v);
    sv.recVtxRef = reco::VertexBaseRef(tVC, std::distance(tVC->begin(), v));
    // this is a new vertex, add it to the list of reco-vertices
    recopv.push_back(sv);
    PrimaryVertexAnalyzer4PUSlimmed::recoPrimaryVertex* vp = &recopv.back();

    // Loop over daughter track(s)
    for (auto iTrack = v->tracks_begin(); iTrack != v->tracks_end(); ++iTrack) {
      auto momentum = (*(*iTrack)).innerMomentum();
      // TODO(rovere) better handle the pixelVertices, whose tracks
      // do not have the innerMomentum defined. This is a temporary
      // hack to overcome this problem.
      if (momentum.mag2() == 0)
        momentum = (*(*iTrack)).momentum();
      if (verbose_) {
        std::cout << "  Daughter momentum:      " << momentum;
        std::cout << std::endl;
      }
      vp->ptsq += (momentum.perp2());
      vp->nRecoTrk++;

      auto matched = r2s_->find(*iTrack);
      if(matched != r2s_->end()) {
        vp->num_matched_sim_tracks++;
      }

    }  // End of for loop on daughters reconstructed tracks
  }    // End of for loop on tracking vertices

  if (verbose_) {
    std::cout << "------- PrimaryVertexAnalyzer4PUSlimmed recoPVs from "
                 "VertexCollection "
                 "-------" << std::endl;
    for (std::vector<recoPrimaryVertex>::iterator v0 = recopv.begin();
         v0 != recopv.end(); v0++) {
      std::cout << "z=" << v0->z << std::endl;
    }
    std::cout << "-----------------------------------------------" << std::endl;
  }  // End of for summary on reconstructed primary vertices.

  // In case of no reco vertices, break here
  if(recopv.empty())
    return recopv;

  // Now compute the closest distance in z between all reconstructed vertex
  // first initialize
  auto prev_z = recopv.back().z;
  for(recoPrimaryVertex& vreco: recopv) {
    vreco.closest_vertex_distance_z = std::abs(vreco.z - prev_z);
    prev_z = vreco.z;
  }
  for (std::vector<recoPrimaryVertex>::iterator vreco = recopv.begin();
       vreco != recopv.end(); vreco++) {
    std::vector<recoPrimaryVertex>::iterator vreco2 = vreco;
    vreco2++;
    for (; vreco2 != recopv.end(); vreco2++) {
      double distance = std::abs(vreco->z - vreco2->z);
      // need both to be complete
      vreco->closest_vertex_distance_z = std::min(vreco->closest_vertex_distance_z, distance);
      vreco2->closest_vertex_distance_z = std::min(vreco2->closest_vertex_distance_z, distance);
    }
  }
  return recopv;
}

void PrimaryVertexAnalyzer4PUSlimmed::resetSimPVAssociation(
    std::vector<simPrimaryVertex> & simpv) {
  for (auto & v : simpv) {
    v.rec_vertices.clear();
  }
}

// ------------ method called to produce the data  ------------
void PrimaryVertexAnalyzer4PUSlimmed::matchSim2RecoVertices(
    std::vector<simPrimaryVertex>& simpv,
    const reco::VertexSimToRecoCollection& vertex_s2r) {
  if (verbose_) {
    std::cout << "PrimaryVertexAnalyzer4PUSlimmed::matchSim2RecoVertices " << std::endl;
  }
  for (std::vector<simPrimaryVertex>::iterator vsim = simpv.begin();
       vsim != simpv.end(); vsim++) {

    auto matched = vertex_s2r.find(vsim->sim_vertex);
    if(matched != vertex_s2r.end()) {
      for(const auto vertexRefQuality: matched->val) {
        vsim->rec_vertices.push_back(&(*(vertexRefQuality.first)));
      }
    }

    if (verbose_) {
      if (vsim->rec_vertices.size()) {
        for (auto const& v : vsim->rec_vertices) {
          std::cout << "Found a matching vertex for genVtx "
                    << vsim->z << " at " << v->z()
                    << " with sign: " << fabs(v->z() - vsim->z) / v->zError()
                    << std::endl;
        }
      } else {
        std::cout << "No matching vertex for " << vsim->z << std::endl;
      }
    }
  }  // end for loop on simulated vertices
  if (verbose_) {
    std::cout << "Done with matching sim vertices" << std::endl;
  }
}

void PrimaryVertexAnalyzer4PUSlimmed::matchReco2SimVertices(
    std::vector<recoPrimaryVertex>& recopv,
    const reco::VertexRecoToSimCollection& vertex_r2s,
    const std::vector<simPrimaryVertex>& simpv) {
  for (std::vector<recoPrimaryVertex>::iterator vrec = recopv.begin();
       vrec != recopv.end(); vrec++) {

    auto matched = vertex_r2s.find(vrec->recVtxRef);
    if(matched != vertex_r2s.end()) {
      for(const auto vertexRefQuality: matched->val) {
        const auto tvPtr = &(*(vertexRefQuality.first));
        vrec->sim_vertices.push_back(tvPtr);
      }

      for(const TrackingVertex *tv: vrec->sim_vertices) {
        // Set pointers to internal simVertex objects
        for(const auto& vv: simpv) {
          if (&(*(vv.sim_vertex)) == tv) {
            vrec->sim_vertices_internal.push_back(&vv);
            continue;
          }
        }

        // Calculate number of shared tracks
        vrec->sim_vertices_num_shared_tracks.push_back(calculateVertexSharedTracks(*(vrec->recVtx), *tv, *r2s_));
      }
    }

    if (verbose_) {
      for (auto v : vrec->sim_vertices) {
        std::cout << "Found a matching vertex for reco: " << vrec->z
                  << " at gen:" << v->position().z() << " with sign: "
                  << fabs(vrec->z - v->position().z()) / vrec->recVtx->zError()
                  << std::endl;
      }
    }
  }  // end for loop on reconstructed vertices
}

void PrimaryVertexAnalyzer4PUSlimmed::analyze(const edm::Event& iEvent,
                                              const edm::EventSetup& iSetup) {
  using std::vector;
  using std::cout;
  using std::endl;
  using edm::Handle;
  using edm::View;
  using edm::LogInfo;
  using namespace reco;

  std::vector<float> pileUpInfo_z;

  // get the pileup information
  edm::Handle<std::vector<PileupSummaryInfo> > puinfoH;
  if (iEvent.getByToken(vecPileupSummaryInfoToken_, puinfoH)) {
    for (auto const& pu_info : *puinfoH.product()) {
      mes_["root_folder"]["GenVtx_vs_BX"]
          ->Fill(pu_info.getBunchCrossing(), pu_info.getPU_NumInteractions());
      if (pu_info.getBunchCrossing() == 0) {
        pileUpInfo_z = pu_info.getPU_zpositions();
        if (verbose_) {
          for (auto const& p : pileUpInfo_z) {
            std::cout << "PileUpInfo on Z vertex: " << p << std::endl;
          }
        }
        break;
      }
    }
  }

  Handle<reco::TrackCollection> recTrks;
  iEvent.getByToken(recoTrackCollectionToken_, recTrks);

  // for the associator
  Handle<View<Track> > trackCollectionH;
  iEvent.getByToken(edmView_recoTrack_Token_, trackCollectionH);

  edm::Handle<TrackingParticleCollection> TPCollectionH;
  iEvent.getByToken(trackingParticleCollectionToken_, TPCollectionH);

  edm::Handle<TrackingVertexCollection> TVCollectionH;
  iEvent.getByToken(trackingVertexCollectionToken_, TVCollectionH);

  // TODO(rovere) the idea is to put in case a track-selector in front
  // of this module and then use its label to get the selected tracks
  // out of the event instead of making an hard-coded selection in the
  // code.

  edm::Handle<reco::SimToRecoCollection> simToRecoH;
  iEvent.getByToken(simToRecoAssociationToken_, simToRecoH);

  edm::Handle<reco::RecoToSimCollection> recoToSimH;
  iEvent.getByToken(recoToSimAssociationToken_, recoToSimH);

  s2r_ = simToRecoH.product();
  r2s_ = recoToSimH.product();

  // Vertex associator
  edm::Handle<reco::VertexToTrackingVertexAssociator> vertexAssociatorH;
  iEvent.getByToken(vertexAssociatorToken_, vertexAssociatorH);
  const reco::VertexToTrackingVertexAssociator& vertexAssociator = *(vertexAssociatorH.product());

  std::vector<simPrimaryVertex> simpv;  // a list of simulated primary
                                        // MC vertices
  // TODO(rovere) use move semantic?
  simpv = getSimPVs(TVCollectionH);
  // TODO(rovere) 1 vertex is not, by definition, pileup, and should
  // probably be subtracted?
  int kind_of_signal_vertex = 0;
  int num_pileup_vertices = simpv.size();
  mes_["root_folder"]["GenAllV_NumVertices"]->Fill(simpv.size());
  bool signal_is_highest_pt = std::max_element(simpv.begin(), simpv.end(),
                                               [](const simPrimaryVertex& lhs,
                                                  const simPrimaryVertex& rhs) {
                                                 return lhs.ptsq < rhs.ptsq;
                                               }) == simpv.begin();
  kind_of_signal_vertex |= (signal_is_highest_pt << HIGHEST_PT);
  mes_["root_folder"]["SignalIsHighestPt2"]->Fill(
      signal_is_highest_pt ? 1. : 0.);
  computePairDistance(simpv,
                      mes_["root_folder"]["GenAllV_PairDistanceZ"]);

  int label_index = -1;
  for (auto const& vertex_token : reco_vertex_collection_tokens_) {
    std::vector<recoPrimaryVertex> recopv;  // a list of reconstructed
                                            // primary MC vertices
    std::string label = reco_vertex_collections_[++label_index].label();
    edm::Handle<edm::View<reco::Vertex>> recVtxs;
    if (!iEvent.getByToken(vertex_token, recVtxs)) {
      LogInfo("PrimaryVertexAnalyzer4PUSlimmed")
          << "Skipping vertex collection: " << label << " since it is missing."
          << std::endl;
      continue;
    }

    reco::VertexRecoToSimCollection vertex_r2s = vertexAssociator.associateRecoToSim(recVtxs, TVCollectionH);
    reco::VertexSimToRecoCollection vertex_s2r = vertexAssociator.associateSimToReco(recVtxs, TVCollectionH);

    resetSimPVAssociation(simpv);
    matchSim2RecoVertices(simpv, vertex_s2r);
    recopv = getRecoPVs(recVtxs);
    computePairDistance(recopv,
                        mes_[label]["RecoAllAssoc2Gen_PairDistanceZ"]);
    matchReco2SimVertices(recopv, vertex_r2s, simpv);

    int num_total_gen_vertices_assoc2reco = 0;
    int num_total_reco_vertices_assoc2gen = 0;
    int num_total_gen_vertices_multiassoc2reco = 0;
    int num_total_reco_vertices_multiassoc2gen = 0;
    int num_total_reco_vertices_duplicate = 0;
    int genpv_position_in_reco_collection = -1;
    for (auto const& v : simpv) {
      float mistag = 1.;
      // TODO(rovere) put selectors here in front of fill* methods.
      if (v.eventId.event() == 0) {
        if (!recVtxs->empty() &&
            std::find(v.rec_vertices.begin(), v.rec_vertices.end(),
                      &((*recVtxs.product())[0])) != v.rec_vertices.end()) {
          mistag = 0.;
          kind_of_signal_vertex |= (1 << IS_ASSOC2FIRST_RECO);
        } else {
          if (v.rec_vertices.size()) {
            kind_of_signal_vertex |= (1 << IS_ASSOC2ANY_RECO);
          }
        }
        mes_[label]["KindOfSignalPV"]->Fill(kind_of_signal_vertex);
        mes_[label]["MisTagRate"]->Fill(mistag);
        mes_[label]["MisTagRate_vs_PU"]->Fill(simpv.size(), mistag);
        mes_[label]["MisTagRate_vs_sum-pt2"]->Fill(v.ptsq, mistag);
        mes_[label]["MisTagRate_vs_Z"]->Fill(v.z, mistag);
        mes_[label]["MisTagRate_vs_R"]->Fill(v.r, mistag);
        mes_[label]["MisTagRate_vs_NumTracks"]->Fill(v.nGenTrk, mistag);
        if (signal_is_highest_pt) {
          mes_[label]["MisTagRateSignalIsHighest"]->Fill(mistag);
          mes_[label]["MisTagRateSignalIsHighest_vs_PU"]->Fill(simpv.size(),
                                                               mistag);
          mes_[label]["MisTagRateSignalIsHighest_vs_sum-pt2"]->Fill(v.ptsq,
                                                                    mistag);
          mes_[label]["MisTagRateSignalIsHighest_vs_Z"]->Fill(v.z, mistag);
          mes_[label]["MisTagRateSignalIsHighest_vs_R"]->Fill(v.r, mistag);
          mes_[label]["MisTagRateSignalIsHighest_vs_NumTracks"]->Fill(v.nGenTrk,
                                                                      mistag);
        } else {
          mes_[label]["MisTagRateSignalIsNotHighest"]->Fill(mistag);
          mes_[label]["MisTagRateSignalIsNotHighest_vs_PU"]->Fill(simpv.size(),
                                                                  mistag);
          mes_[label]["MisTagRateSignalIsNotHighest_vs_sum-pt2"]->Fill(v.ptsq,
                                                                       mistag);
          mes_[label]["MisTagRateSignalIsNotHighest_vs_Z"]->Fill(v.z, mistag);
          mes_[label]["MisTagRateSignalIsNotHighest_vs_R"]->Fill(v.r, mistag);
          mes_[label]["MisTagRateSignalIsNotHighest_vs_NumTracks"]->
              Fill(v.nGenTrk, mistag);
        }
        // Now check at which location the Simulated PV has been
        // reconstructed in the primary vertex collection
        // at-hand. Mark it with fake index -1 if it was not
        // reconstructed at all.

        auto iv = (*recVtxs.product()).begin();
        for (int pv_position_in_reco_collection = 0;
             iv != (*recVtxs.product()).end();
             ++pv_position_in_reco_collection, ++iv) {
          if (std::find(v.rec_vertices.begin(), v.rec_vertices.end(),
                        &(*iv)) != v.rec_vertices.end()) {
            mes_[label]["TruePVLocationIndex"]
                ->Fill(pv_position_in_reco_collection);
            mes_[label]["TruePVLocationIndexCumulative"]
                ->Fill(pv_position_in_reco_collection > 0 ? 1 : 0);

            if (signal_is_highest_pt) {
              mes_[label]["TruePVLocationIndexSignalIsHighest"]
                ->Fill(pv_position_in_reco_collection);
            } else {
              mes_[label]["TruePVLocationIndexSignalIsNotHighest"]
                ->Fill(pv_position_in_reco_collection);
            }

            genpv_position_in_reco_collection = pv_position_in_reco_collection;
            break;
          }
        }

        // If we reached the end, it means that the Simulated PV has not
        // been associated to any reconstructed vertex: mark it as
        // missing in the reconstructed vertex collection using the fake
        // index -1.
        if (iv == (*recVtxs.product()).end()) {
          mes_[label]["TruePVLocationIndex"]->Fill(-1.);
          mes_[label]["TruePVLocationIndexCumulative"]->Fill(-1.);
          if (signal_is_highest_pt)
            mes_[label]["TruePVLocationIndexSignalIsHighest"]->Fill(-1.);
          else
            mes_[label]["TruePVLocationIndexSignalIsNotHighest"]->Fill(-1.);
        }
      }

      if (v.rec_vertices.size()) num_total_gen_vertices_assoc2reco++;
      if (v.rec_vertices.size() > 1) num_total_gen_vertices_multiassoc2reco++;
      // No need to N-tplicate the Gen-related cumulative histograms:
      // fill them only at the first iteration
      if (label_index == 0) fillGenericGenVertexHistograms(v);
      fillRecoAssociatedGenVertexHistograms(label, v);
    }
    calculatePurityAndFillHistograms(label, recopv, genpv_position_in_reco_collection, signal_is_highest_pt);

    mes_[label]["GenAllAssoc2Reco_NumVertices"]
        ->Fill(simpv.size(), simpv.size());
    mes_[label]["GenAllAssoc2RecoMatched_NumVertices"]
        ->Fill(simpv.size(), num_total_gen_vertices_assoc2reco);
    mes_[label]["GenAllAssoc2RecoMultiMatched_NumVertices"]
        ->Fill(simpv.size(), num_total_gen_vertices_multiassoc2reco);
    for (auto & v : recopv) {
      fillGenAssociatedRecoVertexHistograms(label, num_pileup_vertices, v);
      if (v.sim_vertices.size()) {
        num_total_reco_vertices_assoc2gen++;
        if (v.sim_vertices_internal[0]->rec_vertices.size() > 1) {
          num_total_reco_vertices_duplicate++;
        }
      }
      if (v.sim_vertices.size() > 1) num_total_reco_vertices_multiassoc2gen++;
    }
    mes_[label]["RecoAllAssoc2Gen_NumVertices"]
        ->Fill(recopv.size(), recopv.size());
    mes_[label]["RecoAllAssoc2GenMatched_NumVertices"]
        ->Fill(recopv.size(), num_total_reco_vertices_assoc2gen);
    mes_[label]["RecoAllAssoc2GenMultiMatched_NumVertices"]
        ->Fill(recopv.size(), num_total_reco_vertices_multiassoc2gen);
    mes_[label]["RecoAllAssoc2MultiMatchedGen_NumVertices"]
        ->Fill(recopv.size(), num_total_reco_vertices_duplicate);
    mes_[label]["RecoVtx_vs_GenVtx"]->Fill(simpv.size(), recopv.size());
    mes_[label]["MatchedRecoVtx_vs_GenVtx"]
        ->Fill(simpv.size(), num_total_reco_vertices_assoc2gen);

  }
}  // end of analyze

template<class T>
void PrimaryVertexAnalyzer4PUSlimmed::computePairDistance(const T &collection,
                                                          MonitorElement *me) {
  for (unsigned int i = 0; i < collection.size(); ++i) {
    for (unsigned int j = i+1; j < collection.size(); ++j) {
      me->Fill(
          std::abs(collection[i].z-collection[j].z));
    }
  }
}
