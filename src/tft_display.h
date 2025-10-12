#pragma once

#ifdef __cplusplus
extern "C" {
#endif

void tft_display_init(void);
void tft_display_draw_text(const char* text, int x, int y);

#ifdef __cplusplus
}
#endif
