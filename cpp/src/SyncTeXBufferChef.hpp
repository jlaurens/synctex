/**
 * SyncTeXBufferChef.hpp
 * 
 * This file is part of the SyncTeX library.
 * It defines the class for representing nodes in a SyncTeX tree.
 * 
 * Original copyright (c) 2026 jerome DOT laurens AT ube DOT fr
 */

// #pragma once

/**
 * @brief Nodes to represent SyncTeX data.
 * 
 * Used by a Scanner instance.
 */

#ifndef SYNCTEX_BUFFER_CHEF_HPP
#define SYNCTEX_BUFFER_CHEF_HPP

#include <algorithm>
#include <memory>
#include <limits>
#include <functional>
#include <cctype>

#include "SyncTeXUtils.hpp"

namespace SyncTeXpp {

using Restore_f= std::function<void(void)>;

/**
 * @brief Variable sized buffer.
 * 
 * Pure virtual class.
 */
class BufferChef {
    public:
    /**
     * @brief The `max` is 1 Mb.
     * 
     */
    struct Size {
        static constexpr int int_max         = INT_MAX;
        static constexpr int int_max_over_10 = int_max / 10;
        static constexpr int int_max_mod_10  = int_max % 10;
        static constexpr int int10   = (8*sizeof(int))*0.301;
        static constexpr int float10 =
            std::numeric_limits<float>::max_digits10
                + std::numeric_limits<float>::max_exponent10
        ;
        static constexpr int min = std::max(int10, float10);
        static constexpr int max = 2<<19; // 1Mo
        static constexpr int capacity = std::max(min, std::max(PATH_MAX, 2<<12));
    };
    public:
    static inline const std::array<std::pair<std::string, float>, 11> By_unit = {{
        {"sp", 1.0f        },   // 1.0f
        {"pt", 65536.0f    },   // 65536.0f
        {"bp", 65781.76f   },   // 72.27f / 72 * 65536.0
        {"nd", 69925.4829f },   // 685.0f / 642 * 65536
        {"dd", 70124.0864f },   // 1238.0f / 1157 * 65536.0
        {"mm", 186467.9811f},   // 72.27f * 65536 / 25.4
        {"pc", 786432.0f   },   // 12.0f * 65536.0
        {"nc", 839105.7944f},   // 1370.0f / 107 * 65536
        {"cc", 841489.0372f},   // 14856.0f / 1157 * 65536
        {"cm", 1864679.811f},   // 72.27f * 65536 / 2.54
        {"in", 4736286.72f },   // 72.27f * 65536
    }};

