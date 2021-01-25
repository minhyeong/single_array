
//#include "Samc.hpp"
//#include "Samc2.hpp" // zip
#include "Rank_SAMC.hpp"
//#include "Rank_MSB.hpp" // # 無し
//#include "Rank_MSB_z.hpp"// zip
//#include "sdsl/rrr_vector.hpp"

// dataset > 比較用データセット群
//#define FILE_PATH "../dataset/title_100000.txt"
//#define FILE_PATH "../dataset/ja_3word_list.txt"
//#define FILE_PATH "../dataset/ja_word_list.txt"
//#define FILE_PATH "../dataset/en_3word_list.txt"
#define FILE_PATH "../dataset/en_word_list.txt"
//#define FILE_PATH "../dataset/1n-gram.txt"

int main() {
    std::ifstream ifs(FILE_PATH);
    std::vector<std::string> str_set, find_set;

    // Loading Corpus
    if (ifs.fail()) {
        std::cerr << "ERROR : Failed File Open" << std::endl;
        return -1;
    } else {
        std::cout << "Reading for [" << FILE_PATH << "]" << std::endl;
    }
    for (std::string s; getline(ifs, s);) {
        if (not s.empty()) {
            // s.resize(4);  // blugaria の末尾に何かついてるため
            find_set.push_back(s);
            // s = s.substr(1, 7);
            // s[s.size() - 1] |= (1 << 7);  // MSB
            // std::cout << " : " << s << std::endl;
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
