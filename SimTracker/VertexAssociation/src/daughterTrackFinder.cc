#include "SimTracker/VertexAssociation/interface/daughterTrackFinder.h"

//recursive version for simPVs

void daughterTrackFinder(TrackingVertex a, std::vector<TrackingParticleRef>& connectedTrackingParticleRefs, std::vector<TrackingVertex>& connectedTrackingVertices)	{

	//std::vector<TrackingParticle> connectedTracks;
	//bool daughterVertexFoundForThisTrack = false;
	//std::vector<TrackingVertex> foundVertices;
	//loop through all daughterTracks
	// foundVertices is a disposable stock for daughterVertices found in this function call 
	//foundVertices.insert(foundVertices.end(), simVtx);
	// connectedTrackingVertices is a stable reference to daughterVertices found in recursive function calls 
	for(TrackingParticleRefVector::iterator iTrack = a.daughterTracks_begin(); iTrack != a.daughterTracks_end(); ++iTrack)	{

		connectedTrackingParticleRefs.insert(connectedTrackingParticleRefs.end(), *iTrack);

		for(auto iSimPV = (*(iTrack->get())).decayVertices_begin(); iSimPV != (*(iTrack->get())).decayVertices_end(); ++iSimPV)	{
			TrackingVertex simVtx = *(iSimPV->get());

			connectedTrackingVertices.insert(connectedTrackingVertices.end(),simVtx);
			daughterTrackFinder(connectedTrackingVertices.back(), connectedTrackingParticleRefs, connectedTrackingVertices);
		}
		
		/*
		if(daughterVertexFoundForThisTrack)	{
			//std::vector<TrackingVertex> foundVertices = daughterTrackFinder(connectedTrackingVertices.back());
			if(foundVertices.size() > 0)	{
				for(std::vector<TrackingVertex>::iterator it = foundVertices.begin(); it != foundVertices.end(); ++it)	{
					connectedTracks.reserve(connectedTracks.size() + distance(it->daughterTracks_begin(),it->daughterTracks_end()));
					for(TrackingParticleRefVector::iterator tp = it->daughterTracks_begin(); tp != it->daughterTracks_end(); ++tp)	{
						for(std::vector<TrackingParticle>::iterator tp2 = connectedTracks.begin(); tp2 != connectedTracks.end(); ++tp2)	{
							// add only new tracks
							std::cout << "typeid(*(tp->get())).name() " << typeid(*(tp->get())).name() << " typeid(*tp2).name() " << typeid(*tp2).name() << std::endl;
							//TODO: check which is right property to compare for equality
							if( (*(tp->get())).eventId() != (*tp2).eventId()) {
								connectedTracks.insert(connectedTracks.end(), *(tp->get()));
								std::cout << "New daughterTrack added" << std::endl;
							}
						}
					}

					 //connectedTracks.insert(connectedTracks.end(),it->daughterTracks_begin(),it->daughterTracks_end());				
				}
			}

			//daughterTrackFinder(connectedTrackingVertices.back(), connectedTracks, connectedTrackingVertices, TVCollectionH);

		}
		*/
	}

}

/*
reco::Vertex r;

std::vector<reco::Vertex> connectedRecoVertices;

for(auto iTrack = r.tracks_begin(); iTrack != a.tracks_end(); ++iTrack)	{
	std::vector<PrimaryVertexAnalyzer4PUSlimmed::simPrimaryVertex> recoVs = getRecoPVs(const edm::Handle<TrackingVertexCollection>& tVC);
	for(auto iRecoV = recoPVs.begin(); iRecoPV != recoPVs.end(); ++iRecoPV)	{
		reco::Vertex recoVtx = *(iRecoPV->recVtx);
		for(auto iSrcTrack = recoVtx.sourceTracks_begin(); iSrcTrack != simVtx.sourceTracks_end(); ++iSrcTrack)	{
			if( iSrcTrack == iTrack )	{
				connectedTrackingVertices.insert(simVtx);
				break;
			}
		}
	}

}
*/

