#include "natsortalg.h"

#ifdef __WXMSW__
#include <shlwapi.h>
#else
#include <glib.h>
#endif

#ifdef __WXMSW__
//This singleton class and the runtime load of shlwapi.dll is necessary because mingw has no support for StrCmpLogicalW.
//Once this is fixed WinSorter can be deleted and we can call StrCmpLogicalW directly. 
typedef	int(WINAPI*PFN_StrCmpLogicalW)(LPCWSTR, LPCWSTR);
class WinSorter
{
private:
	static HMODULE shlwapiDLL;
	static PFN_StrCmpLogicalW StrCmpLogicalW;
	friend const WinSorter& GetWinSorter();

	WinSorter()
	{
		shlwapiDLL = LoadLibrary(L"shlwapi.dll");
		if (shlwapiDLL)
			StrCmpLogicalW = (PFN_StrCmpLogicalW)GetProcAddress(shlwapiDLL, "StrCmpLogicalW");
	}
	~WinSorter()
	{
		if (shlwapiDLL)
			FreeLibrary(shlwapiDLL);
	}
public:
	static const WinSorter& GetInstance();
	int Compare(const wxString& a, const wxString& b) const;
	
	
};

HMODULE WinSorter::shlwapiDLL = NULL;
PFN_StrCmpLogicalW WinSorter::StrCmpLogicalW = NULL;

int WinSorter::Compare(const wxString& a, const wxString& b) const
{
	if(StrCmpLogicalW != NULL)
		return StrCmpLogicalW( a.wc_str(), b.wc_str());
	else
		return a.CmpNoCase(b);
}

const WinSorter& WinSorter::GetInstance()
{
	static WinSorter sorter;
	return sorter;
}
#endif



int NaturalCompare(const wxString& left, const wxString& right)
{
	int compare = 0;

#ifdef __WXMSW__

	compare = WinSorter::GetInstance().Compare(left,right);

#else
	/* On Linux, files that start with these characters sort after files that don't. */
	#define SORT_LAST_CHAR1 '.'
	#define SORT_LAST_CHAR2 '#'

	bool sort_last_1, sort_last_2;
	sort_last_1 = left[0] == SORT_LAST_CHAR1 || left[0] == SORT_LAST_CHAR2;
	sort_last_2 = right[0] == SORT_LAST_CHAR1 || right[0] == SORT_LAST_CHAR2;

	if (sort_last_1 && !sort_last_2) 
	{
		compare = +1;
	} 
	else if (!sort_last_1 && sort_last_2) 
	{
		compare = -1;
	} 
	else 
	{
		char* left_collation_key = g_utf8_collate_key_for_filename (left.mb_str(), -1);
		char* right_collation_key = g_utf8_collate_key_for_filename (right.mb_str(), -1);
		if(left_collation_key != NULL && right_collation_key != NULL)
		{
			compare = strcmp(left_collation_key, right_collation_key);
			g_free(left_collation_key);
			g_free(right_collation_key);
		}
		else
		{
			compare = left.Cmp(right);
		}
	}
#endif

	return compare;
}