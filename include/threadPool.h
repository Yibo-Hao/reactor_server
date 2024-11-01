#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <queue>
#include <sys/syscall.h>
#include <mutex>
#include <unistd.h>
#include <thread>
#include <condition_variable>
#include <functional>
#include <future>
#include <atomic>
#include <chrono>

class ThreadPool   
{
private:
	std::vector<std::thread> threads_;
	std::queue<std::function<void()>> taskqueue_;
	std::mutex mutex_;
	std::condition_variable condition_;
	std::atomic_bool stop_;
public:
	explicit ThreadPool(size_t threadnum);

    // 把任务添加到队列中。
    void addtask(std::function<void()> task);
    size_t size() const;
    void stop();

    // 在析构函数中将停止线程。
	~ThreadPool();
};