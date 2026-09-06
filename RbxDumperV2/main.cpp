#include "globals.h"

static MODULEENTRY32 getModule(const char* moduleName, DWORD processId) {
    const HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, processId);
    MODULEENTRY32 moduleEntry{};
    if (snapshot != INVALID_HANDLE_VALUE) {
        moduleEntry.dwSize = sizeof(moduleEntry);
        if (Module32First(snapshot, &moduleEntry)) {
            do {
                if (!strcmp(moduleEntry.szModule, moduleName)) {
                    CloseHandle(snapshot);
                    return moduleEntry;
                }
            } while (Module32Next(snapshot, &moduleEntry));
        }
        CloseHandle(snapshot);
    }
    return {};
}

static string getModulePathByName(DWORD processId, const string& moduleName) {
    const HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, processId);
    if (snapshot == INVALID_HANDLE_VALUE) return "";

    MODULEENTRY32 moduleEntry;
    moduleEntry.dwSize = sizeof(MODULEENTRY32);

    if (Module32First(snapshot, &moduleEntry)) {
        do {
            if (_stricmp(moduleEntry.szModule, moduleName.c_str()) == 0) {
                string path(moduleEntry.szExePath);
                CloseHandle(snapshot);
                return path;
            }
        } while (Module32Next(snapshot, &moduleEntry));
    }

    CloseHandle(snapshot);
    return "";
}

static void init(const std::string& path) {
	Logger::Log(Logger::Level::Info, "Initializing...");

    const auto hwnd = FindWindowA(nullptr, "Roblox");
    DWORD processId;
    GetWindowThreadProcessId(hwnd, &processId);

    if (Globals::UpdateResolution()) {
        Logger::Log(Logger::Level::Info, "Detected Roblox window resolution: " + to_string(Globals::windowWidth) + "x" + to_string(Globals::windowHeight));
    } else {
        Logger::Log(Logger::Level::Warning, "Could not detect Roblox window resolution, using default: " + to_string(Globals::windowWidth) + "x" + to_string(Globals::windowHeight));
    }

    if (Globals::UpdateMaxFps()) {
        Logger::Log(Logger::Level::Info, "Detected Roblox max FPS: " + to_string(static_cast<int>(Globals::maxFps)));
    } else {
        Logger::Log(Logger::Level::Warning, "Could not detect Roblox max FPS, using default: " + to_string(static_cast<int>(Globals::maxFps)));
    }

    auto module = getModule("RobloxPlayerBeta.exe", processId);
    Globals::baseAddress = reinterpret_cast<uintptr_t>(module.modBaseAddr);
    if (!Globals::baseAddress) {
        MessageBoxA(0, "OPEN THE FUCKING GAME",
            "Error", MB_ICONERROR | MB_OK);
        exit(20);
    }

    Globals::robloxPath = getModulePathByName(processId, "RobloxPlayerBeta.exe");
    if (const size_t pos = Globals::robloxPath.find("version-"); pos == string::npos) {
        Globals::clientVersion = "unknown";
        Logger::Log(Logger::Level::Warning, "Could not parse client path : (");
    }
    else {
        const size_t end = Globals::robloxPath.find_first_of("\\/", pos);
        Globals::clientVersion = Globals::robloxPath.substr(pos, end - pos);
    }

    Logger::Log(Logger::Level::Info, "Client version: " + Globals::clientVersion);
    Logger::Log(Logger::Level::Info, "Loading roblox from: " + path);
    PatternScanning::Init(path);
    Mem::Initialize(processId);
    Logger::Log(Logger::Level::Success, "Init success!");
}

static void dumpAllOffsets() {
    Dump::StaticDump();
	Dump::General();
    Dump::DataModel();
	Dump::Workspace();
    Dump::Players();
    Dump::Camera();
    Dump::Parts();
    Dump::Mouse();
    Dump::UI();
    Dump::Lighting();
    Dump::Interactables();
	Dump::Animation();
    Dump::Scripts();

    Dump::FFlags();
}

int main(const int argc, char* argv[])
{
    SetProcessDPIAware();
    const auto startTime = chrono::high_resolution_clock::now();
    const HANDLE processHandle = GetCurrentProcess();
    SetPriorityClass(processHandle, REALTIME_PRIORITY_CLASS);
    system(("title RbxDumper (" + Globals::dumperVersion + ")").c_str());

    if (!DumperConstants::load()) {
        system("pause");
        return -1;
    }

    std::string robloxExePath = DumperConstants::dumpedRobloxPath;
    if (argc > 1) {
        robloxExePath = argv[1];
    }

    if (robloxExePath.empty()) {
        Logger::Log(Logger::Level::Error, "dumpedRobloxPath is missing in config.json!");
        system("pause");
        return -1;
    }

    init(robloxExePath);
    dumpAllOffsets();

	Logger::Log(Logger::Level::Info, "Writing offsets to files...");

    const string basePath = Globals::clientVersion + "/";
    fs::create_directories(Globals::clientVersion);

	Globals::offsets.dumpToFile(basePath + "Offsets.txt", RbxDumper::FileType::TXT);
    Globals::offsets.dumpToFile(basePath + "Offsets.hpp", RbxDumper::FileType::HPP);
    Globals::offsets.dumpToFile(basePath + "Offsets.cs", RbxDumper::FileType::CS);
    Globals::offsets.dumpToFile(basePath + "Offsets.json", RbxDumper::FileType::JSON, false);
    Globals::offsets.dumpToFile(basePath + "OffsetsHex.json", RbxDumper::FileType::JSON);

    Globals::offsets.dumpToFile(basePath + "Types.json", RbxDumper::FileType::TYPESJSON);
    Globals::offsets.dumpToFile(basePath + "Struct.hpp", RbxDumper::FileType::STRUCTHPP);

    Globals::fflagOffsets.dumpToFile(basePath + "FFlags.txt", RbxDumper::FileType::TXT);
    Globals::fflagOffsets.dumpToFile(basePath + "FFlags.hpp", RbxDumper::FileType::HPP);
    Globals::fflagOffsets.dumpToFile(basePath + "FFlags.cs", RbxDumper::FileType::CS);
    Globals::fflagOffsets.dumpToFile(basePath + "FFlags.json", RbxDumper::FileType::JSON, false);
    Globals::fflagOffsets.dumpToFile(basePath + "FFlagsHex.json", RbxDumper::FileType::JSON);

    const auto endTime = chrono::high_resolution_clock::now();
    const chrono::duration<double> duration = endTime - startTime;
    const string message = "Dumped " + to_string(Globals::offsets.offsets.size()) + " offsets and " + to_string(Globals::fflagOffsets.offsets.size()) + " FFlags in " + to_string(duration.count()) + "s";
    Logger::Log(Logger::Level::Success, message);
}