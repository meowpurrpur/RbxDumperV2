#include "../globals.h"

void Dump::UI() {
	uintptr_t players = Roblox::FindFirstChild(Globals::dataModel, "Players");
	uintptr_t localPlayer = Roblox::GetChildren(players)[0];
	uintptr_t playerGui = Roblox::FindFirstChild(localPlayer, "PlayerGui");

	uintptr_t freecam = Roblox::FindFirstChild(playerGui, "Freecam");
	uintptr_t enabledUi = Roblox::FindFirstChild(playerGui, "Enabled");
	uintptr_t disabledUi = Roblox::FindFirstChild(playerGui, "NotEnabled");

	Helper::DoBasicScan(BasicScanConfig<bool>(
		"ScreenGui_Enabled",
		"GuiObject",
		{ freecam, enabledUi, disabledUi },
		{ true, true, false },
		0x300
	));

	uintptr_t testFrame1 = Roblox::FindFirstChild(enabledUi, "Frame");

	Helper::DoBasicScan(BasicScanConfig<UDim2>(
		"Position",
		"GuiObject",
		{ testFrame1 },
		{ UDim2{ {0.144f, 27}, {0.122f, 87} } },
		0x0
	));

	Helper::DoBasicScan(BasicScanConfig<UDim2>(
		"Size",
		"GuiObject",
		{ testFrame1 },
		{ UDim2{ {0.433f, 100}, {0.211f, 100} } },
		0x0
	));

	uintptr_t visibleFrame = Roblox::FindFirstChild(enabledUi, "Visible");
	uintptr_t invisibleFrame = Roblox::FindFirstChild(enabledUi, "NotVisible");

	Helper::DoBasicScan(BasicScanConfig<bool>(
		"Visible",
		"GuiObject",
		{ visibleFrame, testFrame1, invisibleFrame },
		{ true, true, false },
		0x400
	));

	uintptr_t imageLabel = Roblox::FindFirstChild(enabledUi, "ImageLabel");

	Helper::DoBasicScan(BasicScanConfig<string>(
		"Image",
		"GuiObject",
		{ imageLabel },
		{ "rbxassetid://73879578225090" }
	));

	uintptr_t textLabel = Roblox::FindFirstChild(enabledUi, "TextLabel");

	Helper::DoBasicScan(BasicScanConfig<string>(
		"Text",
		"GuiObject",
		{ textLabel },
		{ "RbxDumper" }
	));

	Helper::DoBasicScan(BasicScanConfig<string>(
		"RichText",
		"GuiObject",
		{ textLabel },
		{ "<b>RbxDumper</b>" }
	));

	Helper::DoBasicScan(BasicScanConfig<Color3>(
		"BackgroundColor3",
		"GuiObject",
		{ visibleFrame },
		{ Color3{ 0.752941f, 0.752941f, 0.752941f } },
		0x0,
		0x1000,
		0x1,
		false,
		0.01f
	));

	Helper::DoBasicScan(BasicScanConfig<Color3>(
		"BorderColor3",
		"GuiObject",
		{ testFrame1 },
		{ Color3{ 0.752941f, 0.752941f, 0.752941f } },
		0x0,
		0x1000,
		0x1,
		false,
		0.01f
	));

	Helper::DoBasicScan(BasicScanConfig<Color3>(
		"TextColor3",
		"GuiObject",
		{ textLabel },
		{ Color3{ 0.752941f, 0.752941f, 0.752941f } },
		0x0,
		0x1000,
		0x1,
		false,
		0.01f
	));

	Helper::DoBasicScan(BasicScanConfig<int>(
		"LayoutOrder",
		"GuiObject",
		{ textLabel },
		{ -286 }
	));

	Helper::DoBasicScan(BasicScanConfig<int>(
		"ZIndex",
		"GuiObject",
		{ testFrame1 },
		{ 67 }
	));

	Helper::DoBasicScan(BasicScanConfig<float>(
		"BackgroundTransparency",
		"GuiObject",
		{ testFrame1 },
		{ 0.752f }
	));

	uintptr_t rotationOff = Helper::DoBasicScan(BasicScanConfig<float>(
		"Rotation",
		"GuiObject",
		{ textLabel },
		{ 9.72f },
		0x0,
		0x1000,
		0x1,
		false,
		0.01f
	)).value;

	Helper::DoBasicScan(BasicScanConfig<Vector2>(
		"AbsoluteSize",
		"GuiBase2D",
		{ testFrame1 },
		{ Vector2(931.360f, 315.642f) }
	));

	Helper::DoBasicScan(BasicScanConfig<Vector2>(
		"AbsolutePosition",
		"GuiBase2D",
		{ testFrame1 },
		{ Vector2(303.480f, 269.684f) }
	));

	Globals::offsets.add("AbsoluteRotation", "GuiBase2D", rotationOff, "float");

	uintptr_t textBox = Roblox::FindFirstChild(enabledUi, "TextBox");
	uintptr_t uis = Roblox::FindFirstChild(Globals::dataModel, "UserInputService");

	bool found = false;
	for (int i = 0x200; i < 0x400; i++)
	{
		uintptr_t wis = Mem::Read<uintptr_t>(uis + i);
		for (int j = 0x20; j < 0x50; j++)
		{
			uintptr_t focused = Mem::Read<uintptr_t>(wis + j);
			if (focused == textBox)
			{
				Globals::offsets.add("WindowInputState", "UserInputService", i, "unsigned __int64");
				Globals::offsets.add("CurrentTextBox", "WindowInputState", j, "unsigned __int64");
				found = true;
				break;
			}
		}

		if (found) break;
	}

	uintptr_t currentText = Globals::offsets.get("CurrentTextBox").value;
	Globals::offsets.add("CapsLock", "WindowInputState", currentText - 0x8, "bool");

	uintptr_t workspace = Roblox::FindFirstChild(Globals::dataModel, "Workspace");
	uintptr_t decal = Roblox::FindFirstChild(workspace, "Decal");

	Helper::DoBasicScan(BasicScanConfig<string>(
		"Decal_Texture",
		"Textures",
		{ decal },
		{ "rbxassetid://73879578225090" }
	));

	uintptr_t texture = Roblox::FindFirstChild(workspace, "Texture");

	Helper::DoBasicScan(BasicScanConfig<string>(
		"Texture_Texture",
		"Textures",
		{ texture },
		{ "rbxassetid://73879578225090" }
	));
}
