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
																sharedTrackFraction = cms.untracked.double(0.001),
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


associatorByTrackgt001 = VertexAssociatorByPositionAndTracks_cfi.VertexAssociatorByPositionAndTracks.clone(sharedTrackFraction=0.01,associationMode = cms.string(""))
vertexAnalysisByTrackgt001 = vertexAnalysis.clone(vertexAssociator="associatorByTrackgt001", root_folder = cms.untracked.string("Vertexing/PrimaryVertexVByTrackgt001"))

associatorByTrackgt005 = VertexAssociatorByPositionAndTracks_cfi.VertexAssociatorByPositionAndTracks.clone(sharedTrackFraction=0.05,associationMode = cms.string(""))
vertexAnalysisByTrackgt005 = vertexAnalysis.clone(vertexAssociator="associatorByTrackgt005", root_folder = cms.untracked.string("Vertexing/PrimaryVertexVByTrackgt005"))

associatorByTrackgt01 = VertexAssociatorByPositionAndTracks_cfi.VertexAssociatorByPositionAndTracks.clone(sharedTrackFraction=0.1,associationMode = cms.string(""))
vertexAnalysisByTrackgt01 = vertexAnalysis.clone(vertexAssociator="associatorByTrackgt01", root_folder = cms.untracked.string("Vertexing/PrimaryVertexVByTrackgt01"))

associatorByTrackgt03 = VertexAssociatorByPositionAndTracks_cfi.VertexAssociatorByPositionAndTracks.clone(sharedTrackFraction=0.3,associationMode = cms.string(""))
vertexAnalysisByTrackgt03 = vertexAnalysis.clone(vertexAssociator="associatorByTrackgt03", root_folder = cms.untracked.string("Vertexing/PrimaryVertexVByTrackgt03"))


associatorByWTrackgt001 = VertexAssociatorByPositionAndTracks_cfi.VertexAssociatorByPositionAndTracks.clone(sharedTrackFraction=0.01,associationMode = cms.string("W"))
vertexAnalysisByWTrackgt001 = vertexAnalysis.clone(vertexAssociator="associatorByWTrackgt001", root_folder = cms.untracked.string("Vertexing/PrimaryVertexVByWTrackgt001"))

associatorByWTrackgt005 = VertexAssociatorByPositionAndTracks_cfi.VertexAssociatorByPositionAndTracks.clone(sharedTrackFraction=0.05,associationMode = cms.string("W"))
vertexAnalysisByWTrackgt005 = vertexAnalysis.clone(vertexAssociator="associatorByWTrackgt005", root_folder = cms.untracked.string("Vertexing/PrimaryVertexVByWTrackgt005"))

associatorByWTrackgt01 = VertexAssociatorByPositionAndTracks_cfi.VertexAssociatorByPositionAndTracks.clone(sharedTrackFraction=0.1,associationMode = cms.string("W"))
vertexAnalysisByWTrackgt01 = vertexAnalysis.clone(vertexAssociator="associatorByWTrackgt01", root_folder = cms.untracked.string("Vertexing/PrimaryVertexVByWTrackgt01"))

associatorByWTrackgt03 = VertexAssociatorByPositionAndTracks_cfi.VertexAssociatorByPositionAndTracks.clone(sharedTrackFraction=0.3,associationMode = cms.string("W"))
vertexAnalysisByWTrackgt03 = vertexAnalysis.clone(vertexAssociator="associatorByWTrackgt03", root_folder = cms.untracked.string("Vertexing/PrimaryVertexVByWTrackgt03"))


#print dir(VertexAssociatorByPositionAndTracks_cfi)
#print dir(VertexAssociatorByPositionAndTracks_cfi.VertexAssociatorByPositionAndTracks)

associatorByPtgt001 = VertexAssociatorByPositionAndTracks_cfi.VertexAssociatorByPositionAndTracks.clone(sharedTrackFraction=0.01,associationMode = cms.string("Pt"))
vertexAnalysisByPtgt001 = vertexAnalysis.clone(vertexAssociator="associatorByPtgt001", root_folder = cms.untracked.string("Vertexing/PrimaryVertexVByPtgt001"))

associatorByPtgt005 = VertexAssociatorByPositionAndTracks_cfi.VertexAssociatorByPositionAndTracks.clone(sharedTrackFraction=0.05,associationMode = cms.string("Pt"))
vertexAnalysisByPtgt005 = vertexAnalysis.clone(vertexAssociator="associatorByPtgt005", root_folder = cms.untracked.string("Vertexing/PrimaryVertexVByPtgt005"))

