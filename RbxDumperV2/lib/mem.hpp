#pragma once
#include <Windows.h>
#include <winternl.h>
#include <stdexcept>
#include <string>

using namespace std;
typedef CLIENT_ID* PCLIENT_ID;

class DriverException : public runtime_error {
public:
    using runtime_error::runtime_error;
};

class Mem {
    inline static HANDLE processHandle = nullptr;

    typedef NTSTATUS (NTAPI* NtOpenProcess_t)(
        PHANDLE,
        ACCESS_MASK,
        POBJECT_ATTRIBUTES,
        PCLIENT_ID
    );

    typedef NTSTATUS (NTAPI* NtReadVirtualMemory_t)(
        HANDLE,
        PVOID,
        PVOID,
        SIZE_T,
        PSIZE_T
    );

    typedef NTSTATUS (NTAPI* NtWriteVirtualMemory_t)(
        HANDLE,
        PVOID,
        PVOID,
        SIZE_T,
        PSIZE_T
    );

    inline static NtOpenProcess_t ntOpenProcessFn = nullptr;
    inline static NtReadVirtualMemory_t ntReadVirtualMemoryFn = nullptr;
    inline static NtWriteVirtualMemory_t ntWriteVirtualMemoryFn = nullptr;

    static bool ResolveNt() {
        static bool resolved = false;
        if (resolved) return true;

        HMODULE ntdll = GetModuleHandleA("ntdll.dll");
        if (!ntdll) return false;

        ntOpenProcessFn = (NtOpenProcess_t)GetProcAddress(ntdll, "NtOpenProcess");
        ntReadVirtualMemoryFn = (NtReadVirtualMemory_t)GetProcAddress(ntdll, "NtReadVirtualMemory");
        ntWriteVirtualMemoryFn = (NtWriteVirtualMemory_t)GetProcAddress(ntdll, "NtWriteVirtualMemory");

        resolved = ntOpenProcessFn && ntReadVirtualMemoryFn && ntWriteVirtualMemoryFn;
        return resolved;
    }

public:
    inline static DWORD targetProcessId = 0;
    inline static DWORD& TargetProcessId = targetProcessId;

    static bool IsValid(uint64_t address) {
        if (address <= 0x400000 || address == 0xCCCCCCCCCCCCCCCC || address > 0x7FFFFFFFFFFFFFFF)
            return false;
        return true;
    }

    static bool Initialize(DWORD processId) {
        if (!ResolveNt())
            return false;

        OBJECT_ATTRIBUTES objAttr{};
        CLIENT_ID clientId{};

        InitializeObjectAttributes(&objAttr, nullptr, 0, nullptr, nullptr);

        clientId.UniqueProcess = (HANDLE)(uintptr_t)processId;
        clientId.UniqueThread = nullptr;

        NTSTATUS status = ntOpenProcessFn(
            &processHandle,
            PROCESS_ALL_ACCESS,
            &objAttr,
            &clientId
        );

        if (status != 0)
            return false;

        targetProcessId = processId;
        return true;
    }

    template<typename T>
    static T Read(uint64_t address) {
        if (!IsValid(address)) return T{};

        T buffer{};
        SIZE_T bytesRead{};

        ntReadVirtualMemoryFn(
            processHandle,
            (PVOID)address,
            &buffer,
            sizeof(T),
            &bytesRead
        );

        return buffer;
    }

    template<typename T>
    static void Write(uint64_t address, const T& buffer) {
        if (!IsValid(address)) return;

        SIZE_T bytesWritten{};

        ntWriteVirtualMemoryFn(
            processHandle,
            (PVOID)address,
            (PVOID)&buffer,
            sizeof(T),
            &bytesWritten
        );
    }

    static void ReadBytes(uintptr_t addr, void* buffer, size_t size) {
        if (!IsValid(addr)) return;

        SIZE_T bytesRead{};

        NTSTATUS status = ntReadVirtualMemoryFn(
            processHandle,
            (PVOID)addr,
            buffer,
            size,
            &bytesRead
        );

        if (status != 0)
            throw DriverException("NtReadVirtualMemory failed");
    }

    static void Shutdown() {
        if (processHandle && processHandle != INVALID_HANDLE_VALUE) {
            CloseHandle(processHandle);
            processHandle = nullptr;
        }
    }

    static string ReadString(uintptr_t addr) {
        if (addr == 0 || !IsValid(addr))
            return "invalid_str";

        string result;
        result.reserve(204);

        int length = Read<int>(addr + 0x10);
        if (length > 15000 || length <= 0)
            return "invalid_str";

        uintptr_t stringAddr = length >= 16 ? Read<uintptr_t>(addr) : addr;

        int offset = 0;
        while (offset < 200) {
            char ch = Read<char>(stringAddr + offset);
            if (ch == '\0') break;

            result.push_back(ch);
            offset += sizeof(char);
        }

        return result;
    }

    static string ReadRawString(uintptr_t addr) {
        if (addr == 0 || !IsValid(addr))
            return "invalid_str";

        string result;
        result.reserve(256);

        int offset = 0;
        while (offset < 256) {
            char ch = Read<char>(addr + offset);
            if (ch == '\0') break;

            result.push_back(ch);
            offset += sizeof(char);
        }

        return result;
    }

    static void WriteString(uintptr_t addr, const std::string& str) {
        if (addr == 0 || !IsValid(addr))
            return;

        size_t length = str.length();
        if (length > 15000) return;

        uintptr_t stringAddr = length >= 16 ? Read<uintptr_t>(addr) : addr;

        for (size_t i = 0; i < length; ++i)
            Write<char>(stringAddr + i, str[i]);

        Write<int>(addr + 0x10, static_cast<int>(length));
    }

    static void WriteRawString(uintptr_t addr, const std::string& str) {
        if (addr == 0 || !IsValid(addr))
            return;

        size_t length = str.length();

        for (size_t i = 0; i < length; ++i)
            Write<char>(addr + i, str[i]);
    }
};
