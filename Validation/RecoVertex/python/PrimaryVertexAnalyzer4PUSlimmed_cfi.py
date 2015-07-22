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


associatorByWTrackgt01 = VertexAssociatorByPositionAndTracks_cfi.VertexAssociatorByPositionAndTracks.clone(sharedTrackFraction=0.1,associationMode = cms.string("W"))
vertexAnalysisByWTrackgt01 = vertexAnalysis.clone(vertexAssociator="associatorByWTrackgt01", root_folder = cms.untracked.string("Vertexing/PrimaryVertexVByWTrackgt01"))

associatorByWTrackgt03 = VertexAssociatorByPositionAndTracks_cfi.VertexAssociatorByPositionAndTracks.clone(sharedTrackFraction=0.3,associationMode = cms.string("W"))
vertexAnalysisByWTrackgt03 = vertexAnalysis.clone(vertexAssociator="associatorByWTrackgt03", root_folder = cms.untracked.string("Vertexing/PrimaryVertexVByWTrackgt03"))

associatorByWTrackgt05 = VertexAssociatorByPositionAndTracks_cfi.VertexAssociatorByPositionAndTracks.clone(sharedTrackFraction=0.5,associationMode = cms.string("W"))
vertexAnalysisByWTrackgt05 = vertexAnalysis.clone(vertexAssociator="associatorByWTrackgt05", root_folder = cms.untracked.string("Vertexing/PrimaryVertexVByWTrackgt05"))


#print dir(VertexAssociatorByPositionAndTracks_cfi)
#print dir(VertexAssociatorByPositionAndTracks_cfi.VertexAssociatorByPositionAndTracks)

associatorByPtgt01 = VertexAssociatorByPositionAndTracks_cfi.VertexAssociatorByPositionAndTracks.clone(sharedTrackFraction=0.1,associationMode = cms.string("Pt"))
vertexAnalysisByPtgt01 = vertexAnalysis.clone(vertexAssociator="associatorByPtgt01", root_folder = cms.untracked.string("Vertexing/PrimaryVertexVByPtgt01"))

associatorByPtgt03 = VertexAssociatorByPositionAndTracks_cfi.VertexAssociatorByPositionAndTracks.clone(sharedTrackFraction=0.3,associationMode = cms.string("Pt"))
vertexAnalysisByPtgt03 = vertexAnalysis.clone(vertexAssociator="associatorByPtgt03", root_folder = cms.untracked.string("Vertexing/PrimaryVertexVByPtgt03"))

associatorByPtgt05 = VertexAssociatorByPositionAndTracks_cfi.VertexAssociatorByPositionAndTracks.clone(sharedTrackFraction=0.5,associationMode = cms.string("Pt"))
vertexAnalysisByPtgt05 = vertexAnalysis.clone(vertexAssociator="associatorByPtgt05", root_folder = cms.untracked.string("Vertexing/PrimaryVertexVByPtgt05"))


associatorByWPtgt01 = VertexAssociatorByPositionAndTracks_cfi.VertexAssociatorByPositionAndTracks.clone(sharedTrackFraction=0.1,associationMode = cms.string("WPt"))
vertexAnalysisByWPtgt01 = vertexAnalysis.clone(vertexAssociator="associatorByWPtgt01", root_folder = cms.untracked.string("Vertexing/PrimaryVertexVByPtgt01"))

associatorByWPtgt03 = VertexAssociatorByPositionAndTracks_cfi.VertexAssociatorByPositionAndTracks.clone(sharedTrackFraction=0.3,associationMode = cms.string("WPt"))
vertexAnalysisByWPtgt03 = vertexAnalysis.clone(vertexAssociator="associatorByWPtgt03", root_folder = cms.untracked.string("Vertexing/PrimaryVertexVByPtgt03"))