    private:
    int _capacity = Size::capacity;
    int _next_capacity = Size::capacity;
    int _size = 0;
    int _position = 0;
    tlc_hv_WHD_s _last = {0, 0, 0, 0, 0, 0, 0, 0};
    std::unique_ptr<char[]> _data;
    public:
    BufferChef(const BufferChef&) = delete;
    BufferChef& operator=(const BufferChef&) = delete;
    BufferChef(BufferChef&&) = delete;
    BufferChef& operator=(BufferChef&&) = delete;
    public:
    BufferChef(int capacity = Size::min, bool force = false) {
        reset(capacity, force);
    }
    bool reset(int capacity = -1, bool force = false) {
        set_next_capacity(capacity, force);
        _capacity = _next_capacity;
        set_size(0);
        if (_capacity == 0) {
            _data = nullptr;
        } else {
            _data = std::make_unique<char[]>(_capacity+1);
            auto p = _data.get();
            if (p) p[_capacity] = '\0'; // ensure a null terminated buffer
        }
        return _data != nullptr;
    };
    int next_capacity()  { return _next_capacity; }
    int capacity()  { return _capacity; }
    int size()      { return _size; }
    int position()  { return _position; }
    private:
    /**
     * @brief The capacity of the buffer, for the next reset.
     * 
     * 
     * 
     * @param capacity new value when positive, enlarge when negative, no change when 0.
     */
    void set_next_capacity(int capacity = 0, bool force = false)
    {
        if (capacity > 0) {
            if (force) {
                _next_capacity = capacity;
            } else {
                _next_capacity = std::max(
                    Size::min,
                    std::min(capacity, Size::max)
                );
            }
        }
    }
    protected:
    void set_size(int size = 0)
    {
        size = std::min(_capacity, size);
        _size = std::max(0, size);
        advance(0);
    };
    public:
    int enlarge(int delta = 0)
    {
        if (0 < delta) {
            if (delta <= _capacity - _size) {
                _size += delta;
            } else {
                _size = _capacity;
            }
        } else if (delta < 0) {
            if (-delta <= _size) {
                _size += delta;
            } else {
                _size = 0;
            }
        }
        advance(0);
        return _size;
    };
    int advance(int i) {
        if (i < _size - _position) {
            if (-i < _position) {
                _position += i;
            } else {
                _position = 0;
            }
        } else {
            _position = _size;
        }
        return _position;
    }
    void shift() {
        if (_position > 0) {
            auto p = _data.get();
            if (p ) {
                std::move( p + _position, p + _size, p);
                _size -= _position;
                _position = 0;
            }
        }
    }
    const char* data(int &size, int index=0) {
        auto p = _data.get();
        if (p) {
            index += _position;
            if (index >= 0 ) {
                if (index < _size ) {
                    size = _size - index;
                    return p + index;
                }
                size = 0;
                return p + _size;
            } else {
                size = _size;
                return p;
            }
        }
        size = 0;
        return nullptr;
    }
    protected:
    char* mutable_data() {
        return _data.get();
    }
    virtual int fill(char* ptr, int len) = 0;
    public:
    /**
     * @brief Ensure <expected> chars of the buffer are filled.
     * 
     * @param expected 
     * @return Status::Done means that <expected> chars are available
     * @return Status::EndOfCapacity means that only <capacity()> chars are available but more are possible.
     * @return Status::EndOfData means that less than <expected> chars are available and more is not possible.
     * @return Status::Failed means that no data is available at all.
     */
    Status expect(int expected) {
        int available = _size - _position;
        if (expected <= available) {
            return Status::Done;
        }
        auto p = _data.get();
        if (p && _capacity) {
            shift();
            auto len = _capacity - _size;
            int filled = fill(p + _size, len);
            if (filled > 0) enlarge(filled);
            else if (filled < 0) return Status::Error;
            if (_capacity < expected) {
                return Status::EndOfCapacity;
            }
            // now: expected <= _capacity !
            if (_size <= 0) {
                return Status::Failed;
            }
            return _size < expected? Status::EndOfData :  Status::Done;
        }
        return Status::Failed;
    }
    char operator[](int i) const {
        return at(i);
    }
    char at(int i) const {
        auto p = _data.get();
        if (p) {
            if ( i < 0) {
                if (i + _size >= _position ) {
                    return p[i+_size];
                }
            } else if (_position < _size - i) {
                return p[_position + i];
            }
        }
        return 0;
    }

    bool atEnd() {
        return _position >= _size;
    }
    /**
     * @brief Advance past the first EOL marker or to the end.
     * 
     * CRLF on windows
     * 
     * @return Status::Done when we are at the beginning of the next line
     * @return Status::EndOfData means that no new line was found until the end was reached.
     */
    Status skip_endl() {
        while(Status::Done == expect(1)) {
            auto c = at(0);
            if ( c == '\r' ) {
                advance(1);
                if ((Status::Done == expect(1)) && (at(0) == '\n') ) {
                    advance(1);
                }
                return Status::Done;
            } else if ( c == '\n' ) {
                advance(1);
                return Status::Done;
            }
            advance(1);
        }
        return Status::EndOfData;
    };
    /**
     * @brief Advance past the first EOL marker.
     * 
     * CRLF on windows
     * 
     * @return Status::Done when we are at the beginning of the next line
     * @return Status::Failed means that no new line was found until the end is reached, the position is unchanged.
     */
    Status require_endl() {
        Restore_f restore = save();
        while(Status::Done == expect(1)) {
            auto c = at(0);
            if ( c == '\r' ) {
                advance(1);
                if ((Status::Done == expect(1)) && (at(0) == '\n') ) {
                    advance(1);
                }
                return Status::Done;
            } else if ( c == '\n' ) {
                advance(1);
                return Status::Done;
            }
            advance(1);
        }
        restore();
        return Status::EndOfData;
    };
    /**
     * @brief Read a given char from the buffer.
     * 
     * Use `expect(1)`.
     * 
     * @param c The char to read
     * @param nextline skip to the next line when true, defaults to true.
     * @return Status::Done when the char is read, use `advance(1)`,
     * @return Status::EndOfData when there is no more char,
     * @return Status::Failed when there is a char but it is not the expected one.
     */
    Status read_char(char c, bool nextline = false) {
        if (Status::Done == expect(1)) {
            if (at(0) != c) return Status::Failed;
            advance(1);
            if (nextline) skip_endl();
            return Status::Done;
        }
        return Status::EndOfData;
    }

