//
// Created by Доктор Кларик on 22.12.21.
//

#ifndef UNTITLED_CORE_MEMORYREF_H_
#define UNTITLED_CORE_MEMORYREF_H_

#include <cassert>
#include <memory>

#include <core/statusRegister.h>

namespace core {
enum class MemoryRefType {
  BYTE, WORD
};

class MemoryRef {
 private:
  std::shared_ptr<unsigned char> base;
  std::size_t offset;
  MemoryRefType type;

  MemoryRef& operator++() {
      const auto delta = type == MemoryRefType::WORD ? 2 : 1;
      offset += delta;
      return *this;
  }

  MemoryRef& operator--() {
      const auto delta = type == MemoryRefType::WORD ? 2 : 1;
      assert(ssize_t(offset) - ssize_t(delta) >= 0);
      offset -= delta;
      return *this;
  }

  bool operator!=(const MemoryRef &ref) const {
      return ref.base.get() != base.get();
  }

 public:
  MemoryRef(std::shared_ptr<unsigned char> base, std::size_t offset, MemoryRefType type);

  [[nodiscard]] std::uint16_t operator*() const;

  [[nodiscard]] std::uint16_t get() const;

  void set(std::uint16_t val);

  std::uint16_t get_and_increment(std::uint16_t delta);

  friend class MemoryView;

};
}

#endif //UNTITLED_CORE_MEMORYREF_H_
