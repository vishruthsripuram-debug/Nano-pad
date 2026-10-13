// Nano Pad firmware - XIAO ESP32-C6, 4x MX switches, 0.91" 128x32 SSD1306 (I2C)
// Libraries: Adafruit SSD1306 + Adafruit GFX (Library Manager). BLE keyboard uses the built-in ESP32 BLE library.
// Arduino settings: Board "XIAO_ESP32C6", USB CDC On Boot = Enabled
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Preferences.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLEHIDDevice.h>
#include <BLESecurity.h>

const uint8_t BTN[4] = {19, 17, 2, 21};  // SW1..SW4 = D8, D7, D2, D3 (to GND)
#define SDA_PIN 22                        // D4
#define SCL_PIN 23                        // D5

Adafruit_SSD1306 oled(128, 32, &Wire, -1);
Preferences prefs;

String act[4][3];  // [button][0=single, 1=hold, 2=double]
uint16_t holdMs = 500, dblMs = 250;
enum { M_IDLE, M_TEXT, M_IMG, M_PET };
uint8_t mode = M_IDLE, petType = 0, petRate = 5, petSleep = 20;
String txt;
uint8_t img[512];
float hunger = 0;
uint32_t lastAct = 0, feedT = 0, lastTick = 0;
bool dirty = true, lastBle = false, hasOled = false;

struct Btn { bool down = false, fired = false; uint8_t clicks = 0; uint32_t chg = 0, t = 0, rel = 0; } b[4];
// ---------- storage ----------
String key(int i, int j) { return "a" + String(i) + String(j); }
void saveAct(int i, int j) { prefs.putString(key(i, j).c_str(), act[i][j]); }
void saveMisc() {
  prefs.putUShort("hm", holdMs); prefs.putUShort("dm", dblMs);
  prefs.putUChar("mode", mode); prefs.putString("txt", txt);
  prefs.putUChar("pt", petType); prefs.putUChar("pr", petRate); prefs.putUChar("ps", petSleep);
}

// ---------- BLE HID keyboard (built-in ESP32 BLE library, nothing extra to install) ----------
static const uint8_t HID_MAP[] = {
  0x05,0x01,0x09,0x06,0xA1,0x01,0x85,0x01,0x05,0x07,0x19,0xE0,0x29,0xE7,0x15,0x00,0x25,0x01,0x75,0x01,0x95,0x08,0x81,0x02,
  0x95,0x01,0x75,0x08,0x81,0x03,
  0x95,0x05,0x75,0x01,0x05,0x08,0x19,0x01,0x29,0x05,0x91,0x02,
  0x95,0x01,0x75,0x03,0x91,0x03,
  0x95,0x06,0x75,0x08,0x15,0x00,0x25,0x65,0x05,0x07,0x19,0x00,0x29,0x65,0x81,0x00,
  0xC0,
  0x05,0x0C,0x09,0x01,0xA1,0x01,0x85,0x02,0x15,0x00,0x25,0x01,0x75,0x01,0x95,0x06,
  0x09,0xCD,0x09,0xB5,0x09,0xB6,0x09,0xE9,0x09,0xEA,0x09,0xE2,0x81,0x02,
  0x95,0x02,0x81,0x03,0xC0};

BLEHIDDevice* hid;
BLECharacteristic *kbIn, *mediaIn;
volatile bool bleConnected = false;

class SrvCb : public BLEServerCallbacks {
  void onConnect(BLEServer*) { bleConnected = true; }
  void onDisconnect(BLEServer*) { bleConnected = false; BLEDevice::startAdvertising(); }
};

void bleInit() {
  BLEDevice::init("Nano Pad");
  BLEServer* srv = BLEDevice::createServer();
  srv->setCallbacks(new SrvCb());
  hid = new BLEHIDDevice(srv);
  kbIn = hid->inputReport(1);
  mediaIn = hid->inputReport(2);
  hid->outputReport(1);
  hid->manufacturer()->setValue("DIY");
  hid->pnp(0x02, 0xe502, 0xa111, 0x0210);
  hid->hidInfo(0x00, 0x01);
  BLESecurity* sec = new BLESecurity();
  sec->setAuthenticationMode(ESP_LE_AUTH_BOND);
  sec->setCapability(ESP_IO_CAP_NONE);
  sec->setInitEncryptionKey(ESP_BLE_ENC_KEY_MASK | ESP_BLE_ID_KEY_MASK);
  hid->reportMap((uint8_t*)HID_MAP, sizeof(HID_MAP));
  hid->startServices();
  hid->setBatteryLevel(100);
  BLEAdvertising* adv = srv->getAdvertising();
  adv->setAppearance(0x03C1);
  adv->addServiceUUID(hid->hidService()->getUUID());
  adv->start();
}

