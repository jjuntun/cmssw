#ifndef SimTracker_VertexAssociation_VertexAssociatorByPositionAndTracks_h
#define SimTracker_VertexAssociation_VertexAssociatorByPositionAndTracks_h

#include "SimDataFormats/Associations/interface/VertexToTrackingVertexAssociatorBaseImpl.h"
#include "DataFormats/RecoCandidate/interface/TrackAssociation.h"

/**
 * This class associates reco::Vertices and TrackingVertices by their
 * position (maximum distance in Z should be smaller than absZ and
 * sigmaZ*zError of reco::Vertex), and (optionally) by the fraction of
 * tracks shared by reco::Vertex and TrackingVertex divided by the
 * number of tracks in reco::Vertex. This fraction is always used as
 * the quality in the association, i.e. multiple associations are
 * sorted by it in descending order.
 */

/*
namespace vertexAssociation {
  enum class AssociationType { NumberOfTracks, W, Pt, WPt, Pt2, WPt2, HarmPt, WHarmPt, HarmWPt, HarmPtAvg, WHarmPtAvg, HarmWPtAvg };
  
  enum class DividerType { Rreco, RrecoMatched, Sim, SimMatched };

}
*/

namespace vertexAssociation {
  enum class AssociationType;
}

namespace vertexAssociation {
  enum class AssociationType { NumberOfTracks, W, Pt, WPt, Pt2, WPt2, HarmPt, WHarmPt, HarmWPt, HarmPtAvg, WHarmPtAvg, HarmWPtAvg };
  
  enum class DividerType { Reco, RecoMatched, Sim, SimMatched };

  AssociationType getAssociationType(std::string s)
  {
    if(s.compare("") == 0)
      return AssociationType::NumberOfTracks;
    else if(s.compare("W") == 0)
      return AssociationType::W;
    else if(s.compare("Pt") == 0)
      return AssociationType::Pt;
    else if(s.compare("WPt") == 0)
      return AssociationType::WPt;
    else if(s.compare("Pt2") == 0)
      return AssociationType::Pt2;
    else if(s.compare("WPt2") == 0)
      return AssociationType::WPt2;
    else if(s.compare("HarmPt") == 0)
      return AssociationType::HarmPt;
    else if(s.compare("WHarmPt") == 0)
      return AssociationType::WHarmPt;
    else if(s.compare("HarmWPt") == 0)
      return AssociationType::HarmWPt;
    else if(s.compare("HarmPtAvg") == 0)
      return AssociationType::HarmPtAvg;
    else if(s.compare("WHarmPtAvg") == 0)
      return AssociationType::WHarmPtAvg;
    else if(s.compare("HarmWPtAvg") == 0)
      return AssociationType::HarmWPtAvg;

    return AssociationType::NumberOfTracks;
  }

}

class VertexAssociatorByPositionAndTracks : public reco::VertexToTrackingVertexAssociatorBaseImpl {
public:
  VertexAssociatorByPositionAndTracks(const edm::EDProductGetter *productGetter,
                                      double absZ,
                                      double sigmaZ,
                                      double maxRecoZ,
                                      double sharedTrackFraction,
                                      const reco::RecoToSimCollection *trackRecoToSimAssociation,
                                      const reco::SimToRecoCollection *trackSimToRecoAssociation,
																			const std::string& associationMode);

  virtual ~VertexAssociatorByPositionAndTracks();

  /* Associate TrackingVertex to RecoVertex By Hits */
  virtual reco::VertexRecoToSimCollection associateRecoToSim(const edm::Handle<edm::View<reco::Vertex> >& vCH, 
                                                             const edm::Handle<TrackingVertexCollection>& tVCH) const;

  virtual reco::VertexSimToRecoCollection associateSimToReco(const edm::Handle<edm::View<reco::Vertex> >& vCH, 
                                                             const edm::Handle<TrackingVertexCollection>& tVCH) const;

private:
  // ----- member data
  const edm::EDProductGetter * productGetter_;

  const double absZ_;
  const double sigmaZ_;
  const double maxRecoZ_;
  const double sharedTrackFraction_;

  const reco::RecoToSimCollection *trackRecoToSimAssociation_;
  const reco::SimToRecoCollection *trackSimToRecoAssociation_;

  vertexAssociation::AssociationType associationType_;


};

#endif
