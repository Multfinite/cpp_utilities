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

	inline std::string ExtractOriginalFilename(HANDLE hProcess, HMODULE hModule)
	{
		std::string originalFileName{};
		HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, GetProcessId(hProcess));
		if (hSnap != INVALID_HANDLE_VALUE)
		{
			MODULEENTRY32 moduleEntry{ };
			moduleEntry.dwSize = sizeof(moduleEntry);
			if (Module32First(hSnap, &moduleEntry))
			{
				do
				{
					if (moduleEntry.hModule != hModule)
						continue;

					DWORD _useless;
					DWORD infoBufferSize = GetFileVersionInfoSize(moduleEntry.szModule, &_useless);
					if (infoBufferSize == 0)
						continue;
					std::vector<char> infoBuffer(infoBufferSize);
					if (!GetFileVersionInfo(moduleEntry.szModule, NULL, infoBufferSize, infoBuffer.data()))
						continue;
					LPVOID nameBuffer;
					unsigned int nameBufferSize;
					// 0409 04b0 is language: English (American), codepage: UTF-16
					if (!VerQueryValue(infoBuffer.data(), "\\StringFileInfo\\040904b0\\OriginalFilename", &nameBuffer, &nameBufferSize))
						continue;
					
					originalFileName = (const char*) nameBuffer;
					break;
				} while (Module32Next(hSnap, &moduleEntry));
			}
		}
		CloseHandle(hSnap);
		return originalFileName;
	}
}
#endif //UTILITIES_WINAPI_UTITLITIES_HPP