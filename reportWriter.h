#pragma once

#ifndef _REPORT_WRITER_
#define _REPORT_WRITER_

#include <fstream>

namespace Reporter
{
	class FeedMetaData;
	struct Context;

	class ReportWriter
	{
		//Class to write report to output file
		const FeedMetaData& outMetaData;
		std::ofstream reportStream;

	public:
		ReportWriter(const FeedMetaData& metadata1);
		void write(Context& inContext);

	};
}

#endif