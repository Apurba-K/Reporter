
#include "pch.h"

#include <iostream>



#include "feedQueue.h"
#include "context.h"
#include "reportWriter.h"
#include "feedMetaData.h"


Reporter::ReportWriter::ReportWriter(const FeedMetaData& metadata1) : outMetaData(metadata1)
{
	reportStream = std::ofstream(outMetaData.getFeedFile());
}
void Reporter::ReportWriter::write(Context& inContext)
{
	bool feedToProcess = true;
	std::vector<std::string> outRecord;
	std::string reportMsg;

	auto& recordDef = outMetaData.getRecordDefination();
	reportMsg = recordDef.fieldHeader.at(recordDef.recordFields[0]);

	for (unsigned int indx = 1; indx < recordDef.recordFields.size(); indx++)
		reportMsg += outMetaData.getFeedDelimeter() + recordDef.fieldHeader.at(recordDef.recordFields[indx]);

	reportMsg += "\n";
	reportStream << reportMsg;


	while(feedToProcess)
	{
		{
			std::unique_lock<std::mutex> lock(inContext.contextQ.qLock);
			if (!inContext.contextQ.feedq.empty())
			{
				outRecord = inContext.contextQ.feedq.front();
				inContext.contextQ.feedq.pop();
			}
			else
			{
				if (inContext.complete)
					feedToProcess = false;
				continue;
			}
		}

		reportMsg = outRecord[0];

		for (unsigned int indx = 1; indx < outRecord.size(); ++indx)
		{
			reportMsg += outMetaData.getFeedDelimeter() + outRecord[indx];
		}

		reportMsg += "\n";
		reportStream << reportMsg;
	}
	reportStream.close();
	return;
}