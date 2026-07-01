import FWCore.ParameterSet.Config as cms

from step3_RAW2DIGI_RECO_RECOSIM_PAT_VALIDATION_DQM import process #This performs step3 only, not 1,2 and 3 
#If you have an EDAnalyzer, you can change it to that too 
process.ACTSTrackingGeometryProducer_Phase2 = cms.ESProducer("ACTSTrackingGeometryProducer_Phase2") #Change MyProducer to the name of the producer 
             
#process.ACTSTrackingGeometryProducer_Phase2 = cms.EDAnalyzer("ACTSTrackingGeometryProducer_Phase2") change into the name of the analyzer 
#process.path = cms.Path(process.ACTSTrackingGeometryProducer_Phase2) add analyzer name 

process.ACTSTrackingGeometryProducer_Phase2 = cms.ESProducer(
    "ACTSTrackingGeometryProducer_Phase2",
    saveObjfile = cms.untracked.bool(False), 
    saveSvgfile = cms.untracked.bool(False),
    mapMaterial = cms.untracked.bool(False),
    outputObjFile = cms.untracked.string("outputObjFile"),
    outputSvgFile = cms.untracked.string("outputSvgFile"),
    MaterialMaps = cms.untracked.string("MaterialMaps"),
    rangeZ = cms.untracked.vdouble(0,1),
    rangeR = cms.untracked.vdouble(0,1)
)

#process.schedule = cms.Schedule(process.path)