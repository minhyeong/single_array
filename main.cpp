
//#include "Samc.hpp"
#include "Rank_Samc.hpp"
//#include "sdsl/rrr_vector.hpp"

#define FILE_PATH "../corpus/Japan_Postal_Code.txt"
//#define FILE_PATH "../corpus/en.txt"
//#define FILE_PATH "../corpus/test_postal_code.txt"

int main() {

	std::ifstream ifs(FILE_PATH);
	std::vector<std::string> str_set;

	// Loading Corpus
	if (ifs.fail()) {
		std::cerr << "ERROR : Failed File Open" << std::endl;
		return -1;
	}else {
		std::cout << "Reading for [" << FILE_PATH << "]" << std::endl;
	}
	for (std::string s; getline(ifs, s);) {
		if (not s.empty()) {
			str_set.push_back(s);
		}
	} 

	std::sort(str_set.begin(), str_set.end());
	// make SAMC
	sim_ds::Samc<uint32_t> samc(str_set.begin(), str_set.end());

	ifs.seekg(0);
	int failed_num = 0;


//  time measur
	clock_t start = clock();
	
	for (auto& v : str_set) {
		if (not samc.accept(v)) {
			//std::cout << "accept : " << samc.accept(v) << std::endl;
			failed_num++;
		}
	}

	clock_t end = clock();
	const double search_time = static_cast<double>(end - start) / CLOCKS_PER_SEC * 1000.0;

	if (failed_num != 0) {
		std::cout << "failed_num : " << failed_num << std::endl;
	}


	std::cout << "serch time / corpus : " << std::fixed << std::setprecision(8) 
		<< (search_time / str_set.size()) << " [ms/word]" << std::endl;

	return 0;
}
