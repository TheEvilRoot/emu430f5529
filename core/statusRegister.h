//
// Created by theevilroot on 3/23/22.
//

#ifndef UNTITLED_STATUSREGISTER_H
#define UNTITLED_STATUSREGISTER_H

#include <cstdint>

struct StatusRegister {
    std::uint16_t : 7;
    std::uint16_t v : 1;
    std::uint16_t scg1 : 1;
    std::uint16_t scg0 : 1;
    std::uint16_t osc_off : 1;
    std::uint16_t cpu_off : 1;
    std::uint16_t gie: 1;
    std::uint16_t n : 1;
    std::uint16_t z : 1;
    std::uint16_t c : 1;
};


#endif //UNTITLED_STATUSREGISTER_H
