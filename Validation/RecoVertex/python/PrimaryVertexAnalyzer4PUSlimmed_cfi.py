import FWCore.ParameterSet.Config as cms

selectedOfflinePrimaryVertices = cms.EDFilter("VertexSelector",
                                               src = cms.InputTag('offlinePrimaryVertices'),
                                               cut = cms.string("isValid & ndof > 4 & tracksSize > 0 & abs(z) <= 24 & abs(position.Rho) <= 2."),
                                               filter = cms.bool(False)
)

selectedOfflinePrimaryVerticesWithBS = selectedOfflinePrimaryVertices.clone()
selectedOfflinePrimaryVerticesWithBS.src = cms.InputTag('offlinePrimaryVerticesWithBS')

selectedOfflinePrimaryVertices1 = cms.EDFilter("VertexSelector",
                                               src = cms.InputTag('offlinePrimaryVertices1'),
                                               cut = cms.string("isValid & ndof > 4 & tracksSize > 0 & abs(z) <= 24 & abs(position.Rho) <= 2."),
                                               filter = cms.bool(False)
)

selectedOfflinePrimaryVerticesWithBS1 = selectedOfflinePrimaryVertices.clone()
selectedOfflinePrimaryVerticesWithBS1.src = cms.InputTag('offlinePrimaryVerticesWithBS1')

selectedOfflinePrimaryVertices2 = cms.EDFilter("VertexSelector",
                                               src = cms.InputTag('offlinePrimaryVertices2'),
                                               cut = cms.string("isValid & ndof > 4 & tracksSize > 0 & abs(z) <= 24 & abs(position.Rho) <= 2."),
                                               filter = cms.bool(False)
)

selectedOfflinePrimaryVerticesWithBS2 = selectedOfflinePrimaryVertices.clone()
selectedOfflinePrimaryVerticesWithBS2.src = cms.InputTag('offlinePrimaryVerticesWithBS2')

selectedOfflinePrimaryVertices3 = cms.EDFilter("VertexSelector",
                                               src = cms.InputTag('offlinePrimaryVertices3'),
                                               cut = cms.string("isValid & ndof > 4 & tracksSize > 0 & abs(z) <= 24 & abs(position.Rho) <= 2."),
                                               filter = cms.bool(False)
)

selectedOfflinePrimaryVerticesWithBS3 = selectedOfflinePrimaryVertices.clone()
selectedOfflinePrimaryVerticesWithBS3.src = cms.InputTag('offlinePrimaryVerticesWithBS3')

#selectedPixelVertices = selectedOfflinePrimaryVertices.clone()
#selectedPixelVertices.src = cms.InputTag('pixelVertices')

