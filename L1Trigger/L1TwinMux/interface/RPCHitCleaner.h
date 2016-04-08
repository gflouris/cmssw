//-------------------------------------------------
//
//   Class: RPCHitCleaner
//
//   RPCHitCleaner
//
//
//   Author :
//   G. Flouris               U Ioannina    Feb. 2015
//--------------------------------------------------

#ifndef RPC_HITCLEANER_H
#define RPC_HITCLEANER_H

#include "DataFormats/RPCDigi/interface/RPCDigiCollection.h"

#include "CondFormats/L1TObjects/interface/L1TwinMuxParams.h"
#include "CondFormats/DataRecord/interface/L1TwinMuxParamsRcd.h"


#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/EventSetup.h"

#include <iostream>

class RPCHitCleaner  {
public:
  RPCHitCleaner(RPCDigiCollection inrpcDigis);
  ~RPCHitCleaner() {};

  void run(const edm::EventSetup& c);

 ///Return Output RPCCollection
 RPCDigiCollection getRPCCollection(){  return _outrpcDigis;}

private:

  ///Input
  RPCDigiCollection _inrpcDigis;
  ///Output
  RPCDigiCollection _outrpcDigis;

  ///Event Setup Handler
  //edm::ESHandle< L1TwinMuxParams > tmParamsHandle;


  // struct rpc_hit
  // {
  //   int station;
  //   int sector;
  //   int wheel;
  //   RPCDetId detid;
  //   int strip;
  // };

};
#endif
