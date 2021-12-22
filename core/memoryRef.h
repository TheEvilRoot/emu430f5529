//
// Created by Доктор Кларик on 22.12.21.
//

#ifndef UNTITLED_CORE_MEMORYREF_H_
#define UNTITLED_CORE_MEMORYREF_H_

#include <memory>

namespace core {
enum class MemoryRefType {
  BYTE, WORD
};

class MemoryRef {
 private:
  std::shared_ptr<unsigned char> base;
  std::size_t offset;
  MemoryRefType type;

 public:
  MemoryRef(std::shared_ptr<unsigned char> base, std::size_t offset, MemoryRefType type);

  [[nodiscard]] std::uint16_t get() const;

  void set(std::uint16_t val);

  std::uint16_t get_and_increment(std::uint16_t delta);

};
}

#endif //UNTITLED_CORE_MEMORYREF_H_
