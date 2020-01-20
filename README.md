# Heterogeneous ranges

## Goals

### Ease of use

With ranges coming to c++20 it is the goal of this library to use provide a syntax as close as possible to the originals whilst working with the limitations of iterating heterogeneous views. 

### Lazy

Unlike in [boost.hana](https://github.com/boostorg/hana), views are constructed lazily. There are not potential unnecessary copies taking place. Whilst this property is not important for hana's main usecase of operating on types, it can be for many applications.

## Example

Assume you have a tuple of objects and for some reason you're only interested in integers (or any other constant condition).

```cpp
// we only care about the integers in this tuple
auto tup = std::make_tuple("Hello hera!", 50, "Don't care about me", 50);
auto tup_view = hera::tuple_view{tup};

// Predicate must return an integral constant of type bool.
auto only_ints = tup_view | hera::views::filter([](auto x) {
                        return std::is_same<decltype(x), int>{};
                    });

auto res = 0;
hera::for_each(only_ints, [&](auto filtered) { return res += filtered; });

std::cout << "result is: " << res << '\n';

// now we only care about the strings
auto only_str = tup_view | hera::views::filter([](auto str) {
                    return std::is_same<decltype(str), const char*>{};
                });

hera::for_each(only_str, [](auto str) { std::cout << str << '|'; });
std::cout << '\n';
```

The result of the filter must return an integral constant, otherwise the compiler cannot know the size of the range. As a result `filter_view` does not store anything other than the underlying view (not even the predicate), making filtering a zero overhead.

## Available views

- array
- tuple
- filter
- transform
- head
- iota
- move
- ref
- drop
- reorder
- zip
- enumerate

## Available algorithms

- find_if
- for_each
- unpack
- any_of
- all_of
- sort