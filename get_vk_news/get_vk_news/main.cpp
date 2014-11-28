#include <windows.h>
#include <string>
#include <vector>
#include <time.h>
#include "VK.h"
int CopyBoard(const char *str,int ll)
{
    HGLOBAL hglb;
    char *s;
 
    if (!(hglb = GlobalAlloc(GHND, ll))){
        return 1;
    }
    if (!(s = (char *)GlobalLock(hglb))){
        return 1;
    }
	::strcpy_s(s,ll,str);
    GlobalUnlock(hglb);
    if (!OpenClipboard(NULL) || !EmptyClipboard()) {
        GlobalFree(hglb);
    return 1;
    }
    SetClipboardData(CF_TEXT, hglb);
    CloseClipboard();
    return 0;
}
int _stdcall WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow){
	VK::VKPostGetter getter("-52573107");
	std::vector<VK::Post*> posts;
	getter.getPostsAfterId("52573107_1029",posts);
}