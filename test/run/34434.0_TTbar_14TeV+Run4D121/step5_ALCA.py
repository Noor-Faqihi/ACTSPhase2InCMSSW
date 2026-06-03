# Auto generated configuration file
# using: 
# Revision: 1.19 
# Source: /local/reps/CMSSW/CMSSW/Configuration/Applications/python/ConfigBuilder.py,v 
# with command line options: step5 -s ALCA:SiPixelCalSingleMuonLoose+SiPixelCalSingleMuonTight+TkAlMuonIsolated+TkAlMinBias+MuAlOverlaps+EcalESAlign+TkAlZMuMu+TkAlDiMuonAndVertex+HcalCalHBHEMuonProducerFilter+TkAlUpsilonMuMu+TkAlJpsiMuMu --conditions auto:phase2_realistic_T35 --datatier ALCARECO -n 10 --eventcontent ALCARECO --geometry ExtendedRun4D121 --era Phase2C22I13M9 --filein file:step3.root --fileout file:step5.root
import FWCore.ParameterSet.Config as cms

from Configuration.Eras.Era_Phase2C22I13M9_cff import Phase2C22I13M9

process = cms.Process('ALCA',Phase2C22I13M9)

# import of standard configurations
process.load('Configuration.StandardSequences.Services_cff')
process.load('SimGeneral.HepPDTESSource.pythiapdt_cfi')
process.load('FWCore.MessageService.MessageLogger_cfi')
process.load('Configuration.EventContent.EventContent_cff')
process.load('SimGeneral.MixingModule.mixNoPU_cfi')
process.load('Configuration.Geometry.GeometryExtendedRun4D121Reco_cff')
process.load('Configuration.StandardSequences.MagneticField_cff')
process.load('Configuration.StandardSequences.AlCaRecoStreamsMC_cff')
process.load('Configuration.StandardSequences.EndOfProcess_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(10),
    output = cms.optional.untracked.allowed(cms.int32,cms.PSet)
)

# Input source
process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring('file:step3.root'),
    secondaryFileNames = cms.untracked.vstring()
)

process.options = cms.untracked.PSet(
    IgnoreCompletely = cms.untracked.vstring(),
    Rethrow = cms.untracked.vstring(),
    TryToContinue = cms.untracked.vstring(),
    accelerators = cms.untracked.vstring('*'),
    allowUnscheduled = cms.obsolete.untracked.bool,
    canDeleteEarly = cms.untracked.vstring(),
    deleteNonConsumedUnscheduledModules = cms.untracked.bool(True),
    dumpOptions = cms.untracked.bool(False),
    emptyRunLumiMode = cms.obsolete.untracked.string,
    eventSetup = cms.untracked.PSet(
        forceNumberOfConcurrentIOVs = cms.untracked.PSet(
            allowAnyLabel_=cms.required.untracked.uint32
        ),
        numberOfConcurrentIOVs = cms.untracked.uint32(0)
    ),
    fileMode = cms.untracked.string('FULLMERGE'),
    forceEventSetupCacheClearOnNewRun = cms.untracked.bool(False),
    holdsReferencesToDeleteEarly = cms.untracked.VPSet(),
    makeTriggerResults = cms.obsolete.untracked.bool,
    modulesToCallForTryToContinue = cms.untracked.vstring(),
    modulesToIgnoreForDeleteEarly = cms.untracked.vstring(),
    numberOfConcurrentLuminosityBlocks = cms.untracked.uint32(0),
    numberOfConcurrentRuns = cms.untracked.uint32(1),
    numberOfStreams = cms.untracked.uint32(0),
    numberOfThreads = cms.untracked.uint32(1),
    printDependencies = cms.untracked.bool(False),
    sizeOfStackForThreadsInKB = cms.optional.untracked.uint32,
    throwIfIllegalParameter = cms.untracked.bool(True),
    wantSummary = cms.untracked.bool(False)
)

