//
// Created by marcd on 24/12/2021.
//

#ifndef EMBIT_FLATTEN_HPP
#define EMBIT_FLATTEN_HPP

#include <iterator>

template<class, class U>
struct AliasWrapper {
    using Type = U;
};

template<class T, class U = void>
using AliasWrapperT = typename AliasWrapper<T, U>::Type;

template<class T, class Enable = void>
struct HasValueType : std::false_type {};

template<class T>
struct HasValueType<T, AliasWrapperT<typename T::value_type>> : std::true_type {};

template<class T, class Enable = void>
struct HasDifferenceType : std::false_type {};

template<class T>
struct HasDifferenceType<T, AliasWrapperT<typename T::difference_type>> : std::true_type {};

template<class T, class Enable = void>
struct HasPointer : std::false_type {};

template<class T>
struct HasPointer<T, AliasWrapperT<typename T::pointer>> : std::true_type {};

template<class T, class Enable = void>
struct HasIterCat : std::false_type {};

template<class T>
struct HasIterCat<T, AliasWrapperT<typename T::iterator_category>> : std::true_type {};

template<class T, class Enable = void>
struct HasReference : std::false_type {};

template<class T>
struct HasReference<T, AliasWrapperT<typename T::reference>> : std::true_type {};

template<class T>
struct IsIterator {
    static constexpr bool value = HasValueType<T>::value && HasDifferenceType<T>::value && HasPointer<T>::value &&
                                  HasReference<T>::value && HasIterCat<T>::value;
};

template<class T, class = void>
struct IterTraitsOrUnderlyingType {
    using Type = T;
};

template<class T>
struct IterTraitsOrUnderlyingType<T, AliasWrapperT<typename T::iterator>> {
    using Type = std::iterator_traits<typename T::iterator>;
};

template<class T, class U = void>
using IterTraitsOrUnderlyingTypeT = typename IterTraitsOrUnderlyingType<T, U>::Type;

template<bool B>
struct CountDimsHelper;

template<>
struct CountDimsHelper<true> {
    template<class T>
    using Inner = IterTraitsOrUnderlyingTypeT<typename T::value_type>;

    template<class T>
    using type = std::integral_constant<int, 1 + CountDimsHelper<IsIterator<Inner<T>>::value>::template type<Inner<T>>::value>;
};

template<>
struct CountDimsHelper<false> {
    template<class>
    using type = std::integral_constant<int, 0>;
};

template<class T>
using CountDims = typename CountDimsHelper<IsIterator<T>::value>::template type<T>;

// Improvement of https://stackoverflow.com/a/21076724/8729023
template<class Iterator>
class FlattenWrapper {
    Iterator _begin{};
    Iterator _current{};
    Iterator _end{};

    using IterTraits = std::iterator_traits<Iterator>;

public:
    using reference = typename IterTraits::reference;
    using pointer = typename IterTraits::pointer;
    using value_type = typename IterTraits::value_type;
    using iterator_category =
        typename std::common_type<std::bidirectional_iterator_tag, typename IterTraits::iterator_category>::type;
    using difference_type = typename IterTraits::difference_type;

    constexpr FlattenWrapper() = default;

    constexpr FlattenWrapper(Iterator current, Iterator begin, Iterator end) :
        _begin(std::move(begin)),
        _current(std::move(current)),
        _end(std::move(end)) {
    }

    void toEnd() {
        _current = _end;
    }

    constexpr bool hasSome() const noexcept {
        return _current != _end;
    }

    constexpr bool hasPrev() const noexcept {
        return _current != _begin;
    }

    constexpr friend bool operator!=(const FlattenWrapper& a, const FlattenWrapper& b) noexcept {
        return a._current != b._current;
    }

    constexpr friend bool operator==(const FlattenWrapper& a, const FlattenWrapper& b) noexcept {
        return !(a != b); // NOLINT
    }

    constexpr reference operator*() const {
        return *_current;
    }

    constexpr pointer operator->() const {
        return &*this;
    }

    constexpr FlattenWrapper& operator++() {
        ++_current;
        return *this;
    }

    constexpr FlattenWrapper operator++(int) {
        FlattenWrapper tmp(*this);
        ++*this;
        return tmp;
    }

    constexpr FlattenWrapper& operator--() {
        --_current;
        return *this;
    }

    constexpr FlattenWrapper operator--(int) {
        FlattenWrapper tmp(*this);
        ++*this;
        return tmp;
    }
};

