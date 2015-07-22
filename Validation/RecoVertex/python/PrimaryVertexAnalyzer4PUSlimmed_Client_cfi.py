import FWCore.ParameterSet.Config as cms

postProcessorVertex = cms.EDAnalyzer("DQMGenericClient",
                                     subDirs = cms.untracked.vstring("Vertexing/PrimaryVertexV/*"),
                                     efficiency = cms.vstring(
                                         "effic_vs_NumVertices 'Efficiency vs NumVertices' GenAllAssoc2RecoMatched_NumVertices GenAllAssoc2Reco_NumVertices",
                                         "effic_vs_Z 'Efficiency vs Z' GenAllAssoc2RecoMatched_Z GenAllAssoc2Reco_Z",
                                         "effic_vs_R 'Efficiency vs R' GenAllAssoc2RecoMatched_R GenAllAssoc2Reco_R",
                                         "effic_vs_Pt2 'Efficiency vs Sum p_{T}^{2}' GenAllAssoc2RecoMatched_Pt2 GenAllAssoc2Reco_Pt2",
                                         "effic_vs_NumTracks 'Efficiency vs NumTracks' GenAllAssoc2RecoMatched_NumTracks GenAllAssoc2Reco_NumTracks",
                                         "effic_vs_ClosestVertexInZ 'Efficiency vs ClosestVtx in Z' GenAllAssoc2RecoMatched_ClosestDistanceZ GenAllAssoc2Reco_ClosestDistanceZ",
                                         "gen_duplicate_vs_ClosestVertexInZ 'Gen_Duplicate vs ClosestVtx in Z' GenAllAssoc2RecoMultiMatched_ClosestDistanceZ GenAllAssoc2Reco_ClosestDistanceZ",
                                         "gen_duplicate_vs_NumVertices 'Gen_Duplicate vs NumVertices' GenAllAssoc2RecoMultiMatched_NumVertices GenAllAssoc2Reco_NumVertices",
                                         "gen_duplicate_vs_Z 'Gen_Duplicate vs Z' GenAllAssoc2RecoMultiMatched_Z GenAllAssoc2Reco_Z",
                                         "gen_duplicate_vs_R 'Gen_Duplicate vs R' GenAllAssoc2RecoMultiMatched_R GenAllAssoc2Reco_R",
                                         "gen_duplicate_vs_Pt2 'Gen_Duplicate vs Sum p_{T}^{2}' GenAllAssoc2RecoMultiMatched_Pt2 GenAllAssoc2Reco_Pt2",
                                         "gen_duplicate_vs_NumTracks 'Gen_Duplicate vs NumTracks' GenAllAssoc2RecoMultiMatched_NumTracks GenAllAssoc2Reco_NumTracks",
                                         "gen_duplicate_vs_ClosestVertexInZ 'Gen_Duplicate vs ClosestVtx in Z' GenAllAssoc2RecoMultiMatched_ClosestDistanceZ GenAllAssoc2Reco_ClosestDistanceZ",
                                         "merged_vs_NumVertices 'Merged vs NumVertices' RecoAllAssoc2GenMultiMatched_NumVertices RecoAllAssoc2Gen_NumVertices",
                                         "merged_vs_PU 'Merged vs PU' RecoAllAssoc2GenMultiMatched_PU RecoAllAssoc2Gen_PU",
                                         "merged_vs_Z 'Merged vs Z' RecoAllAssoc2GenMultiMatched_Z RecoAllAssoc2Gen_Z",
                                         "merged_vs_R 'Merged vs R' RecoAllAssoc2GenMultiMatched_R RecoAllAssoc2Gen_R",
                                         "merged_vs_Pt2 'Merged vs Sum p_{T}^{2}' RecoAllAssoc2GenMultiMatched_Pt2 RecoAllAssoc2Gen_Pt2",
                                         "merged_vs_NumTracks 'Merged vs NumTracks' RecoAllAssoc2GenMultiMatched_NumTracks RecoAllAssoc2Gen_NumTracks",
                                         "merged_vs_ClosestVertexInZ 'Merged vs ClosestVtx in Z' RecoAllAssoc2GenMultiMatched_ClosestDistanceZ RecoAllAssoc2GenSimForMerge_ClosestDistanceZ",
                                         "fakerate_vs_NumVertices 'Fakerate vs NumVertices' RecoAllAssoc2GenMatched_NumVertices RecoAllAssoc2Gen_NumVertices fake",
                                         "fakerate_vs_PU 'Fakerate vs PU' RecoAllAssoc2GenMatched_PU RecoAllAssoc2Gen_PU fake",
                                         "fakerate_vs_Z 'Fakerate vs Z' RecoAllAssoc2GenMatched_Z RecoAllAssoc2Gen_Z fake",
                                         "fakerate_vs_R 'Fakerate vs R' RecoAllAssoc2GenMatched_R RecoAllAssoc2Gen_R fake",
                                         "fakerate_vs_Pt2 'Fakerate vs Sum p_{T}^{2}' RecoAllAssoc2GenMatched_Pt2 RecoAllAssoc2Gen_Pt2 fake",
                                         "fakerate_vs_Ndof 'Fakerate vs Ndof' RecoAllAssoc2GenMatched_Ndof RecoAllAssoc2Gen_Ndof fake",
                                         "fakerate_vs_NumTracks 'Fakerate vs NumTracks' RecoAllAssoc2GenMatched_NumTracks RecoAllAssoc2Gen_NumTracks fake",
                                         "fakerate_vs_ClosestVertexInZ 'Fakerate vs ClosestVtx in Z' RecoAllAssoc2GenMatched_ClosestDistanceZ RecoAllAssoc2Gen_ClosestDistanceZ fake",
                                         "fakerate_vs_Purity 'Fakerate vs Purity' RecoAllAssoc2GenMatched_Purity RecoAllAssoc2Gen_Purity fake",
                                         "duplicate_vs_NumVertices 'Duplicate vs NumVertices' RecoAllAssoc2MultiMatchedGen_NumVertices RecoAllAssoc2Gen_NumVertices",
                                         "duplicate_vs_PU 'Duplicate vs PU' RecoAllAssoc2MultiMatchedGen_PU RecoAllAssoc2Gen_PU",
                                         "duplicate_vs_Z 'Duplicate vs Z' RecoAllAssoc2MultiMatchedGen_Z RecoAllAssoc2Gen_Z",
                                         "duplicate_vs_R 'Duplicate vs R' RecoAllAssoc2MultiMatchedGen_R RecoAllAssoc2Gen_R",
                                         "duplicate_vs_Pt2 'Duplicate vs Sum p_{T}^{2}' RecoAllAssoc2MultiMatchedGen_Pt2 RecoAllAssoc2Gen_Pt2",
                                         "duplicate_vs_NumTracks 'Duplicate vs NumTracks' RecoAllAssoc2MultiMatchedGen_NumTracks RecoAllAssoc2Gen_NumTracks",
                                         "duplicate_vs_ClosestVertexInZ 'Duplicate vs ClosestsVtx In Z' RecoAllAssoc2MultiMatchedGen_ClosestDistanceZ RecoAllAssoc2Gen_ClosestDistanceZ"
                                     ),
                                     resolution = cms.vstring(
                                         "RecoAllAssoc2GenMatched_ResolX_vs_PU '#sigma(x) vs PU' RecoAllAssoc2GenMatched_ResolX_vs_PU",
                                         "RecoAllAssoc2GenMatched_ResolY_vs_PU '#sigma(y) vs PU' RecoAllAssoc2GenMatched_ResolY_vs_PU",
                                         "RecoAllAssoc2GenMatched_ResolZ_vs_PU '#sigma(z) vs PU' RecoAllAssoc2GenMatched_ResolZ_vs_PU",
                                         "RecoAllAssoc2GenMatched_ResolPt2_vs_PU '#sigma(p_{T}^{2}) vs PU' RecoAllAssoc2GenMatched_ResolPt2_vs_PU",
                                         "RecoAllAssoc2GenMatched_ResolX_vs_NumTracks '#sigma(x) vs NumTracks' RecoAllAssoc2GenMatched_ResolX_vs_NumTracks",
                                         "RecoAllAssoc2GenMatched_ResolY_vs_NumTracks '#sigma(y) vs NumTracks' RecoAllAssoc2GenMatched_ResolY_vs_NumTracks",
                                         "RecoAllAssoc2GenMatched_ResolZ_vs_NumTracks '#sigma(z) vs NumTracks' RecoAllAssoc2GenMatched_ResolZ_vs_NumTracks",
                                         "RecoAllAssoc2GenMatched_ResolPt2_vs_NumTracks '#sigma(p_{T}^{2}) vs NumTracks' RecoAllAssoc2GenMatched_ResolPt2_vs_NumTracks",
                                         "RecoAllAssoc2GenMatchedMerged_ResolX_vs_PU '#sigma(x) vs PU' RecoAllAssoc2GenMatchedMerged_ResolX_vs_PU",
                                         "RecoAllAssoc2GenMatchedMerged_ResolY_vs_PU '#sigma(y) vs PU' RecoAllAssoc2GenMatchedMerged_ResolY_vs_PU",
                                         "RecoAllAssoc2GenMatchedMerged_ResolZ_vs_PU '#sigma(z) vs PU' RecoAllAssoc2GenMatchedMerged_ResolZ_vs_PU",
                                         "RecoAllAssoc2GenMatchedMerged_ResolPt2_vs_PU '#sigma(p_{T}^{2}) vs PU' RecoAllAssoc2GenMatchedMerged_ResolPt2_vs_PU",
                                         "RecoAllAssoc2GenMatchedMerged_ResolX_vs_NumTracks '#sigma(x) vs NumTracks' RecoAllAssoc2GenMatchedMerged_ResolX_vs_NumTracks",
                                         "RecoAllAssoc2GenMatchedMerged_ResolY_vs_NumTracks '#sigma(y) vs NumTracks' RecoAllAssoc2GenMatchedMerged_ResolY_vs_NumTracks",
                                         "RecoAllAssoc2GenMatchedMerged_ResolZ_vs_NumTracks '#sigma(z) vs NumTracks' RecoAllAssoc2GenMatchedMerged_ResolZ_vs_NumTracks",
                                         "RecoAllAssoc2GenMatchedMerged_ResolPt2_vs_NumTracks '#sigma(p_{T}^{2}) vs NumTracks' RecoAllAssoc2GenMatchedMerged_ResolPt2_vs_NumTracks",
                                     ),
                                     outputFileName = cms.untracked.string(""),
                                     verbose = cms.untracked.uint32(5)
)

