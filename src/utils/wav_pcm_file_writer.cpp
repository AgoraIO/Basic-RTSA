//  Agora RTC/MEDIA SDK
//
//  Created by Pengfei Han in 2020-03.
//  Copyright (c) 2020 Agora.io. All rights reserved.
//

#include "wav_pcm_file_writer.h"

#include <errno.h>

WavPcmFileWriter::WavPcmFileWriter(const std::string& outputFilePath, size_t numberOfChannels,
                                   uint32_t sampleRateHz)
    : output_file_path_(outputFilePath),
      number_of_channels_(numberOfChannels),
      sample_rate_hz_(sampleRateHz),
      wav_file_(nullptr),
      received_sample_count_(0) {}

WavPcmFileWriter::~WavPcmFileWriter() {}

WavHeader WavPcmFileWriter::createWAVHeader() {
  wav_header_.numberOfChannels = number_of_channels_;
  wav_header_.bytesPerSample = sizeof(int16_t) * wav_header_.numberOfChannels;
  wav_header_.dataLength = received_sample_count_ * wav_header_.numberOfChannels * sizeof(int16_t);
  wav_header_.streamLength = wav_header_.dataLength + 36;
  wav_header_.sampleRateHz = sample_rate_hz_;
  wav_header_.bitsPerSample = sizeof(int16_t) * 8;
  wav_header_.bytesPerSecond =
      wav_header_.sampleRateHz * sizeof(int16_t) * wav_header_.numberOfChannels;

  return wav_header_;
}

void WavPcmFileWriter::openWriter() {
  unsigned char wav_header[44];
  wav_file_ = fopen(output_file_path_.c_str(), "w+");

  WavHeader wavHeader = createWAVHeader();
  makeWAVHeader(wav_header, wavHeader);

  if (!fwrite(wav_header, sizeof(wav_header), 1, wav_file_)) {
    printf("Error writing WAV header: %s\n", std::strerror(errno));
  }
}

bool WavPcmFileWriter::writeAudioPcmFrame(void* payload_data, size_t sampleCount,
                                          size_t bytesPerSample) {
  received_sample_count_ += sampleCount;
  if (!fwrite(payload_data, sampleCount * bytesPerSample, 1, wav_file_)) {
    printf("\nError writing decoded audio data: %s\n", std::strerror(errno));
    return false;
  }
  fflush(wav_file_);
  return true;
}

void WavPcmFileWriter::closeWriter() {
  unsigned char wav_header[44];
  wav_header_.dataLength = received_sample_count_ * wav_header_.numberOfChannels * sizeof(int16_t);
  makeWAVHeader(wav_header, wav_header_);
  if (fseek(wav_file_, 0, SEEK_SET) || !fwrite(wav_header, sizeof(wav_header), 1, wav_file_)) {
    printf("Error rewriting WAV header: %s\n", std::strerror(errno));
  }
  fclose(wav_file_);
  printf("Dumped PCM file is saved as: %s [channel count: %lu, sample rate: %u] ...\n",
         output_file_path_.c_str(), number_of_channels_, sample_rate_hz_);
}
