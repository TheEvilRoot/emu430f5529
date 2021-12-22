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

};

}


#endif //UNTITLED_CORE_MEMORYVIEW_H_
