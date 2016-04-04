//-------------------------------------------------
//
//   Class: L1TwinMuxProducer
//
//   L1TwinMuxProducer EDProducer
//
//
//   Author :
//   G. Flouris               U Ioannina    Feb. 2015
//--------------------------------------------------



#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include <FWCore/Framework/interface/ConsumesCollector.h>
#include <FWCore/Framework/interface/one/EDProducer.h>
#include <FWCore/ParameterSet/interface/ParameterSet.h>

#include "L1Trigger/L1TwinMux/interface/L1TwinMuxAlgortithm.h"

#include <iostream>
#include <iomanip>

using namespace std;

class L1TwinMuxProducer: public edm::one::EDProducer<edm::one::SharedResources> {
public:
  L1TwinMuxProducer(const edm::ParameterSet & pset);
  ~L1TwinMuxProducer() {}
  void produce(edm::Event & e, const edm::EventSetup& c);
private:
  L1TwinMuxAlgortithm * m_l1tma;
  edm::EDGetToken m_dtdigi, m_dtthetadigi, m_rpcsource;

};




L1TwinMuxProducer::L1TwinMuxProducer(const edm::ParameterSet & pset) {
m_l1tma = new L1TwinMuxAlgortithm();

m_dtdigi      = consumes<L1MuDTChambPhContainer>(pset.getParameter<edm::InputTag>("DTDigi_Source"));
m_dtthetadigi = consumes<L1MuDTChambThContainer>(pset.getParameter<edm::InputTag>("DTThetaDigi_Source"));
m_rpcsource   = consumes<RPCDigiCollection>(pset.getParameter<edm::InputTag>("RPC_Source"));

produces<L1MuDTChambPhContainer>("TwinMuxEmulator");

}

void L1TwinMuxProducer::produce(edm::Event& e, const edm::EventSetup& c) {

  edm::Handle<L1MuDTChambPhContainer> phiDigis;
  edm::Handle<L1MuDTChambThContainer> thetaDigis;
  e.getByToken(m_dtdigi, phiDigis);
  e.getByToken(m_dtthetadigi, thetaDigis);

  edm::Handle<RPCDigiCollection> rpcDigis;
  e.getByToken(m_rpcsource, rpcDigis);

  if (! phiDigis.isValid()){
    //cout << "ERROR:  TwinMux input DT phi digis not valid.\n";
  }
  if (! thetaDigis.isValid()){
    //cout << "ERROR:  TwinMux input DT theta digis not valid.\n";
  }

  std::auto_ptr<L1MuDTChambPhContainer> l1ttmp(new L1MuDTChambPhContainer);
  m_l1tma->run(phiDigis, thetaDigis, rpcDigis,c);
  *l1ttmp = m_l1tma->get_ph_tm_output();

  e.put(l1ttmp,"TwinMuxEmulator");
}


#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE(L1TwinMuxProducer);
