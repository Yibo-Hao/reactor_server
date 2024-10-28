//
// Created by 郝绎博 on 2024/10/28.
//

#ifndef REACTOR_REACTORBUFFER_H
#define REACTOR_REACTORBUFFER_H

#include <string>

class Buffer {
private:
    std::string buf_;
public:
    Buffer();
    ~Buffer();

    void append(const char *data, size_t len);
    size_t size();
    const char *data();
    void clear();
};

#endif //REACTOR_REACTORBUFFER_H
