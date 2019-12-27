#include "stdafx.h"

#include <io.h>
#include <direct.h>
#include <sys/types.h>
#include <sys/stat.h>
//#include "Basic.h"
//#include "util.h"
#include "dirdialog.h"
#include "paths.h"

#define fnsplit(v,w,x,y,z) 	_splitpath(v,w,x,y,z)
#define fnmerge(v,w,x,y,z)  _makepath(v,w,x,y,z)

char	ExePath[_MAX_PATH] = "";

void
InstallExePath(const char* program_pointer) {

	char 		drive[_MAX_DRIVE];
	char 		dir[_MAX_DIR];
	char 		ext[_MAX_EXT], *p;
	char		exefile[_MAX_FNAME];
	const char	*progpath;

	if(NULL == program_pointer)
		progpath = _pgmptr;
	else
		progpath = program_pointer;

	if (!progpath)
		return;

	fnsplit(progpath, drive, dir, exefile, ext);

    // pfad normalisieren z.B. 'c:\and\test\..\' zu 'c:\and\'
    if(p = strstr(dir, "\\..")) {
        *p = 0;
    	if(p = strrchr(dir, '\\'))
           	*(p + 1) = 0;
    }

	fnmerge(ExePath, drive, dir, "", "");
	strupr(ExePath);
}

char *
GetExePath() {

	if(!*ExePath)
		InstallExePath();

	return ExePath;
}

///////////////////////////////////////////////////////////////////////////

DWORD
GetSizeOfFile(const char* filename) {

	struct _stat	st;

	if(0 == _stat(filename, &st)) {
		return st.st_size;
	}
	return 0;
}

long 
GetFileTimeStamp(const char *filename) {

	struct _stat	st;

	if(0 == _stat(filename, &st)) {
		return st.st_mtime;
	}
	return 0;
}

BOOL
FileExist(const char *file)
{
	if ( NULL == file || 0 == *file )
		return FALSE;

	struct stat 	statbuf;

	UINT uiPrevErrMode = SetErrorMode( SEM_NOOPENFILEERRORBOX|SEM_FAILCRITICALERRORS ); 
	int iRet = stat(file, &statbuf);
	SetErrorMode( uiPrevErrMode ); 

	if( 0 == iRet )
        return TRUE;

    return FALSE;
}

BOOL
CopyFileNoRO(LPCSTR lpExistingFileName, LPCSTR lpNewFileName, BOOL bFailIfExists) {

	BOOL ret = CopyFile(lpExistingFileName, lpNewFileName, bFailIfExists);
	
	if(ret) {
		DWORD fa = GetFileAttributes(lpNewFileName);
		if(fa & FILE_ATTRIBUTE_READONLY) {
			fa &= ~FILE_ATTRIBUTE_READONLY;
			SetFileAttributes(lpNewFileName, fa);
		}
	}

	return ret;
}

CString& 
ChangePathToFileName(CString& path)
{
	char 		drive[_MAX_DRIVE];
	char 		dir[_MAX_DIR];
	char 		ext[_MAX_EXT];
	char		file[_MAX_FNAME];
	char		fileName[_MAX_PATH];

	fnsplit( (LPCTSTR)path, drive, dir, file, ext );
	fnmerge( fileName, NULL, NULL, file, ext );

	path = fileName;

	return path;
}

void
ChangePath(CString &path) {

	CBrowser	browser;
	CString t=path;
	char pName[256];

	GetCurrentDirectory(256,pName);
	SetCurrentDirectory((LPCTSTR)path);

	if ( browser.Browse( t, TRUE ) )
		path=t;

	SetCurrentDirectory(pName);
}

void
CleanPath(CString &path) {

	char	tmp_path[_MAX_PATH];

	if(NULL != _fullpath(tmp_path, LPCTSTR(path), _MAX_PATH))
		path = tmp_path;

	int		p = path.ReverseFind('\\');
	
	if(p > 2)
		path = path.Left(p);
	if(p == 2)		 	// Root
		path = path.Left(3);
}

void
MergePath(const CString& Path, const CString& File, CString& Result) {

	if(Path.IsEmpty()) {
		Result = File;
		return;
	}
	char right = Path[Path.GetLength() - 1];
	if(right == '\\') {
		Result = Path;
		Result += File;
		return;
	}
	Result = Path;
	Result += "\\";
	Result += File;
}

void
SplitPath( CString szFullPath, CString* pszDrive, CString* pszDir, CString* pszFileName, CString* pszExt )
{
	char 		drive[_MAX_DRIVE];
	char 		dir[_MAX_DIR];
	char 		ext[_MAX_EXT];
	char		file[_MAX_FNAME];

	fnsplit( (LPCTSTR)szFullPath, drive, dir, file, ext );

	if ( NULL != pszDrive )
		*pszDrive = drive;
	if ( NULL != pszDir )
		*pszDir = dir;
	if ( NULL != pszFileName )
		*pszFileName = file;
	if ( NULL != pszExt )
		*pszExt = ext;
}

CString
MergePath( LPCTSTR szDrive, LPCTSTR szDir, LPCTSTR szFileName, LPCTSTR szExt )
{
	char		fileName[_MAX_PATH];

	fnmerge( fileName, szDrive, szDir, szFileName, szExt );

	CString sRet = fileName;

	return sRet;
}

void CreateUserPath( CString& Result )
{
	Result = GetExePath();
	DWORD size = 128;
	char UserName[128];
	if( !GetUserName( UserName , &size ) )
		return;

	CString UserDir = Result + '\\';
	UserDir += UserName;
	if( !CreateDirectory( UserDir , NULL ) )
	{
		if( GetLastError() != ERROR_ALREADY_EXISTS )
			return;
	}

	Result = UserDir + '\\';
}
 


///////////////////////////////////////////////////////////////////////////

