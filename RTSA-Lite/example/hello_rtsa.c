/*************************************************************
 * File  :  hello_rtsa.c
 * Module:  Agora SD-RTN SDK RTC C API demo application.
 *
 * This is a part of the Agora RTC Service SDK.
 * Copyright (C) 2020 Agora IO
 * All rights reserved.
 *
 *************************************************************/

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <unistd.h>
#include <getopt.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "agora_rtc_api.h"
#include "media_parser.h"

#define TAG_APP     "[app]"
#define TAG_API     "[api]"
#define TAG_EVENT   "[event]"

#define INVALID_FD  -1

#define DEFAULT_CHANNEL_NAME            "demo"
#define DEFAULT_CERTIFACTE_FILENAME     "certificate.bin"
#define DEFAULT_CREDENTIAL_FILENAME     "deviceID.bin"
#define DEFAULT_SEND_VIDEO_FILENAME     "send_video.h264"
#define DEFAULT_SEND_AUDIO_FILENAME     "send_audio.aac"
#define DEFAULT_RECV_VIDEO_FILENAME     "recv_video.h264"
#define DEFAULT_RECV_AUDIO_FILENAME     "recv_audio.aac"

#define LOGS(fmt, ...)          fprintf(stdout, "" fmt "\n", ##__VA_ARGS__)
#define LOGI(fmt, ...)          fprintf(stdout, "I/ " fmt " at %s:%u\n", ##__VA_ARGS__, __PRETTY_FUNCTION__, __LINE__)
#define LOGD(fmt, ...)          fprintf(stdout, "D/ " fmt " at %s:%u\n", ##__VA_ARGS__, __PRETTY_FUNCTION__, __LINE__)
#define LOGE(fmt, ...)          fprintf(stdout, "E/ " fmt " at %s:%u\n", ##__VA_ARGS__, __PRETTY_FUNCTION__, __LINE__)
#define LOGW(fmt, ...)          fprintf(stdout, "W/ " fmt " at %s:%u\n", ##__VA_ARGS__, __PRETTY_FUNCTION__, __LINE__)

typedef struct {
    const char  *p_certificate_file;
    const char  *p_credential_file;
    const char  *p_sdk_log_dir;

    const char  *p_appid;
    const char  *p_token;
    const char  *p_channel;
    uint32_t    uid;

    // video
    int32_t     video_codec;
    int32_t     video_send_enable_flag;
    int32_t     video_send_frames_per_sec;
    const char *video_send_media_path;
    int32_t     video_recv_dump_flag;
    const char *video_recv_media_path;

    int32_t     audio_codec;
    int32_t     audio_send_enable_flag;
    int32_t     audio_send_interval_ms;
    const char *audio_send_media_path;
    int32_t     audio_recv_dump_flag;
    const char *audio_recv_media_path;
} app_config_t;


typedef struct {
    app_config_t    config;

    char            str_certificate[AGORA_CERTIFICATE_MAX_LEN];
    uint32_t        str_certificate_len;
    char            str_credential[AGORA_CREDENTIAL_MAX_LEN];
    uint32_t        str_credential_len;

    media_parser_t  *video_parser_ptr;
    int32_t         video_recv_media_fd;

    media_parser_t  *audio_parser_ptr;
    int32_t         audio_recv_media_fd;

    int32_t         b_stop_flag;
    int32_t         b_join_success_flag;
} app_t;