associatorByPtgt01 = VertexAssociatorByPositionAndTracks_cfi.VertexAssociatorByPositionAndTracks.clone(sharedTrackFraction=0.1,associationMode = cms.string("Pt"))
vertexAnalysisByPtgt01 = vertexAnalysis.clone(vertexAssociator="associatorByPtgt01", root_folder = cms.untracked.string("Vertexing/PrimaryVertexVByPtgt01"))

associatorByPtgt03 = VertexAssociatorByPositionAndTracks_cfi.VertexAssociatorByPositionAndTracks.clone(sharedTrackFraction=0.3,associationMode = cms.string("Pt"))
vertexAnalysisByPtgt03 = vertexAnalysis.clone(vertexAssociator="associatorByPtgt03", root_folder = cms.untracked.string("Vertexing/PrimaryVertexVByPtgt03"))


associatorByWPtgt001 = VertexAssociatorByPositionAndTracks_cfi.VertexAssociatorByPositionAndTracks.clone(sharedTrackFraction=0.01,associationMode = cms.string("WPt"))
vertexAnalysisByWPtgt001 = vertexAnalysis.clone(vertexAssociator="associatorByWPtgt001", root_folder = cms.untracked.string("Vertexing/PrimaryVertexVByWPtgt001"))

associatorByWPtgt005 = VertexAssociatorByPositionAndTracks_cfi.VertexAssociatorByPositionAndTracks.clone(sharedTrackFraction=0.05,associationMode = cms.string("WPt"))
vertexAnalysisByWPtgt005 = vertexAnalysis.clone(vertexAssociator="associatorByWPtgt005", root_folder = cms.untracked.string("Vertexing/PrimaryVertexVByWPtgt005"))

associatorByWPtgt01 = VertexAssociatorByPositionAndTracks_cfi.VertexAssociatorByPositionAndTracks.clone(sharedTrackFraction=0.1,associationMode = cms.string("WPt"))
vertexAnalysisByWPtgt01 = vertexAnalysis.clone(vertexAssociator="associatorByWPtgt01", root_folder = cms.untracked.string("Vertexing/PrimaryVertexVByWPtgt01"))

associatorByWPtgt03 = VertexAssociatorByPositionAndTracks_cfi.VertexAssociatorByPositionAndTracks.clone(sharedTrackFraction=0.3,associationMode = cms.string("WPt"))
vertexAnalysisByWPtgt03 = vertexAnalysis.clone(vertexAssociator="associatorByWPtgt03", root_folder = cms.untracked.string("Vertexing/PrimaryVertexVByWPtgt03"))



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
associatorByPt2gt001 = VertexAssociatorByPositionAndTracks_cfi.VertexAssociatorByPositionAndTracks.clone(sharedTrackFraction=0.01, associationMode = cms.string("Pt2"))
vertexAnalysisByPt2gt001 = vertexAnalysis.clone(vertexAssociator="associatorByPt2gt001", root_folder = cms.untracked.string("Vertexing/PrimaryVertexVByPt2gt001"))

associatorByPt2gt005 = VertexAssociatorByPositionAndTracks_cfi.VertexAssociatorByPositionAndTracks.clone(sharedTrackFraction=0.05, associationMode = cms.string("Pt2"))
vertexAnalysisByPt2gt005 = vertexAnalysis.clone(vertexAssociator="associatorByPt2gt005", root_folder = cms.untracked.string("Vertexing/PrimaryVertexVByPt2gt005"))

associatorByPt2gt01 = VertexAssociatorByPositionAndTracks_cfi.VertexAssociatorByPositionAndTracks.clone(sharedTrackFraction=0.1, associationMode = cms.string("Pt2"))
vertexAnalysisByPt2gt01 = vertexAnalysis.clone(vertexAssociator="associatorByPt2gt01", root_folder = cms.untracked.string("Vertexing/PrimaryVertexVByPt2gt01"))

associatorByPt2gt03 = VertexAssociatorByPositionAndTracks_cfi.VertexAssociatorByPositionAndTracks.clone(sharedTrackFraction=0.3, associationMode = cms.string("Pt2"))
vertexAnalysisByPt2gt03 = vertexAnalysis.clone(vertexAssociator="associatorByPt2gt03", root_folder = cms.untracked.string("Vertexing/PrimaryVertexVByPt2gt03"))


