#ifndef UTILITIES_PORTABLE_EXECUTABLE_HPP
#define UTILITIES_PORTABLE_EXECUTABLE_HPP

#include <Windows.h>

#include <string>
#include <fstream>
#include <cassert>
#include <vector>
#include <variant>

#include "file.hpp"
#include "crc32.hpp"
#include "exceptions.hpp"

namespace PECOFF
{
	using namespace Utilities;

	using GetProcAddressFunction	= FARPROC(__stdcall*)(HMODULE hLibModule, LPCSTR lpProcName);
	using LoadLibraryFunction		= HMODULE(__stdcall*)(LPCSTR lpLibFileName);
	using FreeLibraryFunction		= BOOL(__stdcall*)(HMODULE hLibModule);

	class PortableExecutable final
	{
	public:
		struct file_corrupted_error : public Exceptions::base_error
		{
			file_corrupted_error(std::string function, std::string file, int line) : Exceptions::base_error("File corrupted", function, file, line) {}
			file_corrupted_error(std::string msg, std::string function, std::string file, int line) :Exceptions::base_error(msg, function, file, line) { }
		};
		struct section_not_found_error : public Exceptions::base_error
		{
			std::string const Section;

			section_not_found_error(std::string function, std::string file, int line, std::string_view const& section) : Exceptions::base_error("Section (" + std::string(section) + ") not found", function, file, line), Section(section) {}
			section_not_found_error(std::string msg, std::string function, std::string file, int line, std::string_view const& section) :Exceptions::base_error(msg, function, file, line), Section(section) { }
		};
		struct file_is_not_pe : public Exceptions::base_error
		{
			enum class Type
			{
				IsNotDOS	= 0,
				IsNotPE		= 1,
			};

			Type What;

			file_is_not_pe(std::string function, std::string file, int line, Type what) : Exceptions::base_error("File is not PE", function, file, line), What(what) {}
			file_is_not_pe(std::string msg, std::string function, std::string file, int line, Type what) : Exceptions::base_error(msg, function, file, line), What(what) { }
		};

		struct Thunk
		{
			struct NameSnap
			{
				std::string							Name;
				WORD								W;
			};
			struct OrdinalSnap
			{
				DWORD								Ordinal;
			};

			DWORD									Address;
			IMAGE_THUNK_DATA				Data;

			bool										IsOrdinal;
			std::variant<NameSnap, OrdinalSnap> Snap;
		};
		struct Import
		{
			IMAGE_IMPORT_DESCRIPTOR	Descriptor;
			std::string								Name;
			std::list<Thunk>					Thunks;
		};

		std::list<Import>									Imports;
		std::vector<IMAGE_SECTION_HEADER>	Sections;
		IMAGE_DOS_HEADER								DOSHeader;
		IMAGE_NT_HEADERS								PEHeader;
	private:
	public:
		PortableExecutable() = default;
		PortableExecutable(std::istream& s)
		{
			parse(s, Imports, Sections, DOSHeader, PEHeader);
		}

		IMAGE_SECTION_HEADER const& find_section(std::string name) const
		{
			assert(name.size() <= IMAGE_SIZEOF_SHORT_NAME);

			auto const it = std::find_if(
				Sections.cbegin(), Sections.cend(),
				[&name](IMAGE_SECTION_HEADER const& section)
				{
					return !memcmp(section.Name, name.data(), name.size());
				});

			if (it == Sections.cend())
				throw construct_error_args_no_msg(section_not_found_error, name);
			else return *it;
		}

		static DWORD virtual_to_raw(DWORD const dwAddress, std::vector<IMAGE_SECTION_HEADER>& sections)
		{
			for (auto const& section : sections)
			{
				auto const dwDifference = dwAddress - section.VirtualAddress;
				if (dwDifference < section.SizeOfRawData)
					return section.PointerToRawData + dwDifference;
			}
			return 0;
		}

		static void parse(
			std::istream&												s,
			std::list<Import>&										imports,
			std::vector<IMAGE_SECTION_HEADER>&	sections,
			IMAGE_DOS_HEADER&								dosHeader,
			IMAGE_NT_HEADERS&								peHeader
		) {
			size_t const streamSize	= file_size(s);
			size_t readden				= 0;
			s.clear(); // C++11 will automatically clear EOF bit when seekg calls, but C++17 and above willn't.

			// dos header
			s.seekg(0, std::istream::beg);
			s.read((char*) &dosHeader, sizeof(IMAGE_DOS_HEADER));
			readden = s.gcount();
			if (readden != sizeof(IMAGE_DOS_HEADER))
				throw construct_error_no_msg(file_corrupted_error);
			if (dosHeader.e_magic != IMAGE_DOS_SIGNATURE)
				throw construct_error_args_no_msg(file_is_not_pe, file_is_not_pe::Type::IsNotDOS);

			// pe header
			s.seekg(dosHeader.e_lfanew, std::istream::beg);
			s.read((char*) &peHeader, sizeof(IMAGE_NT_HEADERS));
			readden = s.gcount();
			if (readden != sizeof(IMAGE_NT_HEADERS))
				throw construct_error_no_msg(file_corrupted_error);
			if (peHeader.Signature != IMAGE_NT_SIGNATURE)
				throw construct_error_args_no_msg(file_is_not_pe, file_is_not_pe::Type::IsNotPE);

			// sections
			sections.resize(peHeader.FileHeader.NumberOfSections);
			s.read((char*) sections.data(), sections.size() * sizeof(IMAGE_SECTION_HEADER));
			readden = s.gcount();
			if (readden != sizeof(IMAGE_SECTION_HEADER) * sections.size())
				throw construct_error_no_msg(file_corrupted_error);

			// imports
			auto const& peImports = peHeader.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];
			if (peImports.Size)
			{
				s.seekg(virtual_to_raw(peImports.VirtualAddress, sections), std::istream::beg);

				for (;;)
				{
					IMAGE_IMPORT_DESCRIPTOR importDescriptor{};
					s.read((char*) &importDescriptor, sizeof(IMAGE_IMPORT_DESCRIPTOR));
					size_t readden = s.gcount();

					if (readden != sizeof(IMAGE_IMPORT_DESCRIPTOR))
						throw construct_error_no_msg(file_corrupted_error);

					if (!importDescriptor.Characteristics)
						break;

					Import& item		= imports.emplace_back();
					item.Descriptor	= importDescriptor;
				}
			}

