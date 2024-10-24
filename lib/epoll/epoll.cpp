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


void Epoll::add_fd(int fd, uint32_t op) const
{
    epoll_event ev{};
    ev.data.fd = fd;
    ev.events = op;

    if (epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, fd, &ev) == -1)
    {
        std::cerr << "epoll_ctl() failed" << __FILE__ << __FUNCTION__ << __LINE__ << std::endl;
        exit(-1);
    }
}


std::vector<epoll_event> Epoll::loop(int timeout)
{
    std::vector<epoll_event> evs;

    bzero(events_, sizeof(events_));
    int infds = epoll_wait(epoll_fd_, events_, MaxEvents, timeout);

    if (infds < 0)
    {
        std::cerr << "epoll_wait() failed" << __FILE__ << __FUNCTION__ << __LINE__ << std::endl;
    }


    if (infds == 0)
    {
        std::cout << "epoll_wait() timeout." << __FILE__ << __FUNCTION__ << __LINE__ << std::endl;
    }

    for (int ii = 0; ii < infds; ii++)
    {
        evs.push_back(events_[ii]);
    }

    return evs;
}