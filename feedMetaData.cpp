#pragma once

#include "pch.h"
#include "feedMetaData.h"

#include <iostream>

bool Reporter::FeedMetaData::getElement(tinyxml2::XMLDocument* xmlDoc, tinyxml2::XMLElement** foundelem, const std::string& tag, std::string& errormsg)
{
	*foundelem = nullptr;
	*foundelem = xmlDoc->FirstChildElement(tag.c_str());
	if (*foundelem == NULL)
	{
		errormsg = "FEED-METADATA: tag[" + tag + "] not present in[" + metafile +"]";
		return false;
	}
	return true;
}

bool Reporter::FeedMetaData::getsubElement(tinyxml2::XMLElement** foundelem, const std::string& current, const std::string& tag, std::string& errormsg)
{
	*foundelem = (*foundelem)->FirstChildElement(tag.c_str());
	if (*foundelem == NULL)
	{
		errormsg = "FEED-METADATA: " + current + "/" + tag + " not present in[" + metafile + "]";
		return false;
	}
	return true;
}

bool Reporter::FeedMetaData::getAttribute(tinyxml2::XMLElement* xmlelem, const char* attrNm, const char** attrVal, const std::string& current, std::string& errormsg)
{
	*attrVal = xmlelem->Attribute(attrNm);
	if (*attrVal == NULL)
	{
		errormsg = "FEED-METADATA:" + current + " does not have attribute[" + attrNm + "]";
		return false;
	}
	return true;
}

bool Reporter::FeedMetaData::populateReportDTD(tinyxml2::XMLElement* xmlelem, int count, const std::string& current, const std::string& tag, std::string& errormsg)
{
	const char* attrval = nullptr;
	std::string name, type, header;
	int errorCt = 0;

	xmlelem = xmlelem->FirstChildElement("Field");
		
	for (int i = 0; i < count; ++i)
	{
		if (xmlelem == NULL)
		{
			errormsg = "FEED-METADATA: Field count[" + std::to_string( ++errorCt ) + "] not present";
			return false;
		}
		if (getAttribute(xmlelem, "name", &attrval, current + "Field", errormsg))
			name = std::string(attrval);
		else
		{
			errormsg = "FEED-METADATA: Field count[" + std::to_string( ++errorCt ) + "] does not have attribute[name]";
			return false;
		}

		if (getAttribute(xmlelem, "type", &attrval, current + "Field", errormsg))
			type = std::string(attrval);
		else
		{
			
			std::string msg = "FEED-METADATA: Field count[" + std::to_string( ++errorCt ) + "] does not have attribute[type]\n";
			std::cout << msg;

			type = "string";
			//return false;
		}

		if (getAttribute(xmlelem, "Header", &attrval, current + "Field", errormsg))
			header = std::string(attrval);
		else
			header = name;
		
	    std::cout << "Name:" << name << " Type:" << type << " Header:" << header << "\n";
		recordDef.recordFields.push_back(name);
		recordDef.fieldDef.insert(std::make_pair(name, std::make_pair(type, recordDef.recordFields.size() -1)));
		recordDef.fieldHeader.insert(std::make_pair(name, header));
		
		xmlelem = xmlelem->NextSiblingElement(tag.c_str());
	}
	return true;
}

Reporter::FeedMetaData::FeedMetaData(const std::string& metafile_, const std::string& tag_) : totFieldCt(0), feedTag(tag_), metafile(metafile_), threadCount(0)
{}

bool Reporter::FeedMetaData::build(std::string& errormsg)
{
	bool result = true;

	if (metafile.empty())
	{
		errormsg = "FEED-METADATA: metafile not specified";
		result = false;
	}
	else if (feedTag.empty())
	{
		errormsg = "FEED-METADATA: feed name not specified";
		result = false;
	}
	else
	{
		tinyxml2::XMLDocument* xmlDoc = new tinyxml2::XMLDocument();
		xmlDoc->LoadFile(metafile.c_str());

		std::cout << "FEED-METADATA:" << metafile << "\n";

		if (xmlDoc->Error())
		{
			errormsg = "FEED-METADATA: " + metafile + " not found";
			result = false;
		}
		else
			result = parse(xmlDoc, errormsg);
	}
	return result;
}

bool Reporter::FeedMetaData::parse(tinyxml2::XMLDocument* xmlDoc, std::string& errormsg)
{
	tinyxml2::XMLElement* feedElemenet = nullptr, *foundElem = nullptr;
	const char* attrval = nullptr;
	
	if (!getElement(xmlDoc, &feedElemenet, feedTag, errormsg))
		return false;

	foundElem = feedElemenet;
	if (!getsubElement(&foundElem, feedTag, "FeedFile", errormsg))
		return false;
	else
		feedFile = foundElem->FirstChild()->ToText()->Value();

	foundElem = feedElemenet;
	if (!getsubElement(&foundElem, feedTag, "PreperatorThread", errormsg))
		threadCount = 1;
	else
		threadCount = std::stoi(foundElem->FirstChild()->ToText()->Value());

	foundElem = feedElemenet;
	if (!getsubElement(&foundElem, feedTag, "Fields", errormsg))
		return false;

	if (!getAttribute(foundElem, "count", &attrval, "IncomingFeed/Fields", errormsg))
		return false;
	else
		totFieldCt = std::stoi(attrval);

	if (totFieldCt == 0)
	{
		errormsg = "IncomingFeed/Fields attribute count is zero";
		return false;
	}
	
	if (!getAttribute(foundElem, "delimeter", &attrval, "IncomingFeed/Fields", errormsg))
	{
		errormsg = "IncomingFeed/Fields does not have attribute delimeter";
		return false;
	}
	else
		delimeter = std::string(attrval);
	
	if (!populateReportDTD(foundElem, totFieldCt, "IncomingFeed/Fields", "Field", errormsg))
		return false;

	return true;
}
