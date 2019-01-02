#include "pch.h"

#include <iostream>
#include <fstream>
#include <strstream>


#include "appRowFactory.h"
#include "feedMetaData.h"
#include "context.h"
#include "feedQueue.h"
#include "tickerReportLogic.h"

void Reporter::InComingRowFactory::storeRow(Context& outContext, std::string& line)
{
	int start = 0, end = 0;
	std::vector<std::string> record(metadata.getTotalFieldCt());
	std::string delimeter = metadata.getFeedDelimeter();

	if (!line.empty())
	{
		start = end = 0;
		record.clear();

		while ((end = line.find(delimeter, start)) != std::string::npos)
		{
			record.push_back(std::string(line, start, end - start));
			start = end + delimeter.size();
		}

		record.push_back(std::string(line, start));

		if (record.size() != metadata.getTotalFieldCt())
		{
			std::cout << "INCOMING: record with not sufficient fields[" + line + "]";
			return;
		}

		{
			std::unique_lock<std::mutex> lock(outContext.contextQ.qLock);
			outContext.contextQ.feedq.push(std::move(record));
		}
	}
	return;
}

void Reporter::InComingRowFactory::prepareFeed(Context& outContext)
{
	std::fstream incoming(metadata.getFeedFile());
	if (!incoming)
	{
		std::cout << "INCOMING: failed to read file[" + metadata.getFeedFile() + "]";
		outContext.complete = true;
		return;
	}

	std::string buffer(1000, '\0'), line;
	buffer.assign(buffer.size(), '\0');
	
	while (incoming)
	{
		incoming.read(&buffer[0], buffer.size());
		int lineStart = 0, lineEnd = 0;

		while ((lineEnd = buffer.find("\n", lineStart)) != std::string::npos)
		{
			line += std::string(buffer, lineStart, lineEnd - lineStart);
			lineStart = lineEnd + 1;

			storeRow(outContext, line);
			line = "";
		}
		line = std::string(buffer, lineStart);
		buffer.assign(buffer.size(), '\0');
	}
	storeRow(outContext, line);

	outContext.complete = true;

	std::cout << "INCOMING: Feed receive completed\n";
	return;
}

Reporter::TIckerRowFactory::TIckerRowFactory(const Reporter::FeedMetaData& metadata1) : inMetadata(metadata1)
{
	auto& inRecordDef = inMetadata.getRecordDefination();
	if (inRecordDef.fieldDef.find("Ticker") == inRecordDef.fieldDef.end())
	{
		std::cout << "TICKER_FACTORY: incoming feed metadata does not have ticker \n";
		throw(std::string("Invalid MetaData"));
	}
	tickerPos = inRecordDef.fieldDef.at("Ticker").second;
}

void Reporter::TIckerRowFactory::prepareFeed(Context& inContext)
{
	bool feedToProcess = true;

	while (feedToProcess)
	{
		TickerRecord tickerRecord;
		{
			std::unique_lock<std::mutex> ulock(inContext.contextQ.qLock);
			if (!inContext.contextQ.feedq.empty())
			{
				tickerRecord = inContext.contextQ.feedq.front();
				inContext.contextQ.feedq.pop();
			}
			else
			{
				if (inContext.complete)
					feedToProcess = false;
				continue;
			}
		}

		auto& ticker = tickerRecord[tickerPos];
		if (tickerData.find(ticker) != tickerData.end())
			tickerData[ticker].push_back(tickerRecord);
		else
		{
			TickerRecordStore tickerDataStore;
			tickerDataStore.push_back(tickerRecord);
			tickerData.insert(std::make_pair(ticker, tickerDataStore));
		}
	}
	std::cout << "TICKER-DATA: Ticker row mapping completed\n";
	return;
}

Reporter::TickerReportRowFactory::TickerReportRowFactory(const FeedMetaData& metadata1, const FeedMetaData& metadata2, int thrdCount, const TIckerRowFactory::TickerData& tickerData_) :
	incomingMetadata(metadata1), reportMetaData(metadata2), numberOfThread(thrdCount), tickerData(tickerData_)
{}
	
