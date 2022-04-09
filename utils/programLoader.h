//
// Created by user on 9.04.22.
//

#ifndef UNTITLED_PROGRAMLOADER_H
#define UNTITLED_PROGRAMLOADER_H

#include <core/memoryView.h>
#include <core/registerFile.h>

#include <utils/hex430parser.h>

namespace utils {
    struct ProgramLoader {
        core::MemoryView &ram;
        core::RegisterFile &regs;

        ProgramLoader(core::MemoryView &ram, core::RegisterFile &regs) : ram{ram}, regs{regs} {
        }

        void load(const char *buffer, std::size_t count, std::size_t virt_addr) {
            for (std::size_t i = 0; i < count; i++) {
                ram.get_byte(virt_addr + i).set(buffer[i]);
            }
            regs.get_ref(0x0).set(virt_addr);
        }

        void load_from_program(const Program &program) {
            spdlog::info("loading program from {} with {} fragments", program.file_name, program.fragments.size());
            for (const auto &fragment: program.fragments) {
                for (std::uint16_t ptr = 0; std::uint8_t byte: fragment.bytes) {
                    ram.get_byte(fragment.address + ptr).set(byte);
                    ptr++;
                }
            }
            regs.get_ref(0x0).set(0x4400);
        }

        void load_program_from_file(const std::string& program_path) {
            const auto program = Hex430Parser::read_file(program_path);
            load_from_program(program);
        }

        void load_from_buffer(const unsigned char *data, std::size_t count) {
            const std::size_t ram_addr = 0x200;
            const std::size_t ram_size = 0x9FF - 0x200;
            for (std::size_t i = 0; i < count && i < ram_size; i++) {
                ram.get_byte(ram_addr + i).set(data[i]);
            }
            regs.get_ref(0x0).set(0x0200);
        }
    };
}
#endif//UNTITLED_PROGRAMLOADER_H
