//-------------------------------------------------
//
//   Class: DTRPCBxCorrection
//
//   DTRPCBxCorrection
//
//
//   Author :
//   G. Flouris               U Ioannina    Mar. 2015
//--------------------------------------------------

#ifndef DTRPCBxCorrection_H
#define DTRPCBxCorrection_H

#include "DataFormats/L1DTTrackFinder/interface/L1MuDTChambPhContainer.h"
#include "DataFormats/L1DTTrackFinder/interface/L1MuDTChambThContainer.h"

#include "CondFormats/L1TObjects/interface/L1TwinMuxParams.h"
#include "CondFormats/DataRecord/interface/L1TwinMuxParamsRcd.h"


#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/EventSetup.h"

#include <iostream>

class DTRPCBxCorrection  {
public:
  DTRPCBxCorrection(L1MuDTChambPhContainer , L1MuDTChambPhContainer );
  ~DTRPCBxCorrection() {};

  void run(const edm::EventSetup& c);

 ///Return Output PhContainer
 L1MuDTChambPhContainer getDTContainer(){  return _dt_tsshifted;}

  //edm::ESHandle< L1TwinMuxParams > tmParamsHandle;


private:
  int deltaPhi(int dt_phi, int rpc_strip );
  std::vector<int> concat_delta(std::vector<int>, std::vector<int>, std::vector<int>);
  enum SHIFT {OWN = 0, PLUS = 1, MINUS = -1, UNDEF = 0};
  SHIFT shift;

  ///Output PhContainer
  L1MuDTChambPhContainer _dt_tsshifted;
  L1MuDTChambPhContainer phiDTDigis;
  L1MuDTChambPhContainer phiRPCDigis;

};
#endif
