#include "SimTracker/VertexAssociation/interface/calculateVertexSharedTracks.h"

namespace vertexAssociation
{
  std::string associationTypeEnumToString(const AssociationType& s)
  {
    if(s == vertexAssociation::AssociationType::NumberOfTracks)
      return "NumberOfTracks";
    else if(s == AssociationType::Weighted)
      return "Weighted";
    else if(s == AssociationType::Pt)
      return "Pt";
    else if(s == AssociationType::WPt)
      return "WPt";
    else if(s == AssociationType::Pt2)
      return "Pt2";
    else if(s == AssociationType::WPt2)
      return "WPt2";
    else if(s == AssociationType::HarmPt)
      return "HarmPt";
    else if(s == AssociationType::WHarmPt)
      return "WHarmPt";
    else if(s == AssociationType::HarmWPt)
      return "HarmWPt";
    else if(s == AssociationType::HarmPtAvg)
      return "HarmPtAvg";
    else if(s == AssociationType::WHarmPtAvg)
      return "WHarmPtAvg";
    else if(s == AssociationType::HarmWPtAvg)
      return "HarmWPtAvg";
    else
      throw cms::Exception("Configuration") << "Invalid AssociationType";
  }
}
