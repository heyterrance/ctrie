#ifndef CTRIE_CTRIE_H_
#define CTRIE_CTRIE_H_

#include <type_traits>

namespace ctrie {

template<char... Letters>
struct Word { };

struct Found : std::true_type { };
struct NotFound : std::false_type { };

template<char C, typename... Tries>
struct Find;

template<char C, typename... Children>
struct Trie :
    std::integral_constant<char, C>
{
    static constexpr bool is_word = Find<'\0', Children...>::type::value;
};

template<char C>
struct Find<C>
{
    using type = NotFound;
};

template<char C, typename TrieT, typename... Tries>
struct Find<C, TrieT, Tries...> : Find<C, Tries...>
{ };

template<char C, typename... Cs, typename... Tries>
struct Find<C, Trie<C, Cs...>, Tries...>
{
    using type = Found;
};

template<typename Parent, typename Child>
struct Adopt;

template<char _, typename Child, typename... Children>
struct Adopt<Trie<_, Children...>, Child>
{
    using type = Trie<_, Child, Children...>;
};

template<char C, typename Trie>
struct Remove;

template<char C, char _, typename Child, typename... Children>
struct Remove<C, Trie<_, Child, Children...>> :
    Remove<C, Trie<_, Children..., Child>>
{ };

template<char C, char _, typename... Cs, typename... Children>
struct Remove<
    C,
    Trie<_, Trie<C, Cs...>, Children...>
>
{
    using parent = Trie<_, Children...>;
    using removed = Trie<C, Cs...>;
};

template<typename Word, typename...>
struct Insert;

template<char L, char... Ls, char _, typename... Children>
struct Insert<Word<L, Ls...>, Trie<_, Children...>> :
    Insert<
        Word<L, Ls...>,
        typename Find<L, Children...>::type,
        Trie<_, Children...>
    >
{ };

template<char L, char... Ls, char _, typename... Children>
struct Insert<Word<L, Ls...>, Found, Trie<_, Children...>>
{
private:
    using remove_result = Remove<L, Trie<_, Children...>>;
    using parent = typename remove_result::parent;
    using removed = typename remove_result::removed;

    using child = typename Insert<Word<Ls...>, removed>::type;

public:
    using type = typename Adopt<parent, child>::type;
};

template<typename TrieT>
struct Insert<Word<>, TrieT>
{
    using type = TrieT;
};

template<char L, char... Ls, char _, typename... Children>
struct Insert<Word<L, Ls...>, NotFound, Trie<_, Children...>> :
    Adopt<
        Trie<_, Children...>,
        typename Insert<Word<Ls...>, Trie<L>>::type
    >
{ };

template<typename TrieT, typename... Words>
struct Build
{
    using type = TrieT;
};

template<typename TrieT, typename W, typename... Words>
struct Build<TrieT, W, Words...> :
    Build<
        typename Insert<W, TrieT>::type,
        Words...
    >
{ };

template<typename... Words>
constexpr
typename Build<Trie<'\0'>, Words...>::type build(Words...) { return {}; }

template<typename TrieT, typename... Words>
constexpr
typename Build<TrieT, Words...>::type extend(TrieT, Words...) { return {}; }

template<typename T>
constexpr bool contains(T, const char*) { return false; }

template<typename Func, typename T>
constexpr bool step(T, char, Func) { return false; }

namespace detail {

template<typename... Children>
struct Pack {};

constexpr
bool contains(Pack<>, const char*) { return false; }

template<typename Child, typename... Children>
constexpr
bool contains(Pack<Child, Children...>, const char* word)
{
    return
        contains(Child{}, word) ||
        contains(Pack<Children...>{}, word);
}

template<typename Func, typename TrieT>
constexpr
bool try_step(TrieT trie, char c, Func func)
{
    if (TrieT::value == c) {
        func(trie);
        return true;
    }
    return false;
}

template<typename Func>
constexpr
bool try_step(Pack<>, char, Func)
{
    return false;
}

template<typename Func, typename Child, typename... Children>
constexpr
bool try_step(Pack<Child, Children...>, char c, Func func)
{
    return
        try_step(Child{}, c, func) ||
        try_step(Pack<Children...>{}, c, func);
}

} // namespace detail

template<char C, typename... Children>
constexpr
bool contains(Trie<C, Children...>, const char* word)
{
    return *word == C && detail::contains(detail::Pack<Children...>{}, ++word);
}

template<typename Child, typename... Children>
constexpr
bool contains(Trie<'\0', Child, Children...>, const char* word)
{
    // Root node.
    return
        *word != '\0' &&
        detail::contains(detail::Pack<Child, Children...>{}, word);
}

constexpr
bool contains(Trie<'\0'>, const char* word)
{
    // Leaf node.
    return *word == '\0';
}

template<char C, typename Func, typename Child, typename... Children>
constexpr
bool step(Trie<C, Child, Children...>, char c, Func func)
{
    return
        detail::try_step(Child{}, c, func) ||
        detail::try_step(detail::Pack<Children...>{}, c, func);
}

} // namespace ctrie

template<typename CharT, CharT... Letters>
constexpr
ctrie::Word<Letters..., '\0'> operator""_cword() { return {}; }

#endif
