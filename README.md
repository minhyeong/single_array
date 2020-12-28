
# Single Array

## SingleArrayの研究用

使用 Module
https://github.com/MatsuTaku/SimpleDataStructure.git

## 何かのメモ
- テンプレートの派生クラスから親クラスのメンバへのアクセス
    - 問題のメンバに"this->"をつけると問題解決
    - テンプレートを含む場合の名前解決が通常の場合と異なるかららしい
    ```
    this->write_vec(storage_, os);
    ```
- std::string_view の参照エラー
    - C++17以上であれば問題ないが, 参照エラー表示がでる