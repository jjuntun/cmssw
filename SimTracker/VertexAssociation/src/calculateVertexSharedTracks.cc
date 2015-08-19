#include "SimTracker/VertexAssociation/interface/calculateVertexSharedTracks.h"

namespace {
  template <typename T>
    T sqr(T val) { return val*val; }
}

vertexAssociation::TrackFraction calculateVertexSharedTrackFractions(const reco::Vertex& recoV, const TrackingVertex& simV, const reco::SimToRecoCollection& trackSimToRecoAssociation, const reco::RecoToSimCollection& trackRecoToSimAssociation, vertexAssociation::DividerType dividerType) {

  std::vector<double> numerators = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
  
  unsigned int elements = 0;

  //AssociationTypeStringToEnum::NUMBEROFTRACKS)
  unsigned int sharedTracks = 0;
  for(auto iTrack = recoV.tracks_begin(); iTrack != recoV.tracks_end(); ++iTrack) {
    auto found = trackRecoToSimAssociation.find(*iTrack);

    if(found == trackRecoToSimAssociation.end())
      continue;

    // matched TP equal to any TP of sim vertex => increase counter
    for(const auto& tp: found->val) {
      if(std::find_if(simV.daughterTracks_begin(), simV.daughterTracks_end(), [&](const TrackingParticleRef& vtp) {
          return tp.first == vtp;
        }) != simV.daughterTracks_end()) {
        sharedTracks += 1;
        break;
      }
    }

  }
  numerators[0] = sharedTracks;

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

        double pt = (*iTrack).get()->pt();
        
        //AssociationTypeStringToEnum::Weighted
        numerators[1] += weight;
        //AssociationTypeStringToEnum::PT
        numerators[2] += pt;
        //AssociationTypeStringToEnum::WPT
        numerators[3] += pt*weight;
        //AssociationTypeStringToEnum::PT2
        numerators[4] += sqr(pt);  
        //AssociationTypeStringToEnum::WPT2
        numerators[5] += sqr(pt)*weight;
        //AssociationTypeStringToEnum::HARMPT
        numerators[6] += 1/pt;
        //AssociationTypeStringToEnum::WHARMPT
        if( pt != 0 )
          numerators[7] += weight/pt;
        if( pt*weight != 0 )
        {
          //AssociationTypeStringToEnum::HARMWPT
          numerators[8] += 1/(pt*weight);
          //AssociationTypeStringToEnum::HARMPTAVG
          numerators[9] += 1/(pt*weight);
          //AssociationTypeStringToEnum::WHARMPTAVG
          numerators[10] += 1/(pt*weight);
          //AssociationTypeStringToEnum::HARMWPTAVG
          numerators[11] += 1/(pt*weight);
        }

        elements++;
        break;
      }
    }
  }


  if( numerators[9] != 0)
    numerators[9] = elements/numerators[9];

  if( numerators[10] != 0)
    numerators[10] = elements/numerators[10];
  
  if( numerators[11] != 0)
    numerators[11] = elements/numerators[11];


  // denominators are values for NumberOfTracks, Weighted, Pt, WPt, Pt2, WPt2, HarmPt, WHarmPt, HarmWPt, HarmPtAvg, WHarmPtAvg, HarmWPtAvg
  std::vector<double> denominators = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

  //calculate the divider in the fraction
  if( dividerType == vertexAssociation::DividerType::Reco )
  {
    //"Reco"
    
    unsigned int elements = 0;
    for(auto iTrack = recoV.tracks_begin(); iTrack != recoV.tracks_end(); ++iTrack)  {

      double pt = (*iTrack)->pt();
      float weight = recoV.trackWeight(*iTrack);
      //AssociationTypeStringToEnum::NumberOfTracks
      denominators[0] += 1;

      //AssociationTypeStringToEnum::Weighted
      denominators[1] += weight;

      //AssociationTypeStringToEnum::Pt
      denominators[2] += pt;

      //AssociationTypeStringToEnum::WPt
      denominators[3] += pt*weight;

      //AssociationTypeStringToEnum::Pt2
      denominators[4] += sqr(pt);

      //AssociationTypeStringToEnum::WPt2
      denominators[5] += sqr(pt)*weight;

      //AssociationTypeStringToEnum::HarmPt
      if( pt != 0 )
        denominators[6] += 1/pt;


      //AssociationTypeStringToEnum::WHarmPt
      if( pt != 0 )
        denominators[7] += weight/pt;

      //AssociationTypeStringToEnum::HarmWPt
      if( pt*weight != 0 )
        denominators[8] += 1/(pt*weight);

      //AssociationTypeStringToEnum::HarmPtAvg
      if( pt != 0 )
        denominators[9] += 1/(pt);

      //AssociationTypeStringToEnum::WHarmPtAvg
      if( pt != 0 )
        denominators[10] += weight/pt;

      //AssociationTypeStringToEnum::HarmWPtAvg
      if( pt*weight != 0 )
        denominators[11] += 1/(pt*weight);

      elements++;
    }

    //AssociationTypeStringToEnum::HarmPtAvg
    if( denominators[9] != 0)
      denominators[9] = elements/denominators[9];

    //AssociationTypeStringToEnum::WHarmPtAvg
    if( denominators[10] != 0)
      denominators[10] = elements/denominators[10];

    //AssociationTypeStringToEnum::HarmWPtAvg
    if( denominators[11] != 0)
      denominators[11] = elements/denominators[11];

  }
  else if( dividerType == vertexAssociation::DividerType::RecoMatched )
  {
    //"RecoMatched"

    elements = 0;

    for(auto iTrack = recoV.tracks_begin(); iTrack != recoV.tracks_end(); ++iTrack) {

      double pt = (*iTrack)->pt();
      float weight = recoV.trackWeight(*iTrack);

      auto found = trackRecoToSimAssociation.find(*iTrack);

      if(found == trackRecoToSimAssociation.end())
        continue;

     //AssociationTypeStringToEnum::NumberOfTracks
      denominators[0] += 1;

      //AssociationTypeStringToEnum::Weighted
      denominators[1] += weight;

      //AssociationTypeStringToEnum::Pt
      denominators[2] += pt;

      //AssociationTypeStringToEnum::WPt
      denominators[3] += pt*weight;

      //AssociationTypeStringToEnum::Pt2
      denominators[4] += sqr(pt);

      //AssociationTypeStringToEnum::WPt2
      denominators[5] += sqr(pt)*weight;

      //AssociationTypeStringToEnum::HarmPt
      if( pt != 0 )
        denominators[6] += 1/pt;

      //AssociationTypeStringToEnum::WHarmPt
      if( (*iTrack)->pt() != 0 )
        denominators[7] += weight/pt;

      //AssociationTypeStringToEnum::HarmWPt
      if( (*iTrack)->pt()*weight != 0 )
        denominators[8] += 1/(pt*weight);

      //AssociationTypeStringToEnum::HarmPtAvg
      if( (*iTrack)->pt() != 0 )
        denominators[9] += 1/pt;

      //AssociationTypeStringToEnum::WHarmPtAvg
      if( (*iTrack)->pt() != 0 )
        denominators[10] += weight/pt;

      //AssociationTypeStringToEnum::HarmWPtAvg
      if( (*iTrack)->pt()*weight != 0 )
        denominators[11] += 1/(pt*weight);


      elements++;
    }

    //AssociationTypeStringToEnum::HarmPtAvg
    if( denominators[9] != 0)
      denominators[9] = elements/denominators[9];

    //AssociationTypeStringToEnum::WHarmPtAvg
    if( denominators[10] != 0)
      denominators[10] = elements/denominators[10];

    //AssociationTypeStringToEnum::HarmWPtAvg
    if( denominators[11] != 0)
      denominators[11] = elements/denominators[11];

  }
  else if( dividerType == vertexAssociation::DividerType::Sim )
  {
    //Simulated tracks do not have weights
    //"Sim"

    for(auto iTrack = simV.daughterTracks_begin(); iTrack != simV.daughterTracks_end(); ++iTrack) {

      double pt = (*iTrack).get()->pt();

      //AssociationTypeStringToEnum::NumberOfTracks
      denominators[0] += 1;

      //AssociationTypeStringToEnum::Weighted
      denominators[1] += 1;

      //AssociationTypeStringToEnum::Pt
      denominators[2] += pt;

      //AssociationTypeStringToEnum::WPt
      denominators[3] += pt;

      //AssociationTypeStringToEnum::Pt2
      denominators[4] += sqr(pt);

      //AssociationTypeStringToEnum::WPt2
      denominators[5] += sqr(pt);

      //AssociationTypeStringToEnum::HarmPt
      if( pt != 0 )
        denominators[6] += 1/pt;

      //AssociationTypeStringToEnum::WHarmPt
      if( pt != 0 )
        denominators[7] += 1/pt;

      //AssociationTypeStringToEnum::HarmWPt
      if( pt != 0 )
        denominators[8] += 1/pt;

      //AssociationTypeStringToEnum::HarmPtAvg
      if( pt != 0 )
        denominators[9] += 1/pt;

      //AssociationTypeStringToEnum::WHarmPtAvg
      if( pt != 0 )
        denominators[10] += 1/pt;

      //AssociationTypeStringToEnum::HarmWPtAvg
      if( pt != 0 )
        denominators[11] += 1/pt;

      elements++;
    }

    //AssociationTypeStringToEnum::HarmPtAvg
    if( denominators[9] != 0)
      denominators[9] = elements/denominators[9];

    //AssociationTypeStringToEnum::WHarmPtAvg
    if( denominators[10] != 0)
      denominators[10] = elements/denominators[10];

    //AssociationTypeStringToEnum::HarmWPtAvg
    if( denominators[11] != 0)
      denominators[11] = elements/denominators[11];

  }
  else
  {
    //SimMatched
    for(auto iTrack = simV.daughterTracks_begin(); iTrack != simV.daughterTracks_end(); ++iTrack) {
      auto found = trackSimToRecoAssociation.find(*iTrack);

      if(found == trackSimToRecoAssociation.end())
        continue;

      double pt = (*iTrack).get()->pt();

      //AssociationTypeStringToEnum::NumberOfTracks
      denominators[0] += 1;

      //AssociationTypeStringToEnum::Weighted
      denominators[1] += 1;

      //AssociationTypeStringToEnum::Pt
      denominators[2] += pt;

      //AssociationTypeStringToEnum::WPt
      denominators[3] += pt;

      //AssociationTypeStringToEnum::Pt2
      denominators[4] += sqr(pt);

      //AssociationTypeStringToEnum::WPt2
      denominators[5] += sqr(pt);

      //AssociationTypeStringToEnum::HarmPt
      if( pt != 0 )
        denominators[6] += 1/pt;

      //AssociationTypeStringToEnum::WHarmPt
      if( pt != 0 )
        denominators[7] += 1/pt;

      //AssociationTypeStringToEnum::HarmWPt
      if( pt != 0 )
        denominators[8] += 1/pt;

      //AssociationTypeStringToEnum::HarmPtAvg
      if( pt != 0 )
        denominators[9] += 1/pt;

      //AssociationTypeStringToEnum::WHarmPtAvg
      if( pt != 0 )
        denominators[10] += 1/pt;

      //AssociationTypeStringToEnum::HarmWPtAvg
      if( pt != 0 )
        denominators[11] += 1/pt;
      
      elements++;
    }    

    //AssociationTypeStringToEnum::HarmPtAvg
    if( denominators[9] != 0)
      denominators[9] = elements/denominators[9];

    //AssociationTypeStringToEnum::WHarmPtAvg
    if( denominators[10] != 0)
      denominators[10] = elements/denominators[10];

    //AssociationTypeStringToEnum::HarmWPtAvg
    if( denominators[11] != 0)
      denominators[11] = elements/denominators[11];

  }


  vertexAssociation::TrackFraction tr;
  //Reco or RecoMatched
  if( dividerType == vertexAssociation::DividerType::Reco ||
      dividerType == vertexAssociation::DividerType::RecoMatched )
  {
    //Tracks
    tr.fractions[0] = numerators[0]/denominators[0];
    //Weighted
    tr.fractions[1] = numerators[0]/denominators[1];
    //Pt
    tr.fractions[2] = numerators[1]/denominators[2];
    //WPt
	  tr.fractions[3] = numerators[1]/denominators[3];
    //Pt2
	  tr.fractions[4] = numerators[2]/denominators[4];
    //WPt2
	  tr.fractions[5] = numerators[2]/denominators[5];
	  //HarmPt
    tr.fractions[6] = numerators[6]/denominators[6];
	  //WHarmPt
    tr.fractions[7] = numerators[7]/denominators[7];
	  //HarmWPt
    tr.fractions[8] = numerators[8]/denominators[8];
	  //HarmPtAvg
    tr.fractions[9] = numerators[9]/denominators[9];
	  //WHarmPtAvg
    tr.fractions[10] = numerators[10]/denominators[10];
	  //HarmWPtAvg
    tr.fractions[11] = numerators[11]/denominators[11];
  }
  //Sim or SimMatched
  else
  {
    //Tracks
    tr.fractions[0] = numerators[0]/denominators[0];
    //W
    tr.fractions[1] = numerators[1]/denominators[1];
    //Pt
    tr.fractions[2] = numerators[2]/denominators[2];
    //WPt
    tr.fractions[3] = numerators[3]/denominators[3];
    //Pt2
	  tr.fractions[4] = numerators[4]/denominators[4];
	  //WPt2
    tr.fractions[5] = numerators[5]/denominators[5];
  }
  return tr;

}


