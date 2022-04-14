#pragma once

#ifndef EMBIT_CSTRING_HPP
#    define EMBIT_CSTRING_HPP

#    include "Core.hpp"

namespace embit {
namespace detail {
template<class Char>
class CStringIterator {
    const Char* _iterator{ nullptr };

    using Traits = std::iterator_traits<const Char*>;

public:
    using reference = typename Traits::reference;
    using value_type = typename Traits::value_type;
    using iterator_category = std::forward_iterator_tag;
    using difference_type = typename Traits::difference_type;
    using pointer = typename Traits::pointer;

    constexpr explicit CStringIterator(const Char* it) noexcept : _iterator(it) {
    }

    CStringIterator() = default;

    const Char* get() const {
        return _iterator;
    }

    constexpr reference operator*() const noexcept {
        return *_iterator;
    }

    constexpr CStringIterator& operator++() noexcept {
        ++_iterator;
        return *this;
    }

    constexpr CStringIterator operator++(int) noexcept {
        auto tmp(*this);
        ++*this;
        return tmp;
    }

    constexpr friend bool operator==(const CStringIterator& a, DefaultSentinel) noexcept {
        return *a._iterator == '\0';
    }

    constexpr friend bool operator!=(const CStringIterator& a, DefaultSentinel s) noexcept {
        return !(a == s);
    }

    constexpr friend bool operator==(const CStringIterator& a, const CStringIterator& b) noexcept {
        if (!b._iterator) {
            return a == defaultSentinel;
        }
        if (!a._iterator) {
            return b == defaultSentinel;
        }
        return a._iterator == b._iterator;
    }

    constexpr friend bool operator!=(const CStringIterator& a, const CStringIterator& b) noexcept {
        return !(a == b);
    }

    constexpr View<CStringIterator<Char>, CStringIterator<Char>> toCommon() const noexcept {
        return view(CStringIterator<Char>(_iterator), CStringIterator<Char>(nullptr));
    }

    constexpr explicit operator bool() const noexcept {
        return static_cast<bool>(_iterator);
    }
};
} // namespace detail

template<class Char, class S>
class CStringView : public View<detail::CStringIterator<Char>, S> {
public:
    using iterator = detail::CStringIterator<Char>;
    using Sentinel = S;
    using const_iterator = iterator;

    explicit CStringView(const Char* it, S last) noexcept : View<iterator, S>(iterator(it), last) {
    }

    CStringView() = default;

    constexpr explicit operator bool() const noexcept {
        return static_cast<bool>(this->begin());
    }
};

template<class Char>
constexpr CStringView<Char, DefaultSentinel> cstring(const Char* s) noexcept {
    return CStringView<Char, DefaultSentinel>(s, defaultSentinel);
}

template<class Char>
constexpr CStringView<Char, detail::CStringIterator<Char>> cstring(const Char* first, const Char* last) noexcept {
    using Iter = detail::CStringIterator<Char>;
    return CStringView<Char, Iter>(first, Iter(last));
}

template<class Char>
constexpr CStringView<Char, detail::CStringIterator<Char>> cstring(const Char* first, const std::size_t count) noexcept {
    return cstring(first, first + count);
}

} // namespace embit

#endif // EMBIT_CSTRING_HPP
