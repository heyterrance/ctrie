# Compile-time Trie
A [prefix tree](https://en.wikipedia.org/wiki/Trie) implemented using template metaprogramming.

## Usage

```cpp
// Construct a trie.
static constexpr auto trie = ctrie::build(
    "try"_cword,
    "tree"_cword,
    "trie"_cword,
    "a"_cword,
    "ant"_cword);

// Check if a word is contained in the trie.
static_assert(ctrie::contains(trie, "try"), "");
static_assert(!ctrie::contains(trie, "tre"), "");

// Traversal by characters
bool found_t =
    ctrie::step(trie, 't', [](auto child) {
        ctrie::step(child, 'r', [](auto child2) {
            assert(!child2.is_word);

            ctrie::step(child2, 'y', [](auto child3) {
                assert(child3.is_word);
            });
        });
    });
```

## Dependencies
* C++14
