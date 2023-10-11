/*
 *
 */

#ifndef ESCAPIST_STRING_H
#define ESCAPIST_STRING_H

#include "base.h"
#include "internal/ref_count.h"
#include <type_traits>
#include <memory>
#include <cstring>
#include <cstdlib>
#include <iostream>

template<typename Ch>
struct ICharTrait {
    /**
     * Copy the first {count} of characters from {src} to {dest}
     * @param dest the place to be copied.
     * @param src the source
     * @param count how many characters to be copied.
     */
    static inline void Copy(Ch *dest, const Ch *src, SizeType count) {
        assert(dest && src && count);
        for (; count > 0; ++dest, ++src, --count) {
            *dest = *src;
        }
    }

    /**
     * Copy the first {count} of characters from {src} to {dest},
     * but allow {dest} and {src} to overlap
     * @param dest the place to be copied.
     * @param src the source
     * @param count how many characters to be copied.
     */
    static inline void Move(Ch *dest, const Ch *src, SizeType count) {
        assert(dest && src && count);
        if ((dest <= src) || (dest > src + count)) {
            for (; count; ++dest, ++src, --count) {
                *dest = *src;
            }
        } else {
            dest = dest + count - 1;
            src = src + count - 1;
            for (; count; --dest, --src, --count)
                *dest = *src;
        }
    }

    /**
     * Fill [val] character [count] times to [dest]
     * @param dest destination, the address to be filled.
     * @param val character to fill {dest}
     * @param count how many {val} to be filled.
     */
    static inline void Fill(Ch *dest, const Ch &val, SizeType count) {
        for (; count > 0; ++dest, --count) {
            *dest = val;
        }
    }

    /**
     * @param src null-terminated string
     * @return the length if the given string
     */
    static inline SizeType Length(const Ch *src) {
        SizeType len = 0;
        for (; *src; ++src, ++len);
        return len;
    }

    /**
     * Compare between {left} string and {right} string.
     * @param left the first null-terminated string
     * @param right the second null-terminated string
     * @return zero two strings are equal
     */
    static inline int Compare(const Ch *left, const Ch *right) {
        assert(left && right);
        if (left == right) {
            return 0;
        }
        for (; *left && *right && *left == *right; ++left, ++right);
        return *left - *right;
    }

    /**
     * Compare the first {count} characters between {left} string and {right} string.
     * @param left the first null-terminated string
     * @param right the second null-terminated string
     * @param count the maximum count of compared characters.
     * @return zero if the first {count} of characters of two strings are equal
     */
    static inline int Compare(const Ch *left, const Ch *right, SizeType count) {
        assert(left && right && count);
        // Terminate if some strings terminate,
        // or the one does not equal to another,
        // or the maximum count reaches.
        for (; *left && *right && *left == *right && count; ++left, ++right, --count);
        return *left - *right;
    }

    /**
     * Compare between {left} string and {right} string,
     * but ignore the case.
     * @param left the first null-terminated string
     * @param right the second null-terminated string
     * @return zero two strings are equal
     * TODO: find a more efficient way!!!
     */
    static inline int CompareNoCase(const Ch *left, const Ch *right) {
        assert(left && right);
        if (left == right) {
            return 0;
        }
        for (; *left && *right && tolower(*left) == tolower(*right); ++left, ++right);
        return *left - *right;
    }

    /**
     * Compare the first \p count characters between {left} string and {right} string,
     * but ignore the case.
     * @param left the first null-terminated string
     * @param right the second null-terminated string
     * @param count the maximum count of compared characters.
     * @return zero if the first {count} of characters of two strings are equal
     * TODO: find a more efficient way!!!
     */
    static inline int CompareNoCase(const Ch *left, const Ch *right, SizeType count) {
        assert(left && right);
        for (; *left && *right && tolower(*left) == tolower(*right) && count; ++left, ++right, --count);
        return *left - *right;
    }

    /**
     * Find the first occurrence of \p val in the null-terminated string \p src.
     * @param src the null-terminated string
     * @param val the character to be searched in \p src
     * @return pointer to found character if found; nullptr if no such character is found.
     */
    static inline const Ch *Find(const Ch *src, const Ch &val) {
        assert(src);
        for (; *src; ++src) {
            if (*src == val) {
                return src;
            }
        }
        return nullptr;
    }

    /**
     * Find the first occurrence of \p val in the initial \p count of the null-terminated string \p src.
     * @param src the null-terminated string
     * @param val the character to be searched in \p src
     * @param count maximum amount of characters to examine.
     * @return pointer to found character if found; nullptr if no such character is found.
     */
    static inline const Ch *Find(const Ch *src, const Ch &val, SizeType count) {
        assert(src && count);
        for (; *src && count; ++src, --count) {
            if (*src == val) {
                return src;
            }
        }
        return nullptr;
    }

    /**
     * Find the first occurrence of \p val in the null-terminated string \p src.
     * @param src the null-terminated string
     * @param sub the null-terminated string to be searched in \p src
     * @return pointer to the address of the first occurrence of \p sub if found; nullptr if no such character is found.
     */
    static inline const Ch *Find(const Ch *src, const Ch *sub) {
        assert(src);
        if (!sub) {
            return nullptr;
        }
        for (; *src; ++src) {
            if (*src == *sub) { // the first char occurs, start to check
                const Ch *src_ref = src, // prevent {src} from changing to ensure every char is scanned.
                *sub_ref = sub; // also make a copy of {sub} to go back if it doesn't match.
                for (; *src_ref && *sub_ref && *src_ref == *sub_ref; ++src_ref, ++sub_ref);
                if (!*sub_ref) { // finished scanning, and all char in sub matched.
                    return src; // Thus, return it!
                } // Otherwise, src has terminated, or they do not match at all.
            }
        }
        return nullptr;
    }

    /**
     * Find the first occurrence of \p sub in the initial \p count of the null-terminated string \p src.
     * @param src the null-terminated string
     * @param sub the null-terminated string to be searched in \p src
     * @param count
     * @return pointer to the address of the first occurrence of \p sub if found; nullptr if no such character is found.
     */
    static inline const Ch *Find(const Ch *src, const Ch *sub, SizeType count) {
        assert(src && count);
        if (!sub) {
            return nullptr;
        }
        for (; *src && count; ++src, --count) {
            if (*src == *sub) { // the first char occurs, start to check
                const Ch *src_ref = src, // prevent {src} from changing to ensure every char is scanned.
                *sub_ref = sub; // also make a copy of {sub} to go back if it doesn't match.
                SizeType count_ref = count;
                for (; *src_ref && *sub_ref && *src_ref == *sub_ref && count_ref; ++src_ref, ++sub_ref, --count_ref);
                if (!*sub_ref) { // finished scanning, and all char in sub matched.
                    return src; // Thus, return it!
                } // Otherwise, src has terminated, or they do not match at all.
            }
        }
        return nullptr;
    }

    /**
     * Find the last occurrence of \p val in the null-terminated string \p src.
     * @param src the null-terminated string
     * @param val the character to be searched in \p src
     * @return pointer to found character if found; nullptr if no such character is found.
     */
    static inline const Ch *ReverseFind(const Ch *src, const Ch &val) {
        assert(src);
        const Ch *last = nullptr;
        for (; *src; ++src) {
            if (*src == val) {
                last = src;
            }
        }
        return last;
    }

    /**
     * Find the last occurrence of \p val in the initial \p count of the null-terminated string \p src.
     * @param src the null-terminated string
     * @param val the character to be searched in \p src
     * @param count maximum amount of characters to examine.
     * @return pointer to found character if found; nullptr if no such character is found.
     */
    static inline const Ch *ReverseFind(const Ch *src, const Ch &val, SizeType count) {
        assert(src && count);
        const Ch *last = nullptr;
        for (; *src && count; ++src, --count) {
            if (*src == val) {
                last = src;
            }
        }
        return last;
    }