// calculations for SimAllAssoc2Reco- plottings
// simulated tracks do not have weights
/*
std::vector<double> calculateVertexSharedTrackFractions(const TrackingVertex& simV, const reco::Vertex& recoV, const reco::SimToRecoCollection& trackSimToRecoAssociation, const reco::RecoToSimCollection& trackRecoToSimAssociation, vertexAssociation::DividerType dividerType) {

  std::vector<double> nominators = { 0, 0, 0 };

  //AssociationTypeStringToEnum::NumberOfTracks)

  unsigned int sharedTracks = 0;

  for(auto iTP = simV.daughterTracks_begin(); iTP != simV.daughterTracks_end(); ++iTP) {
    auto found = trackSimToRecoAssociation.find(*iTP);

    if(found == trackSimToRecoAssociation.end())
      continue;

    // matched track equal to any track of reco vertex => increase counter
    for(const auto& tk: found->val) {
      if(std::find_if(recoV.tracks_begin(), recoV.tracks_end(), [&](const reco::TrackBaseRef& vtk) {
          return tk.first.id() == vtk.id() && tk.first.key() == vtk.key();
        }) != recoV.tracks_end()) {
        sharedTracks += 1;
        break;
      }
    }
  }

  nominators[0] = sharedTracks;

  for(auto iTrack = simV.daughterTracks_begin(); iTrack != simV.daughterTracks_end(); ++iTrack) {

    auto found = trackSimToRecoAssociation.find(*iTrack);

    if(found == trackSimToRecoAssociation.end())
      continue;

    
    for(const auto& tp: found->val) {
      
      
      for(auto iReco = recoV.tracks_begin(); iReco != recoV.tracks_end(); iReco++)  {
        if( iReco->id() == tp.first.id() )  {
          if( iReco->key() == tp.first.key() )  {

            double pt = tp.first.get()->pt();
            //AssociationTypeStringToEnum::Pt
            nominators[1] += pt;
            //AssociationTypeStringToEnum::Pt2
            nominators[2] += sqr(pt);
            break;
          }
        }
      }
      
    }                  
  }

  return nominators;
}
*/



