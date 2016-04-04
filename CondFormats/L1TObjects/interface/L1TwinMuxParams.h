///
/// \class L1TwinMuxParams
///
/// Description: Placeholder for BMTF parameters
///
///
/// \author: Giannis Flouris
///

#ifndef L1TwinMuxParams_h
#define L1TwinMuxParams_h

#include <memory>
#include <iostream>
#include <vector>

#include "CondFormats/Serialization/interface/Serializable.h"

class L1TwinMuxParams {
public:
  L1TwinMuxParams();
  enum { Version = 1 };

  class Node {
  public:
    std::string type_;
    unsigned version_;
    std::vector<double> dparams_;
    std::vector<unsigned> uparams_;
    std::vector<int> iparams_;
    std::vector<std::string> sparams_;
    Node(){ type_="unspecified"; version_=0; }
    COND_SERIALIZABLE;
  };
  enum {
    CONFIG = 0,
    NUM_TM_PARAM_NODES=2
  };
  enum { USERPCBXFORDTBELOWQUALITY,
	 NUM_CONFIG_PARAMS};


  ~L1TwinMuxParams() {}

  // FW version
  unsigned fwVersion() const { return fwVersion_; }
  void setFwVersion(unsigned fwVersion) { fwVersion_ = fwVersion; }

  void set_USERPCBXFORDTBELOWQUALITY(int par1) {pnodes_[CONFIG].iparams_[USERPCBXFORDTBELOWQUALITY] = par1;}
  int  get_USERPCBXFORDTBELOWQUALITY() const{return pnodes_[CONFIG].iparams_[USERPCBXFORDTBELOWQUALITY];}



  // print parameters to stream:
  void print(std::ostream&) const;
  friend std::ostream& operator<<(std::ostream& o, const L1TwinMuxParams & p) { p.print(o); return o; }
 private:
  unsigned version_;
  unsigned fwVersion_;

  std::vector<Node> pnodes_;
  // std::vector here is just so we can use "blob" in DB and evade max size limitations...

  COND_SERIALIZABLE;
};
#endif