    /**
     * Find the first occurrence of \p val in the null-terminated string \p src.
     * @param src the null-terminated string
     * @param sub the null-terminated string to be searched in \p src
     * @return pointer to the address of the first occurrence of \p sub if found; nullptr if no such character is found.
     */
    static inline const Ch *ReverseFind(const Ch *src, const Ch *sub) {
        assert(src);
        if (!sub) {
            return nullptr;
        }
        const Ch *last = nullptr;
        for (; *src; ++src) {
            if (*src == *sub) { // the first char occurs, start to check
                const Ch *src_ref = src,
                        *sub_ref = sub;
                for (; *src_ref && *sub_ref && *src_ref == *sub_ref; ++src_ref, ++sub_ref);
                if (!*sub_ref) { // finished scanning, and all char in sub matched.
                    last = src;
                } // Otherwise, src has terminated, or they do not match at all.
            }
        }
        return last;
    }

    /**
     * Find the first occurrence of \p sub in the initial \p count of the null-terminated string \p src.
     * @param src the null-terminated string
     * @param sub the null-terminated string to be searched in \p src
     * @param count
     * @return pointer to the address of the first occurrence of \p sub if found; nullptr if no such character is found.
     */
    static inline const Ch *ReverseFind(const Ch *src, const Ch *sub, SizeType count) {
        assert(src && count);
        if (!sub) {
            return nullptr;
        }
        const Ch *last = nullptr;
        for (; *src && count; ++src, --count) {
            if (*src == *sub) { // the first char occurs, start to check
                const Ch *src_ref = src,
                        *sub_ref = sub;
                SizeType count_ref = count;
                for (; *src_ref && *sub_ref && *src_ref == *sub_ref && count_ref; ++src_ref, ++sub_ref, --count_ref);
                if (!*sub_ref) { // finished scanning, and all char in sub matched.
                    last = src;
                } // Otherwise, src has terminated, or they do not match at all.
            }
        }
        return last;
    }

    // TODO: Find & ReverseFind Ignore Case
    // TODO: NthFind & NthReverseFind & Ignore Case

    /**
     * Reverse the given null-terminated string \p src
     * @param src given null-terminated string
     */
    static inline void Reverse(Ch *src) {
        ICharTrait<Ch>::Reverse(src, ICharTrait<Ch>::Length(src));
    }

    /**
     * Reverse the first \p count of characters of the given null-terminated string \p src
     * @param src given null-terminated string
     */
    static inline void Reverse(Ch *src, SizeType count) {
        assert(src && count);
        Ch *begin = src,
                *end = src + count - 1;
        while (end - begin > 0) {
            *begin = (*begin) ^ (*end);
            *end = (*begin) ^ (*end);
            *begin = (*begin) ^ (*end);
            ++begin, --end;
        }
        // return src;
    }
};

template<>
struct ICharTrait<char> {
    /**
     * Copy the first {count} of characters from {src} to {dest}
     * @param dest the place to be copied.
     * @param src the source
     * @param count how many characters to be copied.
     */
    static inline void Copy(char *dest, const char *src, SizeType count) {
        ::memcpy(reinterpret_cast<void *>(dest), reinterpret_cast<const void *>(src), count);
    }

    /**
     * Copy the first {count} of characters from {src} to {dest},
     * but allow {dest} and {src} to overlap
     * @param dest the place to be copied.
     * @param src the source
     * @param count how many characters to be copied.
     */
    static inline void Move(char *dest, const char *src, SizeType count) {
        ::memmove(reinterpret_cast<void *>(dest), reinterpret_cast<const void *>(src), count);
    }

    /**
     * Fill [val] character [count] times to [dest]
     * @param dest destination, the address to be filled.
     * @param val character to fill {dest}
     * @param count how many {val} to be filled.
     */
    static inline void Fill(char *dest, const char &val, SizeType count) {
        ::memset(reinterpret_cast<char *>(dest), val, count);
    }

    /**
     * @param src null-terminated string
     * @return the length if the given string
     */
    static inline SizeType Length(const char *src) {
        return ::strlen(src);
    }

    /**
     * Compare between {left} string and {right} string.
     * @param left the first null-terminated string
     * @param right the second null-terminated string
     * @return zero two strings are equal
     */
    static inline int Compare(const char *left, const char *right) {
        return ::strcmp(left, right);
    }

    /**
     * Compare the first {count} characters between {left} string and {right} string.
     * @param left the first null-terminated string
     * @param right the second null-terminated string
     * @param count the maximum count of compared characters.
     * @return zero if the first {count} of characters of two strings are equal
     */
    static inline int Compare(const char *left, const char *right, SizeType count) {
        return ::memcmp(reinterpret_cast<const void *>(left), reinterpret_cast<const void *>(right), count);
    }

    /**
     * Compare between {left} string and {right} string,
     * but ignore the case.
     * @param left the first null-terminated string
     * @param right the second null-terminated string
     * @return zero two strings are equal
     * TODO: find a more efficient way!!!
     */
    static inline int CompareNoCase(const char *left, const char *right) {
#ifdef ESCAPIST_OS_WINDOWS
        return ::_strcmpi(left, right);
#else
        return ::strcasecmp(left, right);
#endif
    }

    /**
     * Compare the first \p count characters between {left} string and {right} string,
     * but ignore the case.
     * @param left the first null-terminated string
     * @param right the second null-terminated string
     * @param count the maximum count of compared characters.
     * @return zero if the first {count} of characters of two strings are equal
     * TODO: find a more efficient way!!!
     */
    static inline int CompareNoCase(const char *left, const char *right, SizeType count) {
        assert(left && right);
        for (; *left && *right && tolower(*left) == tolower(*right) && count; ++left, ++right, --count);
        return *left - *right;
    }

    /**
     * Find the first occurrence of \p val in the null-terminated string \p src.
     * @param src the null-terminated string
     * @param val the character to be searched in \p src
     * @return pointer to found character if found; nullptr if no such character is found.
     */
    static inline const char *Find(const char *src, const char &val) {
        return ::strchr(src, val);
    }

    /**
     * Find the first occurrence of \p val in the initial \p count of the null-terminated string \p src.
     * @param src the null-terminated string
     * @param val the character to be searched in \p src
     * @param count maximum amount of characters to examine.
     * @return pointer to found character if found; nullptr if no such character is found.
     */
    static inline const char *Find(const char *src, const char &val, SizeType count) {
        assert(src && count);
        for (; *src && count; ++src, --count) {
            if (*src == val) {
                return src;
            }
        }
        return nullptr;
    }

    /**
     * Find the first occurrence of \p val in the null-terminated string \p src.
     * @param src the null-terminated string
     * @param sub the null-terminated string to be searched in \p src
     * @return pointer to the address of the first occurrence of \p sub if found; nullptr if no such character is found.
     */
    static inline const char *Find(const char *src, const char *sub) {
        return ::strstr(src, sub);
    }

    /**
     * Find the first occurrence of \p sub in the initial \p count of the null-terminated string \p src.
     * @param src the null-terminated string
     * @param sub the null-terminated string to be searched in \p src
     * @param count
     * @return pointer to the address of the first occurrence of \p sub if found; nullptr if no such character is found.
     */
    static inline const char *Find(const char *src, const char *sub, SizeType count) {
        assert(src && count);
        if (!sub) {
            return nullptr;
        }
        for (; *src && count; ++src, --count) {
            if (*src == *sub) { // the first char occurs, start to check
                const char *src_ref = src, // prevent {src} from changing to ensure every char is scanned.
                *sub_ref = sub; // also make a copy of {sub} to go back if it doesn't match.
                SizeType count_ref = count;
                for (; *src_ref && *sub_ref && *src_ref == *sub_ref && count_ref; ++src_ref, ++sub_ref, --count_ref);
                if (!*sub_ref) { // finished scanning, and all char in sub matched.
                    return src; // Thus, return it!
                } // Otherwise, src has terminated, or they do not match at all.
            }
        }
        return nullptr;
    }

