#ifndef SimTracker_VertexAssociation_calculateVertexSharedTracks_h
#define SimTracker_VertexAssociation_calculateVertexSharedTracks_h

#include "DataFormats/RecoCandidate/interface/TrackAssociation.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "SimDataFormats/TrackingAnalysis/interface/TrackingVertex.h"

namespace vertexAssociation {
  enum class AssociationTypeStringToEnum { NumberOfTracks = 0, Weighted, Pt, WPt, Pt2, WPt2, HarmPt, WHarmPt, HarmWPt, HarmPtAvg, WHarmPtAvg, HarmWPtAvg, Size };
  
  enum class DividerType { Reco, RecoMatched, Sim, SimMatched };

  AssociationTypeStringToEnum getAssociationTypeStringToEnum(const std::string& s);

  struct TrackFraction  {
    std::array<double, static_cast<int>(AssociationTypeStringToEnum::Size)> fractions;
    double getFraction(vertexAssociation::AssociationTypeStringToEnum assoc)
    {
      int index = static_cast<int>(assoc);
      return fractions[index];
    }
  };

}

//unsigned int calculateVertexSharedTrackFractions(const reco::Vertex& recoV, const TrackingVertex& simV, const reco::RecoToSimCollection& trackRecoToSimAssociation, vertexAssociation::DividerType);

vertexAssociation::TrackFraction calculateVertexSharedTrackFractions(const reco::Vertex& recoV, const TrackingVertex& simV, const reco::SimToRecoCollection& trackSimToRecoAssociation, const reco::RecoToSimCollection& trackRecoToSimAssociation, vertexAssociation::DividerType);

vertexAssociation::TrackFraction calculateVertexSharedTrackFractions(const TrackingVertex& simV, const reco::Vertex& recoV, const reco::SimToRecoCollection& trackSimToRecoAssociation, const reco::RecoToSimCollection& trackRecoToSimAssociation, vertexAssociation::DividerType);


/*
unsigned int calculateVertexSharedTracks(const reco::Vertex& recoV, const TrackingVertex& simV, const reco::RecoToSimCollection& trackRecoToSimAssociation);
unsigned int calculateVertexSharedTracks(const TrackingVertex& simV, const reco::Vertex& recoV, const reco::SimToRecoCollection& trackSimToRecoAssociation);

std::vector<double> calculateVertexSharedTracksMomentumFractionNumerators(const reco::Vertex& recoV, const TrackingVertex& simV,  const reco::SimToRecoCollection& trackSimToRecoAssociation, const reco::RecoToSimCollection& trackRecoToSimAssociation);
std::vector<std::vector<double>> calculateVertexSharedTracksMomentumFractionDenominators(const reco::Vertex& recoV, const TrackingVertex& simV,  const reco::SimToRecoCollection& trackSimToRecoAssociation, const reco::RecoToSimCollection& trackRecoToSimAssociation);
std::vector<double> calculateVertexSharedTracksMomentumFractionNumerators(const TrackingVertex& simV, const reco::Vertex& recoV, const reco::SimToRecoCollection& trackSimToRecoAssociation, const reco::RecoToSimCollection& trackRecoToSimAssociation);
std::vector<std::vector<double>> calculateVertexSharedTracksMomentumFractionDenominators(const TrackingVertex& simV, const reco::Vertex& recoV, const reco::SimToRecoCollection& trackSimToRecoAssociation, const reco::RecoToSimCollection& trackRecoToSimAssociation);
*/
#endif

