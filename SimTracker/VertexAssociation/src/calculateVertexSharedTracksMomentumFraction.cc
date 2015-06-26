#include "SimTracker/VertexAssociation/interface/calculateVertexSharedTracksMomentumFraction.h"
#include "SimTracker/VertexAssociation/interface/calculateVertexSharedTracks.h"


// Parameters:
//						sumType = "","W","Pt","WPt","Pt2","WPt2", W means weighted sum. Default value: "Pt"
//						divider = defines the denominator in fraction, and is "Reco", "RecoMatched", "Sim" or "SimMatched"

// calculations for RecoAllAssoc2Gen- plottings
double calculateVertexSharedTracksMomentumFraction(const reco::Vertex& recoV, const TrackingVertex& simV, const reco::SimToRecoCollection& trackSimToRecoAssociation, const reco::RecoToSimCollection& trackRecoToSimAssociation, std::string sumType, std::string divider) {
  double sharedMomentumSum = 0;
	double denominator = 0;
	double momentumFraction = 0;
	
	if(sumType.compare("") == 0)
		sharedMomentumSum = calculateVertexSharedTracks(recoV,simV, trackRecoToSimAssociation);
	else if(sumType.compare("W") == 0) {
		for(auto iTrack = recoV.tracks_begin(); iTrack != recoV.tracks_end(); ++iTrack) {
		  auto found = trackRecoToSimAssociation.find(*iTrack);

		  if(found == trackRecoToSimAssociation.end())
		    continue;

			float weight = recoV.trackWeight(*iTrack);
		  // matched TP equal to any TP of sim vertex => increase counter
		  for(const auto& tp: found->val) {
		    if(std::find_if(simV.daughterTracks_begin(), simV.daughterTracks_end(), [&](const TrackingParticleRef& vtp) {
		          return tp.first == vtp;
		        }) != simV.daughterTracks_end()) {
		      sharedMomentumSum += weight;
		      break;
		    }
		  }
		}
	}
	else if(sumType.compare("Pt") == 0) {
		for(auto iTrack = recoV.tracks_begin(); iTrack != recoV.tracks_end(); ++iTrack) {
			auto found = trackRecoToSimAssociation.find(*iTrack);

			if(found == trackRecoToSimAssociation.end())
				continue;

			
			for(const auto& tp: found->val) {
				if(std::find_if(simV.daughterTracks_begin(), simV.daughterTracks_end(), [&](const TrackingParticleRef& vtp) {
							return tp.first == vtp;
						}) != simV.daughterTracks_end()) {
					sharedMomentumSum += (*iTrack).get()->pt();
					break;
				}
			}									
		}
	}
	else if(sumType.compare("WPt") == 0) {
		for(auto iTrack = recoV.tracks_begin(); iTrack != recoV.tracks_end(); ++iTrack) {
			auto found = trackRecoToSimAssociation.find(*iTrack);

			if(found == trackRecoToSimAssociation.end())
				continue;

			float weight = recoV.trackWeight(*iTrack);
			for(const auto& tp: found->val) {
				if(std::find_if(simV.daughterTracks_begin(), simV.daughterTracks_end(), [&](const TrackingParticleRef& vtp) {
							return tp.first == vtp;
						}) != simV.daughterTracks_end()) {
					sharedMomentumSum += (*iTrack).get()->pt()*weight;
					break;				
				}
			}								
		}

	}
	else if(sumType.compare("Pt2") == 0) {
		for(auto iTrack = recoV.tracks_begin(); iTrack != recoV.tracks_end(); ++iTrack) {
			auto found = trackRecoToSimAssociation.find(*iTrack);

			if(found == trackRecoToSimAssociation.end())
				continue;

		
			for(const auto& tp: found->val) {
				if(std::find_if(simV.daughterTracks_begin(), simV.daughterTracks_end(), [&](const TrackingParticleRef& vtp) {
							return tp.first == vtp;
						}) != simV.daughterTracks_end()) {
					sharedMomentumSum += pow((*iTrack).get()->pt(),2);	
					break;
				}
			}								
		}
	}
	else if(sumType.compare("WPt2") == 0) {
		for(auto iTrack = recoV.tracks_begin(); iTrack != recoV.tracks_end(); ++iTrack) {
			auto found = trackRecoToSimAssociation.find(*iTrack);

			if(found == trackRecoToSimAssociation.end())
				continue;

		
			for(const auto& tp: found->val) {
				if(std::find_if(simV.daughterTracks_begin(), simV.daughterTracks_end(), [&](const TrackingParticleRef& vtp) {
							return tp.first == vtp;
						}) != simV.daughterTracks_end()) {
					float weight = recoV.trackWeight(*iTrack);
					sharedMomentumSum += pow((*iTrack).get()->pt(),2)*weight;
					break;		
				}
			}									
		}
	}
	else if(sumType.compare("HarmPt") == 0) {
		for(auto iTrack = recoV.tracks_begin(); iTrack != recoV.tracks_end(); ++iTrack) {
			auto found = trackRecoToSimAssociation.find(*iTrack);

			if(found == trackRecoToSimAssociation.end())
				continue;

		
			for(const auto& tp: found->val) {
				if(std::find_if(simV.daughterTracks_begin(), simV.daughterTracks_end(), [&](const TrackingParticleRef& vtp) {
							return tp.first == vtp;
						}) != simV.daughterTracks_end()) {
					if( (*iTrack).get()->pt() != 0 )
						sharedMomentumSum += 1/((*iTrack).get()->pt());
					break;
				}
			}									
		}
	}
	else if(sumType.compare("WHarmPt") == 0) {
		for(auto iTrack = recoV.tracks_begin(); iTrack != recoV.tracks_end(); ++iTrack) {
			auto found = trackRecoToSimAssociation.find(*iTrack);

			if(found == trackRecoToSimAssociation.end())
				continue;

		
			for(const auto& tp: found->val) {
				if(std::find_if(simV.daughterTracks_begin(), simV.daughterTracks_end(), [&](const TrackingParticleRef& vtp) {
							return tp.first == vtp;
						}) != simV.daughterTracks_end()) {
					float weight = recoV.trackWeight(*iTrack);
					if( (*iTrack).get()->pt() != 0 )
						sharedMomentumSum += weight/((*iTrack).get()->pt());
					break;
				}
			}									
		}
	}
	else if(sumType.compare("HarmWPt") == 0) {
		for(auto iTrack = recoV.tracks_begin(); iTrack != recoV.tracks_end(); ++iTrack) {
			auto found = trackRecoToSimAssociation.find(*iTrack);

			if(found == trackRecoToSimAssociation.end())
				continue;

		
			for(const auto& tp: found->val) {
				if(std::find_if(simV.daughterTracks_begin(), simV.daughterTracks_end(), [&](const TrackingParticleRef& vtp) {
							return tp.first == vtp;
						}) != simV.daughterTracks_end()) {
					float weight = recoV.trackWeight(*iTrack);
					if( (*iTrack).get()->pt()*weight != 0 )
						sharedMomentumSum += 1/((*iTrack).get()->pt()*weight);
					break;
				}
			}									
		}
	}
	else if(sumType.compare("HarmPtAvg") == 0) {
		unsigned int elements = 0;
		for(auto iTrack = recoV.tracks_begin(); iTrack != recoV.tracks_end(); ++iTrack) {
			auto found = trackRecoToSimAssociation.find(*iTrack);

			if(found == trackRecoToSimAssociation.end())
				continue;

		
			for(const auto& tp: found->val) {
				if(std::find_if(simV.daughterTracks_begin(), simV.daughterTracks_end(), [&](const TrackingParticleRef& vtp) {
							return tp.first == vtp;
						}) != simV.daughterTracks_end()) {
					float weight = recoV.trackWeight(*iTrack);
					if( (*iTrack).get()->pt()*weight != 0 )
						sharedMomentumSum += 1/((*iTrack).get()->pt()*weight);
					elements++;
					break;
				}
			}									
		}
		if( sharedMomentumSum != 0)
			sharedMomentumSum = elements/sharedMomentumSum;
	}
	else if(sumType.compare("WHarmPtAvg") == 0) {
		unsigned int elements = 0;
		for(auto iTrack = recoV.tracks_begin(); iTrack != recoV.tracks_end(); ++iTrack) {
			auto found = trackRecoToSimAssociation.find(*iTrack);

			if(found == trackRecoToSimAssociation.end())
				continue;

		
			for(const auto& tp: found->val) {
				if(std::find_if(simV.daughterTracks_begin(), simV.daughterTracks_end(), [&](const TrackingParticleRef& vtp) {
							return tp.first == vtp;
						}) != simV.daughterTracks_end()) {
					float weight = recoV.trackWeight(*iTrack);
					if( (*iTrack).get()->pt()*weight != 0 )
						sharedMomentumSum += 1/((*iTrack).get()->pt()*weight);
					elements++;
					break;
				}
			}									
		}
		if( sharedMomentumSum != 0)
			sharedMomentumSum = elements/sharedMomentumSum;
	}
	else if(sumType.compare("HarmWPtAvg") == 0) {
		unsigned int elements = 0;
		for(auto iTrack = recoV.tracks_begin(); iTrack != recoV.tracks_end(); ++iTrack) {
			auto found = trackRecoToSimAssociation.find(*iTrack);

			if(found == trackRecoToSimAssociation.end())
				continue;

		
			for(const auto& tp: found->val) {
				if(std::find_if(simV.daughterTracks_begin(), simV.daughterTracks_end(), [&](const TrackingParticleRef& vtp) {
							return tp.first == vtp;
						}) != simV.daughterTracks_end()) {
					float weight = recoV.trackWeight(*iTrack);
					if( (*iTrack).get()->pt()*weight != 0 )
						sharedMomentumSum += 1/((*iTrack).get()->pt()*weight);
					elements++;
					break;
				}
			}									
		}
		if( sharedMomentumSum != 0)
			sharedMomentumSum = elements/sharedMomentumSum;
	}
	else //"Pt2"
 {
		for(auto iTrack = recoV.tracks_begin(); iTrack != recoV.tracks_end(); ++iTrack) {
			auto found = trackRecoToSimAssociation.find(*iTrack);

			if(found == trackRecoToSimAssociation.end())
				continue;

		
			for(const auto& tp: found->val) {
				if(std::find_if(simV.daughterTracks_begin(), simV.daughterTracks_end(), [&](const TrackingParticleRef& vtp) {
							return tp.first == vtp;
						}) != simV.daughterTracks_end()) {
					sharedMomentumSum += pow((*iTrack).get()->pt(),2);
					break;
				}
			}									
		}
	}

	//calculate the divider in the fraction
	if(divider.compare("Reco") == 0)
	{
		unsigned int elements = 0;
		for(auto iTrack = recoV.tracks_begin(); iTrack != recoV.tracks_end(); ++iTrack)	{

			float weight = recoV.trackWeight(*iTrack);
			if(sumType.compare("") == 0)
			{
				denominator += 1;
			}
			else if(sumType.compare("W") == 0)
			{
				denominator += weight;
			}
			else if(sumType.compare("Pt") == 0)
			{
				denominator += (*iTrack)->pt();
			}
			else if(sumType.compare("WPt") == 0)
			{
				denominator += (*iTrack)->pt()*weight;
			}
			else if(sumType.compare("Pt2") == 0)
			{
				denominator += pow((*iTrack)->pt(),2);
			}
			else if(sumType.compare("WPt2") == 0)
			{
				denominator += pow((*iTrack)->pt(),2)*weight;
			}
			else if(sumType.compare("HarmPt") == 0)
			{
				if( (*iTrack)->pt() != 0 )
					denominator += 1/((*iTrack)->pt());
			}
			else if(sumType.compare("WHarmPt") == 0)
			{
				if( (*iTrack)->pt() != 0 )
					denominator += weight/((*iTrack)->pt());
			}
			else if(sumType.compare("HarmWPt") == 0)
			{
				if( (*iTrack)->pt()*weight != 0 )
					denominator += 1/((*iTrack)->pt()*weight);
			}
			else if(sumType.compare("HarmPtAvg") == 0)
			{
				if( (*iTrack)->pt() != 0 )
					denominator += 1/((*iTrack)->pt());
			}
			else if(sumType.compare("WHarmPtAvg") == 0)
			{
				if( (*iTrack)->pt() != 0 )
					denominator += weight/((*iTrack)->pt());
			}
			else if(sumType.compare("HarmWPtAvg") == 0)
			{
				if( (*iTrack)->pt()*weight != 0 )
					denominator += 1/((*iTrack)->pt()*weight);
			}
			else // "Pt2"
			{
				denominator += pow((*iTrack)->pt(),2);
			}

			elements++;
		}

		if(sumType.compare("HarmPtAvg") == 0)
			{
				if( denominator != 0)
					denominator = elements/denominator;
			}
		else if(sumType.compare("WHarmPtAvg") == 0)
		{
			if( denominator != 0)
				denominator = elements/denominator;
		}
		else if(sumType.compare("HarmWPtAvg") == 0)
		{
			if( denominator != 0)
				denominator = elements/denominator;
		}
		
	}
	else if(divider.compare("RecoMatched") == 0)
	{
		unsigned int elements = 0;

		for(auto iTrack = recoV.tracks_begin(); iTrack != recoV.tracks_end(); ++iTrack) {
			float weight = recoV.trackWeight(*iTrack);

			auto found = trackRecoToSimAssociation.find(*iTrack);

			if(found == trackRecoToSimAssociation.end())
							continue;

			if(sumType.compare("") == 0)
			{
				denominator += 1;
			}
			else if(sumType.compare("W") == 0)
			{
				denominator += weight;
			}
			else if(sumType.compare("Pt") == 0)
			{
				denominator += (*iTrack).get()->pt();
			}
			else if(sumType.compare("WPt") == 0)
			{
				denominator += (*iTrack).get()->pt()*weight;
			}
			else if(sumType.compare("Pt2") == 0)
			{
				denominator += pow((*iTrack).get()->pt(),2);
			}
			else if(sumType.compare("WPt2") == 0)
			{
				denominator += pow((*iTrack).get()->pt(),2)*weight;
			}
			else if(sumType.compare("HarmPt") == 0)
			{
				if( (*iTrack)->pt() != 0 )
					denominator += 1/((*iTrack)->pt());
			}
			else if(sumType.compare("WHarmPt") == 0)
			{
				if( (*iTrack)->pt() != 0 )
					denominator += weight/((*iTrack)->pt());
			}
			else if(sumType.compare("HarmWPt") == 0)
			{
				if( (*iTrack)->pt()*weight != 0 )
					denominator += 1/((*iTrack)->pt()*weight);
			}
			else if(sumType.compare("HarmPtAvg") == 0)
			{
				if( (*iTrack)->pt() != 0 )
					denominator += 1/((*iTrack)->pt());
			}
			else if(sumType.compare("WHarmPtAvg") == 0)
			{
				if( (*iTrack)->pt() != 0 )
					denominator += weight/((*iTrack)->pt());
			}
			else if(sumType.compare("HarmWPtAvg") == 0)
			{
				if( (*iTrack)->pt()*weight != 0 )
					denominator += 1/((*iTrack)->pt()*weight);
			}
			else // "Pt2"
			{
				denominator += pow((*iTrack).get()->pt(),2);
			}

			elements++;
		}

		if(sumType.compare("HarmPtAvg") == 0)
		{
			if( denominator != 0)
				denominator = elements/denominator;
		}
		else if(sumType.compare("WHarmPtAvg") == 0)
		{
			if( denominator != 0)
				denominator = elements/denominator;
		}
		else if(sumType.compare("HarmWPtAvg") == 0)
		{
			if( denominator != 0)
				denominator = elements/denominator;
		}		
	}
	//Simulated tracks do not have weights
	else if(divider.compare("Sim") == 0)
	{
		unsigned int elements = 0;

		for(auto iTrack = simV.daughterTracks_begin(); iTrack != simV.daughterTracks_end(); ++iTrack) {

			if(sumType.compare("") == 0)
			{
				denominator += 1;
			}
			else if(sumType.compare("W") == 0)
			{
				denominator += 1;
			}
			else if(sumType.compare("Pt") == 0)
			{
				denominator += (*iTrack).get()->pt();
			}
			else if(sumType.compare("WPt") == 0)
			{
				denominator += (*iTrack).get()->pt();
			}
			else if(sumType.compare("Pt2") == 0)
			{
				denominator += pow((*iTrack).get()->pt(),2);
			}
			else if(sumType.compare("WPt2") == 0)
			{
				denominator += pow((*iTrack).get()->pt(),2);
			}
			else if(sumType.compare("HarmPt") == 0)
			{
				if( (*iTrack)->pt() != 0 )
					denominator += 1/((*iTrack)->pt());
			}
			else if(sumType.compare("WHarmPt") == 0)
			{
				if( (*iTrack)->pt() != 0 )
					denominator += 1/((*iTrack)->pt());
			}
			else if(sumType.compare("HarmWPt") == 0)
			{
				if( (*iTrack)->pt() != 0 )
					denominator += 1/((*iTrack)->pt());
			}
			else if(sumType.compare("HarmPtAvg") == 0)
			{
				if( (*iTrack)->pt() != 0 )
					denominator += 1/((*iTrack)->pt());
			}
			else if(sumType.compare("WHarmPtAvg") == 0)
			{
				if( (*iTrack)->pt() != 0 )
					denominator += 1/((*iTrack)->pt());
			}
			else if(sumType.compare("HarmWPtAvg") == 0)
			{
				if( (*iTrack)->pt() != 0 )
					denominator += 1/((*iTrack)->pt());
			}
			else // "Pt2"
			{
				denominator += pow((*iTrack).get()->pt(),2);
			}

			elements++;
		}

		if(sumType.compare("HarmPtAvg") == 0)
		{
			if( denominator != 0)
				denominator = elements/denominator;
		}
		else if(sumType.compare("WHarmPtAvg") == 0)
		{
			if( denominator != 0)
				denominator = elements/denominator;
		}
		else if(sumType.compare("HarmWPtAvg") == 0)
		{
			if( denominator != 0)
				denominator = elements/denominator;
		}
	}
	else if(divider.compare("SimMatched") == 0)
	{
		unsigned int elements = 0;

		for(auto iTrack = simV.daughterTracks_begin(); iTrack != simV.daughterTracks_end(); ++iTrack) {
			auto found = trackSimToRecoAssociation.find(*iTrack);

			if(found == trackSimToRecoAssociation.end())
				continue;

			if(sumType.compare("") == 0)
			{
				denominator += 1;
			}
			else if(sumType.compare("W") == 0)
			{
				denominator += 1;
			}
			else if(sumType.compare("Pt") == 0)
			{
				denominator += (*iTrack).get()->pt();
			}
			else if(sumType.compare("WPt") == 0)
			{
				denominator += (*iTrack).get()->pt();
			}
			else if(sumType.compare("Pt2") == 0)
			{
				denominator += pow((*iTrack).get()->pt(),2);
			}
			else if(sumType.compare("WPt2") == 0)
			{
				denominator += pow((*iTrack).get()->pt(),2);
			}
			else if(sumType.compare("HarmPt") == 0)
			{
				if( (*iTrack)->pt() != 0 )
					denominator += 1/((*iTrack)->pt());
			}
			else if(sumType.compare("WHarmPt") == 0)
			{
				if( (*iTrack)->pt() != 0 )
					denominator += 1/((*iTrack)->pt());
			}
			else if(sumType.compare("HarmWPt") == 0)
			{
				if( (*iTrack)->pt() != 0 )
					denominator += 1/((*iTrack)->pt());
			}
			else if(sumType.compare("HarmPtAvg") == 0)
			{
				if( (*iTrack)->pt() != 0 )
					denominator += 1/((*iTrack)->pt());
			}
			else if(sumType.compare("WHarmPtAvg") == 0)
			{
				if( (*iTrack)->pt() != 0 )
					denominator += 1/((*iTrack)->pt());
			}
			else if(sumType.compare("HarmWPtAvg") == 0)
			{
				if( (*iTrack)->pt() != 0 )
					denominator += 1/((*iTrack)->pt());
			}
			else // "Pt2"
			{
				denominator += pow((*iTrack).get()->pt(),2);
			}
			
			elements++;
		}		

		if(sumType.compare("HarmPtAvg") == 0)
			{
				if( denominator != 0)
					denominator = elements/denominator;
			}
			else if(sumType.compare("WHarmPtAvg") == 0)
			{
				if( denominator != 0)
					denominator = elements/denominator;
			}
			else if(sumType.compare("HarmWPtAvg") == 0)
			{
				if( denominator != 0)
					denominator = elements/denominator;
			}
	}
	else //Calc "Reco"
	{
		unsigned int elements = 0;
		for(auto iTrack = recoV.tracks_begin(); iTrack != recoV.tracks_end(); ++iTrack) {
			float weight = recoV.trackWeight(*iTrack);

			if(sumType.compare("") == 0)
			{
				denominator += 1;
			}
			else if(sumType.compare("W") == 0)
			{
				denominator += weight;
			}
			else if(sumType.compare("Pt") == 0)
			{
				denominator += (*iTrack).get()->pt();
			}
			else if(sumType.compare("WPt") == 0)
			{
				denominator += (*iTrack).get()->pt()*weight;
			}
			else if(sumType.compare("Pt2") == 0)
			{
				denominator += pow((*iTrack).get()->pt(),2);
			}
			else if(sumType.compare("WPt2") == 0)
			{
				denominator += pow((*iTrack).get()->pt(),2)*weight;
			}
			else if(sumType.compare("HarmPt") == 0)
			{
				if( (*iTrack)->pt() != 0 )
					denominator += 1/((*iTrack)->pt());
			}
			else if(sumType.compare("WHarmPt") == 0)
			{
				if( (*iTrack)->pt() != 0 )
					denominator += weight/((*iTrack)->pt());
			}
			else if(sumType.compare("HarmWPt") == 0)
			{
				if( (*iTrack)->pt()*weight != 0 )
				denominator += 1/((*iTrack)->pt()*weight);
			}
			else if(sumType.compare("HarmPtAvg") == 0)
			{
				if( (*iTrack)->pt() != 0 )
					denominator += 1/((*iTrack)->pt());
			}
			else if(sumType.compare("WHarmPtAvg") == 0)
			{
				if( (*iTrack)->pt() != 0 )
					denominator += weight/((*iTrack)->pt());
			}
			else if(sumType.compare("HarmWPtAvg") == 0)
			{
				if( (*iTrack)->pt()*weight != 0 )
					denominator += 1/((*iTrack)->pt()*weight);
			}
			else // "Pt2"
			{
				denominator += pow((*iTrack).get()->pt(),2);
			}

			elements++;
		}

		if(sumType.compare("HarmPtAvg") == 0)
		{
			if(denominator != 0)
				denominator = elements/denominator;
		}
		else if(sumType.compare("WHarmPtAvg") == 0)
		{
			if( denominator != 0)
				denominator = elements/denominator;
		}
		else if(sumType.compare("HarmWPtAvg") == 0)
		{
			if( denominator != 0)
				denominator = elements/denominator;
		}
	}
	if(denominator == 0)
		momentumFraction = 0;
	else
		momentumFraction = (double)sharedMomentumSum/denominator;
  return momentumFraction;
}


