#include "SimTracker/VertexAssociation/interface/VertexAssociatorByPositionAndTracks.h"
#include "SimTracker/VertexAssociation/interface/calculateVertexSharedTracks.h"

#include "FWCore/MessageLogger/interface/MessageLogger.h"



VertexAssociatorByPositionAndTracks::VertexAssociatorByPositionAndTracks(const edm::EDProductGetter *productGetter,
                                                                         double absZ,
                                                                         double sigmaZ,
                                                                         double maxRecoZ,
                                                                         double sharedFraction,
                                                                         const reco::RecoToSimCollection *trackRecoToSimAssociation,
                                                                         const reco::SimToRecoCollection *trackSimToRecoAssociation,
                                                                         const vertexAssociation::AssociationTypeStringToEnum associationType,
const vertexAssociation::DividerType dividerType):
  productGetter_(productGetter),
  absZ_(absZ),
  sigmaZ_(sigmaZ),
  maxRecoZ_(maxRecoZ),
  sharedFraction_(sharedFraction),
  trackRecoToSimAssociation_(trackRecoToSimAssociation),
  trackSimToRecoAssociation_(trackSimToRecoAssociation),
  associationType_(associationType),
  dividerType_(dividerType)
{}

VertexAssociatorByPositionAndTracks::~VertexAssociatorByPositionAndTracks() {}

reco::VertexRecoToSimCollection VertexAssociatorByPositionAndTracks::associateRecoToSim(const edm::Handle<edm::View<reco::Vertex> >& vCH, 
                                                                                        const edm::Handle<TrackingVertexCollection>& tVCH) const {
  reco::VertexRecoToSimCollection ret(productGetter_);

  //textual representation of associationType_ for LogTrace
  std::string associationTypeText;
  switch(associationType_)
  {
    case vertexAssociation::AssociationTypeStringToEnum::NumberOfTracks:
      associationTypeText = "NumberOfTracks";
    case vertexAssociation::AssociationTypeStringToEnum::Weighted:
      associationTypeText = "Weighted";
    case vertexAssociation::AssociationTypeStringToEnum::Pt:
      associationTypeText = "Pt";
    case vertexAssociation::AssociationTypeStringToEnum::WPt:
      associationTypeText = "WPt";
    case vertexAssociation::AssociationTypeStringToEnum::Pt2:
      associationTypeText = "Pt2";
    case vertexAssociation::AssociationTypeStringToEnum::WPt2:
      associationTypeText = "WPt2";
    case vertexAssociation::AssociationTypeStringToEnum::HarmPt:
      associationTypeText = "HarmPt";
    case vertexAssociation::AssociationTypeStringToEnum::WHarmPt:
      associationTypeText = "WHarmPt";
    case vertexAssociation::AssociationTypeStringToEnum::HarmWPt:
      associationTypeText = "HarmWPt";
    case vertexAssociation::AssociationTypeStringToEnum::HarmPtAvg:
      associationTypeText = "HarmPtAvg";
    case vertexAssociation::AssociationTypeStringToEnum::WHarmPtAvg:
      associationTypeText = "WHarmPtAvg";
    case vertexAssociation::AssociationTypeStringToEnum::HarmWPtAvg:
      associationTypeText = "HarmWPtAvg";
    default:
      associationTypeText = "NumberOfTracks";
  }

  //textual representation of dividerType_ for LogTrace
  std::string dividerTypeText;
  switch(dividerType_)
  {
    case vertexAssociation::DividerType::Reco:
      dividerTypeText = "Reco";
    case vertexAssociation::DividerType::RecoMatched:
      dividerTypeText = "RecoMatched";
    case vertexAssociation::DividerType::Sim:
      dividerTypeText = "Sim";
    case vertexAssociation::DividerType::SimMatched:
      dividerTypeText = "SimMatched";
  }

  const edm::View<reco::Vertex>& recoVertices = *vCH;
  const TrackingVertexCollection& simVertices = *tVCH;

  LogDebug("VertexAssociation") << "VertexAssociatorByPositionAndTracks::associateRecoToSim(): associating "
                                << recoVertices.size() << " reco::Vertices to" << simVertices.size() << " TrackingVertices";

  for(size_t iReco=0; iReco != recoVertices.size(); ++iReco) {
    const reco::Vertex& recoVertex = recoVertices[iReco];

    // skip fake vertices
    if(std::abs(recoVertex.z()) > maxRecoZ_ || recoVertex.isFake() || !recoVertex.isValid() || recoVertex.ndof() < 0.)
      continue;

    LogTrace("VertexAssociation") << " reco::Vertex at Z " << recoVertex.z();

    int current_event = -1;
    for(size_t iSim=0; iSim != simVertices.size(); ++iSim) {
      const TrackingVertex& simVertex = simVertices[iSim];

      // Associate only to primary vertices of the in-time pileup
      // events (BX=0, first vertex in each of the events)
      if(simVertex.eventId().bunchCrossing() != 0) continue;
      if(simVertex.eventId().event() != current_event) {
        current_event = simVertex.eventId().event();
      }
      else {
        continue;
      }

      LogTrace("VertexAssociation") << "  Considering TrackingVertex at Z " << simVertex.position().z();

      const double zdiff = std::abs(recoVertex.z() - simVertex.position().z());
      if(zdiff < absZ_ && zdiff / recoVertex.zError() < sigmaZ_) {

        vertexAssociation::TrackFraction tf = calculateVertexSharedTrackFractions(recoVertex, simVertex, *trackSimToRecoAssociation_, *trackRecoToSimAssociation_, dividerType_);

        auto fraction = tf.getFraction(associationType_);


        if(sharedFraction_ < 0 || fraction > sharedFraction_) {
          LogTrace("VertexAssociation") << "   Matched with significance " << zdiff/recoVertex.zError()
                                        << " numerator Reco " << associationTypeText
                                        << " denominator " << dividerTypeText
                                        << " shared fraction " << fraction << " reco Tracks " << recoVertex.tracksSize() << " TrackingParticles " << simVertex.nDaughterTracks();

          ret.insert(reco::VertexBaseRef(vCH, iReco), std::make_pair(TrackingVertexRef(tVCH, iSim), fraction));
        }
     
      }
    }
  }

  ret.post_insert();

  LogDebug("VertexAssociation") << "VertexAssociatorByPositionAndTracks::associateRecoToSim(): finished";

  return ret;
}

