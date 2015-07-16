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
																sharedTrackFraction = cms.untracked.double(-1),
																associationMode = cms.string(""),
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


from SimTracker.VertexAssociation import VertexAssociatorByPositionAndTracks_cfi

associatorByTrackgt01 = VertexAssociatorByPositionAndTracks_cfi.VertexAssociatorByPositionAndTracks.clone(sharedTrackFraction=0.1,associationMode = cms.string(""))
vertexAnalysisByTrackgt01 = vertexAnalysis.clone(vertexAssociator="associatorByTrackgt01", root_folder = cms.untracked.string("Vertexing/PrimaryVertexVByTrackgt01"))

associatorByTrackgt03 = VertexAssociatorByPositionAndTracks_cfi.VertexAssociatorByPositionAndTracks.clone(sharedTrackFraction=0.3,associationMode = cms.string(""))
vertexAnalysisByTrackgt03 = vertexAnalysis.clone(vertexAssociator="associatorByTrackgt03", root_folder = cms.untracked.string("Vertexing/PrimaryVertexVByTrackgt03"))

associatorByTrackgt05 = VertexAssociatorByPositionAndTracks_cfi.VertexAssociatorByPositionAndTracks.clone(sharedTrackFraction=0.5,associationMode = cms.string(""))
vertexAnalysisByTrackgt05 = vertexAnalysis.clone(vertexAssociator="associatorByTrackgt05", root_folder = cms.untracked.string("Vertexing/PrimaryVertexVByTrackgt05"))

#print dir(VertexAssociatorByPositionAndTracks_cfi)
#print dir(VertexAssociatorByPositionAndTracks_cfi.VertexAssociatorByPositionAndTracks)

associatorByPtgt01 = VertexAssociatorByPositionAndTracks_cfi.VertexAssociatorByPositionAndTracks.clone(sharedTrackFraction=0.1,associationMode = cms.string("Pt"))
vertexAnalysisByPtgt01 = vertexAnalysis.clone(vertexAssociator="associatorByPtgt01", root_folder = cms.untracked.string("Vertexing/PrimaryVertexVByPtgt01"))

associatorByPtgt03 = VertexAssociatorByPositionAndTracks_cfi.VertexAssociatorByPositionAndTracks.clone(sharedTrackFraction=0.3,associationMode = cms.string("Pt"))
vertexAnalysisByPtgt03 = vertexAnalysis.clone(vertexAssociator="associatorByPtgt03", root_folder = cms.untracked.string("Vertexing/PrimaryVertexVByPtgt03"))

associatorByPtgt05 = VertexAssociatorByPositionAndTracks_cfi.VertexAssociatorByPositionAndTracks.clone(sharedTrackFraction=0.5,associationMode = cms.string("Pt"))
vertexAnalysisByPtgt05 = vertexAnalysis.clone(vertexAssociator="associatorByPtgt05", root_folder = cms.untracked.string("Vertexing/PrimaryVertexVByPtgt05"))