template<class Iterator, int N>
class FlattenIterator {
    using Inner = FlattenIterator<decltype(std::begin(*std::declval<Iterator>())), N - 1>;

public:
    using reference = typename Inner::reference;
    using pointer = typename Inner::pointer;
    using value_type = typename Inner::value_type;
    using iterator_category =
        typename std::common_type<std::bidirectional_iterator_tag, typename Inner::iterator_category>::type;
    using difference_type = typename Inner::difference_type;

private:
    constexpr void advance() {
        if (_innerIter.hasSome()) {
            return;
        }
        for (++_outerIter; _outerIter.hasSome(); ++_outerIter) {
            const auto begin = std::begin(*_outerIter);
            _innerIter = { begin, begin, std::end(*_outerIter) };
            if (_innerIter.hasSome()) {
                return;
            }
        }
        _innerIter = {};
    }

    FlattenWrapper<Iterator> _outerIter{};
    Inner _innerIter{};

public:
    constexpr FlattenIterator() = default;

    constexpr FlattenIterator(Iterator it, Iterator begin, Iterator end) :
        _outerIter(std::move(it), std::move(begin), std::move(end)) {
        if (_outerIter.hasSome()) {
            const auto beg = std::begin(*_outerIter);
            _innerIter = { beg, beg, std::end(*_outerIter) };
            this->advance();
        }
    }

    constexpr bool hasSome() const {
        return _outerIter.hasSome();
    }

    constexpr bool hasPrev() const {
        return _innerIter.hasPrev() || _outerIter.hasPrev();
    }

    constexpr friend bool operator!=(const FlattenIterator& a, const FlattenIterator& b) noexcept {
        return a._outerIter != b._outerIter || a._innerIter != b._innerIter;
    }

    constexpr friend bool operator==(const FlattenIterator& a, const FlattenIterator& b) noexcept {
        return !(a != b); // NOLINT
    }

    constexpr reference operator*() const {
        return *_innerIter;
    }

    constexpr pointer operator->() const {
        return &*this;
    }

    constexpr FlattenIterator& operator++() {
        ++_innerIter;
        this->advance();
        return *this;
    }

    constexpr FlattenIterator operator++(int) {
        FlattenIterator tmp(*this);
        ++*this;
        return tmp;
    }

    constexpr FlattenIterator& operator--() {
        if (_innerIter.hasPrev()) {
            --_innerIter;
            return *this;
        }
        while (_outerIter.hasPrev()) {
            --_outerIter;
            const auto end = std::end(*_outerIter);
            _innerIter = { end, std::begin(*_outerIter), end };
            if (_innerIter.hasPrev()) {
                --_innerIter;
                return *this;
            }
        }
        return *this;
    }

    constexpr FlattenIterator operator--(int) {
        FlattenIterator tmp(*this);
        --*this;
        return tmp;
    }
};

template<class Iterator>
class FlattenIterator<Iterator, 0> {
    FlattenWrapper<Iterator> _range;
    using Traits = std::iterator_traits<Iterator>;

public:
    using pointer = typename Traits::pointer;
    using reference = typename Traits::reference;
    using value_type = typename Traits::value_type;
    using iterator_category =
        typename std::common_type<std::bidirectional_iterator_tag, typename Traits::iterator_category>::type;
    using difference_type = typename Traits::difference_type;

    constexpr FlattenIterator() = default;

    constexpr FlattenIterator(Iterator it, Iterator begin, Iterator end) :
        _range(std::move(it), std::move(begin), std::move(end)) {
    }

    constexpr bool hasPrev() const noexcept {
        return _range.hasPrev();
    }

    constexpr bool hasSome() const noexcept {
        return _range.hasSome();
    }

    constexpr reference operator*() const {
        return *_range;
    }

    constexpr pointer operator->() const {
        return &*_range;
    }

    constexpr friend bool operator!=(const FlattenIterator& a, const FlattenIterator& b) noexcept {
        return a._range != b._range;
    }

    constexpr friend bool operator==(const FlattenIterator& a, const FlattenIterator& b) noexcept {
        return !(a != b); // NOLINT
    }

    constexpr FlattenIterator& operator++() {
        ++_range;
        return *this;
    }

    constexpr FlattenIterator operator++(int) {
        FlattenIterator tmp(*this);
        ++*this;
        return tmp;
    }

    constexpr FlattenIterator& operator--() {
        --_range;
        return *this;
    }

    constexpr FlattenIterator operator--(int) {
        FlattenIterator tmp(*this);
        --*this;
        return tmp;
    }
};

#endif // EMBIT_FLATTEN_HPP
