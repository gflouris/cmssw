//-------------------------------------------------
//
//   Class: RPCtoDTTranslator
//
//   RPCtoDTTranslator
//
//
//   Author :
//   G. Flouris               U Ioannina    Mar. 2015
//--------------------------------------------------

#include <iostream>
#include <iomanip>
#include <iterator>
#include <cmath>

#include "L1Trigger/L1TwinMux/interface/RPCtoDTTranslator.h"
#include "Geometry/RPCGeometry/interface/RPCRoll.h"
#include "Geometry/RPCGeometry/interface/RPCGeometry.h"
#include "CondFormats/DataRecord/interface/RPCEMapRcd.h"
#include "Geometry/Records/interface/MuonGeometryRecord.h"

using namespace std;


RPCtoDTTranslator::RPCtoDTTranslator(RPCDigiCollection inrpcDigis){

m_rpcDigis = inrpcDigis;

}

void RPCtoDTTranslator::run(const edm::EventSetup& c) {

  std::vector<L1MuDTChambPhDigi> l1ttma_out;
  std::vector<L1MuDTChambPhDigi> l1ttma_hits_out;

  std::vector<rpc_hit> vrpc_hit_layer1, vrpc_hit_layer2, vrpc_hit_st3, vrpc_hit_st4;

  int max_rpc_bx = 2; int min_rpc_bx = -2;

    ///Init structues
	for( auto chamber = m_rpcDigis.begin(); chamber != m_rpcDigis.end(); ++chamber ) {

           RPCDetId detid = (*chamber).first;

               for( auto digi = (*chamber).second.first ; digi != (*chamber).second.second; ++digi ) {
                   if(detid.region()!=0 ) continue; //Region = 0 Barrel
                   if(digi->bx()>max_rpc_bx || digi->bx()<min_rpc_bx) continue; 

                   if(detid.layer()==1) vrpc_hit_layer1.push_back({digi->bx(), detid.station(), detid.sector(), detid.ring(), detid, digi->strip() });
                   if(detid.layer()==2) vrpc_hit_layer2.push_back({digi->bx(), detid.station(), detid.sector(), detid.ring(), detid, digi->strip() });
                   if(detid.station()==3) vrpc_hit_st3.push_back({digi->bx(), detid.station(), detid.sector(), detid.ring(), detid, digi->strip() });
                   if(detid.station()==4) vrpc_hit_st4.push_back({digi->bx(), detid.station(), detid.sector(), detid.ring(), detid, digi->strip() });

               }
    }///for chamber   


   for(int wh=-2; wh<=2; wh++){
    for(int sec=1; sec<=12; sec++){  
        for(int st=1; st<=4; st++){
            int rpcbx = 0;
            std::vector<int> delta_phib;
            bool found_hits = false;
            std::vector<int> rpc2dt_phi, rpc2dt_phib;

            for(unsigned int l1=0; l1<vrpc_hit_layer1.size(); l1++){
                for(unsigned int l2=0; l2<vrpc_hit_layer2.size(); l2++){

                        if(vrpc_hit_layer1[l1].station!=st || vrpc_hit_layer2[l2].station!=st ) continue;
                        if(vrpc_hit_layer1[l1].sector!=sec || vrpc_hit_layer2[l2].sector!=sec ) continue;
                        if(vrpc_hit_layer1[l1].wheel!=wh || vrpc_hit_layer2[l2].wheel!=wh ) continue;

                        int phi1 = radialAngle(vrpc_hit_layer1[l1].detid, c, vrpc_hit_layer1[l1].strip) ;
                        int phi2 = radialAngle(vrpc_hit_layer2[l2].detid, c, vrpc_hit_layer2[l2].strip) ;
                        int average = ( (phi1 + phi2)/2 ); //10-bit
                        rpc2dt_phi.push_back(average<<2);  //Convert and store to 12-bit

                        int x1 = localX(vrpc_hit_layer1[l1].detid, c, vrpc_hit_layer1[l1].strip);
                        int x2 = -localX(vrpc_hit_layer2[l2].detid, c, vrpc_hit_layer2[l2].strip);

                        int phi_b = bendingAngle(x1,x2,average);
                        rpc2dt_phib.push_back(phi_b);
                        
                        ///delta_phib to find the highest pt primitve
                        delta_phib.push_back(abs(phi_b));
                        found_hits = true;
                        rpcbx = vrpc_hit_layer1[l1].bx;

                }

            }
///Use ts2tag variable to store N rpchits for the same st/wheel/sec
            int bx_range = (max_rpc_bx - min_rpc_bx) + 1 ;
            int hit[bx_range] = {0};   ///store the number of RPC->DT for each bx, stored as ts2tag
            for(unsigned int l1=0; l1<vrpc_hit_layer1.size(); l1++){
                if(vrpc_hit_layer1[l1].station!=st || vrpc_hit_layer1[l1].sector!=sec || vrpc_hit_layer1[l1].wheel!=wh) continue;
                    int phi2 = radialAngle(vrpc_hit_layer1[l1].detid, c, vrpc_hit_layer1[l1].strip) ;
                    phi2 = phi2<<2;
                    L1MuDTChambPhDigi rpc2dt_out( vrpc_hit_layer1[l1].bx, wh, sec-1, st, phi2, 0, 3, hit[vrpc_hit_layer1[l1].bx+2], 0, 2);
                    hit[vrpc_hit_layer1[l1].bx+2]++;
                    l1ttma_hits_out.push_back(rpc2dt_out);
            }

            for(unsigned int l2=0; l2<vrpc_hit_layer2.size(); l2++){
                if(vrpc_hit_layer2[l2].station!=st || vrpc_hit_layer2[l2].sector!=sec || vrpc_hit_layer2[l2].wheel!=wh) continue;
                    int phi2 = radialAngle(vrpc_hit_layer2[l2].detid, c, vrpc_hit_layer2[l2].strip) ;
                    phi2 = phi2<<2;
                    //cout<<vrpc_hit_layer2[l2].bx<<"\t"<<wh<<"\t"<<sec<<"\t"<<st<<"\t"<<phi2<<"\t"<<hit[vrpc_hit_layer2[l2].bx+2]<<endl;
                    L1MuDTChambPhDigi rpc2dt_out( vrpc_hit_layer2[l2].bx, wh, sec-1, st, phi2, 0, 3, hit[vrpc_hit_layer2[l2].bx+2] , 0, 2);
                    hit[vrpc_hit_layer2[l2].bx+2]++;
                    l1ttma_hits_out.push_back(rpc2dt_out);
            }

            for(unsigned int l1=0; l1<vrpc_hit_st3.size(); l1++){
                        
                        if(st!=3 || vrpc_hit_st3[l1].station!=3 || vrpc_hit_st3[l1].wheel!=wh || vrpc_hit_st3[l1].sector!=sec) continue;
                        int phi1 = radialAngle(vrpc_hit_st3[l1].detid, c, vrpc_hit_st3[l1].strip) ;
                        phi1 = phi1<<2;
                        L1MuDTChambPhDigi rpc2dt_out( vrpc_hit_st3[l1].bx, wh, sec-1, st, phi1, 0, 3, hit[vrpc_hit_st3[l1].bx+2], 0, 2);
                        hit[vrpc_hit_st3[l1].bx+2]++;
                        l1ttma_out.push_back(rpc2dt_out);
                        l1ttma_hits_out.push_back(rpc2dt_out);
                        //break;
            }

            for(unsigned int l1=0; l1<vrpc_hit_st4.size(); l1++){
                        if(st!=4 || vrpc_hit_st4[l1].station!=4 || vrpc_hit_st4[l1].wheel!=wh || vrpc_hit_st4[l1].sector!=sec) continue;
                        int phi1 = radialAngle(vrpc_hit_st4[l1].detid, c, vrpc_hit_st4[l1].strip) ;
                        phi1 = phi1<<2;
                        L1MuDTChambPhDigi rpc2dt_out( vrpc_hit_st4[l1].bx, wh, sec-1, st, phi1, 0, 3, hit[vrpc_hit_st4[l1].bx] , 0, 2);
                        hit[vrpc_hit_st4[l1].bx]++;
                        l1ttma_out.push_back(rpc2dt_out);
                        l1ttma_hits_out.push_back(rpc2dt_out);
                        //break;
            }             
            if(found_hits){

                int min_index = std::distance(delta_phib.begin(), std::min_element(delta_phib.begin(), delta_phib.end())) + 0;
                L1MuDTChambPhDigi rpc2dt_out( rpcbx, wh, sec-1, st, rpc2dt_phi[min_index], rpc2dt_phib[min_index], 3, 0, 0, 2);
                l1ttma_out.push_back(rpc2dt_out);
            }   

       }
    }
   }
///Container to store RPC->DT for RPC only (only in stations 1 and 2 (2 layers->phib))
m_rpcdt_translated.setContainer(l1ttma_out);
///Container to store RPC->DT for Bx correction
m_rpchitsdt_translated.setContainer(l1ttma_hits_out);

}


