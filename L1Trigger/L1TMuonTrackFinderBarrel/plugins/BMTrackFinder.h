//-------------------------------------------------
//
/**  \class BMTrackFinder
 *
 *   L1 BM Track Finder EDProducer
 *
 *
 *
 *   J. Troconiz              UAM Madrid
 */
//
//--------------------------------------------------
#ifndef BMTrackFinder_h
#define BMTrackFinder_h

#include <FWCore/Framework/interface/one/EDProducer.h>
#include <FWCore/ParameterSet/interface/ParameterSet.h>

#include <string>

class L1MuBMTFSetup;


class BMTrackFinder: public edm::one::EDProducer<edm::one::SharedResources> {
 public:
  /// Constructor
  BMTrackFinder(const edm::ParameterSet & pset);

  /// Destructor
  virtual ~BMTrackFinder();

  /// Produce digis out of raw data
  void produce(edm::Event & e, const edm::EventSetup& c);

 private:

  L1MuBMTFSetup* setup1;

};

#endif
