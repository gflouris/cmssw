// -*- C++ -*-
//
// Package:     SiStripMonitorSummary
// Class  :     SiStripMonitorCondData
//
// Original Author:  Evelyne Delmeire
//



#include "DQM/SiStripMonitorSummary/interface/SiStripMonitorCondData.h"

#include "DQM/SiStripMonitorSummary/interface/SiStripPedestalsDQM.h"
#include "DQM/SiStripMonitorSummary/interface/SiStripNoisesDQM.h"
#include "DQM/SiStripMonitorSummary/interface/SiStripThresholdDQM.h"
#include "DQM/SiStripMonitorSummary/interface/SiStripQualityDQM.h"
#include "DQM/SiStripMonitorSummary/interface/SiStripApvGainsDQM.h"
#include "DQM/SiStripMonitorSummary/interface/SiStripLorentzAngleDQM.h"
#include "DQM/SiStripMonitorSummary/interface/SiStripBackPlaneCorrectionDQM.h"
#include "DQM/SiStripMonitorSummary/interface/SiStripCablingDQM.h"


//
// ----- Constructor
//
SiStripMonitorCondData::SiStripMonitorCondData(edm::ParameterSet const& iConfig):conf_(iConfig){

  monitorPedestals_      = iConfig.getParameter<bool>("MonitorSiStripPedestal");
  monitorNoises_         = iConfig.getParameter<bool>("MonitorSiStripNoise");
  monitorLowThreshold_   = iConfig.getParameter<bool>("MonitorSiStripLowThreshold");
  monitorHighThreshold_  = iConfig.getParameter<bool>("MonitorSiStripHighThreshold");
  monitorQuality_        = iConfig.getParameter<bool>("MonitorSiStripQuality");
  monitorApvGains_       = iConfig.getParameter<bool>("MonitorSiStripApvGain");
  monitorLorentzAngle_   = iConfig.getParameter<bool>("MonitorSiStripLorentzAngle");
  monitorBackPlaneCorrection_   = iConfig.getParameter<bool>("MonitorSiStripBackPlaneCorrection");
  monitorCabling_        = iConfig.getParameter<bool>("MonitorSiStripCabling");

}
// -----



//
// ----- Destructor
//
SiStripMonitorCondData::~SiStripMonitorCondData(){

  if(monitorPedestals_)   { delete pedestalsDQM_   ;}
  if(monitorNoises_)      { delete noisesDQM_      ;}
  if(monitorLowThreshold_) { delete lowthresholdDQM_ ;}
  if(monitorHighThreshold_){ delete highthresholdDQM_;}
  if(monitorQuality_)     { delete qualityDQM_     ;}
  if(monitorApvGains_)    { delete apvgainsDQM_    ;}
  if(monitorLorentzAngle_){ delete lorentzangleDQM_;}
  if(monitorBackPlaneCorrection_){ delete bpcorrectionDQM_;}
  if(monitorCabling_)     { delete cablingDQM_;}

}
// -----



//
// ----- beginRun
//
void SiStripMonitorCondData::dqmBeginRun(edm::Run const& run, edm::EventSetup const& eSetup) {

  if(monitorPedestals_){
    pedestalsDQM_ = new SiStripPedestalsDQM(eSetup,
                                           conf_.getParameter<edm::ParameterSet>("SiStripPedestalsDQM_PSet"),
                                           conf_.getParameter<edm::ParameterSet>("FillConditions_PSet"));
  }


  if(monitorNoises_){
    noisesDQM_ = new SiStripNoisesDQM(eSetup,
                                     conf_.getParameter<edm::ParameterSet>("SiStripNoisesDQM_PSet"),
                                     conf_.getParameter<edm::ParameterSet>("FillConditions_PSet"));
  }


  if(monitorLowThreshold_){
    lowthresholdDQM_ = new SiStripThresholdDQM(eSetup,
                                           conf_.getParameter<edm::ParameterSet>("SiStripLowThresholdDQM_PSet"),
                                           conf_.getParameter<edm::ParameterSet>("FillConditions_PSet"));
  }

  if(monitorHighThreshold_){
    highthresholdDQM_ = new SiStripThresholdDQM(eSetup,
                                           conf_.getParameter<edm::ParameterSet>("SiStripHighThresholdDQM_PSet"),
                                           conf_.getParameter<edm::ParameterSet>("FillConditions_PSet"));
  }

  if(monitorQuality_){
    qualityDQM_ = new SiStripQualityDQM(eSetup,
                                       conf_.getParameter<edm::ParameterSet>("SiStripQualityDQM_PSet"),
                                       conf_.getParameter<edm::ParameterSet>("FillConditions_PSet"));
  }


  if(monitorApvGains_){
    apvgainsDQM_ = new SiStripApvGainsDQM(eSetup,
                                         conf_.getParameter<edm::ParameterSet>("SiStripApvGainsDQM_PSet"),
                                         conf_.getParameter<edm::ParameterSet>("FillConditions_PSet"));
  }


  if(monitorLorentzAngle_){
    lorentzangleDQM_ = new SiStripLorentzAngleDQM(eSetup,
                                                 conf_.getParameter<edm::ParameterSet>("SiStripLorentzAngleDQM_PSet"),
                                                 conf_.getParameter<edm::ParameterSet>("FillConditions_PSet"));
  }

  if(monitorBackPlaneCorrection_){
    bpcorrectionDQM_ = new SiStripBackPlaneCorrectionDQM(eSetup,
                                                 conf_.getParameter<edm::ParameterSet>("SiStripBackPlaneCorrectionDQM_PSet"),
                                                 conf_.getParameter<edm::ParameterSet>("FillConditions_PSet"));
  }
  if(monitorCabling_){
    cablingDQM_ = new SiStripCablingDQM(eSetup,
					conf_.getParameter<edm::ParameterSet>("SiStripCablingDQM_PSet"),
					conf_.getParameter<edm::ParameterSet>("FillConditions_PSet"));
  }
} // beginRun
// -----