static app_t g_app_instance = {
    .config = {
        .p_certificate_file         =   DEFAULT_CERTIFACTE_FILENAME,
        .p_credential_file          =   DEFAULT_CREDENTIAL_FILENAME,
        .p_sdk_log_dir              =   NULL,
        .p_appid                    =   "",
        .p_channel                  =   DEFAULT_CHANNEL_NAME,
        .p_token                    =   "",
        .uid                        =   0,

        .video_codec                =   VIDEO_CODEC_H264,
        .video_send_enable_flag     =   1,
        .video_send_frames_per_sec  =   VIDEO_FRAME_RATE_FPS_30,
        .video_send_media_path      =   DEFAULT_SEND_VIDEO_FILENAME,
        .video_recv_dump_flag       =   1,
        .video_recv_media_path      =   DEFAULT_RECV_VIDEO_FILENAME,

        .audio_codec                =   AUDIO_CODEC_AACLC,
        .audio_send_enable_flag     =   1,
        .audio_send_interval_ms     =   20,
        .audio_send_media_path      =   DEFAULT_SEND_AUDIO_FILENAME,
        .audio_recv_dump_flag       =   1,
        .audio_recv_media_path      =   DEFAULT_RECV_AUDIO_FILENAME,
    },

    .str_certificate_len    = AGORA_CERTIFICATE_MAX_LEN,
    .str_credential_len     = AGORA_CREDENTIAL_MAX_LEN,

    .video_parser_ptr       = NULL,
    .audio_recv_media_fd    = INVALID_FD,

    .audio_parser_ptr       = NULL,
    .audio_recv_media_fd    = INVALID_FD,

    .b_stop_flag            = 0,
    .b_join_success_flag    = 0,
};

app_t *app_get_instance(void)
{
    return &g_app_instance;
}

static void app_signal_handler(int32_t sig)
{
    app_t *p_app = app_get_instance();
    switch (sig) {
    case SIGQUIT:
    case SIGABRT:
    case SIGINT:
        p_app->b_stop_flag = 1;
        break;
    default:
        LOGW("no handler, sig=%d", sig);
    }
}

void app_print_usage(int32_t argc, char **argv)
{
    LOGS("\nUsage: %s [OPTION]", argv[0]);
    LOGS(" -h, --help               : show help info");
    LOGS(" -i, --appId              : application id, either appId OR token MUST be set");
    LOGS(" -t, --token              : token for authentication");
    LOGS(" -c, --channelId          : channel, default is 'demo'");
    LOGS(" -u, --userId             : user id, default is 0");
    LOGS(" -C, --certificateFile    : certificate file path, default is './%s'", DEFAULT_CERTIFACTE_FILENAME);
    LOGS(" -r, --credentialFile     : credential file path, default is './%s'", DEFAULT_CREDENTIAL_FILENAME);
    LOGS(" -V, --videoCodec         : video codec, default is 2");
    LOGS("                            support: 2=H264; 6=GENERIC;");
    LOGS(" -v, --videoFile          : send video file path, default is './%s'", DEFAULT_SEND_VIDEO_FILENAME);
    LOGS(" -f  --fps                : video frame rate, default is 30");
    LOGS(" -A, --audioCodec         : audio codec, default is 8");
    LOGS("                            support: 1=OPUS; 5=G722; 8=AACLC; 9=HEAAC;");
    LOGS(" -a, --audioFile          : send audio file path, default is './%s'", DEFAULT_SEND_AUDIO_FILENAME);
    LOGS(" -l, --sdkLogDir          : agora SDK log directory, default is '.'");
    LOGS("\nExample:");
    LOGS("    %s --appId xxx [--token xxx] --channelId xxx --videoFile ./video.h264 --fps 30 --audioFile ./audio.aac", argv[0]);
}


