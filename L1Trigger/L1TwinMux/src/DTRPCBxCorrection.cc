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

  const L1TwinMuxParamsRcd& tmParamsRcd = c.get<L1TwinMuxParamsRcd>();
  tmParamsRcd.get(tmParamsHandle);
  const L1TwinMuxParams& tmParams = *tmParamsHandle.product();


  m_QualityLimit = tmParams.get_USERPCBXFORDTBELOWQUALITY();
  m_DphiWindow   = tmParams.get_DphiWindowBxShift();

  BxCorrection(0);
  BxCorrection(1);

  m_dt_tsshifted.setContainer(l1ttma_out);


}

void DTRPCBxCorrection::BxCorrection(int track_seg){

  L1MuDTChambPhDigi * dtts=0;
  L1MuDTChambPhDigi * dttsnew=0;
  L1MuDTChambPhDigi * rpcts1=0;

  int bx=0, wheel=0, sector=0, station=1;
  int ibx_dtm = 0, fbx_dtm = 0;
  int ibx_dtp = 0, fbx_dtp = 0;

for (wheel=-2;wheel<=2; wheel++ ){
    for (sector=0;sector<12; sector++ ){
     for (station=1; station<=4; station++){
    	bool shifted[7] = {false, false, false, false,false, false, false};
    	bool dups[7] = {false, false, false, false,false, false, false};
			bool secondTs[7] = {false, false, false, false,false, false, false};

    	L1MuDTChambPhContainer shiftedPhiDTDigis;
			L1MuDTChambPhDigi *dtts_sh2nd = 0;

        for(bx=3; bx>=-3; bx--){
        vector<int> delta_m, delta_p, delta_0;

          for(int rpcbx=bx-1; rpcbx<=bx+1; rpcbx++){
            dtts=0; rpcts1=0; dttsnew = 0;
            dtts = m_phiDTDigis.chPhiSegm(wheel,station,sector,bx ,track_seg);
            if(!dtts) continue;

            int nhits = noRPCHits(m_phiRPCDigis, rpcbx, wheel, sector, station);
            for(int hit=0; hit<nhits; hit++){
            rpcts1 = m_phiRPCDigis.chPhiSegm(wheel, station, sector, rpcbx,hit);
            m_shift = UNDEF;

            if(rpcts1 && dtts && dtts->code()<m_QualityLimit && deltaPhi(dtts->phi(),rpcts1->phi()) < m_DphiWindow){
		           if((dtts->bxNum()-rpcbx)==-1  ) {
		             delta_m.push_back( deltaPhi(dtts->phi(),rpcts1->phi()) );
                 ibx_dtm = dtts->bxNum(); fbx_dtm = rpcbx;
               }

               if((dtts->bxNum()-rpcbx)==0 )  {
		              delta_0.push_back( deltaPhi(dtts->phi(),rpcts1->phi()) );
               }

               if((dtts->bxNum()-rpcbx)==1 )  {
	               delta_p.push_back( deltaPhi(dtts->phi(),rpcts1->phi()) );
      		       ibx_dtp = dtts->bxNum(); fbx_dtp = rpcbx;
                }
              }//end if dtts and quality
            }
        }//end of rpc bx

    vector<int> delta = concat_delta(delta_0, delta_p, delta_m);

    if(delta.size() != 0){
    L1MuDTChambPhDigi *dtts_sh = 0;
    std::vector<L1MuDTChambPhDigi> l1ttma_outsh;

     unsigned int min_index = std::distance(delta.begin(), std::min_element(delta.begin(), delta.end())) + 0;

      if ( ((delta_0.size() <= min_index) && ( min_index < (delta_0.size() + delta_p.size()) ) && delta_p.size()!=0 ) ) {
        dtts = m_phiDTDigis.chPhiSegm(wheel,station,sector,ibx_dtp,track_seg);
        dttsnew = m_phiDTDigis.chPhiSegm(wheel,station,sector,fbx_dtp,track_seg);
				bool shift_1 = false;
      	if(dtts && dtts->code()<m_QualityLimit && (!dttsnew || shifted[fbx_dtp+3] || dups[fbx_dtp+3])) {
      		dtts = m_phiDTDigis.chPhiSegm(wheel,station,sector,ibx_dtp,track_seg);
      		dtts_sh = new L1MuDTChambPhDigi( fbx_dtp , dtts->whNum(), dtts->scNum(), dtts->stNum(),dtts->phi(), dtts->phiB(), dtts->code(), dtts->Ts2Tag(), dtts->BxCnt(),1);
      		l1ttma_outsh.push_back(*dtts_sh);
    			shifted[ibx_dtp+3] = true;
					shift_1 = true;
    		}
				if(dtts && dtts->code()<m_QualityLimit && dttsnew ) dups[ibx_dtp+3] = true;

				if(dtts && dtts->code()<m_QualityLimit && dttsnew && !shift_1 &&track_seg==0 && !m_phiDTDigis.chPhiSegm(wheel,station,sector,fbx_dtp,1)) {
					dtts = m_phiDTDigis.chPhiSegm(wheel,station,sector,ibx_dtp,track_seg);
					if(sign(dtts->phi())!=sign(dttsnew->phi()) && deltaPhi(-(dtts->phi()),dttsnew->phi())>100 ) {

						dtts_sh = new L1MuDTChambPhDigi( fbx_dtp , dtts->whNum(), dtts->scNum(), dtts->stNum(),dtts->phi(), dtts->phiB(), dtts->code(), 1, dtts->BxCnt(),1);
						l1ttma_outsh.push_back(*dtts_sh);
						dups[ibx_dtp+3] = false;
						shifted[ibx_dtp+3] = true;
						secondTs[fbx_dtp+3] = true;
		  		}
				}


			if(dtts && dtts->code()<m_QualityLimit && dttsnew && !shift_1 && track_seg==1 && !m_phiDTDigis.chPhiSegm(wheel,station,sector,fbx_dtp,0)) {
					dtts = m_phiDTDigis.chPhiSegm(wheel,station,sector,ibx_dtp,track_seg);
					if(sign(dtts->phi())!=sign(dttsnew->phi()) && deltaPhi(-(dtts->phi()),dttsnew->phi())>100) {

						dtts_sh = new L1MuDTChambPhDigi( fbx_dtp , dtts->whNum(), dtts->scNum(), dtts->stNum(),dtts->phi(), dtts->phiB(), dtts->code(), 0, dtts->BxCnt(),1);
						l1ttma_outsh.push_back(*dtts_sh);
						dups[ibx_dtp+3] = false;
						shifted[ibx_dtp+3] = true;
						secondTs[fbx_dtp+3] = true;
		  	}
			}
     }
    else if ( (delta_0.size() + delta_p.size()) <= min_index  && delta_m.size()!=0  ) {
        dtts = m_phiDTDigis.chPhiSegm(wheel,station,sector,ibx_dtm,track_seg);
        dttsnew = m_phiDTDigis.chPhiSegm(wheel,station,sector,fbx_dtm,track_seg);

			bool shift_1 = false;
      if(dtts && dtts->code()<m_QualityLimit && (!dttsnew || shifted[fbx_dtm+3] || dups[fbx_dtm+3])) {
      	dtts = m_phiDTDigis.chPhiSegm(wheel,station,sector,ibx_dtm,track_seg);
      	dtts_sh = new L1MuDTChambPhDigi( fbx_dtm , dtts->whNum(), dtts->scNum(), dtts->stNum(),dtts->phi(), dtts->phiB(), dtts->code(), dtts->Ts2Tag(), dtts->BxCnt(),1);
      	l1ttma_outsh.push_back(*dtts_sh);
      	shifted[ibx_dtm+3] = true;
				shift_1 = true;
      }
			if(dtts && dtts->code()<m_QualityLimit && dttsnew  ) dups[ibx_dtm+3] = true;

			if(dtts && dtts->code()<m_QualityLimit && dttsnew && track_seg==0 && !shift_1 && !m_phiDTDigis.chPhiSegm(wheel,station,sector,fbx_dtp,1) ) {
					if(sign(dtts->phi())!=sign(dttsnew->phi())&& deltaPhi(-(dtts->phi()),dttsnew->phi())>100) {
      			dtts = m_phiDTDigis.chPhiSegm(wheel,station,sector,ibx_dtm,track_seg);
						dtts_sh2nd = new L1MuDTChambPhDigi( fbx_dtm , dtts->whNum(), dtts->scNum(), dtts->stNum(),dtts->phi(), dtts->phiB(), dtts->code(), 1, dtts->BxCnt(),1);
						secondTs[fbx_dtm+3] = true;
						dups[ibx_dtm+3] = false;
						shifted[ibx_dtm+3] = true;
					}
      }

			if(dtts && dtts->code()<m_QualityLimit && dttsnew && track_seg==1 && !shift_1 && !m_phiDTDigis.chPhiSegm(wheel,station,sector,fbx_dtp,0) ) {
						if(sign(dtts->phi())!=sign(dttsnew->phi())&& deltaPhi(-(dtts->phi()),dttsnew->phi())>100) {
      				dtts = m_phiDTDigis.chPhiSegm(wheel,station,sector,ibx_dtm,track_seg);
							dtts_sh2nd  = new L1MuDTChambPhDigi( fbx_dtm , dtts->whNum(), dtts->scNum(), dtts->stNum(),dtts->phi(), dtts->phiB(), dtts->code(), 0, dtts->BxCnt(),1);

							secondTs[fbx_dtm+3] = true;
							dups[ibx_dtm+3] = false;
							shifted[ibx_dtm+3] = true;
						}
      }
     }
     shiftedPhiDTDigis.setContainer(l1ttma_outsh);
   }
 }//end of bx

      for(int bx=-3; bx<=3; bx++){
        L1MuDTChambPhDigi * dtts=0;

				if(secondTs[bx+3] ) if(dtts_sh2nd) {l1ttma_out.push_back(*dtts_sh2nd); }

				dtts = shiftedPhiDTDigis.chPhiSegm(wheel,station,sector,bx,track_seg);
        if(dtts){l1ttma_out.push_back(*dtts); continue;}

        if(dups[bx+3]) continue;
        ///if there is no shift then put the original primitive
        dtts = m_phiDTDigis.chPhiSegm(wheel,station,sector,bx,track_seg);
        if(!shifted[bx+3] && dtts) {
           l1ttma_out.push_back(*dtts);
         }
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

int DTRPCBxCorrection::sign(float inv){
	if(inv<0) return -1;
	if(inv>0) return 1;
	return 0;
}

int DTRPCBxCorrection::noRPCHits(L1MuDTChambPhContainer inCon, int bx, int wh, int sec, int st){
  int size = 0;
  const std::vector<L1MuDTChambPhDigi>* vInCon = inCon.getContainer();
  for ( std::vector<L1MuDTChambPhDigi>::const_iterator i  = vInCon->begin();
                     i != vInCon->end();
                     i++ ) {
    if  (bx == i->bxNum() && i->code() != 7 && i->whNum()==wh && i->scNum()==sec && i->stNum()==st) size++;
  }

  return size;
}
