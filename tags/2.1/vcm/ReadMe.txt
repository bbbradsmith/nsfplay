VCM -- Versatile Configuration Management Class Library for C++

コーディング規約(簡易)

[命名規則]

  クラス名は大文字で始める．
    例) Value, ObservableClient
    
  インタフェース名は大文字で始める．

  属性は 小文字で始め，二つ目以降の単語は大文字で始める．
  ただし，ポインタ変数はpで始め，参照はrで始める．
    例) textData, enableCommand, pTextBuffer, rStr

  操作は 小文字で始める．二つ目以降の単語は大文字で始める．
    例) myMethod1, myMethod2, printStatus

  定数はすべて大文字 区切り毎に _ を入れる
    例) TEXT_MODE, CURRENT_BUFFER

[呼び出し規約]

  ・メンバを参照する場合に，thisポインタを省略しない．
  






