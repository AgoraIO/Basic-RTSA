//  Agora RTC/MEDIA SDK
//
//  Created by Pengfei Han in 2019-06.
//  Copyright (c) 2019 Agora.io. All rights reserved.
//
#include "ogg_opus_file_parser.h"

#include <opusfile.h>

int op_decode_cb(void* ctx, OpusMSDecoder* decoder, void* pcm, const ogg_packet* op, int nsamples,
                 int nchannels, int format, int li) {
  struct decode_context* context = (struct decode_context*)ctx;
  context->nsamples = nsamples;
  context->nchannels = nchannels;
  context->format = format;

  context->b_o_s = op->b_o_s;
  context->e_o_s = op->e_o_s;
  context->bytes = op->bytes;
  context->granulepos = op->granulepos;
  context->packetno = op->packetno;
  memcpy(context->packet, op->packet, op->bytes);

  context->total_length += op->bytes;
  ++context->current_packet;

  (void)pcm;
  (void)decoder;
  (void)li;

  return 0;
}

OggOpusFileParser::OggOpusFileParser(const char* filepath)
    : oggOpusFilePath_(strdup(filepath)),
      oggOpusFile_(nullptr),
      sampleRateHz_(48000),
      numberOfChannels_(0),
      eof(false) {}

OggOpusFileParser::~OggOpusFileParser() {
  if (oggOpusFile_) {
    op_free(oggOpusFile_);
  }
  free(static_cast<void*>(oggOpusFilePath_));
}

void OggOpusFileParser::loadMetaInfo(OggOpusFile* oggOpusFile) {
  const OpusHead* head = op_head(oggOpusFile, -1);
  numberOfChannels_ = head->channel_count;
  sampleRateHz_ = head->input_sample_rate;
  if (op_seekable(oggOpusFile)) {
  }
}

bool OggOpusFileParser::open() {
  int ret = 0;
  oggOpusFile_ = op_open_file(oggOpusFilePath_, &ret);
  if (oggOpusFile_) {
    op_set_decode_callback(oggOpusFile_, op_decode_cb, &decode_context_);
    loadMetaInfo(oggOpusFile_);
  }
  return oggOpusFile_ != nullptr;
}

bool OggOpusFileParser::hasNext() {
  opus_int16 pcm[120 * 48 * 2];
  int ret = op_read_stereo(oggOpusFile_, pcm, sizeof(pcm) / sizeof(*pcm));
  return ret >= 0 && !eof;
}

void OggOpusFileParser::getNext(char* buffer, int* length) {
  if (*length > decode_context_.bytes) {
    memcpy(buffer, decode_context_.packet, decode_context_.bytes);
    *length = decode_context_.bytes;

    if (decode_context_.e_o_s) {
      eof = true;
    }
  }
}

agora::rtc::AUDIO_CODEC_TYPE OggOpusFileParser::getCodecType() {
  return agora::rtc::AUDIO_CODEC_OPUS;
}

int OggOpusFileParser::getSampleRateHz() {
  // return sampleRateHz_; // All Opus audio is coded at 48 kHz, and should also be decoded at 48
  // kHz
  return 48000;
}

int OggOpusFileParser::getNumberOfChannels() { return numberOfChannels_; }
