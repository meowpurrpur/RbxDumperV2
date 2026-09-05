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
#define M_PI 3.14159265358979323846f

using namespace std;
namespace fs = filesystem;


#include "lib/logger.h"
#include "lib/mem.hpp"
#include "lib/json.hpp"
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

#include "classes/vectors.h"
#include "classes/helper.h"
#include "classes/roblox.h"
#include "dump/dump.h"