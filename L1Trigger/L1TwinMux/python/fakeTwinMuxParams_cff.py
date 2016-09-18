import FWCore.ParameterSet.Config as cms

import os


twinmuxParamsSource = cms.ESSource(
    "EmptyESSource",
    recordName = cms.string('L1TwinMuxParamsRcd'),
    iovIsRunNotTime = cms.bool(True),
    firstValid = cms.vuint32(1)
)

fakeTwinMuxParams = cms.ESProducer('L1TwinMuxParamsESProducer',
    fwVersion = cms.uint32(1),
    useRpcBxForDtBelowQuality = cms.uint32(4),
    dphiWindowBxShift         = cms.uint32(9999),
    useOnlyRPC                = cms.bool(False),
    useOnlyDT                 = cms.bool(False),
    CorrectDTBxwRPC           = cms.bool(True),
    verbose                   = cms.int32(False),

)
