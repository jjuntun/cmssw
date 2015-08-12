#include "SimTracker/VertexAssociation/interface/calculateVertexSharedTracks.h"


namespace {
  template <typename T>
    T sqr(T val) { return val*val; }
}

unsigned int calculateVertexSharedTracks(const reco::Vertex& recoV, const TrackingVertex& simV, const reco::RecoToSimCollection& trackRecoToSimAssociation) {
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

  return sharedTracks;
}

unsigned int calculateVertexSharedTracks(const TrackingVertex& simV, const reco::Vertex& recoV, const reco::SimToRecoCollection& trackSimToRecoAssociation) {
  unsigned int sharedTracks = 0;

  for(auto iTP = simV.daughterTracks_begin(); iTP != simV.daughterTracks_end(); ++iTP) {
    auto found = trackSimToRecoAssociation.find(*iTP);

    if(found == trackSimToRecoAssociation.end())
      continue;

    // matched track equal to any track of reco vertex => increase counter
    for(const auto& tk: found->val) {
      for(auto iReco = recoV.tracks_begin(); iReco != recoV.tracks_end(); iReco++)  {
        if( iReco->id() == tk.first.id() )  {
          if( iReco->key() == tk.first.key() )  {
            sharedTracks += 1;
            break;
          }
        }
      }
    }
  }

  return sharedTracks;
}