    /**
     * @brief Save the current buffer state
     * 
     * The default implementation returns a noop function.
     * 
     * @return a function to restore the current buffer state when called
     */
    virtual Restore_f save() {
        return [](){};
    }

    /**
     * @brief Reads a prefix at the current position
     * 
     * and point to the character just after the <prefix>
     * 
     * @param prefix 
     * @return Status::Done if the <prefix> was read,
     * @return Status::Failed when there are characters but they do not patch,
     * @return Status::EndOfData when there are no characters at all.
     */
    Status read_string(const std::string_view &prefix) {
        int size = 0;
        auto p = data(size);
        if (p == nullptr || _capacity <= 0) {
            return Status::EndOfData;
        }
        auto len = prefix.length();
        Restore_f restore = nullptr;
        int i = 0; // prefix index
        int j = 0; // data index
        while(true) {
            const auto status = expect(len);
            if (Status::Done == status) {
                for (j = 0 ; j < len ; ++i, ++j) {
                    if (prefix[i] != at(j)) {
                        if (restore) restore();
                        return Status::Failed;
                    }
                }
                advance(len);
                return Status::Done;
            }
            // len > size
            if (Status::EndOfData == status) {
                if (restore) restore();
                return Status::EndOfData;
            }
            // Status::EndOfCapacity == status
            if (!restore) restore = save();
            for (j = 0 ; j < size ; ++i, ++j) {
                if (prefix[i] != at(j)) {
                    if (restore) restore();
                    return Status::Failed;
                }
            }
            // The next <_capacity> characters are goods
            len -= size;
            advance(size);
            // next time we reach the end of the capacity,
            // this is for the whole buffer
            size = _capacity;
        }
        // Unreachable
    }

    /**
     * @brief Require a head string
     * 
     * Read the heading of the next line until it finds <head>.
     * It must be followed by a colon, which is read.
     * 
     * @param head 
     * @param nextline when false, we stay here, otherwise we skip the end of the line, defaults to false,
     * @return Status::Done when we found the header and only then
     */
    Status require_head(
        const std::string_view &head,
        bool nextline = false
    ) {
        auto restore = save();
        auto status = Status::Done;
        while(true) {
            if (Status::Done == (status = read_string(head))) {
                if (Status::Done == (status = read_char(':'))) {
                    if (nextline) skip_endl();
                    return status;
                }
            }
            if (Status::Done < (status = require_endl())) {
                restore();
                return status;
            }
        }
    }

    /**
     * @brief Decode a string
     * 
     * Append to <str> everything up to the next endl or the end
     * @param str 
     * @param separator a separator before the string, defaults to '\0'. 
     * @return Status::Done The string is decoded and the receiver points to the start of the next line
     * @return Status::Failed The separator is missing
     * @return Status::EndOfData The string is decoded but there is no next line
     */
    Status decode(std::string &str, char separator = '\0') {
        auto p = _data.get();
        if (p == nullptr || _capacity <= 0) {
            return Status::EndOfData;        
        }
        if (_position >= _capacity) shift();
        if (separator && Status::Done < read_char(separator)) {
            return Status::Failed; 
        }
        int start = _position; // append count chars from p+start
        int count = 0;

        while(Status::Done == expect(count+1)) {
            auto c = p[start+count];
            if ( c == '\r' ) {
                if (count) {
                    str.append(p+start, p+start+count);
                }
                advance(count+1);
                if ((Status::Done == expect(1)) && (at(0) == '\n') ) {
                    advance(1);
                }
                return Status::Done;
            } else if ( c == '\n' ) {
                if (count) {
                    str.append(p+start, p+start+count);
                }
                advance(count+1);
                return Status::Done;
            }
            ++count;
            if (count == _capacity - start) {
                // End of buffer reached:
                str.append(p + start, p + _capacity);
                advance(count);
                shift();
                start = count = 0;
            }
        }
        if (count) {
            str.append(p+start, p+start+count);
            advance(count);
        }
        return Status::EndOfData;
    }

