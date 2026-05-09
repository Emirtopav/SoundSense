//                        _ _                           _                    _
//         /\            | (_)        /\               | |                  | |
//        /  \  _   _  __| |_  ___   /  \   _ __   __ _| |_   _ _______ _ __|
//        |__  _ __  _ __
//       / /\ \| | | |/ _` | |/ _ \ / /\ \ | '_ \ / _` | | | | |_  / _ \ '__| '_
//       \| '_ \| '_ \ 
//      / ____ \ |_| | (_| | | (_) / ____ \| | | | (_| | | |_| |/ /  __/ |_ | |
//      | | |_) | |_) |
//     /_/    \_\__,_|\__,_|_|\___/_/    \_\_| |_|\__,_|_|\__, /___\___|_(_)|_|
//     |_| .__/| .__/
//                                                         __/ | | |   | |
//                                                        |___/ |_|   |_|
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

public:
  AudioAnalyzer() : smoothedBands(64, 0.0f) {
    CoInitialize(NULL);
    IMMDeviceEnumerator *pEnumerator = nullptr;
    CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL,
                     __uuidof(IMMDeviceEnumerator), (void **)&pEnumerator);
    IMMDevice *pDevice = nullptr;
    pEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &pDevice);
    pDevice->Activate(__uuidof(IAudioClient), CLSCTX_ALL, NULL,
                      (void **)&pAudioClient);
    pAudioClient->GetMixFormat(&pwfx);
    pAudioClient->Initialize(AUDCLNT_SHAREMODE_SHARED,
                             AUDCLNT_STREAMFLAGS_LOOPBACK, 0, 0, pwfx, NULL);
    pAudioClient->GetBufferSize(&bufferFrameCount);
    pAudioClient->GetService(__uuidof(IAudioCaptureClient),
                             (void **)&pCaptureClient);
    pAudioClient->Start();
    pEnumerator->Release();
    pDevice->Release();
  }

  ~AudioAnalyzer() {
    if (pAudioClient)
      pAudioClient->Stop();
    if (pCaptureClient)
      pCaptureClient->Release();
    if (pAudioClient)
      pAudioClient->Release();
    CoUninitialize();
  }

  void Update() {
    UINT32 packetLength = 0;
    pCaptureClient->GetNextPacketSize(&packetLength);
    while (packetLength > 0) {
      BYTE *pData;
      UINT32 numFramesToRead;
      DWORD flags;
      if (FAILED(pCaptureClient->GetBuffer(&pData, &numFramesToRead, &flags,
                                           NULL, NULL)))
        break;
      if (!(flags & AUDCLNT_BUFFERFLAGS_SILENT)) {
        if (pwfx->wFormatTag == WAVE_FORMAT_EXTENSIBLE) {
          WAVEFORMATEXTENSIBLE *pEx = (WAVEFORMATEXTENSIBLE *)pwfx;
          if (pEx->SubFormat == MY_KSDATAFORMAT_SUBTYPE_IEEE_FLOAT) {
            float *fData = (float *)pData;
            for (UINT32 i = 0; i < numFramesToRead; i++) {
              float sample = 0;
              for (int c = 0; c < pwfx->nChannels; c++)
                sample += fData[i * pwfx->nChannels + c];
              sampleBuffer.push_back(sample / pwfx->nChannels);
            }
          }
        }
      }
      pCaptureClient->ReleaseBuffer(numFramesToRead);
      pCaptureClient->GetNextPacketSize(&packetLength);
    }
    if (sampleBuffer.size() > 4096) {
      sampleBuffer.erase(sampleBuffer.begin(),
                         sampleBuffer.begin() + (sampleBuffer.size() - 4096));
    }
  }

  void FFT(std::vector<std::complex<double>> &a) {
    int n = a.size();
    for (int i = 1, j = 0; i < n; i++) {
      int bit = n >> 1;
      for (; j & bit; bit >>= 1)
        j ^= bit;
      j ^= bit;
      if (i < j)
        std::swap(a[i], a[j]);
    }
    for (int len = 2; len <= n; len <<= 1) {
      double ang = 2 * M_PI / len;
      std::complex<double> wlen(cos(ang), sin(ang));
      for (int i = 0; i < n; i += len) {
        std::complex<double> w(1);
        for (int j = 0; j < len / 2; j++) {
          std::complex<double> u = a[i + j], v = a[i + j + len / 2] * w;
          a[i + j] = u + v;
          a[i + j + len / 2] = u - v;
          w *= wlen;
        }
      }
    }
  }

  std::vector<float> Get64Bands(const VisualizerSettings &settings) {
    std::vector<float> currentBands(BANDS, 0.0f);
    if (sampleBuffer.size() >= FFT_SIZE) {
      std::vector<std::complex<double>> fftInput(FFT_SIZE);
      for (int i = 0; i < FFT_SIZE; i++) {
        double window = 0.5 * (1 - cos(2 * M_PI * i / (FFT_SIZE - 1)));
        fftInput[i] = std::complex<double>(
            sampleBuffer[sampleBuffer.size() - FFT_SIZE + i] * window, 0);
      }
      FFT(fftInput);
      float sampleRate = (float)pwfx->nSamplesPerSec;
      for (int b = 0; b < BANDS; b++) {
        float binLow, binHigh;
        if (b < 12) {
          binLow = (float)b + 1;
          binHigh = (float)b + 2;
        } else {
          float fLow =
              300.0f * pow(20000.0f / 300.0f, (float)(b - 12) / (BANDS - 12));
          float fHigh = 300.0f * pow(20000.0f / 300.0f,
                                     (float)(b + 1 - 12) / (BANDS - 12));
          binLow = fLow / (sampleRate / FFT_SIZE);
          binHigh = fHigh / (sampleRate / FFT_SIZE);
        }
        if (binHigh <= binLow)
          binHigh = binLow + 1.0f;
        float maxMag = 0;
        for (float i = binLow; i < binHigh; i += 0.5f) {
          int idx = (int)i;
          if (idx >= FFT_SIZE / 2)
            break;
          float frac = i - idx;
          float mag = ((float)std::abs(fftInput[idx]) * (1.0f - frac) +
                       (float)std::abs(fftInput[idx + 1]) * frac) /
                      (FFT_SIZE / 2);
          if (mag > maxMag)
            maxMag = mag;
        }
        float multiplier = settings.globalSens;
        if (b < 12)
          multiplier *= settings.bassGain;
        else if (b < 32)
          multiplier *= settings.midGain;
        else
          multiplier *= settings.trebleGain;
        currentBands[b] = maxMag * multiplier;
      }
    }
    for (int b = 0; b < BANDS; b++) {
      float val = currentBands[b] * (1.0f - settings.smoothing) +
                  smoothedBands[b] * settings.smoothing;
      if (val > smoothedBands[b])
        smoothedBands[b] = val;
      else
        smoothedBands[b] *= settings.falloff;
    }
    return smoothedBands;
  }
};
