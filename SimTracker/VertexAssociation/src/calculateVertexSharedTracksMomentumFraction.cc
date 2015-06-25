#include "SimTracker/VertexAssociation/interface/calculateVertexSharedTracksMomentumFraction.h"


// Parameters:
//						sumType = "Pt","WPt","Pt2","WPt2", W means weighted sum. Default value: "Pt"
double calculateVertexSharedTracksMomentumFraction(const reco::Vertex& recoV, const TrackingVertex& simV, const reco::RecoToSimCollection& trackRecoToSimAssociation, std::string sumType) {
  double sharedMomentumSum = 0;
	double totalMomentumSum = 0;
	double momentumFraction = 0;
	
	switch(sumType)	{
					case "Pt": {
						boolean trackFound = false;
						for(auto iTrack = recoV.tracks_begin(); iTrack != recoV.tracks_end(); ++iTrack) {
							auto found = trackRecoToSimAssociation.find(*iTrack);
							trackFound = false;

							if(found == trackRecoToSimAssociation.end())
								continue;

							
							for(const auto& tp: found->val) {
								if(!trackFound && std::find_if(simV.daughterTracks_begin(), simV.daughterTracks_end(), [&](const TrackingParticleRef& vtp) {
											return tp.first == vtp;
										}) != simV.daughterTracks_end()) {
									sharedMomentumSum += *(tp.first)->pt();
								}
								totalMomentumSum += *(tp.first)->pt();
							}									
						}
						break;
					}
					case "WPt": {
						boolean trackFound = false;
						for(auto iTrack = recoV.tracks_begin(); iTrack != recoV.tracks_end(); ++iTrack) {
							auto found = trackRecoToSimAssociation.find(*iTrack);
							trackFound = false;

							if(found == trackRecoToSimAssociation.end())
								continue;

							
							for(const auto& tp: found->val) {
								if(!trackFound && std::find_if(simV.daughterTracks_begin(), simV.daughterTracks_end(), [&](const TrackingParticleRef& vtp) {
											return tp.first == vtp;
										}) != simV.daughterTracks_end()) {
									float weight = recoV->trackWeight(*iTrack);
									sharedMomentumSum += *(tp.first)->pt()*weight;
									
								}
								totalMomentumSum += *(tp.first)->pt();
							}								
						}
						break;
					}
					case "Pt2": {
						boolean trackFound = false;
						for(auto iTrack = recoV.tracks_begin(); iTrack != recoV.tracks_end(); ++iTrack) {
							auto found = trackRecoToSimAssociation.find(*iTrack);
							trackFound = false;

							if(found == trackRecoToSimAssociation.end())
								continue;

							
							for(const auto& tp: found->val) {
								if(!trackFound && std::find_if(simV.daughterTracks_begin(), simV.daughterTracks_end(), [&](const TrackingParticleRef& vtp) {
											return tp.first == vtp;
										}) != simV.daughterTracks_end()) {
									sharedMomentumSum += pow(*(tp.first)->pt(),2);
								
								}
								totalMomentumSum += *(tp.first)->pt();
							}								
						}
						break;
					}
					case "WPt2": {
						boolean trackFound = false;
						for(auto iTrack = recoV.tracks_begin(); iTrack != recoV.tracks_end(); ++iTrack) {
							auto found = trackRecoToSimAssociation.find(*iTrack);
							trackFound = false;

							if(found == trackRecoToSimAssociation.end())
								continue;

							
							for(const auto& tp: found->val) {
								if(!trackFound && std::find_if(simV.daughterTracks_begin(), simV.daughterTracks_end(), [&](const TrackingParticleRef& vtp) {
											return tp.first == vtp;
										}) != simV.daughterTracks_end()) {
									float weight = recoV->trackWeight(*iTrack);
									sharedMomentumSum += pow(*(tp.first)->pt(),2)*weight;
							
								}
								totalMomentumSum += *(tp.first)->pt();
							}									
						}
						break;
					}
					default: {
						for(auto iTrack = recoV.tracks_begin(); iTrack != recoV.tracks_end(); ++iTrack) {
							auto found = trackRecoToSimAssociation.find(*iTrack);
							trackFound = false;

							if(found == trackRecoToSimAssociation.end())
								continue;

							
							for(const auto& tp: found->val) {
								if(!trackFound && std::find_if(simV.daughterTracks_begin(), simV.daughterTracks_end(), [&](const TrackingParticleRef& vtp) {
											return tp.first == vtp;
										}) != simV.daughterTracks_end()) {
									sharedMomentumSum += *(tp.first)->pt();
									
								}
								totalMomentumSum += *(tp.first)->pt();
							}									
						}
						break;
					}
  }
	momentumFraction = (double)sharedMomentumSum/totalMomentumSum;
  return momentumFraction;
}


