//
// Created by 郝绎博 on 2024/10/26.
//

#ifndef REACTOR_EVENTLOOP_H
#define REACTOR_EVENTLOOP_H

#include <thread>
#include <utility>
#include <sys/syscall.h>
#include <sys/eventfd.h>
#include <unistd.h>
#include <functional>
#include <queue>
#include <mutex>
#include <unistd.h>
#include <sys/timerfd.h>

#include "epoll.h"
#include "channel.h"

class Epoll;
class Channel;

class EventLoop : public std::enable_shared_from_this<EventLoop> {
private:
    std::unique_ptr<Epoll> ep_;
    std::function<void(EventLoop*)> epoll_timeout_callback_;
    std::queue<std::function<void()>> task_queue_;
    std::mutex mutex_;
    pid_t thread_id_{};
    int wakeup_fd_;
    std::unique_ptr<Channel> wakeup_channel_;
    int timer_fd_;
    bool main_loop_;
    std::unique_ptr<Channel> timer_channel_;
public:
    explicit EventLoop(bool main_loop);
    ~EventLoop();

    void run();
    void update_channel(Channel *ch);
    void remove_channel(Channel *ch);
    bool is_in_loop_thread() const;
    void set_epoll_timeout_callback(std::function<void(EventLoop*)> fn);
    void queue_in_loop(std::function<void()> fn);
    void wakeup() const;
    void handle_wakeup();
    void handle_timer() const;
};

#endif //REACTOR_EVENTLOOP_H
