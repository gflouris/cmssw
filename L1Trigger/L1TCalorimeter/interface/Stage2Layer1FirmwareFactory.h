///
/// \class l1t::CaloStage1FirmwareFactory
///
/// Implementation:
/// Demonstrates how to define the firmware interface.
///
/// \author: R. Alex Barbieri MIT
///


#ifndef Stage2Layer1FirmwareFactory_h
#define Stage2Layer1FirmwareFactory_h

#include <boost/shared_ptr.hpp>

#include "L1Trigger/L1TCalorimeter/interface/Stage2PreProcessor.h"

#include "CondFormats/L1TObjects/interface/FirmwareVersion.h"
#include "CondFormats/L1TObjects/interface/CaloParams.h"


namespace l1t {

  class Stage2Layer1FirmwareFactory {
  public:
    typedef boost::shared_ptr<Stage2PreProcessor> ReturnType;

    ReturnType create(const FirmwareVersion & fwv, CaloParams* params);

    // (Why not make "create" a static member function? You could...
    // But this way allows you to add additional customizations to the
    // factory not necessarily coming from the DB.)
  };

} // namespace

#endif