double calculateVertexSharedTracksMomentumFraction(const TrackingVertex& simV, const reco::Vertex& recoV, const reco::SimToRecoCollection& trackSimToRecoAssociation, std::string sumType) {
  double sharedMomentumSum = 0;
	double totalMomentumSum = 0;
	double momentumFraction = 0;

	switch(sumType)	{
					case "Pt": {
						boolean trackFound = false;
						for(auto iTP = simV.daughterTracks_begin(); iTP != simV.daughterTracks_end(); ++iTP) {
							auto found = trackSimToRecoAssociation.find(*iTP);
							trackFound = false;
							if(found == trackSimToRecoAssociation.end())
								continue;

							// matched track equal to any track of reco vertex => increase counter
							for(const auto& tk: found->val) {
								if(!trackFound && std::find_if(recoV.tracks_begin(), recoV.tracks_end(), [&](const reco::TrackBaseRef& vtk) {
										  return tk.first == vtk;
										}) != recoV.tracks_end()) {
									sharedMomentumSum += *(tk.first)->pt();
									
								}
								totalMomentumSum += *(tp.first)->pt();
							}
						}
					}
					case "WPt": {
						boolean trackFound = false;
						for(auto iTP = simV.daughterTracks_begin(); iTP != simV.daughterTracks_end(); ++iTP) {
							auto found = trackSimToRecoAssociation.find(*iTP);
							trackFound = false;
							if(found == trackSimToRecoAssociation.end())
								continue;

							// matched track equal to any track of reco vertex => increase counter
							for(const auto& tk: found->val) {
								if(!trackFound && std::find_if(recoV.tracks_begin(), recoV.tracks_end(), [&](const reco::TrackBaseRef& vtk) {
										  return tk.first == vtk;
										}) != recoV.tracks_end()) {
									float weight = recoV->trackWeight(*iTP);
									sharedMomentumSum += *(tk.first)->pt()*weight;
									
								}
								totalMomentumSum += *(tp.first)->pt();
							}
						}
					}
					case "Pt2": {
						boolean trackFound = false;
						for(auto iTP = simV.daughterTracks_begin(); iTP != simV.daughterTracks_end(); ++iTP) {
							auto found = trackSimToRecoAssociation.find(*iTP);
							trackFound = false;
							if(found == trackSimToRecoAssociation.end())
								continue;

							// matched track equal to any track of reco vertex => increase counter
							for(const auto& tk: found->val) {
								if(!trackFound && std::find_if(recoV.tracks_begin(), recoV.tracks_end(), [&](const reco::TrackBaseRef& vtk) {
										  return tk.first == vtk;
										}) != recoV.tracks_end()) {
									sharedMomentumSum += pow(*(tk.first)->pt(),2);
									
								}
								totalMomentumSum += *(tp.first)->pt();
							}
						}
					}
					case "WPt2": {
						boolean trackFound = false;
						for(auto iTP = simV.daughterTracks_begin(); iTP != simV.daughterTracks_end(); ++iTP) {
							auto found = trackSimToRecoAssociation.find(*iTP);
							trackFound = false;
							if(found == trackSimToRecoAssociation.end())
								continue;

							// matched track equal to any track of reco vertex => increase counter
							for(const auto& tk: found->val) {
								if(!trackFound && std::find_if(recoV.tracks_begin(), recoV.tracks_end(), [&](const reco::TrackBaseRef& vtk) {
										  return tk.first == vtk;
										}) != recoV.tracks_end()) {
									float weight = recoV->trackWeight(*iTP);
									sharedMomentumSum += pow(*(tk.first)->pt(),2)*weight;
									
								}
								totalMomentumSum += *(tp.first)->pt();
							}
						}
					}
					default: {
						boolean trackFound = false;
						for(auto iTP = simV.daughterTracks_begin(); iTP != simV.daughterTracks_end(); ++iTP) {
							auto found = trackSimToRecoAssociation.find(*iTP);
							trackFound = false;
							if(found == trackSimToRecoAssociation.end())
								continue;

							// matched track equal to any track of reco vertex => increase counter
							for(const auto& tk: found->val) {
								if(!trackFound && std::find_if(recoV.tracks_begin(), recoV.tracks_end(), [&](const reco::TrackBaseRef& vtk) {
										  return tk.first == vtk;
										}) != recoV.tracks_end()) {
									sharedMomentumSum += *(tk.first)->pt();
								
								}
								totalMomentumSum += *(tp.first)->pt();
							}
						}
					}
	}
	momentumFraction = (double)sharedMomentumSum/totalMomentumSum;
  return momentumFraction;
}