    /**
     * Find the last occurrence of \p val in the null-terminated string \p src.
     * @param src the null-terminated string
     * @param val the character to be searched in \p src
     * @return pointer to found character if found; nullptr if no such character is found.
     */
    static inline const char *ReverseFind(const char *src, const char &val) {
        return ::strrchr(src, val);
    }

    /**
     * Find the last occurrence of \p val in the initial \p count of the null-terminated string \p src.
     * @param src the null-terminated string
     * @param val the character to be searched in \p src
     * @param count maximum amount of characters to examine.
     * @return pointer to found character if found; nullptr if no such character is found.
     */
    static inline const char *ReverseFind(const char *src, const char &val, SizeType count) {
        assert(src && count);
        const char *last = nullptr;
        for (; *src && count; ++src, --count) {
            if (*src == val) {
                last = src;
            }
        }
        return last;
    }

    /**
     * Find the first occurrence of \p val in the null-terminated string \p src.
     * @param src the null-terminated string
     * @param sub the null-terminated string to be searched in \p src
     * @return pointer to the address of the first occurrence of \p sub if found; nullptr if no such character is found.
     */
    static inline const char *ReverseFind(const char *src, const char *sub) {
        assert(src);
        if (!sub) {
            return nullptr;
        }
        const char *last = nullptr;
        for (; *src; ++src) {
            if (*src == *sub) { // the first char occurs, start to check
                const char *src_ref = src,
                        *sub_ref = sub;
                for (; *src_ref && *sub_ref && *src_ref == *sub_ref; ++src_ref, ++sub_ref);
                if (!*sub_ref) { // finished scanning, and all char in sub matched.
                    last = src;
                } // Otherwise, src has terminated, or they do not match at all.
            }
        }
        return last;
    }

    /**
     * Find the first occurrence of \p sub in the initial \p count of the null-terminated string \p src.
     * @param src the null-terminated string
     * @param sub the null-terminated string to be searched in \p src
     * @param count
     * @return pointer to the address of the first occurrence of \p sub if found; nullptr if no such character is found.
     */
    static inline const char *ReverseFind(const char *src, const char *sub, SizeType count) {
        assert(src && count);
        if (!sub) {
            return nullptr;
        }
        const char *last = nullptr;
        for (; *src && count; ++src, --count) {
            if (*src == *sub) { // the first char occurs, start to check
                const char *src_ref = src,
                        *sub_ref = sub;
                SizeType count_ref = count;
                for (; *src_ref && *sub_ref && *src_ref == *sub_ref && count_ref; ++src_ref, ++sub_ref, --count_ref);
                if (!*sub_ref) { // finished scanning, and all char in sub matched.
                    last = src;
                } // Otherwise, src has terminated, or they do not match at all.
            }
        }
        return last;
    }

    // TODO: Find & ReverseFind Ignore Case
    // TODO: NthFind & NthReverseFind & Ignore Case

    /**
     * Reverse the given null-terminated string \p src
     * @param src given null-terminated string
     */
    static inline void Reverse(char *src) {
        ICharTrait<char>::Reverse(src, ICharTrait<char>::Length(src));
    }

    /**
     * Reverse the first \p count of characters of the given null-terminated string \p src
     * @param src given null-terminated string
     */
    static inline void Reverse(char *src, SizeType count) {
        assert(src && count);
        char *begin = src,
                *end = src + count - 1;
        while (end - begin > 0) {
            *begin = (*begin) ^ (*end);
            *end = (*begin) ^ (*end);
            *begin = (*begin) ^ (*end);
            ++begin, --end;
        }
        // return src;
    }
};

template<>
struct ICharTrait<wchar_t> {
    /**
     * Copy the first {count} of characters from {src} to {dest}
     * @param dest the place to be copied.
     * @param src the source
     * @param count how many characters to be copied.
     */
    static inline void Copy(wchar_t *dest, const wchar_t *src, SizeType count) {
        ::memcpy(dest, src, count * sizeof(wchar_t));
    }

    /**
     * Copy the first {count} of characters from {src} to {dest},
     * but allow {dest} and {src} to overlap
     * @param dest the place to be copied.
     * @param src the source
     * @param count how many characters to be copied.
     */
    static inline void Move(wchar_t *dest, const wchar_t *src, SizeType count) {
        ::memmove(reinterpret_cast<void *>(dest), reinterpret_cast<const void *>(src), count * sizeof(wchar_t));
    }

    /**
     * Fill [val] character [count] times to [dest]
     * @param dest destination, the address to be filled.
     * @param val character to fill {dest}
     * @param count how many {val} to be filled.
     */
    static inline void Fill(wchar_t *dest, const wchar_t &val, SizeType count) {
        ::wmemset(dest, val, count);
    }

    /**
     * @param src null-terminated string
     * @return the length if the given string
     */
    static inline SizeType Length(const wchar_t *src) {
        return src ? ::wcslen(src) : 0;
    }

    /**
     * Compare between {left} string and {right} string.
     * @param left the first null-terminated string
     * @param right the second null-terminated string
     * @return zero two strings are equal
     */
    static inline int Compare(const wchar_t *left, const wchar_t *right) {
        return ::wcscmp(left, right);
    }

    /**
     * Compare the first {count} characters between {left} string and {right} string.
     * @param left the first null-terminated string
     * @param right the second null-terminated string
     * @param count the maximum count of compared characters.
     * @return zero if the first {count} of characters of two strings are equal
     */
    static inline int Compare(const wchar_t *left, const wchar_t *right, SizeType count) {
        return ::wmemcmp(left, right, count);
    }

    /**
     * Compare between {left} string and {right} string,
     * but ignore the case.
     * @param left the first null-terminated string
     * @param right the second null-terminated string
     * @return zero two strings are equal
     * TODO: find a more efficient way!!!
     */
    static inline int CompareNoCase(const wchar_t *left, const wchar_t *right) {
        assert(left && right);
        if (left == right) {
            return 0;
        }
        for (; *left && *right && tolower(*left) == tolower(*right); ++left, ++right);
        return *left - *right;
    }

    /**
     * Compare the first \p count characters between {left} string and {right} string,
     * but ignore the case.
     * @param left the first null-terminated string
     * @param right the second null-terminated string
     * @param count the maximum count of compared characters.
     * @return zero if the first {count} of characters of two strings are equal
     * TODO: find a more efficient way!!!
     */
    static inline int CompareNoCase(const wchar_t *left, const wchar_t *right, SizeType count) {
        assert(left && right);
        for (; *left && *right && tolower(*left) == tolower(*right) && count; ++left, ++right, --count);
        return *left - *right;
    }

    /**
     * Find the first occurrence of \p val in the null-terminated string \p src.
     * @param src the null-terminated string
     * @param val the character to be searched in \p src
     * @return pointer to found character if found; nullptr if no such character is found.
     */
    static inline const wchar_t *Find(const wchar_t *src, const wchar_t &val) {
        return ::wcschr(src, val);
    }

    /**
     * Find the first occurrence of \p val in the initial \p count of the null-terminated string \p src.
     * @param src the null-terminated string
     * @param val the character to be searched in \p src
     * @param count maximum amount of characters to examine.
     * @return pointer to found character if found; nullptr if no such character is found.
     */
    static inline const wchar_t *Find(const wchar_t *src, const wchar_t &val, SizeType count) {
        assert(src && count);
        for (; *src && count; ++src, --count) {
            if (*src == val) {
                return src;
            }
        }
        return nullptr;
    }

    /**
     * Find the first occurrence of \p val in the null-terminated string \p src.
     * @param src the null-terminated string
     * @param sub the null-terminated string to be searched in \p src
     * @return pointer to the address of the first occurrence of \p sub if found; nullptr if no such character is found.
     */
    static inline const wchar_t *Find(const wchar_t *src, const wchar_t *sub) {
        return ::wcsstr(src, sub);
    }

