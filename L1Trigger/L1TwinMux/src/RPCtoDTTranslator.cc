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

rpcDigis = inrpcDigis;

}

void RPCtoDTTranslator::run(const edm::EventSetup& c) {

  std::vector<L1MuDTChambPhDigi> l1ttma_out;

  std::vector<rpc_hit> vrpc_hit_layer1, vrpc_hit_layer2, vrpc_hit_st34;

	for( auto chamber = rpcDigis.begin(); chamber != rpcDigis.end(); ++chamber ) {

           RPCDetId detid = (*chamber).first;

               for( auto digi = (*chamber).second.first ; digi != (*chamber).second.second; ++digi ) {
                    //cout<<detid.region()<<"\t"<<detid.station()<<"\t"<<endl;
                   if(detid.region()!=0 ) continue; //Region = 0 Barrel
                   //if(detid.station() == 3 || detid.station() == 4) continue;
                   if(digi->bx()>1 || digi->bx()<-1) continue; 

                   if(detid.layer()==1) vrpc_hit_layer1.push_back({detid.station(), detid.sector(), detid.ring(), detid, digi->strip() });
                   if(detid.layer()==2) vrpc_hit_layer2.push_back({detid.station(), detid.sector(), detid.ring(), detid, digi->strip() });
                   if(detid.station() == 3 || detid.station() == 4) vrpc_hit_st34.push_back({detid.station(), detid.sector(), detid.ring(), detid, digi->strip() });

               }///for digicout
    }///for chamber   



   for(int wh=-2; wh<=2; wh++){
    for(int sec=1; sec<=12; sec++){  
        for(int st=1; st<=4; st++){
            std::vector<int> delta_phib;
            bool found_hits = false;
            std::vector<int> rpc2dt_phi, rpc2dt_phib;

            for(unsigned int l1=0; l1<vrpc_hit_layer1.size(); l1++){
                for(unsigned int l2=0; l2<vrpc_hit_layer2.size(); l2++){

                        if(vrpc_hit_layer1[l1].station!=st || vrpc_hit_layer2[l2].station!=st ) continue;
                        if(vrpc_hit_layer1[l1].sector!=sec || vrpc_hit_layer2[l2].sector!=sec ) continue;
                        if(vrpc_hit_layer1[l1].wheel!=wh || vrpc_hit_layer2[l2].wheel!=wh ) continue;

                        ///FIXME average and then shift by 2
                        int phi1 = radialAngle(vrpc_hit_layer1[l1].detid, c, vrpc_hit_layer1[l1].strip) ;
                        int phi2 = radialAngle(vrpc_hit_layer2[l2].detid, c, vrpc_hit_layer2[l2].strip) ;
                        rpc2dt_phi.push_back((phi1+phi2)/2);

                        float x1 = localX(vrpc_hit_layer1[l1].detid, c, vrpc_hit_layer1[l1].strip);
                        float x2 = -localX(vrpc_hit_layer2[l2].detid, c, vrpc_hit_layer2[l2].strip);
                        int phi_b = bendingAngle(x1,x2,phi1, phi2);
                        rpc2dt_phib.push_back(phi_b);

                        //cout<<x1<<"\t"<<x2<<"\t"<<phi1<<"\t"<<phi2<<"\t"<<phi_b<<endl;
                        //cout<<vrpc_hit_layer1[l1].sector<<"\t"<<vrpc_hit_layer2[l2].sector<<endl;
                        //cout<<vrpc_hit_layer1[l1].wheel<<"\t"<<vrpc_hit_layer2[l2].wheel<<endl<<endl;
                        ///delta_phib to find the highest pt primitve
                        delta_phib.push_back(abs(phi_b));
                        found_hits = true;
                        
                }
            }
            for(unsigned int l1=0; l1<vrpc_hit_st34.size(); l1++){
                        
                        if(vrpc_hit_st34[l1].station==1 || vrpc_hit_st34[l1].station==2 ) continue;
                        int phi1 = radialAngle(vrpc_hit_st34[l1].detid, c, vrpc_hit_st34[l1].strip) ;

                        L1MuDTChambPhDigi rpc2dt_out( 0, wh, sec-1, st, phi1, 0, 3, 0, 0, 1);
                        l1ttma_out.push_back(rpc2dt_out);
            }
            if(found_hits){
                int min_index = std::distance(delta_phib.begin(), std::min_element(delta_phib.begin(), delta_phib.end())) + 0;
                L1MuDTChambPhDigi rpc2dt_out( 0, wh, sec-1, st, rpc2dt_phi[min_index], rpc2dt_phib[min_index], 3, 0, 0, 1);
                l1ttma_out.push_back(rpc2dt_out);
            }   

       }
    }
   }

_rpcdt_translated.setContainer(l1ttma_out);

}


///temporary function - will be replaced by LUTs(?)
int RPCtoDTTranslator::radialAngle(RPCDetId detid,const edm::EventSetup& c, int strip){

    int radialAngle;
    // from phiGlobal to radialAngle of the primitive in sector sec in [1..12]
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
    return radialAngle<<2;
}

///temporary function - will be replaced by LUTs(?)
int RPCtoDTTranslator::localX(RPCDetId detid, const edm::EventSetup& c, int strip){

    edm::ESHandle<RPCGeometry> rpcGeometry;
    c.get<MuonGeometryRecord>().get(rpcGeometry);

    const RPCRoll* roll = rpcGeometry->roll(detid);
    GlobalPoint stripPosition = roll->toGlobal(roll->centreOfStrip(strip));
    LocalPoint  stripPositionlocal = roll->toLocal(stripPosition);

    return stripPositionlocal.x(); 
}

int RPCtoDTTranslator::bendingAngle(float x1, float x2, float phi1, float phi2){
    return (atan((x2-x1)/35.) - (phi1+phi2)/(2.*4096.)) * 512.;
}