reco::VertexSimToRecoCollection VertexAssociatorByPositionAndTracks::associateSimToReco(const edm::Handle<edm::View<reco::Vertex> >& vCH, 
                                                                                        const edm::Handle<TrackingVertexCollection>& tVCH) const {
  reco::VertexSimToRecoCollection ret(productGetter_);

  //textual representation of associationType_ for LogTrace
  std::string associationTypeText;
  switch(associationType_)
  {
    case vertexAssociation::AssociationTypeStringToEnum::NumberOfTracks:
      associationTypeText = "NumberOfTracks";
    case vertexAssociation::AssociationTypeStringToEnum::Weighted:
      associationTypeText = "Weighted";
    case vertexAssociation::AssociationTypeStringToEnum::Pt:
      associationTypeText = "Pt";
    case vertexAssociation::AssociationTypeStringToEnum::WPt:
      associationTypeText = "WPt";
    case vertexAssociation::AssociationTypeStringToEnum::Pt2:
      associationTypeText = "Pt2";
    case vertexAssociation::AssociationTypeStringToEnum::WPt2:
      associationTypeText = "WPt2";
    case vertexAssociation::AssociationTypeStringToEnum::HarmPt:
      associationTypeText = "HarmPt";
    case vertexAssociation::AssociationTypeStringToEnum::WHarmPt:
      associationTypeText = "WHarmPt";
    case vertexAssociation::AssociationTypeStringToEnum::HarmWPt:
      associationTypeText = "HarmWPt";
    case vertexAssociation::AssociationTypeStringToEnum::HarmPtAvg:
      associationTypeText = "HarmPtAvg";
    case vertexAssociation::AssociationTypeStringToEnum::WHarmPtAvg:
      associationTypeText = "WHarmPtAvg";
    case vertexAssociation::AssociationTypeStringToEnum::HarmWPtAvg:
      associationTypeText = "HarmWPtAvg";
    default:
      associationTypeText = "NumberOfTracks";
  }

  //textual representation of dividerType_ for LogTrace
  std::string dividerTypeText;
  switch(dividerType_)
  {
    case vertexAssociation::DividerType::Reco:
      dividerTypeText = "Reco";
    case vertexAssociation::DividerType::RecoMatched:
      dividerTypeText = "RecoMatched";
    case vertexAssociation::DividerType::Sim:
      dividerTypeText = "Sim";
    case vertexAssociation::DividerType::SimMatched:
      dividerTypeText = "SimMatched";
  }


  const edm::View<reco::Vertex>& recoVertices = *vCH;
  const TrackingVertexCollection& simVertices = *tVCH;

  LogDebug("VertexAssociation") << "VertexAssociatorByPositionAndTracks::associateSimToReco(): associating "
                                << simVertices.size() << " TrackingVertices to " << recoVertices.size() << " reco::Vertices";

  int current_event = -1;
  for(size_t iSim=0; iSim != simVertices.size(); ++iSim) {
    const TrackingVertex& simVertex = simVertices[iSim];

    // Associate only primary vertices of the in-time pileup
    // events (BX=0, first vertex in each of the events)
    if(simVertex.eventId().bunchCrossing() != 0) continue;
    if(simVertex.eventId().event() != current_event) {
      current_event = simVertex.eventId().event();
    }
    else {
      continue;
    }

    LogTrace("VertexAssociation") << " TrackingVertex at Z " << simVertex.position().z();

    for(size_t iReco=0; iReco != recoVertices.size(); ++iReco) {
      const reco::Vertex& recoVertex = recoVertices[iReco];

      // skip fake vertices
      if(std::abs(recoVertex.z()) > maxRecoZ_ || recoVertex.isFake() || !recoVertex.isValid() || recoVertex.ndof() < 0.)
        continue;

      LogTrace("VertexAssociation") << "  Considering reco::Vertex at Z " << recoVertex.z();

      const double zdiff = std::abs(recoVertex.z() - simVertex.position().z());
      if(zdiff < absZ_ && zdiff / recoVertex.zError() < sigmaZ_) {

        vertexAssociation::TrackFraction tf = calculateVertexSharedTrackFractions(simVertex, recoVertex, *trackSimToRecoAssociation_, *trackRecoToSimAssociation_, dividerType_);

        auto fraction = tf.getFraction(associationType_);

        if(sharedFraction_ < 0 || fraction > sharedFraction_) {

          LogTrace("VertexAssociation") << "   Matched with significance " << zdiff/recoVertex.zError()
                                        << " numerator Sim " << associationTypeText
                                        << " denominator " << dividerTypeText
                                        << " shared fraction " << fraction << " reco Tracks " << recoVertex.tracksSize() << " TrackingParticles " << simVertex.nDaughterTracks();

          ret.insert(TrackingVertexRef(tVCH, iSim), std::make_pair(reco::VertexBaseRef(vCH, iReco), fraction));
        }
        
      }
    }
  }

  ret.post_insert();

  LogDebug("VertexAssociation") << "VertexAssociatorByPositionAndTracks::associateSimToReco(): finished";

  return ret;
}
