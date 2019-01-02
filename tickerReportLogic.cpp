
#include "pch.h"
#include "tickerReportLogic.h"

#include <iostream>

void Reporter::Spread::operator() (const TIckerRowFactory::TickerRecord& record)
{
	double ask = 0, bid = 0, spread = 0;

	ask = std::stod(record[recordDef.fieldDef.at("Ask").second]);
	bid = std::stod(record[recordDef.fieldDef.at("Bid").second]);
	spread = ask - bid;

	if (spread > maxSpread)
		maxSpread = spread;

	if (spread < minSpread)
		minSpread = spread;

	return;
}

double& Reporter::Spread::getMaxSpread() { return maxSpread; }
double& Reporter::Spread::getMinSpread() { return minSpread; }

void Reporter::SumVolume::operator() (const TIckerRowFactory::TickerRecord& record)
{
	unsigned long volume = 0;

	volume = std::stoul(record[recordDef.fieldDef.at("Volume").second]);
	sumVolume += volume;

	return;
}

std::string Reporter::SumVolume::get() { return std::to_string(sumVolume); }

void Reporter::MaxTimeDiff::operator() (const TIckerRowFactory::TickerRecord& record)
{
	int time = std::stoi(record[recordDef.fieldDef.at("Timestamp").second]);
	int tmDiff = time - timeLast;

	if (tmDiff > maxTmDif)
		maxTmDif = tmDiff;
	return;
}

std::string Reporter::MaxTimeDiff::get() { return std::to_string(maxTmDif); }

void Reporter::Ratio::operator() (const TIckerRowFactory::TickerRecord& record)
{
	double bidSize = 0, askSize = 0, ask = 0, bid = 0;

	ask = std::stod(record[recordDef.fieldDef.at("Ask").second]);
	bid = std::stod(record[recordDef.fieldDef.at("Bid").second]);
	bidSize = std::stod(record[recordDef.fieldDef.at("BidSize").second]);
	askSize = std::stod(record[recordDef.fieldDef.at("AskSize").second]);

	totalSize += bidSize + askSize;
	totalValue += (bid * askSize) + (ask * bidSize);
}

std::string  Reporter::Ratio::get()
{
	return std::to_string(totalValue / totalSize);
}