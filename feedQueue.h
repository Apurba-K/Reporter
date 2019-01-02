#pragma once

#ifndef _FEED_QUEUE_
#define _FEED_QUEUE_

#include <queue>
#include <mutex>

namespace Reporter
{
	struct FeedQ
	{
		// Thread safe data structure to communicate between threads
		std::queue<std::vector<std::string> > feedq;
		std::mutex qLock;
	};
}

#endif