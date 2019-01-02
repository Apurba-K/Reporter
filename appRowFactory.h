#pragma once

#ifndef _APP_ROW_FACTORY_
#define _APP_ROW_FACTORY_

#include <vector>
#include <map>
#include <mutex>

namespace Reporter
{
	class FeedMetaData;
	struct Context;
	class ReportLogic;
	
	class RowFactory
	{
		// Interface for Application Data Factory
	public:
		virtual void prepareFeed(Context& outContext) = 0; // methods to produce data
	};

	class InComingRowFactory : public RowFactory
	{
		// Class which interacts with input I/O and read raw data
		const FeedMetaData& metadata;
		void storeRow(Context& outContext, std::string& line); // methods to store raw data in Context
	public:
		InComingRowFactory(const FeedMetaData& metadata_) : metadata(metadata_) {}
		void prepareFeed(Context& outContext) override; // method to produce raw application data
	};

	class TIckerRowFactory : public RowFactory
	{
		// Class to map Ticker data to appropriate storage
	public:
		
		typedef std::vector<std::string> TickerRecord;
		typedef std::vector<TickerRecord> TickerRecordStore;
		typedef std::map<std::string, TickerRecordStore> TickerData;

		TIckerRowFactory(const FeedMetaData& metadata1);
		void prepareFeed(Context& inContext) override;    // method to map ticker data in ticker storage             
		TickerData&& getTickerData() { return std::move(tickerData); } // method to retrieve ticker storage

	private:
		int tickerPos;
		const FeedMetaData& inMetadata;
		TickerData tickerData;
	};

	class TickerReportRowFactory : public RowFactory
	{
		// Class to produce ticker report data
	public:
		TickerReportRowFactory(const FeedMetaData& metadata1, const FeedMetaData& metadata2, int thrdCount, const TIckerRowFactory::TickerData& tickerData_);
				
		void prepareFeed(Context& outContext) override;      // method to produce report from tickerdata store and store same in context
	private:
		
		typedef std::map<std::string, std::shared_ptr<ReportLogic> > ReportLogicStore;
		void prepareFeed(std::mutex& tickerLock, TIckerRowFactory::TickerData::const_iterator& tickerIterator, Context& outContext);  // method for ticker report generation through different reducer thread 
		void prepareReport(Context& outContext, const TIckerRowFactory::TickerRecordStore& tickerDataStore, ReportLogicStore& reportLogic);  // method to produce ticker report

		const FeedMetaData& incomingMetadata;
		const FeedMetaData& reportMetaData;

		int numberOfThread;
		TIckerRowFactory::TickerData tickerData;
	};
}

#endif
