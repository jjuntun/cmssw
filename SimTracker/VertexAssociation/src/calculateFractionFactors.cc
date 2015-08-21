#include "SimTracker/VertexAssociation/interface/calculateVertexSharedTracks.h"

namespace {
  template <typename T>
    T sqr(T val) { return val*val; }
}

namespace vertexAssociation
{
  void TrackFraction::calculateNumerators(const reco::Vertex& recoV, const TrackingVertex& simV,  const reco::RecoToSimCollection& trackRecoToSimAssociation)
  {
    unsigned int numeratorElements = 0;

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
          //AssociationType::NumberOfTracks
          numerators[0] += 1;
          //AssociationType::Weighted
          numerators[1] += weight;
          //AssociationType::Pt
          numerators[2] += pt;
          //AssociationType::WPt
          numerators[3] += pt*weight;
          //AssociationType::Pt2
          numerators[4] += sqr(pt);  
          //AssociationType::WPt2
          numerators[5] += sqr(pt)*weight;
          //AssociationType::HarmPt
          numerators[6] += 1/pt;
          //AssociationType::WHarmPt
          if( pt != 0 )
            numerators[7] += weight/pt;
          if( pt*weight != 0 )
          {
            //AssociationType::HarmWPt
            numerators[8] += 1/(pt*weight);
            //AssociationType::HarmPtAvg
            numerators[9] += 1/(pt*weight);
            //AssociationType::WHarmPtAvg
            numerators[10] += 1/(pt*weight);
            //AssociationType::HarmWPtAvg
            numerators[11] += 1/(pt*weight);
          }

          numeratorElements++;
          break;
        }
      }

    }

    if( numerators[9] != 0)
      numerators[9] = numeratorElements/numerators[9];

    if( numerators[10] != 0)
      numerators[10] = numeratorElements/numerators[10];
    
    if( numerators[11] != 0)
      numerators[11] = numeratorElements/numerators[11];
  }


  void TrackFraction::calculateNumerators(const TrackingVertex& simV, const reco::Vertex& recoV, const reco::SimToRecoCollection& trackSimToRecoAssociation)
  {

    for(auto iTP = simV.daughterTracks_begin(); iTP != simV.daughterTracks_end(); ++iTP) {
      auto found = trackSimToRecoAssociation.find(*iTP);

      if(found == trackSimToRecoAssociation.end())
        continue;

      // matched track equal to any track of reco vertex => increase counter
      for(const auto& tk: found->val) {
        if(std::find_if(recoV.tracks_begin(), recoV.tracks_end(), [&](const reco::TrackBaseRef& vtk) {
            return tk.first.id() == vtk.id() && tk.first.key() == vtk.key();
          }) != recoV.tracks_end()) {

          double pt = tk.first.get()->pt();
          //AssociationType::NumberOfTracks
          numerators[0] += 1;
          //AssociationType::Pt
          numerators[1] += pt;
          //AssociationType::Pt2
          numerators[2] += sqr(pt);
          break;
        }
      }
    }
  }

  void TrackFraction::calculateRecoDenominators(const reco::Vertex& recoV)
  {
    unsigned int denominatorElements = 0;

    for(auto iTrack = recoV.tracks_begin(); iTrack != recoV.tracks_end(); ++iTrack)  {

      double pt = (*iTrack)->pt();
      float weight = recoV.trackWeight(*iTrack);
      //AssociationType::NumberOfTracks
      TrackFraction::denominators[0] += 1;

      //AssociationType::Weighted
      TrackFraction::denominators[1] += weight;

      //AssociationType::Pt
      TrackFraction::denominators[2] += pt;

      //AssociationType::WPt
      TrackFraction::denominators[3] += pt*weight;

      //AssociationType::Pt2
      TrackFraction::denominators[4] += sqr(pt);

      //AssociationType::WPt2
      TrackFraction::denominators[5] += sqr(pt)*weight;

      //AssociationType::HarmPt
      if( pt != 0 )
        TrackFraction::denominators[6] += 1/pt;


      //AssociationType::WHarmPt
      if( pt != 0 )
        TrackFraction::denominators[7] += weight/pt;

      //AssociationType::HarmWPt
      if( pt*weight != 0 )
        TrackFraction::denominators[8] += 1/(pt*weight);

      //AssociationType::HarmPtAvg
      if( pt != 0 )
        TrackFraction::denominators[9] += 1/(pt);

      //AssociationType::WHarmPtAvg
      if( pt != 0 )
        TrackFraction::denominators[10] += weight/pt;

      //AssociationType::HarmWPtAvg
      if( pt*weight != 0 )
        TrackFraction::denominators[11] += 1/(pt*weight);

      denominatorElements++;
    }

    //AssociationType::HarmPtAvg
    if( TrackFraction::denominators[9] != 0)
      TrackFraction::denominators[9] = denominatorElements/TrackFraction::denominators[9];

    //AssociationType::WHarmPtAvg
    if( denominators[10] != 0)
      TrackFraction::denominators[10] = denominatorElements/TrackFraction::denominators[10];

    //AssociationType::HarmWPtAvg
    if( TrackFraction::denominators[11] != 0)
      TrackFraction::denominators[11] = denominatorElements/TrackFraction::denominators[11];

  }


  void TrackFraction::calculateRecoMatchedDenominators(const reco::Vertex& recoV,  const reco::RecoToSimCollection& trackRecoToSimAssociation)
  {
    unsigned int denominatorElements = 0;

    for(auto iTrack = recoV.tracks_begin(); iTrack != recoV.tracks_end(); ++iTrack) {

      double pt = (*iTrack)->pt();
      float weight = recoV.trackWeight(*iTrack);

      auto found = trackRecoToSimAssociation.find(*iTrack);

      if(found == trackRecoToSimAssociation.end())
        continue;

      //AssociationType::NumberOfTracks
      TrackFraction::denominators[0] += 1;

      //AssociationType::Weighted
      TrackFraction::denominators[1] += weight;

      //AssociationType::Pt
      TrackFraction::denominators[2] += pt;

      //AssociationType::WPt
      TrackFraction::denominators[3] += pt*weight;

      //AssociationType::Pt2
      TrackFraction::denominators[4] += sqr(pt);

      //AssociationType::WPt2
      TrackFraction::denominators[5] += sqr(pt)*weight;

      //AssociationType::HarmPt
      if( pt != 0 )
      {
        //AssociationType::HarmPt
        TrackFraction::denominators[6] += 1/pt;
        //AssociationType::WHarmPt
        TrackFraction::denominators[7] += weight/pt;
        //AssociationType::HarmWPt
        TrackFraction::denominators[8] += 1/(pt*weight);
        //AssociationType::HarmPtAvg
        TrackFraction::denominators[9] += 1/pt;
        //AssociationType::WHarmPtAvg
        TrackFraction::denominators[10] += weight/pt;
        //AssociationType::HarmWPtAvg
        TrackFraction::denominators[11] += 1/(pt*weight);
      }

      denominatorElements++;
    }

    //AssociationType::HarmPtAvg
    if( TrackFraction::denominators[9] != 0)
      TrackFraction::denominators[9] = denominatorElements/TrackFraction::denominators[9];

    //AssociationType::WHarmPtAvg
    if( TrackFraction::denominators[10] != 0)
      TrackFraction::denominators[10] = denominatorElements/TrackFraction::denominators[10];

    //AssociationType::HarmWPtAvg
    if( TrackFraction::denominators[11] != 0)
      TrackFraction::denominators[11] = denominatorElements/TrackFraction::denominators[11];

  }


  void TrackFraction::calculateSimDenominators(const TrackingVertex& simV)
  {
    unsigned int denominatorElements = 0;

    for(auto iTrack = simV.daughterTracks_begin(); iTrack != simV.daughterTracks_end(); ++iTrack) {

      double pt = (*iTrack).get()->pt();

      //AssociationType::NumberOfTracks
      TrackFraction::denominators[0] += 1;

      //AssociationType::Weighted
      TrackFraction::denominators[1] += 1;

      //AssociationType::Pt
      TrackFraction::denominators[2] += pt;

      //AssociationType::WPt
      TrackFraction::denominators[3] += pt;

      //AssociationType::Pt2
      TrackFraction::denominators[4] += sqr(pt);

      //AssociationType::WPt2
      TrackFraction::denominators[5] += sqr(pt);

      if( pt != 0 )
      {
        //AssociationType::HarmPt
        TrackFraction::denominators[6] += 1/pt;
        //AssociationType::WHarmPt
        TrackFraction::denominators[7] += 1/pt;
        //AssociationType::HarmWPt
        TrackFraction::denominators[8] += 1/pt;
        //AssociationType::HarmPtAvg
        TrackFraction::denominators[9] += 1/pt;
        //AssociationType::WHarmPtAvg
        TrackFraction::denominators[10] += 1/pt;
        //AssociationType::HarmWPtAvg
        TrackFraction::denominators[11] += 1/pt;
      }

      denominatorElements++;
    }

    //AssociationType::HarmPtAvg
    if( TrackFraction::denominators[9] != 0)
      TrackFraction::denominators[9] = denominatorElements/TrackFraction::denominators[9];

    //AssociationType::WHarmPtAvg
    if( TrackFraction::denominators[10] != 0)
      TrackFraction::denominators[10] = denominatorElements/TrackFraction::denominators[10];

    //AssociationType::HarmWPtAvg
    if( TrackFraction::denominators[11] != 0)
      TrackFraction::denominators[11] = denominatorElements/TrackFraction::denominators[11];

  }

  void TrackFraction::calculateSimMatchedDenominators(const TrackingVertex& simV,  const reco::SimToRecoCollection& trackSimToRecoAssociation)
  {
    unsigned int denominatorElements = 0;

    for(auto iTrack = simV.daughterTracks_begin(); iTrack != simV.daughterTracks_end(); ++iTrack) {
      auto found = trackSimToRecoAssociation.find(*iTrack);

      if(found == trackSimToRecoAssociation.end())
        continue;

      double pt = (*iTrack).get()->pt();

      //AssociationType::NumberOfTracks
      TrackFraction::denominators[0] += 1;

      //AssociationType::Weighted
      TrackFraction::denominators[1] += 1;

      //AssociationType::Pt
      TrackFraction::denominators[2] += pt;

      //AssociationType::WPt
      TrackFraction::denominators[3] += pt;

      //AssociationType::Pt2
      TrackFraction::denominators[4] += sqr(pt);

      //AssociationType::WPt2
      TrackFraction::denominators[5] += sqr(pt);

      if( pt != 0 )
      {
        //AssociationType::HarmPt
        TrackFraction::denominators[6] += 1/pt;
        //AssociationType::WHarmPt
        TrackFraction::denominators[7] += 1/pt;
        //AssociationType::HarmWPt
        TrackFraction::denominators[8] += 1/pt;
        //AssociationType::HarmPtAvg
        TrackFraction::denominators[9] += 1/pt;
        //AssociationType::WHarmPtAvg
        TrackFraction::denominators[10] += 1/pt;
        //AssociationType::HarmWPtAvg
        TrackFraction::denominators[11] += 1/pt;
      }
      
      denominatorElements++;
    }    

    //AssociationType::HarmPtAvg
    if( TrackFraction::denominators[9] != 0)
      TrackFraction::denominators[9] = denominatorElements/TrackFraction::denominators[9];

    //AssociationType::WHarmPtAvg
    if( TrackFraction::denominators[10] != 0)
      TrackFraction::denominators[10] = denominatorElements/TrackFraction::denominators[10];

    //AssociationType::HarmWPtAvg
    if( TrackFraction::denominators[11] != 0)
      TrackFraction::denominators[11] = denominatorElements/TrackFraction::denominators[11];
  }

  void TrackFraction::calculateFractions(DividerType dividerType)
  {
    if( dividerType == vertexAssociation::DividerType::Reco ||
    dividerType == vertexAssociation::DividerType::RecoMatched )
    {
    //Tracks
    fractions[0] = TrackFraction::numerators[0]/TrackFraction::denominators[0];
    //Weighted
    fractions[1] = TrackFraction::numerators[1]/TrackFraction::denominators[1];
    //Pt
    fractions[2] = TrackFraction::numerators[2]/TrackFraction::denominators[2];
    //WPt
    fractions[3] = TrackFraction::numerators[3]/TrackFraction::denominators[3];
    //Pt2
    fractions[4] = TrackFraction::numerators[4]/TrackFraction::denominators[4];
    //WPt2
    fractions[5] = TrackFraction::numerators[5]/TrackFraction::denominators[5];
    //HarmPt
    fractions[6] = TrackFraction::numerators[6]/TrackFraction::denominators[6];
    //WHarmPt
    fractions[7] = TrackFraction::numerators[7]/TrackFraction::denominators[7];
    //HarmWPt
    fractions[8] = TrackFraction::numerators[8]/TrackFraction::denominators[8];
    //HarmPtAvg
    fractions[9] = TrackFraction::numerators[9]/TrackFraction::denominators[9];
    //WHarmPtAvg
    fractions[10] = TrackFraction::numerators[10]/TrackFraction::denominators[10];
    //HarmWPtAvg
    fractions[11] = TrackFraction::numerators[11]/TrackFraction::denominators[11];
    }
    //Sim or SimMatched
    else
    {
      //Tracks
      fractions[0] = TrackFraction::numerators[0]/TrackFraction::denominators[0];
      //W
      fractions[1] = TrackFraction::numerators[1]/TrackFraction::denominators[1];
      //Pt
      fractions[2] = TrackFraction::numerators[2]/TrackFraction::denominators[2];
      //WPt
      fractions[3] = TrackFraction::numerators[3]/TrackFraction::denominators[3];
      //Pt2
      fractions[4] = TrackFraction::numerators[4]/TrackFraction::denominators[4];
      //WPt2
      fractions[5] = TrackFraction::numerators[5]/TrackFraction::denominators[5];
    }
  }
}