// calculations for RecoAllAssoc2Gen- plottings
std::vector<double> calculateVertexSharedTracksMomentumFractionNumerators(const reco::Vertex& recoV, const TrackingVertex& simV, const reco::SimToRecoCollection& trackSimToRecoAssociation, const reco::RecoToSimCollection& trackRecoToSimAssociation) {
  
  std::vector<double> nominators = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
  
  unsigned int elements = 0;

  //AssociationType::NUMBEROFTRACKS)
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
        
        //AssociationType::W
        nominators[1] += weight;
        //AssociationType::PT
        nominators[2] += pt;
        //AssociationType::WPT
        nominators[3] += pt*weight;
        //AssociationType::PT2
        nominators[4] += sqr(pt);  
        //AssociationType::WPT2
        nominators[5] += sqr(pt)*weight;
        //AssociationType::HARMPT
        nominators[6] += 1/pt;
        //AssociationType::WHARMPT
        if( pt != 0 )
          nominators[7] += weight/pt;
        //AssociationType::HARMWPT
        if( pt*weight != 0 )
          nominators[8] += 1/(pt*weight);
        //AssociationType::HARMPTAVG
        if( pt*weight != 0 )
          nominators[9] += 1/(pt*weight);

        //AssociationType::WHARMPTAVG
        if( pt*weight != 0 )
          nominators[10] += 1/(pt*weight);

        //AssociationType::HARMWPTAVG
        if( pt*weight != 0 )
          nominators[11] += 1/(pt*weight);

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

  // denominators are values for NUMBEROFTRACKS, W, PT, WPT, PT2, WPT2, HARMPT, WHARMPT, HARMWPT, HARMPTAVG, WHARMPTAVG, HARMWPTAVG
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
    //AssociationType::NUMBEROFTRACKS
    denominators[0] += 1;

    //AssociationType::W
    denominators[1] += weight;

    //AssociationType::PT
    denominators[2] += pt;

    //AssociationType::WPT
    denominators[3] += pt*weight;

    //AssociationType::PT2
    denominators[4] += sqr(pt);

    //AssociationType::WPT2
    denominators[5] += sqr(pt)*weight;

    //AssociationType::HARMPT
    if( pt != 0 )
      denominators[6] += 1/pt;


    //AssociationType::WHARMPT
    if( pt != 0 )
      denominators[7] += weight/pt;

    //AssociationType::HARMWPT
    if( pt*weight != 0 )
      denominators[8] += 1/(pt*weight);

    //AssociationType::HARMPTAVG
    if( pt != 0 )
      denominators[9] += 1/(pt);

    //AssociationType::WHARMPTAVG
    if( pt != 0 )
      denominators[10] += weight/pt;

    //AssociationType::HARMWPTAVG
    if( pt*weight != 0 )
      denominators[11] += 1/(pt*weight);

    elements++;
  }

  //AssociationType::HARMPTAVG
  if( denominators[9] != 0)
    denominators[9] = elements/denominators[9];

  //AssociationType::WHARMPTAVG
  if( denominators[10] != 0)
    denominators[10] = elements/denominators[10];

  //AssociationType::HARMWPTAVG
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

   //AssociationType::NUMBEROFTRACKS
    denominators[0] += 1;

    //AssociationType::W
    denominators[1] += weight;

    //AssociationType::PT
    denominators[2] += pt;

    //AssociationType::WPT
    denominators[3] += pt*weight;

    //AssociationType::PT2
    denominators[4] += sqr(pt);

    //AssociationType::WPT2
    denominators[5] += sqr(pt)*weight;

    //AssociationType::HARMPT
    if( pt != 0 )
      denominators[6] += 1/pt;

    //AssociationType::WHARMPT
    if( (*iTrack)->pt() != 0 )
      denominators[7] += weight/pt;

    //AssociationType::HARMWPT
    if( (*iTrack)->pt()*weight != 0 )
      denominators[8] += 1/(pt*weight);

    //AssociationType::HARMPTAVG
    if( (*iTrack)->pt() != 0 )
      denominators[9] += 1/pt;

    //AssociationType::WHARMPTAVG
    if( (*iTrack)->pt() != 0 )
      denominators[10] += weight/pt;

    //AssociationType::HARMWPTAVG
    if( (*iTrack)->pt()*weight != 0 )
      denominators[11] += 1/(pt*weight);


    elements++;
  }

  //AssociationType::HARMPTAVG
  if( denominators[9] != 0)
    denominators[9] = elements/denominators[9];

  //AssociationType::WHARMPTAVG
  if( denominators[10] != 0)
    denominators[10] = elements/denominators[10];

  //AssociationType::HARMWPTAVG
  if( denominators[11] != 0)
    denominators[11] = elements/denominators[11];

  denominatorGroups.push_back(denominators);

  denominators.clear();
  denominators = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

  //Simulated tracks do not have weights
  //"Sim") == 0)

  for(auto iTrack = simV.daughterTracks_begin(); iTrack != simV.daughterTracks_end(); ++iTrack) {

    double pt = (*iTrack).get()->pt();

    //AssociationType::NUMBEROFTRACKS
    denominators[0] += 1;

    //AssociationType::W
    denominators[1] += 1;

    //AssociationType::PT
    denominators[2] += pt;

    //AssociationType::WPT
    denominators[3] += pt;

    //AssociationType::PT2
    denominators[4] += sqr(pt);

    //AssociationType::WPT2
    denominators[5] += sqr(pt);

    //AssociationType::HARMPT
    if( pt != 0 )
      denominators[6] += 1/pt;

    //AssociationType::WHARMPT
    if( pt != 0 )
      denominators[7] += 1/pt;

    //AssociationType::HARMWPT
    if( pt != 0 )
      denominators[8] += 1/pt;

    //AssociationType::HARMPTAVG
    if( pt != 0 )
      denominators[9] += 1/pt;

    //AssociationType::WHARMPTAVG
    if( pt != 0 )
      denominators[10] += 1/pt;

    //AssociationType::HARMWPTAVG
    if( pt != 0 )
      denominators[11] += 1/pt;

    elements++;
  }

  //AssociationType::HARMPTAVG
  if( denominators[9] != 0)
    denominators[9] = elements/denominators[9];

  //AssociationType::WHARMPTAVG
  if( denominators[10] != 0)
    denominators[10] = elements/denominators[10];

  //AssociationType::HARMWPTAVG
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

    //AssociationType::NUMBEROFTRACKS
    denominators[0] += 1;

    //AssociationType::W
    denominators[1] += 1;

    //AssociationType::PT
    denominators[2] += pt;

    //AssociationType::WPT
    denominators[3] += pt;

    //AssociationType::PT2
    denominators[4] += sqr(pt);

    //AssociationType::WPT2
    denominators[5] += sqr(pt);

    //AssociationType::HARMPT
    if( pt != 0 )
      denominators[6] += 1/pt;

    //AssociationType::WHARMPT
    if( pt != 0 )
      denominators[7] += 1/pt;

    //AssociationType::HARMWPT
    if( pt != 0 )
      denominators[8] += 1/pt;

    //AssociationType::HARMPTAVG
    if( pt != 0 )
      denominators[9] += 1/pt;

    //AssociationType::WHARMPTAVG
    if( pt != 0 )
      denominators[10] += 1/pt;

    //AssociationType::HARMWPTAVG
    if( pt != 0 )
      denominators[11] += 1/pt;
    
    elements++;
  }    

  //AssociationType::HARMPTAVG
  if( denominators[9] != 0)
    denominators[9] = elements/denominators[9];

  //AssociationType::WHARMPTAVG
  if( denominators[10] != 0)
    denominators[10] = elements/denominators[10];

  //AssociationType::HARMWPTAVG
  if( denominators[11] != 0)
    denominators[11] = elements/denominators[11];

  denominatorGroups.push_back(denominators);

  return denominatorGroups;
}


