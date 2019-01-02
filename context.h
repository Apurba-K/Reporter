#pragma once

#ifndef _REPORTER_CONTEXT_
#define _REPORTER_CONTEXT_

#include <atomic>

#include "feedQueue.h"

namespace Reporter
{
	struct Context
	{
		//Class for logical context of communication between different threads
		FeedQ contextQ;
		std::atomic<bool> complete = false;
	};
}

#endif