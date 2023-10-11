//
// Created by Escap on 10/9/2023.
//

#ifndef ESCAPIST_FLAG_H
#define ESCAPIST_FLAG_H

#include "base.h"
#include <type_traits>
#include <cstdarg>

template<typename Enum>
class Flag {
public:
    using Int = typename std::underlying_type<Enum>::type;

    Flag() noexcept: value_(0) {}

    explicit Flag(const Enum &value) noexcept: value_(Int(value)) {}

    explicit Flag(const Int &value) noexcept: value_(value) {}

    Flag(const Flag<Enum> &other) noexcept: value_(other.value_) {}

    ~Flag() = default;

    Flag<Enum> &AddFlag(const Enum &flag) noexcept {
        value_ |= static_cast<Int>(flag);
        return *this;
    }

    Flag<Enum> &AddFlags(SizeType count, ...) noexcept {
        if (count > 0) {
            va_list argv;
            va_start(argv, count);
            for (; count; --count) {
                value_ |= static_cast<Int>(va_arg(argv, Enum));
            }
            va_end(argv);
        }
        return *this;
    }

    Flag<Enum> &RemoveFlag(const Enum &flag) noexcept {
        value_ ^= static_cast<Int>(flag);
        return *this;
    }

    Flag<Enum> &RemoveFlags(SizeType count, ...) noexcept {
        if (count > 0) {
            va_list argv;
            va_start(argv, count);
            for (; count; --count) {
                value_ ^= static_cast<Int>(va_arg(argv, Enum));
            }
            va_end(argv);
        }
        return *this;
    }

    bool HasFlag(const Enum &flag) const noexcept {
        return (value_ | static_cast<Int>(flag)) == value_;
    }

    bool HasAnyFlags(SizeType count, ...) const noexcept {
        if (count > 0) {
            va_list argv;
            va_start(argv, count);
            for (; count; --count) {
                if ((value_ | static_cast<Int>(va_arg(argv, Enum))) == value_) {
                    return true;
                }
            }
            va_end(argv);
        }
        return false;
    }

    bool HasAnyFlags(const Int &other) const noexcept {
        return (value_ | other) == value_;
    }

    bool HasAnyFlags(const Flag<Enum> &other) const noexcept {
        return (value_ & other.value_) != Int(0); // Reference: QFlag in Qt Framework
    }

    bool HasAllFlags(SizeType count, ...) const noexcept {
        if (count > 0) {
            va_list argv;
            va_start(argv, count);
            for (; count; --count) {
                if ((value_ | static_cast<Int>(va_arg(argv, Enum))) != value_) {
                    return false;
                }
            }
            va_end(argv);
            return true;
        }
        return false;
    }

    bool HasAllFlags(const Int &other) const noexcept {
        return (value_ | other) == value_;
    }

    bool HasAllFlags(const Flag<Enum> &other) const noexcept {
        return (value_ | other.value_) == value_;
    }

private:
    Int value_;
};

#endif //ESCAPIST_FLAG_H
