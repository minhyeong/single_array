
#include "Samc.hpp"
//#include "Rank_Samc.hpp"
//#include "sdsl/rrr_vector.hpp"

//#define FILE_PATH "../corpus/Japan_Postal_Code.txt"
//#define FILE_PATH "../corpus/en.txt"
#define FILE_PATH "../corpus/usa.txt"
//#define FILE_PATH "../corpus/ja.txt"
//#define FILE_PATH "../corpus/dataset.txt"
//#define FILE_PATH "../corpus/enwiki-latest-all-titles-in-ns0"

int main() {
    std::ifstream ifs(FILE_PATH);
    std::vector<std::string> str_set;

    // Loading Corpus
    if (ifs.fail()) {
        std::cerr << "ERROR : Failed File Open" << std::endl;
        return -1;
    } else {
        std::cout << "Reading for [" << FILE_PATH << "]" << std::endl;
    }
    for (std::string s; getline(ifs, s);) {
        if (not s.empty()) {
            // s = s.substr(1, 5);
            str_set.push_back(s);
        }
    }

    // std::ofstream outputfile("../corpus/usa.txt");
    // for (auto v : str_set) {
    //    outputfile << v << "\n";
    //}
    // outputfile.close();

    std::sort(str_set.begin(), str_set.end());
    // str_set.resize(str_set.size()/15); // ****
    // make SAMC
    sim_ds::Samc<uint32_t> samc(str_set.begin(), str_set.end());

    ifs.seekg(0);
    int failed_num = 0;

    //  time measur
    clock_t start = clock();

    for (auto& v : str_set) {
        if (not samc.accept(v)) {
            failed_num++;
        }
    }

    clock_t end = clock();
    const double search_time =
        static_cast<double>(end - start) / CLOCKS_PER_SEC * 1000.0;

    if (failed_num != 0)
        std::cout << "failed_num : " << failed_num << std::endl;

    std::cout << "serch time / corpus : " << std::fixed << std::setprecision(8)
              << (search_time / str_set.size()) << " [ms/word]" << std::endl;

    return 0;
}
