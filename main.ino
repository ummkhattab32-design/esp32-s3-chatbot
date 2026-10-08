#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SPI.h>

// ----------------------
// Hardware setup
// ----------------------
// Adjust these pins to your board
#define TFT_CS   10
#define TFT_DC   9
#define TFT_RST  11

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

String input = "";
String history[6];
int historyCount = 0;

// Color definitions (since ST7735_DARKGREY may not exist)
#define DARKGREY  0x4208
#define LIGHTGREY 0x8410

// ----------------------
// Helper functions
// ----------------------
String clampText(String s, int maxLen) {
  if (s.length() <= maxLen) return s;
  return s.substring(0, maxLen - 1) + "~";
}

void pushHistory(String msg) {
  for (int i = 5; i > 0; i--) {
    history[i] = history[i - 1];
  }
  history[0] = msg;
  if (historyCount < 6) historyCount++;
}

void renderScreen() {
  tft.fillScreen(ST7735_BLACK);
  tft.setTextColor(ST7735_WHITE, ST7735_BLACK);
  tft.setTextSize(1);

  // Title
  tft.setCursor(2, 2);
  tft.println("TinyAI-128");

  // Divider line
  tft.drawFastHLine(0, 16, 128, DARKGREY);

  // Display the last 4 messages
  int y = 20;
  int visible = min(historyCount, 4);

  for (int i = 0; i < visible; i++) {
    String line = clampText(history[i], 18);
    tft.setCursor(2, y);
    tft.println(line);
    y += 16;
  }

  // Input area divider
  tft.drawFastHLine(0, 96, 128, DARKGREY);
  
  // Input label and text
  tft.setCursor(2, 100);
  tft.print("IN:");
  tft.setCursor(22, 100);
  tft.println(clampText(input, 15));

  // Help prompt
  tft.setCursor(2, 112);
  tft.println("say: hello");
}

String generateReply(String text) {
  text.toLowerCase();

  if (text.indexOf("hello") >= 0 || text.indexOf("hi") >= 0) {
    return "Hi! I am TinyAI.";
  }

  if (text.indexOf("name") >= 0) {
    return "I am TinyAI-128.";
  }

  if (text.indexOf("story") >= 0) {
    return "Once, a robot found a lantern.";
  }

  if (text.indexOf("joke") >= 0) {
    return "Why so serious? I am a bot.";
  }

  if (text.indexOf("time") >= 0) {
    return "No clock on board, but I am awake.";
  }

  if (text.indexOf("help") >= 0) {
    return "Try: hello, story, joke, name.";
  }

  return "I heard: " + text + ".";
}

void handleInput(String s) {
  if (s.length() == 0) return;

  pushHistory("YOU: " + s);
  String reply = generateReply(s);
  pushHistory("AI: " + reply);
  renderScreen();
}

void setup() {
  Serial.begin(115200);
  delay(100);

  // Init Adafruit display
  // Use INITR_MINI160x80 for 128x128 displays
  tft.initR(INITR_MINI160x80);
  tft.setRotation(1);
  tft.fillScreen(ST7735_BLACK);

  // Initial screen
  pushHistory("AI: hello");
  renderScreen();

  Serial.println("TinyAI ready. Type a message and press Enter.");
}

void loop() {
  while (Serial.available()) {
    char c = Serial.read();

    if (c == '\n' || c == '\r') {
      handleInput(input);
      input = "";
      renderScreen();
    } else if (c == 8 || c == 127) {
      // Backspace
      if (input.length() > 0) input.remove(input.length() - 1);
    } else if (input.length() < 24) {
      input += c;
    }

    renderScreen();
  }
}
