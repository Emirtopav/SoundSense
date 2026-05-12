#include "AudioAnalyzer.hpp"

AudioAnalyzer::AudioAnalyzer() : smoothedBands(64, 0.0f), m_fftBuffer(2048) {
  HRESULT hr = CoInitialize(NULL);
  IMMDeviceEnumerator *pEnumerator = nullptr;
  hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL,
                   __uuidof(IMMDeviceEnumerator), (void **)&pEnumerator);
  if (FAILED(hr) || !pEnumerator) return;

  IMMDevice *pDevice = nullptr;
  hr = pEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &pDevice);
  if (FAILED(hr) || !pDevice) {
    pEnumerator->Release();
    return;
  }

  hr = pDevice->Activate(__uuidof(IAudioClient), CLSCTX_ALL, NULL,
                    (void **)&pAudioClient);
  if (FAILED(hr) || !pAudioClient) {
    if (pDevice) pDevice->Release();
    if (pEnumerator) pEnumerator->Release();
    return;
  }

  if (FAILED(pAudioClient->GetMixFormat(&pwfx)) || !pwfx) {
    pAudioClient->Release();
    pDevice->Release();
    pEnumerator->Release();
    return;
  }
  pAudioClient->Initialize(AUDCLNT_SHAREMODE_SHARED,
                           AUDCLNT_STREAMFLAGS_LOOPBACK, 0, 0, pwfx, NULL);
  pAudioClient->GetBufferSize(&bufferFrameCount);
  pAudioClient->GetService(__uuidof(IAudioCaptureClient),
                           (void **)&pCaptureClient);
  pAudioClient->Start();
  pEnumerator->Release();
  pDevice->Release();
}

AudioAnalyzer::~AudioAnalyzer() {
  if (pAudioClient)
    pAudioClient->Stop();
  if (pCaptureClient)
    pCaptureClient->Release();
  if (pAudioClient)
    pAudioClient->Release();
  CoUninitialize();
}

void AudioAnalyzer::Update() {
  if (!pCaptureClient || !pwfx) return;
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

void AudioAnalyzer::FFT(std::vector<std::complex<double>> &a) {
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

std::vector<float> AudioAnalyzer::Get64Bands(const VisualizerSettings &settings) {
  std::vector<float> currentBands(BANDS, 0.0f);
  if (!pwfx) return currentBands;
  if (sampleBuffer.size() >= FFT_SIZE) {
    for (int i = 0; i < FFT_SIZE; i++) {
      double window = 0.5 * (1 - cos(2 * M_PI * i / (FFT_SIZE - 1)));
      m_fftBuffer[i] = std::complex<double>(
          sampleBuffer[sampleBuffer.size() - FFT_SIZE + i] * window, 0);
    }
    FFT(m_fftBuffer);
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
        float mag = ((float)std::abs(m_fftBuffer[idx]) * (1.0f - frac) +
                     (float)std::abs(m_fftBuffer[idx + 1]) * frac) /
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
