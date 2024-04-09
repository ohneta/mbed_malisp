# mbed_malisp
## about this
mbed_malisp は mbed環境向けの lispです。

mbed_malisp is lisp for mbed(arm microprocessor development board).

オリジナルのmalispは 1980年, 1985年に Marc Adler氏によって書かれました。

Original malisp written by Marc Adler at 1980, 1985.

2016年に Mbed2 LPC1768用のC++コードに移植されました。

Mbed2 C++ (for LPC1768) ported by ohneta (Takehisa Oneta) at 2016.


オリジナルのライセンスとreadmeファイルは malisptxt に記述されています。

original lisence/readme file is "malisptxt".

## malisp
malispは非常にコンパクトなlisp環境です。

malisp is a very compact lisp environment.

そのコンパクトさゆえ、ガベージコレクションなどlispとして重要な機能は実装されていませんが、コンピュータ言語として構文解釈のコードも含まれており、lispがインタプリタとしてどのように動作するかを知る手がかりととしてはよくできていると思います。

Because of its compactness, it does not implement important functions of Lisp such as garbage collection, but it also includes code for syntax interpretation as a computer language, and can be used as a clue to how Lisp operates as an interpreter. I think it's well done.
