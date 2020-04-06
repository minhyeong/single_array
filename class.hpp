
#ifndef _test_
#define _test_

#include <cstdio>
#include <iostream>
#include <fstream>
#include <set>
#include <string>
#include "sim_ds/graph_util.hpp"
#include "sim_ds/BitVector.hpp"
#include "sim_ds/SuccinctBitVector.hpp"

// SAMC (Single Array with Multi Code)
// reference:
//     M. Fuketa, H. Kitagawa, T. Ogawa, K. Morita, J. Aoe. Compression of double array structure for fixed length keywords. Information Processing and Management Vol 50 p796-806. 2014.
//

// MARK: - Samc

template <typename ValueType>
class _SamcImpl {
public:
	using value_type = ValueType;
	using position_type = long long;
	static constexpr size_t kAlphabetSize = 0x100;
	static constexpr uint8_t kEmptyChar = 0xFF;

protected:
	std::vector<bool> exist_flag_bits_; // ローカルに移動
	sim_ds::SuccinctBitVector<false> sbv_;
	std::vector<uint8_t> storage_;
	using code_type = std::array<value_type, kAlphabetSize>;
	std::vector<code_type> code_table_;
	std::vector<value_type> max_;

public:
	_SamcImpl() = default;

	uint8_t check(size_t index) const {

		//return storage_[index];
		if (sbv_[index]) {
			int n = sbv_.rank(index);
			return storage_[n];
		}
		else {
			return kEmptyChar;
		}
	}

	//uint8_t check(size_t index) const { return storage_[index]; }

	size_t code(size_t depth, uint8_t c) const { return code_table_[depth][c]; }

	size_t max(size_t depth) const { return max_[depth]; }

	size_t size_in_bytes() const {
		size_t size = sim_ds::size_vec(storage_);
		size += sim_ds::size_vec(code_table_);
		size += sim_ds::size_vec(max_);
		return size;
	}

	void Write(std::ostream& os) const {
		write_vec(storage_, os);
		write_vec(code_table_, os);
		write_vec(max_, os);
	}

	void Read(std::istream& is) {
		read_vec(is, storage_);
		read_vec(is, code_table_);
		read_vec(is, max_);
	}

protected:
	template <typename T, typename S>
	_SamcImpl(const sim_ds::graph_util::Trie<T, S>& trie);

private:
	using mask_type = uint64_t;
	static constexpr size_t kMaskWidth = 64;

	position_type y_check_(const std::vector<value_type>& indices, const sim_ds::BitVector& empties) const;

};

//↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓

