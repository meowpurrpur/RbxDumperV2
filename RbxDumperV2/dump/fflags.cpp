// shout out to https://github.com/NtReadVirtualMemory/Roblox-FFlag-Dumper all credit to them ♥

#include "../globals.h"

namespace offsets {
	uintptr_t fflagList = 0x0;
	uintptr_t valueGetSet = 0x30;
	uintptr_t flagToValue = 0x0;
}

void Dump::FFlags() {
	bool foundMap = false;
	uintptr_t scanStart = 0x7000000;
	uintptr_t scanEnd = 0x9000000;
	size_t chunkSize = 0x1000;

	for (uintptr_t currentOffset = scanStart; currentOffset < scanEnd; currentOffset += chunkSize) {
		if (foundMap) {
			break;
		}

		size_t bytesToRead = min(chunkSize, scanEnd - currentOffset);
		std::vector<uint8_t> buffer(bytesToRead);
		Mem::ReadBytes(Globals::baseAddress + currentOffset, buffer.data(), bytesToRead);

		auto* data = reinterpret_cast<uintptr_t*>(buffer.data());
		size_t amountOfPointers = bytesToRead / sizeof(uintptr_t);

		for (size_t i = 0; i < amountOfPointers; i++) {
			uintptr_t maybeMap = data[i];

			if (!maybeMap || maybeMap < 0x100000)
				continue;

			uintptr_t isThisReallyAMapValidation = Mem::Read<uintptr_t>(maybeMap);
			if (!isThisReallyAMapValidation || isThisReallyAMapValidation < 0x10000)
				continue;

			if (isThisReallyAMapValidation == 0x3F800000) {
				uintptr_t wow = currentOffset + (i * sizeof(uintptr_t));

				// Yes this is a Map
				uintptr_t mapStart = Mem::Read<uintptr_t>(maybeMap + 0x8);
				uintptr_t mapEnd = Mem::Read<uintptr_t>(mapStart + 0x8);
				uintptr_t current = Mem::Read<uintptr_t>(mapStart);

				if (mapStart < 0x10000 || mapStart > 0x00007FFFFFFFFFFF) {
					continue;
				}

				if (current < 0x10000 || current > 0x00007FFFFFFFFFFF) {
					continue;
				}

				while (current != 0 && current != mapEnd) {
					std::string name = Mem::ReadString(current + 0x10);
					if (name == "BatchThumbnailMinWaitMs") {
						uintptr_t valueGetSet = Mem::Read<uintptr_t>(current + offsets::valueGetSet);
						for (uintptr_t pointerToValueOffset = 0x0; pointerToValueOffset < 0x300; pointerToValueOffset = pointerToValueOffset + 0x8) {
							uintptr_t pointerToValue = Mem::Read<uintptr_t>(valueGetSet + pointerToValueOffset);
							int value = Mem::Read<int>(pointerToValue);
							if (value == 15) {
								foundMap = true;
								offsets::fflagList = wow;
								offsets::flagToValue = pointerToValueOffset;
								goto flagOffsetFound;
							}
						}
					}

					uintptr_t newCurrent = Mem::Read<uintptr_t>(current);
					if (newCurrent < 0x10000 || newCurrent > 0x00007FFFFFFFFFFF) {
						break;
					}
					if (current == newCurrent) {
						break;
					}
					current = newCurrent;
				}
			}
		}
	}

flagOffsetFound:

	Globals::fflagOffsets.add("Pointer", "FFlagList", offsets::fflagList);
	Globals::fflagOffsets.add("ToFlag", "FFlagList", offsets::valueGetSet);
	Globals::fflagOffsets.add("ToValue", "FFlagList", offsets::flagToValue);

	uintptr_t fflagPointer1 = Mem::Read<uintptr_t>(Globals::baseAddress + offsets::fflagList);
	uintptr_t fflagList = Mem::Read<uintptr_t>(fflagPointer1 + 0x8);

	uintptr_t last = Mem::Read<uintptr_t>(fflagList + 0x8);
	uintptr_t current = fflagList;

	while (current != 0 && current != last)
	{
		std::string name = Mem::ReadString(current + 0x10);
		uintptr_t valueGetSet = Mem::Read<uintptr_t>(current + offsets::valueGetSet);

		if (valueGetSet < 0x10000 || valueGetSet > 0x00007FFFFFFFFFFF) {
			current = Mem::Read<uintptr_t>(current);
			continue;
		}

		std::string mhm = Mem::ReadString(valueGetSet + offsets::flagToValue);

		if (mhm == "True" || mhm == "False")
		{
			// gg
		}
		else
		{
			uintptr_t offset = Mem::Read<uintptr_t>(valueGetSet + offsets::flagToValue) - Globals::baseAddress;

			if (offset < 0x10000 || offset > 0x00007FFFFFFFFFFF) {
				current = Mem::Read<uintptr_t>(current);
				continue;
			}

			for (auto& c : name)
			{
				if (!isalnum(static_cast<unsigned char>(c)))
					c = '_';
			}

			Globals::fflagOffsets.addSilent(name, "FFlags", offset);
		}

		current = Mem::Read<uintptr_t>(current);
	}
}
