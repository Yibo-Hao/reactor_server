#include "epoll.h"

Epoll::Epoll()
{
    if ((epoll_fd_ = epoll_create(1)) == -1)
    {
        std::cerr << "epoll_create() failed" << __FILE__ << __FUNCTION__ << __LINE__ << std::endl;
        exit(-1);
    }
}

Epoll::~Epoll()
{
    close(epoll_fd_);
}

void Epoll:: update_channel(Channel* channel) const
{
    epoll_event ev{};
    ev.data.ptr = channel;
    ev.events = channel->events();
    if (channel->inpoll()) {
        if (epoll_ctl(epoll_fd_, EPOLL_CTL_MOD, channel->fd(), &ev) == -1)
        {
            std::cerr << "epoll_ctl(EPOLL_CTL_MOD) failed" << __FILE__ << __FUNCTION__ << __LINE__ << std::endl;
            exit(-1);
        }
    } else {
        if (epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, channel->fd(), &ev) == -1)
        {
            std::cerr << "epoll_ctl(EPOLL_CTL_ADD) failed" << __FILE__ << __FUNCTION__ << __LINE__ << std::endl;
            exit(-1);
        }
        channel->setinepoll();
    }
}


std::vector<Channel *> Epoll::loop(int timeout)
{
    std::vector<Channel *> channels;

    bzero(events_, sizeof(events_));
    int infds = epoll_wait(epoll_fd_, events_, MaxEvents, timeout);

    if (infds < 0)
    {
        std::cerr << "epoll_wait() failed" << __FILE__ << __FUNCTION__ << __LINE__ << std::endl;
        exit(-1);
    }

    if (infds == 0)
    {
        return channels;
    }

    for (int ii = 0; ii < infds; ii++)
    {
        auto* ch = reinterpret_cast<Channel *>(events_[ii].data.ptr);
        ch->setrevents(events_[ii].events);
        channels.push_back(ch);
    }

    return channels;
}

void Epoll::set_timeout_callback(const std::function<void(EventLoop *)> &cb)
{
    timeout_callback_ = cb;
}