
# Single Array

## SingleArrayの研究用

使用 Module
https://github.com/MatsuTaku/SimpleDataStructure.git

### 13/04/2020
- テンプレートの派生クラスから親クラスのメンバへのアクセス
    - 問題のメンバに"this->"をつけると問題解決
    - テンプレートを含む場合の名前解決が通常の場合と異なるかららしい
    ```
    this->write_vec(storage_, os);
    ```

### 04/20
- SimpleDataStructure
    |Japan_Postal_Code.txt [Byte]|978,620|
    |:-|-:|
    |storage_ [Byte]|7,942|
    |serch time [ms]|0.000,233,00|