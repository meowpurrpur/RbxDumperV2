#include "../globals.h"

inline static const uint8_t expectedBytecode[] = { 0x1b, 0x8a, 0x8a, 0x91, 0xbf, 0x7d, 0x6c, 0x44, 0xb9, 0x94, 0x3f, 0x15, 0x15, 0x97, 0x99, 0x8e, 0xd9, 0xbb, 0x1c, 0x3f, 0x6a, 0x9d, 0xd7, 0xc9, 0x14, 0xce, 0x5c, 0xb0, 0x57, 0xf7, 0xc5, 0x12, 0x85, 0x4d, 0x1f, 0xbf, 0x79, 0xda, 0x1f, 0x6b, 0x19, 0x2c, 0x19, 0x5b, 0xaa, 0x16, 0x56, 0x5f, 0x67, 0x73, 0x79, 0x8e, 0x50, 0x0c, 0xea, 0x1c, 0xd3, 0x78, 0xbf, 0xa6, 0x18, 0xe7, 0x14, 0xc5, 0x6c, 0x5e, 0xba, 0xfa, 0x56, 0xb1, 0x1a, 0x12, 0x53, 0x71, 0x5e, 0x9b, 0xc1, 0x23, 0x1c, 0xc1, 0x18, 0xa8, 0x98, 0xf7, 0xbd, 0xef };

void Dump::Scripts() {
	uintptr_t workspace = Roblox::FindFirstChild(Globals::dataModel, "Workspace");
	uintptr_t localScript = Roblox::FindFirstChild(workspace, "LocalScript");

	uintptr_t bytecodeSize = 0x20;
	uintptr_t bytecodePointer = 0x10;

	string lsGuid = "{704E7537-4EFC-4E9E-BB5A-60CDCA238EAD}";
	string lsHash = "d41d8cd98f00b204e9800998ecf8427e";

	Helper::DoBasicScan(BasicScanConfig<string>(
		"GUID",
		"LocalScript",
		{ localScript },
		{ lsGuid }
	));

	Helper::DoBasicScan(BasicScanConfig<string>(
		"Hash",
		"LocalScript",
		{ localScript },
		{ lsHash }
	));

	uintptr_t localScriptBytecodeOffset = 0;
	for (int i = 0x100; i < 0x1000; i++) {
		try {
			uintptr_t embedded = Mem::Read<uintptr_t>(localScript + i);
			uintptr_t bytecode = Mem::Read<uintptr_t>(embedded + bytecodePointer);
			uintptr_t size = Mem::Read<uintptr_t>(embedded + bytecodeSize);

			if (size != sizeof(expectedBytecode))
			{
				localScriptBytecodeOffset++;
				continue;
			}

			string bytecodeString;
			bytecodeString.resize(size);

			Mem::ReadBytes(bytecode, bytecodeString.data(), size);
			if (memcmp(bytecodeString.data(), expectedBytecode, size) == 0) {
				localScriptBytecodeOffset = i;
				break;
			}
		}
		catch (const exception&) {}
	}

	Globals::offsets.add("ByteCode", "LocalScript", localScriptBytecodeOffset, "unsigned __int64");

	Globals::offsets.add("Size", "ByteCode", bytecodeSize, "unsigned __int64");
	Globals::offsets.add("Pointer", "ByteCode", bytecodePointer, "unsigned __int64");

	uintptr_t moduleScript = Roblox::FindFirstChild(workspace, "ModuleScript");
	string msGuid = "{4330BBB6-38D5-43C8-B302-C2DC97068AF6}";

	Helper::DoBasicScan(BasicScanConfig<string>(
		"GUID",
		"ModuleScript",
		{ moduleScript },
		{ msGuid }
	));

	Helper::DoBasicScan(BasicScanConfig<string>(
		"Hash",
		"ModuleScript",
		{ moduleScript },
		{ lsHash }
	));

	uintptr_t moduleScriptBytecodeOffset = 0x100;
	while (true) {
		if (moduleScriptBytecodeOffset >= 0x1000) {
			moduleScriptBytecodeOffset = 0;
			Logger::Log(Logger::Level::Warning, "ModuleScript Bytecode dump stalled, try rejoining the game.");
			break;
		}

		try {
			uintptr_t embedded = Mem::Read<uintptr_t>(moduleScript + moduleScriptBytecodeOffset);
			uintptr_t bytecode = Mem::Read<uintptr_t>(embedded + bytecodePointer);
			uintptr_t size = Mem::Read<uintptr_t>(embedded + bytecodeSize);

			if (size != sizeof(expectedBytecode))
			{
				moduleScriptBytecodeOffset++;
				continue;
			}

			string bytecodeString;
			bytecodeString.resize(size);

			Mem::ReadBytes(bytecode, bytecodeString.data(), size);
			if (memcmp(bytecodeString.data(), expectedBytecode, size) == 0) {
				break;
			}
		}
		catch (const exception&) {}

		moduleScriptBytecodeOffset++;
	}
	Globals::offsets.add("ByteCode", "ModuleScript", moduleScriptBytecodeOffset, "unsigned __int64");

	uintptr_t script = Roblox::FindFirstChild(workspace, "Script");
	string scriptGuid = "{7C692C7B-5ABE-400F-AA26-035C6BC2A36A}";

	Helper::DoBasicScan(BasicScanConfig<string>(
		"GUID",
		"Script",
		{ script },
		{ scriptGuid }
	));

	Helper::DoBasicScan(BasicScanConfig<string>(
		"Hash",
		"Script",
		{ script },
		{ lsHash }
	));

	uintptr_t scriptBytecodeOffset = 0x100;
	while (true) {
		if (scriptBytecodeOffset >= 0x1000) {
			scriptBytecodeOffset = 0;
			Logger::Log(Logger::Level::Warning, "Script Bytecode dump stalled, try rejoining the game.");
			break;
		}

		try
		{
			uintptr_t embedded = Mem::Read<uintptr_t>(script + scriptBytecodeOffset);
			uintptr_t bytecode = Mem::Read<uintptr_t>(embedded + bytecodePointer);
			uintptr_t size = Mem::Read<uintptr_t>(embedded + bytecodeSize);

			if (size != sizeof(expectedBytecode))
			{
				scriptBytecodeOffset++;
				continue;
			}

			string bytecodeString;
			bytecodeString.resize(size);

			Mem::ReadBytes(bytecode, bytecodeString.data(), size);
			if (memcmp(bytecodeString.data(), expectedBytecode, size) == 0) {
				break;
			}
		}
		catch (const exception&) {}

		scriptBytecodeOffset++;
	}
	Globals::offsets.add("ByteCode", "Script", scriptBytecodeOffset, "unsigned __int64");
}
