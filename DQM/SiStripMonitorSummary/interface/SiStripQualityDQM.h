#ifndef SiStripMonitorSummary_SiStripQualityDQM_h
#define SiStripMonitorSummary_SiStripQualityDQM_h


#include "DQM/SiStripMonitorSummary/interface/SiStripBaseCondObjDQM.h"
#include "CalibFormats/SiStripObjects/interface/SiStripQuality.h"
#include "CalibTracker/Records/interface/SiStripQualityRcd.h"



class SiStripQualityDQM : public SiStripBaseCondObjDQM{

  public:

  SiStripQualityDQM(const edm::EventSetup & eSetup,
                      edm::ParameterSet const& hPSet,
                      edm::ParameterSet const& fPSet);

  virtual ~SiStripQualityDQM();

  void bookHistos(DQMStore::IBooker & ibooker);

  void getActiveDetIds(const edm::EventSetup & eSetup);

  void fillModMEs(const std::vector<uint32_t> & selectedDetIds, const edm::EventSetup& es);
  void fillMEsForDet(const ModMEs& selModME_,uint32_t selDetId_, const TrackerTopology* tTopo);

  void fillSummaryMEs(const std::vector<uint32_t> & selectedDetIds, const edm::EventSetup& es);
  void fillMEsForLayer( /*std::map<uint32_t, ModMEs> selModMEsMap_, */ uint32_t selDetId_, const TrackerTopology* tTopo);
  void fillGrandSummaryMEs(const edm::EventSetup& eSetup);


  unsigned long long getCache(const edm::EventSetup & eSetup){ return eSetup.get<SiStripQualityRcd>().cacheIdentifier();}

  void getConditionObject(const edm::EventSetup & eSetup){
    eSetup.get<SiStripQualityRcd>().get(qualityLabel_,qualityHandle_);
    cacheID_memory = cacheID_current;
  }

  private:
    std::string qualityLabel_ ;
    edm::ESHandle<SiStripQuality> qualityHandle_;
    int NTkBadComponent[4]; //k: 0=BadModule, 1=BadFiber, 2=BadApv, 3=BadStrips
    int NBadComponent[4][19][4];
    std::stringstream ssV[4][19];
    void SetBadComponents(int i, int component,SiStripQuality::BadComponent& BC);

    std::vector<uint32_t> alreadyFilledLayers;
};

#endif