int32_t app_parse_args(app_config_t *p_config, int32_t argc, char **argv)
{
    const char* av_short_option = "hi:t:c:u:C:r:V:v:f:A:a:d:";
    const struct option av_long_option[] = {
        {"help",                0, NULL, 'h'},
        {"appId",               1, NULL, 'i'},
        {"token",               1, NULL, 't'},
        {"channelId",           1, NULL, 'c'},
        {"userId",              1, NULL, 'u'},
        {"certificateFile",     1, NULL, 'C'},
        {"credentialFile",      1, NULL, 'r'},
        {"videoCodec",          1, NULL, 'V'},
        {"videoFile",           1, NULL, 'v'},
        {"fps",                 1, NULL, 'f'},
        {"audioCodec",          1, NULL, 'A'},
        {"audioFile",           1, NULL, 'a'},
        {"frameDuration",       1, NULL, 'd'},
        {0, 0, 0, 0}
    };

    int32_t ch = -1;
    int32_t optidx = 0;
    int32_t rval = 0;

    while (1) {
        optidx++;
        ch = getopt_long(argc, argv, av_short_option, av_long_option, NULL);
        if (ch == -1) {
            break;
        }

        switch (ch) {
        case 'h': {
            rval = -1;
            goto EXIT;
        } break;
        case 'i': {
            p_config->p_appid = optarg;
        } break;
        case 't': {
            p_config->p_token = optarg;
        } break;
        case 'c': {
            p_config->p_channel = optarg;
        } break;
        case 'u': {
            p_config->uid = strtol(optarg, NULL, 10);
        } break;
        case 'C': {
            p_config->p_certificate_file = optarg;
        } break;
        case 'r': {
            p_config->p_credential_file = optarg;
            break;
        } break;
        case 'V': {
            p_config->video_codec = strtol(optarg, NULL, 10);
            break;
        } break;
        case 'v': {
            p_config->video_send_media_path = optarg;
            break;
        } break;
        case 'f': {
            p_config->video_send_frames_per_sec = strtol(optarg, NULL, 10);
        } break;
        case 'A': {
            p_config->audio_codec = strtol(optarg, NULL, 10);
        } break;
        case 'a': {
            p_config->audio_send_media_path = optarg;
        } break;
        case 'd': {
            p_config->audio_send_interval_ms = strtol(optarg, NULL, 10);
        } break;
        default:{
            rval = -1;
            LOGS("%s parse cmd param: %s error.", TAG_APP, argv[optidx]);
            goto EXIT;
        }
        }
    }

    // check key parameters
    if (strcmp(p_config->p_appid, "") == 0) {
        rval = -1;
        LOGE("%s appid MUST be provided", TAG_APP);
        goto EXIT;
    }

    if (!p_config->p_channel || strcmp(p_config->p_channel, "") == 0) {
        rval = -1;
        LOGE("%s invalid channel", TAG_APP);
        goto EXIT;
    }

    if (!p_config->p_certificate_file || strcmp(p_config->p_certificate_file, "") == 0) {
        rval = -1;
        LOGE("%s invalid certificate file path", TAG_APP);
        goto EXIT;
    }

    if (!p_config->p_credential_file || strcmp(p_config->p_credential_file, "") == 0) {
        rval = -1;
        LOGE("%s invalid credential dir", TAG_APP);
        goto EXIT;
    }

    if (p_config->video_send_frames_per_sec <= 0) {
        rval = -1;
        LOGE("%s invalid video send frames_per_second:%d", TAG_APP, p_config->video_send_frames_per_sec);
        goto EXIT;
    }

    if (p_config->audio_send_interval_ms <= 0) {
        rval = -1;
        LOGE("%s invalid audio send interval value:%d", TAG_APP, p_config->audio_send_interval_ms);
        goto EXIT;
    }

    if (p_config->video_codec != VIDEO_CODEC_H264
        && p_config->video_codec != VIDEO_CODEC_GENERIC) {
        rval = -1;
        LOGE("%s invalid video codec:%d", TAG_APP, p_config->video_codec);
        goto EXIT;
    }

    if (!p_config->video_send_media_path || strcmp(p_config->video_send_media_path, "") == 0 ||
        !p_config->video_recv_media_path || strcmp(p_config->video_recv_media_path, "") == 0) {
        rval = -1;
        LOGE("%s invalid video path", TAG_APP);
        goto EXIT;
    }

    if (p_config->audio_codec != AUDIO_CODEC_OPUS
        && p_config->audio_codec != AUDIO_CODEC_G722
        && p_config->audio_codec != AUDIO_CODEC_AACLC
        && p_config->audio_codec != AUDIO_CODEC_HEAAC) {
        rval = -1;
        LOGE("%s invalid audio codec:%d", TAG_APP, p_config->audio_codec);
        goto EXIT;
    }

    if (!p_config->audio_send_media_path || strcmp(p_config->audio_send_media_path, "") == 0 ||
        !p_config->audio_recv_media_path || strcmp(p_config->audio_recv_media_path, "") == 0) {
        rval = -1;
        LOGE("%s invalid audio send path", TAG_APP);
        goto EXIT;
    }

EXIT:
    return rval;
}

