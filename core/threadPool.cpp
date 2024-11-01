#include "threadPool.h"

ThreadPool::ThreadPool(size_t threadnum):stop_(false) 
{
	for (size_t ii = 0; ii < threadnum; ii++)
    {
		threads_.emplace_back([this]
        {
			while (!stop_)
			{
				std::function<void()> task;
				{
					std::unique_lock<std::mutex> lock(this->mutex_);
					this->condition_.wait(lock, [this]
                    { 
                        return (this->stop_ || !this->taskqueue_.empty());
                    });
					if (this->stop_ && this->taskqueue_.empty()) return;
					task = std::move(this->taskqueue_.front());
					this->taskqueue_.pop();
				}

				task();
			}
		});
    }
}

void ThreadPool::addtask(std::function<void()> task)
{
    {
        std::lock_guard<std::mutex> lock(mutex_);
        taskqueue_.push(task);
    }

    condition_.notify_one();
}

ThreadPool::~ThreadPool()
{
	stop_ = true;

	condition_.notify_all();

	for (std::thread &th : threads_) th.join();
}

size_t ThreadPool::size() const
{
    return threads_.size();
}