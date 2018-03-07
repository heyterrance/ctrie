#include <iostream>
#include <cassert>

#include "ctrie.h"

static constexpr auto trie = ctrie::build(
        "app"_cword,
        "apple"_cword,
        "art"_cword);

static_assert(ctrie::contains(trie, "app"), "");
static_assert(ctrie::contains(trie, "apple"), "");
static_assert(!ctrie::contains(trie, "ap"), "");
static_assert(!ctrie::contains(trie, "appl"), "");
static_assert(ctrie::contains(trie, "art"), "");
static_assert(!ctrie::contains(trie, "a"), "");

template<typename TrieT>
bool traverse(TrieT t, const char* word)
{
    if (t.is_word)
        std::cout << "(found a word!)" << std::endl;

    return ctrie::step(t, *word, [word](auto child) {
        std::cout << child.value << '\n';
        traverse(child, word + 1);
    });
}

int main()
{
    traverse(trie, "apple");
    return 0;
}

static constexpr auto trie2 = ctrie::build(
    "try"_cword,
    "tree"_cword,
    "trie2"_cword,
    "a"_cword,
    "ant"_cword);

// Check if a word is contained in the trie2.
static_assert(ctrie::contains(trie2, "try"), "");
static_assert(!ctrie::contains(trie2, "tre"), "");

// Traversal by characters
bool found_t =
    ctrie::step(trie2, 't', [](auto child) {
        ctrie::step(child, 'r', [](auto child2) {
            assert(!child2.is_word);

            ctrie::step(child2, 'y', [](auto child3) {
                assert(child3.is_word);
            });
        });
    });

