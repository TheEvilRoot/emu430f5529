//
// Created by Доктор Кларик on 22.12.21.
//

#include "memoryRef.h"

#include <utility>

#include <utils/utils.h>

using namespace core;

MemoryRef::MemoryRef(std::shared_ptr<unsigned char> base, std::size_t offset, MemoryRefType type):
 base{std::move(base)}, offset{offset}, type{type} { }

std::uint16_t MemoryRef::get() const {
  switch (type) {
    case MemoryRefType::BYTE: return static_cast<std::uint16_t>(base.get()[offset]);
    case MemoryRefType::WORD: return endian::msp<decltype(base.get())>::word_at(base.get(), offset);
  }
}

void MemoryRef::set(std::uint16_t val) {
  switch (type) {
    case MemoryRefType::BYTE:
      base.get()[offset] = static_cast<char>(val & 0xFF);
      return;
    case MemoryRefType::WORD:
      endian::msp<decltype(base.get())>::word_to(base.get(), offset, val);
      return;
  }
}

std::uint16_t MemoryRef::get_and_increment(std::uint16_t delta) {
  const auto value = get();
  if (delta > 0)
    set(value + delta);
  return value;
}

std::uint16_t MemoryRef::operator*() const {
    return get();
}
