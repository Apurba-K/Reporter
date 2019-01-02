#pragma once

#include <queue>
#include <unordered_set>
#include <memory>

namespace Reporter
{
	class InComingRowFactory;
	class BasicField;
	class FeedMetaData;

	class FeedParser
	{
		std::queue<std::unordered_set<std::shared_ptr<Reporter::BasicField>>> incomingRow;
		FeedMetaData& metadata;

	public:
		FeedParser(FeedMetaData& metadata_) :metadata(metadata_) {}
		void parse();
	};
}