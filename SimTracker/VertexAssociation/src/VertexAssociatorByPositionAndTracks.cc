#include "SimTracker/VertexAssociation/interface/VertexAssociatorByPositionAndTracks.h"
#include "SimTracker/VertexAssociation/interface/calculateVertexSharedTracks.h"

#include "FWCore/MessageLogger/interface/MessageLogger.h"



VertexAssociatorByPositionAndTracks::VertexAssociatorByPositionAndTracks(const edm::EDProductGetter *productGetter,
                                                                         double absZ,
                                                                         double sigmaZ,
                                                                         double maxRecoZ,
                                                                         double sharedTrackFraction,
                                                                         const reco::RecoToSimCollection *trackRecoToSimAssociation,
                                                                         const reco::SimToRecoCollection *trackSimToRecoAssociation,
                                                                         const std::string& associationMode):
  productGetter_(productGetter),
  absZ_(absZ),
  sigmaZ_(sigmaZ),
  maxRecoZ_(maxRecoZ),
  sharedTrackFraction_(sharedTrackFraction),
  trackRecoToSimAssociation_(trackRecoToSimAssociation),
  trackSimToRecoAssociation_(trackSimToRecoAssociation),
  associationType_(vertexAssociation::AssociationType::NumberOfTracks)
{

  if(associationMode.compare("") == 0)
    associationType_ = vertexAssociation::AssociationType::NumberOfTracks;
  else if(associationMode.compare("W") == 0)
    associationType_ = vertexAssociation::AssociationType::W;
  else if(associationMode.compare("Pt") == 0)
    associationType_ = vertexAssociation::AssociationType::Pt;
  else if(associationMode.compare("WPt") == 0)
    associationType_ = vertexAssociation::AssociationType::WPt;
  else if(associationMode.compare("Pt2") == 0)
    associationType_ = vertexAssociation::AssociationType::Pt2;
  else if(associationMode.compare("WPt2") == 0)
    associationType_ = vertexAssociation::AssociationType::WPt2;
  else if(associationMode.compare("HarmPt") == 0)
    associationType_ = vertexAssociation::AssociationType::HarmPt;
  else if(associationMode.compare("WHarmPt") == 0)
    associationType_ = vertexAssociation::AssociationType::WHarmPt;
  else if(associationMode.compare("HarmWPt") == 0)
    associationType_ = vertexAssociation::AssociationType::HarmWPt;
  else if(associationMode.compare("HarmPtAvg") == 0)
    associationType_ = vertexAssociation::AssociationType::HarmPtAvg;
  else if(associationMode.compare("WHarmPtAvg") == 0)
    associationType_ = vertexAssociation::AssociationType::WHarmPtAvg;
  else if(associationMode.compare("HarmWPtAvg") == 0)
    associationType_ = vertexAssociation::AssociationType::HarmWPtAvg;

}

VertexAssociatorByPositionAndTracks::~VertexAssociatorByPositionAndTracks() {}

