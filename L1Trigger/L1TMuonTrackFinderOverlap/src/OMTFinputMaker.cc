#include <cmath>
#include <vector>
#include <iostream>

#include "DataFormats/MuonDetId/interface/CSCDetId.h"
#include "DataFormats/MuonDetId/interface/RPCDetId.h"
#include "DataFormats/MuonDetId/interface/DTChamberId.h"
#include "DataFormats/MuonDetId/interface/MuonSubdetId.h"

#include "L1Trigger/L1TMuonTrackFinderOverlap/interface/OMTFinputMaker.h"
#include "L1Trigger/L1TMuonTrackFinderOverlap/interface/OMTFinput.h"
#include "L1Trigger/L1TMuonTrackFinderOverlap/interface/OMTFConfiguration.h"

#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"

///////////////////////////////////////
///////////////////////////////////////
OMTFinputMaker::OMTFinputMaker(){ 

  myInput = new OMTFinput();

  geom.reset(new L1TMuon::GeometryTranslator());

}
///////////////////////////////////////
///////////////////////////////////////
void OMTFinputMaker::initialize(const edm::EventSetup& es){ 

  geom->checkAndUpdateGeometry(es);

}
///////////////////////////////////////
///////////////////////////////////////
OMTFinputMaker::~OMTFinputMaker(){ 

  if(myInput) delete myInput;

}
///////////////////////////////////////
///////////////////////////////////////
bool  OMTFinputMaker::acceptDigi(uint32_t rawId,
				 unsigned int iProcessor,
				 l1t::tftype type){

  unsigned int aMin = OMTFConfiguration::barrelMin[iProcessor];
  unsigned int aMax = OMTFConfiguration::barrelMax[iProcessor];
  unsigned int aSector = 99;

  ///Clean up digis. Remove unconnected detectors
  DetId detId(rawId);
  if (detId.det() != DetId::Muon) 
    edm::LogError("Critical OMTFinputMaker") << "PROBLEM: hit in unknown Det, detID: "<<detId.det()<<std::endl;
  switch (detId.subdetId()) {
  case MuonSubdetId::RPC: {
    RPCDetId aId(rawId);
    
    ///Select RPC chambers connected to OMTF
    if(type==l1t::tftype::omtf_pos &&
       (aId.region()<0 ||
       (aId.region()==0 && aId.ring()!=2) ||
       (aId.region()==0 && aId.station()==4) ||
       (aId.region()==0 && aId.station()==2 && aId.layer()==2 && aId.roll()==1) ||
       (aId.region()==0 && aId.station()==3 && aId.roll()==1) ||
       (aId.region()==1 && aId.station()==4) ||
	///RPC RE1/2 temporarily not used (aId.region()==1 && aId.station()==1 && aId.ring()<2) ||
       (aId.region()==1 && aId.station()>0 && aId.ring()<3))
       ) return false;

    if(type==l1t::tftype::omtf_neg &&
        (aId.region()>0 ||
	(aId.region()==0 && aId.ring()!=-2) ||
	(aId.region()==0 && aId.station()==4) ||
	(aId.region()==0 && aId.station()==2 && aId.layer()==2 && aId.roll()==1) ||
	(aId.region()==0 && aId.station()==3 && aId.roll()==1) ||
	(aId.region()==-1 && aId.station()==4) ||
	 //RPC RE1/2 temporarily not used (aId.region()==1 && aId.station()==1 && aId.ring()<2) ||
	(aId.region()==-1 && aId.station()>0 && aId.ring()<3))
       ) return false;
    
    if(type==l1t::tftype::bmtf && aId.region()!=0) return false;
    
    if(type==l1t::tftype::emtf_pos &&
        (aId.region()<=0 ||
	(aId.station()==1 && aId.ring()==3))) return false;
    if(type==l1t::tftype::emtf_neg &&
        (aId.region()>=0 ||
	(aId.station()==1 && aId.ring()==3))) return false;
    ////////////////
    if(aId.region()==0) aSector = aId.sector();
    if(aId.region()!=0){
      aSector = (aId.sector()-1)*6+aId.subsector();
      aMin = OMTFConfiguration::endcap10DegMin[iProcessor];
      aMax = OMTFConfiguration::endcap10DegMax[iProcessor];
    }

    break;
  }
  case MuonSubdetId::DT: {
    DTChamberId dt(rawId);

    if(type==l1t::tftype::omtf_pos && dt.wheel()!=2) return false;
    if(type==l1t::tftype::omtf_neg && dt.wheel()!=-2) return false;
    if(type==l1t::tftype::emtf_pos || type==l1t::tftype::emtf_neg) return false;
    
    aSector =  dt.sector();   	
    break;
  }
  case MuonSubdetId::CSC: {
    CSCDetId csc(rawId);
    
    if(type==l1t::tftype::omtf_pos &&
       (csc.endcap()==2 || csc.ring()==1 || csc.station()==4)) return false;
    if(type==l1t::tftype::omtf_neg &&
       (csc.endcap()==1 || csc.ring()==1 || csc.station()==4)) return false;

    if(type==l1t::tftype::emtf_pos &&
       (csc.endcap()==2 || (csc.station()==1 && csc.ring()==3))
       ) return false;
    if(type==l1t::tftype::emtf_neg &&
       (csc.endcap()==1 || (csc.station()==1 && csc.ring()==3))
       ) return false;

    aSector =  csc.chamber();   	
    
    aMin = OMTFConfiguration::endcap10DegMin[iProcessor];
    aMax = OMTFConfiguration::endcap10DegMax[iProcessor];

    if( (type==l1t::tftype::emtf_pos || type==l1t::tftype::emtf_neg) &&
	csc.station()>1 && csc.ring()==1){
      aMin = OMTFConfiguration::endcap20DegMin[iProcessor];
      aMax = OMTFConfiguration::endcap20DegMax[iProcessor];
    }    
    break;
  }
  }
  
  if(aMax>aMin && aSector>=aMin && aSector<=aMax) return true;
  if(aMax<aMin && (aSector>=aMin || aSector<=aMax)) return true;

  return false;
}
///////////////////////////////////////
///////////////////////////////////////
bool OMTFinputMaker::filterDigiQuality(const L1TMuon::TriggerPrimitive & aDigi) const{

  switch (aDigi.subsystem()) {
  case L1TMuon::TriggerPrimitive::kDT: {
    if (aDigi.getDTData().bx!= 0 || aDigi.getDTData().BxCntCode!= 0 || aDigi.getDTData().Ts2TagCode!= 0 || aDigi.getDTData().qualityCode<4) return false;  
    break;
  }
  case L1TMuon::TriggerPrimitive::kCSC: { return fabs(aDigi.getCMSGlobalEta())<1.23;} //Filter CSCdigis outsice OMTF eta coverage
  case L1TMuon::TriggerPrimitive::kRPC: {}
  case L1TMuon::TriggerPrimitive::kNSubsystems: {if(aDigi.getBX()!=0) return false; }
  }    
  return true;
}
///////////////////////////////////////
///////////////////////////////////////
unsigned int OMTFinputMaker::getInputNumber(unsigned int rawId, 
					    unsigned int iProcessor,
					    l1t::tftype type){

  unsigned int iInput = 99;
  unsigned int aSector = 99;
  int aMin = OMTFConfiguration::barrelMin[iProcessor];
  int iRoll = 1;
  int nInputsPerSector = 2;

  DetId detId(rawId);
  if (detId.det() != DetId::Muon) 
    edm::LogError("Critical OMTFinputMaker") << "PROBLEM: hit in unknown Det, detID: "<<detId.det()<<std::endl;
  switch (detId.subdetId()) {
  case MuonSubdetId::RPC: {
    RPCDetId rpc(rawId);
    if(rpc.region()==0){
      nInputsPerSector = 4;
      aSector = rpc.sector();
      ///on the 0-2pi border we need to add 1 30 deg sector
      ///to get the correct index
      if(iProcessor==5 && aSector<3) aMin = 0;
      //Use division into rolls
      iRoll = rpc.roll();
      ///Set roll number by hand to keep common input 
      ///number shift formula for all stations
      if(rpc.station()==2 && rpc.layer()==2 && rpc.roll()==2) iRoll = 1;
      if(rpc.station()==3) iRoll = 1;

      ///At the moment do not use RPC chambers splitting into rolls for bmtf part      
      if(type==l1t::tftype::bmtf)iRoll = 1;
    }
    if(rpc.region()!=0){
      aSector = (rpc.sector()-1)*6+rpc.subsector();
      aMin = OMTFConfiguration::endcap10DegMin[iProcessor];
      ///on the 0-2pi border we need to add 4 10 deg sectors
      ///to get the correct index
      if(iProcessor==5 && aSector<5) aMin = -3;
    }
    break;
  }
  case MuonSubdetId::DT: {
    DTChamberId dt(rawId);
    aSector = dt.sector();
    ///on the 0-2pi border we need to add 1 30 deg sector
    ///to get the correct index
    if(iProcessor==5 && aSector<3) aMin = 0;
    break;
  }
  case MuonSubdetId::CSC: {   
    CSCDetId csc(rawId);    
    aSector = csc.chamber();
    aMin = OMTFConfiguration::endcap10DegMin[iProcessor];       
    ///on the 0-2pi border we need to add 4 10deg sectors
    ///to get the correct index
    if(iProcessor==5 && aSector<5) aMin = -3;
    ///Endcap region covers alsgo 10 deg sectors
    ///on the 0-2pi border we need to add 2 20deg sectors
    ///to get the correct index
    if( (type==l1t::tftype::emtf_pos || type==l1t::tftype::emtf_neg) &&
	csc.station()>1 && csc.ring()==1){
      aMin = OMTFConfiguration::endcap20DegMin[iProcessor];
      if(iProcessor==5 && aSector<3) aMin = -1;
    }
    break;
  }
  }

  ///Assume 2 hits per chamber
  iInput = (aSector - aMin)*nInputsPerSector;
  ///Chambers divided into two rolls have rolls number 1 and 3
  iInput+=iRoll-1;

  /////TEST
  //if(type==l1t::tftype::emtf_pos || type==l1t::tftype::emtf_neg) iInput = 0;
  /////////

  
  return iInput;
}
////////////////////////////////////////////
///Helper function for sorting the RPC primitives by strip number
bool rpcPrimitiveCmp(const L1TMuon::TriggerPrimitive *a,
		     const L1TMuon::TriggerPrimitive *b) { return a->getStrip()<b->getStrip(); };
