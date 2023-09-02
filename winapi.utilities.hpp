#ifndef UTILITIES_WINAPI_UTITLITIES_HPP
#define UTILITIES_WINAPI_UTITLITIES_HPP

#include <Windows.h>

#include <string>
#include <cstdio>
#include <tchar.h>
#include <cstring>
#include <Psapi.h>
#include <strsafe.h>
#include <tlhelp32.h>

#include "string.hpp"

namespace Utilities
{
	constexpr auto BufSize = 512;

	inline std::string GetFileNameFromHandle(HANDLE const hFile)
	{
		 std::string pszFileName(MAX_PATH, '\0');
		
		BOOL bSuccess = FALSE;

		// Get the file size.
		DWORD dwFileSizeHi = 0;
		const DWORD dwFileSizeLo = GetFileSize(hFile, &dwFileSizeHi);

		if (dwFileSizeLo == 0 && dwFileSizeHi == 0)		
			throw;
		

		// Create a file mapping object.
		const HANDLE hFileMap = CreateFileMapping(hFile,
		                                          NULL,
		                                          PAGE_READONLY,
		                                          0,
		                                          1,
		                                          NULL);

		if (hFileMap)
		{
			// Create a file mapping to get the file name.
			void* pMem = MapViewOfFile(hFileMap, FILE_MAP_READ, 0, 0, 1);

			if (pMem)
			{
				if (GetMappedFileName(GetCurrentProcess(),
				                      pMem,
				                      pszFileName.data(),
				                      MAX_PATH))
				{
					// Translate path with device name to drive letters.
					TCHAR szTemp[BufSize];
					szTemp[0] = '\0';

					if (GetLogicalDriveStrings(BufSize - 1, szTemp))
					{
						TCHAR szName[MAX_PATH];
						TCHAR szDrive[3] = TEXT(" :");
						BOOL bFound = FALSE;
						TCHAR* p = szTemp;

						do
						{
							// Copy the drive letter to the template string
							*szDrive = *p;

							// Look up each device name
							if (QueryDosDevice(szDrive, szName, MAX_PATH))
							{
								size_t const uNameLen = _tcslen(szName);

								if (uNameLen < MAX_PATH)
								{
									bFound = _tcsnicmp(pszFileName.data(), szName, uNameLen) == 0
										&& *(pszFileName.data() + uNameLen) == _T('\\');

									if (bFound)
									{
										// Reconstruct pszFilename using szTempFile
										// Replace device path with DOS path
										TCHAR szTempFile[MAX_PATH];
										StringCchPrintf(szTempFile,
										                MAX_PATH,
										                TEXT("%s%s"),
										                szDrive,
										                pszFileName.data() + uNameLen);
										StringCchCopyN(pszFileName.data(), MAX_PATH + 1, szTempFile,
										               _tcslen(szTempFile));
									}
								}
							}

							// Go to the next NULL character.
							while (*p++);
						}
						while (!bFound && *p); // end of string
					}
				}
				bSuccess = TRUE;
				UnmapViewOfFile(pMem);
			}

			CloseHandle(hFileMap);
		}

		if (bSuccess)
		{
			pszFileName.resize(strlen(pszFileName.data()));
			return pszFileName;
		}
		throw;
	}

	struct FileVersionInformation
	{
		struct fvi_load_error : std::exception {};
		
		bool				Loaded				= false;
		std::string		Comments;
		std::string		CompanyName;
		std::string		FileDescription;
		std::string		FileVersion;
		std::string		InternalName;
		std::string		LegalCopyright;
		std::string		LegalTrademarks;
		std::string		OriginalFilename;
		std::string		PrivateBuild;
		std::string		ProductName;
		std::string		ProductVersion;
		std::string		SpecialBuild;

		void Load(std::string fileName)
		{
			std::string			vfiTranslation	= "\\VarFileInfo\\Translation";
			std::string			sfiPattern		= "\\StringfileInfo\\%04x%04x\\%s";

			OFSTRUCT			uselessOF;
			auto					l					= [&](HANDLE* p) { if (p && *p) CloseHandle(*p); };
			auto					h					= (HANDLE)OpenFile(fileName.c_str(), &uselessOF, OF_READ);
			std::unique_ptr<HANDLE, decltype(l)> file{ &h, l };
			
			DWORD				useless;
			DWORD				sz[2]; 
									sz[0]				= GetFileSize(file.get(), &sz[1]);
			if(sz[0] == 0 && sz[1] == 0)
				throw fvi_load_error{};
			size_t				size				= sz[0];
			size_t				bufferSize		= GetFileVersionInfoSize(fileName.c_str(), &useless);
			if (bufferSize == 0)
				throw fvi_load_error{};
			std::string			buffer; buffer.resize(bufferSize);
			if (!GetFileVersionInfo(fileName.c_str(), NULL, buffer.size(), buffer.data()))
				throw fvi_load_error{};
			uint16_t*			langInfo;
			uint32_t			cbLang;
			if (!VerQueryValue(buffer.data(), vfiTranslation.c_str(), (LPVOID*) &langInfo, &cbLang))
				throw fvi_load_error{};

			std::map<std::string, std::string&> properties
			{
				{ "Comments"				,	Comments				},
				{ "CompanyName"			,	CompanyName		},
				{ "FileDescription"			,	FileDescription		},
				{ "FileVersion"				,	FileVersion				},
				{ "InternalName"			,	InternalName			},
				{ "LegalCopyright"			,	LegalCopyright		},
				{ "LegalTrademarks"		,	LegalTrademarks	},
				{ "OriginalFilename"		,	OriginalFilename		},
				{ "PrivateBuild"				,	PrivateBuild			},
				{ "ProductName"			,	ProductName			},
				{ "ProductVersion"			,	ProductVersion		},
				{ "SpecialBuild"				,	SpecialBuild			},
			};

			for (auto& p : properties)
			{
				LPVOID			nameBuffer;
				unsigned int	nameBufferSize;
				std::string		sfi					= string_format(sfiPattern, langInfo[0], langInfo[1], p.first.c_str());
				if (!VerQueryValue(buffer.data(), sfi.c_str(), &nameBuffer, &nameBufferSize))
					continue;
				p.second.resize(nameBufferSize);
				memcpy(p.second.data(), nameBuffer, nameBufferSize);
				p.second.resize(strlen(p.second.data()));
			}
			Loaded	= true;
		}
	};
}
#endif //UTILITIES_WINAPI_UTITLITIES_HPP