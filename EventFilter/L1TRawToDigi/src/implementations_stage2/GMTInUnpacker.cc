#include "FWCore/Framework/interface/MakerMacros.h"

#include "EventFilter/L1TRawToDigi/interface/Unpacker.h"

#include "GMTCollections.h"

namespace l1t {
   namespace stage2 {
      class GMTInUnpacker : public Unpacker {
         public:
            virtual bool unpack(const Block& block, UnpackerCollections *coll) override;
      };
   }
}

// Implementation
namespace l1t {
   namespace stage2 {
      bool
      GMTInUnpacker::unpack(const Block& block, UnpackerCollections *coll)
      {
         unsigned int blockId = block.header().getID();
         LogDebug("L1T") << "Block ID  = " << blockId << " size = " << block.header().getSize();

         auto payload = block.payload();

         int nwords = 2; // every link transmits 2 words per event
         int nBX, firstBX, lastBX;
         nBX = int(ceil(block.header().getSize() / nwords));
         getBXRange(nBX, firstBX, lastBX);
         // only use central BX for now
         firstBX = 0;
         lastBX = 0;
         LogDebug("L1T") << "BX override. Set first BX = lastBX = 0.";

         // decide which collection to use according to the link ID
         unsigned int linkId = blockId / 2;
         int processor;
         RegionalMuonCandBxCollection* res;
         tftype trackFinder;
         if (linkId > 47 && linkId < 60) {
            res = static_cast<GMTCollections*>(coll)->getRegionalMuonCandsBMTF();
            trackFinder = tftype::bmtf;
            processor = linkId - 48;
         } else if (linkId > 41 && linkId < 66) {
            res = static_cast<GMTCollections*>(coll)->getRegionalMuonCandsOMTF();
            if (linkId < 48) {
               trackFinder = tftype::omtf_pos;
               processor = linkId - 42;
            } else {
               trackFinder = tftype::omtf_neg;
               processor = linkId - 60;
            }
         } else if (linkId > 35 && linkId < 72) {
            res = static_cast<GMTCollections*>(coll)->getRegionalMuonCandsEMTF();
            if (linkId < 42) {
               trackFinder = tftype::emtf_pos;
               processor = linkId - 36;
            } else {
               trackFinder = tftype::emtf_neg;
               processor = linkId - 66;
            }
         } else {
            edm::LogError("L1T") << "No TF muon expected for link " << linkId;
            return false;
         }
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

               RegionalMuonCand mu = RegionalMuonCand();
                   
               mu.setHwPt((raw_data_00_31 >> 0) & 0x1FF);
               mu.setHwQual((raw_data_00_31 >> 9) & 0xF); 

               // eta is coded as two's complement
               int abs_eta = (raw_data_00_31 >> 13) & 0xFF;
               if ((raw_data_00_31 >> 21) & 0x1) {
                  mu.setHwEta(abs_eta - 256);
               } else {
                  mu.setHwEta(abs_eta);
               }

               mu.setHwPhi((raw_data_00_31 >> 23) & 0xFF);
               // sign is coded as -1^signBit
               int signBit = (raw_data_32_63 >> 0) & 0x1;
               mu.setHwSign(1 - 2*signBit);
               mu.setHwSignValid((raw_data_32_63 >> 1) & 0x1);
               // FIXME: not jet implemented in FW. just a dummy for now
               mu.setHwHF((raw_data_00_31 >> 22) & 0x1);
               // FIXME: just a dummy for now
               mu.setHwTrackAddress((raw_data_32_63 >> 4) & 0x1FFF);
               mu.setTFIdentifiers(processor, trackFinder);
               mu.setDataword(raw_data_32_63, raw_data_00_31);
          
               LogDebug("L1T") << "Mu" << nWord/2 << ": eta " << mu.hwEta() << " phi " << mu.hwPhi() << " pT " << mu.hwPt() << " qual " << mu.hwQual() << " sign " << mu.hwSign() << " sign valid " << mu.hwSignValid();

               res->push_back(bx, mu);
            }
         }
         return true;
      }
   }
}

DEFINE_L1T_UNPACKER(l1t::stage2::GMTInUnpacker);
