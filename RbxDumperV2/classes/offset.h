#pragma once

namespace RbxDumper
{
	class Offset
	{
	public:
		string name;
		string catagory;
		string type;
		uintptr_t value;

		Offset(string name, string catagory, uintptr_t value)
		{
			this->name = name;
			this->catagory = catagory;
			this->value = value;
			this->type = "unknown";
		}

		Offset(string name, string catagory, uintptr_t value, string type)
		{
			this->name = name;
			this->catagory = catagory;
			this->value = value;
			this->type = type;
		}

		string ToString() const;
		string ToHeader() const;
		string ToCS() const;
		void Print() const;

		string toString() const { return ToString(); }
		string toHeader() const { return ToHeader(); }
		string toCS() const { return ToCS(); }
		void print() const { Print(); }
	};

	enum FileType {
		HPP,
		CS,
		JSON,
		TXT,
		TYPESJSON,
		STRUCTHPP
	};

	class OffsetList
	{
	public:
		string namespaceName;
		vector<Offset> offsets;

		void Add(Offset offset, bool silent = false);
		void Add(string name, string catagory, uintptr_t value);
		void Add(string name, string catagory, uintptr_t value, string type);
		void AddSilent(string name, string catagory, uintptr_t value);
		Offset Get(string name, string catagory = "");

		void DumpToFile(string path, FileType type, bool hex = true) const;

		void add(Offset offset, bool silent = false) { Add(offset, silent); }
		void add(string name, string catagory, uintptr_t value) { Add(name, catagory, value); }
		void add(string name, string catagory, uintptr_t value, string type) { Add(name, catagory, value, type); }
		void addSilent(string name, string catagory, uintptr_t value) { AddSilent(name, catagory, value); }
		Offset get(string name, string catagory = "") { return Get(name, catagory); }
		void dumpToFile(string path, FileType type, bool hex = true) const { DumpToFile(path, type, hex); }

		OffsetList(string namespaceName) {
			this->namespaceName = namespaceName;
		}
	};
}
