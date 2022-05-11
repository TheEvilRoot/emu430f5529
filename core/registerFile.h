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
        core::MemoryView constants;
        core::MemoryView memory;
        std::size_t count;

        core::MemoryRef sr;

        RegisterFile(std::size_t regs_count) : constants{6 * 2}, memory{regs_count * 2}, count{regs_count}, sr{memory.get_word(0x2)} {
            constants.get_word(0x0).set(0x0);
            constants.get_word(0x2).set(0x1);
            constants.get_word(0x4).set(0x2);
            constants.get_word(0x6).set(0x4);
            constants.get_word(0x8).set(0x8);
            constants.get_word(0xA).set(static_cast<std::uint16_t>(-1));
        }

        [[nodiscard]] std::uint16_t status() const {
            return sr.get();
        }

        void status(std::uint16_t value) {
            sr.set(value);
        }

        [[nodiscard]] core::MemoryRef get_ref(std::size_t reg_num) const {
            return memory.get_word(reg_num * 2);
        }

        void dump() const {
            memory.dump(0x0, count * 2);
        }
    };


}// namespace core


#endif//UNTITLED_REGISTERFILE_H