int32_t app_load_license_file(const char *path, char *buf, uint32_t *buf_len)
{
    int32_t rval = 0;

    if (*buf_len <= 0) {
        LOGE("%s invalid buffer length %d", TAG_APP, *buf_len);
        goto EXIT;
    }

    int32_t fd = open(path, O_RDONLY);
    if (fd == INVALID_FD) {
        rval = -1;
        LOGE("%s open file failed, path=%s", TAG_APP, path);
        goto EXIT;
    }

    rval = read(fd, buf, *buf_len);
    if (rval <= 0) {
        LOGE("%s read file failed, path=%s rval=%d", TAG_APP, path, rval);
        goto EXIT;
    }
    *buf_len = rval;

EXIT:
    if (fd > 0) {
        close(fd);
        fd = INVALID_FD;
    }
    return rval;
}

static int32_t app_init(app_t *p_app)
{
    int32_t rval = 0;

    signal(SIGQUIT, app_signal_handler);
    signal(SIGABRT, app_signal_handler);
    signal(SIGINT, app_signal_handler);

    app_config_t *p_config = &p_app->config;

    rval = media_parser_create(MEDIA_TYPE_VIDEO, p_config->video_codec, &p_app->video_parser_ptr);
    if (rval < 0 || !p_app->video_parser_ptr) {
        rval = -1;
        LOGE("%s obtain parser failed, rval=%d type=%d codec=%d", TAG_APP, rval, MEDIA_TYPE_VIDEO,
             p_config->video_codec);
        goto EXIT;
    }

    rval = p_app->video_parser_ptr->open(p_app->video_parser_ptr, p_config->video_send_media_path);
    if (rval < 0) {
        LOGE("%s parser open send file:%s failed, rval=%d", TAG_APP, p_config->video_send_media_path, rval);
        goto EXIT;
    }

    rval = media_parser_create(MEDIA_TYPE_AUDIO, p_config->audio_codec, &p_app->audio_parser_ptr);
    if (rval < 0 || !p_app->audio_parser_ptr) {
        LOGE("%s obtain parser failed, rval=%d type=%d codec=%d", TAG_APP, rval, MEDIA_TYPE_AUDIO,
             p_config->audio_codec);
        goto EXIT;
    }

    rval = p_app->audio_parser_ptr->open(p_app->audio_parser_ptr, p_config->audio_send_media_path);
    if (rval < 0) {
        LOGE("%s parser open send file:%s failed, rval=%d", TAG_APP, p_config->audio_send_media_path, rval);
        goto EXIT;
    }

    if (p_config->audio_recv_dump_flag) {
        p_app->audio_recv_media_fd = open(p_config->audio_recv_media_path, O_CREAT|O_RDWR, S_IRUSR|S_IWUSR);
        if (p_app->audio_recv_media_fd == INVALID_FD) {
            rval = -1;
            LOGE("%s open audio dump file:%s failed:%d", TAG_APP, p_config->audio_recv_media_path, p_app->audio_recv_media_fd);
            goto EXIT;
        }
    }

    if (p_config->video_recv_dump_flag) {
        p_app->video_recv_media_fd = open(p_config->video_recv_media_path, O_CREAT|O_RDWR, S_IRUSR|S_IWUSR);
        if (p_app->video_recv_media_fd == INVALID_FD) {
            rval = -1;
            LOGE("%s open video dump file:%s failed", TAG_APP, p_config->video_recv_media_path);
            goto EXIT;
        }
    }

#ifdef SDK_LICENSE_ENABLED
    rval = app_load_license_file(p_config->p_certificate_file, p_app->str_certificate, &p_app->str_certificate_len);
    if (rval < 0) {
        LOGE("%s load certificate failed, path=%s rval=%d", TAG_APP, p_config->p_certificate_file, rval);
        goto EXIT;
    }

    rval = app_load_license_file(p_config->p_credential_file, p_app->str_credential, &p_app->str_credential_len);
    if (rval < 0) {
        LOGE("%s load credential failed, path=%s rval=%d", TAG_APP, p_config->p_certificate_file, rval);
        goto EXIT;
    }
#endif

EXIT:
    return rval;
}

