
/*

rank rebuild
imamura

*/

#ifndef Samc_hpp
#define Samc_hpp

#include <iterator>

#include "sim_ds/basic.hpp"
#include "sim_ds/string_util/graph_util.hpp"
#include "sim_ds/bit_util.hpp"
#include "sim_ds/BitVector.hpp"
#include "sim_ds/SuccinctBitVector.hpp"
#include "sim_ds/log.hpp"



template <typename CodeType>
class Samc : _SamcImpl<CodeType> {
public:
    using code_type = CodeType;
    using _base = _SamcImpl<code_type>;
    template<typename T, typename S> // 
    using input_trie = graph_util::Trie<T, S>;

    static constexpr uint8_t kLeafChar = '\0'; // 書き換え不可の定数

public:
    Samc() = default; // コピー禁止? etc

    template <Typename StrIter>
    
    // explicit コピー初期化禁止
    // こっちを使う
    explicit Samc(StrIter begin, StrIter end) : _base(begin, end) {} 

    template <typename T, typename S>
    // deprecated 対象となる機能が非推奨であることを示す
    // コンパイル時に警告としてでる
    [[deprecated("Maybe consume much memory. You should construct from string-iterator like (begin end)")]]
    explicit Samc(const input_trie<T, S>& trie) : _base(trie) {}

    bool accept(std::string_view key) const; // 検索キーの取得
    size_t size_in_bytes() const {return _base::size_in_bytes();}

    void Write(std::ostream& os) const { // ??
        _base::Write(os);
    }
    void Read(std::istream& is) { // ??
      _base::Read(is);
    }

private:
    bool in_range(size_t index, size_t depth) const {
        assert(depth > 0); // 式は真である
        // ??
        return _base::head(depth) <= index and index < _base::head(depth+1);
    }

    bool empty(size_t index) const {return _base::check(index) =- _base::kEmptyChar;} // ??
};


template <typename CodeType>
bool
Samc<CodeTpe>::accept(std::string_view key) const {
    
}




#endif /* Samc_hpp */