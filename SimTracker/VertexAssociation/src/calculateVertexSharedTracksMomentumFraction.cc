#include "SimTracker/VertexAssociation/interface/calculateVertexSharedTracksMomentumFraction.h"


// Parameters:
//						sumType = "Pt","WPt","Pt2","WPt2", W means weighted sum. Default value: "Pt"
double calculateVertexSharedTracksMomentumFraction(const reco::Vertex& recoV, const TrackingVertex& simV, const reco::RecoToSimCollection& trackRecoToSimAssociation, std::string sumType) {
  double sharedMomentumSum = 0;
	double totalMomentumSum = 0;
	
	switch(sumType)	{
					case "Pt": {
						for(auto iTrack = recoV.tracks_begin(); iTrack != recoV.tracks_end(); ++iTrack) {
							auto found = trackRecoToSimAssociation.find(*iTrack);

							if(found == trackRecoToSimAssociation.end())
								continue;

							
							for(const auto& tp: found->val) {
								if(std::find_if(simV.daughterTracks_begin(), simV.daughterTracks_end(), [&](const TrackingParticleRef& vtp) {
											return tp.first == vtp;
										}) != simV.daughterTracks_end()) {
									sharedMomentumSum += *(tp.first)->pt();
									break;
								}
							}									
						}
						break;
					}
					case "WPt": {
						for(auto iTrack = recoV.tracks_begin(); iTrack != recoV.tracks_end(); ++iTrack) {
							auto found = trackRecoToSimAssociation.find(*iTrack);

							if(found == trackRecoToSimAssociation.end())
								continue;

							
							for(const auto& tp: found->val) {
								if(std::find_if(simV.daughterTracks_begin(), simV.daughterTracks_end(), [&](const TrackingParticleRef& vtp) {
											return tp.first == vtp;
										}) != simV.daughterTracks_end()) {
									float weight = recoV->trackWeight(*iTrack);
									sharedMomentumSum += *(tp.first)->pt()*weight;
									break;
								}
							}								
						}
						break;
					}
					case "Pt2": {
						for(auto iTrack = recoV.tracks_begin(); iTrack != recoV.tracks_end(); ++iTrack) {
							auto found = trackRecoToSimAssociation.find(*iTrack);

							if(found == trackRecoToSimAssociation.end())
								continue;

							
							for(const auto& tp: found->val) {
								if(std::find_if(simV.daughterTracks_begin(), simV.daughterTracks_end(), [&](const TrackingParticleRef& vtp) {
											return tp.first == vtp;
										}) != simV.daughterTracks_end()) {
									sharedMomentumSum += pow(*(tp.first)->pt(),2);
									break;
								}
							}								
						}
						break;
					}
					case "WPt2": {
						for(auto iTrack = recoV.tracks_begin(); iTrack != recoV.tracks_end(); ++iTrack) {
							auto found = trackRecoToSimAssociation.find(*iTrack);

							if(found == trackRecoToSimAssociation.end())
								continue;

							
							for(const auto& tp: found->val) {
								if(std::find_if(simV.daughterTracks_begin(), simV.daughterTracks_end(), [&](const TrackingParticleRef& vtp) {
											return tp.first == vtp;
										}) != simV.daughterTracks_end()) {
									float weight = recoV->trackWeight(*iTrack);
									sharedMomentumSum += pow(*(tp.first)->pt(),2)*weight;
									break;
								}
							}									
						}
						break;
					}
					default: {
						for(auto iTrack = recoV.tracks_begin(); iTrack != recoV.tracks_end(); ++iTrack) {
							auto found = trackRecoToSimAssociation.find(*iTrack);

							if(found == trackRecoToSimAssociation.end())
								continue;

							
							for(const auto& tp: found->val) {
								if(std::find_if(simV.daughterTracks_begin(), simV.daughterTracks_end(), [&](const TrackingParticleRef& vtp) {
											return tp.first == vtp;
										}) != simV.daughterTracks_end()) {
									sharedMomentumSum += *(tp.first)->pt();
									break;
								}
							}									
						}
						break;
					}
  }

  return sharedTracks;
}

