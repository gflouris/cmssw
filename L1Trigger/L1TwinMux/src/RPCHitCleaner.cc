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
  
  //// hits[wheel][station][sector][layer][strip][bx]
  int hits[5][4][12][2][5][200]= {{{{{{0}}}}}};
  //int hits_out[5][4][12][2][5] = {{{{{0}}}}};


  ///Counts the hits per bx per st,sect,etc
  int strip_n1 = -10000;
  vector<int> cluster;
  int cluster_size = 0;
  int cluster_id = 0;
  int itr=0;

  //std::map<int, int[5][4][12][2][5]> map_hits;

	for( auto chamber = m_inrpcDigis.begin(); chamber != m_inrpcDigis.end(); ++chamber ) {
               RPCDetId detid = (*chamber).first;

               for( auto digi = (*chamber).second.first ; digi != (*chamber).second.second; ++digi ) {


//cout<<digi->strip()<<"\t"<<(detid.ring()+2)<<"\t"<<(detid.station()-1)<<"\t"<<(detid.sector()-1)<<"\t"<<(detid.layer()-1)<<"\t"<<(digi->bx()+2)<<"\t"<<cluster_id<<endl;


                    if(digi->strip()+1!=strip_n1) {
                      cluster.push_back(cluster_size);
                      cluster_size = 0;  
                      cluster_id++;
                    }
                    itr++;
                    cluster_size++;
                    ///hit belongs to cluster with clusterid
                    hits[(detid.ring()+2)][(detid.station()-1)][(detid.sector()-1)][(detid.layer()-1)][(digi->bx()+2)][digi->strip()]= cluster_id ;

                    ///strip of i-1
                    strip_n1 = digi->strip();
               }///for digicout
    }///for chamber   

//cout<<"---------"<<cluster.size()<<endl;
  for( auto chamber = m_inrpcDigis.begin(); chamber != m_inrpcDigis.end(); ++chamber ) {
           RPCDetId detid = (*chamber).first;

               for( auto digi = (*chamber).second.first ; digi != (*chamber).second.second; ++digi ) {
                    ///Firmware 'removes' data if hits>=4 in the same unit of detector and time
                  //  cout<<(detid.ring()+2)<<"\t"<<(detid.station()-1)<<"\t"<<(detid.sector()-1)<<"\t"<<(detid.layer()-1)<<"\t"<<(digi->bx()+2)<<"\t"<<endl;                
                //cout<<hits[(detid.ring()+2)][(detid.station()-1)][(detid.sector()-1)][(detid.layer()-1)][(digi->bx()+2)]<<"\t"<<cluster[hits[(detid.ring()+2)][(detid.station()-1)][(detid.sector()-1)][(detid.layer()-1)][(digi->bx()+2)]]<<endl;
                    int cluster_id =  hits[(detid.ring()+2)][(detid.station()-1)][(detid.sector()-1)][(detid.layer()-1)][(digi->bx()+2)][digi->strip()];

                    if( cluster[cluster_id] >=4) continue;


      
                    RPCDigi digi_out(digi->strip(), digi->bx());
                    m_outrpcDigis.insertDigi(detid, digi_out);
                    
                    ///Firmware processes only 2 hits in the same unit of detector and time
                    //if( hits_out[(detid.ring()+2)][(detid.station()-1)][(detid.sector()-1)][(detid.layer()-1)][(digi->bx()+2)] >=2 ) continue;
                    //hits_out[(detid.ring()+2)][(detid.station()-1)][(detid.sector()-1)][(detid.layer()-1)][(digi->bx()+2)]++;
                    

               }///for digicout
    }///for chamber   

//cout<<endl;    
}