// calculations for SimAllAssoc2Reco- plottings
// simulated tracks do not have weights
double calculateVertexSharedTracksMomentumFraction(const TrackingVertex& simV, const reco::Vertex& recoV, const reco::SimToRecoCollection& trackSimToRecoAssociation, const reco::RecoToSimCollection& trackRecoToSimAssociation, std::string sumType, std::string divider) {
  double sharedMomentumSum = 0;
	double denominator = 0;
	double momentumFraction = 0;

	if(sumType.compare("") == 0)
		sharedMomentumSum = calculateVertexSharedTracks(simV,recoV, trackSimToRecoAssociation);
	
	else if(sumType.compare("Pt") == 0) {
		for(auto iTrack = simV.daughterTracks_begin(); iTrack != simV.daughterTracks_end(); ++iTrack) {

			auto found = trackSimToRecoAssociation.find(*iTrack);

			if(found == trackSimToRecoAssociation.end())
				continue;

			
			for(const auto& tp: found->val) {
				
				
				for(auto iReco = recoV.tracks_begin(); iReco != recoV.tracks_end(); iReco++)	{
					if( iReco->id() == tp.first.id() )	{
						if( iReco->key() == tp.first.key() )	{
							sharedMomentumSum += tp.first.get()->pt();
							break;
						}
					}
				}
				
				
				/*
				if(std::find_if(recoV.tracks_begin(), recoV.tracks_end(), [&](const reco::TrackBaseRef& vtp) {
							return tp.first == vtp;
						}) != recoV.tracks_end()) {
					sharedMomentumSum += tp.first.get()->pt();
				}
				*/
				//std::cout << "tempSum " << tempSum << std::endl;
				std::cout << "sharedMomentumSum " << sharedMomentumSum << std::endl;
				//std::cout << "tempSum " << tempSum << std::endl;
			}									
		}
	}
	else if(sumType.compare("Pt2") == 0) {
		for(auto iTrack = simV.daughterTracks_begin(); iTrack != simV.daughterTracks_end(); ++iTrack) {

			auto found = trackSimToRecoAssociation.find(*iTrack);

			if(found == trackSimToRecoAssociation.end())
				continue;

			
			for(const auto& tp: found->val) {
				
				/*
				if(std::find_if(recoV.tracks_begin(), recoV.tracks_end(), [&](const reco::TrackBaseRef& vtp) {
							return tp.first == vtp;
						}) != recoV.tracks_end()) {
					sharedMomentumSum += pow((*(tp.first)).pt(),2);
				}
				*/
				
				for(auto iReco = recoV.tracks_begin(); iReco != recoV.tracks_end(); iReco++)	{
					if( iReco->id() == tp.first.id() )	{
						if( iReco->key() == tp.first.key() )	{
							sharedMomentumSum += pow(tp.first.get()->pt(),2);
							break;
						}
						
					}
				}
				
			}									
		}
	}

	else //"Pt2"
 {
		for(auto iTrack = simV.daughterTracks_begin(); iTrack != simV.daughterTracks_end(); ++iTrack) {

			auto found = trackSimToRecoAssociation.find(*iTrack);

			if(found == trackSimToRecoAssociation.end())
				continue;

			
			for(const auto& tp: found->val) {
				/*
				if(std::find_if(recoV.tracks_begin(), recoV.tracks_end(), [&](const reco::TrackBaseRef& vtp) {
							return tp.first == vtp;
						}) != recoV.tracks_end()) {
					sharedMomentumSum += pow((*(tp.first)).pt(),2);
				}
				*/
				
				for(auto iReco = recoV.tracks_begin(); iReco != recoV.tracks_end(); iReco++)	{
					if( iReco->id() == tp.first.id() )	{
						if( iReco->key() == tp.first.key() )	{
							sharedMomentumSum += pow(tp.first.get()->pt(),2);
							break;
						}
					}
				}
				
				
			}									
		}
		for(auto iTrack = simV.sourceTracks_begin(); iTrack != simV.sourceTracks_end(); ++iTrack) {

			auto found = trackSimToRecoAssociation.find(*iTrack);

			if(found == trackSimToRecoAssociation.end())
				continue;

			
			for(const auto& tp: found->val) {
				/*
				if(std::find_if(recoV.tracks_begin(), recoV.tracks_end(), [&](const reco::TrackBaseRef& vtp) {
							return tp.first == vtp;
						}) != recoV.tracks_end()) {
					sharedMomentumSum += pow((*(tp.first)).pt(),2);
				}
				*/
				
				for(auto iReco = recoV.tracks_begin(); iReco != recoV.tracks_end(); iReco++)	{
					if( iReco->id() == tp.first.id() )	{
						if( iReco->key() == tp.first.key() )	{
							sharedMomentumSum += pow(tp.first.get()->pt(),2);
							break;
						}
					}
				}
				
				
			}									
		}
	}

	//calculate the divider in the fraction
	if(divider.compare("Reco") == 0)
	{
		for(auto iTrack = recoV.tracks_begin(); iTrack != recoV.tracks_end(); ++iTrack)	{

			float weight = recoV.trackWeight(*iTrack);

			if(sumType.compare("") == 0)
			{
				denominator += 1;
			}
			else if(sumType.compare("W") == 0)
			{
				denominator += weight;
			}
			else if(sumType.compare("Pt") == 0)
			{
				denominator += (*iTrack)->pt();
			}
			else if(sumType.compare("WPt") == 0)
			{
				denominator += (*iTrack)->pt()*weight;
			}
			else if(sumType.compare("Pt2") == 0)
			{
				denominator += pow((*iTrack)->pt(),2);
			}
			else if(sumType.compare("WPt2") == 0)
			{
				denominator += pow((*iTrack)->pt(),2)*weight;
			}
			else if(sumType.compare("HarmPt") == 0)
			{
				if( (*iTrack)->pt() != 0 )
					denominator += 1/((*iTrack)->pt());
			}
			else if(sumType.compare("WHarmPt") == 0)
			{
				if( (*iTrack)->pt() != 0 )
					denominator += weight/((*iTrack)->pt());
			}
			else if(sumType.compare("HarmWPt") == 0)
			{
				if( (*iTrack)->pt()*weight != 0 )
					denominator += 1/((*iTrack)->pt()*weight);
			}
			else // "Pt2"
			{
				denominator += pow((*iTrack)->pt(),2);
			}
		}
	}
	else if(divider.compare("RecoMatched") == 0)
	{
		for(auto iTrack = recoV.tracks_begin(); iTrack != recoV.tracks_end(); ++iTrack) {
			float weight = recoV.trackWeight(*iTrack);

			auto found = trackRecoToSimAssociation.find(*iTrack);

			if(found == trackRecoToSimAssociation.end())
							continue;

			if(sumType.compare("") == 0)
			{
				denominator += 1;
			}
			else if(sumType.compare("W") == 0)
			{
				denominator += weight;
			}
			else if(sumType.compare("Pt") == 0)
			{
				denominator += (*iTrack).get()->pt();
			}
			else if(sumType.compare("WPt") == 0)
			{
				denominator += (*iTrack).get()->pt()*weight;
			}
			else if(sumType.compare("Pt2") == 0)
			{
				denominator += pow((*iTrack).get()->pt(),2);
			}
			else if(sumType.compare("WPt2") == 0)
			{
				denominator += pow((*iTrack).get()->pt(),2)*weight;
			}
			else if(sumType.compare("HarmPt") == 0)
			{
				if( (*iTrack)->pt() != 0 )
					denominator += 1/((*iTrack)->pt());
			}
			else if(sumType.compare("WHarmPt") == 0)
			{
				if( (*iTrack)->pt() != 0 )
					denominator += weight/((*iTrack)->pt());
			}
			else if(sumType.compare("HarmWPt") == 0)
			{
				if( (*iTrack)->pt()*weight != 0 )
					denominator += 1/((*iTrack)->pt()*weight);
			}
			else // "Pt2"
			{
				denominator += pow((*iTrack).get()->pt(),2);
			}
		}		
	}
	//Simulated tracks do not have weights
	else if(divider.compare("Sim") == 0)
	{
		for(auto iTrack = simV.daughterTracks_begin(); iTrack != simV.daughterTracks_end(); ++iTrack) {

			if(sumType.compare("") == 0)
			{
				denominator += 1;
			}
			else if(sumType.compare("W") == 0)
			{
				denominator += 1;
			}
			else if(sumType.compare("Pt") == 0)
			{
				denominator += (*iTrack).get()->pt();
			}
			else if(sumType.compare("Pt2") == 0)
			{
				denominator += pow((*iTrack).get()->pt(),2);
			}
			else if(sumType.compare("HarmPt") == 0)
			{
				if( (*iTrack)->pt() != 0 )
					denominator += 1/((*iTrack)->pt());
			}
			else if(sumType.compare("WHarmPt") == 0)
			{
				if( (*iTrack)->pt() != 0 )
					denominator += 1/((*iTrack)->pt());
			}
			else if(sumType.compare("HarmWPt") == 0)
			{
				if( (*iTrack)->pt() != 0 )
					denominator += 1/((*iTrack)->pt());
			}
			else // "Pt2"
			{
				denominator += pow((*iTrack).get()->pt(),2);
			}
		}
	}
	else if(divider.compare("SimMatched") == 0)
	{
		for(auto iTrack = simV.daughterTracks_begin(); iTrack != simV.daughterTracks_end(); ++iTrack) {
			auto found = trackSimToRecoAssociation.find(*iTrack);

			if(found == trackSimToRecoAssociation.end())
				continue;

			if(sumType.compare("") == 0)
			{
				denominator += 1;
			}
			else if(sumType.compare("W") == 0)
			{
				denominator += 1;
			}
			else if(sumType.compare("Pt") == 0)
			{
				denominator += (*iTrack).get()->pt();
			}
			else if(sumType.compare("Pt2") == 0)
			{
				denominator += pow((*iTrack).get()->pt(),2);
			}
			else if(sumType.compare("HarmPt") == 0)
			{
				if( (*iTrack)->pt() != 0 )
					denominator += 1/((*iTrack)->pt());
			}
			else if(sumType.compare("WHarmPt") == 0)
			{
				if( (*iTrack)->pt() != 0 )
					denominator += 1/((*iTrack)->pt());
			}
			else if(sumType.compare("HarmWPt") == 0)
			{
				if( (*iTrack)->pt() != 0 )
					denominator += 1/((*iTrack)->pt());
			}
			else // "Pt2"
			{
				denominator += pow((*iTrack).get()->pt(),2);
			}
		}		
	}
	else //Calc "Reco"
	{
		for(auto iTrack = recoV.tracks_begin(); iTrack != recoV.tracks_end(); ++iTrack) {
			float weight = recoV.trackWeight(*iTrack);

			if(sumType.compare("") == 0)
			{
				denominator += 1;
			}
			else if(sumType.compare("W") == 0)
			{
				denominator += weight;
			}
			else if(sumType.compare("Pt") == 0)
			{
				denominator += (*iTrack).get()->pt();
			}
			else if(sumType.compare("WPt") == 0)
			{
				denominator += (*iTrack).get()->pt()*weight;
			}
			else if(sumType.compare("Pt2") == 0)
			{
				denominator += pow((*iTrack).get()->pt(),2);
			}
			else if(sumType.compare("WPt2") == 0)
			{
				denominator += pow((*iTrack).get()->pt(),2)*weight;
			}
			else if(sumType.compare("HarmPt") == 0)
			{
				if( (*iTrack)->pt() != 0 )
					denominator += 1/((*iTrack)->pt());
			}
			else if(sumType.compare("WHarmPt") == 0)
			{
				if( (*iTrack)->pt() != 0 )
					denominator += weight/((*iTrack)->pt());
			}
			else if(sumType.compare("HarmWPt") == 0)
			{
				if( (*iTrack)->pt()*weight != 0 )
					denominator += 1/((*iTrack)->pt()*weight);
			}
			else // "Pt2"
			{
				denominator += pow((*iTrack).get()->pt(),2);
			}
		}
	}
	if( denominator == 0)
		momentumFraction = 0;
	else
		momentumFraction = (double)sharedMomentumSum/denominator;
  return momentumFraction;
}
