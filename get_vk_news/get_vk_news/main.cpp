#include <windows.h>
#include <string>
#include <vector>
#pragma comment(lib,"tinyxml/tinyxml.lib")
#include "tinyxml\tinyxml.h"
struct post{
	std::string id;
	std::string text;
	time_t datetime;
};
char* months[]={"���","���","���","���","���","���","���","���","���","���","���","���"};
time_t textToDateTime(const char *dt){
	char dates[4][6]={0};
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
		int day=atoi(dates[0]);
		int month;
		for(month=0;month<12 && strcmp(dates[1],months[month])!=0;++month);
		char *pos=strstr(dates[3],":");
		*pos++=0;
		int hour=atoi(dates[3]);
		int min=atoi(pos);
	}else if(j==2){
		int date=atoi(dates[0]);
		if(date==0){
			if(strcmp(dates[0],"�������")==0)date=1;
			if(strcmp(dates[0],"�����")==0)date=2;
			if(date!=0){
				char *pos=strstr(dates[2],":");
				*pos++=0;
				int hour=atoi(dates[2]);
				int min=atoi(pos);
			}
		}else{
			int month;
			for(month=0;month<12 && strcmp(dates[1],months[month])!=0;++month);
			int year=atoi(dates[2]);
		}
	}
	return 0;
}
char* textSplit(const char *dt){
	//dt - ������� ������
	char parts[100][20]={0};
	int j=0,ij=0;
	for(int i=0;dt[i]!='\0';++i){
		if(dt[i]==','){
			++j;
			ij=0;
		}else{
			parts[j][ij++]=dt[i];
		}
	}
	return 0;
}
TiXmlElement * getChildElementWithAttr(TiXmlElement *parent,const char *attr,const char* value){
	if(parent==NULL) return NULL;
	TiXmlElement *e=parent->FirstChildElement();
	while(e!=NULL){
		if(e->Attribute(attr)!=NULL){
			if(strcmp(e->Attribute(attr),value)==0) return e;
		}
		e=e->NextSiblingElement();
	}
	return NULL;
}
char* getElementText(TiXmlElement *parent,char *buff){
	char *temp=buff;
	if(strcmp(parent->Value(),"br")==0){
		temp[0]='\n';
		++temp;
	}
	TiXmlNode *e=parent->FirstChild();
	while(e!=NULL){
		if(dynamic_cast<TiXmlText*>(e)){
			strcpy(temp,e->Value());
			temp+=strlen(e->Value());
		}else if(dynamic_cast<TiXmlElement*>(e)){
			temp=getElementText(dynamic_cast<TiXmlElement*>(e),temp);
		}
		if(*(temp-1)!='\n' && e!=parent->FirstChild()){
			*temp=' ';
			++temp;
		}
		e=e->NextSibling();
		if(e!=NULL && strcmp(e->Value(),"br")==0 && *(temp-1)!='\n'){
			--temp;
		}
	}
	return temp;
}
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
	char ip[16];
	char query[1000];
	char buff[1000000]={0};
	wchar_t wbuff[500000]={0};
	const char docspec[]="<?xml version=\"1.0\" ?>\n<root>";
	const char tempstr[]="<div></div><div></div>";
int _stdcall WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow){
	WSADATA wsaData; 
	WSAStartup(MAKEWORD(2,2), &wsaData);
	SOCKET Sock = socket(AF_INET,SOCK_STREAM,0);
	SOCKADDR_IN addr_Sock; 
	addr_Sock.sin_family = AF_INET; 
	hostent *hp=gethostbyname("vk.com");
	sprintf(ip,"%d.%d.%d.%d",(unsigned char)hp->h_addr[0],(unsigned char)hp->h_addr[1],(unsigned char)hp->h_addr[2],(unsigned char)hp->h_addr[3]);
	addr_Sock.sin_addr.s_addr=inet_addr(ip);
	addr_Sock.sin_port = htons(80); 
	connect(Sock,(sockaddr*)&addr_Sock,sizeof(addr_Sock));
	sprintf(query,"POST /al_wall.php HTTP/1.1\nHost: vk.com\nConnection: keep-alive\nContent-Length: 62\nOrigin: http://vk.com\nX-Requested-With: XMLHttpRequest\nUser-Agent: Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/39.0.2171.65 Safari/537.36\nContent-Type: application/x-www-form-urlencoded\nAccept: */*\nReferer: http://vk.com/mguu_ru\nAccept-Encoding: identity\nAccept-Language: ru-RU,ru;q=0.8,en-US;q=0.6,en;q=0.4\nCookie: remixlang=0; remixstid=135315822_cafe8d09e8b8c59cde; remixdt=-39600; remixrefkey=836edd9622a03cece4; remixflash=15.0.0; remixscreen_depth=24\r\n\r\nact=get_wall&al=1&fixed=&offset=%d&owner_id=-52573107&type=own",0);
	send(Sock,query,668,0);
	char* ger=buff;
	int g=1;
	while(g>0){
		g = recv(Sock, ger, 999999, 0);
		ger+=g;
	}
	FILE *fp;
	fp=fopen("sruden.html","wb");
	fwrite(buff,1,ger-buff,fp);
	fclose(fp);
	std::vector<post*> posts;
	char* pos=strstr(buff,"<div");
	char *pos2=strstr(buff,"<!><!json>[]HTTP");
	*pos2=0;
	TiXmlDocument doc;
	doc.Parse(pos);
	TiXmlElement *e = doc.FirstChildElement("div");
	while(e!=NULL){
		char ht[100000]={0};
		std::string id=e->Attribute("id")+5;
		std::string text;
		time_t datetime;
		TiXmlElement *postinf=getChildElementWithAttr(getChildElementWithAttr(e,"class","post_table"),"class","post_info");
		TiXmlElement *wallText=getChildElementWithAttr(postinf,"class","wall_text");
		if(wallText!=NULL){
			if(getChildElementWithAttr(wallText,"class","published_by_quote")==NULL){
				TiXmlElement *wallPostText=getChildElementWithAttr(wallText,"class","wall_post_text");
				if(wallPostText==NULL){
					wallPostText=getChildElementWithAttr(getChildElementWithAttr(wallText,"id",(std::string("wpt-")+id).c_str()),"class","wall_post_text");
				}
				TiXmlElement *moreLink=getChildElementWithAttr(wallPostText,"class","wall_post_more");
				if(moreLink!=NULL){
					wallPostText->RemoveChild(moreLink->PreviousSibling());
					wallPostText->RemoveChild(moreLink);
				}
				getElementText(wallPostText,ht);
				text.assign(ht);
				text.pop_back();
			}
		}
		TiXmlElement *dt=getChildElementWithAttr(getChildElementWithAttr(postinf,"class","replies"),"class","reply_link_wrap sm")->FirstChildElement()->FirstChildElement()->FirstChildElement();
		getElementText(dt,ht);
		datetime=textToDateTime(ht);
		e=e->NextSiblingElement("div");
	}
	WSACleanup();
}