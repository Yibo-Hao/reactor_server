//
// Created by 郝绎博 on 2024/10/26.
//
#include "eventLoop.h"

EventLoop::EventLoop()
: ep_(std::make_unique<Epoll>()), wakeup_fd_(eventfd(0, EFD_NONBLOCK)), wakeup_channel(std::make_unique<Channel>(this, wakeup_fd_))
{
    wakeup_channel->set_read_callback([this] { handle_wakeup(); });
    wakeup_channel->enablereading();
}

EventLoop::~EventLoop()
= default;

void EventLoop::run()
{
    thread_id_ = ::syscall(SYS_gettid);
    while(true)
    {
        std::vector<Channel*> channels = ep_->loop(10*1000);
        if (channels.empty()) {
            epoll_timeout_callback_(this);
        }
        std::for_each(channels.begin(), channels.end(), [](Channel* channel) {
            channel->handle_event();
        });
    }
}

void EventLoop::update_channel(Channel *ch)
{
    ep_->update_channel(ch);
}

void EventLoop::remove_channel(Channel *ch)
{
    ep_->remove_channel(ch);
}

void EventLoop::set_epoll_timeout_callback(std::function<void(EventLoop*)> fn)
{
    epoll_timeout_callback_ = std::move(fn);
}

bool EventLoop::is_in_loop_thread() const
{
    return thread_id_ == ::syscall(SYS_gettid);
}

void EventLoop::queue_in_loop(std::function<void()> fn)
{
    {
        std::lock_guard<std::mutex> lock(mutex_);
        task_queue_.push(std::move(fn));
    }
    wakeup();
}

void EventLoop::wakeup() const
{
    uint64_t one = 1;
    write(wakeup_fd_, &one, sizeof(one));
}

void EventLoop::handle_wakeup()
{
    uint64_t one = 1;
    read(wakeup_fd_, &one, sizeof(one));

    std::function<void()> fn;
    std::lock_guard<std::mutex> lock(mutex_);

    while(!task_queue_.empty())
    {
        fn = std::move(task_queue_.front());
        task_queue_.pop();
        fn();
    }
}