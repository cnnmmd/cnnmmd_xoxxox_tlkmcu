//--------------------------------------------------------------------------
// 参照

#include "shared.h"
#include "params.h"

//--------------------------------------------------------------------------
// 設定

// 定義：画像：画像データのバッファ構造体
struct Bitmap {
  uint32_t bmpwth;
  uint32_t bmphgt;
  uint8_t* datbgr; // ピクセルデータ（形式：BGR ）
};
// 設定：画像：キャッシュされた画像
Bitmap bmpbak = {0, 0, nullptr};
Bitmap bmp001 = {0, 0, nullptr};
Bitmap bmp002 = {0, 0, nullptr};

//--------------------------------------------------------------------------
// 機能（個別）：画像

// ビットマップ（メモリ上）を画面に表示
void drwmem(const Bitmap &bmp, int x, int y) {
  for (uint32_t row = 0; row < bmp.bmphgt; row++) {
    for (uint32_t col = 0; col < bmp.bmpwth; col++) {
      uint8_t b = bmp.datbgr[(row * bmp.bmpwth + col) * 3];
      uint8_t g = bmp.datbgr[(row * bmp.bmpwth + col) * 3 + 1];
      uint8_t r = bmp.datbgr[(row * bmp.bmpwth + col) * 3 + 2];
      M5.Lcd.drawPixel(x + col, y + (bmp.bmphgt - row - 1), M5.Lcd.color565(r, g, b));
    }
  }
}

/// ビットマップ（ファイル）を画面に表示
bool drwbmp(const char* pthimg, int x, int y) {
  File hdlbmp = SPIFFS.open(pthimg, "r");
  if (!hdlbmp) {
    Serial.println("err: open spiffs" + String(pthimg)); // DBG
    return false;
  }
  // 画像ヘッダ（BMP ）を読み込み
  uint16_t typimg = hdlbmp.read() | (hdlbmp.read() << 8);
  if (typimg != 0x4D42) {
    Serial.println("err: read header bmp"); // DBG
    hdlbmp.close();
    return false;
  }
  hdlbmp.seek(10);
  uint32_t offset = 0;
  for (int i = 0; i < 4; i++) {
    offset |= hdlbmp.read() << (i * 8);
  }
  hdlbmp.seek(18);
  uint32_t imgwth = 0, imghgt = 0;
  for (int i = 0; i < 4; i++) {
    imgwth |= hdlbmp.read() << (i * 8);
  }
  for (int i = 0; i < 4; i++) {
    imghgt |= hdlbmp.read() << (i * 8);
  }
  hdlbmp.seek(offset);
  // ピクセル群を読み込み〜表示（形式：BGR (3 bytes = 24 bits)）
  for (int coy = 0; coy < imghgt; coy++) {
    for (int cox = 0; cox < imgwth; cox++) {
      uint8_t b = hdlbmp.read();
      uint8_t g = hdlbmp.read();
      uint8_t r = hdlbmp.read();
      M5.Lcd.drawPixel(x + cox, y + (imghgt - coy - 1), M5.Lcd.color565(r, g, b));
    }
    // パディングを読み飛ばし（4 bytes）
    if (imgwth % 4 != 0) {
      int padding = 4 - (imgwth * 3) % 4;
      for (int p = 0; p < padding; p++) {
        hdlbmp.read();
      }
    }
  }
  hdlbmp.close();
  return true;
}

