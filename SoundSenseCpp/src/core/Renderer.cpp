#include "Renderer.hpp"
#include "Globals.hpp"

void SetPixel(std::vector<int> &buffer, int x, int y, bool on) {
  if (x < 0 || x >= 128 || y < 0 || y >= 40)
    return;
  int byteIndex = (y * 16) + (x / 8);
  int bitIndex = 7 - (x % 8);
  if (on)
    buffer[byteIndex] |= (1 << bitIndex);
  else
    buffer[byteIndex] &= ~(1 << bitIndex);
}

std::vector<int> RenderBitmap(const std::vector<float> &data,
                              const VisualizerSettings &settings) {
  std::vector<int> buffer(640, 0);
  for (int b = 0; b < 64; b++) {
    int height = (int)(data[b] * 120);
    if (height > 40)
      height = 40;
    if (settings.showPeaks) {
      if (height > g_PeakHeights[b])
        g_PeakHeights[b] = (float)height;
      else
        g_PeakHeights[b] -= 0.4f;
      if (g_PeakHeights[b] < 0)
        g_PeakHeights[b] = 0;
    }
    int peakY = 39 - (int)g_PeakHeights[b];
    if (peakY < 0)
      peakY = 0;
    if (settings.mode == 0) {
      int x = b * 2;
      for (int y = 0; y < height; y++)
        SetPixel(buffer, x, 39 - y, true);
      if (settings.showPeaks)
        SetPixel(buffer, x, peakY, true);
    } else if (settings.mode == 1) {
      int center = 32, offset = b / 2,
          x = (b % 2 == 0) ? (center + offset) : (center - offset - 1);
      int realX = x * 2;
      for (int y = 0; y < height; y++)
        SetPixel(buffer, realX, 39 - y, true);
      if (settings.showPeaks)
        SetPixel(buffer, realX, peakY, true);
    } else if (settings.mode == 2) {
      int x = b * 2;
      if (height > 0)
        SetPixel(buffer, x, 39 - (height - 1), true);
    }
  }
  return buffer;
}
