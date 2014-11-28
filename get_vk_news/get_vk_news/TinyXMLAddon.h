#ifndef __TINYXML_ADDON__
#define __TINYXML_ADDON__
#pragma comment(lib,"tinyxml/tinyxml.lib")
#include "tinyxml\tinyxml.h"
TiXmlElement * getChildElementWithAttr(TiXmlElement *parent,const char *attr,const char* value);
TiXmlElement * getChildElementWithAttrDeep(TiXmlElement *parent,const char *attr,const char* value);
char* getElementText(TiXmlElement *parent,char *buff);
#endif