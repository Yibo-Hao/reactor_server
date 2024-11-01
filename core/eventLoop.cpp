//
// Created by 郝绎博 on 2024/10/26.
//
#include "eventLoop.h"

int create_timer_fd(int sec = 30)
{
    int tfd=timerfd_create(CLOCK_MONOTONIC,TFD_CLOEXEC|TFD_NONBLOCK);
    struct itimerspec timeout{};
    memset(&timeout,0,sizeof(struct itimerspec));
    timeout.it_value.tv_sec = sec;
    timeout.it_value.tv_nsec = 0;
    timerfd_settime(tfd, 0, &timeout, nullptr);
    return tfd;
}

EventLoop::EventLoop(bool main_loop)
: ep_(std::make_unique<Epoll>()), wakeup_fd_(eventfd(0, EFD_NONBLOCK)),
wakeup_channel_(std::make_unique<Channel>(this, wakeup_fd_)), timer_fd_(create_timer_fd()),
timer_channel_(std::make_unique<Channel>(this, timer_fd_)), main_loop_(main_loop)
{
    wakeup_channel_->set_read_callback([this] { handle_wakeup(); });
    wakeup_channel_->enablereading();

    timer_channel_->set_read_callback([this] { handle_timer(); });
    timer_channel_->enablereading();
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

void EventLoop::handle_timer() const
{
    struct itimerspec timeout{};
    memset(&timeout, 0, sizeof(struct itimerspec));
    timeout.it_value.tv_sec = 5;
    timeout.it_value.tv_nsec = 0;
    timerfd_settime(timer_fd_, 0, &timeout, 0);
    if (main_loop_) {
        std::cout << "timer is triggered" << std::endl;
    }
}