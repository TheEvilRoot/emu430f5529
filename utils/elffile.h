//
// Created by Доктор Кларик on 8.01.22.
//

#ifndef UNTITLED_UTILS_ELFFILE_H_
#define UNTITLED_UTILS_ELFFILE_H_

#include <fstream>

namespace elf {

  template<typename T>
  concept IsElfInt = requires(T t) {
    sizeof(t) == 8 || sizeof(t) == 4;
    std::is_integral_v<T>;
    std::is_unsigned_v<T>;
  };

  template<IsElfInt Int>
  struct ElfFile {

    struct Header {
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

    Header header{};

    explicit ElfFile(std::ifstream& fis) {
      fis.read(reinterpret_cast<char*>(&header), sizeof(header));
    }

  };

  // static assertion for header size with 32-bit and 64-bit header int
  static_assert(sizeof(elf::ElfFile<std::uint32_t>::Header{}) == 52);
  static_assert(sizeof(elf::ElfFile<std::uint64_t>::Header{}) == 64);

  typedef elf::ElfFile<std::uint32_t> ElfFile32;
  typedef elf::ElfFile<std::uint32_t> ElfFile64;

}
#endif //UNTITLED_UTILS_ELFFILE_H_
