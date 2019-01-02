#pragma once

#ifndef _PROCESSING_CONTEXT_
#define _PROCESSING_CONTEXT_

#include <queue>
#include  <set>
#include <map>

namespace Reporter
{

	class FieldWrapper;

	class Context
	{
		std::queue<FieldWrapper*> processedRow;
	public:
		void getProcessedRow(std::queue<FieldWrapper*>& incomingQ);
		void storeInContext(FieldWrapper* row);
	};

	class ReportContext : Context
	{
		std::map<std::string,std::set<FieldWrapper>> reportRow;
	};
}

#endif