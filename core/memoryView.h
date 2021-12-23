//
// Created by Доктор Кларик on 22.12.21.
//

#ifndef UNTITLED_CORE_MEMORYVIEW_H_
#define UNTITLED_CORE_MEMORYVIEW_H_

#include <memory>

#include <core/memoryRef.h>

namespace core {

class MemoryView {
 protected:
  std::size_t size;
  std::shared_ptr<unsigned char> data;
  std::size_t max_ptr_set;

 public:
  MemoryView(std::size_t size);

  [[nodiscard]] MemoryRef get_byte(std::size_t virt_ptr) const {
   return {data, virt_ptr, MemoryRefType::BYTE};
  }

  [[nodiscard]] MemoryRef get_word(std::size_t virt_ptr) const {
    return { data, virt_ptr, MemoryRefType::WORD };
  }

  void dump(std::size_t ptr, std::size_t count) const {
      for (auto i = ptr; i < ptr + count && i < size; i++) {
          fprintf(stderr, "%02x ", data.get()[i]);
          if (i > 0 && i % 0x10 == 0) fprintf(stderr, "\n");
      }
      fprintf(stderr, "\n");
  }

};

}


#endif //UNTITLED_CORE_MEMORYVIEW_H_