// ビットマップ（ファイル）をメモリにロード
bool inibmp(const char* pthimg, Bitmap &bmp) {
  File hdlbmp = SPIFFS.open(pthimg, "r");
  if (!hdlbmp) {
    Serial.println("err: open spiffs: " + String(pthimg)); // DBG
    return false;
  }
  // 画像ヘッダ（BMP ）を読み込み
  uint16_t typimg = hdlbmp.read() | (hdlbmp.read() << 8);
  if (typimg != 0x4D42) {
    Serial.println("err: read header bmp: " + String(pthimg)); // DBG
    hdlbmp.close();
    return false;
  }
  hdlbmp.seek(10);
  uint32_t offset = 0;
  for (int i = 0; i < 4; i++) {
    offset |= hdlbmp.read() << (i * 8);
  }
  hdlbmp.seek(18);
  uint32_t imgwth = 0, imghgt = 0;
  for (int i = 0; i < 4; i++) {
    imgwth |= hdlbmp.read() << (i * 8);
  }
  for (int i = 0; i < 4; i++) {
    imghgt |= hdlbmp.read() << (i * 8);
  }
  // ビット深度を確認（3 bytes = 24 bits）
  hdlbmp.seek(28);
  uint16_t dptbpp = hdlbmp.read() | (hdlbmp.read() << 8);
  if (dptbpp != 24) {
    Serial.println("err: read header bmp (3 bytes = 24 bits) : " + String(pthimg)); // DBG
    hdlbmp.close();
    return false;
  }
  // 画像をシーク（ヘッダからピクセル群のオフセットまで）
  hdlbmp.seek(offset);
  // 画像情報を保存
  bmp.bmpwth = imgwth;
  bmp.bmphgt = imghgt;
  size_t lendat = imgwth * imghgt * 3; // 形式（BGR ）
  bmp.datbgr = (uint8_t*)malloc(lendat);
  if (bmp.datbgr == nullptr) {
    Serial.println("err: allocate memory: " + String(pthimg)); // DBG
    hdlbmp.close();
    return false;
  }
  // ピクセルデータを読み込み
  size_t lenrow = (imgwth * 3 + 3) & ~3; // 各行の境界（4 bytes）
  for (uint32_t y = 0; y < imghgt; y++) {
    uint8_t* row = bmp.datbgr + (y * imgwth * 3);
    hdlbmp.read(row, imgwth * 3);
    hdlbmp.seek(hdlbmp.position() + (lenrow - imgwth * 3)); // パディングを読み飛ばし
  }
  hdlbmp.close();
  return true;
}

//--------------------------------------------------------------------------
// 関数

// 画像：初期
void iniimg() {
  inibmp(img001, bmp001);
  inibmp(img002, bmp002);
  drwbmp(imgbak, bakcox, bakcoy);
  drwmem(bmp001, poscox, poscoy);
}

// 通信／音声／画像：サーバから音声データを取得〜音声を再生〜画像を描画
void rcvvce() {
  cliweb.setTimeout(15000); // タイムアウト（15 sec）
  cliweb.begin(urlrcv);
  int resweb = cliweb.GET();
  if (resweb == HTTP_CODE_OK) {
    WiFiClient* stream = cliweb.getStreamPtr();
    uint8_t buffer[lenbff_rcv];

    int flgimg = 0; // 表示：画像
    int cntswt = 0; // 表示：画像
    while (stream->available()) {
      size_t difcon = stream->readBytes(buffer, sizeof(buffer));
      modvol(buffer, sizeof(buffer), volume); // 調整：音量
      size_t difbff;
      i2s_write(I2S_NUM_0, (uint8_t*)buffer, difcon, &difbff, portMAX_DELAY);
      // 表示：画像
      if (cntswt >= maxswt) {
        if (flgimg == 0) {
          drwmem(bmp002, poscox, poscoy);
          flgimg = 1;
        }
        else {
          drwmem(bmp001, poscox, poscoy);
          flgimg = 0;
        }
        cntswt = 0;
      }
      cntswt = cntswt + 1;
      //
    }
  }
  else {
    Serial.println("err: recieve data from server"); // DBG
  }
  cliweb.end();
}

//--------------------------------------------------------------------------
// 処理

// 初期
void setup() {
  inienv();
  cnnnet();
  iniimg();
}

// 反復
void loop() {
  // 初期
  M5.update(); // ボタンの状態を更新
  // 送信
  inii2s_snd();
  setmsg("rec: bgn");
  recvce();
  setmsg("rec: end");
  sndvce();
  endi2s();
  // 受信
  inii2s_rcv();
  setmsg("ply: bgn");
  rcvvce();
  setmsg("ply: end");
  endi2s();
  // 初期
  setmsg("prc: ini");
  while (! (M5.BtnC.wasPressed())) {
    M5.update(); // ボタンの状態を再度更新
    delay(10);
  }
}
