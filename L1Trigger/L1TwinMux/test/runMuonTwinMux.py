# -*- coding: utf-8 -*-
import FWCore.ParameterSet.Config as cms
process = cms.Process("L1TMuonEmulation")
import optparse

process.load("FWCore.MessageLogger.MessageLogger_cfi")

process.MessageLogger.cerr.FwkReport.reportEvery = cms.untracked.int32(50)
process.options = cms.untracked.PSet(wantSummary = cms.untracked.bool(False))

process.source = cms.Source('PoolSource',
 fileNames = cms.untracked.vstring('file:/afs/cern.ch/work/g/gflouris/public/SingleMuPt6180_noanti_10k_eta1.root')
#fileNames = cms.untracked.vstring('file:/afs/cern.ch/work/g/gflouris/public/forGeorge/JPsi/0A018076-8131-E511-8849-0025905C42F2.root')
#fileNames = cms.untracked.vstring('/store/data/Run2015D/DoubleMuon/RAW/v1/000/260/627/00000/FEEE3829-8882-E511-9DAD-02163E0121B9.root')
#fileNames = cms.untracked.vstring('file:///afs/cern.ch/work/g/gflouris/TriggerUpgrade/ParticleGun/CMSSW_7_2_0/src/SingleMuPt140_wheelp2.root')
	                    )

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(100))

# PostLS1 geometry used
process.load('Configuration.Geometry.GeometryExtended2015Reco_cff')
process.load('Configuration.Geometry.GeometryExtended2015_cff')
############################
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_condDBv2_cff')
from Configuration.AlCa.GlobalTag_condDBv2 import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, 'auto:run2_mc', '')


##ES Producer

####Event Setup Producer
process.load('L1Trigger.L1TwinMux.fakeTwinMuxParams_cff')
process.esProd = cms.EDAnalyzer("EventSetupRecordDataGetter",
   toGet = cms.VPSet(
      cms.PSet(record = cms.string('L1TwinMuxParamsRcd'),
               data = cms.vstring('L1TwinMuxParams'))
                   ),
   verbose = cms.untracked.bool(True)
)



###TwinMux Emulator
process.load('L1Trigger.L1TwinMux.simTwinMuxDigis_cfi')
process.simTwinMuxDigisEmu.DTDigi_Source = cms.InputTag("dttfDigis")
process.simTwinMuxDigisEmu.DTThetaDigi_Source = cms.InputTag("dttfDigis")
#process.simTwinMuxDigisEmu.RPC_Source = cms.InputTag("muonRPCDigis")


###TwinMux Simulator
process.load('L1Trigger.L1TMuonBarrel.simTwinMuxDigis_cfi')
process.simTwinMuxDigis.DTDigi_Source = cms.InputTag("dttfDigis")
process.simTwinMuxDigis.DTThetaDigi_Source = cms.InputTag("dttfDigis")


#process.load('Configuration.StandardSequences.RawToDigi_cff')


process.L1TMuonSeq = cms.Sequence(  
				  process.esProd
          #+ process.RawToDigi
				  +process.simTwinMuxDigisEmu
				  +process.simTwinMuxDigis 
)

process.L1TMuonPath = cms.Path(process.L1TMuonSeq)

process.out = cms.OutputModule("PoolOutputModule", 
   fileName = cms.untracked.string("l1twinmux_test.root")
)

process.output_step = cms.EndPath(process.out)
process.schedule = cms.Schedule(process.L1TMuonPath)
process.schedule.extend([process.output_step])
