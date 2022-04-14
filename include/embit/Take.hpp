#pragma once

#ifndef EMBIT_TAKE_HPP
#    define EMBIT_TAKE_HPP

#    include "Core.hpp"

namespace embit {
namespace detail {
template<class Iterator, class Sentinel>
class TakeIterator {
    Iterator _iterator{};
    Sentinel _last{};
    std::size_t _amount{};
    std::size_t _current{};

    using Traits = std::iterator_traits<Iterator>;

public:
    using reference = typename Traits::reference;
    using value_type = typename Traits::value_type;
    using iterator_category = std::forward_iterator_tag;
    using difference_type = typename Traits::difference_type;
    using pointer = typename Traits::pointer;

    TakeIterator(Iterator iterator, Sentinel end, const std::size_t amount) : _iterator(iterator), _amount(amount) {
    }

    TakeIterator() = default;

    constexpr reference operator*() const {
        return *_iterator;
    }

    constexpr TakeIterator& operator++() {
        ++_current;
        ++_iterator;
        return *this;
    }

    constexpr TakeIterator operator++(int) {
        auto tmp(*this);
        ++*this;
        return tmp;
    }

    constexpr friend bool operator==(const TakeIterator& a, DefaultSentinel) noexcept {
        return a._current == a._amount;
    }

    constexpr friend bool operator!=(const TakeIterator& a, DefaultSentinel s) noexcept {
        return !(a == s);
    }

    constexpr friend bool operator==(const TakeIterator& a, const TakeIterator& b) noexcept {
        return a._iterator == b._iterator;
    }

    constexpr friend bool operator!=(const TakeIterator& a, const TakeIterator& b) noexcept {
        return !(a == b);
    }

#    ifdef __cpp_if_constexpr
    EMBIT_CONSTEXPR_CXX_20 void swapView() noexcept {
        if constexpr (std::is_same<Char, Sentinel>::value) {
            std::swap(_iterator, _last);
        }
        else {
            _iterator.swapView();
        }
    }
#    else
    template<class I = Iterator>
    EnableIf<std::is_same<I, Sentinel>::value> swapView() {
        std::swap(_iterator, _last);
    }

    template<class I = Iterator>
    EnableIf<!std::is_same<I, Sentinel>::value> swapView() {
        _iterator.swapView();
    }
#    endif
};
} // namespace detail

template<class Iterator, class Sentinel>
class TakeView : public View<detail::TakeIterator<Iterator, Sentinel>, DefaultSentinel> {
public:
    using iterator = detail::TakeIterator<Iterator, Sentinel>;
    using const_iterator = iterator;

    TakeView(Iterator iter, Sentinel last, const IterDiffType<Iterator> amount) :
        View<iterator, DefaultSentinel>(iterator(std::move(iter), amount), defaultSentinel) {
    }

    TakeView() = default;
};

template<class V>
auto take(V&& view, const IterDiffType<BeginIter<V>> amount) -> TakeView<decltype(std::begin(view)), decltype(std::end(view))> {
    return { std::begin(view), std::end(view), amount };
}

template<class V>
auto counted(V&& view, const IterDiffType<BeginIter<V>> amount)
    -> TakeView<decltype(std::begin(view)), decltype(std::end(view))> {
    return take(view, amount);
}

template<class V>
auto slice(V&& view, const IterDiffType<BeginIter<V>> from, const IterDiffType<BeginIter<V>> to)
    -> TakeView<decltype(std::begin(view)), decltype(std::end(view))> {
    return { std::next(std::begin(view), from), std::end(view), to };
}
} // namespace embit

#endif // EMBIT_TAKE_HPP
