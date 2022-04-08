//
// Created by user on 4.04.22.
//
#include <spdlog/spdlog.h>
#include <gtest/gtest.h>

#include <utils/hex430parser.h>

TEST(Hex430Parser, parseSimpleFile) {
    const auto program = Hex430Parser::read_file("/Volumes/Data/ClionProjects/untitled/msp430/out/simple_1.hex");
    Hex430Parser::write_file(program, "/Volumes/Data/ClionProjects/untitled/msp430/out/simple_1.example.hex");
}

int main(int argc, char* argv[]) {
    spdlog::set_level(spdlog::level::debug);
    spdlog::set_pattern("[%H:%M:%S.%f] [%^%05l%$] %v");
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}