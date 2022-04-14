#pragma once

#ifndef EMBIT_FILTER_HPP
#    define EMBIT_FILTER_HPP

#    include "FunctionWrapper.hpp"

namespace embit {
namespace detail {
template<class Iterator, class Sentinel, class Func>
class FilterIterator {
    Iterator _iterator{};
    Sentinel _last{};
    mutable detail::FunctionWrapper<Func> _f{};

    using Traits = std::iterator_traits<Iterator>;

    constexpr void find() {
        while (_iterator != _last) {
            if (_f(*_iterator)) break;
            ++_iterator;
        }
    }

public:
    using reference = typename Traits::reference;
    using value_type = typename Traits::value_type;
    using iterator_category = std::bidirectional_iterator_tag;
    using difference_type = typename Traits::difference_type;
    using pointer = typename Traits::pointer;

    constexpr FilterIterator(Iterator current, Sentinel last, Func f) :
        _iterator(std::move(current)),
        _last(std::move(last)),
        _f(std::move(f)) {
        find();
    }

    FilterIterator() = default;

    constexpr reference operator*() const {
        return *_iterator;
    }

    constexpr FilterIterator& operator++() {
        ++_iterator;
        find();
        return *this;
    }

    constexpr FilterIterator operator++(int) {
        auto tmp(*this);
        ++*this;
        return tmp;
    }

    constexpr FilterIterator& operator--() {
        while (_iterator != _last) {
            --_iterator;
            if (_f(*_iterator)) break;
        }
        return *this;
    }

    constexpr FilterIterator operator--(int) {
        auto tmp(*this);
        ++*this;
        return tmp;
    }

    constexpr friend bool operator==(const FilterIterator& a, DefaultSentinel) noexcept {
        return a._iterator == a._last;
    }

    constexpr friend bool operator!=(const FilterIterator& a, DefaultSentinel s) noexcept {
        return !(a == s);
    }

    constexpr friend bool operator==(const FilterIterator& a, const FilterIterator& b) noexcept {
        return a._iterator == b._iterator;
    }

    constexpr friend bool operator!=(const FilterIterator& a, const FilterIterator& b) noexcept {
        return !(a == b);
    }

#    ifdef __cpp_if_constexpr
    EMBIT_CONSTEXPR_CXX_20 void swapView() noexcept {
        if constexpr (!HasSwapView<I>::value) {
            std::swap(_iterator, _last);
        }
        else {
            _iterator.swapView();
        }
    }

    constexpr View<FilterIterator<Char, Char, Func>, FilterIterator<Char, Char, Func>> toCommon() const {
        using It = FilterIterator<Char, Char, Func>;
        if constexpr (std::is_same_v<Char, Sentinel>) {
            return view(It(_iterator, _last, _f.get()), It(_last, _last, _f.get()));
        }
        else {
            auto v = _iterator.toCommon();
            auto begin = std::begin(v);
            auto end = std::end(v);
            return view(It(begin, end, _f.get()), It(end, end, _f.get()));
        }
    }
#    else
    template<class I = Iterator>
    EnableIf<!HasSwapView<I>::value> swapView() {
        std::swap(_iterator, _last);
    }

    template<class I = Iterator>
    EnableIf<HasSwapView<I>::value> swapView() {
        _iterator.swapView();
    }

    EMBIT_CONSTEXPR_CXX_20 reference back() const {
        auto tmp(*this);
        tmp.swapView();
        return *--tmp;
    }

    template<class I = Iterator>
    constexpr EnableIf<!HasToCommon<I>::value, View<FilterIterator<I, I, Func>, FilterIterator<I, I, Func>>>
    toCommon() const {
        using It = FilterIterator<I, I, Func>;
        return view(It(_iterator, _last, _f.get()), It(_last, _last, _f.get()));
    }

    template<class I = Iterator>
    constexpr EnableIf<HasToCommon<I>::value, View<FilterIterator<I, I, Func>, FilterIterator<I, I, Func>>>
    toCommon() const {
        using It = FilterIterator<I, I, Func>;
        auto v = _iterator.toCommon();
        auto begin = std::begin(v);
        auto end = std::end(v);
        return view(It(std::move(begin), end, _f.get()), It(end, end, _f.get()));
    }
#    endif
};
} // namespace detail

template<class Iterator, class Sentinel, class Func>
class FilterView : public View<detail::FilterIterator<Iterator, Sentinel, Func>, DefaultSentinel> {
public:
    using iterator = detail::FilterIterator<Iterator, Sentinel, Func>;
    using const_iterator = iterator;

    constexpr FilterView(Iterator first, Sentinel last, Func func) :
        View<iterator, DefaultSentinel>(iterator(std::move(first), std::move(last), std::move(func)), defaultSentinel) {
    }

    FilterView() = default;
};

template<class V, class Func>
constexpr FilterView<BeginIter<V>, EndIter<V>, detail::Decay<Func>> filter(V&& view, Func&& f) {
    return { std::begin(view), std::end(view), std::forward<Func>(f) };
}
} // namespace embit
#endif