//
// Created by Доктор Кларик on 8.01.22.
//

#ifndef UNTITLED_UTILS_ELFFILE_H_
#define UNTITLED_UTILS_ELFFILE_H_

#include <fstream>

namespace elf {
  template<typename T>
  concept Is32BitInt = requires(T t) {
    sizeof(t) == 4;
    std::is_integral_v<T>;
    std::is_unsigned_v<T>;
  };

  template<typename T>
  concept Is64BitInt = requires(T t) {
    sizeof(t) == 8;
    std::is_integral_v<T>;
    std::is_unsigned_v<T>;
  };

  template<typename T>
  concept IsElfInt = Is32BitInt<T> || Is64BitInt<T>;

  template<IsElfInt Int>
  struct ElfHeader {
    std::uint8_t e_ident[16];
    std::uint16_t e_type;
    std::uint16_t e_machine;
    std::uint32_t e_version;
    Int e_entry;
    Int e_phoff;
    Int e_shoff;
    std::uint32_t e_flags;
    std::uint16_t e_ehsize;
    std::uint16_t e_phentsize;
    std::uint16_t e_phnum;
    std::uint16_t e_shentsize;
    std::uint16_t e_shnum;
    std::uint16_t e_shstrndx;
  };

  template<IsElfInt HInt> struct ElfProgramHeader { };

  template<> struct ElfProgramHeader<std::uint32_t> {
    std::uint32_t p_type;
    std::uint32_t p_offset;
    std::uint32_t p_vaddr;
    std::uint32_t p_paddr;
    std::uint32_t p_filesz;
    std::uint32_t p_memsz;
    std::uint32_t p_flags;
    std::uint32_t p_align;
  };

  template<> struct ElfProgramHeader<std::uint64_t> {
    std::uint32_t p_type;
    std::uint32_t p_offset;
    std::uint32_t p_flags;
    std::uint64_t p_vaddr;
    std::uint64_t p_paddr;
    std::uint64_t p_filesz;
    std::uint64_t p_memsz;
  };

  template<IsElfInt T>
  struct ElfFile {

    typedef ElfHeader<T> Header;
    typedef ElfProgramHeader<T> ProgramHeader;

    std::ifstream& fis;
    Header header;
    std::vector<ProgramHeader> programs;

    explicit ElfFile(std::ifstream& fis): fis{fis} {
      fis.read(reinterpret_cast<char*>(&header), sizeof(header));
      fis.seekg(0);
      fis.seekg(header.e_phoff);
      if (header.e_phoff > 0) {
        programs.resize(header.e_phnum);
        for (std::uint32_t i = 0; i < header.e_phnum; i++) {
          fis.read(reinterpret_cast<char *>(&programs[i]), sizeof(ProgramHeader));
        }
      }
    }

    [[nodiscard]] auto get_program(const std::size_t index) const {
      assert(index >= 0 && index < programs.size());
      const auto& program = programs[index];
      fis.seekg(0);
      fis.seekg(program.p_offset);
      auto buffer = std::make_unique<char[]>(program.p_filesz);
      fis.read(buffer.get(), program.p_filesz);
      return std::move(buffer);
    }

  };

  // static assertion for header size with 32-bit and 64-bit header int
  static_assert(sizeof(elf::ElfFile<std::uint32_t>::Header{}) == 52);
  static_assert(sizeof(elf::ElfFile<std::uint64_t>::Header{}) == 64);

  typedef elf::ElfFile<std::uint32_t> ElfFile32;
  typedef elf::ElfFile<std::uint64_t> ElfFile64;

}
#endif //UNTITLED_UTILS_ELFFILE_H_
