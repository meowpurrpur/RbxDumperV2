@echo off
vulkan\vulkan.exe -p RobloxPlayerBeta.exe -d -o DumpedRoblox.exe --resolve-imports -r 0 -w

if exist "RbxDumperV2.exe" (
    RbxDumperV2.exe DumpedRoblox.exe
) else if exist "cmake-build-release\RbxDumperV2\RbxDumperV2.exe" (
    cmake-build-release\RbxDumperV2\RbxDumperV2.exe DumpedRoblox.exe
) else (
    cmake-build-debug\RbxDumperV2\RbxDumperV2.exe DumpedRoblox.exe
)

pause
