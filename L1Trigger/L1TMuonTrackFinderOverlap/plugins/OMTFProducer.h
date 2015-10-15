#ifndef OMTFProducer_H
#define OMTFProducer_H

#include "xercesc/util/XercesDefs.hpp"

#include "DataFormats/L1TMuon/interface/MuonTriggerPrimitive.h"
#include "DataFormats/L1TMuon/interface/MuonTriggerPrimitiveFwd.h"

#include "DataFormats/L1TMuon/interface/RegionalMuonCand.h"
#include "DataFormats/L1TMuon/interface/RegionalMuonCandFwd.h"

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Framework/interface/EDProducer.h"

#include "TRandom3.h"

class OMTFProcessor;
class OMTFConfiguration;
class OMTFConfigMaker;
class OMTFinputMaker;
class OMTFSorter;
class OMTFinput;

class XMLConfigWriter;

namespace XERCES_CPP_NAMESPACE{
  class DOMElement;
  class DOMDocument;
  class DOMImplementation;
}


class OMTFProducer : public edm::EDProducer {
 public:
  OMTFProducer(const edm::ParameterSet&);

  ~OMTFProducer();

  virtual void beginJob();

  virtual void endJob();

  virtual void produce(edm::Event&, const edm::EventSetup&);

 private:

  edm::ParameterSet theConfig;
  edm::InputTag trigPrimSrc;
  edm::EDGetTokenT<L1TMuon::TriggerPrimitiveCollection> inputToken;

  void processCandidates(unsigned int iProcessor, int bx,
			 std::auto_ptr<l1t::RegionalMuonCandBxCollection > & myCands,
			 l1t::RegionalMuonCandBxCollection & myOTFCandidates,
			 l1t::tftype mtfType);

  const L1TMuon::TriggerPrimitiveCollection filterDigis(const L1TMuon::TriggerPrimitiveCollection & vDigi);

  void writeMergedGPs();

  bool dumpResultToXML, dumpDetailedResultToXML, dumpGPToXML;

  ///OMTF objects
  OMTFConfiguration *myOMTFConfig;
  OMTFinputMaker *myInputMaker;
  OMTFSorter *mySorter;
  OMTFProcessor *myOMTF;
  OMTFinput *myInputXML;
  ///
  xercesc::DOMElement *aTopElement;
  OMTFConfigMaker *myOMTFConfigMaker;
  XMLConfigWriter *myWriter;
  ///

};

#endif