associatorByWPt2gt001 = VertexAssociatorByPositionAndTracks_cfi.VertexAssociatorByPositionAndTracks.clone(sharedTrackFraction=0.01, associationMode = cms.string("WPt2"))
vertexAnalysisByWPt2gt001 = vertexAnalysis.clone(vertexAssociator="associatorByWPt2gt001", root_folder = cms.untracked.string("Vertexing/PrimaryVertexVByWPt2gt001"))

associatorByWPt2gt005 = VertexAssociatorByPositionAndTracks_cfi.VertexAssociatorByPositionAndTracks.clone(sharedTrackFraction=0.05, associationMode = cms.string("WPt2"))
vertexAnalysisByWPt2gt005 = vertexAnalysis.clone(vertexAssociator="associatorByWPt2gt005", root_folder = cms.untracked.string("Vertexing/PrimaryVertexVByWPt2gt005"))

associatorByWPt2gt01 = VertexAssociatorByPositionAndTracks_cfi.VertexAssociatorByPositionAndTracks.clone(sharedTrackFraction=0.1, associationMode = cms.string("WPt2"))
vertexAnalysisByWPt2gt01 = vertexAnalysis.clone(vertexAssociator="associatorByWPt2gt01", root_folder = cms.untracked.string("Vertexing/PrimaryVertexVByWPt2gt01"))

associatorByWPt2gt03 = VertexAssociatorByPositionAndTracks_cfi.VertexAssociatorByPositionAndTracks.clone(sharedTrackFraction=0.3, associationMode = cms.string("WPt2"))
vertexAnalysisByWPt2gt03 = vertexAnalysis.clone(vertexAssociator="associatorByWPt2gt03", root_folder = cms.untracked.string("Vertexing/PrimaryVertexVByWPt2gt03"))



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

associatorByHarmPtgt001 = VertexAssociatorByPositionAndTracks_cfi.VertexAssociatorByPositionAndTracks.clone(sharedTrackFraction=0.01,associationMode = cms.string("HarmPt"))
vertexAnalysisByHarmPtgt001 = vertexAnalysis.clone(vertexAssociator="associatorByHarmPtgt001", root_folder = cms.untracked.string("Vertexing/PrimaryVertexVByHarmPtgt001"))

associatorByHarmPtgt005 = VertexAssociatorByPositionAndTracks_cfi.VertexAssociatorByPositionAndTracks.clone(sharedTrackFraction=0.05,associationMode = cms.string("HarmPt"))
vertexAnalysisByHarmPtgt005 = vertexAnalysis.clone(vertexAssociator="associatorByHarmPtgt005", root_folder = cms.untracked.string("Vertexing/PrimaryVertexVByHarmPtgt005"))

associatorByHarmPtgt01 = VertexAssociatorByPositionAndTracks_cfi.VertexAssociatorByPositionAndTracks.clone(sharedTrackFraction=0.1,associationMode = cms.string("HarmPt"))
vertexAnalysisByHarmPtgt01 = vertexAnalysis.clone(vertexAssociator="associatorByHarmPtgt01", root_folder = cms.untracked.string("Vertexing/PrimaryVertexVByHarmPtgt01"))

associatorByHarmPtgt03 = VertexAssociatorByPositionAndTracks_cfi.VertexAssociatorByPositionAndTracks.clone(sharedTrackFraction=0.3,associationMode = cms.string("HarmPt"))
vertexAnalysisByHarmPtgt03 = vertexAnalysis.clone(vertexAssociator="associatorByHarmPtgt03", root_folder = cms.untracked.string("Vertexing/PrimaryVertexVByHarmPtgt03"))


associatorByWHarmPtgt001 = VertexAssociatorByPositionAndTracks_cfi.VertexAssociatorByPositionAndTracks.clone(sharedTrackFraction=0.01,associationMode = cms.string("WHarmPt"))
vertexAnalysisByWHarmPtgt001 = vertexAnalysis.clone(vertexAssociator="associatorByWHarmPtgt001", root_folder = cms.untracked.string("Vertexing/PrimaryVertexVByWHarmPtgt001"))

associatorByWHarmPtgt005 = VertexAssociatorByPositionAndTracks_cfi.VertexAssociatorByPositionAndTracks.clone(sharedTrackFraction=0.05,associationMode = cms.string("WHarmPt"))
vertexAnalysisByWHarmPtgt005 = vertexAnalysis.clone(vertexAssociator="associatorByWHarmPtgt005", root_folder = cms.untracked.string("Vertexing/PrimaryVertexVByWHarmPtgt005"))

