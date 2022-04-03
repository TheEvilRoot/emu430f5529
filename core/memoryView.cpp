//
// Created by Доктор Кларик on 22.12.21.
//

#include "memoryView.h"

using namespace core;

MemoryView::MemoryView(std::size_t size): size{size}, data{std::shared_ptr<unsigned char>(new unsigned char[size])}, max_ptr_set{0} { }