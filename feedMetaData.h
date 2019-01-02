#pragma once

#ifndef _FEED_METATA_DATA_
#define _FEED_METATA_DATA_

#include "tinyxml2.h"

#include <vector>
#include <string>
#include <unordered_map>

namespace Reporter
{
	class FeedMetaData
	{
		// METADATA depecting credential for incoming and outgoing feed
	public:
		struct RecordDefination
		{
			// METADATA for messages.
			std::unordered_map<std::string, std::pair<std::string, int> > fieldDef; // FieldType and location in a record
			std::vector<std::string> recordFields; // Fields in a record
			std::unordered_map<std::string, std::string> fieldHeader; // Header corresponding to specific field
		};
		FeedMetaData(const std::string& metafile, const std::string& tag);

		bool build(std::string& errormsg);

		const RecordDefination& getRecordDefination() const { return recordDef; }
		int getTotalFieldCt() const { return totFieldCt; }
		std::string getFeedDelimeter() const { return delimeter; }
		std::string getFeedFile() const { return feedFile; }
		int getThreadCount() const { return threadCount; }

	private:
		RecordDefination recordDef;
		std::string delimeter, metafile, feedFile, feedTag;
		int totFieldCt, threadCount;
		
		bool parse(tinyxml2::XMLDocument* xmlDoc, std::string& errormsg);
		bool getsubElement(tinyxml2::XMLElement** foundelem, const std::string& current, const std::string& tag, std::string& errormsg);
		bool getElement(tinyxml2::XMLDocument* xmlDoc, tinyxml2::XMLElement** foundelem, const std::string& tag, std::string& errormsg);
		bool getAttribute(tinyxml2::XMLElement* xmlelem, const char* attrNm, const char** attrVal, const std::string& current, std::string& errormsg);
		bool populateReportDTD(tinyxml2::XMLElement* xmlelem, int count, const std::string& current, const std::string& tag, std::string& errormsg);
	};
};

#endif