//
// ----- beginJob
//
void SiStripMonitorCondData::beginJob(void){} //beginJob
// -----



//
// ----- Analyze
//
void SiStripMonitorCondData::analyze(edm::Event const& iEvent, edm::EventSetup const& eSetup){

  if(monitorPedestals_)      { pedestalsDQM_     ->analysis(eSetup);}
  if(monitorNoises_)         { noisesDQM_        ->analysis(eSetup);}
  if(monitorLowThreshold_)   { lowthresholdDQM_  ->analysis(eSetup);}
  if(monitorHighThreshold_)  { highthresholdDQM_ ->analysis(eSetup);}
  if(monitorApvGains_)       { apvgainsDQM_      ->analysis(eSetup);}
  if(monitorLorentzAngle_)   { lorentzangleDQM_  ->analysis(eSetup);}
  if(monitorBackPlaneCorrection_)   { bpcorrectionDQM_  ->analysis(eSetup);}
  if(monitorQuality_)        { qualityDQM_->analysis(eSetup);qualityDQM_->fillGrandSummaryMEs(eSetup);}//fillGrand. for SiStripquality
  if(monitorCabling_)        { cablingDQM_       ->analysis(eSetup);}
} // analyze
// -----

void SiStripMonitorCondData::bookHistograms(DQMStore::IBooker & ibooker , const edm::Run & run, const edm::EventSetup & eSetup){

  if(monitorPedestals_)      { pedestalsDQM_     ->bookHistos(ibooker);}
  if(monitorNoises_)         { noisesDQM_        ->bookHistos(ibooker);}
  if(monitorLowThreshold_)   { lowthresholdDQM_  ->bookHistos(ibooker);}
  if(monitorHighThreshold_)  { highthresholdDQM_ ->bookHistos(ibooker);}
  if(monitorApvGains_)       { apvgainsDQM_->bookHistos(ibooker);}
  if(monitorLorentzAngle_)   { lorentzangleDQM_  ->bookHistos(ibooker);}
  if(monitorBackPlaneCorrection_)   { bpcorrectionDQM_  ->bookHistos(ibooker);}
  if(monitorQuality_)        { qualityDQM_->bookHistos(ibooker);}
  if(monitorCabling_)        { cablingDQM_       ->bookHistos(ibooker);}


}


//
// ----- endRun
//
void SiStripMonitorCondData::endRun(edm::Run const& run, edm::EventSetup const& eSetup) {
  if(monitorPedestals_)      { pedestalsDQM_     ->end();}
  if(monitorNoises_)         { noisesDQM_        ->end();}
  if(monitorLowThreshold_)   { lowthresholdDQM_  ->end();}
  if(monitorHighThreshold_)  { highthresholdDQM_ ->end();}
  if(monitorApvGains_)       { apvgainsDQM_      ->end();}
  if(monitorLorentzAngle_)   { lorentzangleDQM_  ->end();}
  if(monitorBackPlaneCorrection_)   { bpcorrectionDQM_  ->end();}
  if(monitorQuality_)        { qualityDQM_       ->end();}
  if(monitorCabling_)        { cablingDQM_       ->end();}

  bool outputMEsInRootFile     = conf_.getParameter<bool>("OutputMEsInRootFile");
  std::string outputFileName  = conf_.getParameter<std::string>("OutputFileName");

  DQMStore* dqmStore_=edm::Service<DQMStore>().operator->();

  if (outputMEsInRootFile) {
    dqmStore_->showDirStructure();
    dqmStore_->save(outputFileName);
  }


} // endRun
// -----



#include "FWCore/Framework/interface/MakerMacros.h"
  DEFINE_FWK_MODULE(SiStripMonitorCondData);
