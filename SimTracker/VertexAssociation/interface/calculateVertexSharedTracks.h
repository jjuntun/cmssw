#ifndef SimTracker_VertexAssociation_calculateVertexSharedTracks_h
#define SimTracker_VertexAssociation_calculateVertexSharedTracks_h

#include "DataFormats/RecoCandidate/interface/TrackAssociation.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "SimDataFormats/TrackingAnalysis/interface/TrackingVertex.h"

unsigned int calculateVertexSharedTracks(const reco::Vertex& recoV, const TrackingVertex& simV, const reco::RecoToSimCollection& trackRecoToSimAssociation);
unsigned int calculateVertexSharedTracks(const TrackingVertex& simV, const reco::Vertex& recoV, const reco::SimToRecoCollection& trackSimToRecoAssociation);

std::vector<double> calculateVertexSharedTracksMomentumFractionNumerators(const reco::Vertex& recoV, const TrackingVertex& simV,  const reco::SimToRecoCollection& trackSimToRecoAssociation, const reco::RecoToSimCollection& trackRecoToSimAssociation);
std::vector<std::vector<double>> calculateVertexSharedTracksMomentumFractionDenominators(const reco::Vertex& recoV, const TrackingVertex& simV,  const reco::SimToRecoCollection& trackSimToRecoAssociation, const reco::RecoToSimCollection& trackRecoToSimAssociation);
std::vector<double> calculateVertexSharedTracksMomentumFractionNumerators(const TrackingVertex& simV, const reco::Vertex& recoV, const reco::SimToRecoCollection& trackSimToRecoAssociation, const reco::RecoToSimCollection& trackRecoToSimAssociation);
std::vector<std::vector<double>> calculateVertexSharedTracksMomentumFractionDenominators(const TrackingVertex& simV, const reco::Vertex& recoV, const reco::SimToRecoCollection& trackSimToRecoAssociation, const reco::RecoToSimCollection& trackRecoToSimAssociation);

#endif

