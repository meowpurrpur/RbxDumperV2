#include "../globals.h"

#include <Windows.h>
#include <TlHelp32.h>
#include <DbgHelp.h>

#include <vector>
#include <string>
#include <cstdint>

#pragma comment(lib, "Dbghelp.lib")

struct RTTICompleteObjectLocator {
    DWORD signature;
    DWORD offset;
    DWORD cdOffset;
    DWORD typeDescriptor;
    DWORD classDescriptor;
    DWORD baseOffset;
};

struct TypeDescriptor {
    void* vtable;
    uint64_t spare;
    char name[255];
};

struct ModuleInfo {
    uintptr_t baseAddress;
    uintptr_t endAddress;
};

static bool IsAllZeros(const std::string& str) {
    return std::all_of(
        str.begin(),
        str.end(),
        [](char c) { return c == '\0'; }
    );
}

static std::string demangleSymbol(const std::string& kMangledName) {
    if (kMangledName.empty() || kMangledName.size() > 100) {
        return kMangledName;
    }

    std::string mangledName = kMangledName;

    if (mangledName.rfind(".?AV", 0) == 0) {
        mangledName = "?" + mangledName.substr(4);
    }

    char buffer[1024]{};

    DWORD length = UnDecorateSymbolName(
        mangledName.c_str(),
        buffer,
        static_cast<DWORD>(sizeof(buffer)),
        UNDNAME_COMPLETE
    );

    if (length == 0) {
        return mangledName;
    }

    std::string demangledName(buffer, length);

    if (demangledName.rfind(" ??", 0) == 0) {
        demangledName.erase(0, 4);
    }

    return demangledName;
}

std::vector<ModuleInfo> GetModules(DWORD processID) {
    std::vector<ModuleInfo> modules;

    HANDLE snapshot =
        CreateToolhelp32Snapshot(
            TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32,
            processID
        );

    if (snapshot == INVALID_HANDLE_VALUE) {
        return modules;
    }

    MODULEENTRY32 moduleEntry{};
    moduleEntry.dwSize = sizeof(MODULEENTRY32);

    if (Module32First(snapshot, &moduleEntry)) {
        do {
            modules.push_back({
                reinterpret_cast<uintptr_t>(moduleEntry.modBaseAddr),
                reinterpret_cast<uintptr_t>(moduleEntry.modBaseAddr) +
                    moduleEntry.modBaseSize
            });
        }
        while (Module32Next(snapshot, &moduleEntry));
    }

    CloseHandle(snapshot);
    return modules;
}

std::vector<ModuleInfo> real_stored;

static uintptr_t getModuleContaining(
    DWORD processID,
    uintptr_t address
) {
    if (real_stored.empty()) {
        real_stored = GetModules(processID);
    }

    for (const auto& module : real_stored) {
        if (address >= module.baseAddress &&
            address < module.endAddress) {
            return module.baseAddress;
        }
    }

    return 0;
}

std::string GetRTTI(uintptr_t objectAddress) {
    if (!objectAddress || !Mem::IsValid(objectAddress)) {
        return "???";
    }

    const uintptr_t vtableAddress =
        Mem::Read<uintptr_t>(objectAddress);

    if (!vtableAddress || !Mem::IsValid(vtableAddress)) {
        return "???";
    }

    const uintptr_t colAddress =
        Mem::Read<uintptr_t>(
            vtableAddress - sizeof(uintptr_t)
        );

    if (!colAddress || !Mem::IsValid(colAddress)) {
        return "???";
    }

    const RTTICompleteObjectLocator col =
        Mem::Read<RTTICompleteObjectLocator>(colAddress);

    uintptr_t typeInfoAddress =
        getModuleContaining(
            Mem::TargetProcessId,
            colAddress
        ) + col.typeDescriptor;

    if (!typeInfoAddress ||
        !Mem::IsValid(typeInfoAddress)) {
        return "???";
    }

    const TypeDescriptor typeInfo =
        Mem::Read<TypeDescriptor>(typeInfoAddress);

    if (IsAllZeros(typeInfo.name)) {
        return "???";
    }

    return demangleSymbol(typeInfo.name);
}
