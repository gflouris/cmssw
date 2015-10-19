# -*- coding: utf-8 -*-
import FWCore.ParameterSet.Config as cms
process = cms.Process("L1TMuonEmulation")
import os
import sys
import commands

process.load("FWCore.MessageLogger.MessageLogger_cfi")

process.MessageLogger.cerr.FwkReport.reportEvery = cms.untracked.int32(50)
process.options = cms.untracked.PSet(wantSummary = cms.untracked.bool(True))

process.source = cms.Source(
    'PoolSource',
fileNames = cms.untracked.vstring('file:///afs/cern.ch/work/g/gflouris/public/SingleMuPt6180_noanti_50k_eta08.root')
#fileNames = cms.untracked.vstring('/store/caf/user/battilan/L1Trigger/BarrelTF/Generation/v1/SingleMuFlatPt_3GeVto140GeV_GEN_SIM_DIGI_L1_46_1_BrH.root')
    )

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(-1))





####Super Primitives
process.load('L1Trigger.L1TMuonTrackFinderEndCap.L1TMuonTriggerPrimitiveProducer_cfi')
process.load('L1Trigger.L1TMuonTrackFinderEndCap.L1DTTFTrackConverter_cfi')
process.load('L1Trigger.L1TMuonTrackFinderEndCap.L1RPCTFTrackConverter_cfi')
process.load('L1Trigger.L1TMuonTrackFinderBarrel.MBLTProducer_cfi')
process.load('L1Trigger.L1TMuonTrackFinderBarrel.L1ITMuonBarrelPrimitiveProducer_cfi')


#####

process.mMBLTProducer = process.MBLTProducer.clone(
    TriggerPrimitiveSrc = cms.InputTag('L1TMuonTriggerPrimitives'),
)
# PostLS1 geometry used
process.load('Configuration.Geometry.GeometryExtended2015Reco_cff')
process.load('Configuration.Geometry.GeometryExtended2015_cff')
############################
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_condDBv2_cff')
from Configuration.AlCa.GlobalTag_condDBv2 import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, 'auto:run2_mc', '')


process.load('L1Trigger.L1TMuonTrackFinderBarrel.bmtfDigis_cfi')

####BMTF Emulator
process.bmtfEmulator = cms.EDProducer("BMTrackFinder",
    DTDigi_Source = cms.InputTag("L1ITMuonBarrelPrimitiveProducer"),
    DTDigi_Theta_Source = cms.InputTag("simDtTriggerPrimitiveDigis"),

    Debug = cms.untracked.int32(0)

)

process.L1TMuonSeq = cms.Sequence( process.L1TMuonTriggerPrimitives +  # L1TMu primitives container
                                   process.L1DTTFTrackConverter    +
                                   process.L1RPCTFTrackConverters  + 
                                   process.MBLTProducer            +  # chamber level primitive matching
                                   process.L1ITMuonBarrelPrimitiveProducer + # new combined primitive prod. 
                                   process.bmtfEmulator 
)



process.L1TMuonPath = cms.Path(process.L1TMuonSeq)


process.out = cms.OutputModule("PoolOutputModule", 
   fileName = cms.untracked.string("l1tbmtf_superprimitives.root"),
                               )

process.output_step = cms.EndPath(process.out)





process.schedule = cms.Schedule(process.L1TMuonPath)
process.schedule.extend([process.output_step])