static void app_deinit(app_t *p_app)
{
    media_parser_destroy(&p_app->video_parser_ptr);
    media_parser_destroy(&p_app->audio_parser_ptr);

    if (p_app->video_recv_media_fd) {
        close(p_app->video_recv_media_fd);
        p_app->video_recv_media_fd = INVALID_FD;
    }

    if (p_app->audio_recv_media_fd > 0) {
        close(p_app->audio_recv_media_fd);
        p_app->audio_recv_media_fd = INVALID_FD;
    }

    p_app->b_join_success_flag = 0;
    p_app->b_stop_flag = 0;
}

static int32_t app_send_audio(app_t *p_app)
{
    media_parser_t *p_parser = p_app->audio_parser_ptr;

    frame_t frame;
    int32_t rval = p_parser->obtain_frame(p_parser, &frame);
    if (rval < 0) {
        if (rval == -2) {
            p_parser->reset(p_parser);
            LOGI("%s reach the end of audio stream and rewind", TAG_APP);
        } else {
            LOGE("%s obtain audio frame failed, rval=%d frame_len=%d", TAG_APP, rval, frame.len);
        }
        goto EXIT;
    }

    // API: send audio data
    audio_frame_info_t info;
    info.codec = frame.u.audio.codec;

    rval = agora_rtc_send_audio_data(p_app->config.p_channel, frame.ptr, frame.len, &info);
    if (rval < 0) {
        LOGE("%s send video data failed, rval=%d codec=%d len=%d", TAG_API, rval, p_parser->codec,
             frame.len);
        goto EXIT;
    }

EXIT:
    return rval;
}

static int32_t app_send_video(app_t *p_app)
{
    media_parser_t *p_parser = p_app->video_parser_ptr;

    frame_t frame;
    int32_t rval = p_parser->obtain_frame(p_parser, &frame);
    if (rval < 0) {
        if (rval == -2) {
            p_parser->reset(p_parser);
            LOGI("%s reach the end of video stream and rewind", TAG_APP);
        } else {
            LOGE("%s obtain video frame failed, rval=%d frame_len=%d", TAG_APP, rval, frame.len);
        }
        goto EXIT;
    }

    // API: send audio data
    uint8_t stream_id = 0;

    video_frame_info_t info;
    info.codec = frame.u.video.codec;
    info.type = frame.u.video.key_frame_flag ? VIDEO_FRAME_KEY : VIDEO_FRAME_DELTA;
    info.frames_per_sec = p_app->config.video_send_frames_per_sec;
    rval = agora_rtc_send_video_data(p_app->config.p_channel, stream_id, frame.ptr, frame.len, &info);
    if (rval < 0) {
        LOGE("%s send data failed, rval=%d codec=%d len=%d is_key_frame=%d", TAG_API, rval,
             frame.u.video.codec, frame.len, frame.u.video.key_frame_flag);
        goto EXIT;
    }

EXIT:
    return rval;
}

uint64_t app_get_time_ms()
{
    struct timeval tv;
    if (gettimeofday(&tv, NULL) < 0) {
        return 0;
    }
    return (((uint64_t)tv.tv_sec * (uint64_t)1000) + tv.tv_usec / 1000);
}

void app_sleep_ms(int64_t ms)
{
    usleep(ms * 1000);
}

static void __on_join_channel_success(const char *channel, int32_t elapsed)
{
    app_t *p_app = app_get_instance();
    p_app->b_join_success_flag = 1;
    LOGI("%s join success, channel=%s elapsed=%d", TAG_EVENT, channel, elapsed);
}

static void __on_error(const char *channel, int code, const char *msg)
{
    app_t *p_app = app_get_instance();
    if (code == ERR_INVALID_APP_ID) {
        p_app->b_stop_flag = 1;
        LOGE("%s invalid appId, please double-check, code=%u msg=%s", TAG_EVENT, code, msg);
    } else if (code == ERR_INVALID_CHANNEL_NAME) {
        p_app->b_stop_flag = 1;
        LOGE("%s invalid channel, please double-check, ch=%s code=%u msg=%s", TAG_EVENT, channel, code, msg);
    } else if (code == ERR_INVALID_TOKEN || code == ERR_TOKEN_EXPIRED) {
        p_app->b_stop_flag = 1;
        LOGE("%s invalid token, please double-check, code=%u msg=%s", TAG_EVENT, code, msg);
    } else {
        LOGW("%s ch=%s code=%u msg=%s", TAG_EVENT, channel, code, msg);
    }
}