    /**
     * @brief decode a decimal integer at the current position, with default value
     * 
     * No overflow management, finger crossed.
     * We make the assumption that integers where written
     * 
     * @param value where the integer is stored
     * @param separator a char just before the integer, in general ',' or ':', defaults to '\0'.
     * @param default_value used when '=' is scanned, defaults to 0
     * @return Status::Done means everything went OK
     * @return Status::EndOfData means that no data is available.
     * @return Status::Failed for all other cases
     * @return 
     */
    Status decode(int& value, char separator = '\0', int default_value = 0) {
        auto p = _data.get();
        if (p == nullptr || _capacity <= 0) {
            return Status::EndOfData;
        }
        if (Status::Done < expect(1)) return Status::EndOfData;
        auto c = p[_position];
        if (separator) {
            if (c != separator) return Status::Failed;
            advance(1);
            if (Status::Done < expect(1)) return Status::Failed;
            c = p[_position];
        }
        bool negative = false;
        if ( c=='=' ) {
            advance(1);
            value = default_value;
            return Status::Done;
        }
        if (c == '-') {
            negative = true;
            advance(1);
            if (Status::Done < expect(1)) return Status::Failed;
            c = p[_position];
        }
        if (!std::isdigit(c)) return Status::Failed;
        value = c - '0';
        advance(1);
        while (Status::Done == expect(1)) {
            c = p[_position];
            if (std::isdigit(c)) {
                value = 10 * value + (c - '0');
                advance(1);
                continue;
            }
            // c is not a digit, we can return
            if (negative) value = -value;
            return Status::Done;
        } // end of while ((Status::Done == expect(1)))
        return Status::EndOfData;
    }

    /**
     * @brief decode a decimal integer at the current position, with default value
     * 
     * No overflow management, finger crossed.
     * We make the assumption that integers where written
     * 
     * @param value where the integer is stored
     * @param default_value used when '=' is scanned
     * @return Status::Done means everything went OK
     * @return Status::EndOfData means that no data is available.
     * @return Status::Failed for all other cases
     * @return 
     */
    Status decode(int& value, int default_value) {
        return decode(value, '\0', default_value);
    }

    /**
     * @brief Decode a float
     * 
     * A float is an integer when there is no dot or [+|-] digits?.digits when there is.
     * 
     * @param  value contains the result
     * @return 
     */
    Status decode(float & value, char separator = '\0') {
        auto p = _data.get();
        if (p == nullptr || _capacity <= 0) {
            return Status::EndOfData;
        }
        auto current = [this, p] (char &c) -> Status {
            if (Status::Done < expect(1)) return Status::EndOfData;
            c = p[_position];
            return Status::Done;
        };
        auto next = [this, current] (char &c) -> Status {
            advance(1);
            return current(c);
        };
        char c = 0;
        if (Status::Done < current(c)) return Status::EndOfData;
        if (separator) {
            if (c != separator) return Status::Failed;
            if (Status::Done < next(c)) return Status::Failed;
        }
        bool negative = false;
        if (c == '-') {
            negative = true;
            if (Status::Done < next(c)) return Status::Failed;
        }
        bool before = true; // before the dot
        int count = 0; // how many digits after the dot
        if (c == '.') {
            before = false;
            if (Status::Done < next(c)) return Status::Failed;
        }
        if (!std::isdigit(c)) return Status::Failed;
        value = c - '0';
        if (!before) ++count;
        while (Status::Done == next(c)) {
            if (std::isdigit(c)) {
                value = 10 * value + (c - '0');
                if (!before) ++count;
            } else if (before && (c == '.')) {
                before = false;
            } else {
                if (negative) value = -value;
                while (count > 3) {
                    count -= 3;
                    value /= 1000;
                }
                while (count--) {
                    value /= 10;
                }
                return Status::Done;
            }
        } // end of while ((Status::Done == expect(1)))
        return Status::EndOfData;
    }

