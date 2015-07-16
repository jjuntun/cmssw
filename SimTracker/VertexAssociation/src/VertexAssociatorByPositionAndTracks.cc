

#include "SimTracker/VertexAssociation/interface/VertexAssociatorByPositionAndTracks.h"
#include "SimTracker/VertexAssociation/interface/calculateVertexSharedTracks.h"
#include "SimTracker/VertexAssociation/interface/calculateVertexSharedTracksMomentumFraction.h"

#include "FWCore/MessageLogger/interface/MessageLogger.h"

VertexAssociatorByPositionAndTracks::VertexAssociatorByPositionAndTracks(const edm::EDProductGetter *productGetter,
                                                                         double absZ,
                                                                         double sigmaZ,
                                                                         double maxRecoZ,
                                                                         double sharedTrackFraction,																
                                                                         const reco::RecoToSimCollection *trackRecoToSimAssociation,
                                                                         const reco::SimToRecoCollection *trackSimToRecoAssociation,
																																				 const std::string associationMode):
  productGetter_(productGetter),
  absZ_(absZ),
  sigmaZ_(sigmaZ),
  maxRecoZ_(maxRecoZ),
  sharedTrackFraction_(sharedTrackFraction),
  trackRecoToSimAssociation_(trackRecoToSimAssociation),
  trackSimToRecoAssociation_(trackSimToRecoAssociation),
	associationMode_(associationMode)
{}

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
        auto sharedTracks = calculateVertexSharedTracks(recoVertex, simVertex, *trackRecoToSimAssociation_);
        auto fraction = double(sharedTracks)/recoVertex.tracksSize();

        //double momentumFraction = calculateVertexSharedTracksMomentumFraction(recoVertex, simVertex, *trackSimToRecoAssociation_, *trackRecoToSimAssociation_, momentumSumType_, std::string("Reco"));

				double momentumFraction = 0;

				if(associationMode_ == "")
        	momentumFraction = calculateVertexSharedTracksMomentumFraction(recoVertex, simVertex, *trackSimToRecoAssociation_, *trackRecoToSimAssociation_, "", std::string("Reco"));
				else if(associationMode_ == "W")
        	momentumFraction = calculateVertexSharedTracksMomentumFraction(recoVertex, simVertex, *trackSimToRecoAssociation_, *trackRecoToSimAssociation_, "W", std::string("Reco"));
				else if(associationMode_ == "Pt")
        	momentumFraction = calculateVertexSharedTracksMomentumFraction(recoVertex, simVertex, *trackSimToRecoAssociation_, *trackRecoToSimAssociation_, "Pt", std::string("Reco"));
				else if(associationMode_ == "WPt")
        	momentumFraction = calculateVertexSharedTracksMomentumFraction(recoVertex, simVertex, *trackSimToRecoAssociation_, *trackRecoToSimAssociation_, "WPt", std::string("Reco"));
				else if(associationMode_ == "Pt2")
        	momentumFraction = calculateVertexSharedTracksMomentumFraction(recoVertex, simVertex, *trackSimToRecoAssociation_, *trackRecoToSimAssociation_, "Pt2", std::string("Reco"));
				else if(associationMode_ == "WPt2")
        	momentumFraction = calculateVertexSharedTracksMomentumFraction(recoVertex, simVertex, *trackSimToRecoAssociation_, *trackRecoToSimAssociation_, "WPt2", std::string("Reco"));
				else if(associationMode_ == "HarmPt")
        	momentumFraction = calculateVertexSharedTracksMomentumFraction(recoVertex, simVertex, *trackSimToRecoAssociation_, *trackRecoToSimAssociation_, "HarmPt", std::string("Reco"));
				else if(associationMode_ == "WHarmPt")
        	momentumFraction = calculateVertexSharedTracksMomentumFraction(recoVertex, simVertex, *trackSimToRecoAssociation_, *trackRecoToSimAssociation_, "WHarmPt", std::string("Reco"));
				else if(associationMode_ == "HarmWPt")
        	momentumFraction = calculateVertexSharedTracksMomentumFraction(recoVertex, simVertex, *trackSimToRecoAssociation_, *trackRecoToSimAssociation_, "HarmWPt", std::string("Reco"));
				else if(associationMode_ == "HarmPtAvg")
        	momentumFraction = calculateVertexSharedTracksMomentumFraction(recoVertex, simVertex, *trackSimToRecoAssociation_, *trackRecoToSimAssociation_, "HarmPtAvg", std::string("Reco"));
				else if(associationMode_ == "WHarmPtAvg")
        	momentumFraction = calculateVertexSharedTracksMomentumFraction(recoVertex, simVertex, *trackSimToRecoAssociation_, *trackRecoToSimAssociation_, "WHarmPtAvg", std::string("Reco"));
				else if(associationMode_ == "HarmWPtAvg")
        	momentumFraction = calculateVertexSharedTracksMomentumFraction(recoVertex, simVertex, *trackSimToRecoAssociation_, *trackRecoToSimAssociation_, "HarmWPtAvg", std::string("Reco"));
				else
        	momentumFraction = calculateVertexSharedTracksMomentumFraction(recoVertex, simVertex, *trackSimToRecoAssociation_, *trackRecoToSimAssociation_, "", std::string("Reco"));



        if(associationMode_.compare("") == 0) {

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
        auto sharedTracks = calculateVertexSharedTracks(simVertex, recoVertex, *trackSimToRecoAssociation_);
        auto fraction = double(sharedTracks)/recoVertex.tracksSize();

				double momentumFraction = 0;

				if(associationMode_ == "")
        	momentumFraction = calculateVertexSharedTracksMomentumFraction(simVertex, recoVertex, *trackSimToRecoAssociation_, *trackRecoToSimAssociation_, "", std::string("Reco"));
				else if(associationMode_ == "W")
        	momentumFraction = calculateVertexSharedTracksMomentumFraction(simVertex, recoVertex, *trackSimToRecoAssociation_, *trackRecoToSimAssociation_, "W", std::string("Reco"));
				else if(associationMode_ == "Pt")
        	momentumFraction = calculateVertexSharedTracksMomentumFraction(simVertex, recoVertex, *trackSimToRecoAssociation_, *trackRecoToSimAssociation_, "Pt", std::string("Reco"));
				else if(associationMode_ == "WPt")
        	momentumFraction = calculateVertexSharedTracksMomentumFraction(simVertex, recoVertex, *trackSimToRecoAssociation_, *trackRecoToSimAssociation_, "WPt", std::string("Reco"));
				else if(associationMode_ == "Pt2")
        	momentumFraction = calculateVertexSharedTracksMomentumFraction(simVertex, recoVertex, *trackSimToRecoAssociation_, *trackRecoToSimAssociation_, "Pt2", std::string("Reco"));
				else if(associationMode_ == "WPt2")
        	momentumFraction = calculateVertexSharedTracksMomentumFraction(simVertex, recoVertex, *trackSimToRecoAssociation_, *trackRecoToSimAssociation_, "WPt2", std::string("Reco"));
				else if(associationMode_ == "HarmPt")
        	momentumFraction = calculateVertexSharedTracksMomentumFraction(simVertex, recoVertex, *trackSimToRecoAssociation_, *trackRecoToSimAssociation_, "HarmPt", std::string("Reco"));
				else if(associationMode_ == "WHarmPt")
        	momentumFraction = calculateVertexSharedTracksMomentumFraction(simVertex, recoVertex, *trackSimToRecoAssociation_, *trackRecoToSimAssociation_, "WHarmPt", std::string("Reco"));
				else if(associationMode_ == "HarmWPt")
        	momentumFraction = calculateVertexSharedTracksMomentumFraction(simVertex, recoVertex, *trackSimToRecoAssociation_, *trackRecoToSimAssociation_, "HarmWPt", std::string("Reco"));
				else if(associationMode_ == "HarmPtAvg")
        	momentumFraction = calculateVertexSharedTracksMomentumFraction(simVertex, recoVertex, *trackSimToRecoAssociation_, *trackRecoToSimAssociation_, "HarmPtAvg", std::string("Reco"));
				else if(associationMode_ == "WHarmPtAvg")
        	momentumFraction = calculateVertexSharedTracksMomentumFraction(simVertex, recoVertex, *trackSimToRecoAssociation_, *trackRecoToSimAssociation_, "WHarmPtAvg", std::string("Reco"));
				else if(associationMode_ == "HarmWPtAvg")
        	momentumFraction = calculateVertexSharedTracksMomentumFraction(simVertex, recoVertex, *trackSimToRecoAssociation_, *trackRecoToSimAssociation_, "HarmWPtAvg", std::string("Reco"));
				else
        	momentumFraction = calculateVertexSharedTracksMomentumFraction(simVertex, recoVertex, *trackSimToRecoAssociation_, *trackRecoToSimAssociation_, "", std::string("Reco"));

				if(associationMode_.compare("") == 0)	{
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

