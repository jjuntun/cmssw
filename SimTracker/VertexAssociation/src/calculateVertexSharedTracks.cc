#include "SimTracker/VertexAssociation/interface/calculateVertexSharedTracks.h"

vertexAssociation::TrackFraction calculateVertexSharedTrackFractions(const reco::Vertex& recoV, const TrackingVertex& simV, const reco::SimToRecoCollection& trackSimToRecoAssociation, const reco::RecoToSimCollection& trackRecoToSimAssociation, vertexAssociation::DividerType dividerType) {

  vertexAssociation::TrackFraction trackFraction;
  
  trackFraction.calculateNumerators(recoV, simV, trackRecoToSimAssociation);

  //calculate the divider in the fraction
  if( dividerType == vertexAssociation::DividerType::Reco )
  {
    //"Reco" 
    trackFraction.calculateRecoDenominators(recoV);
  }
  else if( dividerType == vertexAssociation::DividerType::RecoMatched )
  {
    //"RecoMatched"
    trackFraction.calculateRecoMatchedDenominators(recoV, trackRecoToSimAssociation);

  }
  else if( dividerType == vertexAssociation::DividerType::Sim )
  {
    //Simulated tracks do not have weights
    //"Sim"
    trackFraction.calculateSimDenominators(simV);

  }
  else
  {
    //SimMatched
    trackFraction.calculateSimMatchedDenominators(simV, trackSimToRecoAssociation);
  }

  trackFraction.calculateFractions(dividerType);

  return trackFraction;

}

// returns TrackFractions for GenAllAssoc2Reco- plots
vertexAssociation::TrackFraction calculateVertexSharedTrackFractions(const TrackingVertex& simV, const reco::Vertex& recoV, const reco::SimToRecoCollection& trackSimToRecoAssociation, const reco::RecoToSimCollection& trackRecoToSimAssociation, vertexAssociation::DividerType dividerType) {

  vertexAssociation::TrackFraction trackFraction;
  
  trackFraction.calculateNumerators(simV, recoV, trackSimToRecoAssociation);

  //calculate the divider in the fraction
  if( dividerType == vertexAssociation::DividerType::Reco )
  {
    //"Reco"
    trackFraction.calculateRecoDenominators(recoV);
  }
  else if( dividerType == vertexAssociation::DividerType::RecoMatched )
  {
    //"RecoMatched"
    trackFraction.calculateRecoMatchedDenominators(recoV, trackRecoToSimAssociation);
  }
  else if( dividerType == vertexAssociation::DividerType::Sim )
  {
    //Simulated tracks do not have weights
    //"Sim"
    trackFraction.calculateSimDenominators(simV);

  }
  else
  {
    //"SimMatched"
    trackFraction.calculateSimMatchedDenominators(simV, trackSimToRecoAssociation);
  }

  trackFraction.calculateFractions(dividerType);

  return trackFraction;
}

