#pragma once

#include <limits>
#include <cstdint>


namespace foo {
#pragma warning(push)
#pragma warning(disable: 4244)

    /** @brief A class that represents a time value. */
    class Time {
    public:

        /** @brief Returns the high-resolution current system time. */
        static Time now();

        /** @brief Returns a zero time. */
        static constexpr Time zero() { return 0; }

        /** @brief Returns the max representable time. */
        static constexpr Time max() { return std::numeric_limits<int64_t>::max(); }

        /** @brief Returns the min representable time. */
        static constexpr Time min() { return std::numeric_limits<int64_t>::min(); }

        /** @brief Default constructor. */
        Time() = default;

    public:

        /** @brief Returns the time in seconds. */
        constexpr double  seconds()      const { return usec / 1000000.0; }

        /** @brief Returns the time in milliseconds. */
        constexpr int     milliseconds() const { return usec / 1000; }

        /** @brief Returns the time in microseconds. */
        constexpr int64_t microseconds() const { return usec; }

    public:

        constexpr Time operator-() const {
            return -usec;
        }

        Time& operator+=(const Time& tm) {
            usec += tm.usec;
            return *this;
        }

        Time& operator-=(const Time& tm) {
            usec -= tm.usec;
            return *this;
        }

        Time& operator*=(double amt) {
            usec *= amt;
            return *this;
        }

        Time& operator/=(double amt) {
            usec /= amt;
            return *this;
        }

        Time& operator%=(const Time& tm) {
            usec %= tm.usec;
            return *this;
        }

    private:

        constexpr Time(int64_t usec) noexcept
            : usec(usec) {

        }

        int64_t usec;

        friend constexpr Time seconds(double);
        friend constexpr Time milliseconds(int);
        friend constexpr Time microseconds(int64_t);

    };

    // Time Construction

    //! @brief Constructs time with a value expressed in seconds.
    inline constexpr Time seconds(double amount) {
        return static_cast<int64_t>(amount * 1000000);
    }

    //! @brief Constructs time with a value expressed in milliseconds.
    inline constexpr Time milliseconds(int amount) {
        return static_cast<int64_t>(amount) * 1000;
    }

    //! @brief Constructs time with a value expressed in microseconds.
    inline constexpr Time microseconds(int64_t amount) {
        return amount;
    }

    // Relational Operators

    inline constexpr bool operator==(const Time& lhs, const Time& rhs) {
        return lhs.microseconds() == rhs.microseconds();
    }

    inline constexpr bool operator!=(const Time& lhs, const Time& rhs) {
        return !(lhs == rhs);
    }

    inline constexpr bool operator>(const Time& lhs, const Time& rhs) {
        return lhs.microseconds() > rhs.microseconds();
    }

    inline constexpr bool operator<(const Time& lhs, const Time& rhs) {
        return lhs.microseconds() < rhs.microseconds();
    }

    inline constexpr bool operator>=(const Time& lhs, const Time& rhs) {
        return !(lhs < rhs);
    }

    inline constexpr bool operator<=(const Time& lhs, const Time& rhs) {
        return !(lhs > rhs);
    }

    // Arithmetic Operators

    inline constexpr Time operator+(const Time& lhs, const Time& rhs) {
        return microseconds(lhs.microseconds() + rhs.microseconds());
    }

    inline constexpr Time operator-(const Time& lhs, const Time& rhs) {
        return microseconds(lhs.microseconds() - rhs.microseconds());
    }

    inline constexpr Time operator*(const Time& lhs, double rhs) {
        return microseconds(lhs.microseconds() * rhs);
    }

    inline constexpr Time operator/(const Time& lhs, double rhs) {
        return microseconds(lhs.microseconds() / rhs);
    }

    inline constexpr double operator/(const Time& lhs, const Time& rhs) {
        return lhs.microseconds() / static_cast<double>(rhs.microseconds());
    }

    inline constexpr Time operator%(const Time& lhs, const Time& rhs) {
        return microseconds(lhs.microseconds() % rhs.microseconds());
    }

#ifdef FOO_INCLUDE_UDL

    inline constexpr Time operator "" _sec(long double seconds_) {
        return seconds(seconds_);
    }

    inline constexpr Time operator "" _sec(unsigned long long int seconds_) {
        return seconds(seconds_);
    }

    inline constexpr Time operator "" _msec(unsigned long long int milliseconds_) {
        return milliseconds(milliseconds_);
    }

    inline constexpr Time operator "" _usec(unsigned long long int microseconds_) {
        return microseconds(microseconds_);
    }

#undef FOO_INCLUDE_UDL
#endif

#pragma warning(pop)

}