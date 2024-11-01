#pragma once

#include <iostream>
#include <string>
#include <ctime>

class Timestamp
{
private:
    time_t second_since_epoch_;
public:
    Timestamp();
    explicit Timestamp(int64_t second_since_epoch_);

    static Timestamp now();

    time_t to_int() const;
    std::string to_string() const;
};