void kbSend(uint8_t mod, const uint8_t* k, int n) {
  uint8_t r[8] = {mod, 0, 0, 0, 0, 0, 0, 0};
  for (int i = 0; i < n && i < 6; i++) r[2 + i] = k[i];
  kbIn->setValue(r, 8); kbIn->notify();
}
void mediaSend(uint8_t bit) {
  uint8_t r = bit;
  mediaIn->setValue(&r, 1); mediaIn->notify(); delay(15);
  r = 0; mediaIn->setValue(&r, 1); mediaIn->notify();
}

struct K { const char* n; uint8_t c; };
static const K NAMED[] = {{"ENTER",0x28},{"ESC",0x29},{"BACKSPACE",0x2A},{"TAB",0x2B},{"SPACE",0x2C},{"DELETE",0x4C},
  {"RIGHT",0x4F},{"LEFT",0x50},{"DOWN",0x51},{"UP",0x52},{"HOME",0x4A},{"END",0x4D},{"PGUP",0x4B},{"PGDN",0x4E}};

bool toHid(char c, uint8_t& mod, uint8_t& code) {
  mod = 0;
  if (c >= 'a' && c <= 'z') { code = 0x04 + c - 'a'; return true; }
  if (c >= 'A' && c <= 'Z') { mod = 0x02; code = 0x04 + c - 'A'; return true; }
  if (c >= '1' && c <= '9') { code = 0x1E + c - '1'; return true; }
  if (c == '0') { code = 0x27; return true; }
  if (c == ' ') { code = 0x2C; return true; }
  if (c == '\n') { code = 0x28; return true; }
  static const char SH[] = ")!@#$%^&*(";
  for (int i = 0; i < 10; i++) if (c == SH[i]) { mod = 0x02; code = i == 0 ? 0x27 : 0x1E + i - 1; return true; }
  static const char U[] = "-=[]\\;'`,./", S[] = "_+{}|:\"~<>?";
  static const uint8_t C[] = {0x2D,0x2E,0x2F,0x30,0x31,0x33,0x34,0x35,0x36,0x37,0x38};
  for (int i = 0; i < 11; i++) {
    if (c == U[i]) { code = C[i]; return true; }
    if (c == S[i]) { mod = 0x02; code = C[i]; return true; }
  }
  return false;
}

void typeText(const String& s) {
  for (size_t i = 0; i < s.length(); i++) {
    uint8_t m = 0, c = 0;
    if (!toHid(s[i], m, c)) continue;
    kbSend(m, &c, 1); delay(8); kbSend(0, nullptr, 0); delay(8);
  }
}

void sendCombo(String s) {
  s.trim(); s.toUpperCase();
  static const char* MEDIA[] = {"PLAYPAUSE", "NEXT", "PREV", "VOLUP", "VOLDOWN", "MUTE"};
  for (int i = 0; i < 6; i++) if (s == MEDIA[i]) { mediaSend(1 << i); return; }
  uint8_t mod = 0, keys[6]; int n = 0, st = 0;
  while (st < (int)s.length()) {
    int p = s.indexOf('+', st); if (p < 0) p = s.length();
    String t = s.substring(st, p); t.trim(); st = p + 1;
    if (t == "CTRL") mod |= 1;
    else if (t == "SHIFT") mod |= 2;
    else if (t == "ALT") mod |= 4;
    else if (t == "GUI" || t == "WIN" || t == "CMD") mod |= 8;
    else if (n < 6) {
      uint8_t c = 0, m2 = 0;
      for (auto& k : NAMED) if (t == k.n) c = k.c;
      if (!c && t.length() > 1 && t[0] == 'F') { int f = t.substring(1).toInt(); if (f >= 1 && f <= 12) c = 0x3A + f - 1; }
      if (!c && t.length() == 1) toHid(tolower(t[0]), m2, c);
      if (c) keys[n++] = c;
    }
  }
  kbSend(mod, keys, n); delay(20); kbSend(0, nullptr, 0);
}

