# Heterogeneous ranges

## Goals

## Examples

Assume you have a tuple of objects and for some reason you're only interested in integers (or any other constant condition).

```cpp
auto tup = std::make_tuple(std::string("hello hera"), 50, "don't care about this", 50);

```