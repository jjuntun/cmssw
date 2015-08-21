#ifndef SimTracker_VertexAssociation_calculateVertexSharedTracks_h
#define SimTracker_VertexAssociation_calculateVertexSharedTracks_h

#include "DataFormats/RecoCandidate/interface/TrackAssociation.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "SimDataFormats/TrackingAnalysis/interface/TrackingVertex.h"

namespace vertexAssociation {
  enum class AssociationType { NumberOfTracks = 0, Weighted, Pt, WPt, Pt2, WPt2, HarmPt, WHarmPt, HarmWPt, HarmPtAvg, WHarmPtAvg, HarmWPtAvg, Size };
  
  enum class DividerType { Reco = 0, RecoMatched, Sim, SimMatched, Size };

  AssociationType associationTypeStringToEnum(const std::string& s);
  std::string associationTypeEnumToString(const AssociationType& s);

  struct TrackFraction  {
    std::vector<double> numerators = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    std::vector<double> denominators = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    std::array<double, static_cast<int>(AssociationType::Size)> fractions;
    double getFraction(vertexAssociation::AssociationType assoc)
    {
      int index = static_cast<int>(assoc);
      return fractions[index];
    }

    void calculateNumerators(const reco::Vertex& recoV, const TrackingVertex& simV, const reco::RecoToSimCollection& trackRecoToSimAssociation);

    void calculateNumerators(const TrackingVertex& simV, const reco::Vertex& recoV, const reco::SimToRecoCollection& trackSimToRecoAssociation);

    void calculateRecoDenominators(const reco::Vertex& recoV);

    void calculateRecoMatchedDenominators(const reco::Vertex& recoV,  const reco::RecoToSimCollection& trackRecoToSimAssociation);

    void calculateSimDenominators(const TrackingVertex& simV);

    void calculateSimMatchedDenominators(const TrackingVertex& simV,  const reco::SimToRecoCollection& trackSimToRecoAssociation);

    void calculateFractions(DividerType dividerType);

  };

}

vertexAssociation::TrackFraction calculateVertexSharedTrackFractions(const reco::Vertex& recoV, const TrackingVertex& simV, const reco::SimToRecoCollection& trackSimToRecoAssociation, const reco::RecoToSimCollection& trackRecoToSimAssociation, vertexAssociation::DividerType);

vertexAssociation::TrackFraction calculateVertexSharedTrackFractions(const TrackingVertex& simV, const reco::Vertex& recoV, const reco::SimToRecoCollection& trackSimToRecoAssociation, const reco::RecoToSimCollection& trackRecoToSimAssociation, vertexAssociation::DividerType);

#endif