postProcessorVertexByTrackgt001 = postProcessorVertex.clone(subDirs = cms.untracked.vstring("Vertexing/PrimaryVertexVByTrackgt001/*"))
postProcessorVertexByTrackgt005 = postProcessorVertex.clone(subDirs = cms.untracked.vstring("Vertexing/PrimaryVertexVByTrackgt005/*"))
postProcessorVertexByTrackgt01 = postProcessorVertex.clone(subDirs = cms.untracked.vstring("Vertexing/PrimaryVertexVByTrackgt01/*"))
postProcessorVertexByTrackgt03 = postProcessorVertex.clone(subDirs = cms.untracked.vstring("Vertexing/PrimaryVertexVByTrackgt03/*"))


postProcessorVertexByWTrackgt001 = postProcessorVertex.clone(subDirs = cms.untracked.vstring("Vertexing/PrimaryVertexVByWTrackgt001/*"))
postProcessorVertexByWTrackgt005 = postProcessorVertex.clone(subDirs = cms.untracked.vstring("Vertexing/PrimaryVertexVByWTrackgt005/*"))
postProcessorVertexByWTrackgt01 = postProcessorVertex.clone(subDirs = cms.untracked.vstring("Vertexing/PrimaryVertexVByWTrackgt01/*"))
postProcessorVertexByWTrackgt03 = postProcessorVertex.clone(subDirs = cms.untracked.vstring("Vertexing/PrimaryVertexVByWTrackgt03/*"))


