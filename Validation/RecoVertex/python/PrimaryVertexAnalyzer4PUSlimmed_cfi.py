import FWCore.ParameterSet.Config as cms

selectedOfflinePrimaryVertices = cms.EDFilter("VertexSelector",
                                               src = cms.InputTag('offlinePrimaryVertices'),
                                               cut = cms.string("isValid & ndof > 4 & tracksSize > 0 & abs(z) <= 24 & abs(position.Rho) <= 2."),
                                               filter = cms.bool(False)
)

selectedOfflinePrimaryVerticesWithBS = selectedOfflinePrimaryVertices.clone()
selectedOfflinePrimaryVerticesWithBS.src = cms.InputTag('offlinePrimaryVerticesWithBS')

#selectedPixelVertices = selectedOfflinePrimaryVertices.clone()
#selectedPixelVertices.src = cms.InputTag('pixelVertices')

vertexAnalysis = cms.EDAnalyzer("PrimaryVertexAnalyzer4PUSlimmed",
                                use_only_charged_tracks = cms.untracked.bool(True),
                                verbose = cms.untracked.bool(False),
                                sigma_z_match = cms.untracked.double(3.0),
                                abs_z_match = cms.untracked.double(0.1),
                                root_folder = cms.untracked.string("Vertexing/PrimaryVertexV"),
                                recoTrackProducer = cms.untracked.InputTag("generalTracks"),
                                trackingParticleCollection = cms.untracked.InputTag("mix", "MergedTrackTruth"),
                                trackingVertexCollection = cms.untracked.InputTag("mix", "MergedTrackTruth"),
                                trackAssociatorMap = cms.untracked.InputTag("trackingParticleRecoTrackAsssociation"),
                                vertexAssociator = cms.untracked.InputTag("VertexAssociatorByPositionAndTracks"),
                                vertexRecoCollections = cms.VInputTag("offlinePrimaryVertices",
                                                                      "offlinePrimaryVerticesWithBS",
#                                                                      "pixelVertices",
                                                                      "selectedOfflinePrimaryVertices",
                                                                      "selectedOfflinePrimaryVerticesWithBS",
#                                                                      "selectedPixelVertices"
                                ),
)


from SimTracker.VertexAssociation import VertexAssociatorByPositionAndTracks

associatorByTrackgt01 = VertexAssociatorByPositionAndTracks.clone(sharedTrackFraction=0.1)
vertexAnalysisByTrackgt01 = vertexAnalysis.clone(
	vertexAssociator = "associatorByTrackgt01",
  root_folder = "Vertexing/PrimaryVertexVByTrackgt01"
)

associatorByPt2gt01 = VertexAssociatorByPositionAndTracks.clone(sharedMomentumFraction=0.1)
vertexAnalysisByPt2gt01 = vertexAnalysis.clone(
	vertexAssociator = "associatorByPt2gt01",
  root_folder = "Vertexing/PrimaryVertexVByPt2gt01"
)

associatorByHarmPt2gt01 = VertexAssociatorByPositionAndTracks.clone(sharedMomentumFraction=0.1, momentumAssociationMode="HarmPt2")
vertexAnalysisByHarmPt2gt01 = vertexAnalysis.clone(
	vertexAssociator = "associatorByHarmPt2gt01",
  root_folder = "Vertexing/PrimaryVertexVByHarmPt2gt01"
)

associatorByHarmPt2Avggt01 = VertexAssociatorByPositionAndTracks.clone(sharedMomentumFraction=0.1, momentumAssociationMode="HarmPt2Avg")
vertexAnalysisByHarmPt2gt01 = vertexAnalysis.clone(
	vertexAssociator = "associatorByHarmPt2Avggt01",
  root_folder = "Vertexing/PrimaryVertexVByHarmPt2Avggt01"
)

vertexAnalysisSequence = cms.Sequence(cms.ignore(selectedOfflinePrimaryVertices)
                                      * cms.ignore(selectedOfflinePrimaryVerticesWithBS)
#                                      * cms.ignore(selectedPixelVertices)
                                      * vertexAnalysis
	* associatorByTrackgt01
  * vertexAnalysisByTrackgt01
	* associatorByPt2gt01
  * vertexAnalysisByPt2gt01
	* associatorByHarmPt2gt01
  * vertexAnalysisByHarmPt2gt01
	* associatorByHarmPt2Avggt01
  * vertexAnalysisByHarmPt2Avggt01
)

