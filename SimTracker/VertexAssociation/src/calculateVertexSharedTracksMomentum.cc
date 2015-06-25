#include "SimTracker/VertexAssociation/interface/calculateVertexSharedTracksMomentum.h"


//Parameters :
//						momentumAssociationMode = "Pt": pt sum, "WPt": weighted pt sum, "Pt2": pt2 sum, "WPt2": weighted pt2 sum
double calculateVertexSharedTracksMomentum(const reco::Vertex& recoV, const TrackingVertex& simV, const reco::RecoToSimCollection& trackRecoToSimAssociation, std::string momentumAssociationMode) {
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
        if(momentumAssociationMode.compare("Pt"))
							sharedMomentums += pt;
				else if(momentumAssociationMode.compare("WPt"))
							sharedMomentums += recoV.trackWeight(*iTrack)*pt;
				else if(momentumAssociationMode.compare("Pt2"))
							sharedMomentums += pt*pt;
				else if(momentumAssociationMode.compare("WPt2"))
							sharedMomentums += recoV.trackWeight(*iTrack)*pt*pt;
				else
							sharedMomentums += pt;
      }
    }
  }

  return sharedMomentums;
}

double calculateVertexSharedTracksMomentum(const TrackingVertex& simV, const reco::Vertex& recoV, const reco::SimToRecoCollection& trackSimToRecoAssociation, std::string momentumAssociationMode) {
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
        if(momentumAssociationMode.compare("Pt"))
							sharedMomentums += pt;
				else if(momentumAssociationMode.compare("WPt"))
							sharedMomentums += recoV.trackWeight(*iTP)*pt;
				else if(momentumAssociationMode.compare("Pt2"))
							sharedMomentums += pt*pt;
				else if(momentumAssociationMode.compare("WPt2"))
							sharedMomentums += recoV.trackWeight(*iTP)*pt*pt;
				else
							sharedMomentums += pt;

      }
    }
  }

  return sharedMomentums;
}

