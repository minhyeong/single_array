
#include "Samc.hpp"
//#include "Samc2.hpp"
//#include "Samc3.hpp"
//#include "Rank_SAMC.hpp"
//#include "Rank_MSB.hpp"
//#include "Rank_MSB_z.hpp"

int main(int argc, char* argv[]) {
    std::ifstream ifs(argv[1]);
    std::vector<std::string> str_set, find_set;

    // Loading Corpus
    if (ifs.fail()) {
        std::cerr << "ERROR : Failed File Open" << std::endl;
        return -1;
    }
    for (std::string s; getline(ifs, s);) {
        if (not s.empty()) {
            s.resize(s.length() - 1);  // 行末に改行等ついてるとき削除すること
            find_set.push_back(s);
            // s = s.substr(1, 7); // 終端記号の削減を行うとき必要
            str_set.push_back(s);
        }
    }

    std::sort(str_set.begin(), str_set.end());
    sim_ds::Samc<uint32_t> samc(str_set.begin(), str_set.end());

    ifs.seekg(0);
    int failed_num = 0;

    clock_t start = clock();
    for (auto& v : find_set) {
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
