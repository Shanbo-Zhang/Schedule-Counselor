//
// Created by Escap on 9/28/2023.
//

#ifndef SCLIB_TIME_H
#define SCLIB_TIME_H

#include "base.h"

class Time {
public:

    Time() : second_(-1), minute_(-1), hour_(-1) {}

    Time(const int &hour, const int &minute, const int &second)
            : hour_(hour), minute_(minute), second_(second) {
//        assert(hour >= 0 && hour < 24);
//        assert(minute >= 0 && minute < 60);
//        assert(second >= 0 && second < 60);
    }

    Time(const time_t &time) noexcept {
        // TODO: TimeZone
    }

    Time(const Time &other) noexcept
            : hour_(other.hour_), minute_(other.minute_), second_(other.second_) {}

    Time(Time &&other) noexcept
            : hour_(other.hour_), minute_(other.minute_), second_(other.second_) {}

    bool IsValid() const noexcept {
        return (hour_ >= 0 && hour_ < 24)
               && (minute_ >= 0 && minute_ < 60)
               && (second_ >= 0 && second_ < 60);
    }

    int Hour() const noexcept {
        return hour_;
    }

    int Minute() const noexcept {
        return minute_;
    }

    int Second() const noexcept {
        return second_;
    }

    Time &SetHour(const int &hour) {
        assert(hour >= 0 && hour < 23);
        hour_ = hour;
        return *this;
    }

    Time &SetMinute(const int &minute) {
        assert(minute >= 0 && minute < 60);
        minute_ = minute;
        return *this;
    }

    Time &SetSecond(const int &second) {
        assert(second >= 0 && second < 60);
        second_ = second;
        return *this;
    }

    Time &AddSecond(const int &second) {
        second_ += second;
        AddMinute(second_ / 60);
        second_ %= 60;
        return *this;
    }

    Time &AddMinute(const int &minute) {
        minute_ += minute;
        AddHour(minute_ / 60);
        minute_ %= 60;
        return *this;
    }

    Time &AddHour(const int &hour) {
        hour_ = (hour_ + hour) % 24;
        return *this;
    }

    static int Compare(const int &left_hour, const int &left_minute, const int &left_second,
                       const int &right_hour, const int &right_minute, const int &right_second) noexcept {
        if (left_hour == right_hour) {
            if (left_minute == right_minute) {
                if (left_second == right_second) {
                    return 0;
                } else {
                    return right_second > left_second ? 1 : -1;
                }
            } else {
                return right_minute > left_minute ? 1 : -1;
            }
        } else {
            return right_hour > left_hour ? 1 : -1;
        }
    }

    static int Compare(const Time &left, const Time &right) noexcept {
        return Time::Compare(left.hour_, left.minute_, left.second_,
                             right.hour_, right.minute_, right.second_);
    }

    int CompareTo(const int &hour, const int &minute, const int &second) const noexcept {
        return Time::Compare(hour_, minute_, second_, hour, minute, second);
    }

    int CompareTo(const Time &other) const noexcept {
        return Time::Compare(*this, other);
    }

    Time &operator=(const Time &other) {
        new(this)Time(other);
        return *this;
    }

private:
    int hour_;
    int minute_;
    int second_;
};

#endif //LIB_TIME_H
