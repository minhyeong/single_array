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
    ```
    - 通らない場合 CMakeLists.txt の28行目を下記に変更
        ```
        target_link_libraries(main PRIVATE sim_ds) /// sdsl を使わないため
        ```

## 暫定出力結果
- データセット
    - [enwiki-latest-all-titles-in-ns0.gz](https://dumps.wikimedia.org/enwiki/latest/)
        - ソート後 100,000 文字分使用
            - 大きすぎるとメモリフロー起こす

### SAMC
```
CHECK : 778327786 [Byte]
CODE : 225288 [Byte]
Array Size : 778553074 [Byte]
serch time / corpus : 0.00168506 [ms/word]
```

### RANK
```
storage_ : 1283016 [B]
code_table_ : 225288 [B]
Rank_Dic & bit : 209748 [B]
All Size : 1718052 [B]
serch time / corpus : 未計測
```