postProcessorVertexByPtgt001 = postProcessorVertex.clone(subDirs = cms.untracked.vstring("Vertexing/PrimaryVertexVByPtgt001/*"))
postProcessorVertexByPtgt005 = postProcessorVertex.clone(subDirs = cms.untracked.vstring("Vertexing/PrimaryVertexVByPtgt005/*"))
postProcessorVertexByPtgt01 = postProcessorVertex.clone(subDirs = cms.untracked.vstring("Vertexing/PrimaryVertexVByPtgt01/*"))
postProcessorVertexByPtgt03 = postProcessorVertex.clone(subDirs = cms.untracked.vstring("Vertexing/PrimaryVertexVByPtgt03/*"))


postProcessorVertexByWPtgt001 = postProcessorVertex.clone(subDirs = cms.untracked.vstring("Vertexing/PrimaryVertexVByWPtgt001/*"))
postProcessorVertexByWPtgt005 = postProcessorVertex.clone(subDirs = cms.untracked.vstring("Vertexing/PrimaryVertexVByWPtgt005/*"))
postProcessorVertexByWPtgt01 = postProcessorVertex.clone(subDirs = cms.untracked.vstring("Vertexing/PrimaryVertexVByWPtgt01/*"))
postProcessorVertexByWPtgt03 = postProcessorVertex.clone(subDirs = cms.untracked.vstring("Vertexing/PrimaryVertexVByWPtgt03/*"))


