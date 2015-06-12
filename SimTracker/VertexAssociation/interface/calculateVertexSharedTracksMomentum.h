#ifndef SimTracker_VertexAssociation_calculateVertexSharedTracksMomentum_h
#define SimTracker_VertexAssociation_calculateVertexSharedTracksMomentum_h

#include "DataFormats/RecoCandidate/interface/TrackAssociation.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "SimDataFormats/TrackingAnalysis/interface/TrackingVertex.h"

unsigned int calculateVertexSharedTracksMomentum(const reco::Vertex& recoV, const TrackingVertex& simV, const reco::RecoToSimCollection& trackRecoToSimAssociation, int mode);
unsigned int calculateVertexSharedTracksMomentum(const TrackingVertex& simV, const reco::Vertex& recoV, const reco::SimToRecoCollection& trackSimToRecoAssociation, int mode);

#endif
