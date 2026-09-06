#include "../globals.h"

static constexpr bool areMatricesSimilar(const ViewMatrix_t& matA, const ViewMatrix_t& matB, float epsilon = 0.0001f)
{
	for (int r = 0; r < 4; r++)
	{
		for (int c = 0; c < 4; c++)
		{
			float diff = matA.arrData[r][c] - matB.arrData[r][c];
			if (diff < 0) diff = -diff;
			if (diff > epsilon)
				return false;
		}
	}
	return true;
}

void Dump::StaticDump()
{
	Logger::Log(Logger::Level::Debug, "Loading buffer into disassembler...");
	Dissasembler::Decoder dec(PatternScanning::buffer);

	Logger::Log(Logger::Level::Info, "Scanning for TaskScheduler::Pointer...");
	uintptr_t taskSchedulerPointer = 0;
	auto taskInstructions = Dissasembler::FindString("Default job arbiter must always be valid", dec, 50, 150);

	for (const auto& instruction : taskInstructions)
	{
		if (instruction.info.mnemonic == ZYDIS_MNEMONIC_MOV &&
			instruction.operands[0].type == ZYDIS_OPERAND_TYPE_MEMORY) {
			uintptr_t addr = instruction.address + instruction.info.length + dec.GetMemoryDisplacement(instruction);
			uintptr_t possibleSched = Mem::Read<uintptr_t>(Globals::baseAddress + addr);

			for (int i = 0x0; i < 0xff; i++)
			{
				double targetFps = 1 / Mem::Read<double>(possibleSched + i);
				if (abs(targetFps - Globals::maxFps) < 1.0) {
					taskSchedulerPointer = addr;
					break;
				}
			}

			if (taskSchedulerPointer != 0) break;
		}
	}

	if (!taskSchedulerPointer) {
		Logger::Log(Logger::Level::Error, "!!!!! TaskScheduler::Pointer failed to dump !!!!!");
		system("pause");
	}

	Logger::Log(Logger::Level::Info, "Scanning for VisualEngine::Pointer...");
	uintptr_t visualEnginePointer = 0;
	vector<Dissasembler::Instruction> visualInstructions = Dissasembler::FindString("AutoCapture_{}_Frames-{}-{}.raw", dec);

	for (const auto& instruction : visualInstructions)
	{
		if (instruction.info.mnemonic == ZYDIS_MNEMONIC_XCHG)
		{
			uintptr_t addr = instruction.address + instruction.info.length + dec.GetMemoryDisplacement(instruction);
			for (int j = 8; j < 50; j++)
			{
				uintptr_t possibleVisual = Mem::Read<uintptr_t>(Globals::baseAddress + addr + j);
				for (int i = 0x500; i < 0x1000; i++)
				{
					Vector2 dimensions = Mem::Read<Vector2>(possibleVisual + i);
					if (abs(dimensions.x - Globals::windowResolution.x) <= 5.f && abs(dimensions.y - Globals::windowResolution.y) <= 5.f)
					{
						visualEnginePointer = addr + j;
						break;
					}
				}

				if (visualEnginePointer != 0) break;
			}

			if (visualEnginePointer != 0) break;
		}
	}

	if (!visualEnginePointer) {
		Logger::Log(Logger::Level::Error, "!!!!! VisualEngine::Pointer failed to dump !!!!!");
		system("pause");
	}

	Logger::Log(Logger::Level::Info, "Scanning for FakeDataModel::Pointer...");
	uintptr_t fakeDataModelPointer = 0;

	for (uintptr_t offset = 0x8000000; offset < 0x9000000; offset += sizeof(uintptr_t))
	{
		auto possibleDm = Mem::Read<uintptr_t>(Globals::baseAddress + offset);

		if (GetRTTI(possibleDm) == "RBX::DataModel")
		{
			fakeDataModelPointer = offset;
			break;
		}
	}

	if (!fakeDataModelPointer)
	{
		Logger::Log(Logger::Level::Error, "!!!!! FakeDataModel::Pointer failed to dump !!!!!");
		system("pause");
	}

	Logger::Log(Logger::Level::Success, "Finished static dump!");

	Globals::offsets.add("Pointer", "TaskScheduler", taskSchedulerPointer, "unsigned __int64");
	Globals::offsets.add("Pointer", "VisualEngine", visualEnginePointer, "unsigned __int64");
	Globals::offsets.add("Pointer", "FakeDataModel", fakeDataModelPointer, "unsigned __int64");

	uintptr_t taskScheduler = Mem::Read<uintptr_t>(Globals::baseAddress + taskSchedulerPointer);

	uintptr_t jobStartOff = 0;
	uintptr_t jobEndOff = 0;
	uintptr_t jobNameOff = 0;
	for (int i = 0x50; i < 0x250; i++)
	{
		uintptr_t jobStart = Mem::Read<uintptr_t>(taskScheduler + i);
		if (!Mem::IsValid(jobStart)) continue;

		for (int j = 0x50; j < 0x250; j++)
		{
			uintptr_t jobEnd = Mem::Read<uintptr_t>(taskScheduler + j);
			if (!Mem::IsValid(jobEnd)) continue;
			if (jobEnd - jobStart >= 0x1000) continue;

			for (uintptr_t jobPtr = jobStart; jobPtr < jobEnd; jobPtr += 0x10) {
				uintptr_t job = Mem::Read<uintptr_t>(jobPtr);
				if (!Mem::IsValid(job)) continue;

				for (int r = 0x10; r < 0x30; r++)
				{
					string jobName = Mem::ReadString(job + r);
					if (jobName == "RenderJob") {
						jobStartOff = i;
						jobEndOff = j;
						jobNameOff = r;
						goto foundjob;
					}
				}
			}
		}
	}

foundjob: {
	Globals::offsets.add("JobStart", "TaskScheduler", jobStartOff, "unsigned __int64");
	Globals::offsets.add("JobEnd", "TaskScheduler", jobEndOff, "unsigned __int64");
	Globals::offsets.add("JobName", "TaskScheduler", jobNameOff, "string");
}

	uintptr_t fakeDataModel = Mem::Read<uintptr_t>(Globals::baseAddress + fakeDataModelPointer);
	RbxDumper::Offset realDataModelOffset = Helper::DoRTTIScan(RTTIScanConfig(
		"RealDataModel",
		"FakeDataModel",
		{ fakeDataModel },
		{ "RBX::DataModel" },
		"unsigned __int64"
	));

	Globals::dataModel = Mem::Read<uintptr_t>(fakeDataModel + realDataModelOffset.value);
	Logger::Log(Logger::Level::Success, "DataModel found at: " + to_string(Globals::dataModel));

	uintptr_t visualEngine = Mem::Read<uintptr_t>(Globals::baseAddress + visualEnginePointer);
	Helper::DoBasicScan(BasicScanConfig<Vector2>(
		"Dimensions",
		"VisualEngine",
		{ visualEngine },
		{ Globals::windowResolution }
	));

	ViewMatrix_t targetMatrix{
		0.7275f, 0.0000f, 0.0000f, 0.0000f,
		0.0000f, 1.2493f, -0.3348f, -5.6194f,
		0.0000f, 0.0000f, 0.0000f, 0.1000f,
		0.0000f, -0.2588f, -0.9659f, 13.6642f
	};

	for (uintptr_t offset = 0x0; offset <= 0x1000; offset += 0x2)
	{
		auto candidateMatrix = Mem::Read<ViewMatrix_t>(visualEngine + offset);

		if (areMatricesSimilar(candidateMatrix, targetMatrix))
		{
			Globals::offsets.add("ViewMatrix", "VisualEngine", offset, "ViewMatrix_t");
			break;
		}
	}

	// compatibility, dont dump these anymore
	Globals::offsets.add("LightingValid", "RenderView", 0, "bool");
	Globals::offsets.add("SkyValid", "RenderView", 0, "bool");
	Globals::offsets.add("VisualEngine", "RenderView", 0, "unsigned __int64");
	Globals::offsets.add("DeviceD3D11", "RenderView", 0, "unsigned __int64");
	Globals::offsets.add("RequireBypass", "ScriptContext", 0);
	Globals::offsets.add("IsCoreScript", "ModuleScript", 0);
	Globals::offsets.add("SensitivityPointer", "MouseService", 0, "float");
	Globals::offsets.add("Pointer", "PlayerConfigurer", 0, "unsigned __int64");

	Helper::DoRTTIScan(RTTIScanConfig(
		"RenderView",
		"VisualEngine",
		{ visualEngine },
		{ "RBX::Graphics::RenderView" }
	));
}