// 編集中
template <typename ValueType>
template <typename T, typename S>
_SamcImpl<ValueType>::_SamcImpl(const sim_ds::graph_util::Trie<T, S>& trie) {
	std::vector<size_t> node_indexes = { sim_ds::graph_util::kRootIndex };
	//std::vector<bool> bit_strings;

	storage_.emplace_back('^');
	max_.emplace_back(0);
	position_type max_index = 0;

	while (max_index >= 0) {
#ifndef NDEBUG // 文字の格納階層について？
		std::cerr << "depth: " << max_.size()
			<< ", block_height: " << max_index << std::endl;
#endif
		std::array<std::vector<value_type>, kAlphabetSize> indices_list; // kAlphabetSize = 0x100
		size_t height = max_index + 1;
		for (size_t i = 0; i < height; i++) {
			auto index = storage_.size() - height + i;
			
			if (storage_[index] == kEmptyChar) { // kEmptyChar = 0xFF
				exist_flag_bits_.push_back(false);
				continue;
			}
			else {
				exist_flag_bits_.push_back(true);
			}
			trie.node(node_indexes[index]).for_each_edge([&](uint8_t c, auto e) {
				indices_list[c].push_back(i);
			});

		}

#ifndef NDEBUG
		std::cerr << "ycheck for each char..." << std::endl;;
#endif
		sim_ds::BitVector empties;
		auto old_size = storage_.size();
		max_index = -1;
		code_table_.emplace_back();
		for (size_t i = 0; i < kAlphabetSize; i++) {
			auto& indices = indices_list[i];
			if (indices.empty()) {
				code_table_.back()[i] = 0;
				continue;
			}
#ifndef NDEBUG 
			std::cerr << i << ':' << uint8_t(i) << ", indices: " << indices.size() << std::endl;
#endif
			empties.resize(max_index + 1 + height, true);
			auto y_front = y_check_(indices, empties);
			max_index = std::max(max_index, y_front + position_type(indices.back()));
			assert(height + y_front <= std::numeric_limits<value_type>::max());
			code_table_.back()[i] = height + y_front;
			storage_.resize(old_size + max_index + 1, kEmptyChar);
			node_indexes.resize(old_size + max_index + 1);

			for (auto id : indices) {
				auto index = y_front + id;
				auto abs_index = old_size + index;
				assert(storage_[abs_index] == kEmptyChar and
					empties[index]);
				storage_[abs_index] = i;
				empties[index] = false;
				auto parent_index = abs_index - (height + y_front);
				auto parent_node = trie.node(node_indexes[parent_index]);
				node_indexes[abs_index] = parent_node.target(i);
			}
		}
		if (max_index == -1) {
			code_table_.erase(code_table_.end() - 1);
			break;
		}
		max_.push_back(storage_.size() - 1);
#ifndef NDEBUG
		auto used = sim_ds::SuccinctBitVector<false>(empties).rank_0(empties.size());
		double per_used = double(used) / empties.size() * 100;
		std::cerr << "used: " << std::fixed << std::setprecision(2) << " %" << per_used << std::endl << std::endl;
#endif
	}	
	int bit_counter = 0; // bit の位置をメモ
	std::vector<int> bit_number_list_;
	std::vector<uint8_t> temp_storage;
	for (auto v : storage_) {
		bit_counter++;
		if (v != kEmptyChar) {
			temp_storage.push_back(v);
			bit_number_list_.push_back(bit_counter);
		}
	}
	storage_ = temp_storage; // 上書き
	temp_storage.clear();

	sbv_ = sim_ds::SuccinctBitVector<false>(std::move(exist_flag_bits_)); // 簡潔データ構造

	int num_element_exist = sbv_.rank(exist_flag_bits_.size());
	
	std::cout << "数 0 / 1 : " << sbv_.size() - storage_.size() <<" / "<< storage_.size() << std::endl;
	std::cout << "exist_flag_bits_ memory [byte] : " << sbv_.size_in_bytes() << std::endl;
	std::cout << "array memory [byte] : " << storage_.size() * sizeof(uint8_t) << std::endl;
	std::cout << "code_table memory [byte] : " << code_table_.size() * sizeof(code_type) << std::endl;
	std::cout << "sum memory [byte] : " << sbv_.size_in_bytes() + storage_.size() * sizeof(uint8_t) + code_table_.size() * sizeof(code_type) << std::endl;
	
//	std::cout << "" << std::endl;
	std::cout << "bit memorize list : " << bit_number_list_.size() * sizeof(int32_t) << std::endl; // ビット位置保存リスト

	// 11/25

	

	



}
//↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑


/* 上のコードの原型 */
//↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓
/*
template <typename ValueType>
template <typename T, typename S>
_SamcImpl<ValueType>::_SamcImpl(const sim_ds::graph_util::Trie<T, S>& trie) {
	std::vector<size_t> node_indexes = { sim_ds::graph_util::kRootIndex };
	storage_.emplace_back('^'); // root
	max_.emplace_back(0);
	position_type max_index = 0;
	while (max_index >= 0) {
#ifndef NDEBUG
		std::cerr << "depth: " << max_.size()
			<< ", block_height: " << max_index << std::endl;
#endif
		std::array<std::vector<value_type>, kAlphabetSize> indices_list;
		size_t height = max_index + 1;
		for (size_t i = 0; i < height; i++) {
			auto index = storage_.size() - height + i;
			if (storage_[index] == kEmptyChar) // kEmptyChar = 0xFF;
				continue;
			trie.node(node_indexes[index]).for_each_edge([&](uint8_t c, auto e) {
				indices_list[c].push_back(i);
				});
		}
#ifndef NDEBUG
		std::cerr << "ycheck for each char..." << std::endl;;
#endif
		 sim_ds::BitVector empties;
		auto old_size = storage_.size();
		max_index = -1;
		code_table_.emplace_back();
		for (size_t i = 0; i < kAlphabetSize; i++) {
			auto& indices = indices_list[i];
			if (indices.empty()) {
				code_table_.back()[i] = 0;
				continue;
			}
#ifndef NDEBUG
			std::cerr << i << ':' << uint8_t(i) << ", indices: " << indices.size() << std::endl;
#endif
			empties.resize(max_index + 1 + height, true);
			auto y_front = y_check_(indices, empties);
			max_index = std::max(max_index, y_front + position_type(indices.back()));
			assert(height + y_front <= std::numeric_limits<value_type>::max());
			code_table_.back()[i] = height + y_front;
			storage_.resize(old_size + max_index + 1, kEmptyChar);
			node_indexes.resize(old_size + max_index + 1);
			for (auto id : indices) {
				auto index = y_front + id;
				auto abs_index = old_size + index;
				assert(storage_[abs_index] == kEmptyChar and
					empties[index]);
				storage_[abs_index] = i;
				empties[index] = false;
				auto parent_index = abs_index - (height + y_front);
				auto parent_node = trie.node(node_indexes[parent_index]);
				node_indexes[abs_index] = parent_node.target(i);
			}
		}
		if (max_index == -1) {
			code_table_.erase(code_table_.end() - 1);
			break;
		}
		max_.push_back(storage_.size() - 1);
#ifndef NDEBUG
		auto used = sim_ds::SuccinctBitVector<false>(empties).rank_0(empties.size());
		double per_used = double(used) / empties.size() * 100;
		std::cerr << "used: " << std::fixed << std::setprecision(2) << " %" << per_used << std::endl << std::endl;
#endif
	}

	std::cout << "" << std::endl;
}
*/
//↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑


