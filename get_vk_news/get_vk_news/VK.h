#ifndef __VK__
#define __VK__
#include <Windows.h>
#include <string.h>
#include <stdio.h>
#include <string>
#include <vector>
#include <time.h>
#pragma comment(lib,"tinyxml/tinyxml.lib")
#include "tinyxml\tinyxml.h"
#include "TinyXMLAddon.h"
#include <algorithm> 
namespace VK{
	time_t VKTimeToTime_t(const char *dt);
	struct Post{
		std::string id;
		std::string text;
		time_t dateTime;
		~Post();
	};
	class VKPostHtmlExtractor{
	public:
		VKPostHtmlExtractor();
		int getDetailPostHtml(const char* postId,char *buff);
		int getPostsHtmlWithOffset(const char* ownerId,const int offset,char *buff);
		virtual ~VKPostHtmlExtractor();
	private:
		static const char postsQueryTemplate[];
		static const char postQueryTemplate[];
		int getResponseOnQuery(const char* query,char *buffer);
		int initConnection();
		SOCKET s;
	};
	
	class VKPostParser{
	public:
		static int parsePosts(const char* htmlText,std::vector<Post*> &posts);
		static int parseDetailPost(const char* htmlText,Post &post);
	};

	class VKPostGetter{
	public:
		VKPostGetter(const char *groupId);
		void getPostById(const char* postId,Post& post);
		void getPostsAfterId(const char* postId,std::vector<Post*> &posts);
		void getPostsBetweenIds(const char *afterPostId,const char *beforePostId,std::vector<Post*> &posts);
		void getPostsAfterTime(time_t time,std::vector<Post*> &posts); 
		virtual ~VKPostGetter();
	private:
		char *groupId;
		VKPostHtmlExtractor extractor;
	};
}
#endif