# SingleArrayの研究用

## 環境
- gcc 9.3.0
- boost 107500
    ```
    git submodule update --init
    mkdir build
    cd build
    cmake ..
    make
    ./main ファイルのPATH
    ```
    - 通らない場合 CMakeLists.txt の28行目を下記に変更
        ```
        target_link_libraries(main PRIVATE sim_ds) /// sdsl を使わないため
        ```

## ファイルの説明
- Samc.hpp
    - 従来手法
- Samc2.hpp
    - データセットが固定長の場合に用いる
- Samc3.hpp
    - 終端記号の削減を行った場合
        ```
        main.cpp 22行目
        s = s.substr(1, 7); のコメント分を解除
        ```
- Rank_SAMC.hpp
    - 従来手法に完備辞書を入れたもの
- Rank_MSB.hpp
    - 従来手法から終端記号を削減し、完備辞書を入れたもの
        ```
        main.cpp 22行目
        s = s.substr(1, 7); のコメント分を解除
        ```
- Rank_MSB_z.hpp
    - データセットが固定長の場合に用いるRank_SAMC