#include "../globals.h"

vector<string> splitString(const string& input, char delimiter) {
	vector<string> result;
	string current;
	for (char c : input) {
		if (c == delimiter) {
			if (!current.empty()) result.push_back(current);
			current.clear();
		}
		else {
			current += c;
		}
	}
	if (!current.empty()) result.push_back(current);
	return result;
}

void dumpMouse()
{
	uintptr_t workspace = Roblox::FindFirstChild(Globals::dataModel, "Workspace");
	uintptr_t mouseService = Roblox::FindFirstChild(Globals::dataModel, "MouseService");

	uintptr_t folder = Roblox::FindFirstChild(workspace, "MousePosition");
	uintptr_t part = Roblox::GetChildren(folder)[0];

	string mousePosString = Roblox::GetName(part);
	vector<string> parts = splitString(mousePosString, ',');
	Vector2 mousePos = Vector2(stof(parts[0]), stof(parts[1]));

	if (mousePos.x < 100 || mousePos.y < 100)
	{
		Logger::Log(Logger::Level::Warning, "Move your mouse to a different position");
		system("pause");
		return dumpMouse();
	}

	bool found = false;
	for (int i = 0xE0; i <= 0x250; i++) {
		uintptr_t inputObj = Mem::Read<uintptr_t>(mouseService + i);

		for (int j = 0xB0; j <= 0x100; j++) {
			Vector2 position = Mem::Read<Vector2>(inputObj + j);
			if (abs(position.x - mousePos.x) <= 1.0f && abs(position.y - mousePos.y) <= 75.0f) {
				found = true;
				Globals::offsets.add("InputObject", "MouseService", i, "unsigned __int64");
				Globals::offsets.add("InputObject2", "MouseService", i + 0x10, "unsigned __int64");
				Globals::offsets.add("MousePosition", "MouseService", j, "Vector2");
				break;
			}
		}

		if (found) break;
	}

	if (!found) {
		Logger::Log(Logger::Level::Warning, "mouse dump failed pls try again");
		system("pause");
		return dumpMouse();
	}
}

void Dump::Mouse() {
	dumpMouse();

	uintptr_t workspace = Roblox::FindFirstChild(Globals::dataModel, "Workspace");
	uintptr_t players = Roblox::FindFirstChild(Globals::dataModel, "Players");
	uintptr_t localPlayer = Roblox::GetChildren(players)[0];

	uintptr_t playerMouseOffset = 0xB00;
	while (true) {
		uintptr_t playerMouse = Mem::Read<uintptr_t>(localPlayer + playerMouseOffset);

		bool found = false;
		for (int i = 0x100; i <= 0x250; i++) {
			uintptr_t workspaceMaybe = Mem::Read<uintptr_t>(playerMouse + i);
			if (workspaceMaybe == workspace) {
				found = true;
				Globals::offsets.add("Workspace", "PlayerMouse", i, "unsigned __int64");
				break;
			}
		}

		if (found) break;
		playerMouseOffset++;
	}

	Globals::offsets.add("Mouse", "Player", playerMouseOffset, "unsigned __int64");

	uintptr_t playerMouse = Mem::Read<uintptr_t>(localPlayer + playerMouseOffset);
	Helper::DoBasicScan(BasicScanConfig<string>(
		"Icon",
		"PlayerMouse",
		{ playerMouse },
		{ "meow" }
	));
}
