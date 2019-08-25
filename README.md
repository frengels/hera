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
                        if constexpr (hera::same_as<decltype(x), int>)
                        {
                            return std::true_type{};
                        }
                        else
                        {
                            return std::false_type{};
                        }
                    });

auto res = 0;
hera::for_each(only_ints, [&](auto filtered) { return res += filtered; });

std::cout << "result is: " << res << '\n';

// now we only care about the strings
auto only_str = tup_view | hera::views::filter([](auto str) {
                    if constexpr (hera::same_as<decltype(str), const char*>)
                    {
                        return std::true_type{};
                    }
                    else
                    {
                        return std::false_type{};
                    }
                });

hera::for_each(only_str, [](auto str) { std::cout << str << '|'; });
std::cout << '\n';
```