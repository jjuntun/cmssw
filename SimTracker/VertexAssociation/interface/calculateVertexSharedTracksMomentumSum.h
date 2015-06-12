#ifndef SimTracker_VertexAssociation_calculateVertexSharedTracksMomentumSumType_h
#define SimTracker_VertexAssociation_calculateVertexSharedTracksMomentumSumType_h

#include "DataFormats/RecoCandidate/interface/TrackAssociation.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "SimDataFormats/TrackingAnalysis/interface/TrackingVertex.h"

double calculateVertexSharedTracksMomentumSum(const reco::Vertex& recoV, const TrackingVertex& simV, const reco::RecoToSimCollection& trackRecoToSimAssociation, std::string momentumSumType);
double calculateVertexSharedTracksMomentumSum(const TrackingVertex& simV, const reco::Vertex& recoV, const reco::SimToRecoCollection& trackSimToRecoAssociation, std::string momentumSumType);

#endif