associatorByWHarmPtgt01 = VertexAssociatorByPositionAndTracks_cfi.VertexAssociatorByPositionAndTracks.clone(sharedTrackFraction=0.1,associationMode = cms.string("WHarmPt"))
vertexAnalysisByWHarmPtgt01 = vertexAnalysis.clone(vertexAssociator="associatorByWHarmPtgt01", root_folder = cms.untracked.string("Vertexing/PrimaryVertexVByWHarmPtgt01"))

associatorByWHarmPtgt03 = VertexAssociatorByPositionAndTracks_cfi.VertexAssociatorByPositionAndTracks.clone(sharedTrackFraction=0.3,associationMode = cms.string("WHarmPt"))
vertexAnalysisByWHarmPtgt03 = vertexAnalysis.clone(vertexAssociator="associatorByWHarmPtgt03", root_folder = cms.untracked.string("Vertexing/PrimaryVertexVByWHarmPtgt03"))



associatorByHarmWPtgt001 = VertexAssociatorByPositionAndTracks_cfi.VertexAssociatorByPositionAndTracks.clone(sharedTrackFraction=0.01,associationMode = cms.string("HarmWPt"))
vertexAnalysisByHarmWPtgt001 = vertexAnalysis.clone(vertexAssociator="associatorByHarmWPtgt001", root_folder = cms.untracked.string("Vertexing/PrimaryVertexVByHarmWPtgt001"))

associatorByHarmWPtgt005 = VertexAssociatorByPositionAndTracks_cfi.VertexAssociatorByPositionAndTracks.clone(sharedTrackFraction=0.05,associationMode = cms.string("HarmWPt"))
vertexAnalysisByHarmWPtgt005 = vertexAnalysis.clone(vertexAssociator="associatorByHarmWPtgt005", root_folder = cms.untracked.string("Vertexing/PrimaryVertexVByHarmWPtgt005"))

associatorByHarmWPtgt01 = VertexAssociatorByPositionAndTracks_cfi.VertexAssociatorByPositionAndTracks.clone(sharedTrackFraction=0.1,associationMode = cms.string("HarmWPt"))
vertexAnalysisByHarmWPtgt01 = vertexAnalysis.clone(vertexAssociator="associatorByHarmWPtgt01", root_folder = cms.untracked.string("Vertexing/PrimaryVertexVByHarmWPtgt01"))

associatorByHarmWPtgt03 = VertexAssociatorByPositionAndTracks_cfi.VertexAssociatorByPositionAndTracks.clone(sharedTrackFraction=0.3,associationMode = cms.string("HarmWPt"))
vertexAnalysisByHarmWPtgt03 = vertexAnalysis.clone(vertexAssociator="associatorByHarmWPtgt03", root_folder = cms.untracked.string("Vertexing/PrimaryVertexVByHarmWPtgt03"))




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

associatorByHarmPtAvggt001 = VertexAssociatorByPositionAndTracks_cfi.VertexAssociatorByPositionAndTracks.clone(sharedTrackFraction=0.01, associationMode= cms.string("HarmPtAvg"))
vertexAnalysisByHarmPtAvggt001 = vertexAnalysis.clone(vertexAssociator="associatorByHarmPtAvggt001", root_folder = cms.untracked.string("Vertexing/PrimaryVertexVByHarmPtAvggt001"))

associatorByHarmPtAvggt005 = VertexAssociatorByPositionAndTracks_cfi.VertexAssociatorByPositionAndTracks.clone(sharedTrackFraction=0.05, associationMode= cms.string("HarmPtAvg"))
vertexAnalysisByHarmPtAvggt005 = vertexAnalysis.clone(vertexAssociator="associatorByHarmPtAvggt005", root_folder = cms.untracked.string("Vertexing/PrimaryVertexVByHarmPtAvggt005"))

associatorByHarmPtAvggt01 = VertexAssociatorByPositionAndTracks_cfi.VertexAssociatorByPositionAndTracks.clone(sharedTrackFraction=0.1, associationMode= cms.string("HarmPtAvg"))
vertexAnalysisByHarmPtAvggt01 = vertexAnalysis.clone(vertexAssociator="associatorByHarmPtAvggt01", root_folder = cms.untracked.string("Vertexing/PrimaryVertexVByHarmPtAvggt01"))

