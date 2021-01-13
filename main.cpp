
//#include "Samc.hpp"
#include "Rank_Samc.hpp"
//#include "sdsl/rrr_vector.hpp"

#define FILE_PATH "../corpus/title_100000.txt"
//#define FILE_PATH "../corpus/title_10000.txt"

//#define FILE_PATH "../corpus/en.txt"
//#define FILE_PATH "../corpus/test.txt"
//#define FILE_PATH "../corpus/ken.txt"
//#define FILE_PATH "../corpus/usa.txt"

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
            // s = s.substr(1, 7);
            str_set.push_back(s);
        }
    }

    /*std::vector<std::string> str_0000_9999;
    for (int i = 0; i <= 99999; i++) {
        if (i < 10)
            str_0000_9999.push_back("0000" + std::to_string(i));
        else if (i < 100)
            str_0000_9999.push_back("000" + std::to_string(i));
        else if (i < 1000)
            str_0000_9999.push_back("00" + std::to_string(i));
        else if (i < 10000)
            str_0000_9999.push_back("0" + std::to_string(i));
        else
            str_0000_9999.push_back(std::to_string(i));
    }
    str_set = str_0000_9999;*/

    /*std::ofstream outputfile("../corpus/00000_99999.txt");
    for (auto v : str_set) {
        outputfile << v << "\n";
    }
    outputfile.close();*/

    std::sort(str_set.begin(), str_set.end());
    // str_set.resize(str_set.size()/15); // ****
    // str_set.resize(100000);
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
              << (search_time / (str_set.size())) << " [ms/word]" << std::endl;

    return 0;
}
