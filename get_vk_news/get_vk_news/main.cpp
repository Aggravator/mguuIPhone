#include <windows.h>
#include <string>
#include <vector>
#include <algorithm>
#include <time.h>
#include "VK.h"

void deletePost(VK::Post* post){
	delete post;
}
void erasePosts(std::vector<VK::Post*> &posts,std::vector<VK::Post*>::iterator begin,std::vector<VK::Post*>::iterator end){
	std::for_each(begin,end,deletePost);
	posts.erase(begin,end);
}
int _stdcall WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow){
	VK::VKPostGetter getter("-75776018");
	std::vector<VK::Post*> posts;
	getter.getPostsAfterId("75776018_7120",posts);
	if(posts.size()>0){
		erasePosts(posts,posts.begin(),posts.end());
	}
	return 0;
	//getter.getPostsAfterId("52573107_1029",posts);
}