    /**
     * @brief Decode the tlc+hv+wgd combos
     * 
     * @param value value storage for the depth
     * @return Status::Done on success and only then.
     * On failure variables given by refernce are left untouched. 
     */
    Status decode_tlc_hv_WHD(
        tlc_hv_WHD_s &value,
        char separator = '\0',
        bool next = false
    ) {
        Status status;
        auto restore = save();
        tlc_hv_WHD_s v;
        if (Status::Done < (status = decode(v.t, separator)) ||
            Status::Done < (status = decode(v.l, ','))
        ) {
            restore();
            return status;
        }
        if (Status::Done < (status = decode(v.c, ','))) {
            v.c = value.c;
        }
        if (Status::Done < (status = decode(v.h, ':', _last.h)) ||
            Status::Done < (status = decode(v.v, ',', _last.v)) ||
            Status::Done < (status = decode(v.W, ':', _last.W)) ||
            Status::Done < (status = decode(v.H, ',', _last.H)) ||
            Status::Done < (status = decode(v.D, ',', _last.D))
        ) {
            restore();
            return status;
        }
        value = v;
        skip_endl();
        return Status::Done;
    }

    /**
     * @brief Decode the tlc+hv combos
     * 
     * @param value value storage for the depth
     * @param separator before the information decoded, defaults to '\0', none.
     * @param next whether
     * @return Status::Done on success and only then.
     * On failure variables given by refernce are left untouched. 
     */
    Status decode_tlc_hv(tlc_hv_s &value, char separator = '\0', bool next = false) {
        auto restore = save();
        tlc_hv_s v;
        Status status;
        if (Status::Done < (status = decode(v.t, separator)) ||
            Status::Done < (status = decode(v.l, ','))
        ) {
            restore();
            return status;
        }
        if (Status::Done < (status = decode(v.c, ','))) {
            v.c = value.c;
        }
        if (Status::Done < (status = decode(v.h, ':', _last.h)) ||
            Status::Done < (status = decode(v.v, ',', _last.v))
        ) {
            restore();
            return status;
        }
        value = v;
        skip_endl();
        return Status::Done;
    }

    /**
     * @brief Decode the tlc+hv+W combos
     * 
     * @param value value storage for the depth
     * @param separator before the information decoded, defaults to '\0', none.
     * @param value value storage for the depth
     * @return Status::Done on success and only then.
     * On failure variables given by reference are left untouched. 
     */
    Status decode_tlc_hv_W(tlc_hv_W_s &value, char separator = '\0', bool next = false) {
        auto restore = save();
        tlc_hv_W_s v;
        Status status;
        if (Status::Done < (status = decode(v.t, separator)) ||
            Status::Done < (status = decode(v.l, ','))
        ) {
            restore();
            return status;
        }
        if (Status::Done < (status = decode(v.c, ','))) {
            v.c = value.c;
        }
        if (Status::Done < (status = decode(v.h, ':', _last.h)) ||
            Status::Done < (status = decode(v.v, ',', _last.v)) ||
            Status::Done < (status = decode(v.W, ':', _last.W))
        ) {
            restore();
            return status;
        }
        value = v;
        skip_endl();
        return Status::Done;
    }

    /**
     * @brief Decode the t+hv combos-
     * 
     * @param value value storage for the depth
     * @param separator before the information decoded, defaults to '\0', none.
     * @param next whether
     * @return Status::Done on success and only then.
     * On failure variables given by refernce are left untouched. 
     */
    Status decode_t_hv(t_hv_s &value, char separator = '\0', bool next = false) {
        auto restore = save();
        t_hv_s v;
        Status status;
        if (Status::Done < (status = decode(v.t, separator)) ||
            Status::Done < (status = decode(v.h, ':', _last.h)) ||
            Status::Done < (status = decode(v.v, ',', _last.v))
        ) {
            restore();
            return status;
        }
        value = v;
        skip_endl();
        return Status::Done;
    }

    /**
     * @brief Read a float and a dimention
     * 
     *
     * @param value storage for the result
     * @return Status::Done when properly done,
     * @return Status::ErrorDataFloat when no float is there,
     * @return Status::ErrorDataUnit when no unit is there.
     */
    Status decode_united(float &value) {
        auto restore = save();
        auto status = decode(value);
        if (Status::Done < status) {
            /* restore(); Useless */
            return Status::ErrorDataFloat;
        }
        for (const auto& pair: By_unit) {
            if ( Status::Done == read_string(pair.first)) {
                value *= pair.second;
                return Status::Done;
            }
        }
        restore();
        return Status::ErrorDataUnit;
    }

#ifdef SYNCTEX_TEST
    friend class TestInspector;
#endif

};

}; // namespace SyncTeXpp

#endif // SYNCTEX_BUFFER_CHEF_HPP

