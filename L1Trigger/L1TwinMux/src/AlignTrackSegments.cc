//-------------------------------------------------
//
//   Class: AlignTrackSegments
//
//   AlignTrackSegments
//
//
//   Author :
//   G. Flouris               U Ioannina    Mar. 2015
//--------------------------------------------------
#include <iostream>
#include <iomanip>
#include <iterator>

#include "L1Trigger/L1TwinMux/interface/AlignTrackSegments.h"

using namespace std;

AlignTrackSegments::AlignTrackSegments(L1MuDTChambPhContainer inphiDigis)  {
  	phiDigis = inphiDigis;
};


void AlignTrackSegments::run( const edm::EventSetup& c) {



  std::vector<L1MuDTChambPhDigi> l1ttma_out;

  L1MuDTChambPhDigi const* ts1=0;
  L1MuDTChambPhDigi const* ts2=0;

  int bx=0, wheel=0, sector=0, station=1;


for(bx=-1; bx<=1; bx++){
for (wheel=-3;wheel<=3; wheel++ ){
	for (sector=0;sector<12; sector++ ){
     for (station=1; station<=4; station++){
        ts1 = phiDigis.chPhiSegm1(wheel,station,sector,bx);
        ts2 = phiDigis.chPhiSegm2(wheel,station,sector,bx);

        if(ts1) {
            L1MuDTChambPhDigi shifted_out( bx , ts1->whNum(), ts1->scNum(), ts1->stNum(),ts1->phi(), ts1->phiB(), ts1->code(), ts1->Ts2Tag(), ts1->BxCnt());
    	    l1ttma_out.push_back(shifted_out);

        }
        if(!ts1 && ts2) {
        	//std::cout<<bx<<"   "<<ts2->bxNum()<<"   "<<ts2->Ts2Tag()<<"   "<<ts2->BxCnt()<<std::endl;
           L1MuDTChambPhDigi shifted_out( ts2->bxNum() , ts2->whNum(), ts2->scNum(), ts2->stNum(),ts2->phi(), ts2->phiB(), ts2->code(), ts2->Ts2Tag(), ts2->BxCnt());
    	   l1ttma_out.push_back(shifted_out);
        }	
    	
    	if(ts1 && ts2) {
	      // 	std::cout<<bx<<"   "<<ts1->bxNum()<<"   "<<ts1->Ts2Tag()<<"   "<<ts1->BxCnt()<<std::endl;
          //	std::cout<<bx<<"   "<<ts2->bxNum()<<"   "<<ts2->Ts2Tag()<<"   "<<ts2->BxCnt()<<std::endl;
           L1MuDTChambPhDigi shifted_out( ts2->bxNum() - ts2->Ts2Tag() , ts2->whNum(), ts2->scNum(), ts2->stNum(),ts2->phi(), ts2->phiB(), ts2->code(), ts2->Ts2Tag(), ts2->BxCnt());
    	   l1ttma_out.push_back(shifted_out);

        }	

        //if(ts1 &&ts2) std::cout<<std::endl;


     }}}}

_dt_tsshifted.setContainer(l1ttma_out);

}

