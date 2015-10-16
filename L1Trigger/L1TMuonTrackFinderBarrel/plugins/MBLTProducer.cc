
// framework include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "FWCore/Framework/interface/MakerMacros.h"
#include <FWCore/Framework/interface/ConsumesCollector.h>

// user include files
#include "DataFormats/L1TMuon/interface/MuonTriggerPrimitive.h"
#include "DataFormats/L1TMuon/interface/MuonTriggerPrimitiveFwd.h"

#include "DataFormats/L1TMuon/interface/MBLTCollection.h"
#include "DataFormats/L1TMuon/interface/MBLTCollectionFwd.h"

#include "DataFormats/MuonDetId/interface/RPCDetId.h"
#include "DataFormats/MuonDetId/interface/DTChamberId.h"

class MBLTProducer : public edm::EDProducer {
public:
  MBLTProducer(const edm::ParameterSet&);
  ~MBLTProducer() {}

  void produce(edm::Event&, const edm::EventSetup&);
private:
  edm::InputTag _trigPrimSrc;
  double _maxDeltaPhi;
};


MBLTProducer::MBLTProducer( const edm::ParameterSet& iConfig )
  : _trigPrimSrc(iConfig.getParameter<edm::InputTag>("TriggerPrimitiveSrc")),
    _maxDeltaPhi(iConfig.getParameter<double>("MaxDeltaPhi"))
{
  produces<L1TMuon::MBLTContainer>().setBranchAlias("MBLTContainer");
  consumes<L1TMuon::TriggerPrimitiveCollection>(iConfig.getParameter<edm::InputTag>("TriggerPrimitiveSrc"));

}

void
MBLTProducer::produce( edm::Event& iEvent,
				       const edm::EventSetup& iSetup )
{

  std::auto_ptr<L1TMuon::MBLTContainer>
    out ( new L1TMuon::MBLTContainer );
  L1TMuon::MBLTContainer & tracksMap = *out;

  edm::Handle<L1TMuon::TriggerPrimitiveCollection> tps;
  iEvent.getByLabel(_trigPrimSrc, tps);

  auto tp = tps->cbegin();
  auto tpbeg = tps->cbegin();
  auto tpend = tps->cend();
  for( ; tp != tpend; ++tp ) {

    DTChamberId key;

    L1TMuon::TriggerPrimitive::subsystem_type type = tp->subsystem();
    switch ( type ) {

    case L1TMuon::TriggerPrimitive::kDT :
      key = tp->detId<DTChamberId>();
      break;

    case L1TMuon::TriggerPrimitive::kRPC : {
      if ( tp->detId<RPCDetId>().region() ) continue; // endcap
      int station = tp->detId<RPCDetId>().station();
      int sector  = tp->detId<RPCDetId>().sector();
      int wheel = tp->detId<RPCDetId>().ring();
      key = DTChamberId( wheel, station, sector );
      break;
    }

    default : continue;
    }

    if ( out->find( key ) == out->end() ) {
      tracksMap[key] = L1TMuon::MBLTCollection( key );
    }

    L1TMuon::TriggerPrimitiveRef tpref(tps, tp - tpbeg);
    tracksMap[key].addStub( tpref );
  }


  L1TMuon::MBLTContainer::iterator st = out->begin();
  L1TMuon::MBLTContainer::iterator stend = out->end();
  for ( ; st != stend; ++st ) st->second.associate( _maxDeltaPhi );

  iEvent.put(out);

}

#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE(MBLTProducer);