    /**
     * Find the first occurrence of \p sub in the initial \p count of the null-terminated string \p src.
     * @param src the null-terminated string
     * @param sub the null-terminated string to be searched in \p src
     * @param count
     * @return pointer to the address of the first occurrence of \p sub if found; nullptr if no such character is found.
     */
    static inline const wchar_t *Find(const wchar_t *src, const wchar_t *sub, SizeType count) {
        assert(src && count);
        if (!sub) {
            return nullptr;
        }
        for (; *src && count; ++src, --count) {
            if (*src == *sub) { // the first char occurs, start to check
                const wchar_t *src_ref = src, // prevent {src} from changing to ensure every char is scanned.
                *sub_ref = sub; // also make a copy of {sub} to go back if it doesn't match.
                SizeType count_ref = count;
                for (; *src_ref && *sub_ref && *src_ref == *sub_ref && count_ref; ++src_ref, ++sub_ref, --count_ref);
                if (!*sub_ref) { // finished scanning, and all char in sub matched.
                    return src; // Thus, return it!
                } // Otherwise, src has terminated, or they do not match at all.
            }
        }
        return nullptr;
    }

    /**
     * Find the last occurrence of \p val in the null-terminated string \p src.
     * @param src the null-terminated string
     * @param val the character to be searched in \p src
     * @return pointer to found character if found; nullptr if no such character is found.
     */
    static inline const wchar_t *ReverseFind(const wchar_t *src, const wchar_t &val) {
        return ::wcsrchr(src, val);
    }

    /**
     * Find the last occurrence of \p val in the initial \p count of the null-terminated string \p src.
     * @param src the null-terminated string
     * @param val the character to be searched in \p src
     * @param count maximum amount of characters to examine.
     * @return pointer to found character if found; nullptr if no such character is found.
     */
    static inline const wchar_t *ReverseFind(const wchar_t *src, const wchar_t &val, SizeType count) {
        assert(src && count);
        const wchar_t *last = nullptr;
        for (; *src && count; ++src, --count) {
            if (*src == val) {
                last = src;
            }
        }
        return last;
    }

    /**
     * Find the first occurrence of \p val in the null-terminated string \p src.
     * @param src the null-terminated string
     * @param sub the null-terminated string to be searched in \p src
     * @return pointer to the address of the first occurrence of \p sub if found; nullptr if no such character is found.
     */
    static inline const wchar_t *ReverseFind(const wchar_t *src, const wchar_t *sub) {
        assert(src);
        if (!sub) {
            return nullptr;
        }
        const wchar_t *last = nullptr;
        for (; *src; ++src) {
            if (*src == *sub) { // the first char occurs, start to check
                const wchar_t *src_ref = src,
                        *sub_ref = sub;
                for (; *src_ref && *sub_ref && *src_ref == *sub_ref; ++src_ref, ++sub_ref);
                if (!*sub_ref) { // finished scanning, and all char in sub matched.
                    last = src;
                } // Otherwise, src has terminated, or they do not match at all.
            }
        }
        return last;
    }

    /**
     * Find the first occurrence of \p sub in the initial \p count of the null-terminated string \p src.
     * @param src the null-terminated string
     * @param sub the null-terminated string to be searched in \p src
     * @param count
     * @return pointer to the address of the first occurrence of \p sub if found; nullptr if no such character is found.
     */
    static inline const wchar_t *ReverseFind(const wchar_t *src, const wchar_t *sub, SizeType count) {
        assert(src && count);
        if (!sub) {
            return nullptr;
        }
        const wchar_t *last = nullptr;
        for (; *src && count; ++src, --count) {
            if (*src == *sub) { // the first char occurs, start to check
                const wchar_t *src_ref = src,
                        *sub_ref = sub;
                SizeType count_ref = count;
                for (; *src_ref && *sub_ref && *src_ref == *sub_ref && count_ref; ++src_ref, ++sub_ref, --count_ref);
                if (!*sub_ref) { // finished scanning, and all char in sub matched.
                    last = src;
                } // Otherwise, src has terminated, or they do not match at all.
            }
        }
        return last;
    }

    // TODO: Find & ReverseFind Ignore Case
    // TODO: NthFind & NthReverseFind & Ignore Case

    /**
 * Reverse the given null-terminated string \p src
 * @param src given null-terminated string
 */
    static inline void Reverse(wchar_t *src) {
        ICharTrait<wchar_t>::Reverse(src, ICharTrait<wchar_t>::Length(src));
    }

    /**
     * Reverse the first \p count of characters of the given null-terminated string \p src
     * @param src given null-terminated string
     */
    static inline void Reverse(wchar_t *src, SizeType count) {
        assert(src && count);
        wchar_t *begin = src,
                *end = src + count - 1;
        while (end - begin > 0) {
            *begin = (*begin) ^ (*end);
            *end = (*begin) ^ (*end);
            *begin = (*begin) ^ (*end);
            ++begin, --end;
        }
        // return src;
    }
};

template<typename Ch>
class BasicString {
public:
    /**
     * Creates an empty instance
     */
    BasicString() : mode_(Mode::Null), data_(nullptr), first_(nullptr), last_(nullptr), end_(nullptr) {}

    /**
     * Creates an instance with \p count occurrence pf \p ch.
     * Remains \p front_offset before the first \p ch and \p back_offset after the last \p ch.
     * @param count the amount of \p ch added into the instance
     * @param ch character
     * @param front_offset the amount of space remained before the first \p ch.
     * @param back_offset the amount of space remained after the last \p ch.
     */
    BasicString(SizeType count, const Ch &ch, SizeType front_offset = 0, SizeType back_offset = 0) {
        if (count) {
            if (Ch *pos = BasicString<Ch>::SimpleAllocate(front_offset + count + back_offset, nullptr) + front_offset) {
                ICharTrait<Ch>::Fill(pos, ch, count);
            }
        } else {
            new(this)BasicString<Ch>();
        }
    }

    /**
     * Creates an instance with \p str without remaining spaces.
     * @param str string added to the instance
     */
    BasicString(const Ch *str) : BasicString(str, ICharTrait<Ch>::Length(str), 0, 0) {}

    /**
     * Creates an instance with the first \p len characters of c-style null-terminated string starting at \p str.
     * Remains \p front_offset before the first \p ch and \p back_offset after the last \p ch.
     * @param str the address of the character sequence
     * @param len the length of \p str; the amount of characters to be added
     * @param front_offset the amount of space remained before the \p str.
     * @param back_offset the amount of space remained after the \p str.
     */
    BasicString(const Ch *str, SizeType len, SizeType front_offset = 0, SizeType back_offset = 0) {
        if (str && len) {
            if (Ch *pos = BasicString<Ch>::SimpleAllocate(front_offset + len + back_offset, nullptr) + front_offset) {
                ICharTrait<Ch>::Copy(pos, str, len);
            }
        } else {
            new(this)BasicString<Ch>();
        }
    }

    /**
     * COPY CONSTRUCTOR
     * Creates an instance with another instance.
     * If \p other is large enough, it will trigger sharing process.
     * @param other another instance
     */
    BasicString(const BasicString<Ch> &other) : mode_(other.mode_) {
        if (&other == this) {
            return; // don't do anything if the input is the current instance.
        }

        /*
         * Copy all these things directly, because there are three cases:
         *  - 1. mode == Null: the input instance is also null, then its member should be all 0.
         *  - 2. mode == Small: the small_ is valid, and we just need to directly copy all these things.
         *  - 3. mode == Allocate: the current instance will share with input instance (CODE BELOW).
         */
        ::memcpy(this, &other, sizeof(BasicString<Ch>));
        if (mode_ == Mode::Allocate) {
            if (data_) { // prevent from violation.
                if (*data_) { // the reference count has existed, then just need to add it.
                    (**data_).IncrementRef();
                } else { // otherwise, create a new one and initialize it to 2.
                    *data_ = new RefCount(2);
                }
            } else {
                new(this)BasicString<Ch>();
            }
        }
    }

