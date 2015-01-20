#ifndef __NATSORTALG_H__
#define __NATSORTALG_H__

#include <wx/string.h>

//Emulates the Windows Explorer/Nautilus file sort order.
//returns -1 if left<right, 0 if left==right, 1 if left>right
//for more info see http://blog.codinghorror.com/sorting-for-humans-natural-sort-order/
int NaturalCompare(const wxString& left, const wxString& right);

#endif