template <typename ValueType>
typename _SamcImpl<ValueType>::position_type
_SamcImpl<ValueType>::y_check_(const std::vector<value_type>& indices, const sim_ds::BitVector& empties) const {
	const auto word_size = (empties.size() - 1) / kMaskWidth + 1;
	auto field_bits = [&](size_t i) {
		return i < word_size ? empties.data()[i] : 0;
	};
	assert(position_type(empties.size()) + indices.front() - indices.back() >= 0);
	auto field_size = (empties.size() + indices.front() - indices.back()) / kMaskWidth + 1;
	position_type heads = indices.front();
	for (size_t i = 0; i < field_size; i++) {
		mask_type candidates = -1ull;
		for (auto id : indices) {
			auto p = (id - heads) / kMaskWidth;
			auto insets = (id - heads) % kMaskWidth;
			if (insets == 0) {
				candidates &= field_bits(p + i);
			}
			else {
				candidates &= (field_bits(p + i) >> insets) | (field_bits(p + i + 1) << (kMaskWidth - insets));
			}
			if (candidates == 0)
				break;
		}
		if (candidates)
			return position_type(kMaskWidth) * i + sim_ds::bit_util::ctz(candidates) - heads;
	}
	return empties.size() + indices.front() - indices.back(); // ERROR
}



template <typename ValueType>
class Samc : _SamcImpl<ValueType> {
public:
	using value_type = ValueType;
	using _base = _SamcImpl<value_type>;
	template <typename T, typename S>
	using input_trie = sim_ds::graph_util::Trie<T, S>;

	static constexpr uint8_t kLeafChar = sim_ds::graph_util::kLeafChar;

public:
	Samc() = default;

	template <typename T, typename S>
	Samc(const input_trie<T, S>& trie) : _base(trie) {}

	bool accept(std::string_view key) const;

	size_t size_in_bytes() const { return _base::size_in_bytes(); }

	void Write(std::ostream& os) const {
		_base::Write(os);
	}

	void Read(std::istream& is) {
		_base::Read(is);
	}

private:
	bool in_range(size_t index, size_t depth) const {
		assert(depth > 0);
		return index > _base::max(depth - 1) and index <= _base::max(depth);
	}

	bool empty(size_t index) const { return _base::check(index) == _base::kEmptyChar; }

};


// 編集中
template <typename ValueType>
bool
Samc<ValueType>::accept(std::string_view key) const {
	size_t node = 0;
	size_t depth = 0;
	for (; depth < key.size(); depth++) {
		uint8_t c = key[depth];
		auto target = node + _base::code(depth, c);
		if (not in_range(target, depth + 1) or
			_base::check(target) != c) {
			return false;
		}
		node = target;
	}
	auto terminal = node + _base::code(depth, kLeafChar);
	return (in_range(terminal, depth + 1) and
		_base::check(terminal) == kLeafChar);
}
#endif // #endif

/*
template <typename ValueType>
bool
Samc<ValueType>::accept(std::string_view key) const {
	size_t node = 0;
	size_t depth = 0;
	for (; depth < key.size(); depth++) {
		uint8_t c = key[depth];
		auto target = node + _base::code(depth, c);
		if (not in_range(target, depth + 1) or
			_base::check(target) != c) {
			return false;
		}
		node = target;
	}
	auto terminal = node + _base::code(depth, kLeafChar);
	return (in_range(terminal, depth + 1) and
		_base::check(terminal) == kLeafChar);
}
#endif // #endif
*/