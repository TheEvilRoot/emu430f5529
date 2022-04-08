//
// Created by user on 4.04.22.
//

#ifndef UNTITLED_PROGRAM_H
#define UNTITLED_PROGRAM_H

#include <string>
#include <vector>

struct Fragment {
    std::uint16_t address;
    std::vector<std::uint8_t> bytes;
};

struct Program {
    std::string file_name;
    std::vector<Fragment> fragments;
};

#endif//UNTITLED_PROGRAM_H