////////////////////////////////////////////
const OMTFinput * OMTFinputMaker::buildInputForProcessor(const L1TMuon::TriggerPrimitiveCollection & vDigi,
							 unsigned int iProcessor,
							 l1t::tftype type){
  myInput->clear();	
  std::ostringstream myStr;

  std::map<unsigned int, std::vector<const L1TMuon::TriggerPrimitive *> > detMap;
  
  ///Prepare inpout for individual processors.
  unsigned int nGlobalPhi = OMTFConfiguration::nPhiBins;
  for (const auto &digiIt:vDigi) { 

    ///Check it the data fits into given processor input range
    if(!acceptDigi(digiIt.rawId(), iProcessor, type)) continue;
    if(!filterDigiQuality(digiIt)) continue;

    digiIt.print(myStr);

    unsigned int hwNumber = OMTFConfiguration::getLayerNumber(digiIt.rawId());

    if(OMTFConfiguration::hwToLogicLayer.find(hwNumber)==OMTFConfiguration::hwToLogicLayer.end()) continue;
    unsigned int iLayer = OMTFConfiguration::hwToLogicLayer[hwNumber];   
    int iPhi =  digiIt.getCMSGlobalPhi()/(2.0*M_PI)*nGlobalPhi;    
    int iEta =  digiIt.getCMSGlobalEta()/2.61*240;
    unsigned int iInput= getInputNumber(digiIt.rawId(), iProcessor, type);

    if(digiIt.subsystem()!=L1TMuon::TriggerPrimitive::kRPC) myInput->addLayerHit(iLayer,iInput,iPhi,iEta);

    switch (digiIt.subsystem()) {
    case L1TMuon::TriggerPrimitive::kDT: {
      myInput->addLayerHit(iLayer+1,iInput,digiIt.getDTData().bendingAngle,iEta);
      break;
    }
    case L1TMuon::TriggerPrimitive::kCSC: {
      //myInput->addLayerHit(iLayer+1,iInput,digiIt.getCSCData().pattern,iEta);
      ////
      break;
    }
    case L1TMuon::TriggerPrimitive::kRPC: {
      if(detMap.find(digiIt.rawId())==detMap.end()) detMap[digiIt.rawId()] = std::vector<const L1TMuon::TriggerPrimitive *>(0);
      detMap[digiIt.rawId()].push_back(&(digiIt));
      break;
    }
    case L1TMuon::TriggerPrimitive::kNSubsystems: {}
    };    
  }

  ///Decluster hits in each RPC detId
  typedef std::tuple<unsigned int,const L1TMuon::TriggerPrimitive *, const L1TMuon::TriggerPrimitive *> halfDigi;
  std::vector<halfDigi> result;

  for(auto detIt:detMap) {   
    std::sort(detIt.second.begin(), detIt.second.end(),rpcPrimitiveCmp);    
    for(auto stripIt: detIt.second) {
      if (result.empty() || std::get<0>(result.back()) != detIt.first) result.push_back(halfDigi(detIt.first,stripIt,stripIt));
      else if (stripIt->getStrip() - std::get<2>(result.back())->getStrip() == 1) std::get<2>(result.back()) = stripIt;
      else if (stripIt->getStrip() - std::get<2>(result.back())->getStrip() > 1) result.push_back(halfDigi(detIt.first,stripIt,stripIt));
    }
  }
  for(auto halfDigiIt:result){
    RPCDetId detid(std::get<0>(halfDigiIt));
    L1TMuon::TriggerPrimitive aRpcPrimitiveBeg(detid,std::get<1>(halfDigiIt)->getStrip(), 0, 0);
    L1TMuon::TriggerPrimitive aRpcPrimitiveEnd(detid,std::get<2>(halfDigiIt)->getStrip(), 0, 0);
    
    ///Decluster PRC digis. Consecutive set of fires strips has a ccordinate
    ///equal to begin+end, which is the mean expresses in half RPC strips granularity.
    float phi1 = geom->calculateGlobalPhi(aRpcPrimitiveBeg);
    float phi2 = geom->calculateGlobalPhi(aRpcPrimitiveEnd);
    float phi = (phi1+phi2)/2.0;
    ///If phi1 is close to Pi, and phi2 close to -Pi the results phi is 0
    ///instead -pi
    if(phi1*phi2<0 && fabs(phi1)>M_PI/2.0) phi = (M_PI-phi)*(1 - 2*std::signbit(phi));
    int iPhi =  phi/(2.0*M_PI)*nGlobalPhi;
    int iEta =  std::get<1>(halfDigiIt)->getCMSGlobalEta()/2.61*240;
    unsigned int hwNumber = OMTFConfiguration::getLayerNumber(std::get<0>(halfDigiIt));
    unsigned int iLayer = OMTFConfiguration::hwToLogicLayer[hwNumber];
    unsigned int iInput= getInputNumber(std::get<0>(halfDigiIt), iProcessor, type);
    
    myInput->addLayerHit(iLayer,iInput,iPhi,iEta);
    myStr<<detid
	 <<"halfDigi: "<<std::get<1>(halfDigiIt)->getStrip()<<" "
	 <<std::get<2>(halfDigiIt)->getStrip()
         <<" phi: "<<floor(phi * OMTFConfiguration::nPhiBins/(2*M_PI))
	 <<" hwNumber: "<<hwNumber
	 <<" iInput: "<<iInput
	 <<" iLayer: "<<iLayer
	 <<std::endl;
  }

  edm::LogInfo("OMTFInputMaker")<<myStr.str();
  
  return myInput;
}
////////////////////////////////////////////
////////////////////////////////////////////

