#include "threadPool.h"

ThreadPool::ThreadPool(size_t threadnum):stop_(false) 
{
	for (size_t ii = 0; ii < threadnum; ii++)
    {
		threads_.emplace_back([this]
        {
            printf("create thread(%ld).\n",syscall(SYS_gettid));     // 显示线程ID。
            std::cout << "子线程：" << std::this_thread::get_id() << std::endl;

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

                printf("thread is %ld.\n",syscall(SYS_gettid));
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