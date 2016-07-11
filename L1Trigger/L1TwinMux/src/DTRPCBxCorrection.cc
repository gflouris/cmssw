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
  	m_phiDTDigis = inphiDTDigis;
    m_phiRPCDigis = inphiRPCDigis;

};


void DTRPCBxCorrection::run( const edm::EventSetup& c) {

  BxCorrection(1);
  BxCorrection(2);

  m_dt_tsshifted.setContainer(l1ttma_out);

  const L1TwinMuxParamsRcd& tmParamsRcd = c.get<L1TwinMuxParamsRcd>();
  tmParamsRcd.get(tmParamsHandle);
  const L1TwinMuxParams& tmParams = *tmParamsHandle.product();

  m_QualityLimit = tmParams.get_USERPCBXFORDTBELOWQUALITY();

}

void DTRPCBxCorrection::BxCorrection(int track_seg){


  L1MuDTChambPhDigi const* dtts=0;
  L1MuDTChambPhDigi const* rpcts1=0;

  int bx=0, wheel=0, sector=0, station=1;


for (wheel=-2;wheel<=2; wheel++ ){
    for (sector=0;sector<12; sector++ ){
     for (station=1; station<=4; station++){
        vector<int> delta_m, delta_p, delta_0;

        for(bx=-1; bx<=1; bx++){
            dtts=0; rpcts1=0;
            if(track_seg==1)
              dtts = m_phiDTDigis.chPhiSegm1(wheel,station,sector,bx);
            else
              dtts = m_phiDTDigis.chPhiSegm2(wheel,station,sector,bx - 1 );
            rpcts1 = m_phiRPCDigis.chPhiSegm1(wheel, station, sector, 0);
            m_shift = UNDEF;

            if(rpcts1 && dtts && dtts->code()<m_QualityLimit ){
                        if(dtts->bxNum()==-1) delta_m.push_back( deltaPhi(dtts->phi(),rpcts1->phi()) );
                        if(dtts->bxNum()==0)  delta_0.push_back( deltaPhi(dtts->phi(),rpcts1->phi()) );
                        if(dtts->bxNum()==1)  delta_p.push_back( deltaPhi(dtts->phi(),rpcts1->phi()) );
              }//end if dtts and quality

          if(!rpcts1 && dtts){
            L1MuDTChambPhDigi shifted_out( dtts->bxNum() , dtts->whNum(), dtts->scNum(), dtts->stNum(),dtts->phi(), dtts->phiB(), dtts->code(), dtts->Ts2Tag(), dtts->BxCnt());
            l1ttma_out.push_back(shifted_out);
          }          

          if(rpcts1 && dtts && dtts->code()>=m_QualityLimit){ 
            L1MuDTChambPhDigi shifted_out( dtts->bxNum() , dtts->whNum(), dtts->scNum(), dtts->stNum(),dtts->phi(), dtts->phiB(), dtts->code(), dtts->Ts2Tag(), dtts->BxCnt());
            l1ttma_out.push_back(shifted_out);
          }
    }//end of bx



    vector<int> delta = concat_delta(delta_0, delta_p, delta_m);

    if(delta.size() != 0){
     
     unsigned int min_index = std::distance(delta.begin(), std::min_element(delta.begin(), delta.end())) + 0;
     ///Track Segment 1
     if (delta_0.size()!=0) {
      if(track_seg==1)
        dtts = m_phiDTDigis.chPhiSegm1(wheel,station,sector,0);
      else 
        dtts = m_phiDTDigis.chPhiSegm2(wheel,station,sector,0-1);

      L1MuDTChambPhDigi shifted_out1( dtts->bxNum() , dtts->whNum(), dtts->scNum(), dtts->stNum(),dtts->phi(), dtts->phiB(), dtts->code(), dtts->Ts2Tag(), dtts->BxCnt());
      l1ttma_out.push_back(shifted_out1);

      if(delta_p.size()!=0){
        if(track_seg==1)
          dtts = m_phiDTDigis.chPhiSegm1(wheel,station,sector,1);
        else
          dtts = m_phiDTDigis.chPhiSegm2(wheel,station,sector,1-1);

        L1MuDTChambPhDigi shifted_out1( dtts->bxNum() , dtts->whNum(), dtts->scNum(), dtts->stNum(),dtts->phi(), dtts->phiB(), dtts->code(), dtts->Ts2Tag(), dtts->BxCnt());
        l1ttma_out.push_back(shifted_out1);
      }

      if(delta_m.size()!=0){
        if(track_seg==1)     
          dtts = m_phiDTDigis.chPhiSegm1(wheel,station,sector,-1);
        else
          dtts = m_phiDTDigis.chPhiSegm2(wheel,station,sector,-1-1);
        
        L1MuDTChambPhDigi shifted_out1( dtts->bxNum() , dtts->whNum(), dtts->scNum(), dtts->stNum(),dtts->phi(), dtts->phiB(), dtts->code(), dtts->Ts2Tag(), dtts->BxCnt());
        l1ttma_out.push_back(shifted_out1);
      }
     }//cout<<"No Shift"<<endl;
     else if ( delta_0.size() ==0 &&  ((delta_0.size() <= min_index) && ( min_index < delta_p.size() ) && delta_p.size()!=0 ) ) {
      if(track_seg==1)
        dtts = m_phiDTDigis.chPhiSegm1(wheel,station,sector,0);
        else
          dtts = m_phiDTDigis.chPhiSegm2(wheel,station,sector,0-1);

      if(!dtts || dtts->code()<m_QualityLimit) {
      if(track_seg==1)  
        dtts = m_phiDTDigis.chPhiSegm1(wheel,station,sector,1);
      else
        dtts = m_phiDTDigis.chPhiSegm2(wheel,station,sector,1-1);
      
      L1MuDTChambPhDigi shifted_out1( dtts->bxNum() - 1, dtts->whNum(), dtts->scNum(), dtts->stNum(),dtts->phi(), dtts->phiB(), dtts->code(), dtts->Ts2Tag(), dtts->BxCnt());
      l1ttma_out.push_back(shifted_out1);
      } 
      else{
        if(track_seg==1)      
          dtts = m_phiDTDigis.chPhiSegm1(wheel,station,sector,1);
        else
          dtts = m_phiDTDigis.chPhiSegm2(wheel,station,sector,1-1);

        L1MuDTChambPhDigi shifted_out1( dtts->bxNum() , dtts->whNum(), dtts->scNum(), dtts->stNum(),dtts->phi(), dtts->phiB(), dtts->code(), dtts->Ts2Tag(), dtts->BxCnt());
        l1ttma_out.push_back(shifted_out1);
      }

     }//cout<<"Shift from p"<<endl;
     else if ( delta_0.size() ==0 &&  delta_p.size() <= min_index  && delta_m.size()!=0  ) {
      if(track_seg==1)    
        dtts = m_phiDTDigis.chPhiSegm1(wheel,station,sector,0);
        else
          dtts = m_phiDTDigis.chPhiSegm2(wheel,station,sector,0-1);

      if(!dtts || dtts->code()<m_QualityLimit) {
        if(track_seg==1)
          dtts = m_phiDTDigis.chPhiSegm1(wheel,station,sector,-1);
        else
          dtts = m_phiDTDigis.chPhiSegm2(wheel,station,sector,-1-1);

        L1MuDTChambPhDigi shifted_out1( dtts->bxNum() + 1, dtts->whNum(), dtts->scNum(), dtts->stNum(),dtts->phi(), dtts->phiB(), dtts->code(), dtts->Ts2Tag(), dtts->BxCnt());
        l1ttma_out.push_back(shifted_out1);
      }
      else{
        if(track_seg==1)      
          dtts = m_phiDTDigis.chPhiSegm1(wheel,station,sector,-1);
        else
          dtts = m_phiDTDigis.chPhiSegm2(wheel,station,sector,-1-1);

        L1MuDTChambPhDigi shifted_out1( dtts->bxNum() , dtts->whNum(), dtts->scNum(), dtts->stNum(),dtts->phi(), dtts->phiB(), dtts->code(), dtts->Ts2Tag(), dtts->BxCnt());
        l1ttma_out.push_back(shifted_out1);
      }
     }//cout<<"Shift from m"<<endl;
    }

   }//end of station
   }//end of sc
  }//end of wheel

}

int DTRPCBxCorrection::deltaPhi(int dt_phi, int rpc2dt_phi ){
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

