#include "../globals.h"
#include <regex>

void Dump::DataModel() {
	Helper::DoBasicScan(BasicScanConfig<uintptr_t>(
		"PlaceId",
		"DataModel",
		{ Globals::dataModel },
		{ DumperConstants::placeId }
	));

	Helper::DoBasicScan(BasicScanConfig<uintptr_t>(
		"GameId",
		"DataModel",
		{ Globals::dataModel },
		{ DumperConstants::gameId }
	));

	Helper::DoBasicScan(BasicScanConfig<uintptr_t>(
		"CreatorId",
		"DataModel",
		{ Globals::dataModel },
		{ DumperConstants::creatorId }
	));

	Helper::DoBasicScan(BasicScanConfig<uintptr_t>(
		"GameLoaded",
		"DataModel",
		{ Globals::dataModel },
		{ 31 },
		0x500
	));

	uintptr_t workspace = Roblox::FindFirstChild(Globals::dataModel, "Workspace");
	uintptr_t jobIdFolder = Roblox::FindFirstChild(workspace, "GetAJob");
	string jobId = Roblox::GetName(Roblox::GetChildren(jobIdFolder)[0]);

	Helper::DoBasicScan(BasicScanConfig<string>(
		"JobId",
		"DataModel",
		{ Globals::dataModel },
		{ jobId }
	));

	Helper::DoBasicScan(BasicScanConfig<uintptr_t>(
		"Workspace",
		"DataModel",
		{ Globals::dataModel },
		{ workspace }
	));

	uintptr_t childrenPtr = Mem::Read<uintptr_t>(Mem::Read<uintptr_t>(Globals::dataModel + Globals::offsets.get("ChildrenStart").value));
	uintptr_t scriptContext = Roblox::FindFirstChild(Globals::dataModel, "Script Context");

	Helper::DoBasicScan(BasicScanConfig<uintptr_t>(
		"ScriptContext",
		"DataModel",
		{ childrenPtr },
		{ scriptContext }
	));

	uintptr_t folder = Roblox::FindFirstChild(workspace, "PlaceVersion");
	uintptr_t part = Roblox::GetChildren(folder)[0];
	int expectedVersion = stoi(Roblox::GetName(part));

	Helper::DoBasicScan(BasicScanConfig<int>(
		"PlaceVersion",
		"DataModel",
		{ Globals::dataModel },
		{ expectedVersion }
	));

	uintptr_t runService = Roblox::FindFirstChild(Globals::dataModel, "Run Service");
	uintptr_t heartbeatJobOffset = 0xe0;
	uintptr_t heartbeatFpsOffset = 0;

	while (true) {
		uintptr_t heartbeat = Mem::Read<uintptr_t>(runService + heartbeatJobOffset);

		for (int i = 0xb0; i < 0xFF; i++) {
			double fps = 1 / Mem::Read<double>(heartbeat + i);
			if (abs(fps - Globals::maxFps) < 30.0) {
				heartbeatFpsOffset = i;
				break;
			}
		}

		if (heartbeatFpsOffset != 0) break;
		heartbeatJobOffset++;
	}

	Globals::offsets.add("HeartbeatTask", "RunService", heartbeatJobOffset, "unsigned __int64");
	Globals::offsets.add("HeartbeatFPS", "RunService", heartbeatFpsOffset, "double");

	for (int i = 0; i < 0x1000; i++) {
		string maybeIp = Mem::ReadString(Globals::dataModel + i);
		regex pattern(R"(^((25[0-5]|2[0-4]\d|1\d\d|\d\d|\d)\.){3}(25[0-5]|2[0-4]\d|1\d\d|\d\d|\d)\|(\d{1,5})$)");

		if (regex_match(maybeIp, pattern)) {
			Globals::offsets.add("ServerIP", "DataModel", i, "string");
		}
	}

	uintptr_t taskScheduler = Mem::Read<uintptr_t>(Globals::baseAddress + Globals::offsets.get("Pointer", "TaskScheduler").value);
	uintptr_t jobStart = Mem::Read<uintptr_t>(taskScheduler + Globals::offsets.get("JobStart", "TaskScheduler").value);
	uintptr_t jobEnd = Mem::Read<uintptr_t>(taskScheduler + Globals::offsets.get("JobEnd", "TaskScheduler").value);

	uintptr_t renderJob = 0;
	for (uintptr_t jobPtr = jobStart; jobPtr < jobEnd; jobPtr += 0x10) {
		uintptr_t job = Mem::Read<uintptr_t>(jobPtr);
		if (!job || !Mem::IsValid(job)) continue;

		string jobName = Mem::ReadString(job + Globals::offsets.get("JobName", "TaskScheduler").value);
		if (jobName == "RenderJob") {
			renderJob = job;
		}
	}

	if (!renderJob) {
		Logger::Log(Logger::Level::Warning, "Failed to find RenderJob in TaskScheduler!");
	}

	uintptr_t renderViewOffset = Helper::DoRTTIScan(RTTIScanConfig(
		"RenderView",
		"RenderJob",
		{ renderJob },
		{ "RBX::Graphics::RenderView" },
		"unsigned __int64"
	)).value;

	uintptr_t renderView = Mem::Read<uintptr_t>(renderJob + renderViewOffset);
	bool rvFound = false;
	for (int i = 0x100; i < 0x250; i++)
	{
		uintptr_t ptr1 = Mem::Read<uintptr_t>(Globals::dataModel + i);
		uintptr_t ptr2 = Mem::Read<uintptr_t>(ptr1 + 0x8);
		for (int j = 0x8; j < 0x50; j++)
		{
			uintptr_t ptr3 = Mem::Read<uintptr_t>(ptr2 + j);
			if (ptr3 == renderView)
			{
				Globals::offsets.add("ToRenderView1", "DataModel", i, "unsigned __int64");
				Globals::offsets.add("ToRenderView2", "DataModel", 0x8, "unsigned __int64");
				Globals::offsets.add("ToRenderView3", "DataModel", j, "unsigned __int64");
				rvFound = true;
				break;
			}
		}

		if (rvFound) break;
	}

	uintptr_t targetFpsOffset = 0;
	while (true) {
		if (targetFpsOffset >= 0x1000) break;
		double targetFps = 1 / Mem::Read<double>(taskScheduler + targetFpsOffset);
		if (abs(targetFps - Globals::maxFps) < 1.0) {
			break;
		}
		targetFpsOffset++;
	}
	Globals::offsets.add("MaxFPS", "TaskScheduler", targetFpsOffset, "double");

	uintptr_t fakeDatamodel = 0;
	uintptr_t maybeRealDatamodel = 0;
	bool found = false;

	for (uintptr_t offset1 = 0x20; offset1 < 0x100; offset1 += 0x1) {
		fakeDatamodel = Mem::Read<uintptr_t>(renderJob + offset1);
		if (!Mem::IsValid(fakeDatamodel)) continue;

		for (uintptr_t offset2 = 0x100; offset2 < 0x300; offset2 += 0x1) {
			maybeRealDatamodel = Mem::Read<uintptr_t>(fakeDatamodel + offset2);
			if (!Mem::IsValid(maybeRealDatamodel)) continue;

			string name = Roblox::GetName(maybeRealDatamodel);
			if (name == "Ugc") {
				Globals::offsets.add("FakeDataModel", "RenderJob", offset1, "unsigned __int64");
				Globals::offsets.add("RealDataModel", "RenderJob", offset2, "unsigned __int64");

				found = true;
				break;
			}
		}
		if (found) break;
	}

	uintptr_t visualEngine = Mem::Read<uintptr_t>(Globals::baseAddress + Globals::offsets.get("Pointer", "VisualEngine").value);
	uintptr_t fakeDataModel = Mem::Read<uintptr_t>(Globals::baseAddress + Globals::offsets.get("Pointer", "FakeDataModel").value);

	Helper::DoBasicScan(BasicScanConfig<uintptr_t>(
		"FakeDataModel",
		"VisualEngine",
		{ visualEngine },
		{ fakeDataModel }
	));
}
