//
// Created by Доктор Кларик on 31.01.22.
//

#ifndef UNTITLED_UTILS_HEX430PARSER_H_
#define UNTITLED_UTILS_HEX430PARSER_H_

#include <fstream>

struct Hex430Parser {

  struct Program {
    std::size_t address;
    std::vector<std::uint8_t> bytes{};
    bool is_finished{false};

    explicit Program(std::size_t addr): address{addr} { }
  };

  static auto parse_file(const std::string& fileName) {
    std::ifstream ifs(fileName);
    std::string buffer;
    std::vector<Program> programs;
    do {
      std::getline(ifs, buffer);
      if (buffer.empty()) break;
      if (buffer.front() == 'q') break;
      if (programs.empty() || programs.back().is_finished) {
        if (buffer.front() != '@') throw std::logic_error{"Expected @address"};
        const auto addr_string = buffer.substr(1);
        const auto addr_value = std::stoul(addr_string, 0 , 16);
        programs.emplace_back(addr_value);
      } else {
        buffer.find(' ')
      }
    } while (!buffer.empty());
  }

};

#endif //UNTITLED_UTILS_HEX430PARSER_H_
