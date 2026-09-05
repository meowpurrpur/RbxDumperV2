#pragma once

namespace Roblox {
	vector<uintptr_t> GetChildren(uintptr_t instance);
	string GetName(uintptr_t instance);
	string GetClassN(uintptr_t instance);
	uintptr_t FindFirstChild(uintptr_t instance, string name);
}