associatorByHarmPtAvggt03 = VertexAssociatorByPositionAndTracks_cfi.VertexAssociatorByPositionAndTracks.clone(sharedTrackFraction=0.3, associationMode= cms.string("HarmPtAvg"))
vertexAnalysisByHarmPtAvggt03 = vertexAnalysis.clone(vertexAssociator="associatorByHarmPtAvggt03", root_folder = cms.untracked.string("Vertexing/PrimaryVertexVByHarmPtAvggt03"))



associatorByWHarmPtAvggt001 = VertexAssociatorByPositionAndTracks_cfi.VertexAssociatorByPositionAndTracks.clone(sharedTrackFraction=0.01, associationMode= cms.string("WHarmPtAvg"))
vertexAnalysisByWHarmPtAvggt001 = vertexAnalysis.clone(vertexAssociator="associatorByWHarmPtAvggt001", root_folder = cms.untracked.string("Vertexing/PrimaryVertexVByWHarmPtAvggt001"))

associatorByWHarmPtAvggt005 = VertexAssociatorByPositionAndTracks_cfi.VertexAssociatorByPositionAndTracks.clone(sharedTrackFraction=0.05, associationMode= cms.string("WHarmPtAvg"))
vertexAnalysisByWHarmPtAvggt005 = vertexAnalysis.clone(vertexAssociator="associatorByWHarmPtAvggt005", root_folder = cms.untracked.string("Vertexing/PrimaryVertexVByWHarmPtAvggt005"))

associatorByWHarmPtAvggt01 = VertexAssociatorByPositionAndTracks_cfi.VertexAssociatorByPositionAndTracks.clone(sharedTrackFraction=0.1, associationMode= cms.string("WHarmPtAvg"))
vertexAnalysisByWHarmPtAvggt01 = vertexAnalysis.clone(vertexAssociator="associatorByWHarmPtAvggt01", root_folder = cms.untracked.string("Vertexing/PrimaryVertexVByWHarmPtAvggt01"))

associatorByWHarmPtAvggt03 = VertexAssociatorByPositionAndTracks_cfi.VertexAssociatorByPositionAndTracks.clone(sharedTrackFraction=0.3, associationMode= cms.string("WHarmPtAvg"))
vertexAnalysisByWHarmPtAvggt03 = vertexAnalysis.clone(vertexAssociator="associatorByWHarmPtAvggt03", root_folder = cms.untracked.string("Vertexing/PrimaryVertexVByWHarmPtAvggt03"))



associatorByHarmWPtAvggt001 = VertexAssociatorByPositionAndTracks_cfi.VertexAssociatorByPositionAndTracks.clone(sharedTrackFraction=0.01, associationMode= cms.string("HarmWPtAvg"))
vertexAnalysisByHarmWPtAvggt001 = vertexAnalysis.clone(vertexAssociator="associatorByHarmWPtAvggt001", root_folder = cms.untracked.string("Vertexing/PrimaryVertexVByHarmWPtAvggt001"))

associatorByHarmWPtAvggt005 = VertexAssociatorByPositionAndTracks_cfi.VertexAssociatorByPositionAndTracks.clone(sharedTrackFraction=0.05, associationMode= cms.string("HarmWPtAvg"))
vertexAnalysisByHarmWPtAvggt005 = vertexAnalysis.clone(vertexAssociator="associatorByHarmWPtAvggt005", root_folder = cms.untracked.string("Vertexing/PrimaryVertexVByHarmWPtAvggt005"))

associatorByHarmWPtAvggt01 = VertexAssociatorByPositionAndTracks_cfi.VertexAssociatorByPositionAndTracks.clone(sharedTrackFraction=0.1, associationMode= cms.string("HarmWPtAvg"))
vertexAnalysisByHarmWPtAvggt01 = vertexAnalysis.clone(vertexAssociator="associatorByHarmWPtAvggt01", root_folder = cms.untracked.string("Vertexing/PrimaryVertexVByHarmWPtAvggt01"))

