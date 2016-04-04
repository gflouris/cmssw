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
#include <iostream>
#include <iomanip>
#include <iterator>

#include "L1Trigger/L1TwinMux/interface/DTRPCBxCorrection.h"

using namespace std;

DTRPCBxCorrection::DTRPCBxCorrection(L1MuDTChambPhContainer inphiDTDigis, L1MuDTChambPhContainer inphiRPCDigis)  {
  	phiDTDigis = inphiDTDigis;
    phiRPCDigis = inphiRPCDigis;

};


void DTRPCBxCorrection::run( const edm::EventSetup& c) {

  //const L1TwinMuxParamsRcd& tmParamsRcd = c.get<L1TwinMuxParamsRcd>();
  //tmParamsRcd.get(tmParamsHandle);
  //const L1TwinMuxParams& tmParams = *tmParamsHandle.product();


  std::vector<L1MuDTChambPhDigi> l1ttma_out;


  L1MuDTChambPhDigi const* dtts1=0;
  L1MuDTChambPhDigi const* rpcts1=0;

  L1MuDTChambPhDigi const* dtts2=0;

  int bx=0, wheel=0, sector=0, station=1;





for (wheel=-3;wheel<=3; wheel++ ){
    for (sector=0;sector<12; sector++ ){
     for (station=1; station<=4; station++){
        vector<int> delta_m, delta_p, delta_0;
        vector<int> delta_m_ts2, delta_p_ts2, delta_0_ts2;

        for(bx=-1; bx<=1; bx++){
          //for(bxrpc=-1; bxrpc<=1; bxrpc++){

            dtts1 = phiDTDigis.chPhiSegm1(wheel,station,sector,bx);
            rpcts1 = phiRPCDigis.chPhiSegm1(wheel, station, sector, 0);
            dtts2 = phiDTDigis.chPhiSegm2(wheel,station,sector,bx - 1 );
            shift = UNDEF;

            //if(rpcts1 && dtts1 && dtts1->code()<tmParams.get_USERPCBXFORDTBELOWQUALITY() ){
            if(rpcts1 && dtts1 && dtts1->code()<4 ){
                        if(dtts1->bxNum()==-1) delta_m.push_back( deltaPhi(dtts1->phi(),rpcts1->phi()) );
                        if(dtts1->bxNum()==0)  delta_0.push_back( deltaPhi(dtts1->phi(),rpcts1->phi()) );
                        if(dtts1->bxNum()==1)  delta_p.push_back( deltaPhi(dtts1->phi(),rpcts1->phi()) );
                        //cout<<"bx   "<<dtts1->bxNum()<<"   "<<rpcts1->bxNum()<<endl;
                        //cout<<"phi  "<<dtts1->phi()<<"   "<<rpcts1->phi()<<endl;
              }//end if dtts1 and quality
           //if(dtts1 && dtts1->code()>=tmParams.get_USERPCBXFORDTBELOWQUALITY()){

          if(!rpcts1 && dtts1){
            L1MuDTChambPhDigi shifted_out( dtts1->bxNum() , dtts1->whNum(), dtts1->scNum(), dtts1->stNum(),dtts1->phi(), dtts1->phiB(), dtts1->code(), dtts1->Ts2Tag(), dtts1->BxCnt());
            l1ttma_out.push_back(shifted_out);

          }          

          if(rpcts1 && dtts1 && dtts1->code()>=4){ 
            L1MuDTChambPhDigi shifted_out( dtts1->bxNum() , dtts1->whNum(), dtts1->scNum(), dtts1->stNum(),dtts1->phi(), dtts1->phiB(), dtts1->code(), dtts1->Ts2Tag(), dtts1->BxCnt());
            l1ttma_out.push_back(shifted_out);
          }


           ////Track segment 2
            if(rpcts1 && dtts2 && dtts2->code()<4 ){
                        if(dtts2->bxNum()==-1) delta_m_ts2.push_back( deltaPhi(dtts2->phi(),rpcts1->phi()) );
                        if(dtts2->bxNum()==0)  delta_0_ts2.push_back( deltaPhi(dtts2->phi(),rpcts1->phi()) );
                        if(dtts2->bxNum()==1)  delta_p_ts2.push_back( deltaPhi(dtts2->phi(),rpcts1->phi()) );
                        //cout<<"bx   "<<dtts2->bxNum()<<"   "<<rpcts1->bxNum()<<endl;
                        //cout<<"phi  "<<dtts2->phi()<<"   "<<rpcts1->phi()<<endl;
              }//end if dtts2 and quality
           //if(dtts2 && dtts2->code()>=tmParams.get_USERPCBXFORDTBELOWQUALITY()){

          if(!rpcts1 && dtts2){
            L1MuDTChambPhDigi shifted_out( dtts2->bxNum() , dtts2->whNum(), dtts2->scNum(), dtts2->stNum(),dtts2->phi(), dtts2->phiB(), dtts2->code(), dtts2->Ts2Tag(), dtts2->BxCnt());
            l1ttma_out.push_back(shifted_out);

          }          

          if(rpcts1 && dtts2 && dtts2->code()>=4){ 
            L1MuDTChambPhDigi shifted_out( dtts2->bxNum() , dtts2->whNum(), dtts2->scNum(), dtts2->stNum(),dtts2->phi(), dtts2->phiB(), dtts2->code(), dtts2->Ts2Tag(), dtts2->BxCnt());
            l1ttma_out.push_back(shifted_out);
          }

          //if (dtts2) 
            //L1MuDTChambPhDigi shifted_out( dtts2->bxNum() , dtts2->whNum(), dtts2->scNum(), dtts2->stNum(),dtts2->phi(), dtts2->phiB(), dtts2->code(), dtts2->Ts2Tag(), dtts2->BxCnt());
            //l1ttma_out.push_back(shifted_out);
          //}
        
    }//end of bx



    vector<int> delta = concat_delta(delta_0, delta_p, delta_m);
    vector<int> delta_ts2 = concat_delta(delta_0_ts2, delta_p_ts2, delta_m_ts2);

    if(delta.size() != 0){
     
     unsigned int min_index = std::distance(delta.begin(), std::min_element(delta.begin(), delta.end())) + 0;
     //cout<<"Size of delta: "<<delta.size()<<endl;
     //cout<<"Size of zero: "<<delta_0.size()<<endl;
     //cout<<"Size of p: "<<delta_p.size()<<endl;
     //cout<<"Size of m: "<<delta_m.size()<<endl;
     //cout<<"Min index:   :"<<min_index<<endl;
     int old_bx = 0;
     if (min_index < delta_0.size() && delta_0.size()!=0) {shift = OWN; old_bx=0;}//cout<<"No Shift"<<endl;
     else if (((delta_0.size() <= min_index) && ( min_index < delta_p.size() ) && delta_p.size()!=0 ) ) {shift = MINUS; old_bx=1;}//cout<<"Shift from p"<<endl;
     else if (  delta_p.size() <= min_index  && delta_m.size()!=0  ) {shift = PLUS; old_bx=-1;}//cout<<"Shift from m"<<endl;
     dtts1 = phiDTDigis.chPhiSegm1(wheel,station,sector,old_bx);
     //cout<<"Old bx = "<<old_bx<<endl;
     //cout<<dtts1->bxNum()<<"\t"<<shift<<endl;

    if( delta_0.size()!=0 || (delta_m.size()!=0 || delta_p.size()!=0)) {
      int new_bx = dtts1->bxNum() +shift;
      //cout<<"New bx = "<<new_bx<<"\t OldBx"<<old_bx<<endl;

      L1MuDTChambPhDigi shifted_out1( new_bx , dtts1->whNum(), dtts1->scNum(), dtts1->stNum(),dtts1->phi(), dtts1->phiB(), dtts1->code(), dtts1->Ts2Tag(), dtts1->BxCnt());
      l1ttma_out.push_back(shifted_out1);


    }

    }


    if(delta_ts2.size() != 0){
     
     unsigned int min_index = std::distance(delta_ts2.begin(), std::min_element(delta_ts2.begin(), delta_ts2.end())) + 0;
     //cout<<"Size of delta: "<<delta.size()<<endl;
     //cout<<"Size of zero: "<<delta_0.size()<<endl;
     //cout<<"Size of p: "<<delta_p.size()<<endl;
     //cout<<"Size of m: "<<delta_m.size()<<endl;
     //cout<<"Min index:   :"<<min_index<<endl;
     int old_bx = 0;
     if (min_index < delta_0_ts2.size() && delta_0_ts2.size()!=0) {shift = OWN; old_bx=0;}//cout<<"No Shift"<<endl;
     else if (((delta_0_ts2.size() <= min_index) && ( min_index < delta_p_ts2.size() ) && delta_p_ts2.size()!=0 ) ) {shift = MINUS; old_bx=1;}//cout<<"Shift from p"<<endl;
     else if (  delta_p_ts2.size() <= min_index  && delta_m_ts2.size()!=0  ) {shift = PLUS; old_bx=-1;}//cout<<"Shift from m"<<endl;
     dtts2 = phiDTDigis.chPhiSegm2(wheel,station,sector,old_bx-1);
     //cout<<"Old bx = "<<old_bx<<endl;
     //cout<<dtts1->bxNum()<<"\t"<<shift<<endl;

    if( delta_0_ts2.size()!=0 || (delta_m_ts2.size()!=0 || delta_p_ts2.size()!=0)) {
      int new_bx = dtts2->bxNum() +shift;
      //cout<<"New bx = "<<new_bx<<"\t OldBx"<<old_bx<<endl;

      L1MuDTChambPhDigi shifted_out2( new_bx , dtts2->whNum(), dtts2->scNum(), dtts2->stNum(),dtts2->phi(), dtts2->phiB(), dtts2->code(), dtts2->Ts2Tag(), dtts2->BxCnt());
      l1ttma_out.push_back(shifted_out2);


    }

    }

   }//end of station
   }//end of sc
  }//end of wheel



_dt_tsshifted.setContainer(l1ttma_out);

}



int DTRPCBxCorrection::deltaPhi(int dt_phi, int rpc2dt_phi ){

//delta_p(i) <= std_logic_vector(abs(signed(phi_dt_0) - signed(phi_rpc_0(i))));
int delta_phi = abs( dt_phi - rpc2dt_phi );
return delta_phi;

}

std::vector<int> DTRPCBxCorrection::concat_delta(vector<int> delta_0, vector<int> delta_p, vector<int> delta_m){
    vector<int> delta;
    delta.insert(delta.end(), delta_0.begin(), delta_0.end());
    delta.insert(delta.end(), delta_p.begin(), delta_p.end());
    delta.insert(delta.end(), delta_m.begin(), delta_m.end());
    return delta;
}

