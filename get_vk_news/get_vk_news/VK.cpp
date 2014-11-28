#include "VK.h"
namespace VK{
	char vkips[4][16]={0};
	SOCKADDR_IN vk_addr;
	const char* months[]={"янв","фев","мар","апр","мая","июн","июл","авг","сен","окт","ноя","дек"};
	Post::~Post(){
		::MessageBoxA(0,"adfda","adfadf",0);
	}
	bool refreshVK(){
		WSADATA wsaData;
		WSAStartup(MAKEWORD(2,2), &wsaData);
		hostent *hp=gethostbyname("vk.com");
		int i;
		for(i=0;i<(hp->h_length-1>4 ? 4:hp->h_length-1);++i)
			sprintf(vkips[i],"%d.%d.%d.%d",(unsigned char)hp->h_addr_list[i][0],(unsigned char)hp->h_addr_list[i][1],(unsigned char)hp->h_addr_list[i][2],(unsigned char)hp->h_addr_list[i][3]);
		if(i<4)vkips[i][0]='\0';
		vk_addr.sin_family = AF_INET; 
		vk_addr.sin_addr.s_addr=inet_addr(vkips[0]);
		vk_addr.sin_port = htons(80);
		return true;
	}
	bool f=refreshVK();
	time_t VKTimeToTime_t(const char *dt){
		char dates[4][6]={0};
		time_t result=0;
		tm *timeTransform,tmtf;
		int j=0,ij=0;
		bool hasChanged=true;
		for(int i=0;dt[i]!=0;++i){
			if(dt[i]!=' '){
				if(!hasChanged){
					hasChanged=true;
					++j;
					ij=0;
				}
				dates[j][ij++]=dt[i];
			}else if(hasChanged)hasChanged=false;
		}
		if(j==3){
			result=time(0);
			timeTransform=localtime(&result);
			timeTransform->tm_mday=atoi(dates[0]);
			int month;
			for(month=0;month<12 && strcmp(dates[1],months[month])!=0;++month);
			timeTransform->tm_mon=month;
			char *pos=strstr(dates[3],":");
			*pos++=0;
			timeTransform->tm_hour=atoi(dates[3])-1;
			timeTransform->tm_min=atoi(pos)-1;
			timeTransform->tm_sec=0;
			result=mktime(timeTransform);
		}else if(j==2){
			int date=atoi(dates[0]);
			if(date==0){
				result=time(0);
				if(strcmp(dates[0],"сегодня")==0)date=1;
				if(strcmp(dates[0],"вчера")==0)date=2;
				result-=(date-1)*60*60*24;
				timeTransform=localtime(&result);
				result=0;
				if(date!=0){
					char *pos=strstr(dates[2],":");
					*pos++=0;
					timeTransform->tm_hour=atoi(dates[2])-1;
					timeTransform->tm_min=atoi(pos)-1;
					timeTransform->tm_sec=0;
					result=mktime(timeTransform);
				}
			}
		}
		return result;
	}

	const char VKPostHtmlExtractor::postsQueryTemplate[]="POST /al_wall.php HTTP/1.1\nHost: vk.com\nConnection: keep-alive\nContent-Length: %d\nOrigin: http://vk.com\nX-Requested-With: XMLHttpRequest\nUser-Agent: Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/39.0.2171.65 Safari/537.36\nContent-Type: application/x-www-form-urlencoded\nAccept: */*\nReferer: http://vk.com/mguu_ru\nAccept-Encoding: identity\nAccept-Language: ru-RU,ru;q=0.8,en-US;q=0.6,en;q=0.4\nCookie: remixlang=0; remixstid=135315822_cafe8d09e8b8c59cde; remixdt=-39600; remixrefkey=836edd9622a03cece4; remixflash=15.0.0; remixscreen_depth=24\r\n\r\nact=get_wall&al=1&fixed=&offset=%d&owner_id=%s&type=own";
	const char VKPostHtmlExtractor::postQueryTemplate[]="POST /wkview.php HTTP/1.1\nHost: vk.com\nConnection: keep-alive\nContent-Length: %d\nOrigin: http://vk.com\nX-Requested-With: XMLHttpRequest\nUser-Agent: Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/39.0.2171.71 Safari/537.36\nContent-Type: application/x-www-form-urlencoded\nAccept: */*\nReferer: http://vk.com/do_magic_at_cmc_since_2014\nAccept-Encoding: identity\nAccept-Language: ru-RU,ru;q=0.8,en-US;q=0.6,en;q=0.4\nCookie: remixlang=0; remixdt=-39600; remixrefkey=186221f76096524285; remixsid=58805f56e7387f7178600982243f8ba07c3d9ff724b015c39001e; remixflash=15.0.0; remixscreen_depth=24; remixseenads=1\r\n\r\nact=show&al=1&loc=mguu_ru&w=wall-%s";
	VKPostHtmlExtractor::VKPostHtmlExtractor(){
	}
	int VKPostHtmlExtractor::getDetailPostHtml(const char* postId,char *buff){
		char query[1000];
		sprintf(query,this->postQueryTemplate,postId);
		if(send(this->s,query,strlen(query),0)==SOCKET_ERROR){
			if(initConnection()!=0) return 1;
			send(this->s,query,strlen(query),0);
		}
		unsigned short int byteCount=10;
		while(byteCount>0){
			byteCount = recv(s,buff, 999999, 0);
			buff+=byteCount;
		}
		return 0;
	}
	int VKPostHtmlExtractor::getPostsHtmlWithOffset(const char* ownerId,const int offset,char *buff){
		char query[1000];
		sprintf(query,"%d",offset);
		int addLength=strlen(query);
		buff[0]=0;
		sprintf(query,this->postsQueryTemplate,60+addLength,offset,ownerId);
		if(send(this->s,query,strlen(query)+4,0)==SOCKET_ERROR){
			if(initConnection()!=0) return 1;
			send(this->s,query,strlen(query)+4,0);
		}
		int byteCount;
		do{
			byteCount = recv(this->s,buff, 100000, 0);
			buff+=byteCount;
		}while(byteCount>0);
		return 0;
	}
	int VKPostHtmlExtractor::initConnection(){
		closesocket(s);
		s = socket(AF_INET,SOCK_STREAM,0);
		if(connect(s,(sockaddr*)&vk_addr,sizeof(vk_addr))==SOCKET_ERROR) return 1;
		return 0;
	}
	VKPostHtmlExtractor::~VKPostHtmlExtractor(){
		closesocket(s);
	}
	
