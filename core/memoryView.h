//
// Created by Доктор Кларик on 22.12.21.
//

#ifndef UNTITLED_CORE_MEMORYVIEW_H_
#define UNTITLED_CORE_MEMORYVIEW_H_

#include <memory>
#include <vector>
#include <cassert>
#include <array>

#include <core/memoryRef.h>
#include <core/memoryRegion.h>

#include <spdlog/spdlog.h>
#include <spdlog/fmt/bin_to_hex.h>

namespace core {

class MemoryView {
 private:

 protected:
  std::size_t max_ptr_set;
  std::vector<core::MemoryRegion> regions;

 public:
  std::size_t size;
  std::shared_ptr<unsigned char> data;

  MemoryView(std::size_t size);

  void add_region(MemoryRegion region) {
      regions.push_back(std::move(region));
  }

  [[nodiscard]] MemoryRef get_byte(std::size_t virt_ptr) const {
      ensure_access(virt_ptr, MemoryRefType::BYTE);
      return {data, virt_ptr, MemoryRefType::BYTE};
  }

  [[nodiscard]] MemoryRef get_word(std::size_t virt_ptr) const {
    ensure_access(virt_ptr, MemoryRefType::WORD);
    return { data, virt_ptr, MemoryRefType::WORD };
  }

  void ensure_access(std::size_t virt_addr, MemoryRefType ref_type) const {
      for (const auto& reg : regions) {
          if (!reg.can_access(virt_addr, ref_type))
              assert(false);
      }
  }

  void dump(std::size_t, std::size_t) const {
  }

};

}


#endif //UNTITLED_CORE_MEMORYVIEW_H_
