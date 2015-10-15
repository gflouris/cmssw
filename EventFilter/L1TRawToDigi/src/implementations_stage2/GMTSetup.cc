#include "FWCore/Framework/interface/stream/EDProducerBase.h"
#include "FWCore/Utilities/interface/InputTag.h"

#include "EventFilter/L1TRawToDigi/interface/Packer.h"
#include "EventFilter/L1TRawToDigi/interface/Unpacker.h"

#include "EventFilter/L1TRawToDigi/interface/PackingSetup.h"

#include "GMTCollections.h"
#include "GMTTokens.h"

namespace l1t {
   namespace stage2 {
      class GMTSetup : public PackingSetup {
         public:
            virtual std::unique_ptr<PackerTokens> registerConsumes(const edm::ParameterSet& cfg, edm::ConsumesCollector& cc) override {
               return std::unique_ptr<PackerTokens>(new GMTTokens(cfg, cc));
            };

            virtual void fillDescription(edm::ParameterSetDescription& desc) override {};

            virtual PackerMap getPackers(int fed, unsigned int fw) override {
               PackerMap res;

               //res[{1, 1}] = {
               //   PackerFactory::get()->make("stage2::GMTPacker"),
               //};

               return res;
            };

            virtual void registerProducts(edm::stream::EDProducerBase& prod) override {
               prod.produces<RegionalMuonCandBxCollection>("BMTF");
               prod.produces<RegionalMuonCandBxCollection>("OMTF");
               prod.produces<RegionalMuonCandBxCollection>("EMTF");
               prod.produces<MuonBxCollection>();
            };

            virtual std::unique_ptr<UnpackerCollections> getCollections(edm::Event& e) override {
               return std::unique_ptr<UnpackerCollections>(new GMTCollections(e));
            };

            virtual UnpackerMap getUnpackers(int fed, int board, int amc, unsigned int fw) override {
               UnpackerMap res;

               auto gmt_in_unp = UnpackerFactory::get()->make("stage2::GMTInUnpacker");
               auto gmt_out_unp = UnpackerFactory::get()->make("stage2::GMTOutUnpacker");

               for (int iLink = 72; iLink < 144; iLink += 2)
                   res[iLink] = gmt_in_unp;
               for (int oLink = 1; oLink < 9; oLink += 2)
                   res[oLink] = gmt_out_unp;

               return res;
            };
      };
   }
}

DEFINE_L1T_PACKING_SETUP(l1t::stage2::GMTSetup);
