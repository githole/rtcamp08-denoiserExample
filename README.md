# rtcamp08-denoiserExample
レイトレ合宿8のデノイズ部門用サンプルコード。
`src/denoiser.cpp`に実装してある`denoiser::denoise()`を置き換えることでデノイズ部門のレギュレーションを満たした実行ファイルを作ることが出来る。

# 実行方法
以下の様に実行する。この場合、`color.hdr albedo.hdr normal.hdr`が入力、`denoised.hdr`が出力である。この引数順はデノイズ部門のレギュレーションで定められたものである。
なお、各入力ファイルはassetsフォルダに入れてある。

`denoiser.exe color.hdr albedo.hdr normal.hdr denoised.hdr`

# 入力画像について
本番で入力される画像も、基本的にはassetsフォルダに入れてあるサンプル画像と同様の設定でレンダリングされたものを使う。
ただし、本番ではサンプルとは異なるシーンを使用し、spp 設定も異なる可能性がある。

## color.hdr
モンテカルロノイズを含んだリニアHDR画像。1 spp。

## albedo.hdr
反射率が格納されたリニアHDR画像。

## normal.hdr
法線が格納されたリニアHDR画像。
画像上の値のレンジは[0.0, 1.0]となっている。各RGB値について、2倍して1を引いて[-1.0, 1.0]のレンジにマッピングすることでワールド空間における法線(x, y, z)を得ることが出来る。