    /**
     *
     * @param other
     * @param offset
     * @param count
     * @param front_offset
     * @param back_offset
     */
    BasicString(const BasicString<Ch> &other, SizeType offset, SizeType count,
                SizeType front_offset = 0, SizeType back_offset = 0) {
        assert(count < other.last_ - other.first_ - offset);
        new(this)BasicString<Ch>(other.first_ + offset, count, front_offset, back_offset);
    }

    /**
     * DESTRUCTOR;
     * Releases all unnecessary data if they are not sharing with other instances.
     */
    ~BasicString() {
        if (mode_ == Mode::Allocate) {
            if (data_) {
                if (*data_ && (**data_).Value() > 1) {
                    (**data_).DecrementRef();
                    return;
                } else {
                    delete *data_;
                }
                ::free((void *) data_);
            }
        }
    }

    /**
     * @return the length of the string, in terms of characters
     */
    SizeType Length() const noexcept {
        if (mode_ == Mode::Null) {
            return 0;
        } else if (mode_ == Mode::Small) {
            return SmallLength();
        } else {
            return first_ ? last_ - first_ : 0;
        }
    }

    /**
     * @return \b true if the length is zero.
     */
    bool IsEmpty() const noexcept {
        if (mode_ == Mode::Null) {
            return true;
        } else if (mode_ == Mode::Small) {
            return small_[kSmallLen] == kSmallLen;
        } else {
            return last_ == first_;
        }
    }

    /**
     * @return the maximum number of characters the instance can store without enlarging.
     */
    SizeType Capacity() const noexcept {
        if (mode_ == Mode::Null) {
            return 0;
        } else if (mode_ == Mode::Small) {
            return kSmallCap;
        } else {
            return first_ ? end_ - first_ : 0;
        }
    }

    /**
     * Ensures the instance can store \p capacity amount of characters.
     * This is usually called before the instance will be write externally.
     * @param capacity the amount of characters required.
     * @return the current instance
     */
    BasicString<Ch> &EnsureCapacity(const SizeType &capacity) {
        if (mode_ == Mode::Null) { // if the current mode is null,
            if (capacity) { // and the capacity is nonzero, then allocate memory for intended capacity.
                SimpleAllocate(SmallLength(), capacity, nullptr);
            }
        } else if (mode_ == Mode::Small) { // if the current mode is small,
            if (capacity >= kSmallCap) { // and the intended capacity is larger than stack can store,
                SizeType len(SmallLength()); // change the mode to Allocate with intended capacity.
                Ch old[kSmallCap];
                ICharTrait<Ch>::Copy(old, small_, len);
                if (Ch *pos = SimpleAllocate(len, capacity, nullptr)) {
                    ICharTrait<Ch>::Copy(pos, old, len);
                }
            }
        } else {
            if (data_) {
                SizeType old_len = last_ - first_;
                if (capacity > end_ - first_) { // if the capacity is smaller than intended capacity,
                    if (*data_ && (**data_).Value() > 1) { // if the instance is sharing,
                        (**data_).DecrementRef(); // then detach and allocate (temporarily) independent data.
                        Ch *old = first_;
                        if (Ch *pos = SimpleAllocate(old_len, capacity, nullptr)) {
                            ICharTrait<Ch>::Copy(pos, old, old_len);
                        }
                    } else { // otherwise, enlarge it by simply call realloc
                        RefCount **old_data = data_;
                        SizeType len = last_ - first_;
                        data_ = static_cast<RefCount **>(::realloc(data_, TotCap(capacity)));
                        if (old_data != data_) { // if the data moves, then reassign other member variables.
                            first_ = (Ch *) (data_ + 1);
                            last_ = first_ + len;
                        }
                        end_ = first_ + capacity; // this must be done even we do not move the memory.
                    }
                }
            }
        }
        return *this;
    }

    Ch &At(SizeType index) {
        if (mode_ == Mode::Small) {
            assert(index < kSmallLen);
            return small_[index];
        } else if (mode_ == Mode::Allocate) {
            assert(data_);
            SizeType len = last_ - first_;
            assert(index < len);
            if ((*data_) && (**data_).Value() > 1) {
                (**data_).DecrementRef();
                Ch *old = first_;
                if (Ch *pos = SimpleAllocate(len, nullptr)) {
                    ICharTrait<Ch>::Copy(pos, old, len);
                }
            }
            return first_ + index;
        }
        assert(false);
    }

    const Ch &ConstAt(SizeType index) {
        if (mode_ == small_) {
            assert(index < SmallLength());
            return small_[index];
        } else if (mode_ == Mode::Allocate) {
            assert(data_ && index < last_ - first_);
            return first_ + index;
        }
        assert(false);
    }

    /**
     * @return the address of contiguous memory of the string, mutable
     */
    Ch *Data() {
        if (mode_ == Mode::Null) {
            return nullptr;
        } else if (mode_ == Mode::Small) {
            return small_;
        } else if (data_) {
            if (*data_ && (**data_).Value() > 1) {
                (**data_).DecrementRef();
                SizeType len = last_ - first_;
                Ch *old = first_, *pos = SimpleAllocate(len, nullptr);
                if (pos) {
                    ICharTrait<Ch>::Copy(pos, first_, len);
                }
            }
            return first_;
        }
        return nullptr;
    }

    /**
     * @return the address of contiguous memory of the string, not mutable
     */
    const Ch *ConstData() const noexcept {
        if (mode_ == Mode::Null) {
            return nullptr;
        } else if (mode_ == Mode::Small) {
            return small_;
        } else if (data_) {
            return first_;
        }
        return nullptr;
    }

    int CompareTo(const Ch *other) const noexcept {
        return ICharTrait<Ch>::Compare(ConstData(), other);
    }

    int CompareTo(const BasicString<Ch> &other) const noexcept {
        return ICharTrait<Ch>::Compare(ConstData(), other.ConstData());
    }

    int CompareToNoCase(const Ch *other) const noexcept {
        return ICharTrait<Ch>::CompareNoCase(ConstData(), other);
    }

    int CompareToNoCase(const BasicString<Ch> &other) const noexcept {
        return ICharTrait<Ch>::CompareNoCase(ConstData(), other.ConstData());
    }

    SizeType IndexOf(const Ch &ch) {
        const Ch *str(ConstData());
        if (const Ch *pos = ICharTrait<Ch>::Find(str, ch)) {
            return pos - str;
        }
        return -1;
    }

    SizeType IndexOf(const Ch &ch, SizeType occurrence) {
        const Ch *str(ConstData()), *curr(ICharTrait<Ch>::Find(str, ch)), *prev(nullptr);
        if (str) {
            for (; occurrence && curr; --occurrence) {
                prev = curr;
                curr = ICharTrait<Ch>::Find(prev + 1, ch);
            }
        }
        return occurrence ? -1 : prev - str;
    }

    SizeType IndexOf(const Ch &ch, SizeType offset, SizeType occurrence) {
        SizeType len(Length());
        const Ch *str(ConstData());
        if (!str || offset >= len) {
            return -1;
        }
        str += offset;
        const Ch *curr(ICharTrait<Ch>::Find(str, ch)), *prev(nullptr);
        for (; occurrence && curr; --occurrence) {
            prev = curr;
            curr = ICharTrait<Ch>::Find(prev + 1, ch);
        }
        return occurrence ? -1 : prev - str + offset;
    }

    SizeType IndexOf(const Ch *sub) {
        const Ch *str(ConstData());
        if (const Ch *pos = ICharTrait<Ch>::Find(str, sub)) {
            return pos - str;
        }
        return -1;
    }

    SizeType IndexOf(const Ch *sub, SizeType occurrence) {
        SizeType sub_len(ICharTrait<Ch>::Length(sub));
        const Ch *str(ConstData()), *curr(ICharTrait<Ch>::Find(str, sub)), *prev(nullptr);
        if (str && sub_len && sub) {
            for (; occurrence && curr; --occurrence) {
                prev = curr;
                curr = ICharTrait<Ch>::Find(prev + sub_len, sub);
            }
        }
        return occurrence ? -1 : prev - str;
    }

