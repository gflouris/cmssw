//-------------------------------------------------
//
//   Class: RPCHitCleaner
//
//   RPCHitCleaner 
//
//
//   Author :
//   G. Flouris               U Ioannina    Mar. 2015
//--------------------------------------------------

#include <iostream>
#include <iomanip>
#include <iterator>
#include <cmath>

#include "L1Trigger/L1TwinMux/interface/RPCHitCleaner.h"
#include "Geometry/RPCGeometry/interface/RPCRoll.h"
#include "Geometry/RPCGeometry/interface/RPCGeometry.h"
#include "CondFormats/DataRecord/interface/RPCEMapRcd.h"
#include "Geometry/Records/interface/MuonGeometryRecord.h"
#include "L1Trigger/L1TwinMux/interface/RPCtoDTTranslator.h"

using namespace std;


RPCHitCleaner::RPCHitCleaner(RPCDigiCollection inrpcDigis){

m_inrpcDigis = inrpcDigis;

}

void RPCHitCleaner::run(const edm::EventSetup& c) {
  //cout<<"RPCHitCleaner"<<endl;
	for( auto chamber = m_inrpcDigis.begin(); chamber != m_inrpcDigis.end(); ++chamber ) {
           int hits_st2 = 0, hits_st4 = 0;
           RPCDetId detid = (*chamber).first;

               for( auto digi = (*chamber).second.first ; digi != (*chamber).second.second; ++digi ) {

                    RPCDigi digi_out(digi->strip(), digi->bx());
                    if(digi->bx()!=0) continue;

                    m_outrpcDigis.insertDigi(detid, digi_out);

                    if(detid.station() == 2) hits_st2++;
                    if(hits_st2>4) break;

                    if(detid.station() == 4) hits_st4++;
                    if(hits_st4>4) break;

               }///for digicout
    }///for chamber   
//cout<<endl;    
}

