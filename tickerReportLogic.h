#pragma once

#ifndef _TICKER_REPORT_LOGIC_
#define _TICKER_REPORT_LOGIC_

#include "feedMetaData.h"
#include "appRowFactory.h"

namespace Reporter
{

	class ReportLogic
	{
	// interface for logic to prepare different report field 
	protected:
		const FeedMetaData::RecordDefination& recordDef;
	public:
		ReportLogic(const FeedMetaData::RecordDefination& recordDef_) : recordDef(recordDef_) {}
		virtual void operator() (const TIckerRowFactory::TickerRecord& record) {}
		virtual std::string get() = 0;
		virtual void refresh() = 0;
	};

	class Spread : public ReportLogic
	{
		// class to calculate Spread
		double minSpread = 10000000000, maxSpread = 0;

	public:
		using ReportLogic::ReportLogic;

		void operator() (const TIckerRowFactory::TickerRecord& record);
		double& getMinSpread();
		double& getMaxSpread();
		void refresh() { minSpread = 10000000000, maxSpread = 0; }
	};

	class MinSpread : public Spread
	{
		// class for min spread
	public:
		using Spread::Spread;
		std::string get() override { return std::to_string(getMinSpread()); }
	};

	class MaxSpread : public Spread
	{
		// class for max spread
	public:
		using Spread::Spread;
		std::string get() override { return std::to_string(getMaxSpread()); }
	};
	
	class SumVolume : public ReportLogic
	{
		// class to identify sum volume
		unsigned long sumVolume = 0;

	public:
		using ReportLogic::ReportLogic;

		void operator() (const TIckerRowFactory::TickerRecord& record);
		std::string get() override;
		void refresh() { sumVolume = 0; }
	};

	class MaxTimeDiff : public ReportLogic
	{
		// class to calculate time difference between tickers
		int maxTmDif = 0;
		int timeLast = 0;
	public:
		using ReportLogic::ReportLogic;

		void operator() (const TIckerRowFactory::TickerRecord& record);
		std::string get() override;
		void refresh() { maxTmDif = 0, timeLast = 0; }
	};

	class Ratio : public ReportLogic
	{
		// class to identify ticker ratio
		double totalSize = 0, totalValue = 0;
	public:
		using ReportLogic::ReportLogic;

		void operator() (const TIckerRowFactory::TickerRecord& record);
		std::string get() override;
		void refresh() { totalSize = 0, totalValue = 0; }
	};

}

#endif