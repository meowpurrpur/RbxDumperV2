#include "../globals.h"

void Dump::Interactables() {
	uintptr_t workspace = Roblox::FindFirstChild(Globals::dataModel, "Workspace");
	uintptr_t proximityPrompt = Roblox::FindFirstChild(workspace, "ProximityPrompt");

	Helper::DoBasicScan(BasicScanConfig<string>(
		"ActionText",
		"ProximityPrompt",
		{ proximityPrompt },
		{ "This is action text" }
	));

	Helper::DoBasicScan(BasicScanConfig<string>(
		"ObjectText",
		"ProximityPrompt",
		{ proximityPrompt },
		{ "This is object text" }
	));

	Helper::DoBasicScan(BasicScanConfig<float>(
		"HoldDuration",
		"ProximityPrompt",
		{ proximityPrompt },
		{ 0.282f },
		0x0,
		0x1000,
		0x1,
		false,
		0.01f
	));

	Helper::DoBasicScan(BasicScanConfig<float>(
		"MaxActivationDistance",
		"ProximityPrompt",
		{ proximityPrompt },
		{ 10.882f },
		0x0,
		0x1000,
		0x1,
		false,
		0.01f
	));

	Helper::DoBasicScan(BasicScanConfig<int>(
		"KeyCode",
		"ProximityPrompt",
		{ proximityPrompt },
		{ 101 }
	));

	Helper::DoBasicScan(BasicScanConfig<int>(
		"GamepadKeyCode",
		"ProximityPrompt",
		{ proximityPrompt },
		{ 1000 }
	));

	uintptr_t proximityPrompt2 = Roblox::FindFirstChild(workspace, "ProximityPrompt2");
	uintptr_t proximityPrompt3 = Roblox::FindFirstChild(workspace, "ProximityPrompt3");

	Helper::DoBasicScan(BasicScanConfig<bool>(
		"Enabled",
		"ProximityPrompt",
		{ proximityPrompt, proximityPrompt2, proximityPrompt3 },
		{ true, false, true }
	));

	Helper::DoBasicScan(BasicScanConfig<bool>(
		"RequiresLineOfSight",
		"ProximityPrompt",
		{ proximityPrompt, proximityPrompt2, proximityPrompt3 },
		{ true, true, false }
	));

	uintptr_t clickDetector = Roblox::FindFirstChild(workspace, "ClickDetector");

	Helper::DoBasicScan(BasicScanConfig<float>(
		"MaxActivationDistance",
		"ClickDetector",
		{ clickDetector },
		{ 32.211f },
		0x0,
		0x1000,
		0x1,
		false,
		0.01f
	));

	Helper::DoBasicScan(BasicScanConfig<string>(
		"MouseIcon",
		"ClickDetector",
		{ clickDetector },
		{ "rbxassetid://73879578225090" }
	));

	uintptr_t dragDetector = Roblox::FindFirstChild(workspace, "DragDetector");
	uintptr_t dragDetectorPart = Roblox::FindFirstChild(workspace, "dragdetectpart");

	Helper::DoBasicScan(BasicScanConfig<uintptr_t>(
		"ReferenceInstance",
		"DragDetector",
		{ dragDetector },
		{ dragDetectorPart }
	));

	Helper::DoBasicScan(BasicScanConfig<float>(
		"MaxActivationDistance",
		"DragDetector",
		{ dragDetector },
		{ 743.321f }
	));

	Helper::DoBasicScan(BasicScanConfig<float>(
		"MaxDragAngle",
		"DragDetector",
		{ dragDetector },
		{ 120.83f }
	));

	Helper::DoBasicScan(BasicScanConfig<Vector3>(
		"MaxDragTranslation",
		"DragDetector",
		{ dragDetector },
		{ Vector3(128.83f, 129.83f, 130.83f) }
	));

	Helper::DoBasicScan(BasicScanConfig<float>(
		"MinDragAngle",
		"DragDetector",
		{ dragDetector },
		{ 4.842f }
	));

	Helper::DoBasicScan(BasicScanConfig<Vector3>(
		"MinDragTranslation",
		"DragDetector",
		{ dragDetector },
		{ Vector3(118.83f, 119.83f, 110.83f) }
	));

	Helper::DoBasicScan(BasicScanConfig<string>(
		"ActivatedCursorIcon",
		"DragDetector",
		{ dragDetector },
		{ "rbxassetid://73879578225091" }
	));

	Helper::DoBasicScan(BasicScanConfig<string>(
		"CursorIcon",
		"DragDetector",
		{ dragDetector },
		{ "rbxassetid://73879578225090" }
	));

	Helper::DoBasicScan(BasicScanConfig<float>(
		"MaxForce",
		"DragDetector",
		{ dragDetector },
		{ 7452.386f }
	));

	Helper::DoBasicScan(BasicScanConfig<float>(
		"MaxTorque",
		"DragDetector",
		{ dragDetector },
		{ 236.872f }
	));

	Helper::DoBasicScan(BasicScanConfig<float>(
		"Responsiveness",
		"DragDetector",
		{ dragDetector },
		{ 881.415f }
	));
}