# Production Info
process.configurationMetadata = cms.untracked.PSet(
    annotation = cms.untracked.string('step5 nevts:10'),
    name = cms.untracked.string('Applications'),
    version = cms.untracked.string('$Revision: 1.19 $')
)

# Output definition


# Additional output definition
process.ALCARECOStreamEcalESAlign = cms.OutputModule("PoolOutputModule",
    SelectEvents = cms.untracked.PSet(
        SelectEvents = cms.vstring('pathALCARECOEcalESAlign')
    ),
    dataset = cms.untracked.PSet(
        dataTier = cms.untracked.string('ALCARECO'),
        filterName = cms.untracked.string('EcalESAlign')
    ),
    eventAutoFlushCompressedSize = cms.untracked.int32(5242880),
    fileName = cms.untracked.string('EcalESAlign.root'),
    outputCommands = cms.untracked.vstring(
        'drop *',
        'keep ESDigiCollection_ecalPreshowerDigis_*_*',
        'keep SiPixelClusteredmNewDetSetVector_ecalAlCaESAlignTrackReducer_*_*',
        'keep SiStripClusteredmNewDetSetVector_ecalAlCaESAlignTrackReducer_*_*',
        'keep TrackingRecHitsOwned_ecalAlCaESAlignTrackReducer_*_*',
        'keep recoTrackExtras_ecalAlCaESAlignTrackReducer_*_*',
        'keep recoTracks_ecalAlCaESAlignTrackReducer_*_*',
        'keep recoBeamSpot_offlineBeamSpot_*_*'
    )
)
process.ALCARECOStreamHcalCalHBHEMuonProducerFilter = cms.OutputModule("PoolOutputModule",
    SelectEvents = cms.untracked.PSet(
        SelectEvents = cms.vstring('pathALCARECOHcalCalHBHEMuonProducerFilter')
    ),
    dataset = cms.untracked.PSet(
        dataTier = cms.untracked.string('ALCARECO'),
        filterName = cms.untracked.string('HcalCalHBHEMuonProducerFilter')
    ),
    eventAutoFlushCompressedSize = cms.untracked.int32(5242880),
    fileName = cms.untracked.string('HcalCalHBHEMuonProducerFilter.root'),
    outputCommands = cms.untracked.vstring(
        'drop *',
        'keep *_alcaHcalHBHEMuonProducer_hbheMuon_*'
    )
)
process.ALCARECOStreamMuAlOverlaps = cms.OutputModule("PoolOutputModule",
    SelectEvents = cms.untracked.PSet(
        SelectEvents = cms.vstring('pathALCARECOMuAlOverlaps')
    ),
    dataset = cms.untracked.PSet(
        dataTier = cms.untracked.string('ALCARECO'),
        filterName = cms.untracked.string('MuAlOverlaps')
    ),
    eventAutoFlushCompressedSize = cms.untracked.int32(5242880),
    fileName = cms.untracked.string('MuAlOverlaps.root'),
    outputCommands = cms.untracked.vstring(
        'drop *',
        'keep *_ALCARECOMuAlOverlaps_*_*',
        'keep *_ALCARECOMuAlOverlapsGeneralTracks_*_*',
        'keep *_muonCSCDigis_*_*',
        'keep *_muonDTDigis_*_*',
        'keep *_muonRPCDigis_*_*',
        'keep *_dt1DRecHits_*_*',
        'keep *_dt2DSegments_*_*',
        'keep *_dt4DSegments_*_*',
        'keep *_csc2DRecHits_*_*',
        'keep *_cscSegments_*_*',
        'keep *_rpcRecHits_*_*',
        'keep L1AcceptBunchCrossings_*_*_*',
        'keep L1GlobalTriggerReadoutRecord_gtDigis_*_*',
        'keep *_TriggerResults_*_*',
        'keep *_offlineBeamSpot_*_*',
        'keep *_offlinePrimaryVertices_*_*',
        'keep DcsStatuss_scalersRawToDigi_*_*'
    )
)
process.ALCARECOStreamSiPixelCalSingleMuonLoose = cms.OutputModule("PoolOutputModule",
    SelectEvents = cms.untracked.PSet(
        SelectEvents = cms.vstring('pathALCARECOSiPixelCalSingleMuonLoose')
    ),
    dataset = cms.untracked.PSet(
        dataTier = cms.untracked.string('ALCARECO'),
        filterName = cms.untracked.string('SiPixelCalSingleMuonLoose')
    ),
    eventAutoFlushCompressedSize = cms.untracked.int32(5242880),
    fileName = cms.untracked.string('SiPixelCalSingleMuonLoose.root'),
    outputCommands = cms.untracked.vstring(
        'drop *',
        'keep *_ALCARECOSiPixelCalSingleMuonLoose_*_*',
        'keep *_muons__*',
        'keep *_offlinePrimaryVertices_*_*',
        'keep *_*riggerResults_*_HLT'
    )
)
process.ALCARECOStreamSiPixelCalSingleMuonTight = cms.OutputModule("PoolOutputModule",
    SelectEvents = cms.untracked.PSet(
        SelectEvents = cms.vstring('pathALCARECOSiPixelCalSingleMuonTight')
    ),
    dataset = cms.untracked.PSet(
        dataTier = cms.untracked.string('ALCARECO'),
        filterName = cms.untracked.string('SiPixelCalSingleMuonTight')
    ),
    eventAutoFlushCompressedSize = cms.untracked.int32(5242880),
    fileName = cms.untracked.string('SiPixelCalSingleMuonTight.root'),
    outputCommands = cms.untracked.vstring(
        'drop *',
        'keep *_ALCARECOSiPixelCalSingleMuonTight_*_*',
        'keep *_muons__*',
        'keep *_offlinePrimaryVertices_*_*',
        'keep *_*riggerResults_*_HLT',
        'keep *_*closebyPixelClusters*_*_*',
        'keep *_*trackDistances*_*_*',
        'keep PileupSummaryInfos_addPileupInfo_*_*'
    )
)
process.ALCARECOStreamTkAlDiMuonAndVertex = cms.OutputModule("PoolOutputModule",
    SelectEvents = cms.untracked.PSet(
        SelectEvents = cms.vstring('pathALCARECOTkAlDiMuonAndVertex')
    ),
    dataset = cms.untracked.PSet(
        dataTier = cms.untracked.string('ALCARECO'),
        filterName = cms.untracked.string('TkAlDiMuonAndVertex')
    ),
    eventAutoFlushCompressedSize = cms.untracked.int32(5242880),
    fileName = cms.untracked.string('TkAlDiMuonAndVertex.root'),
    outputCommands = cms.untracked.vstring(
        'drop *',
        'keep recoTracks_ALCARECOTkAlDiMuon_*_*',
        'keep recoTrackExtras_ALCARECOTkAlDiMuon_*_*',
        'keep TrackingRecHitsOwned_ALCARECOTkAlDiMuon_*_*',
        'keep SiPixelClusteredmNewDetSetVector_ALCARECOTkAlDiMuon_*_*',
        'keep recoTracks_ALCARECOTkAlDiMuonVertexTracks_*_*',
        'keep recoTrackExtras_ALCARECOTkAlDiMuonVertexTracks_*_*',
        'keep TrackingRecHitsOwned_ALCARECOTkAlDiMuonVertexTracks_*_*',
        'keep SiPixelClusteredmNewDetSetVector_ALCARECOTkAlDiMuonVertexTracks_*_*',
        'keep L1AcceptBunchCrossings_*_*_*',
        'keep L1GlobalTriggerReadoutRecord_gtDigis_*_*',
        'keep *_TriggerResults_*_*',
        'keep DcsStatuss_scalersRawToDigi_*_*',
        'keep *_offlinePrimaryVertices_*_*',
        'keep *_offlineBeamSpot_*_*',
        'keep LHERunInfoProduct_*_*_*',
        'keep LHEEventProduct_*_*_*',
        'keep GenRunInfoProduct_generator_*_*',
        'keep GenLumiInfoHeader_generator_*_*',
        'keep GenLumiInfoProduct_generator_*_*',
        'keep GenEventInfoProduct_generator_*_*',
        'keep GenFilterInfo_*_*_*',
        'keep *_TkAlDiMuonAndVertexGenMuonSelector_*_*',
        'keep PileupSummaryInfos_*_*_*',
        'keep int_*_bunchSpacing_*',
        'keep *_genPUProtons_*_*',
        'keep Phase2TrackerCluster1DedmNewDetSetVector_ALCARECOTkAlDiMuon_*_*',
        'keep Phase2TrackerCluster1DedmNewDetSetVector_ALCARECOTkAlDiMuonVertexTracks_*_*'
    )
)
process.ALCARECOStreamTkAlJpsiMuMu = cms.OutputModule("PoolOutputModule",
    SelectEvents = cms.untracked.PSet(
        SelectEvents = cms.vstring('pathALCARECOTkAlJpsiMuMu')
    ),
    dataset = cms.untracked.PSet(
        dataTier = cms.untracked.string('ALCARECO'),
        filterName = cms.untracked.string('TkAlJpsiMuMu')
    ),
    eventAutoFlushCompressedSize = cms.untracked.int32(5242880),
    fileName = cms.untracked.string('TkAlJpsiMuMu.root'),
    outputCommands = cms.untracked.vstring(
        'drop *',
        'keep recoTracks_ALCARECOTkAlJpsiMuMu_*_*',
        'keep recoTrackExtras_ALCARECOTkAlJpsiMuMu_*_*',
        'keep TrackingRecHitsOwned_ALCARECOTkAlJpsiMuMu_*_*',
        'keep SiPixelClusteredmNewDetSetVector_ALCARECOTkAlJpsiMuMu_*_*',
        'keep L1AcceptBunchCrossings_*_*_*',
        'keep L1GlobalTriggerReadoutRecord_gtDigis_*_*',
        'keep *_TriggerResults_*_*',
        'keep *_offlinePrimaryVertices_*_*',
        'keep LHERunInfoProduct_*_*_*',
        'keep LHEEventProduct_*_*_*',
        'keep GenRunInfoProduct_generator_*_*',
        'keep GenLumiInfoHeader_generator_*_*',
        'keep GenLumiInfoProduct_generator_*_*',
        'keep GenEventInfoProduct_generator_*_*',
        'keep GenFilterInfo_*_*_*',
        'keep *_TkAlJpsiMuMuGenMuonSelector_*_*',
        'keep PileupSummaryInfos_*_*_*',
        'keep int_*_bunchSpacing_*',
        'keep *_genPUProtons_*_*',
        'keep DCSRecord_onlineMetaDataDigis_*_*',
        'keep Phase2TrackerCluster1DedmNewDetSetVector_ALCARECOTkAlJpsiMuMu_*_*'
    )
)
process.ALCARECOStreamTkAlMinBias = cms.OutputModule("PoolOutputModule",
    SelectEvents = cms.untracked.PSet(
        SelectEvents = cms.vstring('pathALCARECOTkAlMinBias')
    ),
    dataset = cms.untracked.PSet(
        dataTier = cms.untracked.string('ALCARECO'),
        filterName = cms.untracked.string('TkAlMinBias')
    ),
    eventAutoFlushCompressedSize = cms.untracked.int32(5242880),
    fileName = cms.untracked.string('TkAlMinBias.root'),
    outputCommands = cms.untracked.vstring(
        'drop *',
        'keep recoTracks_ALCARECOTkAlMinBias_*_*',
        'keep recoTrackExtras_ALCARECOTkAlMinBias_*_*',
        'keep TrackingRecHitsOwned_ALCARECOTkAlMinBias_*_*',
        'keep SiPixelClusteredmNewDetSetVector_ALCARECOTkAlMinBias_*_*',
        'keep L1AcceptBunchCrossings_*_*_*',
        'keep L1GlobalTriggerReadoutRecord_gtDigis_*_*',
        'keep *_TriggerResults_*_*',
        'keep *_offlinePrimaryVertices_*_*',
        'keep *_offlineBeamSpot_*_*',
        'keep DCSRecord_onlineMetaDataDigis_*_*',
        'keep OnlineLuminosityRecord_onlineMetaDataDigis_*_*',
        'keep Phase2TrackerCluster1DedmNewDetSetVector_ALCARECOTkAlMinBias_*_*'
    )
)
process.ALCARECOStreamTkAlMuonIsolated = cms.OutputModule("PoolOutputModule",
    SelectEvents = cms.untracked.PSet(
        SelectEvents = cms.vstring('pathALCARECOTkAlMuonIsolated')
    ),
    dataset = cms.untracked.PSet(
        dataTier = cms.untracked.string('ALCARECO'),
        filterName = cms.untracked.string('TkAlMuonIsolated')
    ),
    eventAutoFlushCompressedSize = cms.untracked.int32(5242880),
    fileName = cms.untracked.string('TkAlMuonIsolated.root'),
    outputCommands = cms.untracked.vstring(
        'drop *',
        'keep recoTracks_ALCARECOTkAlMuonIsolated_*_*',
        'keep recoTrackExtras_ALCARECOTkAlMuonIsolated_*_*',
        'keep TrackingRecHitsOwned_ALCARECOTkAlMuonIsolated_*_*',
        'keep SiPixelClusteredmNewDetSetVector_ALCARECOTkAlMuonIsolated_*_*',
        'keep L1AcceptBunchCrossings_*_*_*',
        'keep L1GlobalTriggerReadoutRecord_gtDigis_*_*',
        'keep *_TriggerResults_*_*',
        'keep *_offlinePrimaryVertices_*_*',
        'keep DCSRecord_onlineMetaDataDigis_*_*',
        'keep Phase2TrackerCluster1DedmNewDetSetVector_ALCARECOTkAlMuonIsolated_*_*'
    )
)
process.ALCARECOStreamTkAlUpsilonMuMu = cms.OutputModule("PoolOutputModule",
    SelectEvents = cms.untracked.PSet(
        SelectEvents = cms.vstring('pathALCARECOTkAlUpsilonMuMu')
    ),
    dataset = cms.untracked.PSet(
        dataTier = cms.untracked.string('ALCARECO'),
        filterName = cms.untracked.string('TkAlUpsilonMuMu')
    ),
    eventAutoFlushCompressedSize = cms.untracked.int32(5242880),
    fileName = cms.untracked.string('TkAlUpsilonMuMu.root'),
    outputCommands = cms.untracked.vstring(
        'drop *',
        'keep recoTracks_ALCARECOTkAlUpsilonMuMu_*_*',
        'keep recoTrackExtras_ALCARECOTkAlUpsilonMuMu_*_*',
        'keep TrackingRecHitsOwned_ALCARECOTkAlUpsilonMuMu_*_*',
        'keep SiPixelClusteredmNewDetSetVector_ALCARECOTkAlUpsilonMuMu_*_*',
        'keep L1AcceptBunchCrossings_*_*_*',
        'keep L1GlobalTriggerReadoutRecord_gtDigis_*_*',
        'keep *_TriggerResults_*_*',
        'keep *_offlinePrimaryVertices_*_*',
        'keep LHERunInfoProduct_*_*_*',
        'keep LHEEventProduct_*_*_*',
        'keep GenRunInfoProduct_generator_*_*',
        'keep GenLumiInfoHeader_generator_*_*',
        'keep GenLumiInfoProduct_generator_*_*',
        'keep GenEventInfoProduct_generator_*_*',
        'keep GenFilterInfo_*_*_*',
        'keep *_TkAlUpsilonMuMuGenMuonSelector_*_*',
        'keep PileupSummaryInfos_*_*_*',
        'keep int_*_bunchSpacing_*',
        'keep *_genPUProtons_*_*',
        'keep DCSRecord_onlineMetaDataDigis_*_*',
        'keep Phase2TrackerCluster1DedmNewDetSetVector_ALCARECOTkAlUpsilonMuMu_*_*'
    )
)
process.ALCARECOStreamTkAlZMuMu = cms.OutputModule("PoolOutputModule",
    SelectEvents = cms.untracked.PSet(
        SelectEvents = cms.vstring('pathALCARECOTkAlZMuMu')
    ),
    dataset = cms.untracked.PSet(
        dataTier = cms.untracked.string('ALCARECO'),
        filterName = cms.untracked.string('TkAlZMuMu')
    ),
    eventAutoFlushCompressedSize = cms.untracked.int32(5242880),
    fileName = cms.untracked.string('TkAlZMuMu.root'),
    outputCommands = cms.untracked.vstring(
        'drop *',
        'keep recoTracks_ALCARECOTkAlZMuMu_*_*',
        'keep recoTrackExtras_ALCARECOTkAlZMuMu_*_*',
        'keep TrackingRecHitsOwned_ALCARECOTkAlZMuMu_*_*',
        'keep SiPixelClusteredmNewDetSetVector_ALCARECOTkAlZMuMu_*_*',
        'keep L1AcceptBunchCrossings_*_*_*',
        'keep L1GlobalTriggerReadoutRecord_gtDigis_*_*',
        'keep *_TriggerResults_*_*',
        'keep *_offlinePrimaryVertices_*_*',
        'keep *_offlineBeamSpot_*_*',
        'keep LHERunInfoProduct_*_*_*',
        'keep LHEEventProduct_*_*_*',
        'keep GenRunInfoProduct_generator_*_*',
        'keep GenLumiInfoHeader_generator_*_*',
        'keep GenLumiInfoProduct_generator_*_*',
        'keep GenEventInfoProduct_generator_*_*',
        'keep GenFilterInfo_*_*_*',
        'keep *_TkAlZMuMuGenMuonSelector_*_*',
        'keep PileupSummaryInfos_*_*_*',
        'keep int_*_bunchSpacing_*',
        'keep *_genPUProtons_*_*',
        'keep DCSRecord_onlineMetaDataDigis_*_*',
        'keep Phase2TrackerCluster1DedmNewDetSetVector_ALCARECOTkAlZMuMu_*_*'
    )
)

