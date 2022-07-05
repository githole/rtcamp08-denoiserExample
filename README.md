# rtcamp08-denoiserExample
レイトレ合宿8のデノイズ部門用サンプルコード。
`src/main.cpp`に実装してある`denoiser::denoise()`を置き換えることでデノイズ部門のレギュレーションを満たした実行ファイルを作ることが出来る。

# 実行方法
以下の様に実行する。この場合、`color.hdr albedo.hdr normal.hdr`が入力、`denoised.hdr`が出力である。この引数順はデノイズ部門のレギュレーションで定められたものである。
なお、各入力ファイルはassetsフォルダに入れてある。

`denoiser.exe color.hdr albedo.hdr normal.hdr denoised.hdr`
