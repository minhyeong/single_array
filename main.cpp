
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

	clock_t start = clock(); // ���s�J�n
	// �R�[�p�X�̎擾
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
	clock_t end = clock(); // ���s�I��
	const double exe_time = static_cast<double>(end - start) / CLOCKS_PER_SEC * 1000.0; // ���s���Ԍv������
	ifs.seekg(0);
	int failed_num = 0; // 1��ɑ΂��錟�����Ԃ̑���

	start = clock();
	for (auto& v : str_set) {
		if (not samc.accept(v)) {
			failed_num++;
		}
	}
	end = clock();
	const double seartch_time = static_cast<double>(end - start) / CLOCKS_PER_SEC * 1000.0;


	std::cout << "���s���� : " << exe_time << std::endl;
	std::cout << "�S�Ă̌�������" << seartch_time << std::endl;
	std::cout << "1�񓖂���̕��ό������� : " << seartch_time / str_set.size() << std::endl;
}

