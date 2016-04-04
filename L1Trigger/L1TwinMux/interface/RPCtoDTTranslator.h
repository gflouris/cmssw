//-------------------------------------------------
//
//   Class: RPCtoDTTranslator
//
//   RPCtoDTTranslator
//
//
//   Author :
//   G. Flouris               U Ioannina    Feb. 2015
//--------------------------------------------------

#ifndef RPC_DTTranslator_H
#define RPC_DTTranslator_H

#include "DataFormats/L1DTTrackFinder/interface/L1MuDTChambPhContainer.h"
#include "DataFormats/L1DTTrackFinder/interface/L1MuDTChambThContainer.h"
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

class RPCtoDTTranslator  {
public:
  RPCtoDTTranslator(RPCDigiCollection inrpcDigis);
  ~RPCtoDTTranslator() {};

  void run(const edm::EventSetup& c);

 ///Return Output PhContainer
 L1MuDTChambPhContainer getDTContainer(){  return _rpcdt_translated;}
 static int radialAngle(RPCDetId , const edm::EventSetup& , int);

private:

  ///Output PhContainer
  L1MuDTChambPhContainer _rpcdt_translated;
  RPCDigiCollection rpcDigis;
  ///Event Setup Handler
  //edm::ESHandle< L1TwinMuxParams > tmParamsHandle;
  int bendingAngle(float, float, float, float );
  int localX(RPCDetId , const edm::EventSetup&, int );


  struct rpc_hit
  {
    int station;
    int sector;
    int wheel;
    RPCDetId detid;
    int strip;
  };

};
#endif
