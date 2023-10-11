//
// Created by Escap on 9/27/2023.
//

#ifndef ESCLIB_DATE_H
#define ESCLIB_DATE_H

#include "base.h"

class Date {
public:
    Date() : day_(-1), month_(-1), year_(-1) {}

    Date(const int &year, const int &month, const int &day)
            : year_(year), month_(month), day_(day) {
//        assert(year > 0);
//        assert(month > 0 && month <= 12);
//        assert(day > 0 && day < Date::days_per_month_[IsLeapYear(year_)][month]);
    }

    Date(const Date &other) noexcept
            : year_(other.year_), month_(other.month_), day_(other.day_) {}

    Date(Date &&other) noexcept
            : year_(other.year_), month_(other.month_), day_(other.day_) {}

    static bool IsLeapYear(const int &year) {
        return ((year % 100 != 0) && (year % 4) == 0) || (year % 400 == 0);
    }

    bool IsValid() const noexcept {
        return year_ > 0
               && (month_ > 0 && month_ <= 12)
               && (day_ > 0 && day_ < Date::days_per_month_[IsLeapYear(year_)][month_]);
    }

    int Year() const noexcept {
        return year_;
    }

    int Month() const noexcept {
        return month_;
    }

    int Day() const noexcept {
        return day_;
    }

    Date &SetYear(const int &year) {
        assert(year > 0);
        year_ = year;
        return *this;
    }

    Date &SetMonth(const int &month) {
        assert(month > 0 && month < 12);
        month_ = month;
        return *this;
    }

    Date &SetDay(const int &day) {
        assert(day > 0 && day <= Date::days_per_month_[IsLeapYear(year_)][month_]);
        day_ = day;
        return *this;
    }

    Date &AddDay(const int &day) {
        day_ += day;
        while (day_ > Date::days_per_month_[IsLeapYear(year_)][month_]) {
            day_ -= Date::days_per_month_[IsLeapYear(year_)][month_];
            ++month_;
            if (month_ > 12) {
                month_ = 1;
                ++year_;
            }
        }
        return *this;
    }

    Date &AddMonth(const int &month) {
        month_ += month;
        AddYear(month / 12);
        month_ = month_ % 12;
        if (day_ > Date::days_per_month_[IsLeapYear(year_)][month_]) {
            day_ = Date::days_per_month_[IsLeapYear(year_)][month_];
        }
        return *this;
    }

    Date &AddYear(const int &year) {
        year_ += year;
        return *this;
    }

    static int Compare(const int &left_year, const int &left_month, const int &left_day,
                       const int &right_year, const int &right_month, const int &right_day) noexcept {
        if (left_year == right_year) {
            if (left_month == right_month) {
                if (left_day == right_day) {
                    return 0;
                } else {
                    return right_day > left_day ? 1 : -1;
                }
            } else {
                return right_month > left_month ? 1 : -1;
            }
        } else {
            return right_year > left_year ? 1 : -1;
        }
    }

    static int Compare(const Date &left, const Date &right) noexcept {
        return Date::Compare(left.year_, left.month_, left.day_,
                             right.year_, right.month_, right.day_);
    }

    int CompareTo(const int &year, const int &month, const int &day) const noexcept {
        return Date::Compare(year_, month_, day_, year, month, day);
    }

    int CompareTo(const Date &other) const noexcept {
        return Date::Compare(*this, other);
    }

    Date &operator=(const Date &other) {
        new(this)Date(other);
        return *this;
    }

private:
    int day_;
    int month_;
    int year_;

    static int days_per_month_[2][12];
};

int Date::days_per_month_[2][12] = {
        {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},
        {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}
};

#endif //ESCLIB_DATE_H
