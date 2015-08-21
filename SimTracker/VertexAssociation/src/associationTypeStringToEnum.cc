#include "SimTracker/VertexAssociation/interface/calculateVertexSharedTracks.h"

namespace vertexAssociation
{
  AssociationType associationTypeStringToEnum(const std::string& s)
  {
    if(s.compare("Tracks") == 0)
      return AssociationType::NumberOfTracks;
    else if(s.compare("Weighted") == 0)
      return AssociationType::Weighted;
    else if(s.compare("Pt") == 0)
      return AssociationType::Pt;
    else if(s.compare("WPt") == 0)
      return AssociationType::WPt;
    else if(s.compare("Pt2") == 0)
      return AssociationType::Pt2;
    else if(s.compare("WPt2") == 0)
      return AssociationType::WPt2;
    else if(s.compare("HarmPt") == 0)
      return AssociationType::HarmPt;
    else if(s.compare("WHarmPt") == 0)
      return AssociationType::WHarmPt;
    else if(s.compare("HarmWPt") == 0)
      return AssociationType::HarmWPt;
    else if(s.compare("HarmPtAvg") == 0)
      return AssociationType::HarmPtAvg;
    else if(s.compare("WHarmPtAvg") == 0)
      return AssociationType::WHarmPtAvg;
    else if(s.compare("HarmWPtAvg") == 0)
      return AssociationType::HarmWPtAvg;
    else
      throw cms::Exception("Configuration") << "Invalid string '" << s <<
    "' for AssociationType enumeration";
  }
}
