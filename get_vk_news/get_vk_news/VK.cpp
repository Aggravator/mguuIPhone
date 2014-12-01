#include "VK.h"
namespace VK{
	char vkips[4][16]={0};
	SOCKADDR_IN vk_addr;
	const char* months[]={"янв","фев","мар","апр","мая","июн","июл","авг","сен","окт","ноя","дек"};
	void deletePost(Post* post){
		delete post;
	}
	Post::~Post(){
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
		char dates[4][8]={0};
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
	const char VKPostHtmlExtractor::postQueryTemplate[]="GET /wall-%s HTTP/1.1\nHost: vk.com\nConnection: keep-alive\nCache-Control: max-age=0\nAccept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8\nUser-Agent: Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/39.0.2171.71 Safari/537.36\nAccept-Encoding: identity\nAccept-Language: ru-RU,ru;q=0.8,en-US;q=0.6,en;q=0.4\n\n";
	VKPostHtmlExtractor::VKPostHtmlExtractor(){
	}
	int VKPostHtmlExtractor::getResponseOnQuery(const char* query,char *buff){
		if(send(this->s,query,strlen(query)+2,0)==SOCKET_ERROR){
			if(initConnection()!=0) return 1;
			send(this->s,query,strlen(query)+2,0);
		}
		bool isFirst=true;
		int byteCount;
		do{
			byteCount = recv(this->s,buff, 50000, 0);
			if((byteCount==-1 || byteCount==0) && isFirst){
				this->initConnection();
				byteCount=1;
				send(this->s,query,strlen(query)+4,0);
			}else buff+=byteCount;
			isFirst=false;
		}while(byteCount>0);
	}
	int VKPostHtmlExtractor::getDetailPostHtml(const char* postId,char *buff){
		char query[1000];
		sprintf(query,this->postQueryTemplate,postId);
		this->getResponseOnQuery(query,buff);
		return 0;
	}
	int VKPostHtmlExtractor::getPostsHtmlWithOffset(const char* ownerId,const int offset,char *buff){
		char query[1000];
		sprintf(query,"%d",offset);
		int addLength=strlen(query);
		buff[0]=0;
		sprintf(query,this->postsQueryTemplate,61+addLength,offset,ownerId);
		this->getResponseOnQuery(query,buff);
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
					if(tpost->text.size()>0)tpost->text.pop_back();
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
		const char *posS=strstr(htmlText,"<body");
		const char *posE=strstr(htmlText,"</body>");
		char *data=new char[posE-posS+7];
		memcpy(data,posS,posE-posS+7);
		doc.Parse(data);
		delete[] data;
		char ht[100000]={0};		
		TiXmlElement *e = getChildElementWithAttrDeep(&doc,"class","fw_post_table");
		TiXmlElement *temp=getChildElementWithAttrDeep(e,"class","fl_l fw_post_date");
		getElementText(temp,ht);
		*(ht+strlen(ht)-2)='\0';
		post.dateTime=VKTimeToTime_t(ht);
		temp=getChildElementWithAttrDeep(e,"class","fw_post_info");
		temp=temp->FirstChildElement();
		while(temp!=NULL){
			if(temp->Attribute("id")!=NULL){
				if(strncmp(temp->Attribute("id"),"wpt",3)==0){
					post.id=temp->Attribute("id")+4;
					temp=getChildElementWithAttrDeep(e,"class","wall_post_text");
					if(temp!=NULL){
						getElementText(temp,ht);
						post.text=ht;
					}
					break;
				}
			}
			temp=temp->NextSiblingElement();
		}
		return 0;
	}
	VKPostGetter::VKPostGetter(const char *groupId){
		this->groupId=new char[strlen(groupId)+1];
		strcpy(this->groupId,groupId);
	}
	void VKPostGetter::getPostById(const char* postId,Post& post){
		char buff[400000];
		extractor.getDetailPostHtml(postId,buff);
		post.text="";
		post.id="";
		post.dateTime=0;
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
						std::for_each(posts.begin()+j,posts.end(),deletePost);
						posts.erase(posts.begin()+j,posts.end());
						break;
					}
				}
				shift=posts.size();
				i+=10;
			}
		}
		for(int i=0;i<posts.size();++i)
			if(posts[i]->dateTime==0)
				this->getPostById(posts[i]->id.c_str(),*posts[i]);
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
						std::for_each(posts.begin()+j,posts.end(),deletePost);
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
