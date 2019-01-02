#include "pch.h"

#include <utility>

#include "feedQueue.h"

void Reporter::FeedQ::push(std::vector<std::string>&& record)
{
	std::unique_lock<std::mutex> lock(qLock);
	feedq.push(std::move(record));
	return;
}

void Reporter::FeedQ::push(const std::vector<std::string>& record)
{
	std::unique_lock<std::mutex> lock(qLock);
	feedq.push(record);
	return;
}

std::vector<std::string>&& Reporter::FeedQ::pop()
{
	std::unique_lock<std::mutex> lock(qLock);
	auto record = feedq.front();
	feedq.pop();
	return std::move(record);
}