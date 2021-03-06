// mycp.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <windows.h>
#include <stdlib.h>

void cp(LPWSTR s, LPWSTR t)
{
	WIN32_FIND_DATA fd,tfd;
	HANDLE fh = FindFirstFile(s, &fd);

	wcscpy(tfd.cFileName, fd.cFileName);
	tfd.ftCreationTime = fd.ftCreationTime;
	tfd.ftLastAccessTime = fd.ftLastAccessTime;
	tfd.ftLastWriteTime = fd.ftLastWriteTime;
	
	DWORD attr = GetFileAttributes(s);

	if (fd.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY)
	{
		WCHAR CurDir[200];
		GetCurrentDirectory(200, CurDir);

		if (!SetCurrentDirectory(t))
		{
			CreateDirectory(t, NULL);
			SetCurrentDirectory(t);
		}
		
		CreateDirectory(fd.cFileName, NULL);

		SetCurrentDirectory(CurDir);
		SetCurrentDirectory(fd.cFileName);

		WCHAR source[200], target[200], nsource[200];
		wcscpy(source, s);
		wcscpy(target, t);
		wcscat(source, L"\\*.*");
		wcscat(target, L"\\");
		wcscat(target, fd.cFileName);

		HANDLE sfh = FindFirstFile(source, &fd);
		//cp(fd.cFileName, target);

		while (FindNextFile(sfh, &fd))
		{
			if (fd.cFileName[0] == '.')
				continue;
			wcscpy(nsource, s);
			wcscat(nsource, L"\\");
			wcscat(nsource, fd.cFileName);
			cp(nsource, target);
		}
		
		SetCurrentDirectory(t);
		HANDLE dir = CreateFile(tfd.cFileName, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);
		//SetFileAttributes()
		SetFileTime(dir, &tfd.ftCreationTime, &tfd.ftLastAccessTime, &tfd.ftLastWriteTime);
		
		SetFileAttributes(tfd.cFileName, attr);
		CloseHandle(dir);

		FindClose(sfh);

	}
	else
	{
		HANDLE fs = CreateFile(
			fd.cFileName,
			GENERIC_READ,
			FILE_SHARE_READ,
			NULL,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,
			NULL
		);

		if (!SetCurrentDirectory(t))
		{
			CreateDirectory(t, NULL);
			SetCurrentDirectory(t);
		}

		HANDLE nfs = CreateFile(
			fd.cFileName,
			GENERIC_WRITE,
			FILE_SHARE_WRITE,
			NULL,
			CREATE_ALWAYS,
			FILE_ATTRIBUTE_NORMAL,
			NULL
		);

		DWORD rlen, wlen;
		char *buffer;
		buffer = (char*)malloc((fd.nFileSizeHigh * (MAXDWORD + 1)) + fd.nFileSizeLow) + 1;
		ReadFile(fs, buffer, (fd.nFileSizeHigh * (MAXDWORD + 1)) + fd.nFileSizeLow, &rlen, NULL);
		WriteFile(nfs, buffer, rlen, &wlen, NULL);

		SetFileAttributes(fd.cFileName, attr);
		SetFileTime(nfs, &fd.ftCreationTime, &fd.ftLastAccessTime, &fd.ftLastWriteTime);
		
		CloseHandle(fs);
		CloseHandle(nfs);
	}

	
	

	FindClose(fh);
}


int main(int argc, char **argv)
{
	size_t len1 = strlen(argv[1]) + 1;
	size_t converted1 = 0;
	WCHAR source[200];
	mbstowcs_s(&converted1, source, len1, argv[1], _TRUNCATE);

	size_t len2 = strlen(argv[2]) + 1;
	size_t converted2 = 0;
	WCHAR target[200];
	mbstowcs_s(&converted2, target, len2, argv[2], _TRUNCATE);

	
	WCHAR SourceDir[200], TargetDir[200];
	if (argv[1][1] != ':')
	{
		GetCurrentDirectory(200, SourceDir);
		wcscat(SourceDir, L"\\");
		wcscat(SourceDir, source);
	}
	if (argv[2][1] != ':')
	{
		GetCurrentDirectory(200, TargetDir);
		wcscat(TargetDir, L"\\");
		wcscat(TargetDir, target);
	}

	cp(SourceDir, TargetDir);
	
}

