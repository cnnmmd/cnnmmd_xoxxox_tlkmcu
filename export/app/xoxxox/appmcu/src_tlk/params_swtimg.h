//--------------------------------------------------------------------------

#ifndef PARAMS
#define PARAMS

//--------------------------------------------------------------------------
// 参照

#include <Arduino.h>

//--------------------------------------------------------------------------
// 定義：変数

// 通信
const char* wifiid = "XXX"; // 無線（Wifi）：ＩＤ
const char* wifipw = "XXX"; // 無線（Wifi）：パスワード
const char* srvadr = "192.168.0.1"; // アドレス
const uint16_t srvprt = 10001;
const char* pthsnd = "/sps000";
const char* pthrcv = "/gpp000";

// 音声
const char* pthaud = "/vcetmp.pcm"; // 音声ファイルの格納場所（一時的）
const int secrec = 5; // 録音時間（秒）
const float volume = 0.5; // 調整：音量

// 画像
// ビットマップ画像のファイル名
constexpr char* imgbak = "/img001.bmp";
constexpr char* img001 = "/img001_001.bmp";
constexpr char* img002 = "/img001_002.bmp";
// 静的画像の表示位置
constexpr int bakcox = 40;
constexpr int bakcoy = 0;
// 動的画像の表示位置
constexpr int poscox = 150;
constexpr int poscoy = 140;
//
constexpr int maxswt = 10; // 画像を切り替える間隔（音声のバッファを１フレームとして、Ｎフレームごとに）

//--------------------------------------------------------------------------

#endif // PARAMS