static void __on_key_frame_gen_req(const char *channel, uint32_t uid, uint8_t stream_id)
{
    LOGD("%s ch=%s uid=%u stream_id=%d", TAG_EVENT, channel, uid, stream_id);
}

static void __on_user_joined(const char *channel, uint32_t uid, int elapsed_ms)
{
    LOGD("%s ch=%s uid=%u elapsed_ms=%d", TAG_EVENT, channel, uid, elapsed_ms);
}

static void __on_user_offline(const char *channel, uint32_t uid, int reason)
{
    LOGD("%s ch=%s uid=%u reason=%d", TAG_EVENT, channel, uid, reason);
}

static void __on_audio_data(const char *channel, const uint32_t uid, uint16_t sent_ts,
                            uint8_t codec, const void *data, size_t len)
{
    // LOGD("%s ch=%s uid=%u sent_ts=%d codec=%d len=%d", TAG_EVENT, channel, uid, sent_ts, codec, len);
    app_t *p_app = app_get_instance();
    if (p_app->config.audio_recv_dump_flag
     && p_app->audio_recv_media_fd != INVALID_FD) {
        write(p_app->audio_recv_media_fd, data, len);
    }
}

static void __on_video_data(const char *channel, const uint32_t uid, uint16_t sent_ts,
                            uint8_t codec, uint8_t stream_id, int32_t is_key_frame,
                            const void *data, size_t len)
{
    // LOGD("%s ch=%s uid=%u stream_id=%d key_frame=%d len=%d", TAG_EVENT, channel, uid, stream_id, is_key_frame, len);
    app_t *p_app = app_get_instance();
    if (p_app->config.video_recv_dump_flag
     && p_app->video_recv_media_fd != INVALID_FD) {
        write(p_app->video_recv_media_fd, data, len);
    }
}

static void __on_rdt_availability_changed(const char *channel, int is_available)
{
    LOGD("%s ch=%s is_available=%d", TAG_EVENT, channel, is_available);
}

static void __on_cmd(const char *channel, uint32_t uid, int cmd, const void *param_ptr,
				   size_t param_len)
{
    LOGD("%s ch=%s uid=%u cmd=%d param_len=%d", TAG_EVENT, channel, uid, cmd, (int32_t)param_len);
}

static void __on_target_bitrate_changed(const char *channel, uint32_t target_bps)
{
    LOGD("%s ch=%s target_bps=%u", TAG_EVENT, channel, target_bps);
}

static void __on_connection_lost(const char *channel)
{
    LOGD("%s ch=%s", TAG_EVENT, channel);
}

static void __on_rejoin_channel_success(const char *channel, int elapsed_ms)
{
    LOGD("%s ch=%s elapsed_ms=%d", TAG_EVENT, channel, elapsed_ms);
}

static agora_rtc_event_handler_t event_handler = {
    .on_join_channel_success     = __on_join_channel_success,
    .on_error                    = __on_error,
    .on_user_joined              = __on_user_joined,
    .on_user_offline             = __on_user_offline,
    .on_key_frame_gen_req        = __on_key_frame_gen_req,
    .on_audio_data               = __on_audio_data,
    .on_video_data               = __on_video_data,
    .on_rdt_availability_changed = __on_rdt_availability_changed,
    .on_cmd                      = __on_cmd,
    .on_target_bitrate_changed   = __on_target_bitrate_changed,
    .on_connection_lost          = __on_connection_lost,
    .on_rejoin_channel_success   = __on_rejoin_channel_success,
};