postProcessorVertexByPt2gt001 = postProcessorVertex.clone(subDirs = cms.untracked.vstring("Vertexing/PrimaryVertexVByPt2gt001/*"))
postProcessorVertexByPt2gt005 = postProcessorVertex.clone(subDirs = cms.untracked.vstring("Vertexing/PrimaryVertexVByPt2gt005/*"))
postProcessorVertexByPt2gt01 = postProcessorVertex.clone(subDirs = cms.untracked.vstring("Vertexing/PrimaryVertexVByPt2gt01/*"))
postProcessorVertexByPt2gt03 = postProcessorVertex.clone(subDirs = cms.untracked.vstring("Vertexing/PrimaryVertexVByPt2gt03/*"))


postProcessorVertexByWPt2gt001 = postProcessorVertex.clone(subDirs = cms.untracked.vstring("Vertexing/PrimaryVertexVByWPt2gt001/*"))
postProcessorVertexByWPt2gt005 = postProcessorVertex.clone(subDirs = cms.untracked.vstring("Vertexing/PrimaryVertexVByWPt2gt005/*"))
postProcessorVertexByWPt2gt01 = postProcessorVertex.clone(subDirs = cms.untracked.vstring("Vertexing/PrimaryVertexVByWPt2gt01/*"))
postProcessorVertexByWPt2gt03 = postProcessorVertex.clone(subDirs = cms.untracked.vstring("Vertexing/PrimaryVertexVByWPt2gt03/*"))


postProcessorVertexByHarmPtgt001 = postProcessorVertex.clone(subDirs = cms.untracked.vstring("Vertexing/PrimaryVertexVByHarmPtgt001/*"))
postProcessorVertexByHarmPtgt005 = postProcessorVertex.clone(subDirs = cms.untracked.vstring("Vertexing/PrimaryVertexVByHarmPtgt005/*"))
postProcessorVertexByHarmPtgt01 = postProcessorVertex.clone(subDirs = cms.untracked.vstring("Vertexing/PrimaryVertexVByHarmPtgt01/*"))
postProcessorVertexByHarmPtgt03 = postProcessorVertex.clone(subDirs = cms.untracked.vstring("Vertexing/PrimaryVertexVByHarmPtgt03/*"))


postProcessorVertexByWHarmPtgt001 = postProcessorVertex.clone(subDirs = cms.untracked.vstring("Vertexing/PrimaryVertexVByWHarmPtgt001/*"))
postProcessorVertexByWHarmPtgt005 = postProcessorVertex.clone(subDirs = cms.untracked.vstring("Vertexing/PrimaryVertexVByWHarmPtgt005/*"))
postProcessorVertexByWHarmPtgt01 = postProcessorVertex.clone(subDirs = cms.untracked.vstring("Vertexing/PrimaryVertexVByWHarmPtgt01/*"))
postProcessorVertexByWHarmPtgt03 = postProcessorVertex.clone(subDirs = cms.untracked.vstring("Vertexing/PrimaryVertexVByWHarmPtgt03/*"))


postProcessorVertexByHarmWPtgt001 = postProcessorVertex.clone(subDirs = cms.untracked.vstring("Vertexing/PrimaryVertexVByHarmWPtgt001/*"))
postProcessorVertexByHarmWPtgt005 = postProcessorVertex.clone(subDirs = cms.untracked.vstring("Vertexing/PrimaryVertexVByHarmWPtgt005/*"))
postProcessorVertexByHarmWPtgt01 = postProcessorVertex.clone(subDirs = cms.untracked.vstring("Vertexing/PrimaryVertexVByHarmWPtgt01/*"))
postProcessorVertexByHarmWPtgt03 = postProcessorVertex.clone(subDirs = cms.untracked.vstring("Vertexing/PrimaryVertexVByHarmWPtgt03/*"))


