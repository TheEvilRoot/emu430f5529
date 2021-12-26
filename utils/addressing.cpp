#include "addressing.h"

#include <cassert>

SourceAddressing * SourceAddressing::from_source(std::uint16_t reg, std::uint16_t mode, std::uint16_t bw) {
  const auto ref_type = bw == 0 ? core::MemoryRefType::WORD : core::MemoryRefType::BYTE;
  switch (mode) {
    case 0x00: return new RegisterDirect(reg, ref_type);
    case 0x01: return new RegisterIndexed(reg, ref_type);
    case 0x02: return new RegisterIndirect(reg, 0x0, ref_type);
    case 0x03: return new RegisterIndirect(reg, ref_type == core::MemoryRefType::BYTE ? 1 : 2, ref_type);
    default: assert(false);
  }
}

SourceAddressing * SourceAddressing::from_destination(std::uint16_t reg, std::uint16_t mode, std::uint16_t bw) {
    const auto ref_type = bw == 0 ? core::MemoryRefType::WORD : core::MemoryRefType::BYTE;
    switch (mode) {
        case 0x0: return new RegisterDirect(reg, ref_type);
        case 0x1: return new RegisterIndexed(reg, ref_type);
        default: assert(false);
    }
}
