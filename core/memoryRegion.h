//
// Created by theevilroot on 24.12.2021.
//

#ifndef UNTITLED_MEMORYREGION_H
#define UNTITLED_MEMORYREGION_H

#include <core/memoryRef.h>

#include <utility>
#include <string>

namespace core {

enum class MemoryRegionAccess { ONLY_WORD, ONLY_BYTE, WORD_BYTE };

struct MemoryRegion {
    std::string name;
    std::size_t start_addr;
    std::size_t end_addr;
    MemoryRegionAccess access_mode;

    MemoryRegion(std::string name, std::size_t start, std::size_t end, MemoryRegionAccess mode):
     name{std::move(name)}, start_addr{start}, end_addr{end}, access_mode{mode} { }

    MemoryRegion(std::string name, MemoryRegionAccess mode, std::size_t start, std::size_t length):
     name{std::move(name)}, start_addr{start}, end_addr{std::size_t(start + length)}, access_mode{mode} { }

    [[nodiscard]] bool can_access(std::size_t virt_addr, core::MemoryRefType ref_type) const {
        if (virt_addr >= start_addr && virt_addr <= end_addr) {
            return MemoryRegion::is_accessible(access_mode, ref_type);
        }
        return true;
    }

    static bool is_accessible(MemoryRegionAccess mode, core::MemoryRefType ref_type) {
        switch (mode) {
            case MemoryRegionAccess::WORD_BYTE: return true;
            case MemoryRegionAccess::ONLY_BYTE: return ref_type == MemoryRefType::BYTE;
            case MemoryRegionAccess::ONLY_WORD: return ref_type == MemoryRefType::WORD;
        }
    }
};

}

#endif //UNTITLED_MEMORYREGION_H