reco::VertexRecoToSimCollection VertexAssociatorByPositionAndTracks::associateRecoToSim(const edm::Handle<edm::View<reco::Vertex> >& vCH, 
                                                                                        const edm::Handle<TrackingVertexCollection>& tVCH) const {
  reco::VertexRecoToSimCollection ret(productGetter_);

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

        double momentumFraction = 0;

        std::vector<double> numerators = calculateVertexSharedTracksMomentumFractionNumerators(recoVertex, simVertex, *trackSimToRecoAssociation_, *trackRecoToSimAssociation_);
        std::vector<std::vector<double>> denominatorGroups = calculateVertexSharedTracksMomentumFractionDenominators(recoVertex, simVertex, *trackSimToRecoAssociation_, *trackRecoToSimAssociation_);
        

        std::vector<double> denominators = denominatorGroups[0];

        auto sharedTracks = numerators[0];
        auto fraction = double(sharedTracks)/recoVertex.tracksSize();

        switch(associationType_)
        {
          case vertexAssociation::AssociationType::NumberOfTracks:
            if( denominators[0] != 0 )
              momentumFraction = numerators[0]/denominators[0];
            break;
          case vertexAssociation::AssociationType::W:
            if( denominators[1] != 0 )
              momentumFraction = numerators[1]/denominators[1];
            break;
          case vertexAssociation::AssociationType::Pt:
            if( denominators[2] != 0 )
              momentumFraction = numerators[2]/denominators[2];
            break;
          case vertexAssociation::AssociationType::WPt:
            if( denominators[3] != 0 )
              momentumFraction = numerators[3]/denominators[3];
            break;
          case vertexAssociation::AssociationType::Pt2:
            if( denominators[4] != 0 )
              momentumFraction = numerators[4]/denominators[4];
            break;
          case vertexAssociation::AssociationType::WPt2:
            if( denominators[5] != 0 )
              momentumFraction = numerators[5]/denominators[5];
            break;
          case vertexAssociation::AssociationType::HarmPt:
            if( denominators[6] != 0 )
              momentumFraction = numerators[6]/denominators[6];
            break;
          case vertexAssociation::AssociationType::WHarmPt:
            if( denominators[7] != 0 )
              momentumFraction = numerators[7]/denominators[7];
            break;
          case vertexAssociation::AssociationType::HarmWPt:
            if( denominators[8] != 0 )
              momentumFraction = numerators[8]/denominators[8];
            break;
          case vertexAssociation::AssociationType::HarmPtAvg:
            if( denominators[9] != 0 )
              momentumFraction = numerators[9]/denominators[9];
            break;
          case vertexAssociation::AssociationType::WHarmPtAvg:
            if( denominators[10] != 0 )
              momentumFraction = numerators[10]/denominators[10];
            break;
          case vertexAssociation::AssociationType::HarmWPtAvg:
            if( denominators[11] != 0 )
              momentumFraction = numerators[11]/denominators[11];
            break;
        }

        if(associationType_ == vertexAssociation::AssociationType::NumberOfTracks) {

          if(sharedTrackFraction_ < 0 || fraction > sharedTrackFraction_) {
            LogTrace("VertexAssociation") << "   Matched with significance " << zdiff/recoVertex.zError()
                                          << " shared tracks " << sharedTracks << " reco Tracks " << recoVertex.tracksSize() << " TrackingParticles " << simVertex.nDaughterTracks() << " momentum fraction of shared tracks and tracks of all simulated and reconstructed vertices " << momentumFraction;

            ret.insert(reco::VertexBaseRef(vCH, iReco), std::make_pair(TrackingVertexRef(tVCH, iSim), sharedTracks));
          }
        }
        else if(sharedTrackFraction_ < 0 || momentumFraction > sharedTrackFraction_) {
          LogTrace("VertexAssociation") << "   Matched with significance " << zdiff/recoVertex.zError()
                                        << " shared tracks " << sharedTracks << " reco Tracks " << recoVertex.tracksSize() << " TrackingParticles " << simVertex.nDaughterTracks() << " momentum fraction of shared tracks and tracks of all simulated and reconstructed vertices " << momentumFraction;

          ret.insert(reco::VertexBaseRef(vCH, iReco), std::make_pair(TrackingVertexRef(tVCH, iSim), sharedTracks));
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

        double momentumFraction = 0;
     
        std::vector<double> numerators = calculateVertexSharedTracksMomentumFractionNumerators(simVertex, recoVertex, *trackSimToRecoAssociation_, *trackRecoToSimAssociation_);
       std::vector<std::vector<double>> denominatorGroups = calculateVertexSharedTracksMomentumFractionDenominators(simVertex, recoVertex, *trackSimToRecoAssociation_, *trackRecoToSimAssociation_);

        std::vector<double> denominators = denominatorGroups[0];

        auto sharedTracks = numerators[0];
        auto fraction = double(sharedTracks)/recoVertex.tracksSize();

        switch(associationType_)
        {
          case vertexAssociation::AssociationType::NumberOfTracks:
            if( denominators[0] != 0 )
            momentumFraction = numerators[0]/denominators[0];
            break;
          case vertexAssociation::AssociationType::W:
            if( denominators[1] != 0 )
            momentumFraction = numerators[1]/denominators[1];
            break;
          case vertexAssociation::AssociationType::Pt:
            if( denominators[2] != 0 )
            momentumFraction = numerators[2]/denominators[2];
            break;
          case vertexAssociation::AssociationType::WPt:
            if( denominators[3] != 0 )
            momentumFraction = numerators[3]/denominators[3];
            break;
          case vertexAssociation::AssociationType::Pt2:
            if( denominators[4] != 0 )
            momentumFraction = numerators[4]/denominators[4];
            break;
          case vertexAssociation::AssociationType::WPt2:
            if( denominators[5] != 0 )
            momentumFraction = numerators[5]/denominators[5];
            break;
          case vertexAssociation::AssociationType::HarmPt:
            if( denominators[6] != 0 )
            momentumFraction = numerators[6]/denominators[6];
            break;
          case vertexAssociation::AssociationType::WHarmPt:
            if( denominators[7] != 0 )
            momentumFraction = numerators[7]/denominators[7];
            break;
          case vertexAssociation::AssociationType::HarmWPt:
            if( denominators[8] != 0 )
            momentumFraction = numerators[8]/denominators[8];
            break;
          case vertexAssociation::AssociationType::HarmPtAvg:
            if( denominators[9] != 0 )
            momentumFraction = numerators[9]/denominators[9];
            break;
          case vertexAssociation::AssociationType::WHarmPtAvg:
            if( denominators[10] != 0 )
            momentumFraction = numerators[10]/denominators[10];
            break;
          case vertexAssociation::AssociationType::HarmWPtAvg:
            if( denominators[11] != 0 )
            momentumFraction = numerators[11]/denominators[11];
            break;
        }


        if(associationType_ == vertexAssociation::AssociationType::NumberOfTracks)  {
          if(sharedTrackFraction_ < 0 || fraction > sharedTrackFraction_) {

            LogTrace("VertexAssociation") << "   Matched with significance " << zdiff/recoVertex.zError()
                                          << " shared tracks " << sharedTracks << " reco Tracks " << recoVertex.tracksSize() << " TrackingParticles " << simVertex.nDaughterTracks() << " momentum fraction of shared tracks and tracks of all simulated and reconstructed vertices " << momentumFraction;

            ret.insert(TrackingVertexRef(tVCH, iSim), std::make_pair(reco::VertexBaseRef(vCH, iReco), sharedTracks));
          }
        }
        else if(sharedTrackFraction_ < 0 || momentumFraction > sharedTrackFraction_) {
          LogTrace("VertexAssociation") << "   Matched with significance " << zdiff/recoVertex.zError()
                                        << " shared tracks " << sharedTracks << " reco Tracks " << recoVertex.tracksSize() << " TrackingParticles " << simVertex.nDaughterTracks() << " momentum fraction of shared tracks and tracks of all simulated and reconstructed vertices " << momentumFraction;

          ret.insert(TrackingVertexRef(tVCH, iSim), std::make_pair(reco::VertexBaseRef(vCH, iReco), sharedTracks));
        }
      }
    }
  }

  ret.post_insert();

  LogDebug("VertexAssociation") << "VertexAssociatorByPositionAndTracks::associateSimToReco(): finished";

  return ret;
}
