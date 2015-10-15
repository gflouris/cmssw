#include "FWCore/Framework/interface/MakerMacros.h"

#include "EventFilter/L1TRawToDigi/interface/Unpacker.h"

#include "GMTCollections.h"

namespace l1t {
   namespace stage2 {
      class GMTOutUnpacker : public Unpacker {
         public:
            virtual bool unpack(const Block& block, UnpackerCollections *coll) override;
      };
   }
}

// Implementation
namespace l1t {
   namespace stage2 {
      bool
      GMTOutUnpacker::unpack(const Block& block, UnpackerCollections *coll)
      {
         LogDebug("L1T") << "Block ID  = " << block.header().getID() << " size = " << block.header().getSize();

         auto payload = block.payload();

         int nwords = 2; // every link transmits 2 words per event
         int nBX, firstBX, lastBX;
         nBX = int(ceil(block.header().getSize() / nwords));
         getBXRange(nBX, firstBX, lastBX);
         // only use central BX for now
         firstBX = 0;
         lastBX = 0;
         LogDebug("L1T") << "BX override. Set first BX = lastBX = 0.";

         auto res = static_cast<GMTCollections*>(coll)->getMuons();
         res->setBXRange(firstBX, lastBX);

         LogDebug("L1T") << "nBX = " << nBX << " first BX = " << firstBX << " lastBX = " << lastBX;

         // Initialise index
         int unsigned i = 0;

         // Loop over multiple BX and then number of muons filling muon collection
         for (int bx = firstBX; bx <= lastBX; ++bx) {
            for (unsigned nWord = 0; nWord < block.header().getSize(); nWord += 2) {
               uint32_t raw_data_00_31 = payload[i++];
               uint32_t raw_data_32_63 = payload[i++];        
               LogDebug("L1T") << "raw_data_00_31 = 0x" << hex << raw_data_00_31 << " raw_data_32_63 = 0x" << raw_data_32_63;
               // skip empty muons (all 64 bits 0)
               if (raw_data_00_31 == 0 && raw_data_32_63 == 0) {
                  LogDebug("L1T") << "Raw data is zero. Skip.";
                  continue;
               }

               Muon mu = Muon();
                   
               mu.setHwPt((raw_data_00_31 >> 10) & 0x1FF);
               mu.setHwQual((raw_data_00_31 >> 19) & 0xF); 

               // eta is coded as two's complement
               int abs_eta = (raw_data_00_31 >> 23) & 0xFF;
               if ((raw_data_00_31 >> 31) & 0x1) {
                  mu.setHwEta(abs_eta - 256);
               } else {
                  mu.setHwEta(abs_eta);
               }

               mu.setHwPhi((raw_data_00_31 >> 0) & 0x3FF);
               mu.setHwIso((raw_data_32_63 >> 0) & 0x3); 
               // charge is coded as -1^chargeBit
               int chargeBit = (raw_data_32_63 >> 2) & 0x1;
               mu.setHwCharge(1 - 2*chargeBit);
               mu.setHwChargeValid((raw_data_32_63 >> 3) & 0x1);
          
               LogDebug("L1T") << "Mu" << nWord/2 << ": eta " << mu.hwEta() << " phi " << mu.hwPhi() << " pT " << mu.hwPt() << " iso " << mu.hwIso() << " qual " << mu.hwQual() << " charge " << mu.hwCharge() << " charge valid " << mu.hwChargeValid();

               res->push_back(bx, mu);
            }
         }
         return true;
      }
   }
}

DEFINE_L1T_UNPACKER(l1t::stage2::GMTOutUnpacker);