# Other statements
process.ALCARECOEventContent.outputCommands.extend(process.OutALCARECOTkAlMinBias_noDrop.outputCommands)
process.ALCARECOEventContent.outputCommands.extend(process.OutALCARECOTkAlMuonIsolated_noDrop.outputCommands)
process.ALCARECOEventContent.outputCommands.extend(process.OutALCARECOTkAlZMuMu_noDrop.outputCommands)
process.ALCARECOEventContent.outputCommands.extend(process.OutALCARECOTkAlDiMuonAndVertex_noDrop.outputCommands)
process.ALCARECOEventContent.outputCommands.extend(process.OutALCARECOTkAlJpsiMuMu_noDrop.outputCommands)
process.ALCARECOEventContent.outputCommands.extend(process.OutALCARECOTkAlUpsilonMuMu_noDrop.outputCommands)
process.ALCARECOEventContent.outputCommands.extend(process.OutALCARECOSiPixelCalSingleMuonLoose_noDrop.outputCommands)
process.ALCARECOEventContent.outputCommands.extend(process.OutALCARECOSiPixelCalSingleMuonTight_noDrop.outputCommands)
process.ALCARECOEventContent.outputCommands.extend(process.OutALCARECOEcalESAlign_noDrop.outputCommands)
process.ALCARECOEventContent.outputCommands.extend(process.OutALCARECOHcalCalHBHEMuonProducerFilter_noDrop.outputCommands)
process.ALCARECOEventContent.outputCommands.extend(process.OutALCARECOMuAlOverlaps_noDrop.outputCommands)
from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, 'auto:phase2_realistic_T35', '')

