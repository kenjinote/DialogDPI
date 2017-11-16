# DialogDPI
DPI 対応したダイアログの拡縮具合を取得して描画ピクセルに反映させるプログラム。

DLU で 66 x 61 の矩形は DPI が 100% のとき 99 x 99 ピクセルで表示される。
※ ダイアログのスタイルに DS_FIXEDSYS がついていて、フォントに "MS Shell Dlg" が設定されている場合。

||サイズ|
|:--:|:--:|
|DLU|66x61(DLU)|
|DPI=100%|99x99(ピクセル)|
|DPI=125%|116x122(ピクセル)|
|DPI=150%|149x145(ピクセル)|
|DPI=175%|165x175(ピクセル)|
|DPI=200%|198x191(ピクセル)|
|DPI=225%|215x221(ピクセル)|
|DPI=250%|248x252(ピクセル)|
|DPI=300%|297x297(ピクセル)|
|DPI=350%|330x343(ピクセル)|

DLU から ピクセル数を求める計算式は
(DLU) x (Dialog Base Unit) = ピクセル数
となる。

(Dialog Base Unit) はダイアログに設定されているフォントによって変わり、
コード中にある GetActualDialogBaseUnits 関数で求めることができます。
