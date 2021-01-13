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

||SAMC|RANK|LOUDS|
|:-|:-|:-|:-|
|CHECK[B]|778,327,786|1,283,016||
|Code[B]|225,288|225,288|||
|FID_rank[B]||121,623,788||
|All_Size|778,553,074|123,132,092|1,793,028|
|検索時間[ms/word]|0.001,810,21|0.002,628,26|0.021,866,1|

## 日本郵便番号
||SAMC|RANK|LOUDS|
|:-|:-|:-|:-|
|CHECK[B]|4,914,166|268,953||
|Code[B]|8,200|8,200|||
|FID_rank[B]||769,040||
|All_Size|4,922,366|1,046,193|225,337|
|検索時間[ms/word]|0.000,388,66|0.000,754,61|0.002,976,88|

## USA zip_code
||SAMC|RANK|LOUDS|
|:-|:-|:-|:-|
|CHECK[B]|206,833|91,252||
|Code[B]|6,152|6,152|||
|FID_rank[B]||32,744||
|All_Size|212,985|130,148|75,133|
|検索時間[ms/word]|0.000,227,72|0.000,534,28|0.002,006,81|