# Path and EndPath definitions
process.endjob_step = cms.EndPath(process.endOfProcess)
process.ALCARECOStreamEcalESAlignOutPath = cms.EndPath(process.ALCARECOStreamEcalESAlign)
process.ALCARECOStreamHcalCalHBHEMuonProducerFilterOutPath = cms.EndPath(process.ALCARECOStreamHcalCalHBHEMuonProducerFilter)
process.ALCARECOStreamMuAlOverlapsOutPath = cms.EndPath(process.ALCARECOStreamMuAlOverlaps)
process.ALCARECOStreamSiPixelCalSingleMuonLooseOutPath = cms.EndPath(process.ALCARECOStreamSiPixelCalSingleMuonLoose)
process.ALCARECOStreamSiPixelCalSingleMuonTightOutPath = cms.EndPath(process.ALCARECOStreamSiPixelCalSingleMuonTight)
process.ALCARECOStreamTkAlDiMuonAndVertexOutPath = cms.EndPath(process.ALCARECOStreamTkAlDiMuonAndVertex)
process.ALCARECOStreamTkAlJpsiMuMuOutPath = cms.EndPath(process.ALCARECOStreamTkAlJpsiMuMu)
process.ALCARECOStreamTkAlMinBiasOutPath = cms.EndPath(process.ALCARECOStreamTkAlMinBias)
process.ALCARECOStreamTkAlMuonIsolatedOutPath = cms.EndPath(process.ALCARECOStreamTkAlMuonIsolated)
process.ALCARECOStreamTkAlUpsilonMuMuOutPath = cms.EndPath(process.ALCARECOStreamTkAlUpsilonMuMu)
process.ALCARECOStreamTkAlZMuMuOutPath = cms.EndPath(process.ALCARECOStreamTkAlZMuMu)