associatorByHarmWPtAvggt03 = VertexAssociatorByPositionAndTracks_cfi.VertexAssociatorByPositionAndTracks.clone(sharedTrackFraction=0.3, associationMode= cms.string("HarmWPtAvg"))
vertexAnalysisByHarmWPtAvggt03 = vertexAnalysis.clone(vertexAssociator="associatorByHarmWPtAvggt03", root_folder = cms.untracked.string("Vertexing/PrimaryVertexVByHarmWPtAvggt03"))


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
	* associatorByTrackgt001
  * vertexAnalysisByTrackgt001
	* associatorByTrackgt005
  * vertexAnalysisByTrackgt005
	* associatorByTrackgt01
  * vertexAnalysisByTrackgt01
	* associatorByTrackgt03
  * vertexAnalysisByTrackgt03

	* associatorByWTrackgt001
  * vertexAnalysisByWTrackgt001
	* associatorByWTrackgt005
  * vertexAnalysisByWTrackgt005
	* associatorByWTrackgt01
  * vertexAnalysisByWTrackgt01
	* associatorByWTrackgt03
  * vertexAnalysisByWTrackgt03

	* associatorByPtgt001
  * vertexAnalysisByPtgt001
	* associatorByPtgt005
  * vertexAnalysisByPtgt005
	* associatorByPtgt01
  * vertexAnalysisByPtgt01
	* associatorByPtgt03
  * vertexAnalysisByPtgt03

	* associatorByWPtgt001
  * vertexAnalysisByWPtgt001
	* associatorByWPtgt005
  * vertexAnalysisByWPtgt005
	* associatorByWPtgt01
  * vertexAnalysisByWPtgt01
	* associatorByWPtgt03
  * vertexAnalysisByWPtgt03

	* associatorByPt2gt001
  * vertexAnalysisByPt2gt001
	* associatorByPt2gt005
  * vertexAnalysisByPt2gt005
	* associatorByPt2gt01
  * vertexAnalysisByPt2gt01
	* associatorByPt2gt03
  * vertexAnalysisByPt2gt03

	* associatorByWPt2gt001
  * vertexAnalysisByWPt2gt001
	* associatorByWPt2gt005
  * vertexAnalysisByWPt2gt005
	* associatorByWPt2gt01
  * vertexAnalysisByWPt2gt01
	* associatorByWPt2gt03
  * vertexAnalysisByWPt2gt03

	* associatorByHarmPtgt001
  * vertexAnalysisByHarmPtgt001
	* associatorByHarmPtgt005
  * vertexAnalysisByHarmPtgt005
	* associatorByHarmPtgt01
  * vertexAnalysisByHarmPtgt01
	* associatorByHarmPtgt03
  * vertexAnalysisByHarmPtgt03

	* associatorByWHarmPtgt001
  * vertexAnalysisByWHarmPtgt001
	* associatorByWHarmPtgt005
  * vertexAnalysisByWHarmPtgt005
	* associatorByWHarmPtgt01
  * vertexAnalysisByWHarmPtgt01
	* associatorByWHarmPtgt03
  * vertexAnalysisByWHarmPtgt03

	* associatorByHarmWPtgt001
  * vertexAnalysisByHarmWPtgt001
	* associatorByHarmWPtgt005
  * vertexAnalysisByHarmWPtgt005
	* associatorByHarmWPtgt01
  * vertexAnalysisByHarmWPtgt01
	* associatorByHarmWPtgt03
  * vertexAnalysisByHarmWPtgt03

	* associatorByHarmPtAvggt001
  * vertexAnalysisByHarmPtAvggt001
	* associatorByHarmPtAvggt005
  * vertexAnalysisByHarmPtAvggt005
	* associatorByHarmPtAvggt01
  * vertexAnalysisByHarmPtAvggt01
	* associatorByHarmPtAvggt03
  * vertexAnalysisByHarmPtAvggt03

	* associatorByWHarmPtAvggt001
  * vertexAnalysisByWHarmPtAvggt001
	* associatorByWHarmPtAvggt005
  * vertexAnalysisByWHarmPtAvggt005
	* associatorByWHarmPtAvggt01
  * vertexAnalysisByWHarmPtAvggt01
	* associatorByWHarmPtAvggt03
  * vertexAnalysisByWHarmPtAvggt03

	* associatorByHarmWPtAvggt001
  * vertexAnalysisByHarmWPtAvggt001
	* associatorByHarmWPtAvggt005
  * vertexAnalysisByHarmWPtAvggt005
	* associatorByHarmWPtAvggt01
  * vertexAnalysisByHarmWPtAvggt01
	* associatorByHarmWPtAvggt03
  * vertexAnalysisByHarmWPtAvggt03
)

