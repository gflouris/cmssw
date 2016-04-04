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



  //const L1TwinMuxParamsRcd& tmParamsRcd = c.get<L1TwinMuxParamsRcd>();
  //tmParamsRcd.get(tmParamsHandle);
  //const L1TwinMuxParams& tmParams = *tmParamsHandle.product();

  L1MuDTChambPhContainer l1ttmacont_out;
 
  AlignTrackSegments *alignedDTs = new AlignTrackSegments(*inphiDigis);
  alignedDTs->run(c);
  L1MuDTChambPhContainer phiDigis = alignedDTs->getDTContainer();

  RPCtoDTTranslator *dt_from_rpc = new RPCtoDTTranslator(*rpcDigis);
  dt_from_rpc->run(c);
  L1MuDTChambPhContainer rpcPhiDigis = dt_from_rpc->getDTContainer();




  //std::vector<L1MuDTChambPhDigi> l1ttma_out;

  DTRPCBxCorrection *rpc_dt_bx = new DTRPCBxCorrection(phiDigis,rpcPhiDigis);
  rpc_dt_bx->run(c);
  L1MuDTChambPhContainer dtrpcbxPhiDigis = rpc_dt_bx->getDTContainer();

  //std::vector<L1MuDTChambPhDigi> l1ttma_out;
  _tm_phi_output = dtrpcbxPhiDigis;


/*
  L1MuDTChambPhDigi const* ts1=0;
  L1MuDTChambPhDigi const* ts2=0;

  int bx=0, wheel=0, sector=0, station=1;





for (wheel=-3;wheel<=3; wheel++ ){
    for (sector=0;sector<12; sector++ ){
     for (station=1; station<=4; station++){
        vector<int> delta_m, delta_p, delta_0;

        for(bx=-1; bx<=1; bx++){

        ts1 = phiDigis.chPhiSegm1(wheel,station,sector,bx);
        ts2 = phiDigis.chPhiSegm2(wheel,station,sector,bx - 1 );
        shift = UNDEF;

        if(ts1 && ts1->code()<tmParams.get_USERPCBXFORDTBELOWQUALITY() ){

            //cout<<"bx  "<<bx<<"    wh"<<wheel<<"     sc"<<sector<<endl;
            for( auto chamber = rpcDigis->begin(); chamber != rpcDigis->end(); ++chamber ) {

            RPCDetId detid = (*chamber).first;

                for( auto digi = (*chamber).second.first ; digi != (*chamber).second.second; ++digi ) {
                    if( digi->bx() != 0  ) continue;
                    if(detid.region()!=0 ) continue; //Region = 0 Barrel
                    if(detid.station() != ts1->stNum()) continue;
                    if(detid.sector() != ts1->scNum()+1) continue;
                    if(detid.ring() != ts1->whNum()) continue;
                    int rpc2dt_phi = radialAngle(detid, c, digi->strip());

                    if(ts1->bxNum()==-1) delta_m.push_back( deltaPhi(ts1->phi(),rpc2dt_phi) );
                    if(ts1->bxNum()==0)  delta_0.push_back( deltaPhi(ts1->phi(),rpc2dt_phi) );
                    if(ts1->bxNum()==1)  delta_p.push_back( deltaPhi(ts1->phi(),rpc2dt_phi) );


                        //cout<<"dt   "<<ts1->bxNum()<<"   "<<ts1->phi()<<endl;
                        //cout<<"rpc  "<<digi->bx()<<"   "<<rpc2dt_phi<<endl;
                        //cout<<ts1->phi()/ (float)((96.*detid.roll()) + (digi->strip()-1) );
                        //cout<<"dt   "<<ts1->phi()*0.043945313<<"   rpc  "<<digi->strip()*(5./16.)<<endl;
                        //cout<<"dphi  "<<deltaPhi(ts1->phi(),digi->strip())<<endl;

               }
            }


          }//end if ts1 and quality
       if(ts1 && ts1->code()>=tmParams.get_USERPCBXFORDTBELOWQUALITY()){
        L1MuDTChambPhDigi shifted_out( ts1->bxNum() , ts1->whNum(), ts1->scNum(), ts1->stNum(),ts1->phi(), ts1->phiB(), ts1->code(), ts1->Ts2Tag(), ts1->BxCnt());
        l1ttma_out.push_back(shifted_out);
      }
      

      if (ts2) {
        L1MuDTChambPhDigi shifted_out( ts2->bxNum() , ts2->whNum(), ts2->scNum(), ts2->stNum(),ts2->phi(), ts2->phiB(), ts2->code(), ts2->Ts2Tag(), ts2->BxCnt());
        l1ttma_out.push_back(shifted_out);
      }
    
    }//end of bx



    vector<int> delta = concat_delta(delta_0, delta_p, delta_m);

    if(delta.size() != 0){
     
     unsigned int min_index = std::distance(delta.begin(), std::min_element(delta.begin(), delta.end())) + 1;
     //cout<<"Size of delta: "<<delta.size()<<endl;
     //cout<<"Size of zero: "<<delta_0.size()<<endl;
     //cout<<"Size of p: "<<delta_p.size()<<endl;
     //cout<<"Size of m: "<<delta_m.size()<<endl;
     //cout<<"Min index:   :"<<min_index<<endl;
     int old_bx = 0;
     if (min_index <= delta_0.size() && delta_0.size()!=0) {shift = OWN; old_bx=0;}//cout<<"No Shift"<<endl;
     else if (((delta_0.size() < min_index) && ( min_index <= delta_p.size() ) && delta_p.size()!=0 ) ) {shift = MINUS; old_bx=1;}//cout<<"Shift from p"<<endl;
     else if (((delta_p.size() < min_index) && ( min_index <= delta_m.size() ) && delta_m.size()!=0 ) ) {shift = PLUS; old_bx=-1;}//cout<<"Shift from m"<<endl;
     ts1 = phiDigis.chPhiSegm1(wheel,station,sector,old_bx);
     //cout<<ts1->bxNum()<<"\t"<<shift<<endl;

   //if(ts1){
       if(delta_0.size()!=0 ) {


          ts1 = phiDigis.chPhiSegm1(wheel,station,sector,0);
          L1MuDTChambPhDigi shifted_out( 0 , ts1->whNum(), ts1->scNum(), ts1->stNum(),ts1->phi(), ts1->phiB(), ts1->code(), ts1->Ts2Tag(), ts1->BxCnt());
          l1ttma_out.push_back(shifted_out);

          ts1 = phiDigis.chPhiSegm1(wheel,station,sector,1);
          if(ts1) L1MuDTChambPhDigi shifted_out( 1 , ts1->whNum(), ts1->scNum(), ts1->stNum(),ts1->phi(), ts1->phiB(), ts1->code(), ts1->Ts2Tag(), ts1->BxCnt());
          l1ttma_out.push_back(shifted_out);

          ts1 = phiDigis.chPhiSegm1(wheel,station,sector,-1);
          if(ts1) L1MuDTChambPhDigi shifted_out( -1 , ts1->whNum(), ts1->scNum(), ts1->stNum(),ts1->phi(), ts1->phiB(), ts1->code(), ts1->Ts2Tag(), ts1->BxCnt());
          l1ttma_out.push_back(shifted_out);

        }



        else if( delta_0.size()==0 && (delta_m.size()!=0 || delta_p.size()!=0)) {
          int new_bx = ts1->bxNum() +shift;
          cout<<"New bx = "<<new_bx<<"\t OldBx"<<old_bx<<endl;

          L1MuDTChambPhDigi shifted_out1( new_bx , ts1->whNum(), ts1->scNum(), ts1->stNum(),ts1->phi(), ts1->phiB(), ts1->code(), ts1->Ts2Tag(), ts1->BxCnt());
          l1ttma_out.push_back(shifted_out1);


        }

    //}
    }




   }//end of station
   }//end of sc
  }//end of wheel

*/
}

