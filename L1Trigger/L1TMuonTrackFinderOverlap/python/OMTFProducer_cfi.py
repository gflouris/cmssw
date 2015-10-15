import FWCore.ParameterSet.Config as cms

###OMTF emulator configuration
omtfEmulator = cms.EDProducer("OMTFProducer",
                              TriggerPrimitiveSrc = cms.InputTag('L1TMuonTriggerPrimitives'),
                              dumpResultToXML = cms.bool(False),
                              dumpDetailedResultToXML = cms.bool(False),
                              XMLDumpFileName = cms.string("TestEvents.xml"),                                     
                              dumpGPToXML = cms.bool(False),  
                              readEventsFromXML = cms.bool(False),
                              eventsXMLFiles = cms.vstring("TestEvents.xml"),
                              dropRPCPrimitives = cms.bool(False),                                    
                              dropDTPrimitives = cms.bool(False),                                    
                              dropCSCPrimitives = cms.bool(False),   
                              omtf = cms.PSet(
                                  patternsXMLFiles = cms.VPSet(
                                       cms.PSet(patternsXMLFile = cms.FileInPath("L1Trigger/L1TMuonTrackFinderOverlap/data/Patterns_ipt6_31_750_4x.xml")),
                                      ),
                                  configXMLFile = cms.FileInPath("L1Trigger/L1TMuonTrackFinderOverlap/data/hwToLogicLayer_750.xml"),
                              )
)


