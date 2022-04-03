//
// Created by theevilroot on 27.12.2021.
//

#ifndef UNTITLED_REGISTERFILE_H
#define UNTITLED_REGISTERFILE_H

#include <core/memoryView.h>
#include <core/statusRegister.h>

namespace core {

    class RegisterFile {
    public:
        core::MemoryView memory;
        std::size_t count;

        RegisterFile(std::size_t regs_count) : memory{regs_count * 2}, count{regs_count} {}

        [[nodiscard]] core::MemoryRef get_ref(std::size_t reg_num) const {
            return memory.get_word(reg_num * 2);
        }

        void dump() const {
            memory.dump(0x0, count * 2);
        }
    };


}// namespace core


#endif//UNTITLED_REGISTERFILE_H
