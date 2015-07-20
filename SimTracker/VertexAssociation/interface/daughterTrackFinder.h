#ifndef SimTracker_VertexAssociation_daughterTrackFinder_h
#define SimTracker_VertexAssociation_daughterTrackFinder_h

#include "SimDataFormats/TrackingAnalysis/interface/TrackingParticle.h"
#include "SimDataFormats/TrackingAnalysis/interface/TrackingVertex.h"
#include "FWCore/Framework/interface/Event.h"

void daughterTrackFinder(TrackingVertex a, std::vector<TrackingParticleRef>& connectedTrackingParticleRefs, std::vector<TrackingVertex>& connectedTrackingVertices);

#endif
