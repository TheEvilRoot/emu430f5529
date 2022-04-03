//
// Created by theevilroot on 3/20/22.
//

#ifndef UNTITLED_EMUGUI_H
#define UNTITLED_EMUGUI_H

#include <imgui.h>

#include <cstdint>

namespace emugui {
    template<typename Backend>
    struct EmuGui {
        Backend backend{};

        explicit EmuGui() = default;

        void run() {
            backend.initiate("Emulator");
        }

        bool render(std::uint16_t pc) {
            if (backend.isTerminated()) {
                backend.terminate();
                return false;
            }
            backend.renderPrepare();
            ImGui::Begin("Hello");
            ImGui::Text("PC = %d", pc);
            ImGui::End();
            backend.renderFinalize();
            return true;
        }
    };
}

#endif //UNTITLED_EMUGUI_H
