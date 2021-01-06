//
//  SuccinctBitVector.hpp
//  BitVector_test
//
//  Created by 松本拓真 on 2018/12/30.
//

#ifndef SuccinctBitVector_hpp
#define SuccinctBitVector_hpp

#include "sim_ds/BitVector.hpp"

namespace sim_ds {

template <bool UseSelect = true>
class SuccinctBitVector {
   private:
    BitVector bits_;
    /* Contains large/small block as follows:
     * || large tip - 64bits - || small tip reversal - 9bits - | ... * 8||   --
     * 127bits per block
     */
    std::vector<uint64_t> basic_block_;
    // Available if only UseSelect
    std::vector<uint32_t> select_tips_;

   public:
    using char_type = uint8_t;                                  // default 8
    static constexpr size_t kAlphabetSize = 0x100;              // 修正用
    static constexpr char_type kEmptyChar = kAlphabetSize - 1;  // 修正用

    SuccinctBitVector() = default;

    explicit SuccinctBitVector(BitVector&& bits);

    SuccinctBitVector(const BitVector& bits)
        : SuccinctBitVector(BitVector(bits)) {}

    SuccinctBitVector(std::initializer_list<bool> bits)
        : SuccinctBitVector(BitVector(bits)) {}

    SuccinctBitVector(std::istream& is) { Read(is); }

    bool operator[](size_t index) const { return bits_[index]; }

    size_t rank_1(const size_t index) const;

    size_t rank(size_t index) const { return rank_1(index); }

    size_t rank_0(size_t index) const { return index - rank_1(index); }

    size_t select(size_t index) const;

    size_t size() const { return bits_.size(); }

    const auto* data() const { return bits_.data(); }

    size_t num_blocks() const { return basic_block_.size() / 2; }

    size_t size_in_bytes() const {
        auto size = bits_.size_in_bytes();
        size += size_vec(basic_block_);
        if constexpr (UseSelect) size += size_vec(select_tips_);
        return size;
    }

    void Read(std::istream& is) {
        bits_.Read(is);
        read_vec(is, basic_block_);
        if constexpr (UseSelect) read_vec(is, select_tips_);
    }

    void Write(std::ostream& os) const {
        bits_.Write(os);
        write_vec(basic_block_, os);
        if constexpr (UseSelect) write_vec(select_tips_, os);
    }
};

template <bool UseSelect>
SuccinctBitVector<UseSelect>::SuccinctBitVector(BitVector&& bits)
    : bits_(std::forward<BitVector>(bits)) {
    if (bits_.empty()) {
        basic_block_.assign(2, 0);
        return;
    }
    // https://takeda25.hatenablog.jp/entry/20140201/1391250137
    size_t basic_block_size = bits_.size() / 512 + 1;
    basic_block_.resize(basic_block_size * 2);

    const auto* data = bits_.data();        // ビット列に変換？
    size_t sum = bit_util::popcnt(*data);   // 1 ビットの出現数
    uint64_t sum_word = 0;                  //
    basic_block_[0] = basic_block_[1] = 0;  // basic_block_[1] が0の理由は?
    size_t i = 0;
    for (i = 1; i < bits_.size() / 64; i++) {  // 大ブロックごとの記録
        if (i % 8 == 0) {
            size_t j = i / 8 * 2;
            basic_block_[j - 1] = sum_word;
            basic_block_[j] = basic_block_[j - 2] + sum;
            sum_word = sum = 0;
        } else {
            sum_word |= sum << (63 - 9 * (i % 8));  // ??
        }
        sum += bit_util::popcnt(*(++data));  // なぜ同じものを？
    }
    if (i % 8 != 0) {  // 小ブロックごとの記録
        size_t j = i / 8 * 2;
        sum_word |= sum << (63 - 9 * (i % 8));
        basic_block_[j + 1] = sum_word;
    } else {  // 小ブロック全てのビットパターンに対する記録?
        size_t j = i / 8 * 2;
        basic_block_[j - 1] = sum_word;
        basic_block_[j] = basic_block_[j - 2] + sum;
        basic_block_[j + 1] = 0;
    }

    if constexpr (UseSelect) {
        size_t sum_threshold = 512;
        select_tips_.push_back(0);
        for (size_t j = 1; j < basic_block_size; j++) {
            if (sum_threshold <= basic_block_[j * 2]) {
                select_tips_.push_back(j - 1);
                sum_threshold += 512;
            }
        }
        select_tips_.push_back(basic_block_size - 1);
    }
}

template <bool UseSelect>
size_t SuccinctBitVector<UseSelect>::rank_1(const size_t index) const {
    size_t block_index = index / 512 * 2;
    // std::cout << "test : " << "" <<std::endl;
    return (basic_block_[block_index] +
            ((basic_block_[block_index + 1] >> (63 - 9 * ((index / 64) % 8))) &
             bit_util::width_mask<9>)  // 下位 9 bitを抽出
            +bit_util::cnt(bits_.data()[index / 64],
                           index % 64));  // ビットパターンを探してる？
}

template <>
size_t SuccinctBitVector<false>::select([[maybe_unused]] size_t index) const {
    throw "select(size_t) is not supported. You must use SuccinctBitVector<true>.";
}

template <>
size_t SuccinctBitVector<true>::select(size_t index) const {
    size_t left = 0, right = num_blocks();
    size_t i = index;

    if (select_tips_.size() != 0) {
        auto select_tip_index = i / 512;
        left = select_tips_[select_tip_index];
        right = select_tips_[select_tip_index + 1] + 1;
    }
    while (left + 1 < right) {
        const auto center = (left + right) / 2;
        if (i < basic_block_[center * 2]) {
            right = center;
        } else {
            left = center;
        }
    }
    i += 1;  // for i+1 th
    i -= basic_block_[left * 2];

    auto second_tip = [&](size_t idx) {
        return (basic_block_[idx / 8 * 2 + 1] >> (63 - 9 * (idx % 8))) &
               bit_util::width_mask<9>;
    };
    size_t offset = 1;
    size_t second_tip_size = size() / 64 + 1;
    for (size_t index = left * 8 + offset;
         offset < 8 && index < second_tip_size && i > second_tip(index);
         offset++, index++)
        continue;
    i -= second_tip(left * 8 + --offset);

    size_t ret = left * 512 + offset * 64;
    auto bits = bits_.data()[ret / 64];

    return ret + bit_util::sel(bits, i) - 1;  // 0 index
}

}  // namespace sim_ds

#endif /* SuccinctBitVector_hpp */
