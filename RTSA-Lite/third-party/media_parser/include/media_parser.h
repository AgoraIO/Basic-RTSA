/*************************************************************
 * Author:    wangjiangyuan (wangjiangyuan@agora.io)
 * Date  :    Oct 21th, 2020
 * Module:    Agora SD-RTN SDK RTC C API demo application.
 *
 *
 * This is a part of the Agora RTC Service SDK.
 * Copyright (C) 2020 Agora IO
 * All rights reserved.
 *
 *************************************************************/

#ifndef __MEDIA_PARSER_H__
#define __MEDIA_PARSER_H__

#include <stdlib.h>
#include <stdint.h>

#define FILE_PATH_MAX_LEN        512

typedef struct media_parser_s media_parser_t;

typedef struct {
    int32_t     type;
    uint8_t     *ptr;
    uint32_t    len;

    union {
        struct {
            int32_t     codec;
            uint32_t    key_frame_flag;
        } video;

        struct {
            int32_t     codec;
            int32_t     sampleRateHz;
            int32_t     samplesPerChannel;
            int32_t     numberOfChannels;
        } audio;
    } u;
} frame_t;

struct media_parser_s {
    int32_t     type;
    int32_t     codec;
    void        *p_ctx;

    int32_t (*open)(media_parser_t *h, const char *path);
    int32_t (*obtain_frame)(media_parser_t *h, frame_t *p_frame);
    int32_t (*reset)(media_parser_t *h);
    int32_t (*close)(media_parser_t *h);
};

typedef enum {
    MEDIA_TYPE_AUDIO,
    MEDIA_TYPE_VIDEO,
} media_type_e;

int32_t media_parser_create(int32_t type, int32_t codec, media_parser_t **pp_parser);

void media_parser_destroy(media_parser_t **pp_parser);

#endif /* __MEDIA_PARSER_H__ */