int32_t main(int32_t argc, char **argv)
{
    app_t *p_app = app_get_instance();
    app_config_t *p_config = &p_app->config;

    // 0. app parse args
    int32_t rval = app_parse_args(p_config, argc, argv);
    if (rval != 0) {
        app_print_usage(argc, argv);
        goto EXIT;
    }

    LOGS("%s Welcome to RTSA SDK v%s", TAG_APP, agora_rtc_get_version());

    // 1. app init
    rval = app_init(p_app);
    if (rval < 0) {
        LOGE("%s init failed, rval=%d", TAG_APP, rval);
        goto EXIT;
    }

#ifdef SDK_LICENSE_ENABLED
    // 2. API: verify license
    rval = agora_rtc_license_verify(p_app->str_certificate, p_app->str_certificate_len, p_app->str_credential, p_app->str_credential_len);
    if (rval < 0) {
        LOGE("%s verify license failed, pls confirm, rval=%d error=%s", TAG_API, rval, agora_rtc_err_2_str(rval));
        goto EXIT;
    }
#endif

    // 3. API: init agora rtc sdk
    int32_t appid_len = strlen(p_config->p_appid);
    void *p_appid = (void *)(appid_len == 0 ? NULL : p_config->p_appid);
    rval = agora_rtc_init(p_appid, p_config->uid, &event_handler, p_config->p_sdk_log_dir);
    if (rval < 0) {
        LOGE("%s agora sdk init failed, rval=%d error=%s", TAG_API, rval, agora_rtc_err_2_str(rval));
        goto EXIT;
    }

    // 4. API: join channel
    int32_t token_len = strlen(p_config->p_token);
    void *p_token = (void *)(token_len == 0 ? NULL : p_config->p_token);

    rval = agora_rtc_join_channel(p_config->p_channel, p_token, token_len);
    if (rval < 0) {
        LOGE("%s join channel %s failed, rval=%d error=%s", TAG_API, p_config->p_channel, rval, agora_rtc_err_2_str(rval));
        goto EXIT;
    }

    // 5. wait until join channel success or Ctrl-C trigger stop
    while (1) {
        if (p_app->b_stop_flag || p_app->b_join_success_flag) {
            break;
        }
        app_sleep_ms(10);
    }

    // 6. media transmit loop with pace sender
    int64_t video_predict_time_ms = 0;
    int64_t audio_predict_time_ms = 0;
    int32_t video_send_interval_ms = 1000 / p_config->video_send_frames_per_sec;

    while (!p_app->b_stop_flag) {
        int64_t cur_time_ms = app_get_time_ms();

        if (p_config->video_send_enable_flag) {
            if (video_predict_time_ms == 0) {
                video_predict_time_ms = cur_time_ms;
            }

            if (cur_time_ms >= video_predict_time_ms) {
                app_send_video(p_app);
                video_predict_time_ms += video_send_interval_ms;
            }
        }

        if (p_config->audio_send_enable_flag) {
            if (audio_predict_time_ms == 0) {
                audio_predict_time_ms = cur_time_ms;
            }

            if (cur_time_ms >= audio_predict_time_ms) {
                app_send_audio(p_app);
                audio_predict_time_ms += p_config->audio_send_interval_ms;
            }
        }

        int64_t sleep_ms = 0;
        if (p_config->audio_send_enable_flag || p_config->video_send_enable_flag) {
            if (audio_predict_time_ms > 0 && audio_predict_time_ms <= video_predict_time_ms) {
                sleep_ms = audio_predict_time_ms - cur_time_ms;
            } else {
                sleep_ms = video_predict_time_ms - cur_time_ms;
            }
        } else {
            sleep_ms = 100;
        }

        // LOGD("%s sleep_ms:%02d cur_time_ms:%ld audio_predict_time_ms:%ld video_predict_time_ms:%ld",
        //     TAG_APP, (int)sleep_ms, cur_time_ms, audio_predict_time_ms, video_predict_time_ms);

        if (sleep_ms > 0) {
            app_sleep_ms(sleep_ms);
        }
    }

    // 7. API: leave channel
    agora_rtc_leave_channel(p_config->p_channel);

    // 8. API: fini rtc sdk
    agora_rtc_fini();

EXIT:
    // 9. app deinit
    app_deinit(p_app);
    return rval;
}