"""
vertexAnalysisByTrackgt01 = cms.EDAnalyzer(
	"PrimaryVertexAnalyzer4PUSlimmed",
  use_only_charged_tracks = cms.untracked.bool(True),
  verbose = cms.untracked.bool(False),
  sigma_z_match = cms.untracked.double(3.0),
  abs_z_match = cms.untracked.double(0.1),
	sharedTrackFraction = cms.untracked.double(0.1),
	sharedMomentumFraction = cms.untracked.double(0.1),
	associationMode = cms.untracked.int32(5),
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
associatorByPt2gt01 = VertexAssociatorByPositionAndTracks_cfi.VertexAssociatorByPositionAndTracks.clone(sharedTrackFraction=0.1, associationMode = cms.string("Pt2"))
vertexAnalysisByPt2gt01 = vertexAnalysis.clone(vertexAssociator="associatorByPt2gt01", root_folder = cms.untracked.string("Vertexing/PrimaryVertexVByPt2gt01"))

associatorByPt2gt03 = VertexAssociatorByPositionAndTracks_cfi.VertexAssociatorByPositionAndTracks.clone(sharedTrackFraction=0.3, associationMode = cms.string("Pt2"))
vertexAnalysisByPt2gt03 = vertexAnalysis.clone(vertexAssociator="associatorByPt2gt03", root_folder = cms.untracked.string("Vertexing/PrimaryVertexVByPt2gt03"))

associatorByPt2gt05 = VertexAssociatorByPositionAndTracks_cfi.VertexAssociatorByPositionAndTracks.clone(sharedTrackFraction=0.5, associationMode = cms.string("Pt2"))
vertexAnalysisByPt2gt05 = vertexAnalysis.clone(vertexAssociator="associatorByPt2gt05", root_folder = cms.untracked.string("Vertexing/PrimaryVertexVByPt2gt05"))

"""
vertexAnalysisByPt2gt01 = cms.EDAnalyzer(
	#sharedMomentumFraction = -1.0,
	#vertexAssociator = cms.untracked.InputTag("associatorByPt2gt01"),

	"PrimaryVertexAnalyzer4PUSlimmed",
  use_only_charged_tracks = cms.untracked.bool(True),
  verbose = cms.untracked.bool(False),
  sigma_z_match = cms.untracked.double(3.0),
  abs_z_match = cms.untracked.double(0.1),
	sharedTrackFraction = cms.untracked.double(0.1),
	sharedMomentumFraction = cms.untracked.double(0.1),
	associationMode = cms.untracked.int32(6),
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
"""

#associatorByHarmPt2gt01 = VertexAssociatorByPositionAndTracks_cfi.VertexAssociatorByPositionAndTracks.clone(sharedMomentumFraction=0.0, momentumAssociationMode="HarmPt2")
associatorByHarmPt2gt01 = VertexAssociatorByPositionAndTracks_cfi.VertexAssociatorByPositionAndTracks.clone(sharedTrackFraction=0.1,associationMode = cms.string("WHarmPt"))
vertexAnalysisByHarmPt2gt01 = vertexAnalysis.clone(vertexAssociator="associatorByHarmPt2gt01", root_folder = cms.untracked.string("Vertexing/PrimaryVertexVByHarmPt2gt01"))

associatorByHarmPt2gt03 = VertexAssociatorByPositionAndTracks_cfi.VertexAssociatorByPositionAndTracks.clone(sharedTrackFraction=0.3,associationMode = cms.string("WHarmPt"))
vertexAnalysisByHarmPt2gt03 = vertexAnalysis.clone(vertexAssociator="associatorByHarmPt2gt03", root_folder = cms.untracked.string("Vertexing/PrimaryVertexVByHarmPt2gt03"))

associatorByHarmPt2gt05 = VertexAssociatorByPositionAndTracks_cfi.VertexAssociatorByPositionAndTracks.clone(sharedTrackFraction=0.5,associationMode = cms.string("WHarmPt"))
vertexAnalysisByHarmPt2gt05 = vertexAnalysis.clone(vertexAssociator="associatorByHarmPt2gt05", root_folder = cms.untracked.string("Vertexing/PrimaryVertexVByHarmPt2gt05"))

"""
vertexAnalysisByHarmPt2gt01 = cms.EDAnalyzer(
	#sharedMomentumFraction=-1.0,
	#momentumAssociationMode="HarmPt2",
	#vertexAssociator = cms.untracked.InputTag("associatorByHarmPt2gt01"),

	"PrimaryVertexAnalyzer4PUSlimmed",
  use_only_charged_tracks = cms.untracked.bool(True),
  verbose = cms.untracked.bool(False),
  sigma_z_match = cms.untracked.double(3.0),
  abs_z_match = cms.untracked.double(0.1),
	sharedTrackFraction = cms.untracked.double(0.1),
	sharedMomentumFraction = cms.untracked.double(0.1),
	associationMode = cms.untracked.int32(4),
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
"""

#associatorByHarmPt2Avggt01 = VertexAssociatorByPositionAndTracks_cfi.VertexAssociatorByPositionAndTracks.clone(sharedMomentumFraction=0.0, momentumAssociationMode="HarmPt2Avg")
associatorByHarmPt2Avggt01 = VertexAssociatorByPositionAndTracks_cfi.VertexAssociatorByPositionAndTracks.clone(sharedTrackFraction=0.1, associationMode= cms.string("HarmPt2"))
vertexAnalysisByHarmPt2Avggt01 = vertexAnalysis.clone(vertexAssociator="associatorByHarmPt2Avggt01", root_folder = cms.untracked.string("Vertexing/PrimaryVertexVByHarmPt2Avggt01"))

associatorByHarmPt2Avggt03 = VertexAssociatorByPositionAndTracks_cfi.VertexAssociatorByPositionAndTracks.clone(sharedTrackFraction=0.3, associationMode= cms.string("HarmPt2"))
vertexAnalysisByHarmPt2Avggt03 = vertexAnalysis.clone(vertexAssociator="associatorByHarmPt2Avggt03", root_folder = cms.untracked.string("Vertexing/PrimaryVertexVByHarmPt2Avggt03"))

associatorByHarmPt2Avggt05 = VertexAssociatorByPositionAndTracks_cfi.VertexAssociatorByPositionAndTracks.clone(sharedTrackFraction=0.5, associationMode= cms.string("HarmPt2"))
vertexAnalysisByHarmPt2Avggt05 = vertexAnalysis.clone(vertexAssociator="associatorByHarmPt2Avggt01", root_folder = cms.untracked.string("Vertexing/PrimaryVertexVByHarmPt2Avggt05"))

"""
vertexAnalysisByHarmPt2Avggt01 = cms.EDAnalyzer(
	#sharedMomentumFraction=-1.0, 
	#momentumAssociationMode="HarmPt2Avg",
	#vertexAssociator = cms.untracked.InputTag("associatorByHarmPt2Avggt01"),

	"PrimaryVertexAnalyzer4PUSlimmed",
  use_only_charged_tracks = cms.untracked.bool(True),
  verbose = cms.untracked.bool(False),
  sigma_z_match = cms.untracked.double(3.0),
  abs_z_match = cms.untracked.double(0.1),
	sharedTrackFraction = cms.untracked.double(0.1),
	sharedMomentumFraction = cms.untracked.double(0.1),
	associationMode = cms.untracked.int32(2),
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
	* associatorByTrackgt01
  * vertexAnalysisByTrackgt01
	* associatorByTrackgt03
  * vertexAnalysisByTrackgt03
	* associatorByTrackgt05
  * vertexAnalysisByTrackgt05
	* associatorByPtgt01
  * vertexAnalysisByPtgt01
	* associatorByPtgt03
  * vertexAnalysisByPtgt03
	* associatorByPtgt05
  * vertexAnalysisByPtgt05
	* associatorByPt2gt01
  * vertexAnalysisByPt2gt01
	* associatorByPt2gt03
  * vertexAnalysisByPt2gt03
	* associatorByPt2gt05
  * vertexAnalysisByPt2gt05
	* associatorByHarmPt2gt01
  * vertexAnalysisByHarmPt2gt01
	* associatorByHarmPt2gt03
  * vertexAnalysisByHarmPt2gt03
	* associatorByHarmPt2gt05
  * vertexAnalysisByHarmPt2gt05
	* associatorByHarmPt2Avggt01
  * vertexAnalysisByHarmPt2Avggt01
	* associatorByHarmPt2Avggt03
  * vertexAnalysisByHarmPt2Avggt03
	* associatorByHarmPt2Avggt05
  * vertexAnalysisByHarmPt2Avggt05
)

