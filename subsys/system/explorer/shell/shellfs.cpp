/*
 * Copyright 2003 Martin Fuchs
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */


 //
 // Explorer clone
 //
 // shellfs.cpp
 //
 // Martin Fuchs, 23.07.2003
 //


#include "../utility/utility.h"
#include "../utility/shellclasses.h"

#include "../globals.h"

#include "entries.h"
#include "shellfs.h"


bool ShellDirectory::fill_w32fdata_shell(LPCITEMIDLIST pidl, SFGAOF attribs, WIN32_FIND_DATA* pw32fdata, BY_HANDLE_FILE_INFORMATION* pbhfi)
{
	bool bhfi_valid = false;

	if (!( (attribs & SFGAO_FILESYSTEM) && SUCCEEDED(
				SHGetDataFromIDList(_folder, pidl, SHGDFIL_FINDDATA, pw32fdata, sizeof(WIN32_FIND_DATA))) )) {
		WIN32_FILE_ATTRIBUTE_DATA fad;
		IDataObject* pDataObj;

		STGMEDIUM medium = {0, {0}, 0};
		FORMATETC fmt = {g_Globals._cfStrFName, 0, DVASPECT_CONTENT, -1, TYMED_HGLOBAL};

		HRESULT hr = _folder->GetUIObjectOf(0, 1, &pidl, IID_IDataObject, 0, (LPVOID*)&pDataObj);

		if (SUCCEEDED(hr)) {
			hr = pDataObj->GetData(&fmt, &medium);

			pDataObj->Release();

			if (SUCCEEDED(hr)) {
				LPCTSTR path = (LPCTSTR)GlobalLock(medium.UNION_MEMBER(hGlobal));
				UINT sem_org = SetErrorMode(SEM_FAILCRITICALERRORS);

				if (GetFileAttributesEx(path, GetFileExInfoStandard, &fad)) {
					pw32fdata->dwFileAttributes = fad.dwFileAttributes;
					pw32fdata->ftCreationTime = fad.ftCreationTime;
					pw32fdata->ftLastAccessTime = fad.ftLastAccessTime;
					pw32fdata->ftLastWriteTime = fad.ftLastWriteTime;

					if (!(fad.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
						pw32fdata->nFileSizeLow = fad.nFileSizeLow;
						pw32fdata->nFileSizeHigh = fad.nFileSizeHigh;
					}
				}

				HANDLE hFile = CreateFile(path, GENERIC_READ, FILE_SHARE_READ|FILE_SHARE_WRITE|FILE_SHARE_DELETE,
											0, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, 0);

				if (hFile != INVALID_HANDLE_VALUE) {
					if (GetFileInformationByHandle(hFile, pbhfi))
						bhfi_valid = true;

					CloseHandle(hFile);
				}

				SetErrorMode(sem_org);

				GlobalUnlock(medium.UNION_MEMBER(hGlobal));
				GlobalFree(medium.UNION_MEMBER(hGlobal));
			}
		}
	}

	if (!(attribs & SFGAO_FILESYSTEM))	// Archiv files should not be displayed as folders in explorer view.
		if (attribs & (SFGAO_FOLDER|SFGAO_HASSUBFOLDER))
			pw32fdata->dwFileAttributes |= FILE_ATTRIBUTE_DIRECTORY;

	if (attribs & SFGAO_READONLY)
		pw32fdata->dwFileAttributes |= FILE_ATTRIBUTE_READONLY;

	if (attribs & SFGAO_COMPRESSED)
		pw32fdata->dwFileAttributes |= FILE_ATTRIBUTE_COMPRESSED;

	return bhfi_valid;
}


LPITEMIDLIST ShellEntry::create_absolute_pidl(HWND hwnd)
{
	if (_up/* && _up->_etype==ET_SHELL*/) {
		LPITEMIDLIST pidl = _pidl.create_absolute_pidl(static_cast<ShellDirectory*>(_up)->_folder, hwnd);

		if (pidl)
			return pidl;
	}

	return &*_pidl;
}


 // get full path of a shell entry
void ShellEntry::get_path(PTSTR path) const
{
	path[0] = TEXT('\0');

	HRESULT hr = path_from_pidl(get_parent_folder(), &*_pidl, path, MAX_PATH);
}


 // get full path of a shell folder
void ShellDirectory::get_path(PTSTR path) const
{
	path[0] = TEXT('\0');

	SFGAOF attribs = 0;
	HRESULT hr = S_OK;

	if (!_folder.empty())
		hr = const_cast<ShellFolder&>(_folder)->GetAttributesOf(1, (LPCITEMIDLIST*)&_pidl, &attribs);

	if (SUCCEEDED(hr) && (attribs&SFGAO_FILESYSTEM))
		hr = path_from_pidl(get_parent_folder(), &*_pidl, path, MAX_PATH);
}


BOOL ShellEntry::launch_entry(HWND hwnd, UINT nCmdShow)
{
	BOOL ret = TRUE;

	SHELLEXECUTEINFO shexinfo;

	shexinfo.cbSize = sizeof(SHELLEXECUTEINFO);
	shexinfo.fMask = SEE_MASK_IDLIST;
	shexinfo.hwnd = hwnd;
	shexinfo.lpVerb = NULL;
	shexinfo.lpFile = NULL;
	shexinfo.lpParameters = NULL;
	shexinfo.lpDirectory = NULL;
	shexinfo.nShow = nCmdShow;
	shexinfo.lpIDList = create_absolute_pidl(hwnd);

	if (!ShellExecuteEx(&shexinfo)) {
		display_error(hwnd, GetLastError());
		ret = FALSE;
	}

	if (shexinfo.lpIDList != &*_pidl)
		ShellMalloc()->Free(shexinfo.lpIDList);

	return ret;
}


static HICON extract_icon(IShellFolder* folder, LPCITEMIDLIST pidl)
{
	IExtractIcon* pExtract;

	if (SUCCEEDED(folder->GetUIObjectOf(0, 1, (LPCITEMIDLIST*)&pidl, IID_IExtractIcon, 0, (LPVOID*)&pExtract))) {
		TCHAR path[_MAX_PATH];
		unsigned flags;
		HICON hicon;
		int idx;

		if (SUCCEEDED(pExtract->GetIconLocation(GIL_FORSHELL, path, _MAX_PATH, &idx, &flags))) {
			if (!(flags & GIL_NOTFILENAME)) {
				if (idx == -1)
					idx = 0;	// special case for some control panel applications

				if ((int)ExtractIconEx(path, idx, 0, &hicon, 1) > 0)
					flags &= ~GIL_DONTCACHE;
			} else {
				HICON hIconLarge = 0;

				HRESULT hr = pExtract->Extract(path, idx, &hIconLarge, &hicon, MAKELONG(0/*GetSystemMetrics(SM_CXICON)*/,GetSystemMetrics(SM_CXSMICON)));

				if (SUCCEEDED(hr))
					DestroyIcon(hIconLarge);
			}

			return hicon;
		}
	}

	return 0;
}


void ShellDirectory::read_directory()
{
	int level = _level + 1;

	Entry* first_entry = NULL;
	Entry* last = NULL;

	/*if (_folder.empty())
		return;*/

	ShellItemEnumerator enumerator(_folder, SHCONTF_FOLDERS|SHCONTF_NONFOLDERS|SHCONTF_INCLUDEHIDDEN|SHCONTF_SHAREABLE|SHCONTF_STORAGE);

	HRESULT hr_next = S_OK;

	do {
#define FETCH_ITEM_COUNT	32
		LPITEMIDLIST pidls[FETCH_ITEM_COUNT];
		ULONG cnt = 0;
		ULONG n;

		memset(pidls, 0, sizeof(pidls));

		hr_next = enumerator->Next(FETCH_ITEM_COUNT, pidls, &cnt);

		/* don't break yet now: Registry Explorer Plugin returns E_FAIL!
		if (!SUCCEEDED(hr_next))
			break; */

		if (hr_next == S_FALSE)
			break;

		for(n=0; n<cnt; ++n) {
			WIN32_FIND_DATA w32fd;
			BY_HANDLE_FILE_INFORMATION bhfi;
			bool bhfi_valid = false;

			memset(&w32fd, 0, sizeof(WIN32_FIND_DATA));

			SFGAOF attribs = ~SFGAO_FILESYSTEM; //SFGAO_HASSUBFOLDER|SFGAO_FOLDER; SFGAO_FILESYSTEM sorgt daf�r, da� "My Documents" anstatt von "Martin's Documents" angezeigt wird
			HRESULT hr = _folder->GetAttributesOf(1, (LPCITEMIDLIST*)&pidls[n], &attribs);

			if (SUCCEEDED(hr)) {
				if (attribs != ~SFGAO_FILESYSTEM)
					bhfi_valid = fill_w32fdata_shell(pidls[n], attribs, &w32fd, &bhfi);
				else
					attribs = 0;
			} else
				attribs = 0;

			Entry* entry;

			if (w32fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				entry = new ShellDirectory(this, pidls[n], _hwnd);
			else
				entry = new ShellEntry(this, pidls[n]);

			if (!first_entry)
				first_entry = entry;

			if (last)
				last->_next = entry;

			memcpy(&entry->_data, &w32fd, sizeof(WIN32_FIND_DATA));

			if (bhfi_valid)
				memcpy(&entry->_bhfi, &bhfi, sizeof(BY_HANDLE_FILE_INFORMATION));

			if (!entry->_data.cFileName[0])
				/*hr = */name_from_pidl(_folder, pidls[n], entry->_data.cFileName, MAX_PATH, SHGDN_INFOLDER|0x2000/*0x2000=SHGDN_INCLUDE_NONFILESYS*/);

			 // get display icons for files and virtual objects
			if (!(entry->_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ||
				!(attribs & SFGAO_FILESYSTEM)) {
				entry->_hicon = extract_icon(_folder, pidls[n]);

				if (!entry->_hicon)
					entry->_hicon = (HICON)-1;	// don't try again later
			}

			entry->_down = NULL;
			entry->_expanded = false;
			entry->_scanned = false;
			entry->_level = level;
			entry->_shell_attribs = attribs;
			entry->_bhfi_valid = bhfi_valid;

			last = entry;
		}
	} while(SUCCEEDED(hr_next));

	if (last)
		last->_next = NULL;

	_down = first_entry;
	_scanned = true;
}

const void* ShellDirectory::get_next_path_component(const void* p)
{
	LPITEMIDLIST pidl = (LPITEMIDLIST)p;

	if (!pidl || !pidl->mkid.cb)
		return NULL;

	 // go to next element
	pidl = (LPITEMIDLIST)((LPBYTE)pidl+pidl->mkid.cb);

	return pidl;
}

Entry* ShellDirectory::find_entry(const void* p)
{
	LPITEMIDLIST pidl = (LPITEMIDLIST) p;

	for(Entry*entry=_down; entry; entry=entry->_next) {
		ShellEntry* e = static_cast<ShellEntry*>(entry);

		if (e->_pidl && e->_pidl->mkid.cb==pidl->mkid.cb && !memcmp(e->_pidl, pidl, e->_pidl->mkid.cb))
			return entry;
	}

	return NULL;
}
