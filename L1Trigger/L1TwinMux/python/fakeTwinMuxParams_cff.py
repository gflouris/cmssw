import FWCore.ParameterSet.Config as cms

import os


bmbtfParamsSource = cms.ESSource(
    "EmptyESSource",
    recordName = cms.string('L1TwinMuxParamsRcd'),
    iovIsRunNotTime = cms.bool(True),
    firstValid = cms.vuint32(1)
)

fakeBmtfParams = cms.ESProducer('L1TwinMuxParamsESProducer',
    fwVersion = cms.uint32(1),
    useRpcBxForDtBelowQuality = cms.uint32(5),


)
