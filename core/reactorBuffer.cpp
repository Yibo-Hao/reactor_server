//
// Created by 郝绎博 on 2024/10/28.
//

#include "reactorBuffer.h"

Buffer::Buffer()
= default;

Buffer::~Buffer()
= default;

void Buffer::append(const char *data, size_t len) {
    buf_.append(data, len);
}

void Buffer::append_with_head(const char *data, size_t len) {
    buf_.append((char*)&len, 4);
    buf_.append(data, len);
}

size_t Buffer::size() {
    return buf_.size();
}

const char *Buffer::data() {
    return buf_.data();
}

void Buffer::clear() {
    buf_.clear();
}

void Buffer::erase(size_t pos, size_t len) {
    buf_.erase(pos, len);
}