///function - will be replaced by LUTs(?)
int RPCtoDTTranslator::radialAngle(RPCDetId detid,const edm::EventSetup& c, int strip){

    int radialAngle;
    // from phiGlobal to radialAngle of the primitive in sector sec in [1..12] <- RPC scheme
    edm::ESHandle<RPCGeometry> rpcGeometry;
    c.get<MuonGeometryRecord>().get(rpcGeometry);

    const RPCRoll* roll = rpcGeometry->roll(detid);
    GlobalPoint stripPosition = roll->toGlobal(roll->centreOfStrip(strip));

    double globalphi = stripPosition.phi(); 
    int sector = (roll->id()).sector();
    if ( sector == 1) radialAngle = int( globalphi*1024 );
    else {
        if ( globalphi >= 0) radialAngle = int( (globalphi-(sector-1)*Geom::pi()/6)*1024 );
        else radialAngle = int( (globalphi+(13-sector)*Geom::pi()/6)*1024 );
    }
    return radialAngle;
}

///function - will be replaced by LUTs(?)
int RPCtoDTTranslator::localX(RPCDetId detid, const edm::EventSetup& c, int strip){
    edm::ESHandle<RPCGeometry> rpcGeometry;
    c.get<MuonGeometryRecord>().get(rpcGeometry);

    const RPCRoll* roll = rpcGeometry->roll(detid);
    return roll->centreOfStrip(strip).x()/2.;
}

int RPCtoDTTranslator::bendingAngle(int x1, int x2, int phi){   
//int RPCtoDTTranslator::bendingAngle(int phi){   
    int rvalue = atan((x1-x2)/17.) * 512. - phi;
    return rvalue;
    //int rvalue = phi * 7;
    //if(rvalue>512) return 512;
    //if(rvalue<-512) return -512;
    //return rvalue;
}
