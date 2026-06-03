import FWCore.ParameterSet.Config as cms

from step3_RAW2DIGI_RECO_RECOSIM_PAT_VALIDATION_DQM import process
#If you have an EDAnalyzer, you can change it to that too 
process.Phase2GeometryProducer = cms.EDProducer("Phase2GeometryProducer") #Change MyProducer to the name of the producer 

process.path = cms.Path(process.Phase2GeometryProducer)

process.schedule = cms.Schedule(process.path)