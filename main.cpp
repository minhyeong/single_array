
#include "class.hpp"

#define FILE_PATH "../corpus/Japan_Postal_Code.txt"

int main() {

	std::ifstream ifs(FILE_PATH);
	std::vector<std::string> str_set;

	clock_t start = clock();
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
	clock_t end = clock();
	const double exe_time = static_cast<double>(end - start) / CLOCKS_PER_SEC * 1000.0; // ŔsÔvŞĘ
	ifs.seekg(0);
	int failed_num = 0;

	start = clock();
	for (auto& v : str_set) {
		if (not samc.accept(v)) {
			failed_num++;
		}
	}
	end = clock();
	const double seartch_time = static_cast<double>(end - start) / CLOCKS_PER_SEC * 1000.0;


	std::cout << "exe time : " << exe_time << std::endl;
	std::cout << "serch time" << seartch_time << std::endl;
	std::cout << "serch time / corpus : " << seartch_time / str_set.size() << std::endl;
}

