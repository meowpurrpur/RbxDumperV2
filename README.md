# RbxDumperV2

An offset and FFlag dumper for Roblox.

---

## Installation

### Option A: Prebuilt Release
1. Go to the [Releases](https://git.imtheo.lol/theo/RbxDumperV2/releases) page.
2. Download `release.zip` and extract it into a folder. The zip includes `RbxDumperV2.exe`, `download_vulkan.bat`, and `run_dumper.bat`.
3. Run `download_vulkan.bat` once to download `vulkan.exe` into the `vulkan/` folder.

### Option B: Building from Source
1. Clone the repository:
   ```cmd
   git clone https://git.imtheo.lol/theo/RbxDumperV2.git
   cd RbxDumperV2
   ```
2. Run `download_vulkan.bat` once to download `vulkan.exe` into the `vulkan/` folder.
3. Build using CMake in **Release** (or **Debug**) mode:
   ```cmd
   cmake -B cmake-build-release -DCMAKE_BUILD_TYPE=Release
   cmake --build cmake-build-release --config Release
   ```

---

## Configuration

Create a file named `config.json` in the same directory as the scripts/executable:

```json
{
    "dumpedRobloxPath": "DumpedRoblox.exe",
    "userDisplayName": "YOUR_DISPLAY_NAME",
    "placeId": 113264082193197,
    "gameId": 8220026920,
    "creatorId": 8979812863,
    "userId": YOUR_USER_ID
}
```

### Config Notes:
- Keep `placeId`, `gameId`, `creatorId`, and `dumpedRobloxPath` as default unless using your own game.
- Change `userDisplayName` and `userId` to your own account's display name and user ID.

---

## How to Run

1. Open Roblox and join the [game](https://www.roblox.com/games/113264082193197/Dumper9000) using the account specified in `config.json`.
   - Make sure your Roblox is set to **fullscreen at 1920x1080** resolution.
   - Set your max frame rate to **144 FPS**.
2. Once in-game, run `run_dumper.bat`.
   - This runs `vulkan` to dump the Roblox process to `DumpedRoblox.exe`.
   - Then it automatically launches `RbxDumperV2.exe`.
3. The dumped offsets and FFlags will be saved in a versioned directory (e.g. `version-xxxxxxxx/`).

---

## Contributing

All development, issues, and pull requests are managed on the self-hosted Git instance at:
👉 **[https://git.imtheo.lol/theo/RbxDumperV2](https://git.imtheo.lol/theo/RbxDumperV2)**

Pull requests or issues submitted elsewhere will not be reviewed or accepted.

### AI & LLM Policy
- **AI-assisted contributions are allowed, but work must be mostly human-written.**
- Fully or predominantly AI-generated pull requests, commits, or issue reports will be rejected.
- You must personally understand, review, and test every line of code you submit. Do not submit unvetted LLM output.

### Submitting Changes
1. Create an account on [git.imtheo.lol](https://git.imtheo.lol) if you do not already have one.
2. Fork the repository and create a feature branch for your changes (`git checkout -b feature/my-change`).
3. Ensure your code conforms to modern C++20 standards and matches the existing codebase conventions.

### Reporting Issues
- Provide detailed steps to reproduce the bug, including your client version, environment details, and relevant console/error logs.

---

## License

This project is licensed under the [GNU General Public License v3.0](LICENSE) - Copyright (C) 2026 theo.