associatorByWPtgt05 = VertexAssociatorByPositionAndTracks_cfi.VertexAssociatorByPositionAndTracks.clone(sharedTrackFraction=0.5,associationMode = cms.string("WPt"))
vertexAnalysisByWPtgt05 = vertexAnalysis.clone(vertexAssociator="associatorByWPtgt05", root_folder = cms.untracked.string("Vertexing/PrimaryVertexVByPtgt05"))



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
"""

#associatorByPt2gt01 = VertexAssociatorByPositionAndTracks_cfi.VertexAssociatorByPositionAndTracks.clone(sharedMomentumFraction=0.0)
associatorByPt2gt01 = VertexAssociatorByPositionAndTracks_cfi.VertexAssociatorByPositionAndTracks.clone(sharedTrackFraction=0.1, associationMode = cms.string("Pt2"))
vertexAnalysisByPt2gt01 = vertexAnalysis.clone(vertexAssociator="associatorByPt2gt01", root_folder = cms.untracked.string("Vertexing/PrimaryVertexVByPt2gt01"))

associatorByPt2gt03 = VertexAssociatorByPositionAndTracks_cfi.VertexAssociatorByPositionAndTracks.clone(sharedTrackFraction=0.3, associationMode = cms.string("Pt2"))
vertexAnalysisByPt2gt03 = vertexAnalysis.clone(vertexAssociator="associatorByPt2gt03", root_folder = cms.untracked.string("Vertexing/PrimaryVertexVByPt2gt03"))

associatorByPt2gt05 = VertexAssociatorByPositionAndTracks_cfi.VertexAssociatorByPositionAndTracks.clone(sharedTrackFraction=0.5, associationMode = cms.string("Pt2"))
vertexAnalysisByPt2gt05 = vertexAnalysis.clone(vertexAssociator="associatorByPt2gt05", root_folder = cms.untracked.string("Vertexing/PrimaryVertexVByPt2gt05"))


associatorByWPt2gt01 = VertexAssociatorByPositionAndTracks_cfi.VertexAssociatorByPositionAndTracks.clone(sharedTrackFraction=0.1, associationMode = cms.string("WPt2"))
vertexAnalysisByWPt2gt01 = vertexAnalysis.clone(vertexAssociator="associatorByWPt2gt01", root_folder = cms.untracked.string("Vertexing/PrimaryVertexVByPt2gt01"))

associatorByWPt2gt03 = VertexAssociatorByPositionAndTracks_cfi.VertexAssociatorByPositionAndTracks.clone(sharedTrackFraction=0.3, associationMode = cms.string("WPt2"))
vertexAnalysisByWPt2gt03 = vertexAnalysis.clone(vertexAssociator="associatorByWPt2gt03", root_folder = cms.untracked.string("Vertexing/PrimaryVertexVByPt2gt03"))

associatorByWPt2gt05 = VertexAssociatorByPositionAndTracks_cfi.VertexAssociatorByPositionAndTracks.clone(sharedTrackFraction=0.5, associationMode = cms.string("WPt2"))
vertexAnalysisByWPt2gt05 = vertexAnalysis.clone(vertexAssociator="associatorByWPt2gt05", root_folder = cms.untracked.string("Vertexing/PrimaryVertexVByPt2gt05"))

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
associatorByHarmPtgt01 = VertexAssociatorByPositionAndTracks_cfi.VertexAssociatorByPositionAndTracks.clone(sharedTrackFraction=0.1,associationMode = cms.string("HarmPt"))
vertexAnalysisByHarmPtgt01 = vertexAnalysis.clone(vertexAssociator="associatorByHarmPtgt01", root_folder = cms.untracked.string("Vertexing/PrimaryVertexVByHarmPtgt01"))

associatorByHarmPtgt03 = VertexAssociatorByPositionAndTracks_cfi.VertexAssociatorByPositionAndTracks.clone(sharedTrackFraction=0.3,associationMode = cms.string("HarmPt"))
vertexAnalysisByHarmPtgt03 = vertexAnalysis.clone(vertexAssociator="associatorByHarmPtgt03", root_folder = cms.untracked.string("Vertexing/PrimaryVertexVByHarmPtgt03"))

associatorByHarmPtgt05 = VertexAssociatorByPositionAndTracks_cfi.VertexAssociatorByPositionAndTracks.clone(sharedTrackFraction=0.5,associationMode = cms.string("HarmPt"))
vertexAnalysisByHarmPtgt05 = vertexAnalysis.clone(vertexAssociator="associatorByHarmPtgt05", root_folder = cms.untracked.string("Vertexing/PrimaryVertexVByHarmPtgt05"))


associatorByWHarmPtgt01 = VertexAssociatorByPositionAndTracks_cfi.VertexAssociatorByPositionAndTracks.clone(sharedTrackFraction=0.1,associationMode = cms.string("WHarmPt"))
vertexAnalysisByWHarmPtgt01 = vertexAnalysis.clone(vertexAssociator="associatorByWHarmPtgt01", root_folder = cms.untracked.string("Vertexing/PrimaryVertexVByWHarmPtgt01"))

associatorByWHarmPtgt03 = VertexAssociatorByPositionAndTracks_cfi.VertexAssociatorByPositionAndTracks.clone(sharedTrackFraction=0.3,associationMode = cms.string("WHarmPt"))
vertexAnalysisByWHarmPtgt03 = vertexAnalysis.clone(vertexAssociator="associatorByWHarmPtgt03", root_folder = cms.untracked.string("Vertexing/PrimaryVertexVByWHarmPtgt03"))

associatorByWHarmPtgt05 = VertexAssociatorByPositionAndTracks_cfi.VertexAssociatorByPositionAndTracks.clone(sharedTrackFraction=0.5,associationMode = cms.string("WHarmPt"))
vertexAnalysisByWHarmPtgt05 = vertexAnalysis.clone(vertexAssociator="associatorByWHarmPtgt05", root_folder = cms.untracked.string("Vertexing/PrimaryVertexVByWHarmPtgt05"))


associatorByHarmWPtgt01 = VertexAssociatorByPositionAndTracks_cfi.VertexAssociatorByPositionAndTracks.clone(sharedTrackFraction=0.1,associationMode = cms.string("HarmWPt"))
vertexAnalysisByHarmWPtgt01 = vertexAnalysis.clone(vertexAssociator="associatorByHarmWPtgt01", root_folder = cms.untracked.string("Vertexing/PrimaryVertexVByHarmWPtgt01"))

associatorByHarmWPtgt03 = VertexAssociatorByPositionAndTracks_cfi.VertexAssociatorByPositionAndTracks.clone(sharedTrackFraction=0.3,associationMode = cms.string("HarmWPt"))
vertexAnalysisByHarmWPtgt03 = vertexAnalysis.clone(vertexAssociator="associatorByHarmWPtgt03", root_folder = cms.untracked.string("Vertexing/PrimaryVertexVByHarmWPtgt03"))

associatorByHarmWPtgt05 = VertexAssociatorByPositionAndTracks_cfi.VertexAssociatorByPositionAndTracks.clone(sharedTrackFraction=0.5,associationMode = cms.string("HarmWPt"))
vertexAnalysisByHarmWPtgt05 = vertexAnalysis.clone(vertexAssociator="associatorByHarmWPtgt05", root_folder = cms.untracked.string("Vertexing/PrimaryVertexVByHarmWPtgt05"))




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
associatorByHarmPtAvggt01 = VertexAssociatorByPositionAndTracks_cfi.VertexAssociatorByPositionAndTracks.clone(sharedTrackFraction=0.1, associationMode= cms.string("HarmPtAvg"))
vertexAnalysisByHarmPtAvggt01 = vertexAnalysis.clone(vertexAssociator="associatorByHarmPtAvggt01", root_folder = cms.untracked.string("Vertexing/PrimaryVertexVByHarmPtAvggt01"))

associatorByHarmPtAvggt03 = VertexAssociatorByPositionAndTracks_cfi.VertexAssociatorByPositionAndTracks.clone(sharedTrackFraction=0.3, associationMode= cms.string("HarmPtAvg"))
vertexAnalysisByHarmPtAvggt03 = vertexAnalysis.clone(vertexAssociator="associatorByHarmPtAvggt03", root_folder = cms.untracked.string("Vertexing/PrimaryVertexVByHarmPtAvggt03"))

associatorByHarmPtAvggt05 = VertexAssociatorByPositionAndTracks_cfi.VertexAssociatorByPositionAndTracks.clone(sharedTrackFraction=0.5, associationMode= cms.string("HarmPtAvg"))
vertexAnalysisByHarmPtAvggt05 = vertexAnalysis.clone(vertexAssociator="associatorByHarmPtAvggt01", root_folder = cms.untracked.string("Vertexing/PrimaryVertexVByHarmPtAvggt05"))


associatorByWHarmPtAvggt01 = VertexAssociatorByPositionAndTracks_cfi.VertexAssociatorByPositionAndTracks.clone(sharedTrackFraction=0.1, associationMode= cms.string("WHarmPtAvg"))
vertexAnalysisByWHarmPtAvggt01 = vertexAnalysis.clone(vertexAssociator="associatorByWHarmPtAvggt01", root_folder = cms.untracked.string("Vertexing/PrimaryVertexVByWHarmPtAvggt01"))

associatorByWHarmPtAvggt03 = VertexAssociatorByPositionAndTracks_cfi.VertexAssociatorByPositionAndTracks.clone(sharedTrackFraction=0.3, associationMode= cms.string("WHarmPtAvg"))
vertexAnalysisByWHarmPtAvggt03 = vertexAnalysis.clone(vertexAssociator="associatorByWHarmPtAvggt03", root_folder = cms.untracked.string("Vertexing/PrimaryVertexVByWHarmPtAvggt03"))

associatorByWHarmPtAvggt05 = VertexAssociatorByPositionAndTracks_cfi.VertexAssociatorByPositionAndTracks.clone(sharedTrackFraction=0.5, associationMode= cms.string("WHarmPtAvg"))
vertexAnalysisByWHarmPtAvggt05 = vertexAnalysis.clone(vertexAssociator="associatorByWHarmPtAvggt01", root_folder = cms.untracked.string("Vertexing/PrimaryVertexVByWHarmPtAvggt05"))


associatorByHarmWPtAvggt01 = VertexAssociatorByPositionAndTracks_cfi.VertexAssociatorByPositionAndTracks.clone(sharedTrackFraction=0.1, associationMode= cms.string("HarmWPtAvg"))
vertexAnalysisByHarmWPtAvggt01 = vertexAnalysis.clone(vertexAssociator="associatorByHarmWPtAvggt01", root_folder = cms.untracked.string("Vertexing/PrimaryVertexVByHarmWPtAvggt01"))

associatorByHarmWPtAvggt03 = VertexAssociatorByPositionAndTracks_cfi.VertexAssociatorByPositionAndTracks.clone(sharedTrackFraction=0.3, associationMode= cms.string("HarmWPtAvg"))
vertexAnalysisByHarmWPtAvggt03 = vertexAnalysis.clone(vertexAssociator="associatorByHarmWPtAvggt03", root_folder = cms.untracked.string("Vertexing/PrimaryVertexVByHarmWPtAvggt03"))

associatorByHarmWPtAvggt05 = VertexAssociatorByPositionAndTracks_cfi.VertexAssociatorByPositionAndTracks.clone(sharedTrackFraction=0.5, associationMode= cms.string("HarmWPtAvg"))
vertexAnalysisByHarmWPtAvggt05 = vertexAnalysis.clone(vertexAssociator="associatorByHarmWPtAvggt01", root_folder = cms.untracked.string("Vertexing/PrimaryVertexVByHarmWPtAvggt05"))

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
"""

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
	* associatorByWTrackgt01
  * vertexAnalysisByWTrackgt01
	* associatorByWTrackgt03
  * vertexAnalysisByWTrackgt03
	* associatorByWTrackgt05
  * vertexAnalysisByWTrackgt05
	* associatorByPtgt01
  * vertexAnalysisByPtgt01
	* associatorByPtgt03
  * vertexAnalysisByPtgt03
	* associatorByPtgt05
  * vertexAnalysisByPtgt05
	* associatorByWPtgt01
  * vertexAnalysisByWPtgt01
	* associatorByWPtgt03
  * vertexAnalysisByWPtgt03
	* associatorByWPtgt05
  * vertexAnalysisByWPtgt05
	* associatorByPt2gt01
  * vertexAnalysisByPt2gt01
	* associatorByPt2gt03
  * vertexAnalysisByPt2gt03
	* associatorByPt2gt05
  * vertexAnalysisByPt2gt05
	* associatorByWPt2gt01
  * vertexAnalysisByWPt2gt01
	* associatorByWPt2gt03
  * vertexAnalysisByWPt2gt03
	* associatorByWPt2gt05
  * vertexAnalysisByWPt2gt05
	* associatorByHarmPtgt01
  * vertexAnalysisByHarmPtgt01
	* associatorByHarmPtgt03
  * vertexAnalysisByHarmPtgt03
	* associatorByHarmPtgt05
  * vertexAnalysisByHarmPtgt05
	* associatorByWHarmPtgt01
  * vertexAnalysisByWHarmPtgt01
	* associatorByWHarmPtgt03
  * vertexAnalysisByWHarmPtgt03
	* associatorByWHarmPtgt05
  * vertexAnalysisByWHarmPtgt05
	* associatorByHarmWPtgt01
  * vertexAnalysisByHarmWPtgt01
	* associatorByHarmWPtgt03
  * vertexAnalysisByHarmWPtgt03
	* associatorByHarmWPtgt05
  * vertexAnalysisByHarmWPtgt05
	* associatorByHarmPtAvggt01
  * vertexAnalysisByHarmPtAvggt01
	* associatorByHarmPtAvggt03
  * vertexAnalysisByHarmPtAvggt03
	* associatorByHarmPtAvggt05
  * vertexAnalysisByHarmPtAvggt05
	* associatorByWHarmPtAvggt01
  * vertexAnalysisByWHarmPtAvggt01
	* associatorByWHarmPtAvggt03
  * vertexAnalysisByWHarmPtAvggt03
	* associatorByWHarmPtAvggt05
  * vertexAnalysisByWHarmPtAvggt05
	* associatorByHarmWPtAvggt01
  * vertexAnalysisByHarmWPtAvggt01
	* associatorByHarmWPtAvggt03
  * vertexAnalysisByHarmWPtAvggt03
	* associatorByHarmWPtAvggt05
  * vertexAnalysisByHarmWPtAvggt05
)

