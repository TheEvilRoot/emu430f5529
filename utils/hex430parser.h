//
// Created by Доктор Кларик on 31.01.22.
//

#ifndef UNTITLED_UTILS_HEX430PARSER_H_
#define UNTITLED_UTILS_HEX430PARSER_H_

#include <spdlog/spdlog.h>

#include <fstream>
#include <stdexcept>

#include <utils/utils.h>
#include <utils/program.h>

struct Hex430Parser {

    struct Address {
        std::uint16_t value;
    };

    struct Bytes {
        std::vector<std::uint8_t> bytes;
    };

    struct Quit { };

    typedef std::variant<Quit, Bytes, Address> ParserState;

    struct Parser {
        std::vector<Fragment> fragments{};
        std::optional<Fragment> current{};

        void finalize() {
            if (current) {
                fragments.push_back(current.value());
            }
        }

        void new_fragment(std::uint16_t address) {
            finalize();
            current = Fragment{address, {}};
        }

        void push_bytes(const std::vector<std::uint8_t>& bytes) {
            if (current) {
                auto& value = current.value();
                std::for_each(std::begin(bytes), std::end(bytes), [&value](auto c) {
                    value.bytes.push_back(c);
                });
            }
        }
    };

    static auto is_whitespace(int c) {
        return c == ' ' || c == '\n';
    }

    static ParserState read_stream_part(std::istream& ifs) {
        int buff = ifs.get();
        if (buff < 0) {
            return Quit{};
        }
        if (buff == '@') {
            std::string addr;
            while (!is_whitespace(buff = ifs.get())) {
                addr += char(buff);
            }
            return Address{std::uint16_t(std::stoi(addr, nullptr, 16))};
        }
        if (buff == 'q') {
            return Quit{};
        }
        std::vector<std::uint8_t> bytes;
        std::vector<std::uint8_t> byte_buffer;
        std::string byte;
        byte += char(buff);
        while ((buff = ifs.get()) != '\n') {
            if (buff == '\n') break;
            if (buff == ' ' || buff == '\r') continue;
            byte += char(buff);
            if (byte.size() == 2) {
                byte_buffer.emplace_back(std::stoi(byte, nullptr, 16));
                byte = "";
            }
            if (byte_buffer.size() == 2) {
                bytes.push_back(byte_buffer[1]);
                bytes.push_back(byte_buffer[0]);
                byte_buffer.clear();
//                bytes.push_back(((byte_buffer[0] & 0xF) << 4) | ((byte_buffer[0] & 0xF0) >> 4));
//                bytes.push_back(((byte_buffer[1] & 0xF) << 4) | ((byte_buffer[1] & 0xF0) >> 4));
            }
        }
        if (!byte_buffer.empty()) {
            bytes.push_back(byte_buffer[0]);
        }
        return Bytes{bytes};
    }

    static auto read_stream(std::istream& ifs) {
        Parser parser;
        while (!ifs.eof()) {
            const auto has_next = std::visit(overloaded{
               [](const Quit&) { return false; },
               [&parser](const Address& a) {
                 parser.new_fragment(a.value);
                 return true;
               },
               [&parser](const Bytes& b) {
                 parser.push_bytes(b.bytes);
                 return true;
               }
            }, read_stream_part(ifs));
            if (!has_next)
                break;
        }
        parser.finalize();
        return parser.fragments;
    }

    static auto read_file(const std::string& file_path) {
        std::ifstream ifs(file_path);
        Program program{file_path, read_stream(ifs)};
        return program;
    }

    static auto write_stream(const Program& program, std::ostream& ofs) {
        for (const auto &fragment : program.fragments) {
            char buffer[6]{};
            sprintf(buffer, "@%04x", fragment.address);
            ofs << buffer << "\n";
            std::size_t i = 0;
            const std::size_t bytes_size = fragment.bytes.size();
            constexpr static std::size_t line_size = 16;
            for (std::size_t s = 0; std::uint8_t byte : fragment.bytes) {
                if (i > 0 && i % line_size == 0)
                    ofs << "\n";
                sprintf(buffer, "%02X", byte);
                ofs << std::string{buffer, 2};
                if (s < bytes_size - 1)
                    ofs << " ";
                i++;
                s++;
            }
            if (i % line_size > 0) ofs << "\n";
        }
        ofs << "q\n";
    }

    static auto write_file(const Program& program, const std::string& file_path) {
        std::ofstream ofs(file_path);
        write_stream(program, ofs);
        ofs.close();
    }

};

#endif//UNTITLED_UTILS_HEX430PARSER_H_
