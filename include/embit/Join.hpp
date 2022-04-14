//
// Created by marcd on 14/04/2022.
//

#ifndef EMBIT_JOIN_HPP
#define EMBIT_JOIN_HPP

#include <embit/Core.hpp>

namespace embit {
namespace detail {
template<class Iterator, class Sentinel, class Char>
class JoinIterator {
    bool _useIterator{};
    Iterator _iterator{};
    Sentinel _last{};
    const Char* _delimiter{ nullptr };
    const Char* _delimiterIterator{ nullptr };

public:
    JoinIterator() = default;

    JoinIterator(Iterator first, Sentinel last, const Char* delimiter) :
        _useIterator(true),
        _iterator(std::move(first)),
        _last(std::move(last)),
        _delimiter(delimiter),
        _delimiterIterator(_delimiter) {
    }

    char operator*() const {
        if (_useIterator) {
            return *_iterator;
        }
        else {
            return *_delimiterIterator;
        }
    }

    JoinIterator& operator++() {
        if (_useIterator) {
            _useIterator = false;
            ++_iterator;
            _delimiterIterator = _delimiter;
        }
        else {
            _delimiterIterator++;
            _useIterator = *_delimiterIterator == '\0';
        }
        return *this;
    }

    friend bool operator!=(const JoinIterator& lhs, const JoinIterator& rhs) {
        return lhs._iterator != rhs._last;
    }

    friend bool operator!=(const JoinIterator& lhs, const embit::DefaultSentinel) {
        return lhs._iterator != lhs._last;
    }
};
} // namespace detail

template<class Iterator, class Sentinel, class Char>
class Join {
public:
    using iterator = detail::JoinIterator<Iterator, Sentinel, Char>;

private:
    detail::JoinIterator<Iterator, Sentinel, Char> _iterator;

public:
    using iterator = detail::JoinIterator<Iterator, Sentinel, Char>;

    Join() = default;

    Join(Iterator i, Sentinel s, const Char* delimiter) : _iterator(std::move(i), std::move(s), delimiter) {
    }

    iterator begin() const {
        return _iterator;
    }

    DefaultSentinel end() const {
        return embit::defaultSentinel;
    }
};

template<class V, class Char>
Join<embit::BeginIter<V>, embit::EndIter<V>, Char> join(V&& view, const Char* delimiter) {
    return { std::begin(view), std::end(view), delimiter };
}
} // namespace embit
#endif // EMBIT_JOIN_HPP
