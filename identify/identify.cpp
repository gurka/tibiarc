#include <algorithm>
#include <filesystem>
#include <iostream>
#include <memory>
#include <unistd.h>

#include "gamestate.hpp"
#include "memoryfile.hpp"
#include "recordings.hpp"
#include "recordings.hpp"
#include "versions.hpp"

namespace {
    bool verbose = false;
}

std::vector<std::filesystem::path> getClientDirs(const std::filesystem::path& clientsDir) {
    std::vector<std::filesystem::path> clientDirs;

    if (!std::filesystem::is_directory(clientsDir)) {
        std::cerr << "error: " << clientsDir << " is not a directory\n";
        return clientDirs;
    }

    for (const auto& dirEntry : std::filesystem::directory_iterator(clientsDir)) {
        if (!dirEntry.is_directory()) {
            continue;
        }

        // Check if it's named like x.yz
        const auto dirName = dirEntry.path().filename().string();
        if (dirName.size() == 4 &&
                std::isdigit(dirName.at(0)) &&
                dirName.at(1) == '.' &&
                std::isdigit(dirName.at(2)) &&
                std::isdigit(dirName.at(3))) {

            // Check if it contains Tibia.dat, Tibia.spr and Tibia.pic
            if (!std::filesystem::is_regular_file(dirEntry / std::filesystem::path("Tibia.dat"))) {
                std::cerr << "warning: Tibia.dat is missing in " << dirEntry << "\n";
                continue;
            }
            if (!std::filesystem::is_regular_file(dirEntry / std::filesystem::path("Tibia.spr"))) {
                std::cerr << "warning: Tibia.spr is missing in " << dirEntry << "\n";
                continue;
            }
            if (!std::filesystem::is_regular_file(dirEntry / std::filesystem::path("Tibia.pic"))) {
                std::cerr << "warning: Tibia.pic is missing in " << dirEntry << "\n";
                continue;
            }

            if (verbose) {
                std::cout << "Found client dir " << dirEntry << "\n";
            }
            clientDirs.push_back(dirEntry);
        }
    }

    return clientDirs;
}

int main(int argc, char **argv) {
    for (int c; (c = getopt(argc, argv, "v")) != -1; ) {
        switch (c) {
            case 'v':
                verbose = true;
                break;
            case '?':
                std::cerr << "usage: " << argv[0] << " [-v] CLIENTS_DIR RECORDING_FILE\n";
                return 1;
            default:
                abort();
        }
    }

    if (argc - optind != 2) {
        std::cerr << "usage: " << argv[0] << " [-v] CLIENTS_DIR RECORDING_FILE\n";
        return 1;
    }

    const std::string inputPath(argv[argc - 1]);
    const trc::MemoryFile file(inputPath);

    // Guess format
    const auto format = trc::Recordings::GuessFormat(inputPath, file.Reader());
    if (format == trc::Recordings::Format::Unknown) {
        std::cerr << "error: unknown recording format\n";
        return 1;
    }

    auto clientDirs = getClientDirs(argv[argc - 2]);
    if (clientDirs.empty()) {
        std::cerr << "error: no valid client dirs found\n";
        return 1;
    }

    // Sort clientDirs, newest version first
    std::sort(clientDirs.begin(), clientDirs.end(), [](const std::filesystem::path& a, const std::filesystem::path& b) {
        const auto aStr = a.string();
        const auto bStr = b.string();
        const auto aVersion = ((aStr.at(aStr.size() - 4) - '0') * 100) +
                ((aStr.at(aStr.size() - 2) - '0') * 10) +
                (aStr.at(aStr.size() - 1) - '0');
        const auto bVersion = ((bStr.at(bStr.size() - 4) - '0') * 100) +
                ((bStr.at(bStr.size() - 2) - '0') * 10) +
                (bStr.at(bStr.size() - 1) - '0');
        return aVersion > bVersion;
    });

    for (const auto& clientDir : clientDirs) {
        const trc::MemoryFile pic((clientDir / std::filesystem::path("Tibia.pic")));
        const trc::MemoryFile spr((clientDir / std::filesystem::path("Tibia.spr")));
        const trc::MemoryFile dat((clientDir / std::filesystem::path("Tibia.dat")));

        const auto clientDirStr = clientDir.string();
        int major = clientDirStr.at(clientDirStr.size() - 4) - '0';
        int minor = (clientDirStr.at(clientDirStr.size() - 2) - '0') * 10 + clientDirStr.at(clientDirStr.size() - 1) - '0';
        int preview = 0;

        if (verbose) {
            std::cout << "Checking version " << major << "." << minor << "\n";
        }
        bool ok = true;
        try {
            const auto version = std::make_unique<trc::Version>(major, minor, preview, pic.Reader(), spr.Reader(), dat.Reader());
            const auto recording = trc::Recordings::Read(format, file.Reader(), *version);
            const auto gamestate = std::make_unique<trc::Gamestate>(*version);

            for (const auto& frame : recording->Frames) {
                for (const auto& event : frame.Events) {
                    event->Update(*gamestate);
                }
            }
        } catch (trc::InvalidDataError& error) {
            ok = false;
        }

        if (ok) {
            std::cout << major << "." << minor;
            return 0;
        }
    }

    // No valid version found
    return 1;
}