/*
int L1TwinMuxAlgortithm::deltaPhi(int dt_phi, int rpc2dt_phi ){

//delta_p(i) <= std_logic_vector(abs(signed(phi_dt_0) - signed(phi_rpc_0(i))));
int delta_phi = abs( dt_phi - rpc2dt_phi );
return delta_phi;

}

std::vector<int> L1TwinMuxAlgortithm::concat_delta(vector<int> delta_0, vector<int> delta_p, vector<int> delta_m){
    vector<int> delta;
    delta.insert(delta.end(), delta_0.begin(), delta_0.end());
    delta.insert(delta.end(), delta_p.begin(), delta_p.end());
    delta.insert(delta.end(), delta_m.begin(), delta_m.end());
    return delta;
}


///temporary xcheck function - will be replaced by LUTs
int L1TwinMuxAlgortithm::radialAngle(RPCDetId detid,const edm::EventSetup& c, int strip){

    int radialAngle;
    // from phiGlobal to radialAngle of the primitive in sector sec in [1..12]
    edm::ESHandle<RPCGeometry> rpcGeometry;
    c.get<MuonGeometryRecord>().get(rpcGeometry);

    const RPCRoll* roll = rpcGeometry->roll(detid);
    GlobalPoint stripPosition = roll->toGlobal(roll->centreOfStrip(strip));

    double globalphi = stripPosition.phi(); 
    int sector = (roll->id()).sector();
    if ( sector == 1) radialAngle = int( globalphi*4096 );
    else {
        if ( globalphi >= 0) radialAngle = int( (globalphi-(sector-1)*Geom::pi()/6)*4096 );
        else radialAngle = int( (globalphi+(13-sector)*Geom::pi()/6)*4096 );
    }
    return radialAngle;
}


*/