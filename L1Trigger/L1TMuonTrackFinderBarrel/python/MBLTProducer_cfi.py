import FWCore.ParameterSet.Config as cms

MBLTProducer = cms.EDProducer(
    'MBLTProducer',
    TriggerPrimitiveSrc = cms.InputTag('L1TMuonTriggerPrimitives'),
    MaxDeltaPhi = cms.double( 0.05 )
    )
