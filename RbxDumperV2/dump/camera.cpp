#include "../globals.h"

void Dump::Camera() {
	uintptr_t workspace = Roblox::FindFirstChild(Globals::dataModel, "Workspace");
	uintptr_t currentCamera = Roblox::FindFirstChild(workspace, "Camera");
	uintptr_t camera2 = Roblox::FindFirstChild(workspace, "Camera2");
	uintptr_t camera3 = Roblox::FindFirstChild(workspace, "Camera3");

	Helper::DoBasicScan(BasicScanConfig<uintptr_t>(
		"CurrentCamera",
		"Workspace",
		{ workspace },
		{ currentCamera }
	));

	Helper::DoBasicScan(BasicScanConfig<Vector3>(
		"Position",
		"Camera",
		{ currentCamera },
		{ Vector3(0.f, 7.733f, 12.074f) }
	));

	uintptr_t camRotOffset = 0;
	while (true) {
		if (camRotOffset >= 0xFFFF) {
			camRotOffset = 0;
			Logger::Log(Logger::Level::Warning, "CameraRotation dump stalled, try rejoining the game.");
			break;
		};

		Matrix3x3 rotation = Mem::Read<Matrix3x3>(currentCamera + camRotOffset);
		Vector3 rotationVector = rotation.MatrixToEulerAngles();

		if (abs(rotationVector.x - 15) <= 0.1f) {
			break;
		}

		camRotOffset++;
	}
	Globals::offsets.add("Rotation", "Camera", camRotOffset, "Matrix3x3");

	uintptr_t players = Roblox::FindFirstChild(Globals::dataModel, "Players");
	uintptr_t localPlayer = Roblox::GetChildren(players)[0];
	uintptr_t character = Roblox::FindFirstChild(workspace, Roblox::GetName(localPlayer));
	uintptr_t humanoid = Roblox::FindFirstChild(character, "Humanoid");

	Helper::DoBasicScan(BasicScanConfig<uintptr_t>(
		"CameraSubject",
		"Camera",
		{ currentCamera },
		{ humanoid }
	));

	Helper::DoBasicScan(BasicScanConfig<float>(
		"FieldOfView",
		"Camera",
		{ currentCamera },
		{ 1.3166f }
	));

	float imagePlaneDepth = 1.0f / (2.0f * tanf(1.3166f / 2.0f));
	Helper::DoBasicScan(BasicScanConfig<float>(
		"ImagePlaneDepth",
		"Camera",
		{ currentCamera },
		{ imagePlaneDepth }
	));

	RbxDumper::Offset zoomMinDistance = Helper::DoBasicScan(BasicScanConfig<float>(
		"MinZoomDistance",
		"Player",
		{ localPlayer },
		{ 0.528f }
	));

	Helper::DoBasicScan(BasicScanConfig<float>(
		"MaxZoomDistance",
		"Player",
		{ localPlayer },
		{ 128.f }
	));

	Helper::DoBasicScan(BasicScanConfig<__int32>(
		"CameraMode",
		"Player",
		{ localPlayer },
		{ 0 },
		zoomMinDistance.value
	));

	Helper::DoBasicScan(BasicScanConfig<__int32>(
		"CameraType",
		"Camera",
		{ currentCamera },
		{ 5 },
		0x100
	));

	Helper::DoBasicScan(BasicScanConfig<int16_t>(
		"Viewport",
		"Camera",
		{ currentCamera },
		{ 1920 }
	));

	Helper::DoBasicScan(BasicScanConfig<Vector2>(
		"ViewportSize",
		"Camera",
		{ currentCamera },
		{ Vector2(1920.f, 1080.f) }
	));
}