vertexAssociation::TrackFraction calculateVertexSharedTrackFractions(const TrackingVertex& simV, const reco::Vertex& recoV, const reco::SimToRecoCollection& trackSimToRecoAssociation, const reco::RecoToSimCollection& trackRecoToSimAssociation, vertexAssociation::DividerType dividerType) {

  std::vector<double> numerators = { 0, 0, 0 };

  //AssociationTypeStringToEnum::NumberOfTracks)
  unsigned int sharedTracks = 0;

  for(auto iTP = simV.daughterTracks_begin(); iTP != simV.daughterTracks_end(); ++iTP) {
    auto found = trackSimToRecoAssociation.find(*iTP);

    if(found == trackSimToRecoAssociation.end())
      continue;

    // matched track equal to any track of reco vertex => increase counter
    for(const auto& tk: found->val) {
      if(std::find_if(recoV.tracks_begin(), recoV.tracks_end(), [&](const reco::TrackBaseRef& vtk) {
          return tk.first.id() == vtk.id() && tk.first.key() == vtk.key();
        }) != recoV.tracks_end()) {
        sharedTracks += 1;
        break;
      }
    }
  }

  numerators[0] = sharedTracks;

  for(auto iTrack = simV.daughterTracks_begin(); iTrack != simV.daughterTracks_end(); ++iTrack) {

    auto found = trackSimToRecoAssociation.find(*iTrack);

    if(found == trackSimToRecoAssociation.end())
      continue;

    
    for(const auto& tp: found->val) {
      
      
      for(auto iReco = recoV.tracks_begin(); iReco != recoV.tracks_end(); iReco++)  {
        if( iReco->id() == tp.first.id() )  {
          if( iReco->key() == tp.first.key() )  {

            double pt = tp.first.get()->pt();
            //AssociationTypeStringToEnum::Pt
            numerators[1] += pt;
            //AssociationTypeStringToEnum::Pt2
            numerators[2] += sqr(pt);
            break;
          }
        }
      }
      
    }                  
  }


  // denominators are values for NumberOfTracks, W, Pt, WPt, Pt2, WPt2, HarmPt, WHarmPt, HarmWPt, HarmPtAvg, WHarmPtAvg, HarmWPtAvg
  std::vector<double> denominators;
  // groups are Reco, RecoMatched, Sim, SimMatched
  std::vector<std::vector<double>> denominatorGroups;

  denominators = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

  //calculate the divider in the fraction
  if( dividerType == vertexAssociation::DividerType::Reco )
  {
    //"Reco"
    unsigned int elements = 0;
    for(auto iTrack = recoV.tracks_begin(); iTrack != recoV.tracks_end(); ++iTrack)  {

      double pt = (*iTrack)->pt();
      float weight = recoV.trackWeight(*iTrack);

      //AssociationTypeStringToEnum::NumberOfTracks
      denominators[0] += 1;

      //AssociationTypeStringToEnum::Weighted
      denominators[1] += weight;

      //AssociationTypeStringToEnum::Pt
      denominators[2] += pt;

      //AssociationTypeStringToEnum::WPt
      denominators[3] += pt*weight;

      //AssociationTypeStringToEnum::Pt2
      denominators[4] += sqr(pt);

      //AssociationTypeStringToEnum::WPt2
      denominators[5] += sqr(pt)*weight;

      //AssociationTypeStringToEnum::HarmPt
      if( pt != 0 )
        denominators[6] += 1/pt;

      //AssociationTypeStringToEnum::WHarmPt
      if( pt != 0 )
        denominators[7] += weight/pt;

      //AssociationTypeStringToEnum::HarmWPt
      if( pt*weight != 0 )
        denominators[8] += 1/(pt*weight);

      //AssociationTypeStringToEnum::HarmPtAvg
      if( pt != 0 )
        denominators[9] += 1/pt;

      //AssociationTypeStringToEnum::WHarmPtAvg
      if( pt != 0 )
        denominators[10] += 1/pt;

      //AssociationTypeStringToEnum::HarmWPtAvg
      if( pt != 0 )
        denominators[11] += 1/pt;
      
      elements++;
    }    

    //AssociationTypeStringToEnum::HarmPtAvg
    if( denominators[9] != 0)
      denominators[9] = elements/denominators[9];

    //AssociationTypeStringToEnum::WHarmPtAvg
    if( denominators[10] != 0)
      denominators[10] = elements/denominators[10];

    //AssociationTypeStringToEnum::HarmWPtAvg
    if( denominators[11] != 0)
      denominators[11] = elements/denominators[11];

  }
  else if( dividerType == vertexAssociation::DividerType::RecoMatched )
  {
    //"RecoMatched"
    unsigned int elements = 0;
    for(auto iTrack = recoV.tracks_begin(); iTrack != recoV.tracks_end(); ++iTrack) {

      double pt = (*iTrack).get()->pt();
      float weight = recoV.trackWeight(*iTrack);

      auto found = trackRecoToSimAssociation.find(*iTrack);

      if(found == trackRecoToSimAssociation.end())
              continue;

      //AssociationTypeStringToEnum::NumberOfTracks
      denominators[0] += 1;

      //AssociationTypeStringToEnum::Weighted
      denominators[1] += weight;

      //AssociationTypeStringToEnum::Pt
      denominators[2] += pt;

      //AssociationTypeStringToEnum::WPt
      denominators[3] += pt*weight;

      //AssociationTypeStringToEnum::Pt2
      denominators[4] += sqr(pt);

      //AssociationTypeStringToEnum::WPt2
      denominators[5] += sqr(pt)*weight;

      //AssociationTypeStringToEnum::HarmPt
      if( pt != 0 )
        denominators[6] += 1/pt;

      //AssociationTypeStringToEnum::WHarmPt
      if( pt != 0 )
        denominators[7] += weight/pt;

      //AssociationTypeStringToEnum::HarmWPt
      if( pt*weight != 0 )
        denominators[8] += 1/(pt*weight);

      //AssociationTypeStringToEnum::HarmPtAvg
      if( pt != 0 )
        denominators[9] += 1/pt;

      //AssociationTypeStringToEnum::WHarmPtAvg
      if( pt != 0 )
        denominators[10] += 1/pt;

      //AssociationTypeStringToEnum::HarmWPtAvg
      if( pt != 0 )
        denominators[11] += 1/pt;
      
      elements++;
    }    

    //AssociationTypeStringToEnum::HarmPtAvg
    if( denominators[9] != 0)
      denominators[9] = elements/denominators[9];

    //AssociationTypeStringToEnum::WHarmPtAvg
    if( denominators[10] != 0)
      denominators[10] = elements/denominators[10];

    //AssociationTypeStringToEnum::HarmWPtAvg
    if( denominators[11] != 0)
      denominators[11] = elements/denominators[11];    

  }
  else if( dividerType == vertexAssociation::DividerType::Sim )
  {
    //Simulated tracks do not have weights
    //"Sim"
    unsigned int elements = 0;
    for(auto iTrack = simV.daughterTracks_begin(); iTrack != simV.daughterTracks_end(); ++iTrack) {

      double pt = (*iTrack).get()->pt();

      //AssociationTypeStringToEnum::NumberOfTracks
      denominators[0] += 1;

      //AssociationTypeStringToEnum::Weighted
      denominators[1] += 1;

      //AssociationTypeStringToEnum::Pt
      denominators[2] += pt;

      //AssociationTypeStringToEnum::Pt2
      denominators[3] += sqr(pt);

      //AssociationTypeStringToEnum::HarmPt
      if( pt != 0 )
        denominators[4] += 1/pt;

      //AssociationTypeStringToEnum::WHarmPt
      if( pt != 0 )
        denominators[5] += 1/pt;

      //AssociationTypeStringToEnum::HarmWPt
      if( pt != 0 )
        denominators[6] += 1/pt;

      //AssociationTypeStringToEnum::HarmPtAvg
      if( pt != 0 )
        denominators[9] += 1/pt;

      //AssociationTypeStringToEnum::WHarmPtAvg
      if( pt != 0 )
        denominators[10] += 1/pt;

      //AssociationTypeStringToEnum::HarmWPtAvg
      if( pt != 0 )
        denominators[11] += 1/pt;
      
      elements++;
    }    

    //AssociationTypeStringToEnum::HarmPtAvg
    if( denominators[9] != 0)
      denominators[9] = elements/denominators[9];

    //AssociationTypeStringToEnum::WHarmPtAvg
    if( denominators[10] != 0)
      denominators[10] = elements/denominators[10];

    //AssociationTypeStringToEnum::HarmWPtAvg
    if( denominators[11] != 0)
      denominators[11] = elements/denominators[11];

  }
  else
  {
    //"SimMatched"
    unsigned int elements = 0;
    for(auto iTrack = simV.daughterTracks_begin(); iTrack != simV.daughterTracks_end(); ++iTrack) {
      auto found = trackSimToRecoAssociation.find(*iTrack);

      if(found == trackSimToRecoAssociation.end())
        continue;

      double pt = (*iTrack).get()->pt();

      //AssociationTypeStringToEnum::NumberOfTracks
      denominators[0] += 1;

      //AssociationTypeStringToEnum::Weighted
      denominators[1] += 1;

      //AssociationTypeStringToEnum::Pt
      denominators[2] += pt;

      //AssociationTypeStringToEnum::Pt2
      denominators[3] += sqr(pt);

      //AssociationTypeStringToEnum::HarmPt
      if( pt != 0 )
        denominators[4] += 1/pt;

      //AssociationTypeStringToEnum::WHarmPt
      if( pt != 0 )
        denominators[5] += 1/pt;

      //AssociationTypeStringToEnum::HarmWPt
      if( pt != 0 )
        denominators[6] += 1/pt;

      //AssociationTypeStringToEnum::HarmPtAvg
      if( pt != 0 )
        denominators[9] += 1/pt;

      //AssociationTypeStringToEnum::WHarmPtAvg
      if( pt != 0 )
        denominators[10] += 1/pt;

      //AssociationTypeStringToEnum::HarmWPtAvg
      if( pt != 0 )
        denominators[11] += 1/pt;
      
      elements++;
    }    

    //AssociationTypeStringToEnum::HarmPtAvg
    if( denominators[9] != 0)
      denominators[9] = elements/denominators[9];

    //AssociationTypeStringToEnum::WHarmPtAvg
    if( denominators[10] != 0)
      denominators[10] = elements/denominators[10];

    //AssociationTypeStringToEnum::HarmWPtAvg
    if( denominators[11] != 0)
      denominators[11] = elements/denominators[11];

  }

  vertexAssociation::TrackFraction tr;
  //Reco or RecoMatched
  if( dividerType == vertexAssociation::DividerType::Reco || 
      dividerType == vertexAssociation::DividerType::RecoMatched )
  {
    //Tracks
    tr.fractions[0] = numerators[0]/denominators[0];
    //Weighted
    tr.fractions[1] = numerators[0]/denominators[1];
    //Pt
    tr.fractions[2] = numerators[1]/denominators[2];
    //WPt
	  tr.fractions[3] = numerators[1]/denominators[3];
    //Pt2
	  tr.fractions[4] = numerators[2]/denominators[4];
    //WPt2
	  tr.fractions[5] = numerators[2]/denominators[5];
  }
  //Sim or SimMatched
  else
  {
    //Tracks
    tr.fractions[0] = numerators[0]/denominators[0];
    //Pt
    tr.fractions[2] = numerators[1]/denominators[2];
    //Pt2
	  tr.fractions[4] = numerators[2]/denominators[4];
  }
  return tr;
}
/*
// calculations for RecoAllAssoc2Gen- plottings
std::vector<double> calculateVertexSharedTracksMomentumFractionNumerators(const reco::Vertex& recoV, const TrackingVertex& simV, const reco::SimToRecoCollection& trackSimToRecoAssociation, const reco::RecoToSimCollection& trackRecoToSimAssociation) {
  
  std::vector<double> nominators = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
  
  unsigned int elements = 0;

  //AssociationTypeStringToEnum::NUMBEROFTRACKS)
  nominators[0] = calculateVertexSharedTracks(recoV,simV, trackRecoToSimAssociation);

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

        double pt = (*iTrack).get()->pt();
        
        //AssociationTypeStringToEnum::Weighted
        nominators[1] += weight;
        //AssociationTypeStringToEnum::PT
        nominators[2] += pt;
        //AssociationTypeStringToEnum::WPT
        nominators[3] += pt*weight;
        //AssociationTypeStringToEnum::PT2
        nominators[4] += sqr(pt);  
        //AssociationTypeStringToEnum::WPT2
        nominators[5] += sqr(pt)*weight;
        //AssociationTypeStringToEnum::HARMPT
        nominators[6] += 1/pt;
        //AssociationTypeStringToEnum::WHARMPT
        if( pt != 0 )
          nominators[7] += weight/pt;
        if( pt*weight != 0 )
        {
          //AssociationTypeStringToEnum::HARMWPT
          nominators[8] += 1/(pt*weight);
          //AssociationTypeStringToEnum::HARMPTAVG
          nominators[9] += 1/(pt*weight);
          //AssociationTypeStringToEnum::WHARMPTAVG
          nominators[10] += 1/(pt*weight);
          //AssociationTypeStringToEnum::HARMWPTAVG
          nominators[11] += 1/(pt*weight);
        }

        elements++;
        break;
      }
    }
  }


  if( nominators[9] != 0)
    nominators[9] = elements/nominators[9];

  if( nominators[10] != 0)
    nominators[10] = elements/nominators[10];
  
  if( nominators[11] != 0)
    nominators[11] = elements/nominators[11];
  
  return nominators;
}


std::vector<std::vector<double>> calculateVertexSharedTracksMomentumFractionDenominators(const reco::Vertex& recoV, const TrackingVertex& simV, const reco::SimToRecoCollection& trackSimToRecoAssociation, const reco::RecoToSimCollection& trackRecoToSimAssociation) {

  // denominators are values for NumberOfTracks, Weighted, Pt, WPt, Pt2, WPt2, HarmPt, WHarmPt, HarmWPt, HarmPtAvg, WHarmPtAvg, HarmWPtAvg
  std::vector<double> denominators;
  // groups are Reco, RecoMatched, Sim, SimMatched
  std::vector<std::vector<double>> denominatorGroups;

  //calculate the divider in the fraction
  //"Reco"

  denominators = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
  
  unsigned int elements = 0;
  for(auto iTrack = recoV.tracks_begin(); iTrack != recoV.tracks_end(); ++iTrack)  {

    double pt = (*iTrack)->pt();
    float weight = recoV.trackWeight(*iTrack);
    //AssociationTypeStringToEnum::NumberOfTracks
    denominators[0] += 1;

    //AssociationTypeStringToEnum::Weighted
    denominators[1] += weight;

    //AssociationTypeStringToEnum::Pt
    denominators[2] += pt;

    //AssociationTypeStringToEnum::WPt
    denominators[3] += pt*weight;

    //AssociationTypeStringToEnum::Pt2
    denominators[4] += sqr(pt);

    //AssociationTypeStringToEnum::WPt2
    denominators[5] += sqr(pt)*weight;

    //AssociationTypeStringToEnum::HarmPt
    if( pt != 0 )
      denominators[6] += 1/pt;


    //AssociationTypeStringToEnum::WHarmPt
    if( pt != 0 )
      denominators[7] += weight/pt;

    //AssociationTypeStringToEnum::HarmWPt
    if( pt*weight != 0 )
      denominators[8] += 1/(pt*weight);

    //AssociationTypeStringToEnum::HarmPtAvg
    if( pt != 0 )
      denominators[9] += 1/(pt);

    //AssociationTypeStringToEnum::WHarmPtAvg
    if( pt != 0 )
      denominators[10] += weight/pt;

    //AssociationTypeStringToEnum::HarmWPtAvg
    if( pt*weight != 0 )
      denominators[11] += 1/(pt*weight);

    elements++;
  }

  //AssociationTypeStringToEnum::HarmPtAvg
  if( denominators[9] != 0)
    denominators[9] = elements/denominators[9];

  //AssociationTypeStringToEnum::WHarmPtAvg
  if( denominators[10] != 0)
    denominators[10] = elements/denominators[10];

  //AssociationTypeStringToEnum::HarmWPtAvg
  if( denominators[11] != 0)
    denominators[11] = elements/denominators[11];

  denominatorGroups.push_back(denominators);

  denominators.clear();
  denominators = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  
  //"RecoMatched") == 0)

  elements = 0;

  for(auto iTrack = recoV.tracks_begin(); iTrack != recoV.tracks_end(); ++iTrack) {

    double pt = (*iTrack)->pt();
    float weight = recoV.trackWeight(*iTrack);

    auto found = trackRecoToSimAssociation.find(*iTrack);

    if(found == trackRecoToSimAssociation.end())
      continue;

   //AssociationTypeStringToEnum::NumberOfTracks
    denominators[0] += 1;

    //AssociationTypeStringToEnum::Weighted
    denominators[1] += weight;

    //AssociationTypeStringToEnum::Pt
    denominators[2] += pt;

    //AssociationTypeStringToEnum::WPt
    denominators[3] += pt*weight;

    //AssociationTypeStringToEnum::Pt2
    denominators[4] += sqr(pt);

    //AssociationTypeStringToEnum::WPt2
    denominators[5] += sqr(pt)*weight;

    //AssociationTypeStringToEnum::HarmPt
    if( pt != 0 )
      denominators[6] += 1/pt;

    //AssociationTypeStringToEnum::WHarmPt
    if( (*iTrack)->pt() != 0 )
      denominators[7] += weight/pt;

    //AssociationTypeStringToEnum::HarmWPt
    if( (*iTrack)->pt()*weight != 0 )
      denominators[8] += 1/(pt*weight);

    //AssociationTypeStringToEnum::HarmPtAvg
    if( (*iTrack)->pt() != 0 )
      denominators[9] += 1/pt;

    //AssociationTypeStringToEnum::WHarmPtAvg
    if( (*iTrack)->pt() != 0 )
      denominators[10] += weight/pt;

    //AssociationTypeStringToEnum::HarmWPtAvg
    if( (*iTrack)->pt()*weight != 0 )
      denominators[11] += 1/(pt*weight);


    elements++;
  }

  //AssociationTypeStringToEnum::HarmPtAvg
  if( denominators[9] != 0)
    denominators[9] = elements/denominators[9];

  //AssociationTypeStringToEnum::WHarmPtAvg
  if( denominators[10] != 0)
    denominators[10] = elements/denominators[10];

  //AssociationTypeStringToEnum::HarmWPtAvg
  if( denominators[11] != 0)
    denominators[11] = elements/denominators[11];

  denominatorGroups.push_back(denominators);

  denominators.clear();
  denominators = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

  //Simulated tracks do not have weights
  //"Sim"

  for(auto iTrack = simV.daughterTracks_begin(); iTrack != simV.daughterTracks_end(); ++iTrack) {

    double pt = (*iTrack).get()->pt();

    //AssociationTypeStringToEnum::NumberOfTracks
    denominators[0] += 1;

    //AssociationTypeStringToEnum::Weighted
    denominators[1] += 1;

    //AssociationTypeStringToEnum::Pt
    denominators[2] += pt;

    //AssociationTypeStringToEnum::WPt
    denominators[3] += pt;

    //AssociationTypeStringToEnum::Pt2
    denominators[4] += sqr(pt);

    //AssociationTypeStringToEnum::WPt2
    denominators[5] += sqr(pt);

    //AssociationTypeStringToEnum::HarmPt
    if( pt != 0 )
      denominators[6] += 1/pt;

    //AssociationTypeStringToEnum::WHarmPt
    if( pt != 0 )
      denominators[7] += 1/pt;

    //AssociationTypeStringToEnum::HarmWPt
    if( pt != 0 )
      denominators[8] += 1/pt;

    //AssociationTypeStringToEnum::HarmPtAvg
    if( pt != 0 )
      denominators[9] += 1/pt;

    //AssociationTypeStringToEnum::WHarmPtAvg
    if( pt != 0 )
      denominators[10] += 1/pt;

    //AssociationTypeStringToEnum::HarmWPtAvg
    if( pt != 0 )
      denominators[11] += 1/pt;

    elements++;
  }

  //AssociationTypeStringToEnum::HarmPtAvg
  if( denominators[9] != 0)
    denominators[9] = elements/denominators[9];

  //AssociationTypeStringToEnum::WHarmPtAvg
  if( denominators[10] != 0)
    denominators[10] = elements/denominators[10];

  //AssociationTypeStringToEnum::HarmWPtAvg
  if( denominators[11] != 0)
    denominators[11] = elements/denominators[11];


  denominatorGroups.push_back(denominators);

  denominators.clear();
  denominators = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

  //SimMatched
  for(auto iTrack = simV.daughterTracks_begin(); iTrack != simV.daughterTracks_end(); ++iTrack) {
    auto found = trackSimToRecoAssociation.find(*iTrack);

    if(found == trackSimToRecoAssociation.end())
      continue;

    double pt = (*iTrack).get()->pt();

    //AssociationTypeStringToEnum::NumberOfTracks
    denominators[0] += 1;

    //AssociationTypeStringToEnum::Weighted
    denominators[1] += 1;

    //AssociationTypeStringToEnum::Pt
    denominators[2] += pt;

    //AssociationTypeStringToEnum::WPt
    denominators[3] += pt;

    //AssociationTypeStringToEnum::Pt2
    denominators[4] += sqr(pt);

    //AssociationTypeStringToEnum::WPt2
    denominators[5] += sqr(pt);

    //AssociationTypeStringToEnum::HarmPt
    if( pt != 0 )
      denominators[6] += 1/pt;

    //AssociationTypeStringToEnum::WHarmPt
    if( pt != 0 )
      denominators[7] += 1/pt;

    //AssociationTypeStringToEnum::HarmWPt
    if( pt != 0 )
      denominators[8] += 1/pt;

    //AssociationTypeStringToEnum::HarmPtAvg
    if( pt != 0 )
      denominators[9] += 1/pt;

    //AssociationTypeStringToEnum::WHarmPtAvg
    if( pt != 0 )
      denominators[10] += 1/pt;

    //AssociationTypeStringToEnum::HarmWPtAvg
    if( pt != 0 )
      denominators[11] += 1/pt;
    
    elements++;
  }    

  //AssociationTypeStringToEnum::HarmPtAvg
  if( denominators[9] != 0)
    denominators[9] = elements/denominators[9];

  //AssociationTypeStringToEnum::WHarmPtAvg
  if( denominators[10] != 0)
    denominators[10] = elements/denominators[10];

  //AssociationTypeStringToEnum::HarmWPtAvg
  if( denominators[11] != 0)
    denominators[11] = elements/denominators[11];

  denominatorGroups.push_back(denominators);

  return denominatorGroups;
}


// calculations for SimAllAssoc2Reco- plottings
// simulated tracks do not have weights

std::vector<double> calculateVertexSharedTracksMomentumFractionNumerators(const TrackingVertex& simV, const reco::Vertex& recoV, const reco::SimToRecoCollection& trackSimToRecoAssociation, const reco::RecoToSimCollection& trackRecoToSimAssociation) {

  std::vector<double> nominators = { 0, 0, 0 };

  //AssociationTypeStringToEnum::NumberOfTracks)
  nominators[0] = calculateVertexSharedTracks(simV,recoV, trackSimToRecoAssociation);

  for(auto iTrack = simV.daughterTracks_begin(); iTrack != simV.daughterTracks_end(); ++iTrack) {

    auto found = trackSimToRecoAssociation.find(*iTrack);

    if(found == trackSimToRecoAssociation.end())
      continue;

    
    for(const auto& tp: found->val) {
      
      
      for(auto iReco = recoV.tracks_begin(); iReco != recoV.tracks_end(); iReco++)  {
        if( iReco->id() == tp.first.id() )  {
          if( iReco->key() == tp.first.key() )  {

            double pt = tp.first.get()->pt();
            //AssociationTypeStringToEnum::Pt
            nominators[1] += pt;
            //AssociationTypeStringToEnum::Pt2
            nominators[2] += sqr(pt);
            break;
          }
        }
      }
      
    }                  
  }

  return nominators;
}




std::vector<std::vector<double>> calculateVertexSharedTracksMomentumFractionDenominators(const TrackingVertex& simV, const reco::Vertex& recoV, const reco::SimToRecoCollection& trackSimToRecoAssociation, const reco::RecoToSimCollection& trackRecoToSimAssociation) {

  // denominators are values for NumberOfTracks, W, Pt, WPt, Pt2, WPt2, HarmPt, WHarmPt, HarmWPt, HarmPtAvg, WHarmPtAvg, HarmWPtAvg
  std::vector<double> denominators;
  // groups are Reco, RecoMatched, Sim, SimMatched
  std::vector<std::vector<double>> denominatorGroups;

  denominators = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

  //calculate the divider in the fraction
  //"Reco"
  unsigned int elements = 0;
  for(auto iTrack = recoV.tracks_begin(); iTrack != recoV.tracks_end(); ++iTrack)  {

    double pt = (*iTrack)->pt();
    float weight = recoV.trackWeight(*iTrack);

    //AssociationTypeStringToEnum::NumberOfTracks
    denominators[0] += 1;

    //AssociationTypeStringToEnum::Weighted
    denominators[1] += weight;

    //AssociationTypeStringToEnum::Pt
    denominators[2] += pt;

    //AssociationTypeStringToEnum::WPt
    denominators[3] += pt*weight;

    //AssociationTypeStringToEnum::Pt2
    denominators[4] += sqr(pt);

    //AssociationTypeStringToEnum::WPt2
    denominators[5] += sqr(pt)*weight;

    //AssociationTypeStringToEnum::HarmPt
    if( pt != 0 )
      denominators[6] += 1/pt;

    //AssociationTypeStringToEnum::WHarmPt
    if( pt != 0 )
      denominators[7] += weight/pt;

    //AssociationTypeStringToEnum::HarmWPt
    if( pt*weight != 0 )
      denominators[8] += 1/(pt*weight);

    //AssociationTypeStringToEnum::HarmPtAvg
    if( pt != 0 )
      denominators[9] += 1/pt;

    //AssociationTypeStringToEnum::WHarmPtAvg
    if( pt != 0 )
      denominators[10] += 1/pt;

    //AssociationTypeStringToEnum::HarmWPtAvg
    if( pt != 0 )
      denominators[11] += 1/pt;
    
    elements++;
  }    

  //AssociationTypeStringToEnum::HarmPtAvg
  if( denominators[9] != 0)
    denominators[9] = elements/denominators[9];

  //AssociationTypeStringToEnum::WHarmPtAvg
  if( denominators[10] != 0)
    denominators[10] = elements/denominators[10];

  //AssociationTypeStringToEnum::HarmWPtAvg
  if( denominators[11] != 0)
    denominators[11] = elements/denominators[11];

  denominatorGroups.push_back(denominators);

  denominators.clear();
  denominators = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

  //"RecoMatched"
  elements = 0;
  for(auto iTrack = recoV.tracks_begin(); iTrack != recoV.tracks_end(); ++iTrack) {

    double pt = (*iTrack).get()->pt();
    float weight = recoV.trackWeight(*iTrack);

    auto found = trackRecoToSimAssociation.find(*iTrack);

    if(found == trackRecoToSimAssociation.end())
            continue;

    //AssociationTypeStringToEnum::NumberOfTracks
    denominators[0] += 1;

    //AssociationTypeStringToEnum::Weighted
    denominators[1] += weight;

    //AssociationTypeStringToEnum::Pt
    denominators[2] += pt;

    //AssociationTypeStringToEnum::WPt
    denominators[3] += pt*weight;

    //AssociationTypeStringToEnum::Pt2
    denominators[4] += sqr(pt);

    //AssociationTypeStringToEnum::WPt2
    denominators[5] += sqr(pt)*weight;

    //AssociationTypeStringToEnum::HarmPt
    if( pt != 0 )
      denominators[6] += 1/pt;

    //AssociationTypeStringToEnum::WHarmPt
    if( pt != 0 )
      denominators[7] += weight/pt;

    //AssociationTypeStringToEnum::HarmWPt
    if( pt*weight != 0 )
      denominators[8] += 1/(pt*weight);

    //AssociationTypeStringToEnum::HarmPtAvg
    if( pt != 0 )
      denominators[9] += 1/pt;

    //AssociationTypeStringToEnum::WHarmPtAvg
    if( pt != 0 )
      denominators[10] += 1/pt;

    //AssociationTypeStringToEnum::HarmWPtAvg
    if( pt != 0 )
      denominators[11] += 1/pt;
    
    elements++;
  }    

  //AssociationTypeStringToEnum::HarmPtAvg
  if( denominators[9] != 0)
    denominators[9] = elements/denominators[9];

  //AssociationTypeStringToEnum::WHarmPtAvg
  if( denominators[10] != 0)
    denominators[10] = elements/denominators[10];

  //AssociationTypeStringToEnum::HarmWPtAvg
  if( denominators[11] != 0)
    denominators[11] = elements/denominators[11];    

  denominatorGroups.push_back(denominators);
  denominators.clear();
  denominators = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

  //Simulated tracks do not have weights
  //"Sim"
  elements = 0;
  for(auto iTrack = simV.daughterTracks_begin(); iTrack != simV.daughterTracks_end(); ++iTrack) {

    double pt = (*iTrack).get()->pt();

    //AssociationTypeStringToEnum::NumberOfTracks
    denominators[0] += 1;

    //AssociationTypeStringToEnum::Weighted
    denominators[1] += 1;

    //AssociationTypeStringToEnum::Pt
    denominators[2] += pt;

    //AssociationTypeStringToEnum::Pt2
    denominators[3] += sqr(pt);

    //AssociationTypeStringToEnum::HarmPt
    if( pt != 0 )
      denominators[4] += 1/pt;

    //AssociationTypeStringToEnum::WHarmPt
    if( pt != 0 )
      denominators[5] += 1/pt;

    //AssociationTypeStringToEnum::HarmWPt
    if( pt != 0 )
      denominators[6] += 1/pt;

    //AssociationTypeStringToEnum::HarmPtAvg
    if( pt != 0 )
      denominators[9] += 1/pt;

    //AssociationTypeStringToEnum::WHarmPtAvg
    if( pt != 0 )
      denominators[10] += 1/pt;

    //AssociationTypeStringToEnum::HarmWPtAvg
    if( pt != 0 )
      denominators[11] += 1/pt;
    
    elements++;
  }    

  //AssociationTypeStringToEnum::HarmPtAvg
  if( denominators[9] != 0)
    denominators[9] = elements/denominators[9];

  //AssociationTypeStringToEnum::WHarmPtAvg
  if( denominators[10] != 0)
    denominators[10] = elements/denominators[10];

  //AssociationTypeStringToEnum::HarmWPtAvg
  if( denominators[11] != 0)
    denominators[11] = elements/denominators[11];

  denominatorGroups.push_back(denominators);

  denominators.clear();
  denominators = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

  //"SimMatched"
  for(auto iTrack = simV.daughterTracks_begin(); iTrack != simV.daughterTracks_end(); ++iTrack) {
    auto found = trackSimToRecoAssociation.find(*iTrack);

    if(found == trackSimToRecoAssociation.end())
      continue;

    double pt = (*iTrack).get()->pt();

    //AssociationTypeStringToEnum::NumberOfTracks
    denominators[0] += 1;

    //AssociationTypeStringToEnum::Weighted
    denominators[1] += 1;

    //AssociationTypeStringToEnum::Pt
    denominators[2] += pt;

    //AssociationTypeStringToEnum::Pt2
    denominators[3] += sqr(pt);

    //AssociationTypeStringToEnum::HarmPt
    if( pt != 0 )
      denominators[4] += 1/pt;

    //AssociationTypeStringToEnum::WHarmPt
    if( pt != 0 )
      denominators[5] += 1/pt;

    //AssociationTypeStringToEnum::HarmWPt
    if( pt != 0 )
      denominators[6] += 1/pt;

    //AssociationTypeStringToEnum::HarmPtAvg
    if( pt != 0 )
      denominators[9] += 1/pt;

    //AssociationTypeStringToEnum::WHarmPtAvg
    if( pt != 0 )
      denominators[10] += 1/pt;

    //AssociationTypeStringToEnum::HarmWPtAvg
    if( pt != 0 )
      denominators[11] += 1/pt;
    
    elements++;
  }    

  //AssociationTypeStringToEnum::HarmPtAvg
  if( denominators[9] != 0)
    denominators[9] = elements/denominators[9];

  //AssociationTypeStringToEnum::WHarmPtAvg
  if( denominators[10] != 0)
    denominators[10] = elements/denominators[10];

  //AssociationTypeStringToEnum::HarmWPtAvg
  if( denominators[11] != 0)
    denominators[11] = elements/denominators[11];

  denominatorGroups.push_back(denominators);

  return denominatorGroups;
}
*/