    SizeType IndexOf(const Ch *sub, SizeType offset, SizeType occurrence) {
        SizeType len(Length()), sub_len(ICharTrait<Ch>::Length(sub));
        const Ch *str(ConstData());
        if (!str || !sub_len || offset >= len - sub_len) {
            return -1;
        }
        str += offset;
        const Ch *curr(ICharTrait<Ch>::Find(str, sub)), *prev(nullptr);
        for (; occurrence && curr; --occurrence) {
            prev = curr;
            curr = ICharTrait<Ch>::Find(prev + sub_len, str);
        }
        return occurrence ? -1 : prev - str + offset;
    }

    SizeType IndexOf(const BasicString<Ch> &other) {
        return IndexOf(other.ConstData());
    }

    SizeType IndexOf(const BasicString<Ch> &other, SizeType occurrence) {
        return IndexOf(other.ConstData(), occurrence);
    }

    SizeType IndexOf(const BasicString<Ch> &other, SizeType offset, SizeType occurrence) {
        return IndexOf(other.ConstData(), offset, occurrence);
    }

    SizeType LastIndexOf(const Ch &ch) {
        const Ch *str(ConstData());
        if (const Ch *pos = ICharTrait<Ch>::ReverseFind(str, ch)) {
            return pos - str;
        }
        return -1;
    }

    SizeType LastIndexOf(const Ch *&sub) {
        const Ch *str(ConstData());
        if (const Ch *pos = ICharTrait<Ch>::ReverseFind(str, sub)) {
            return pos - str;
        }
        return -1;
    }

    SizeType LastIndexOf(const BasicString<Ch> &other) {
        return LastIndexOf(other.ConstData());
    }

    BasicString<Ch> &Assign(const Ch *str) {
        return Assign(str, ICharTrait<Ch>::Length(str), 0, 0);
    }

    BasicString<Ch> &Assign(const Ch *str, SizeType len,
                            SizeType front_offset = 0, SizeType back_offset = 0) {
        if (str && len) {
            if (Ch *pos = AssignImpl(front_offset + len + back_offset) + front_offset) {
                ICharTrait<Ch>::Copy(pos, str, len);
            }
        }
        return *this;
    }

    BasicString<Ch> &Assign(const BasicString<Ch> &other) {
        if (mode_ == Mode::Allocate) {
            if (*data_ && (**data_).Value() > 1) {
                (**data_).DecrementRef();
            } else {
                delete *data_;
                ::free(data_);
            }
        }
        new(this)BasicString<Ch>(other);
        return *this;
    }

    BasicString<Ch> &Assign(const BasicString<Ch> &other, SizeType offset, SizeType len,
                            SizeType front_offset = 0, SizeType back_offset = 0) {
        SizeType total(other.Length());
        if (offset + len > total) {
            len = total - offset;
        }
        if (other.mode_ != Mode::Null && total) {
            if (Ch *pos = AssignImpl(front_offset + len + back_offset) + front_offset) {
                ICharTrait<Ch>::Copy(pos, other.ConstData() + offset, len);
            }
        }
        return *this;
    }

    /**
     * Extends the string by putting additional \p count consecutive copies of character \p ch at the end of the instance.
     * Remains \p front_offset before the first \p ch and \p back_offset after the last \p ch.
     * @param ch additional character
     * @param count number of copies
     * @param front_offset the amount of space remained before the \p str.
     * @param back_offset the amount of space remained after the \p str.
     * @return the current instance
     */
    BasicString<Ch> &Append(const Ch &ch, SizeType count = 1, SizeType front_offset = 0, SizeType back_offset = 0) {
        if (ch && count) {
            if (Ch *pos = GrowthAppend(front_offset + count + back_offset) + front_offset) {
                ICharTrait<Ch>::Fill(pos, ch, count);
            }
        }
        return *this;
    }

    /**
     * Extends the string by putting the c-style null-terminated string \p str at the end of the instance.
     * Remains \p front_offset before the first character and \p back_offset after the last character.
     * @param str the additional c-style null-terminated string
     * @param front_offset the amount of space remained before the \p str.
     * @param back_offset the amount of space remained after the \p str.
     * @return the current instance
     */
    BasicString<Ch> &Append(const Ch *str, SizeType front_offset = 0, SizeType back_offset = 0) {
        return BasicString<Ch>::Append(str, ICharTrait<Ch>::Length(str), front_offset, back_offset);
    }

    /**
     * Extends the string by putting first \p len characters of \p str at the end of the instance.
     * Remains \p front_offset before the first character and \p back_offset after the last character.
     * @param str the additional array of characters
     * @param len the amount of characters to be added
     * @param front_offset the amount of space remained before the \p str.
     * @param back_offset the amount of space remained after the \p str.
     * @return the current instance
     */
    BasicString<Ch> &Append(const Ch *str, SizeType len, SizeType front_offset = 0, SizeType back_offset = 0) {
        if (str && len) {
            if (Ch *pos = GrowthAppend(front_offset + len + back_offset) + front_offset) {
                ICharTrait<Ch>::Copy(pos, str, len);
            }
        }
        return *this;
    }

    BasicString<Ch> &Append(const BasicString<Ch> &other, SizeType front_offset = 0, SizeType back_offset = 0) {
        if (data_ || front_offset || back_offset) {
            if (other.mode_ == Mode::Small) {
                return Append(other.small_, other.SmallLength(), front_offset, back_offset);
            } else if (other.mode_ == Mode::Allocate) {
                return Append(other.first_, other.last_ - first_, front_offset, back_offset);
            }
        } else {
            new(this)BasicString<Ch>(other);
        }
        return *this;
    }

    BasicString<Ch> &Append(const BasicString<Ch> &other, SizeType offset, SizeType len,
                            SizeType front_offset = 0, SizeType back_offset = 0) {
        if (other.mode_ == Mode::Small) {
            return Append(other.small_ + offset, len, front_offset, back_offset);
        } else if (other.mode_ == Mode::Allocate) {
            return Append(other.first_ + offset, len, front_offset, back_offset);
        }
        return *this;
    }

    /**
     * Extends the string by putting additional \p count consecutive copies of character \p ch at the front of the instance.
     * Remains \p front_offset before the first \p ch and \p back_offset after the last \p ch.
     * @param ch additional character
     * @param count number of copies
     * @param front_offset the amount of space remained before the \p str.
     * @param back_offset the amount of space remained after the \p str.
     * @return the current instance
     */
    BasicString<Ch> &Prepend(const Ch &ch, SizeType count = 1, SizeType front_offset = 0, SizeType back_offset = 0) {
        if (ch && count) {
            if (Ch *pos = GrowthPrepend(front_offset + count + back_offset) + front_offset) {
                ICharTrait<Ch>::Fill(pos, ch, count);
            }
        }
        return *this;
    }

    /**
     * Extends the string by putting the c-style null-terminated string \p str at the front of the instance.
     * Remains \p front_offset before the first character and \p back_offset after the last character.
     * @param str the additional c-style null-terminated string
     * @param front_offset the amount of space remained before the \p str.
     * @param back_offset the amount of space remained after the \p str.
     * @return the current instance
     */
    BasicString<Ch> &Prepend(const Ch *str, SizeType front_offset = 0, SizeType back_offset = 0) {
        return BasicString<Ch>::Prepend(str, ICharTrait<Ch>::Length(str), front_offset, back_offset);
    }

    /**
     * Extends the string by putting first \p len characters of \p str at the front of the instance.
     * Remains \p front_offset before the first character and \p back_offset after the last character.
     * @param str the additional array of characters
     * @param len the amount of characters to be added
     * @param front_offset the amount of space remained before the \p str.
     * @param back_offset the amount of space remained after the \p str.
     * @return the current instance
     */
    BasicString<Ch> &Prepend(const Ch *str, SizeType len, SizeType front_offset = 0, SizeType back_offset = 0) {
        if (str && len) {
            if (Ch *pos = GrowthPrepend(front_offset + len + back_offset) + front_offset) {
                ICharTrait<Ch>::Copy(pos, str, len);
            }
        }
        return *this;
    }

