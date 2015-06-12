#include "SimTracker/VertexAssociation/interface/calculateVertexSharedTracksMomentum.h"

unsigned int calculateVertexSharedTracksMomentum(const reco::Vertex& recoV, const TrackingVertex& simV, const reco::RecoToSimCollection& trackRecoToSimAssociation, MomentumAssociationMode momentumAssociationMode) {
  double sharedMomentums = 0;
  for(auto iTrack = recoV.tracks_begin(); iTrack != recoV.tracks_end(); ++iTrack) {
    auto found = trackRecoToSimAssociation.find(*iTrack);

    if(found == trackRecoToSimAssociation.end())
      continue;

    // matched TP equal to any TP of sim vertex => increase momentum sum
    for(const auto& tp: found->val) {
      if(std::find_if(simV.daughterTracks_begin(), simV.daughterTracks_end(), [&](const TrackingParticleRef& vtp) {
            return tp.first == vtp;
          }) != simV.daughterTracks_end()) {
				
				double pt = (*(tp.first)).pt();
        switch(momentumAssociationMode)	{
						case MomentumAssociationMode::Pt : sharedMomentums += pt;
						case MomentumAssociationMode::WPt : sharedMomentums += recoV.trackWeight(*iTP)*pt;
						case MomentumAssociationMode::Pt2 : sharedMomentums += pt*pt;
						case MomentumAssociationMode::WPt2 : sharedMomentums += recoV.trackWeight(*iTP)*pt*pt;
				}
        break;
      }
    }
  }

  return sharedMomentums;
}

unsigned int calculateVertexSharedTracksMomentum(const TrackingVertex& simV, const reco::Vertex& recoV, const reco::SimToRecoCollection& trackSimToRecoAssociation, MomentumAssociationMode momentumAssociationMode) {
  double sharedMomentums = 0;
  for(auto iTP = simV.daughterTracks_begin(); iTP != simV.daughterTracks_end(); ++iTP) {
    auto found = trackSimToRecoAssociation.find(*iTP);

    if(found == trackSimToRecoAssociation.end())
      continue;

    // matched track equal to any track of reco vertex => increase momentum sum
    for(const auto& tk: found->val) {
      if(std::find_if(recoV.tracks_begin(), recoV.tracks_end(), [&](const reco::TrackBaseRef& vtk) {
            return tk.first == vtk;
          }) != recoV.tracks_end()) {

        double pt = (*(tk.first)).pt();
				switch(momentumAssociationMode)	{
						case MomentumAssociationMode::Pt : sharedMomentums += pt;
						case MomentumAssociationMode::WPt : sharedMomentums += recoV.trackWeight(*iTP)*pt;
						case MomentumAssociationMode::Pt2 : sharedMomentums += pt*pt;
						case MomentumAssociationMode::WPt2 : sharedMomentums += recoV.trackWeight(*iTP)*pt*pt;
				}
        break;
      }
    }
  }

  return sharedMomentums;
}