double calculateVertexSharedTracksMomentumFraction(const TrackingVertex& simV, const reco::Vertex& recoV, const reco::SimToRecoCollection& trackSimToRecoAssociation, std::string sumType) {
  double sharedMomentumSum = 0;

	switch(sumType)	{
					case "Pt": {
						for(auto iTP = simV.daughterTracks_begin(); iTP != simV.daughterTracks_end(); ++iTP) {
							auto found = trackSimToRecoAssociation.find(*iTP);

							if(found == trackSimToRecoAssociation.end())
								continue;

							// matched track equal to any track of reco vertex => increase counter
							for(const auto& tk: found->val) {
								if(std::find_if(recoV.tracks_begin(), recoV.tracks_end(), [&](const reco::TrackBaseRef& vtk) {
										  return tk.first == vtk;
										}) != recoV.tracks_end()) {
									sharedMomentumSum += *(tk.first)->pt();
									break;
								}
							}
						}
					}
					case "WPt": {
						for(auto iTP = simV.daughterTracks_begin(); iTP != simV.daughterTracks_end(); ++iTP) {
							auto found = trackSimToRecoAssociation.find(*iTP);

							if(found == trackSimToRecoAssociation.end())
								continue;

							// matched track equal to any track of reco vertex => increase counter
							for(const auto& tk: found->val) {
								if(std::find_if(recoV.tracks_begin(), recoV.tracks_end(), [&](const reco::TrackBaseRef& vtk) {
										  return tk.first == vtk;
										}) != recoV.tracks_end()) {
									float weight = recoV->trackWeight(*iTP);
									sharedMomentumSum += *(tk.first)->pt()*weight;
									break;
								}
							}
						}
					}
					case "Pt2": {
						for(auto iTP = simV.daughterTracks_begin(); iTP != simV.daughterTracks_end(); ++iTP) {
							auto found = trackSimToRecoAssociation.find(*iTP);

							if(found == trackSimToRecoAssociation.end())
								continue;

							// matched track equal to any track of reco vertex => increase counter
							for(const auto& tk: found->val) {
								if(std::find_if(recoV.tracks_begin(), recoV.tracks_end(), [&](const reco::TrackBaseRef& vtk) {
										  return tk.first == vtk;
										}) != recoV.tracks_end()) {
									sharedMomentumSum += pow(*(tk.first)->pt(),2);
									break;
								}
							}
						}
					}
					case "WPt2": {
						for(auto iTP = simV.daughterTracks_begin(); iTP != simV.daughterTracks_end(); ++iTP) {
							auto found = trackSimToRecoAssociation.find(*iTP);

							if(found == trackSimToRecoAssociation.end())
								continue;

							// matched track equal to any track of reco vertex => increase counter
							for(const auto& tk: found->val) {
								if(std::find_if(recoV.tracks_begin(), recoV.tracks_end(), [&](const reco::TrackBaseRef& vtk) {
										  return tk.first == vtk;
										}) != recoV.tracks_end()) {
									float weight = recoV->trackWeight(*iTP);
									sharedMomentumSum += pow(*(tk.first)->pt(),2)*weight;
									break;
								}
							}
						}
					}
					default: {
						for(auto iTP = simV.daughterTracks_begin(); iTP != simV.daughterTracks_end(); ++iTP) {
							auto found = trackSimToRecoAssociation.find(*iTP);

							if(found == trackSimToRecoAssociation.end())
								continue;

							// matched track equal to any track of reco vertex => increase counter
							for(const auto& tk: found->val) {
								if(std::find_if(recoV.tracks_begin(), recoV.tracks_end(), [&](const reco::TrackBaseRef& vtk) {
										  return tk.first == vtk;
										}) != recoV.tracks_end()) {
									sharedMomentumSum += *(tk.first)->pt();
									break;
								}
							}
						}
					}
	}

  return sharedTracks;
}
