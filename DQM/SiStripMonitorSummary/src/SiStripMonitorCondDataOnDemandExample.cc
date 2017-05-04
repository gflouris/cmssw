// -*- C++ -*-
//
// Package:     SiStripMonitorSummary
// Class  :     SiStripMonitorCondDataOnDemandExample
//
// Original Author:  Evelyne Delmeire
//

#include "DQM/SiStripMonitorSummary/interface/SiStripMonitorCondDataOnDemandExample.h"
#include "DQM/SiStripMonitorSummary/interface/SiStripClassToMonitorCondData.h"


//
// ----- Constructor
//
SiStripMonitorCondDataOnDemandExample::SiStripMonitorCondDataOnDemandExample(edm::ParameterSet const& iConfig):conf_(iConfig){}
// -----



//
// ----- Destructor
//
SiStripMonitorCondDataOnDemandExample::~SiStripMonitorCondDataOnDemandExample(){}
// -----




//
// ----- beginRun
//
void SiStripMonitorCondDataOnDemandExample::dqmBeginRun(edm::Run const& run, edm::EventSetup const& eSetup) {
  eventCounter_=0;
  condDataMonitoring_ = new SiStripClassToMonitorCondData(conf_);
  condDataMonitoring_->beginRun(eSetup);



} // beginRun
// -----

void SiStripMonitorCondDataOnDemandExample::bookHistograms(DQMStore::IBooker & ibooker , const edm::Run & run, const edm::EventSetup & eSetup){
  condDataMonitoring_->bookHistos(ibooker);
}



//
// ----- beginJob
//
void SiStripMonitorCondDataOnDemandExample::beginJob(void){} //beginJob



//
// ----- Analyze
//
void SiStripMonitorCondDataOnDemandExample::analyze(edm::Event const& iEvent, edm::EventSetup const& eSetup){
  //eventCounter_++;

  // on demand type I : eventSetup and detId to be passed
  // output : ME's corresponding to that detId


  //if(eventCounter_==1){ condDataMonitoring_ -> getModMEsOnDemand(eSetup,369125542);}

  // on demand type II : eventSetup, subdetector-type(TIB/TOB/TEC/TID),
  //                                 subdetector-side for TEC/TID (0 for TIB and TOB)
  //                                 layer_number (0=all layers)

  //if(eventCounter_==2){ condDataMonitoring_ -> getLayerMEsOnDemand(eSetup,"TEC",0,1);}
  //condDataMonitoring_ -> getModMEsOnDemand(eSetup,369125542);
  condDataMonitoring_ -> getLayerMEsOnDemand(eSetup,"TEC",2,4);



} // analyze
// -----



//
// ----- endRun
//
void SiStripMonitorCondDataOnDemandExample::endRun(edm::Run const& run, edm::EventSetup const& eSetup) {

  condDataMonitoring_->endRun(eSetup);

} // endRun
// -----



#include "FWCore/Framework/interface/MakerMacros.h"
  DEFINE_FWK_MODULE(SiStripMonitorCondDataOnDemandExample);