void Reporter::TickerReportRowFactory::prepareReport(Reporter::Context& outContext, const Reporter::TIckerRowFactory::TickerRecordStore& tickerRecordStore, ReportLogicStore& reportLogic)
{
	std::vector<std::string> outRecord(reportMetaData.getTotalFieldCt());
	bool wrongRecord = false;

	const FeedMetaData::RecordDefination& inRecordDef = incomingMetadata.getRecordDefination();
	const FeedMetaData::RecordDefination& reportRecDef = reportMetaData.getRecordDefination();

	for (auto& record : tickerRecordStore)
	{
		wrongRecord = false;

		for (auto& field : reportRecDef.recordFields)
		{
			if (inRecordDef.fieldDef.find(field) != inRecordDef.fieldDef.end())
				continue;

			try
			{
				reportLogic[field]->operator() (record);
			}
			catch (std::invalid_argument& e)
			{
				wrongRecord = true;
				std::cout << e.what() << "\n";
			}
			catch (std::out_of_range& e)
			{
				wrongRecord = true;
				std::cout << e.what() << "\n";
			}

			if (wrongRecord)
			{
				std::cout << "REPORT-ROW: invalid input record[";
				for (auto& elem : record)
					std::cout << elem << incomingMetadata.getFeedDelimeter();
				std::cout << "]" << std::endl;
				break;
			}
		}
	}

	int indx = 0;
	for (auto& field : reportRecDef.recordFields)
		if (inRecordDef.fieldDef.find(field) != inRecordDef.fieldDef.end())
			outRecord[indx++] = tickerRecordStore[0][inRecordDef.fieldDef.at(field).second];
		else
		{
			std::string val = reportLogic[field]->get();
			outRecord[indx++] = val;
		}

	{
		std::unique_lock<std::mutex> lock(outContext.contextQ.qLock);
		outContext.contextQ.feedq.push(outRecord);
	}

	std::string msg = "TICKER-REPORT-DATA: produced for ticker[" + tickerRecordStore[0][inRecordDef.fieldDef.at("Ticker").second] + "]\n";
	std::cout << msg;
	return;
}

void Reporter::TickerReportRowFactory::prepareFeed(std::mutex& tickerLock, TIckerRowFactory::TickerData::const_iterator& tickerIterator, Context& outContext)
{
	TIckerRowFactory::TickerData::const_iterator local;
	/*
	std::strstream idMsg;
	idMsg << std::this_thread::get_id();
	std::string msg;
	*/

	ReportLogicStore reportLogic;
	reportLogic.insert(std::make_pair(std::string("MinSpread"), std::shared_ptr<ReportLogic>(static_cast<ReportLogic*>(new MinSpread(incomingMetadata.getRecordDefination())))));
	reportLogic.insert(std::make_pair(std::string("MaxSpread"), std::shared_ptr<ReportLogic>(static_cast<ReportLogic*>(new MaxSpread(incomingMetadata.getRecordDefination())))));
	reportLogic.insert(std::make_pair(std::string("SumVolume"), std::shared_ptr<ReportLogic>(static_cast<ReportLogic*>(new SumVolume(incomingMetadata.getRecordDefination())))));
	reportLogic.insert(std::make_pair(std::string("MaxTimeDiff"), std::shared_ptr<ReportLogic>(static_cast<ReportLogic*>(new MaxTimeDiff(incomingMetadata.getRecordDefination())))));
	reportLogic.insert(std::make_pair(std::string("Ratio"), std::shared_ptr<ReportLogic>(static_cast<ReportLogic*>(new Ratio(incomingMetadata.getRecordDefination())))));

	bool completed = false;

	do
	{
		{
			std::unique_lock<std::mutex> lock(tickerLock);
			if (tickerIterator != tickerData.end())
			{
				local = tickerIterator;
				++tickerIterator;
			}
			else
				completed = true;
		}

		if (!completed)
		{
			for (auto logic = reportLogic.begin(); logic != reportLogic.end(); ++logic)
				logic->second->refresh();

			prepareReport(outContext, local->second, reportLogic);
			tickerData.erase(local);
		}
	} while (!completed);
	return;
}
void Reporter::TickerReportRowFactory::prepareFeed(Context& outContext)
{
	std::mutex tickerLock;

	if (tickerData.empty())
	{
		outContext.complete = true;
		return;
	}

	auto tickerIterator = tickerData.begin();
	
	std::vector<std::thread> reporters;

	for (int indx = 0; indx < numberOfThread; ++indx)
	{
		auto reportFn = [this, &tickerLock, &tickerIterator, &outContext]()
		{
			prepareFeed(tickerLock, tickerIterator, outContext);
			return;
		};

		std::thread reporterTh(reportFn);
		reporters.push_back(std::move(reporterTh));
	}
	 
	for (auto& th : reporters)
		th.join();

	outContext.complete = true;

	std::cout << "TICKER-REPORT: report data preparation completed\n";

	return;
}



	