vertexAnalysis = cms.EDAnalyzer("PrimaryVertexAnalyzer4PUSlimmed",
                                use_only_charged_tracks = cms.untracked.bool(True),
                                verbose = cms.untracked.bool(False),
                                sigma_z_match = cms.untracked.double(3.0),
                                abs_z_match = cms.untracked.double(0.1),
																sharedTrackFraction = cms.untracked.double(-1.0),
																sharedMomentumFraction = cms.untracked.double(-1.0),
																momentumAssociationMode = cms.untracked.int32(5),
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

#from SimTracker.VertexAssociation import VertexAssociatorByPositionAndTracks_cfi

#print dir(VertexAssociatorByPositionAndTracks_cfi)
#print dir(VertexAssociatorByPositionAndTracks_cfi.VertexAssociatorByPositionAndTracks)

#associatorByTrackgt01 = VertexAssociatorByPositionAndTracks_cfi.VertexAssociatorByPositionAndTracks.clone(sharedTrackFraction=0.0)

vertexAnalysisByTrackgt01 = cms.EDAnalyzer(
	"PrimaryVertexAnalyzer4PUSlimmed",
  use_only_charged_tracks = cms.untracked.bool(True),
  verbose = cms.untracked.bool(False),
  sigma_z_match = cms.untracked.double(3.0),
  abs_z_match = cms.untracked.double(0.1),
	sharedTrackFraction = cms.untracked.double(-1.0),
	sharedMomentumFraction = cms.untracked.double(-1.0),
	momentumAssociationMode = cms.untracked.int32(5),
  root_folder = cms.untracked.string("Vertexing/PrimaryVertexVByTrackgt01"),
	recoTrackProducer = cms.untracked.InputTag("generalTracks"),
  trackingParticleCollection = cms.untracked.InputTag("mix", "MergedTrackTruth"),
  trackingVertexCollection = cms.untracked.InputTag("mix", "MergedTrackTruth"),
  trackAssociatorMap = cms.untracked.InputTag("trackingParticleRecoTrackAsssociation"),
	vertexAssociator = cms.untracked.InputTag("VertexAssociatorByPositionAndTracks"),
	vertexRecoCollections = cms.VInputTag("offlinePrimaryVertices1",
																				"offlinePrimaryVerticesWithBS1",
																				"selectedOfflinePrimaryVertices1",
                                        "selectedOfflinePrimaryVerticesWithBS1",
																				)
)

#associatorByPt2gt01 = VertexAssociatorByPositionAndTracks_cfi.VertexAssociatorByPositionAndTracks.clone(sharedMomentumFraction=0.0)
vertexAnalysisByPt2gt01 = cms.EDAnalyzer(
	#sharedMomentumFraction = -1.0,
	#vertexAssociator = cms.untracked.InputTag("associatorByPt2gt01"),

	"PrimaryVertexAnalyzer4PUSlimmed",
  use_only_charged_tracks = cms.untracked.bool(True),
  verbose = cms.untracked.bool(False),
  sigma_z_match = cms.untracked.double(3.0),
  abs_z_match = cms.untracked.double(0.1),
	sharedTrackFraction = cms.untracked.double(-1.0),
	sharedMomentumFraction = cms.untracked.double(-1.0),
	momentumAssociationMode = cms.untracked.int32(5),
 	root_folder = cms.untracked.string("Vertexing/PrimaryVertexVByPt2gt01"),
	recoTrackProducer = cms.untracked.InputTag("generalTracks"),
  trackingParticleCollection = cms.untracked.InputTag("mix", "MergedTrackTruth"),
  trackingVertexCollection = cms.untracked.InputTag("mix", "MergedTrackTruth"),
  trackAssociatorMap = cms.untracked.InputTag("trackingParticleRecoTrackAsssociation"),
	vertexAssociator = cms.untracked.InputTag("VertexAssociatorByPositionAndTracks"),
	vertexRecoCollections = cms.VInputTag("offlinePrimaryVertices2",
																				"offlinePrimaryVerticesWithBS2",
																				"selectedOfflinePrimaryVertices2",
                                        "selectedOfflinePrimaryVerticesWithBS2",
																				)
)

#associatorByHarmPt2gt01 = VertexAssociatorByPositionAndTracks_cfi.VertexAssociatorByPositionAndTracks.clone(sharedMomentumFraction=0.0, momentumAssociationMode="HarmPt2")
vertexAnalysisByHarmPt2gt01 = cms.EDAnalyzer(
	#sharedMomentumFraction=-1.0,
	#momentumAssociationMode="HarmPt2",
	#vertexAssociator = cms.untracked.InputTag("associatorByHarmPt2gt01"),

	"PrimaryVertexAnalyzer4PUSlimmed",
  use_only_charged_tracks = cms.untracked.bool(True),
  verbose = cms.untracked.bool(False),
  sigma_z_match = cms.untracked.double(3.0),
  abs_z_match = cms.untracked.double(0.1),
	sharedTrackFraction = cms.untracked.double(-1.0),
	sharedMomentumFraction = cms.untracked.double(-1.0),
	momentumAssociationMode = cms.untracked.int32(5),
  root_folder = cms.untracked.string("Vertexing/PrimaryVertexVByHarmPt2gt01"),
	recoTrackProducer = cms.untracked.InputTag("generalTracks"),
  trackingParticleCollection = cms.untracked.InputTag("mix", "MergedTrackTruth"),
  trackingVertexCollection = cms.untracked.InputTag("mix", "MergedTrackTruth"),
  trackAssociatorMap = cms.untracked.InputTag("trackingParticleRecoTrackAsssociation"),
	vertexAssociator = cms.untracked.InputTag("VertexAssociatorByPositionAndTracks"),
	vertexRecoCollections = cms.VInputTag("offlinePrimaryVertices3",
																				"offlinePrimaryVerticesWithBS3",
																				"selectedOfflinePrimaryVertices3",
                                        "selectedOfflinePrimaryVerticesWithBS3",
																				)
)

#associatorByHarmPt2Avggt01 = VertexAssociatorByPositionAndTracks_cfi.VertexAssociatorByPositionAndTracks.clone(sharedMomentumFraction=0.0, momentumAssociationMode="HarmPt2Avg")
vertexAnalysisByHarmPt2Avggt01 = cms.EDAnalyzer(
	#sharedMomentumFraction=-1.0, 
	#momentumAssociationMode="HarmPt2Avg",
	#vertexAssociator = cms.untracked.InputTag("associatorByHarmPt2Avggt01"),

	"PrimaryVertexAnalyzer4PUSlimmed",
  use_only_charged_tracks = cms.untracked.bool(True),
  verbose = cms.untracked.bool(False),
  sigma_z_match = cms.untracked.double(3.0),
  abs_z_match = cms.untracked.double(0.1),
	sharedTrackFraction = cms.untracked.double(-1.0),
	sharedMomentumFraction = cms.untracked.double(-1.0),
	momentumAssociationMode = cms.untracked.int32(5),
  root_folder = cms.untracked.string("Vertexing/PrimaryVertexVByHarmPt2Avggt01"),
	recoTrackProducer = cms.untracked.InputTag("generalTracks"),
  trackingParticleCollection = cms.untracked.InputTag("mix", "MergedTrackTruth"),
  trackingVertexCollection = cms.untracked.InputTag("mix", "MergedTrackTruth"),
  trackAssociatorMap = cms.untracked.InputTag("trackingParticleRecoTrackAsssociation"),
	vertexAssociator = cms.untracked.InputTag("VertexAssociatorByPositionAndTracks"),
	vertexRecoCollections = cms.VInputTag("offlinePrimaryVertices4",
																				"offlinePrimaryVerticesWithBS4",
																				"selectedOfflinePrimaryVertices4",
                                        "selectedOfflinePrimaryVerticesWithBS4",
																				)
)

vertexAnalysisSequence = cms.Sequence(cms.ignore(selectedOfflinePrimaryVertices)
                                      * cms.ignore(selectedOfflinePrimaryVerticesWithBS)
#                                      * cms.ignore(selectedPixelVertices)
                                      * vertexAnalysis
#	* associatorByTrackgt01
#  * vertexAnalysisByTrackgt01
#	* associatorByPt2gt01
#  * vertexAnalysisByPt2gt01
#	* associatorByHarmPt2gt01
#  * vertexAnalysisByHarmPt2gt01
#	* associatorByHarmPt2Avggt01
#  * vertexAnalysisByHarmPt2Avggt01
)