postProcessorVertexByHarmPtAvggt001 = postProcessorVertex.clone(subDirs = cms.untracked.vstring("Vertexing/PrimaryVertexVByHarmPtAvggt001/*"))
postProcessorVertexByHarmPtAvggt005 = postProcessorVertex.clone(subDirs = cms.untracked.vstring("Vertexing/PrimaryVertexVByHarmPtAvggt005/*"))
postProcessorVertexByHarmPtAvggt01 = postProcessorVertex.clone(subDirs = cms.untracked.vstring("Vertexing/PrimaryVertexVByHarmPtAvggt01/*"))
postProcessorVertexByHarmPtAvggt03 = postProcessorVertex.clone(subDirs = cms.untracked.vstring("Vertexing/PrimaryVertexVByHarmPtAvggt03/*"))


postProcessorVertexByWHarmPtAvggt001 = postProcessorVertex.clone(subDirs = cms.untracked.vstring("Vertexing/PrimaryVertexVByWHarmPtAvggt001/*"))
postProcessorVertexByWHarmPtAvggt005 = postProcessorVertex.clone(subDirs = cms.untracked.vstring("Vertexing/PrimaryVertexVByWHarmPtAvggt005/*"))
postProcessorVertexByWHarmPtAvggt01 = postProcessorVertex.clone(subDirs = cms.untracked.vstring("Vertexing/PrimaryVertexVByWHarmPtAvggt01/*"))
postProcessorVertexByWHarmPtAvggt03 = postProcessorVertex.clone(subDirs = cms.untracked.vstring("Vertexing/PrimaryVertexVByWHarmPtAvggt03/*"))


postProcessorVertexByHarmWPtAvggt001 = postProcessorVertex.clone(subDirs = cms.untracked.vstring("Vertexing/PrimaryVertexVByHarmWPtAvggt001/*"))
postProcessorVertexByHarmWPtAvggt005 = postProcessorVertex.clone(subDirs = cms.untracked.vstring("Vertexing/PrimaryVertexVByHarmWPtAvggt005/*"))
postProcessorVertexByHarmWPtAvggt01 = postProcessorVertex.clone(subDirs = cms.untracked.vstring("Vertexing/PrimaryVertexVByHarmWPtAvggt01/*"))
postProcessorVertexByHarmWPtAvggt03 = postProcessorVertex.clone(subDirs = cms.untracked.vstring("Vertexing/PrimaryVertexVByHarmWPtAvggt03/*"))


postProcessorVertexStandAlone = cms.Sequence(postProcessorVertex
* postProcessorVertexByTrackgt001
* postProcessorVertexByTrackgt005
* postProcessorVertexByTrackgt01
* postProcessorVertexByTrackgt03

* postProcessorVertexByWTrackgt001
* postProcessorVertexByWTrackgt005
* postProcessorVertexByWTrackgt01
* postProcessorVertexByWTrackgt03

* postProcessorVertexByPtgt001
* postProcessorVertexByPtgt005
* postProcessorVertexByPtgt01
* postProcessorVertexByPtgt03

* postProcessorVertexByWPtgt001
* postProcessorVertexByWPtgt005
* postProcessorVertexByWPtgt01
* postProcessorVertexByWPtgt03

* postProcessorVertexByPt2gt001
* postProcessorVertexByPt2gt005
* postProcessorVertexByPt2gt01
* postProcessorVertexByPt2gt03

* postProcessorVertexByWPt2gt001
* postProcessorVertexByWPt2gt005
* postProcessorVertexByWPt2gt01
* postProcessorVertexByWPt2gt03

* postProcessorVertexByHarmPtgt001
* postProcessorVertexByHarmPtgt005
* postProcessorVertexByHarmPtgt01
* postProcessorVertexByHarmPtgt03

* postProcessorVertexByWHarmPtgt001
* postProcessorVertexByWHarmPtgt005
* postProcessorVertexByWHarmPtgt01
* postProcessorVertexByWHarmPtgt03

* postProcessorVertexByHarmWPtgt001
* postProcessorVertexByHarmWPtgt005
* postProcessorVertexByHarmWPtgt01
* postProcessorVertexByHarmWPtgt03

* postProcessorVertexByHarmPtAvggt001
* postProcessorVertexByHarmPtAvggt005
* postProcessorVertexByHarmPtAvggt01
* postProcessorVertexByHarmPtAvggt03

* postProcessorVertexByWHarmPtAvggt001
* postProcessorVertexByWHarmPtAvggt005
* postProcessorVertexByWHarmPtAvggt01
* postProcessorVertexByWHarmPtAvggt03

* postProcessorVertexByHarmWPtAvggt001
* postProcessorVertexByHarmWPtAvggt005
* postProcessorVertexByHarmWPtAvggt01
* postProcessorVertexByHarmWPtAvggt03
)