    BasicString<Ch> &Prepend(const BasicString<Ch> &other, SizeType front_offset = 0, SizeType back_offset = 0) {
        if (data_ || front_offset || back_offset) {
            if (other.mode_ == Mode::Small) {
                return Prepend(other.small_, other.SmallLength(), front_offset, back_offset);
            } else if (other.mode_ == Mode::Allocate) {
                return Prepend(other.first_, other.last_ - first_, front_offset, back_offset);
            }
        } else {
            new(this)BasicString<Ch>(other);
        }
        return *this;
    }

    BasicString<Ch> &Prepend(const BasicString<Ch> &other, SizeType offset, SizeType len,
                             SizeType front_offset = 0, SizeType back_offset = 0) {
        if (other.mode_ == Mode::Small) {
            return Prepend(other.small_ + offset, len, front_offset, back_offset);
        } else if (other.mode_ == Mode::Allocate) {
            return Prepend(other.first_ + offset, len, front_offset, back_offset);
        }
        return *this;
    }

    /**
     * Extends the string by putting additional \p count consecutive copies of character \p ch
     * at the \p index position of the instance.
     * Remains \p front_offset before the first \p ch and \p back_offset after the last \p ch.
     * @param ch additional character
     * @param count number of copies
     * @param front_offset the amount of space remained before the \p str.
     * @param back_offset the amount of space remained after the \p str.
     * @return the current instance
     */
    BasicString<Ch> &Insert(SizeType index, const Ch &ch, SizeType count = 1,
                            SizeType front_offset = 0, SizeType back_offset = 0) {
        if (ch && count) {
            if (Ch *pos = GrowthInsert(index, front_offset + count + back_offset) + front_offset) {
                ICharTrait<Ch>::Fill(pos, ch, count);
            }
        }
        return *this;
    }

    /**
     * Extends the string by putting the c-style null-terminated string \p str
     * at the \p index position of the instance.
     * Remains \p front_offset before the first character and \p back_offset after the last character.
     * @param str the additional c-style null-terminated string
     * @param front_offset the amount of space remained before the \p str.
     * @param back_offset the amount of space remained after the \p str.
     * @return the current instance
     */
    BasicString<Ch> &Insert(SizeType index, const Ch *str, SizeType front_offset = 0, SizeType back_offset = 0) {
        return BasicString<Ch>::Insert(index, str, ICharTrait<Ch>::Length(str), front_offset, back_offset);
    }

    /**
     * Extends the string by putting first \p len characters of \p str
     * at the \p index position of the instance.
     * Remains \p front_offset before the first character and \p back_offset after the last character.
     * @param str the additional array of characters
     * @param len the amount of characters to be added
     * @param front_offset the amount of space remained before the \p str.
     * @param back_offset the amount of space remained after the \p str.
     * @return the current instance
     */
    BasicString<Ch> &Insert(SizeType index, const Ch *str, SizeType len,
                            SizeType front_offset = 0, SizeType back_offset = 0) {
        if (str && len) {
            if (Ch *pos = GrowthInsert(index, front_offset + len + back_offset) + front_offset) {
                ICharTrait<Ch>::Copy(pos, str, len);
            }
        }
        return *this;
    }

    BasicString<Ch> &Insert(SizeType index, const BasicString<Ch> &other,
                            SizeType front_offset = 0, SizeType back_offset = 0) {
        if (data_ || front_offset || back_offset) {
            if (other.mode_ == Mode::Small) {
                return Insert(index, other.small_, other.SmallLength(), front_offset, back_offset);
            } else if (other.mode_ == Mode::Allocate) {
                return Insert(index, other.first_, other.last_ - first_, front_offset, back_offset);
            }
        } else {
            new(this)BasicString<Ch>(other);
        }
        return *this;
    }

    BasicString<Ch> &Insert(SizeType index, const BasicString<Ch> &other, SizeType offset, SizeType len,
                            SizeType front_offset = 0, SizeType back_offset = 0) {
        if (other.mode_ == Mode::Small) {
            return Insert(index, other.small_ + offset, len, front_offset, back_offset);
        } else if (other.mode_ == Mode::Allocate) {
            return Insert(index, other.first_ + offset, len, front_offset, back_offset);
        }
        return *this;
    }

    BasicString<Ch> &Remove(SizeType index, SizeType count) {
        if (mode_ == Mode::Small) {
            SizeType old_len(SmallLength()), new_len(old_len - count);
            ICharTrait<Ch>::Move(small_ + index + count, small_ + index, old_len - index - count);
            SetSmallLength(new_len, true);
        } else if (mode_ == Mode::Allocate) {
            if (data_) {
                if (*data_ && (**data_).Value() > 1) {
                    (**data_).DecrementRef();
                    SizeType old_len(last_ - first_), new_len(old_len - count);
                    Ch *old_str = first_, *new_str(SimpleAllocate(new_len, nullptr));
                    if (new_str) {
                        ICharTrait<Ch>::Copy(new_str, old_str, index);
                        ICharTrait<Ch>::Copy(new_str + index, old_str + index + count, old_len - index - count);
                    }
                } else {
                    ICharTrait<Ch>::Move(first_ + index, first_ + index + count, last_ - first_ - index - count);
                    last_ -= count;
                }
            }
        }
        return *this;
    }

private:
    enum class Mode {
        // The instance is empty.
        Null,

        // The string stores the char sequence in the stack.
        Small,

        // The string stores the char sequence in the heap.
        Allocate
    };

    using RefCount = Internal::ReferenceCount;

    struct GeneralBuffer {
        RefCount **ref_;
        Ch *first_;
        Ch *last_;
        Ch *end_;
    };

    Mode mode_;
    union {
        unsigned char bytes_[sizeof(GeneralBuffer)];
        Ch small_[sizeof(GeneralBuffer) / sizeof(Ch)];
        struct {
            RefCount **data_;
            Ch *first_;
            Ch *last_;
            Ch *end_;
        };
    };

    static constexpr SizeType kSmallCap = sizeof(GeneralBuffer) / sizeof(Ch);
    static constexpr SizeType kSmallLen = kSmallCap - 1;
    static constexpr SizeType kMinCap = (sizeof(Ch *) * 8) / sizeof(Ch);

    static constexpr SizeType Cap(SizeType len) {
        if (len) {
            if (len <= kMinCap) {
                return kMinCap;
            } else {
                return len * 1.5;
            }
        }
        return 0;
    }

    static constexpr SizeType TotCap(SizeType capacity) {
        return sizeof(RefCount *) + capacity * sizeof(Ch);
    }

    SizeType SmallLength() const noexcept {
        return kSmallLen - SizeType(small_[kSmallLen]);
    }

    void SetSmallLength(const SizeType &len, bool putZero) {
        small_[kSmallLen] = Ch(kSmallLen - len);
        if (len != kSmallLen && putZero) {
            small_[len] = Ch(0);
        }
    }

    Ch *SimpleAllocate(const SizeType &len, RefCount *const &rc) {
        if (len > kSmallLen) {
            mode_ = Mode::Allocate;
            SizeType cap(Cap(len));
            data_ = static_cast<RefCount **>(::malloc(TotCap(cap)));
            assert(data_);
            *data_ = rc;
            first_ = (Ch *) (data_ + 1);
            last_ = first_ + len;
            end_ = first_ + cap;
            *last_ = Ch(0);
            return first_;
        } else {
            mode_ = Mode::Small;
            SetSmallLength(len, true);
            return small_;
        }
    }

    Ch *SimpleAllocate(const SizeType &len, const SizeType &cap, RefCount *const &rc) {
        if (cap > kSmallCap) {
            mode_ = Mode::Allocate;
            data_ = static_cast<RefCount **>(::malloc(TotCap(cap)));
            assert(data_);
            *data_ = rc;
            first_ = (Ch *) (data_ + 1);
            last_ = first_ + len;
            end_ = first_ + cap;
            *last_ = Ch(0);
            return first_;
        } else {
            mode_ = Mode::Small;
            SetSmallLength(len, true);
            return small_;
        }
    }

