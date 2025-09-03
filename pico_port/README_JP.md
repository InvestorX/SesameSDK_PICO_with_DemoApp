# Raspberry Pi Pico W 向け SesameSDK

これは ESP32 SesameSDK を Raspberry Pi Pico W に移植したバージョンで、Sesame5 スマートロックとの BLE 通信を可能にします。

## 必要なもの

- Raspberry Pi Pico W (Bluetooth 機能に必要)
- Pico SDK のインストールと設定
- CMake 3.13 以降

## ビルド方法

1. Pico SDK 環境の設定:
```bash
export PICO_SDK_PATH=/path/to/pico-sdk
```

2. ビルドディレクトリを作成してプロジェクトをビルド:
```bash
mkdir build
cd build
cmake ..
make
```

3. 生成された `SesameSDK_Pico.uf2` ファイルを Pico W に書き込み

## 機能

- **自動デバイス探索**: 近くの Sesame5 スマートロックを自動的に探索・接続
- **自動ロック機能**: Sesame5 が解錠位置に達したとき、Pico W が自動的にロック指令を送信
- **BLE 通信**: BTstack を使用した安定した Bluetooth Low Energy 通信

## ハードウェア設定

1. Raspberry Pi Pico W を USB でコンピュータに接続
2. ブートローダーモード時にデバイスは USB ストレージデバイスとして表示されます
3. `.uf2` ファイルをデバイスにコピー
4. Pico W が自動的に再起動して Sesame デバイスのスキャンを開始

## ESP32 版との違い

- **ビルドシステム**: ESP-IDF の代わりに Pico SDK CMake を使用
- **BLE スタック**: NimBLE の代わりに BTstack を使用
- **ログ出力**: ESP_LOG マクロの代わりに printf を使用
- **ハードウェア**: ESP32-C3 の代わりに Pico W をターゲット

## コード構造

```
src/
├── main.c              # メインアプリケーションエントリーポイント
├── ble_central.c       # BTstack を使用した BLE セントラル実装
├── ble_central.h       # BLE インターフェース定義
├── candy.h             # Sesame プロトコル定数
├── sesame/
│   ├── ssm.c           # Sesame ステートマシン
│   ├── ssm.h           # Sesame データ構造
│   ├── ssm_cmd.c       # Sesame コマンド処理
│   └── ssm_cmd.h       # Sesame コマンド定義
└── utils/
    ├── uECC.c          # 楕円曲線暗号
    ├── aes-cbc-cmac.c  # AES 暗号化
    ├── c_ccm.c         # CCM モード暗号化
    ├── TI_aes_128.c    # AES-128 実装
    └── utils.c         # ユーティリティ関数
```

## シリアル出力

Pico W は USB CDC 経由でデバッグ情報を出力します。仮想 COM ポートに接続すると以下が確認できます:
- Bluetooth 初期化ステータス
- デバイス探索メッセージ
- 接続ステータス
- Sesame ロック/アンロック イベント

## ライセンス

このプロジェクトは元の ESP32 版と同じ MIT ライセンスを維持しています。

## 謝辞

- CANDY HOUSE による元の ESP32 実装
- Pico SDK と BTstack を使用して Raspberry Pi Pico W に移植