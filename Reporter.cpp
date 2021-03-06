// Reporter.cpp : This file contains the 'main' function. Program execution begins and ends there.

#include "pch.h"
#include "feedMetaData.h"
#include "appRowFactory.h"
#include "context.h"
#include "reportWriter.h"

#include <iostream>
#include <thread>


int main()
{
	std::string errormsg;

	try
	{
		Reporter::FeedMetaData incoming("IncomingFeed.xml", "IncomingFeed");
		if (!incoming.build(errormsg))
			std::cout << errormsg << std::endl;
		else
		{
			Reporter::FeedMetaData outgoing("OutgoingFeed.xml", "OutgoingFeed");
			if (!outgoing.build(errormsg))
				std::cout << errormsg << std::endl;
			else
			{
				Reporter::InComingRowFactory inRowFactory(incoming);
				Reporter::TIckerRowFactory tickerFactory(incoming);
				Reporter::Context tickerContext;
				auto readInComingFeed = [&inRowFactory, &tickerContext]() { inRowFactory.prepareFeed(tickerContext); };
				auto mapTickerData = [&tickerFactory, &tickerContext]() { tickerFactory.prepareFeed(tickerContext); };

				std::thread feedReader(readInComingFeed);
				std::thread tickerProducer(mapTickerData);

				feedReader.join();
				tickerProducer.join();
				
				Reporter::Context reportContext;
				Reporter::TickerReportRowFactory ticketerReportFactory(incoming, outgoing, outgoing.getThreadCount(), tickerFactory.getTickerData());
				Reporter::ReportWriter reportWriter(outgoing);
				auto makeTickerReport = [&ticketerReportFactory, &reportContext]() { ticketerReportFactory.prepareFeed(reportContext); };
				auto writeTickerReport = [&reportWriter, &reportContext]() { reportWriter.write(reportContext); };

				std::thread tickerReportProducer(makeTickerReport);
				std::thread reporter(writeTickerReport);

				tickerReportProducer.join();
				reporter.join();
			}
		}
	}
	catch (...)
	{
		std::cout << "Unhandled Exception";
	}
	return 0;
}