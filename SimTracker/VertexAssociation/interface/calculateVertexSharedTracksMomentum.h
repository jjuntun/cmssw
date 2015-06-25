#ifndef SimTracker_VertexAssociation_calculateVertexSharedTracksMomentum_h
#define SimTracker_VertexAssociation_calculateVertexSharedTracksMomentum_h

#include "DataFormats/RecoCandidate/interface/TrackAssociation.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "SimDataFormats/TrackingAnalysis/interface/TrackingVertex.h"

double calculateVertexSharedTracksMomentum(const reco::Vertex& recoV, const TrackingVertex& simV, const reco::RecoToSimCollection& trackRecoToSimAssociation, std::string mode);
double calculateVertexSharedTracksMomentum(const TrackingVertex& simV, const reco::Vertex& recoV, const reco::SimToRecoCollection& trackSimToRecoAssociation, std::string mode);

#endif
