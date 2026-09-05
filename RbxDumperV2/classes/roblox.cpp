#include "../globals.h"
namespace Roblox {
	vector<uintptr_t> GetChildren(uintptr_t instance)
	{
		uintptr_t childrenStart = Globals::offsets.get("ChildrenStart", "Instance").value;
		vector<uintptr_t> vec;
		uintptr_t childrenListPtr = Mem::Read<uintptr_t>(instance + childrenStart);
		if (!childrenListPtr || !Mem::IsValid(childrenListPtr)) return {};

		uintptr_t listStart = Mem::Read<uintptr_t>(childrenListPtr);
		uintptr_t listEnd = Mem::Read<uintptr_t>(childrenListPtr + 0x8);
		if (!Mem::IsValid(listStart) || !Mem::IsValid(listEnd)) return {};

		for (uintptr_t ptr = listStart; ptr < listEnd; ptr += 0x8) {
			uintptr_t child = Mem::Read<uintptr_t>(ptr);
			if (child && Mem::IsValid(child)) vec.emplace_back(child);
		}
		return vec;
	}

	string GetName(uintptr_t instance)
	{
		uintptr_t nameBaseOff = Globals::offsets.get("NameContainer", "Instance").value;
		uintptr_t nameFieldOff = Globals::offsets.get("Name", "Instance").value;

		uintptr_t nameBase = Mem::Read<uintptr_t>(instance + nameBaseOff);
		if (!Mem::IsValid(nameBase)) return "invalid_str";

		return Mem::ReadString(nameBase + nameFieldOff);
	}

	string GetClassN(uintptr_t instance)
	{
		return "i didnt make this function yet :(";
	}

	uintptr_t FindFirstChild(uintptr_t instance, string name)
	{
		for (auto child : GetChildren(instance)) {
			if (GetName(child) == name)
				return child;
		}
		return 0;
	}
}
