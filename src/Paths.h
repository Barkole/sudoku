#ifndef UTIL_PATH_H_INCLUDE

#define UTIL_PATH_H_INCLUDE

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// global path helpers

void InstallExePath(const char* program_pointer = NULL);
char* GetExePath();

//////////////////////////////////////////////////////////////////////////////////

DWORD		GetSizeOfFile(const char* filename);
long		GetFileTimeStamp(const char *filename);
BOOL		FileExist(const char *file);
BOOL		CopyFileNoRO(LPCSTR lpExistingFileName, LPCSTR lpNewFileName, BOOL bFailIfExists);
CString		&ChangePathToFileName( CString &path );

void		ChangePath(CString &path);
void		CleanPath(CString &path);
void		MergePath(const CString& Path, const CString& File, CString& Result);
void		SplitPath( CString szFullPath, CString* pszDrive, CString* pszDir, CString* pszFileName, CString* pszExt );
CString		MergePath( LPCTSTR szDrive, LPCTSTR szDir, LPCTSTR szFileName, LPCTSTR szExt );
void		CreateUserPath( CString& Result );

///////////////////////////////////////////////////////////////////////////////////////////////////////////////


#endif // UTIL_PATH_H_INCLUDE