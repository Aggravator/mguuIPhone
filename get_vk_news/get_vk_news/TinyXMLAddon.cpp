#include "TinyXMLAddon.h"
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
TiXmlElement * getChildElementWithAttrDeep(TiXmlElement *parent,const char *attr,const char* value){
	if(parent==NULL) return NULL;
	TiXmlElement *e=parent->FirstChildElement();
	TiXmlElement *pp;
	while(e!=NULL){
		if(e->Attribute(attr)!=NULL){
			if(strcmp(e->Attribute(attr),value)==0) return e;
		}
		pp=getChildElementWithAttrDeep(e,attr,value);
		if(pp!=NULL)return pp;
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