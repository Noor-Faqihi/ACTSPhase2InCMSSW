import FWCore.ParameterSet.Config as cms

from step3_RAW2DIGI_RECO_RECOSIM_PAT_VALIDATION_DQM import process
#If you have an EDAnalyzer, you can change it to that too 
process.ACTSTrackingGeometryProducer_Phase2 = cms.EDProducer("ACTSTrackingGeometryProducer_Phase2") #Change MyProducer to the name of the producer 

process.path = cms.Path(process.ACTSTrackingGeometryProducer_Phase2)

process.schedule = cms.Schedule(process.path)