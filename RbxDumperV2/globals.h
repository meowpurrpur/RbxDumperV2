#include <Windows.h>
#include <iostream>
#include <d3d9.h>
#include <dwmapi.h>
#include <shared_mutex>
#include <TlHelp32.h>
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <filesystem>
#include <regex>
#define M_PI 3.14159265358979323846f

using namespace std;
namespace fs = filesystem;


#include "lib/logger.h"
#include "lib/mem.hpp"
#include "lib/json.hpp"
#include "classes/vectors.h"
#include "lib/rtti.hpp"
#include "lib/patternScanning.hpp"
#include "lib/dissasemble.h"

using json = nlohmann::ordered_json;

#include "classes/offset.h"
namespace Globals {
	inline RbxDumper::OffsetList offsets = RbxDumper::OffsetList("Offsets");
	inline RbxDumper::OffsetList fflagOffsets = RbxDumper::OffsetList("FFlagOffsets");

	inline uintptr_t baseAddress = 0;
	inline uintptr_t dataModel = 0;

	inline string dumperVersion = "2.2.0";
	inline string clientVersion = "";
	inline string robloxPath = "";

	inline bool isFullDump = false;

	inline int windowWidth = 1920;
	inline int windowHeight = 1080;
	inline Vector2 windowResolution = Vector2(1920.f, 1080.f);

	inline double maxFps = 144.0;

	inline bool UpdateMaxFps() {
		char localAppData[MAX_PATH];
		if (!GetEnvironmentVariableA("LOCALAPPDATA", localAppData, MAX_PATH)) {
			return false;
		}

		std::filesystem::path settingsPath = std::filesystem::path(localAppData) / "Roblox" / "GlobalBasicSettings_13.xml";
		if (!std::filesystem::exists(settingsPath)) {
			return false;
		}

		std::ifstream file(settingsPath);
		if (!file.is_open()) {
			return false;
		}

		std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

		std::regex capRegex(R"(<int[^>]*name=["']FramerateCap["'][^>]*>\s*(\d+)\s*</int>)");
		std::smatch match;
		if (std::regex_search(content, match, capRegex) && match.size() > 1) {
			try {
				double fps = std::stod(match[1].str());
				if (fps > 0.0) {
					maxFps = fps;
					return true;
				}
			} catch (...) {}
		}

		std::string marker = "name=\"FramerateCap\">";
		size_t pos = content.find(marker);
		if (pos != std::string::npos) {
			pos += marker.length();
			size_t endPos = content.find("</int>", pos);
			if (endPos != std::string::npos) {
				try {
					double fps = std::stod(content.substr(pos, endPos - pos));
					if (fps > 0.0) {
						maxFps = fps;
						return true;
					}
				} catch (...) {}
			}
		}

		return false;
	}

	inline bool UpdateResolution() {
		HWND hwnd = FindWindowA(nullptr, "Roblox");
		if (hwnd) {
			RECT rect{};
			if (GetClientRect(hwnd, &rect)) {
				int width = rect.right - rect.left;
				int height = rect.bottom - rect.top;
				if (width > 0 && height > 0) {
					windowWidth = width;
					windowHeight = height;
					windowResolution = Vector2(static_cast<float>(width), static_cast<float>(height));
					return true;
				}
			}
		}
		return false;
	}
}

namespace DumperConstants {
	inline uintptr_t placeId = 0;
	inline uintptr_t gameId = 0;
	inline uintptr_t creatorId = 0;
	inline uintptr_t userId = 0;
	inline std::string dumpedRobloxPath = "";
	inline std::string userDisplayName = "";

	inline bool load(const std::string& configPath = "config.json") {
		std::string resolvedPath = configPath;
		if (!std::filesystem::exists(resolvedPath) && std::filesystem::exists("../" + configPath)) {
			resolvedPath = "../" + configPath;
		}

		if (!std::filesystem::exists(resolvedPath)) {
			Logger::Log(Logger::Level::Error, "config.json not found!");
			return false;
		}

		Logger::Log(Logger::Level::Info, "config.json found");

		try {
			std::ifstream inFile(resolvedPath);
			if (!inFile.is_open()) {
				Logger::Log(Logger::Level::Error, "Failed to open config file: " + resolvedPath);
				return false;
			}

			json config = json::parse(inFile);

			if (config.contains("dumpedRobloxPath")) dumpedRobloxPath = config["dumpedRobloxPath"].get<std::string>();
			else if (config.contains("robloxPath")) dumpedRobloxPath = config["robloxPath"].get<std::string>();
			else if (config.contains("dumpPath")) dumpedRobloxPath = config["dumpPath"].get<std::string>();

			if (config.contains("userDisplayName")) userDisplayName = config["userDisplayName"].get<std::string>();

			if (config.contains("placeId")) placeId = config["placeId"].get<uintptr_t>();
			if (config.contains("gameId")) gameId = config["gameId"].get<uintptr_t>();
			if (config.contains("creatorId")) creatorId = config["creatorId"].get<uintptr_t>();
			if (config.contains("userId")) userId = config["userId"].get<uintptr_t>();

			return true;
		} catch (const std::exception& e) {
			Logger::Log(Logger::Level::Error, std::string("Error parsing ") + resolvedPath + ": " + e.what());
			return false;
		}
	}
}

#include "classes/helper.h"
#include "classes/roblox.h"
#include "dump/dump.h"