# Schedule definition
process.schedule = cms.Schedule(process.pathALCARECOTkAlMinBias,process.pathALCARECOTkAlMuonIsolated,process.pathALCARECOTkAlZMuMu,process.pathALCARECOTkAlDiMuonAndVertex,process.pathALCARECOTkAlJpsiMuMu,process.pathALCARECOTkAlUpsilonMuMu,process.pathALCARECOSiPixelCalSingleMuonLoose,process.pathALCARECOSiPixelCalSingleMuonTight,process.pathALCARECOEcalESAlign,process.pathALCARECOHcalCalHBHEMuonProducerFilter,process.pathALCARECOMuAlOverlaps,process.pathALCARECOMuAlOverlapsGeneralTracks,process.endjob_step,process.ALCARECOStreamEcalESAlignOutPath,process.ALCARECOStreamHcalCalHBHEMuonProducerFilterOutPath,process.ALCARECOStreamMuAlOverlapsOutPath,process.ALCARECOStreamSiPixelCalSingleMuonLooseOutPath,process.ALCARECOStreamSiPixelCalSingleMuonTightOutPath,process.ALCARECOStreamTkAlDiMuonAndVertexOutPath,process.ALCARECOStreamTkAlJpsiMuMuOutPath,process.ALCARECOStreamTkAlMinBiasOutPath,process.ALCARECOStreamTkAlMuonIsolatedOutPath,process.ALCARECOStreamTkAlUpsilonMuMuOutPath,process.ALCARECOStreamTkAlZMuMuOutPath)
from PhysicsTools.PatAlgos.tools.helpers import associatePatAlgosToolsTask
associatePatAlgosToolsTask(process)



# Customisation from command line

# Add early deletion of temporary data products to reduce peak memory need
from Configuration.StandardSequences.earlyDeleteSettings_cff import customiseEarlyDelete
process = customiseEarlyDelete(process)
# End adding early deletion