// calculations for SimAllAssoc2Reco- plottings
// simulated tracks do not have weights

std::vector<double> calculateVertexSharedTracksMomentumFractionNumerators(const TrackingVertex& simV, const reco::Vertex& recoV, const reco::SimToRecoCollection& trackSimToRecoAssociation, const reco::RecoToSimCollection& trackRecoToSimAssociation) {

  std::vector<double> nominators = { 0, 0, 0 };

  //AssociationType::NUMBEROFTRACKS)
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
            //AssociationType::PT
            nominators[1] += pt;
            //AssociationType::PT2
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

  // denominators are values for NUMBEROFTRACKS, W, PT, WPT, PT2, WPT2, HARMPT, WHARMPT, HARMWPT, HARMPTAVG, WHARMPTAVG, HARMWPTAVG
  std::vector<double> denominators;
  // groups are Reco, RecoMatched, Sim, SimMatched
  std::vector<std::vector<double>> denominatorGroups;

  denominators = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

  //calculate the divider in the fraction
  //"Reco") == 0)
  unsigned int elements = 0;
  for(auto iTrack = recoV.tracks_begin(); iTrack != recoV.tracks_end(); ++iTrack)  {

    double pt = (*iTrack)->pt();
    float weight = recoV.trackWeight(*iTrack);

    //AssociationType::NUMBEROFTRACKS
    denominators[0] += 1;

    //AssociationType::W
    denominators[1] += weight;

    //AssociationType::PT
    denominators[2] += pt;

    //AssociationType::WPT
    denominators[3] += pt*weight;

    //AssociationType::PT2
    denominators[4] += sqr(pt);

    //AssociationType::WPT2
    denominators[5] += sqr(pt)*weight;

    //AssociationType::HARMPT
    if( pt != 0 )
      denominators[6] += 1/pt;

    //AssociationType::WHARMPT
    if( pt != 0 )
      denominators[7] += weight/pt;

    //AssociationType::HARMWPT
    if( pt*weight != 0 )
      denominators[8] += 1/(pt*weight);

    //AssociationType::HARMPTAVG
    if( pt != 0 )
      denominators[9] += 1/pt;

    //AssociationType::WHARMPTAVG
    if( pt != 0 )
      denominators[10] += 1/pt;

    //AssociationType::HARMWPTAVG
    if( pt != 0 )
      denominators[11] += 1/pt;
    
    elements++;
  }    

  //AssociationType::HARMPTAVG
  if( denominators[9] != 0)
    denominators[9] = elements/denominators[9];

  //AssociationType::WHARMPTAVG
  if( denominators[10] != 0)
    denominators[10] = elements/denominators[10];

  //AssociationType::HARMWPTAVG
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

    //AssociationType::NUMBEROFTRACKS
    denominators[0] += 1;

    //AssociationType::W
    denominators[1] += weight;

    //AssociationType::PT
    denominators[2] += pt;

    //AssociationType::WPT
    denominators[3] += pt*weight;

    //AssociationType::PT2
    denominators[4] += sqr(pt);

    //AssociationType::WPT2
    denominators[5] += sqr(pt)*weight;

    //AssociationType::HARMPT
    if( pt != 0 )
      denominators[6] += 1/pt;

    //AssociationType::WHARMPT
    if( pt != 0 )
      denominators[7] += weight/pt;

    //AssociationType::HARMWPT
    if( pt*weight != 0 )
      denominators[8] += 1/(pt*weight);

    //AssociationType::HARMPTAVG
    if( pt != 0 )
      denominators[9] += 1/pt;

    //AssociationType::WHARMPTAVG
    if( pt != 0 )
      denominators[10] += 1/pt;

    //AssociationType::HARMWPTAVG
    if( pt != 0 )
      denominators[11] += 1/pt;
    
    elements++;
  }    

  //AssociationType::HARMPTAVG
  if( denominators[9] != 0)
    denominators[9] = elements/denominators[9];

  //AssociationType::WHARMPTAVG
  if( denominators[10] != 0)
    denominators[10] = elements/denominators[10];

  //AssociationType::HARMWPTAVG
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

    //AssociationType::NUMBEROFTRACKS
    denominators[0] += 1;

    //AssociationType::W
    denominators[1] += 1;

    //AssociationType::PT
    denominators[2] += pt;

    //AssociationType::PT2
    denominators[3] += sqr(pt);

    //AssociationType::HARMPT
    if( pt != 0 )
      denominators[4] += 1/pt;

    //AssociationType::WHARMPT
    if( pt != 0 )
      denominators[5] += 1/pt;

    //AssociationType::HARMWPT
    if( pt != 0 )
      denominators[6] += 1/pt;

    //AssociationType::HARMPTAVG
    if( pt != 0 )
      denominators[9] += 1/pt;

    //AssociationType::WHARMPTAVG
    if( pt != 0 )
      denominators[10] += 1/pt;

    //AssociationType::HARMWPTAVG
    if( pt != 0 )
      denominators[11] += 1/pt;
    
    elements++;
  }    

  //AssociationType::HARMPTAVG
  if( denominators[9] != 0)
    denominators[9] = elements/denominators[9];

  //AssociationType::WHARMPTAVG
  if( denominators[10] != 0)
    denominators[10] = elements/denominators[10];

  //AssociationType::HARMWPTAVG
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

    //AssociationType::NUMBEROFTRACKS
    denominators[0] += 1;

    //AssociationType::W
    denominators[1] += 1;

    //AssociationType::PT
    denominators[2] += pt;

    //AssociationType::PT2
    denominators[3] += sqr(pt);

    //AssociationType::HARMPT
    if( pt != 0 )
      denominators[4] += 1/pt;

    //AssociationType::WHARMPT
    if( pt != 0 )
      denominators[5] += 1/pt;

    //AssociationType::HARMWPT
    if( pt != 0 )
      denominators[6] += 1/pt;

    //AssociationType::HARMPTAVG
    if( pt != 0 )
      denominators[9] += 1/pt;

    //AssociationType::WHARMPTAVG
    if( pt != 0 )
      denominators[10] += 1/pt;

    //AssociationType::HARMWPTAVG
    if( pt != 0 )
      denominators[11] += 1/pt;
    
    elements++;
  }    

  //AssociationType::HARMPTAVG
  if( denominators[9] != 0)
    denominators[9] = elements/denominators[9];

  //AssociationType::WHARMPTAVG
  if( denominators[10] != 0)
    denominators[10] = elements/denominators[10];

  //AssociationType::HARMWPTAVG
  if( denominators[11] != 0)
    denominators[11] = elements/denominators[11];

  denominatorGroups.push_back(denominators);

  return denominatorGroups;
}