			size_t const stringSize = 0x100;
			for (Import& item : imports)
			{
				item.Name.reserve(stringSize);
				s.seekg(virtual_to_raw(item.Descriptor.Name, sections), std::istream::beg);
				s.getline(item.Name.data(), stringSize); s.clear();

				// thunks
				s.seekg(virtual_to_raw(item.Descriptor.FirstThunk, sections), std::istream::beg);

				for (;;)
				{
					Thunk thunk{};
					s.read((char*) &thunk.Data, sizeof(IMAGE_THUNK_DATA));
					size_t readden = s.gcount();

					if (readden != sizeof(IMAGE_THUNK_DATA))
						throw construct_error_no_msg(file_corrupted_error);

					if (!thunk.Data.u1.AddressOfData)
						break;

					item.Thunks.push_back(thunk);					
				}

				auto thunkAddress = reinterpret_cast<IMAGE_THUNK_DATA*>(item.Descriptor.FirstThunk);
				for (auto& thunk : item.Thunks)
				{
					thunk.Address							= reinterpret_cast<DWORD>(thunkAddress++);
					thunk.IsOrdinal							= IMAGE_SNAP_BY_ORDINAL(thunk.Data.u1.Ordinal);

					if (thunk.IsOrdinal)
					{
						Thunk::OrdinalSnap&	snap		= thunk.Snap.emplace<Thunk::OrdinalSnap>();
						snap.Ordinal							= IMAGE_ORDINAL(thunk.Data.u1.Ordinal);
					}
					else
					{
						Thunk::NameSnap&	snap		= thunk.Snap.emplace<Thunk::NameSnap>();
						s.seekg(virtual_to_raw(thunk.Data.u1.AddressOfData, sections), std::istream::beg);
						s.read((char*) &snap.W, sizeof(WORD));
						snap.Name.reserve(stringSize);
						s.getline(snap.Name.data(), stringSize); s.clear();
					}
				}
			}
		}

		static bool read_bytes(std::istream& s, DWORD const rawAddress, size_t const size, void* const dest)
		{
			s.clear(); // clear EOF bit.
			s.seekg(rawAddress, std::istream::beg);
			s.read((char*) dest, size);
			size_t readden = s.gcount();
			return readden == size;
		}
		static bool read_cstring(std::istream& s, DWORD const rawAddress, std::string& result, size_t const length = 0x100)
		{
			result.resize(length);
			s.clear(); // clear EOF bit.
			s.seekg(rawAddress, std::istream::beg);
			s.read(result.data(), length);
			size_t readden = s.gcount();
			bool ok = readden == length;
			if (ok)
				result.resize(strlen(result.data()));
			return ok;
		}
	};

#pragma pack(push, 1)
	struct Kernel32
	{
		static constexpr const char*	GetProcAddressFunctionImport	= "GETPROCADDRESS";
		static constexpr const char*	LoadLibraryAFunctionImport		= "LOADLIBRARYA";
		static constexpr const char*	FreeLibraryFunctionImport			= "FREELIBRARY";

		GetProcAddressFunction			GetProcAddressFunc		= nullptr;
		LoadLibraryFunction				LoadLibraryFunc				= nullptr;
		FreeLibraryFunction				FreeLibraryFunc				= nullptr;

		Kernel32() = default;
		Kernel32(PortableExecutable& pe)
		{
			auto const dwImageBase = pe.PEHeader.OptionalHeader.ImageBase;

			for (auto const& import : pe.Imports)
			{
				if (_strcmpi(import.Name.c_str(), "KERNEL32.DLL") == 0)
				{					
					for (auto const& thunk : import.Thunks)
					{
						if (_strcmpi(std::get<PortableExecutable::Thunk::NameSnap>(thunk.Snap).Name.c_str(), GetProcAddressFunctionImport) == 0)
							GetProcAddressFunc = GetProcAddressFunction(dwImageBase + thunk.Address);
						else if (_strcmpi(std::get<PortableExecutable::Thunk::NameSnap>(thunk.Snap).Name.c_str(), LoadLibraryAFunctionImport) == 0)
							LoadLibraryFunc = LoadLibraryFunction(dwImageBase + thunk.Address);
						else if (_strcmpi(std::get<PortableExecutable::Thunk::NameSnap>(thunk.Snap).Name.c_str(), FreeLibraryFunctionImport) == 0)
							FreeLibraryFunc = FreeLibraryFunction(dwImageBase + thunk.Address);
					}
				}
			}
		}

		bool is_injectable() const
		{
			return
				LoadLibraryFunc != nullptr && 
				GetProcAddressFunc != nullptr && 
				FreeLibraryFunc != nullptr;
		}
	};
#pragma pack(pop)
}

#endif //UTILITIES_PORTABLE_EXECUTABLE_HPP