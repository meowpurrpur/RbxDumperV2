@echo off
if not exist "vulkan" mkdir vulkan
curl -L "https://github.com/atrexus/vulkan/releases/download/v2.1.2/vulkan.exe" -o "vulkan\vulkan.exe"

echo vulkan has been downloaded.
pause
