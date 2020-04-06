
#include "class.hpp"
#include <windows.h>
//#include "sdsl/bit_vectors.hpp"
//#include "sdsl/suffix_arrays.hpp"

//using namespace sdsl;

#define FILE_PATH "../../corpus/Japan_Postal_Code.txt"
//#define FILE_PATH "../../../corpus/test_postal_code.txt"

int main() {

	std::ifstream ifs(FILE_PATH);
	std::vector<std::string> str_set;

	clock_t start = clock(); // 実行開始
	// コーパスの取得
	if (ifs.fail()) {
		std::cerr << "ERROR : Failed File Open" << std::endl;
		return -1;
	}else {
		std::cout << "Reading for [" << FILE_PATH << "]" << std::endl;
	}

	Samc<size_t>::input_trie<char, uint32_t> trie;
	
	for (std::string s; getline(ifs, s);) {
		if (not s.empty()) {
			str_set.push_back(s);
		}
		trie.insert(s, 1);
	}
	Samc<uint32_t> samc(trie);
	clock_t end = clock(); // 実行終了
	const double exe_time = static_cast<double>(end - start) / CLOCKS_PER_SEC * 1000.0; // 実行時間計測結果
	ifs.seekg(0);
	int failed_num = 0; // 1回に対する検索時間の測定

	start = clock();
	for (auto& v : str_set) {
		if (not samc.accept(v)) {
			failed_num++;
		}
	}
	end = clock();
	const double seartch_time = static_cast<double>(end - start) / CLOCKS_PER_SEC * 1000.0;


	std::cout << "実行時間 : " << exe_time << std::endl;
	std::cout << "全ての検索時間" << seartch_time << std::endl;
	std::cout << "1回当たりの平均検索時間 : " << seartch_time / str_set.size() << std::endl;
}