    Ch *GrowthAppend(const SizeType &count) {
        if (mode_ == Mode::Null) {
            return SimpleAllocate(count, nullptr);
        } else if (mode_ == Mode::Small) {
            SizeType old_len(SmallLength()), new_len(old_len + count);
            if (new_len > kSmallLen) {
                Ch old[kSmallCap];
                ICharTrait<Ch>::Copy(old, small_, old_len);
                Ch *pos = SimpleAllocate(new_len, nullptr);
                if (pos) {
                    ICharTrait<Ch>::Copy(pos, old, old_len);
                }
                return pos + old_len;
            } else {
                SetSmallLength(new_len, true);
                return small_ + old_len;
            }
        } else {
            if (data_) {
                SizeType old_len(last_ - first_), new_len(old_len + count);
                if (*data_ && (**data_).Value() > 1) {
                    (**data_).DecrementRef();
                    Ch *old = first_, *pos = SimpleAllocate(new_len, nullptr);
                    if (pos) {
                        ICharTrait<Ch>::Copy(pos, old, old_len);
                    }
                    return pos + old_len;
                } else {
                    if (new_len >= end_ - first_) {
                        SizeType new_cap(Cap(new_len));
                        RefCount **old_data = data_;
                        RefCount *old_ref = *data_;
                        data_ = static_cast<RefCount **>(::realloc(data_, TotCap(new_cap)));
                        if (data_ != old_data) {
                            first_ = (Ch *) (data_ + 1);
                            *data_ = old_ref;
                        }
                        last_ = first_ + new_len;
                        end_ = first_ + new_cap;
                    } else {
                        last_ += count;
                    }
                    return first_ + old_len;
                }
            } else {
                return SimpleAllocate(count, nullptr);
            }
        }
    }

    Ch *GrowthPrepend(const SizeType &count) {
        if (mode_ == Mode::Null) {
            return SimpleAllocate(count, nullptr);
        } else if (mode_ == Mode::Small) {
            SizeType old_len(SmallLength()), new_len(old_len + count);
            if (new_len > kSmallLen) {
                Ch old[kSmallCap];
                ICharTrait<Ch>::Copy(old, small_, old_len);
                Ch *pos = SimpleAllocate(new_len, nullptr);
                if (pos) {
                    ICharTrait<Ch>::Copy(pos + count, old, old_len);
                }
                return pos;
            } else {
                SetSmallLength(new_len, true);
                ICharTrait<Ch>::Move(small_ + count, small_, old_len);
                return small_;
            }
        } else {
            SizeType old_len(last_ - first_), new_len(old_len + count);
            if (*data_ && (**data_).Value() > 1) {
                (**data_).DecrementRef();
                Ch *old_str(first_), *new_str(SimpleAllocate(new_len, nullptr));
                if (new_str) {
                    ICharTrait<Ch>::Copy(new_str + count, old_str, old_len);
                }
            } else {
                SizeType old_cap(end_ - first_);
                if (new_len < old_cap) {
                    last_ += count;
                    ICharTrait<Ch>::Move(first_ + count, first_, old_len);
                } else {
                    SizeType new_cap(Cap(new_len));
                    if (new_cap - old_cap > old_cap * 2) {
                        RefCount **old_data = data_;
                        RefCount *old_ref = *data_;
                        Ch *old_str = first_;
                        Ch *new_str(SimpleAllocate(new_len, old_ref));
                        ICharTrait<Ch>::Copy(new_str + count, old_str, old_len);
                        ::free((void *) data_);
                    } else {
                        RefCount **old_data = data_;
                        RefCount *old_ref = *data_;
                        data_ = static_cast<RefCount **>(::realloc(data_, TotCap(new_cap)));
                        if (data_ != old_data) {
                            first_ = (Ch *) (data_ + 1);
                        }
                        last_ = first_ + new_len;
                        end_ = first_ + new_cap;
                        ICharTrait<Ch>::Move(first_ + count, first_, old_len);
                    }
                }
            }
            return first_;
        }
    }

    Ch *GrowthInsert(const SizeType &index, const SizeType &count) {
        if (mode_ == Mode::Small) {
            SizeType old_len(SmallLength()), new_len(old_len + count);
            if (new_len > kSmallLen) {
                Ch old[kSmallCap];
                ICharTrait<Ch>::Copy(old, small_, old_len);
                Ch *pos = SimpleAllocate(new_len, nullptr);
                if (pos) {
                    ICharTrait<Ch>::Copy(pos, old, index);
                    ICharTrait<Ch>::Copy(pos + index + count, old + index, old_len - index);
                }
                return pos + index;
            } else {
                SetSmallLength(new_len, true);
                ICharTrait<Ch>::Move(small_ + index + count, small_ + index, old_len - index);
                return small_ + index;
            }
        } else {
            SizeType old_len(last_ - first_), new_len(old_len + count);
            if (*data_ && (**data_).Value() > 1) {
                (**data_).DecrementRef();
                Ch *old_str(first_), *new_str(SimpleAllocate(new_len, nullptr));
                if (new_str) {
                    ICharTrait<Ch>::Copy(new_str, old_str, index);
                    ICharTrait<Ch>::Copy(new_str + index + count, old_str + index, old_len - index);
                }
            } else {
                SizeType old_cap(end_ - first_);
                if (new_len < old_cap) {
                    last_ += count;
                    ICharTrait<Ch>::Move(first_ + index + count, first_ + index, old_len - index);
                } else {
                    SizeType new_cap(Cap(new_len));
                    if (new_cap - old_cap > old_cap * 2) {
                        RefCount **old_data = data_;
                        RefCount *old_ref = *data_;
                        Ch *old_str = first_;
                        Ch *new_str(SimpleAllocate(new_len, old_ref));
                        ICharTrait<Ch>::Copy(new_str, old_str, index);
                        ICharTrait<Ch>::Copy(new_str + index + count, old_str + index, old_len - index);
                        ::free((void *) data_);
                    } else {
                        RefCount **old_data = data_;
                        RefCount *old_ref = *data_;
                        data_ = static_cast<RefCount **>(::realloc(data_, TotCap(new_cap)));
                        if (data_ != old_data) {
                            first_ = (Ch *) (data_ + 1);
                        }
                        last_ = first_ + new_len;
                        end_ = first_ + new_cap;
                        ICharTrait<Ch>::Move(first_ + index + count, first_ + index, old_len - index);
                    }
                }
            }
            return first_ + index;
        }
    }

    Ch *AssignImpl(SizeType new_len) {
        if (mode_ == Mode::Null) {
            return SimpleAllocate(new_len, nullptr);
        } else if (mode_ == Mode::Small) {
            SizeType old_len(SmallLength());
            if (new_len < kSmallLen) {
                SetSmallLength(new_len, true);
                return small_;
            } else {
                return SimpleAllocate(new_len, nullptr);
            }
        } else {
            if (*data_ && (**data_).Value() > 1) {
                (**data_).DecrementRef();
                return SimpleAllocate(new_len, nullptr);
            } else {
                SizeType old_cap(end_ - first_);
                if (new_len < old_cap) {
                    last_ = first_ + new_len;
                } else {
                    SizeType new_cap(Cap(new_len));
                    RefCount **old_data = data_;
                    RefCount *old_ref = *data_;
                    data_ = static_cast<RefCount **>(::realloc(data_, TotCap(new_cap)));
                    if (data_ != old_data) {
                        first_ = (Ch *) (data_ + 1);
                        *data_ = old_ref;
                    }
                    last_ = first_ + new_len;
                    end_ = first_ + new_cap;
                }
                *last_ = Ch(0);
                return first_;
            }
        }
    }
};

using String = BasicString<Char>;

#endif //ESCAPIST_STRING_H
