//-------------------------------------------------
//
//   Class: L1TwinMuxAlgortithm
//
//   L1TwinMuxAlgortithm
//
//
//   Author :
//   G. Flouris               U Ioannina    Feb. 2015
//--------------------------------------------------

#include <iostream>
#include <iomanip>
#include <iterator>

#include "L1Trigger/L1TwinMux/interface/L1TwinMuxAlgortithm.h"
#include "Geometry/RPCGeometry/interface/RPCRoll.h"
#include "Geometry/RPCGeometry/interface/RPCGeometry.h"
#include "CondFormats/DataRecord/interface/RPCEMapRcd.h"
#include "Geometry/Records/interface/MuonGeometryRecord.h"
#include "L1Trigger/L1TwinMux/interface/AlignTrackSegments.h"
#include "L1Trigger/L1TwinMux/interface/RPCtoDTTranslator.h"
#include "L1Trigger/L1TwinMux/interface/DTRPCBxCorrection.h"

using namespace std;


void L1TwinMuxAlgortithm::run(
                                                            edm::Handle<L1MuDTChambPhContainer> inphiDigis,
                                                            edm::Handle<L1MuDTChambThContainer> thetaDigis,
                                                            edm::Handle<RPCDigiCollection> rpcDigis,
                                                            const edm::EventSetup& c) {


  ///ES Parameters
  //const L1TwinMuxParamsRcd& tmParamsRcd = c.get<L1TwinMuxParamsRcd>();
  //tmParamsRcd.get(tmParamsHandle);
  //const L1TwinMuxParams& tmParams = *tmParamsHandle.product();
 
  ///Align track segments that are coming in bx-1.
  AlignTrackSegments *alignedDTs = new AlignTrackSegments(*inphiDigis);
  alignedDTs->run(c);
  L1MuDTChambPhContainer phiDigis = alignedDTs->getDTContainer();
  //_tm_phi_output = phiDigis; //only DTs

  ///Translate RPC digis to DT primitives.
  RPCtoDTTranslator *dt_from_rpc = new RPCtoDTTranslator(*rpcDigis);
  dt_from_rpc->run(c);
  L1MuDTChambPhContainer rpcPhiDigis = dt_from_rpc->getDTContainer();
  //_tm_phi_output = rpcPhiDigis; //only RPCs

  ///Correct(in bx) DT primitives by comparing them to RPC.
  DTRPCBxCorrection *rpc_dt_bx = new DTRPCBxCorrection(phiDigis,rpcPhiDigis);
  rpc_dt_bx->run(c);
  L1MuDTChambPhContainer dtrpcbxPhiDigis = rpc_dt_bx->getDTContainer();
  //_tm_phi_output = dtrpcbxPhiDigis; //only DT corrected with RPCs

  //Add RPC primitives in case that there are no DT primitives. 
  std::vector<L1MuDTChambPhDigi> l1ttma_out;
  L1MuDTChambPhDigi const* dtts1=0;
  L1MuDTChambPhDigi const* dtts2=0;

  L1MuDTChambPhDigi const* rpcts1=0;

  int bx=0, wheel=0, sector=0, station=1;


  for(bx=-1; bx<=1; bx++){
    for (wheel=-3;wheel<=3; wheel++ ){
      for (sector=0;sector<12; sector++ ){
        for (station=1; station<=4; station++){

          dtts1=0; dtts2=0; rpcts1=0;

          dtts1 = dtrpcbxPhiDigis.chPhiSegm1(wheel,station,sector,bx);
          dtts2 = dtrpcbxPhiDigis.chPhiSegm2(wheel,station,sector,bx - 1 );

          rpcts1 = rpcPhiDigis.chPhiSegm1(wheel,station,sector,bx);

          if(!dtts1 && !dtts2 && !rpcts1 ) continue;

          if(dtts1) { 
            L1MuDTChambPhDigi dt_ts1 = *dtts1;
            l1ttma_out.push_back(dt_ts1);
          }
          if(dtts2) { 
            L1MuDTChambPhDigi dt_ts2 = *dtts2;
            l1ttma_out.push_back(dt_ts2);
          }

          if(!dtts1 && !dtts2 && rpcts1 && station<=2 ) { 
            L1MuDTChambPhDigi dt_rpc( rpcts1->bxNum() , rpcts1->whNum(), rpcts1->scNum(), rpcts1->stNum(),rpcts1->phi(), rpcts1->phiB(), rpcts1->code(), rpcts1->Ts2Tag(), rpcts1->BxCnt(),1); 
            l1ttma_out.push_back(dt_rpc);
          }
          //else if(!dtts2 && rpcts1 && station<=2 ) { 
          //  L1MuDTChambPhDigi dt_rpc( rpcts1->bxNum() , rpcts1->whNum(), rpcts1->scNum(), rpcts1->stNum(),rpcts1->phi(), rpcts1->phiB(), rpcts1->code(), 1, rpcts1->BxCnt(),1); 
          //  l1ttma_out.push_back(dt_rpc);
          //}

        }
      }
    }
  }

_tm_phi_output.setContainer(l1ttma_out);

}