void feed() { hunger = 0; feedT = millis(); dirty = true; }

void run(const String& a) {
  lastAct = millis(); dirty = true;
  if (!a.length()) return;
  if (a == "PET FEED") { feed(); return; }
  if (!bleConnected) return;
  if (a.startsWith("TYPE:")) typeText(a.substring(5)); else sendCombo(a);
}

// ---------- buttons ----------
void scan() {
  uint32_t n = millis();
  for (int i = 0; i < 4; i++) {
    Btn& x = b[i];
    bool d = !digitalRead(BTN[i]);
    if (d != x.down && n - x.chg > 20) {
      x.chg = n; x.down = d; lastAct = n;
      if (d) { x.fired = false; x.t = n; }
      else if (!x.fired) {
        if (act[i][2].length()) {
          if (++x.clicks >= 2) { x.clicks = 0; run(act[i][2]); } else x.rel = n;
        } else run(act[i][0]);
      }
    }
    if (x.down && !x.fired && act[i][1].length() && n - x.t >= holdMs) { x.fired = true; x.clicks = 0; run(act[i][1]); }
    if (x.clicks == 1 && !x.down && n - x.rel > dblMs) { x.clicks = 0; run(act[i][0]); }
  }
}

// ---------- display ----------
void drawPet() {
  int x = 52, y = 10;
  bool sleepy = millis() - lastAct > petSleep * 1000UL;
  bool happy = millis() - feedT < 1500 && feedT;
  if (petType == 1) { oled.fillRect(x, y + 4, 24, 12, 1); oled.fillRect(x + 2, y, 6, 6, 1); oled.fillRect(x + 16, y, 6, 6, 1); }
  else if (petType == 2) { oled.fillRect(x, y, 24, 16, 1); for (int i = 0; i < 4; i++) oled.fillRect(x + i * 6, y + 16, 4, 4, 1); }
  else { oled.fillRect(x, y + 2, 24, 14, 1); oled.fillRect(x + 4, y, 16, 4, 1); }
  if (sleepy) { oled.fillRect(x + 5, y + 8, 5, 1, 0); oled.fillRect(x + 14, y + 8, 5, 1, 0); oled.setTextSize(1); oled.setCursor(x + 30, y - 6); oled.print("z"); }
  else { oled.fillRect(x + 6, y + 6, 3, 3, 0); oled.fillRect(x + 15, y + 6, 3, 3, 0); }
  if (happy) { oled.fillRect(x + 8, y + 12, 8, 1, 0); }
  else if (hunger > 70) { oled.fillRect(x + 9, y + 12, 6, 1, 0); oled.fillRect(x + 8, y + 13, 1, 1, 0); oled.fillRect(x + 15, y + 13, 1, 1, 0); }
  oled.drawRect(0, 28, 128, 4, 1);
  oled.fillRect(0, 28, (int)((100 - hunger) * 1.28f), 4, 1);
}

void draw() {
  if (!hasOled) return;
  oled.clearDisplay(); oled.setTextColor(1);
  if (mode == M_IMG) oled.drawBitmap(0, 0, img, 128, 32, 1);
  else if (mode == M_TEXT) {
    bool big = txt.length() <= 10;
    oled.setTextSize(big ? 2 : 1); oled.setCursor(0, big ? 8 : 0); oled.print(txt);
  } else if (mode == M_PET) drawPet();
  else {
    oled.setTextSize(2); oled.setCursor(0, 0); oled.print("NANO PAD");
    oled.setTextSize(1); oled.setCursor(0, 22); oled.print(bleConnected ? "BLE connected" : "BLE pairing...");
  }
  oled.display();
}

// ---------- serial commands ----------
void reply(bool ok) { Serial.println(ok ? "OK" : "ERR"); }

