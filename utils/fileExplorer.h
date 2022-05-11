//
// Created by user on 10.04.22.
//

#ifndef UNTITLED_FILEEXPLORER_H
#define UNTITLED_FILEEXPLORER_H

#include <filesystem>
#include <string>
#include <utility>
#include <vector>

namespace utils {
    struct File {
        std::filesystem::path path;
        std::string display_name;
        bool is_directory;
        bool is_back;
        bool is_selectable;

        File(std::filesystem::path path, std::string display_name, bool is_directory, bool is_back, bool is_selectable) : path{std::move(path)}, display_name{std::move(display_name)}, is_directory{is_directory}, is_back{is_back}, is_selectable{is_selectable} {
        }
    };

    struct FileExplorer {
        constexpr static std::size_t bufferSize = 2048;

        struct State {
            std::filesystem::path current{};
            std::vector<File> files{};
        };

        std::deque<State> state{};
        char buffer[bufferSize]{};


        FileExplorer() {
            set_current(std::filesystem::current_path());
        }

        static bool is_file_selectable(const std::filesystem::path &path) {
            if (path.has_extension()) {
                return path.extension() == ".hex";
            }
            return false;
        }

        static auto get_files(const std::filesystem::path &path) {
            std::vector<File> files{};
            if (path.has_parent_path()) {
                files.emplace_back(File{path.parent_path(), "..", true, true, true});
            }
            auto iterator = std::filesystem::directory_iterator(path);
            constexpr auto get_file = [](const std::filesystem::path &p, std::vector<File> &files) {
                const auto filename = p.filename().empty() ? p.string() : p.filename().string();
                if (is_directory(p)) {
                    files.emplace_back(p, filename + "/", true, false, true);
                } else {
                    files.emplace_back(p, filename, false, false, is_file_selectable(p));
                }
            };
            for (const auto &file: iterator) {
                get_file(file, files);
            }
            return files;
        }


        void set_current(const std::string &value) {
            const auto path = std::filesystem::path(value);
            std::memcpy(buffer, value.c_str(), std::min(bufferSize, value.length()));
            buffer[std::min(value.length(), bufferSize - 1)] = 0;
            state.push_back(State{path, get_files(path)});
            if (state.size() > 2) {
                state.pop_front();
            }
        }

        std::optional<File> select(const File &file) {
            if (file.is_directory) {
                set_current(file.path.string());
                return std::optional<File>{};
            } else if (file.is_selectable) {
                return file;
            }
            return std::optional<File>{};
        }
    };
}

#endif//UNTITLED_FILEEXPLORER_H
