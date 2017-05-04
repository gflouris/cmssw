#ifndef SiStripMonitorSummary_SiStripMonitorCondDataOnDemandExample_h
#define SiStripMonitorSummary_SiStripMonitorCondDataOnDemandExample_h
// -*- C++ -*-
//
// Package:     SiStripMonitorSummary
// Class  :     SiStripMonitorCondDataOnDemandExample
//
// Original Author:  Evelyne Delmeire
//


// system include files
#include <memory>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"

#include <DQMServices/Core/interface/DQMEDAnalyzer.h>

#include <iostream>
#include <string>

class SiStripClassToMonitorCondData;

class SiStripMonitorCondDataOnDemandExample : public DQMEDAnalyzer {

 public:

   explicit SiStripMonitorCondDataOnDemandExample(const edm::ParameterSet&);

   ~SiStripMonitorCondDataOnDemandExample();

   virtual void beginJob() ;
   void dqmBeginRun(edm::Run const& run, edm::EventSetup const& eSetup) override;
   virtual void analyze(const edm::Event&, const edm::EventSetup&);
   virtual void endRun(edm::Run const& run, edm::EventSetup const& eSetup);
   void bookHistograms(DQMStore::IBooker & ibooker , const edm::Run & run, const edm::EventSetup & eSetup);


 private:
  int eventCounter_;
  edm::ParameterSet conf_;
  SiStripClassToMonitorCondData*    condDataMonitoring_ ;

};

#endif
