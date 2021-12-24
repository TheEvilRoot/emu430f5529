#define LITTLE

#include <iostream>
#include <utility>

#include <emulator/emulator.h>

#include <unistd.h>
#include <fcntl.h>

struct Program {
    const std::string file_name;
    const std::unique_ptr<unsigned char[]> data;
    const std::size_t size;

    Program(std::string  file_name, std::unique_ptr<unsigned char[]> data, std::size_t size):
     file_name{std::move(file_name)}, data{std::move(data)}, size{size} { }

    explicit Program(std::string  file_name): file_name{std::move(file_name)}, data{nullptr}, size{0} { }
};

auto load_file(const std::string& file_name) {
    int fd = open(file_name.c_str(), O_RDONLY);
    if (fd < 0)
        return Program(file_name);
    auto buffer = std::make_unique<unsigned char[]>(0x10000);
    std::size_t count = 0;
    while (true) {
        const auto c = read(fd, buffer.get() + count, 0x100);
        if (c > 0) count += c;
        if (c < 0x100) break;
    }
    return Program(file_name, std::move(buffer), count);
}

int main() {
  emu::Emulator emulator;
  Program prog = load_file("../out/shellcode");
  std::cout << "Program " << prog.file_name << " size:" << prog.size << "\n";
  emulator.load_from_buffer(prog.data.get(), prog.size);
  emulator.run();
}