	int VKPostParser::parsePosts(const char* htmlText,std::vector<Post*> &posts){
		TiXmlDocument doc;
		const char *pos=strstr(htmlText,"<div");
		doc.Parse(pos);
		TiXmlElement *e = doc.FirstChildElement("div");
		char ht[100000]={0};
		while(e!=NULL){
			Post *tpost=new Post();
			tpost->id=e->Attribute("id")+5;
			TiXmlElement *postinf=getChildElementWithAttr(getChildElementWithAttr(e,"class","post_table"),"class","post_info");
			TiXmlElement *wallText=getChildElementWithAttr(postinf,"class","wall_text");
			if(wallText!=NULL){
				if(getChildElementWithAttr(wallText,"class","published_by_quote")==NULL){
					TiXmlElement *wallPostText=getChildElementWithAttr(wallText,"class","wall_post_text");
					if(wallPostText==NULL){
						wallPostText=getChildElementWithAttr(getChildElementWithAttr(wallText,"id",(std::string("wpt-")+tpost->id).c_str()),"class","wall_post_text");
					}
					TiXmlElement *moreLink=getChildElementWithAttr(wallPostText,"class","wall_post_more");
					if(moreLink!=NULL){
						wallPostText->RemoveChild(moreLink->PreviousSibling());
						wallPostText->RemoveChild(moreLink);
					}
					getElementText(wallPostText,ht);
					tpost->text.assign(ht);
					tpost->text.pop_back();
				}
			}
			TiXmlElement *dt=getChildElementWithAttr(getChildElementWithAttr(postinf,"class","replies"),"class","reply_link_wrap sm")->FirstChildElement()->FirstChildElement()->FirstChildElement();
			getElementText(dt,ht);
			tpost->dateTime=VKTimeToTime_t(ht);
			posts.push_back(tpost);
			e=e->NextSiblingElement("div");
		}
		return 0;
	}
	int VKPostParser::parseDetailPost(const char* htmlText,Post &post){
		TiXmlDocument doc;
		const char *pos=strstr(htmlText,"<div");
		doc.Parse(pos);
		TiXmlElement *e = doc.FirstChildElement("div");
		char ht[100000]={0};		
		TiXmlElement *temp=getChildElementWithAttrDeep(e,"class","wl_owner_head_date");
		//getChildElementWithAttr(getChildElementWithAttr(getChildElementWithAttr(e,"id","wl_head_wrap"),"class","wl_owner_head_wrap clear_fix"),"class","wl_owner_head_date");
		temp=getChildElementWithAttr(temp,"class","wl_owner_head_date");
		post.id=temp->Attribute("href")+6;
		getElementText(temp,ht);
		post.dateTime=VKTimeToTime_t(ht);
		temp=getChildElementWithAttrDeep(e,"class","wall_post_text");
		getElementText(temp,ht);
		post.text.assign(ht);
		post.text.pop_back();
		return 0;
	}
	VKPostGetter::VKPostGetter(const char *groupId){
		this->groupId=new char[strlen(groupId)+1];
		strcpy(this->groupId,groupId);
	}
	void VKPostGetter::getPostById(const char* postId,Post& post){
		char buff[40000];
		extractor.getDetailPostHtml(postId,buff);
		VKPostParser::parseDetailPost(buff,post);
	}
	void VKPostGetter::getPostsAfterId(const char* postId,std::vector<Post*> &posts){
		char buff[1000000];
		int i=0,terminator=true,shift=posts.size(),j;
		while(terminator){
			extractor.getPostsHtmlWithOffset(groupId,i,buff);
			VKPostParser::parsePosts(buff,posts);
			if(shift==posts.size()) terminator=false;
			else{
				for(j=shift;j<posts.size();++j){
					if(posts[j]->id==postId){
						terminator=false;
						posts.erase(posts.begin()+j,posts.end());
						break;
					}
				}
				shift=posts.size();
				i+=10;
			}
		}
	}
	void VKPostGetter::getPostsAfterTime(time_t time,std::vector<Post*> &posts){
		char buff[1000000];
		int i=0,terminator=true,shift=posts.size(),j;
		while(terminator){
			extractor.getPostsHtmlWithOffset(groupId,i,buff);
			VKPostParser::parsePosts(buff,posts);
			if(shift==posts.size()) terminator=false;
			else{
				for(j=shift;j<posts.size();++j){
					if(posts[j]->dateTime==0){
						this->getPostById(posts[j]->id.c_str(),*posts[j]);
					}
					if(posts[j]->dateTime<=time){
						terminator=false;
						posts.erase(posts.begin()+j,posts.end());
						break;
					}
				}
				shift=posts.size();
				i+=10;
			}
		}
	}
	VKPostGetter::~VKPostGetter(){
		delete[] this->groupId;
	}
}
