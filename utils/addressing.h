//
// Created by Доктор Кларик on 22.12.21.
//

#ifndef UNTITLED_UTILS_ADDRESSING_H_
#define UNTITLED_UTILS_ADDRESSING_H_

#include <core/memoryRef.h>
#include <core/memoryView.h>
#include <core/registerFile.h>

class SourceAddressing {
 public:
  const core::MemoryRefType ref_type;

  explicit SourceAddressing(core::MemoryRefType ref_type): ref_type{ref_type} { }

  virtual core::MemoryRef get_ref(core::MemoryRef &pc, core::RegisterFile &regs, core::MemoryView &ram) const = 0;

  [[nodiscard]] virtual std::string to_string() const = 0;

  static SourceAddressing * from_source(std::uint16_t reg, std::uint16_t mode, std::uint16_t bw);

  static SourceAddressing * from_destination(std::uint16_t reg, std::uint16_t mode, std::uint16_t bw);

};

class RegisterDirect : public SourceAddressing {
 public:
  std::uint16_t reg;

  RegisterDirect(std::uint16_t reg, core::MemoryRefType ref_type):
   SourceAddressing(ref_type), reg{reg} { }

  core::MemoryRef get_ref(core::MemoryRef &/*pc*/, core::RegisterFile &regs, core::MemoryView &/*ram*/) const override {
    return regs.get_ref(reg);
  }

  [[nodiscard]] std::string to_string() const override {
      return "R" + std::to_string(reg);
  }
};

class RegisterIndexed : public SourceAddressing {
 public:
  std::uint16_t reg;

  RegisterIndexed(std::uint16_t reg, core::MemoryRefType ref_type):
   SourceAddressing(ref_type), reg{reg} { }

  core::MemoryRef get_ref(core::MemoryRef &pc, core::RegisterFile &regs, core::MemoryView &ram) const override {
    const auto next_word = ram.get_word(pc.get_and_increment(0x2)).get();
//    const auto next_word = ram.get_word(pc.get()).get();
    const auto base = regs.get_ref(reg).get();
    switch (ref_type) {
      case core::MemoryRefType::BYTE: return ram.get_byte(base + next_word);
      case core::MemoryRefType::WORD: return ram.get_word(base + next_word);
    }
  }

  [[nodiscard]] std::string to_string() const override {
      return "x(R" + std::to_string(reg) + ")";
  }
};

class RegisterIndirect : public SourceAddressing {
public:
    std::uint16_t reg;
    std::uint16_t delta;

    RegisterIndirect(std::uint16_t reg, std::uint16_t delta, core::MemoryRefType ref_type):
            SourceAddressing(ref_type), reg{reg}, delta{delta} { }

    core::MemoryRef get_ref(core::MemoryRef &/*pc*/, core::RegisterFile &regs, core::MemoryView &ram) const override {
        const auto address = regs.get_ref(reg).get_and_increment(delta);
//        const auto address = regs.get_word(reg).get();
        switch (ref_type) {
            case core::MemoryRefType::BYTE: return ram.get_byte(address);
            case core::MemoryRefType::WORD: return ram.get_word(address);
        }
    }

    [[nodiscard]] std::string to_string() const override {
        if (delta > 0) {
            return "@R" + std::to_string(reg) + "+";
        } else {
            return "@R" + std::to_string(reg);
        }
    }
};

#endif //UNTITLED_UTILS_ADDRESSING_H_
