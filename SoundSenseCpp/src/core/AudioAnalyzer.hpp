//                            _
//         /\         /\     | |
//        /  \       /  \    | |__  _ __  _ __
//       / /\ \     / /\ \   | '_ \| '_ \| '_ \ 
//      / ____ \ _ / ____ \ _| | | | |_) | |_) |
//     /_/    \_(_)_/    \_(_)_| |_| .__/| .__/
//                                 | |   | |
//                                 |_|   |_|
#pragma once
#include <audioclient.h>
#include <initguid.h>
#include <ksmedia.h>
#include <mmdeviceapi.h>
#include <vector>
#include <windows.h>

DEFINE_GUID(MY_KSDATAFORMAT_SUBTYPE_IEEE_FLOAT, 0x00000003, 0x0000, 0x0010,
            0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71);

#include <algorithm>
#include <cmath>
#include <complex>
#include <iostream>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

struct VisualizerSettings {
  float bassGain = 1.0f;
  float midGain = 1.0f;
  float trebleGain = 1.0f;
  float globalSens = 1.0f;
  int mode = 0;
  float falloff = 0.85f;
  float smoothing = 0.5f;
  bool showPeaks = true;
  int brightness = 128;
  int rotation = 0; // 0, 90, 180, 270
};

class AudioAnalyzer {
private:
  const int FFT_SIZE = 2048;
  const int BANDS = 64;

  IAudioClient *pAudioClient = nullptr;
  IAudioCaptureClient *pCaptureClient = nullptr;
  WAVEFORMATEX *pwfx = nullptr;
  UINT32 bufferFrameCount;

  std::vector<float> sampleBuffer;
  std::vector<float> smoothedBands;
  std::vector<std::complex<double>> m_fftBuffer;

public:
  AudioAnalyzer();
  ~AudioAnalyzer();

  void Update();
  void FFT(std::vector<std::complex<double>> &a);
  std::vector<float> Get64Bands(const VisualizerSettings &settings);
};
