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

  //// hits[wheel][station][sector][layer][strip][roll][bx]
    int hits[5][4][12][2][5][3][200]= {{{{{{{0}}}}}}};

  vector<int> vcluster_size;
  int cluster_size = 0;
  int cluster_id = -1;
  int itr=0;

	for( auto chamber = m_inrpcDigis.begin(); chamber != m_inrpcDigis.end(); ++chamber ) {

               RPCDetId detid = (*chamber).first;
               int strip_n1 = -10000;
               int bx_n1 = -10000;
               if(detid.region()!=0 ) continue; //Region = 0 Barrel


               for( auto digi = (*chamber).second.first ; digi != (*chamber).second.second; ++digi ) {

                     if(fabs(digi->bx())>3 ) continue;
                    ///Create cluster ids and store their size
                    if((digi->strip()+1!=strip_n1)|| digi->bx()!=bx_n1){
                      if(itr!=0)vcluster_size.push_back(cluster_size);
                      cluster_size = 0;
                      cluster_id++;
                    }

                    itr++;
                    cluster_size++;
                    ///hit belongs to cluster with clusterid
                    hits[(detid.ring()+2)][(detid.station()-1)][(detid.sector()-1)][(detid.layer()-1)][(digi->bx()+2)][detid.roll()][digi->strip()]= cluster_id ;

                    ///strip of i-1
                    strip_n1 = digi->strip();
                    bx_n1 = digi->bx();

               }///for digicout
    }///for chamber
vcluster_size.push_back(cluster_size);

  for( auto chamber = m_inrpcDigis.begin(); chamber != m_inrpcDigis.end(); ++chamber ) {
           RPCDetId detid = (*chamber).first;
           if(detid.region()!=0 ) continue; //Region = 0 Barrel

               int strips[5] = {0};
               int cluster_n1 = -10;
              ///Store the last bx of strips coming from one st/sec/wheel/layer
              ///iterates from bx=1 to bx=-1
              int bx_hits[5][4][12][2][200]= {{{{{0}}}}};
              //int bx_hits[5][4][12][2]= {{{{0}}}};
               for( auto digi = (*chamber).second.first ; digi != (*chamber).second.second; ++digi ) {
                      if(fabs(digi->bx())>3 ) continue;
                      bx_hits[(detid.ring()+2)][(detid.station()-1)][(detid.sector()-1)][(detid.layer()-1)][detid.roll()] = digi->bx();
  //                    cout<<digi->bx()<<"\t"<<(detid.ring())<<"\t"<<(detid.station())<<"\t"<<(detid.sector())<<"\t"<<(detid.layer())<<"\t"<<digi->strip()<<endl;
                }

               for( auto digi = (*chamber).second.first ; digi != (*chamber).second.second; ++digi ) {
                   if(fabs(digi->bx())>3 ) continue;
                    int cluster_id =  hits[(detid.ring()+2)][(detid.station()-1)][(detid.sector()-1)][(detid.layer()-1)][(digi->bx()+2)][detid.roll()][digi->strip()];

                    ///Remove clusters with size>=4
                    if( vcluster_size[cluster_id] >=4 ) continue;
                    ///keep only one bx per st/sec/wheel/layer
                    //cout<<digi->bx()<<"\t"<<(detid.ring())<<"\t"<<(detid.station())<<"\t"<<(detid.sector())<<"\t"<<(detid.layer())<<"\t"<<digi->strip()<<"\t"<<detid.roll()<<endl;
//cout<<"bx hit"<<bx_hits[(detid.ring()+2)][(detid.station()-1)][(detid.sector()-1)][(detid.layer()-1)][detid.roll()]<<endl;

                    if(digi->bx()!=bx_hits[(detid.ring()+2)][(detid.station()-1)][(detid.sector()-1)][(detid.layer()-1)][detid.roll()] ) continue;
  //                  cout<<"bx hit after c"<<bx_hits[(detid.ring()+2)][(detid.station()-1)][(detid.sector()-1)][(detid.layer()-1)][detid.roll()]<<endl;

                    ///Count strips in a cluster
                    if(cluster_n1 != cluster_id) {strips[digi->bx()+3] = {0}; }//cout<<"reset"<<cluster_n1<<"\t"<<cluster_id<<endl;}
                    strips[digi->bx()+3] ++ ;
                    cluster_n1 = cluster_id;
                   ///FIXME keeps only the second strip if size of cluster=2
                   if( vcluster_size[cluster_id] ==2 && strips[digi->bx()+3]==2) {
                    RPCDigi digi_out(digi->strip(), digi->bx());
                    m_outrpcDigis.insertDigi(detid, digi_out);
                   }



                    if( vcluster_size[cluster_id] ==3 && strips[digi->bx()+3]!=2) continue;
                    if( vcluster_size[cluster_id] ==2 ) continue;
                    RPCDigi digi_out(digi->strip(), digi->bx());
                    m_outrpcDigis.insertDigi(detid, digi_out);

               }///for digicout
    }///for chamber

//cout<<endl;
}
