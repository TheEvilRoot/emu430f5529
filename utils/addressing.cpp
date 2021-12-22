#include "addressing.h"

#include <cassert>

SourceAddressing * SourceAddressing::from_source(std::uint16_t reg, std::uint16_t mode, std::uint16_t bw) {
  switch (mode) {
    case 0x00: return new RegisterDirect(reg, bw == 0 ? core::MemoryRefType::WORD : core::MemoryRefType::BYTE);
    case 0x01: return new RegisterIndexed(reg, bw == 0 ? core::MemoryRefType::WORD : core::MemoryRefType::BYTE);
    default: assert(false);
  }
}

SourceAddressing * SourceAddressing::from_destination(std::uint16_t reg, std::uint16_t mode, std::uint16_t bw) {
  switch (mode) {
    default: assert(false);
  }
}
