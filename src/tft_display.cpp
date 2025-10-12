#include <LovyanGFX.hpp>

class LGFX : public lgfx::LGFX_Device {
  lgfx::Panel_ILI9341 _panel;
  lgfx::Bus_SPI _bus;

public:
  LGFX() {
    {
      auto cfg = _bus.config();
      cfg.spi_host = SPI2_HOST;
      cfg.spi_mode = 0;
      cfg.freq_write = 40000000;
      cfg.freq_read = 16000000;
      cfg.spi_3wire = false;
      cfg.use_lock = true;
      cfg.dma_channel = 1;
      cfg.pin_sclk = 18;
      cfg.pin_mosi = 23;
      cfg.pin_miso = -1;
      cfg.pin_dc   = 2;
      _bus.config(cfg);
      _panel.setBus(&_bus);
    }

    {
      auto cfg = _panel.config();
      cfg.pin_cs           = 5;
      cfg.pin_rst          = 4;
      cfg.pin_busy         = -1;
      cfg.memory_width     = 240;
      cfg.memory_height    = 320;
      cfg.panel_width      = 240;
      cfg.panel_height     = 320;
      cfg.offset_x         = 0;
      cfg.offset_y         = 0;
      cfg.offset_rotation  = 0;
      cfg.dummy_read_pixel = 8;
      cfg.dummy_read_bits  = 1;
      cfg.readable         = false;
      cfg.invert           = false;
      cfg.rgb_order        = false;
      cfg.dlen_16bit       = false;
      cfg.bus_shared       = true;
      _panel.config(cfg);
    }

    setPanel(&_panel);
  }
};

LGFX tft;

extern "C" void tft_display_init()
{
    tft.init();
    tft.setRotation(1);  // 0-3 for landscape/portrait
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_YELLOW);
    tft.setTextSize(2);
    tft.setCursor(10, 30);
    tft.println("Welcome to Homiva!");

    // Color bars
    tft.fillRect(10, 70, 220, 20, TFT_RED);
    tft.fillRect(10, 100, 180, 20, TFT_GREEN);
    tft.fillRect(10, 130, 120, 20, TFT_BLUE);
    tft.drawRect(10, 160, 200, 20, TFT_WHITE);
    tft.fillRect(10, 160, 140, 20, TFT_CYAN);

    tft.setCursor(10, 200);
    tft.setTextColor(TFT_WHITE);
    tft.setTextSize(1);
    tft.println("Smart Tank UI Demo v1.0");
}

extern "C" void tft_display_draw_text(const char* text, int x, int y)
{
    tft.setCursor(x, y);
    tft.setTextColor(TFT_MAGENTA);
    tft.setTextSize(2);
    tft.println(text);
}