void handle(String l) {
  l.trim();
  if (l.startsWith("TEST ")) { run(l.substring(5)); return reply(true); }
  if (l == "STATUS") { Serial.printf("BLE=%d\n", bleConnected); return; }
  if (l == "DUMP") {
    Serial.printf("BLE=%d hold=%u dbl=%u\n", bleConnected, holdMs, dblMs);
    for (int i = 0; i < 4; i++) Serial.printf("B%d S=[%s] H=[%s] D=[%s]\n", i + 1, act[i][0].c_str(), act[i][1].c_str(), act[i][2].c_str());
    return;
  }
  if (l.startsWith("SET ")) {
    int n = l.substring(4, 5).toInt() - 1;
    String r = l.substring(6);
    int sp = r.indexOf(' ');
    if (n < 0 || n > 3 || sp < 0) return reply(false);
    String kind = r.substring(0, sp), v = r.substring(sp + 1); v.trim();
    int k = kind == "SINGLE" ? 0 : kind == "HOLD" ? 1 : kind == "DOUBLE" ? 2 : -1;
    if (k < 0) return reply(false);
    if (k > 0) {
      int ls = v.lastIndexOf(' ');
      if (ls < 0) return reply(false);
      int ms = v.substring(ls + 1).toInt();
      v = v.substring(0, ls); v.trim();
      if (k == 1) holdMs = constrain(ms, 100, 2000); else dblMs = constrain(ms, 100, 600);
    }
    if (v == "NONE") v = "";
    act[n][k] = v; saveAct(n, k); saveMisc();
    return reply(true);
  }
  if (l.startsWith("SCREEN TEXT ")) {
    txt = l.substring(12); txt.replace("\"", ""); mode = M_TEXT;
  } else if (l.startsWith("SCREEN IMG ")) {
    String bits = l.substring(11);
    if (bits.length() != 4096) return reply(false);
    memset(img, 0, 512);
    for (int i = 0; i < 4096; i++) if (bits[i] == '1') img[i >> 3] |= 1 << (7 - (i & 7));
    prefs.putBytes("img", img, 512); mode = M_IMG;
  } else if (l == "SCREEN PET ENABLE") mode = M_PET;
  else if (l.startsWith("PET TYPE ")) { String t = l.substring(9); petType = t == "cat" ? 1 : t == "ghost" ? 2 : 0; }
  else if (l.startsWith("PET HUNGER_RATE ")) petRate = constrain(l.substring(16).toInt(), 1, 10);
  else if (l.startsWith("PET SLEEP_TIME ")) petSleep = constrain(l.substring(15).toInt(), 5, 60);
  else if (l == "PET FEED") feed();
  else if (l == "PET RESET") { hunger = 0; feedT = 0; }
  else if (l == "SCREEN IDLE") mode = M_IDLE;
  else return reply(false);
  saveMisc(); dirty = true; reply(true);
}

void setup() {
  Serial.setRxBufferSize(8192);
  Serial.begin(115200);
  for (int i = 0; i < 4; i++) pinMode(BTN[i], INPUT_PULLUP);
  Wire.begin(SDA_PIN, SCL_PIN);
  Wire.beginTransmission(0x3C);
  hasOled = (Wire.endTransmission() == 0);
  if (hasOled) oled.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  prefs.begin("nanopad", false);
  for (int i = 0; i < 4; i++) for (int j = 0; j < 3; j++) act[i][j] = prefs.getString(key(i, j).c_str(), "");
  holdMs = prefs.getUShort("hm", 500); dblMs = prefs.getUShort("dm", 250);
  mode = prefs.getUChar("mode", M_IDLE); txt = prefs.getString("txt", "");
  petType = prefs.getUChar("pt", 0); petRate = prefs.getUChar("pr", 5); petSleep = prefs.getUChar("ps", 20);
  prefs.getBytes("img", img, 512);
  bleInit();
  lastAct = millis();
}

void loop() {
  scan();
  static String line;
  while (Serial.available()) {
    char c = Serial.read();
    if (c == '\n') { handle(line); line = ""; }
    else if (c != '\r' && line.length() < 6000) line += c;
  }
  uint32_t n = millis();
  if (mode == M_PET && n - lastTick >= 1000) { lastTick = n; hunger = min(100.0f, hunger + petRate * 0.2f); dirty = true; }
  bool ble = bleConnected;
  if (ble != lastBle) { lastBle = ble; dirty = true; }
  if (dirty) { dirty = false; draw(); }
}
