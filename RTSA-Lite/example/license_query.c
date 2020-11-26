/*************************************************************
 * File  :  license_query.c
 * Module:  Agora SD-RTN SDK RTC C API license status query.
 *
 * This is a tool to activate license Agora RTC Service SDK.
 * Copyright (C) 2020 Agora IO
 * All rights reserved.
 *
 *************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <unistd.h>
#include <getopt.h>
#include <fcntl.h>

#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <netdb.h>

#include "jsmn.h"
#include "http_parser.h"

#define LOGS(fmt, ...)  fprintf(stdout, "" fmt "\n", ##__VA_ARGS__)
#define LOGI(fmt, ...)  fprintf(stdout, "I/ " fmt " at %s:%u\n", ##__VA_ARGS__, __PRETTY_FUNCTION__, __LINE__)
#define LOGD(fmt, ...)  fprintf(stdout, "D/ " fmt " at %s:%u\n", ##__VA_ARGS__, __PRETTY_FUNCTION__, __LINE__)
#define LOGE(fmt, ...)  fprintf(stdout, "E/ " fmt " at %s:%u\n", ##__VA_ARGS__, __PRETTY_FUNCTION__, __LINE__)
#define LOGW(fmt, ...)  fprintf(stdout, "W/ " fmt " at %s:%u\n", ##__VA_ARGS__, __PRETTY_FUNCTION__, __LINE__)

#define TAG_APP     "[app]"

#define INVALID_FD                      -1
#define MAX_BUF_LEN                     1024
#define APPID_MAX_LEN                   64
#define CUSTOMER_KEY_MAX_LEN            64
#define CUSTOMER_SECRET_MAX_LEN         64
#define LICENSE_KEY_MAX_LEN             128
#define TIME_MAX_LEN                    128

#define DEFAULT_CONFIG_FILE             "./license.cfg"
#define DEFAULT_OUTPUT_FILE             "./license_status.csv"
#define DEFAULT_HTTP_BUFFER_SIZE        (1*1024*1024)
#define DEFAULT_HTTP_BODY_SIZE          (1*1024*1024)
#define DEFAULT_JSON_TOKENS_MAX_COUNT   8192

typedef struct {
    const char  *p_config_file;
    const char  *p_output_file;
} app_config_t;

typedef struct {
    char        str_license_key[LICENSE_KEY_MAX_LEN];
    char        str_create_time[TIME_MAX_LEN];
    char        str_valid_period[TIME_MAX_LEN];
    char        str_activated[16];
    char        str_activate_time[TIME_MAX_LEN];
} app_license_entry_t;

typedef struct {
    app_config_t            config;
    jsmn_parser             json_parser;
    http_parser             http_parser;

    // input: load from config license.cfg
    char                    str_appid[APPID_MAX_LEN];
    char                    str_customer_key[CUSTOMER_KEY_MAX_LEN];
    char                    str_customer_secret[CUSTOMER_SECRET_MAX_LEN];

    int32_t                 http_buffer_size;
    int32_t                 http_buffer_len;
    char                    *http_buffer_ptr;

    int32_t                 http_body_size;
    int32_t                 http_body_len;
    char                    *http_body_ptr;

    int32_t                 json_token_size;
    jsmntok_t               *json_token_ptr;

    int32_t                 b_query_success;
    int32_t                 result_license_tab_count;
    app_license_entry_t     *result_license_tab_ptr;
} app_t;

static app_t g_app_instance = {
    .config = {
        .p_config_file      =   DEFAULT_CONFIG_FILE,
        .p_output_file      =   DEFAULT_OUTPUT_FILE,
    },

    .b_query_success     =   0,
};

static int jsoneq(const char *json, jsmntok_t *tok, const char *s) {
  if (tok->type == JSMN_STRING && (int)strlen(s) == tok->end - tok->start &&
      strncmp(json + tok->start, s, tok->end - tok->start) == 0) {
    return 0;
  }
  return -1;
}

app_t *app_get_instance(void)
{
    return &g_app_instance;
}

void app_print_usage(int32_t argc, char **argv)
{
    LOGS("\nUsage: %s [OPTION]", argv[0]);
    LOGS(" -h, --help               : show help info");
    LOGS(" -c, --configFile         : config file (intput), default is '%s'", DEFAULT_CONFIG_FILE);
    LOGS(" -o, --outputFile         : certificate directory (output), default is '%s'", DEFAULT_OUTPUT_FILE);
    LOGS("\nExample:");
    LOGS("    %s --configFile %s --outputFile %s", argv[0], DEFAULT_CONFIG_FILE, DEFAULT_OUTPUT_FILE);
}

int32_t app_parse_args(app_config_t *p_config, int32_t argc, char **argv)
{
    const char* av_short_option = "hc:o:";
    const struct option av_long_option[] = {
        {"help",            0, NULL, 'h'},
        {"configFile",      1, NULL, 'c'},
        {"outputFile",      1, NULL, 'o'},
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
        case 'c': {
            p_config->p_config_file = optarg;
        } break;
        case 'o': {
            p_config->p_output_file = optarg;
        } break;
        default:{
            rval = -1;
            LOGS("%s parse cmd param: %s error.", TAG_APP, argv[optidx]);
            goto EXIT;
        }
        }
    }

    // check key parameters
    if (!p_config->p_config_file || strcmp(p_config->p_config_file, "") == 0) {
        rval = -1;
        LOGE("%s config path is empty", TAG_APP);
        goto EXIT;
    }

    rval = access(p_config->p_config_file, F_OK | R_OK);
    if (rval != 0) {
        LOGE("%s load config failed, path:%s", TAG_APP, p_config->p_config_file);
        goto EXIT;
    }

EXIT:
    return rval;
}

int32_t app_load_license_file(const char *path, char *buf, uint32_t *buf_len)
{
    int32_t rval = 0;

    if (*buf_len <= 0) {
        LOGE("%s: invalid buffer length %d", TAG_APP, *buf_len);
        goto EXIT;
    }

    int32_t fd = open(path, O_RDONLY);
    if (fd == INVALID_FD) {
        rval = -1;
        LOGE("%s: open file failed, path=%s", TAG_APP, path);
        goto EXIT;
    }

    rval = read(fd, buf, *buf_len);
    if (rval <= 0) {
        LOGE("%s: read file failed, path=%s rval=%d", TAG_APP, path, rval);
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

int32_t app_save_file(const char *path, char *buffer, uint32_t *buf_len)
{
    int32_t rval = 0;

    if (*buf_len <= 0) {
        LOGE("%s invalid buffer length:%d", TAG_APP, *buf_len);
        goto EXIT;
    }

    int32_t fd = open(path, O_CREAT|O_RDWR, S_IRUSR|S_IWUSR);
    if (fd == INVALID_FD) {
        rval = -1;
        LOGE("%s open file failed, path=%s", TAG_APP, path);
        goto EXIT;
    }

    *buf_len = write(fd, buffer, *buf_len);
    if (*buf_len <= 0) {
        rval = -1;
        LOGE("%s write file failed, path=%s rval=%d", TAG_APP, path, rval);
        goto EXIT;
    }

EXIT:
    if (fd > 0) {
        close(fd);
        fd = INVALID_FD;
    }
    return rval;
}

static int32_t app_init(app_t *p_app)
{
    // 0. load config.json
    // 1. check parameter

    app_config_t *p_config = &p_app->config;

    char buffer[MAX_BUF_LEN];
    uint32_t buffer_len = MAX_BUF_LEN;
    int32_t rval = app_load_license_file(p_config->p_config_file, buffer, &buffer_len);
    if (rval < 0) {
        LOGE("%s load license config failed, path=%s rval=%d", TAG_APP, p_config->p_config_file, rval);
        goto EXIT;
    }

    // json init
    jsmntok_t tokens[DEFAULT_JSON_TOKENS_MAX_COUNT];
    jsmn_init(&p_app->json_parser);
    rval = jsmn_parse(&p_app->json_parser, buffer, buffer_len, tokens, DEFAULT_JSON_TOKENS_MAX_COUNT);
    if (rval <= 0) {
        LOGE("%s parse config failed, token_num=%d", TAG_APP, rval);
        goto EXIT;
    }

    int32_t token_num = rval;
    if (token_num < 1 || tokens[0].type != JSMN_OBJECT) {
        rval = -1;
        LOGE("%s parse json failed", TAG_APP);
        goto EXIT;
    }

    int32_t str_len = 0;
    for (int32_t i = 1; i < token_num; i++) {
        if (jsoneq(buffer, &tokens[i], "appId") == 0) {
            i++;
            str_len = tokens[i].end - tokens[i].start;
            memcpy(p_app->str_appid, buffer + tokens[i].start, str_len);
            p_app->str_appid[str_len] = '\0';
        } else if (jsoneq(buffer, &tokens[i], "customerKey") == 0) {
            i++;
            str_len = tokens[i].end - tokens[i].start;
            memcpy(p_app->str_customer_key, buffer + tokens[i].start, str_len);
            p_app->str_customer_key[str_len] = '\0';
        } else if (jsoneq(buffer, &tokens[i], "customerSecret") == 0) {
            i++;
            str_len = tokens[i].end - tokens[i].start;
            memcpy(p_app->str_customer_secret, buffer + tokens[i].start, str_len);
            p_app->str_customer_secret[str_len] = '\0';
        } else {
            // ignore other Key-Value
        }
    }

    if (strcmp(p_app->str_appid, "") == 0
     || strstr(p_app->str_appid, "YOUR") != NULL
     || strstr(p_app->str_appid, "APPID") != NULL) {
        rval = -1;
        LOGE("invalid appid:%s", p_app->str_appid);
        goto EXIT;
    }

    if (strcmp(p_app->str_customer_key, "") == 0
     || strstr(p_app->str_customer_key, "YOUR") != NULL
     || strstr(p_app->str_customer_key, "KEY") != NULL) {
        rval = -1;
        LOGE("invalid customer key:%s", p_app->str_customer_key);
        goto EXIT;
    }

    if (strcmp(p_app->str_customer_secret, "") == 0
     || strstr(p_app->str_customer_secret, "YOUR") != NULL
     || strstr(p_app->str_customer_secret, "SECRET") != NULL) {
        rval = -1;
        LOGE("invalid customer secret:%s", p_app->str_customer_secret);
        goto EXIT;
    }

    p_app->http_buffer_size = DEFAULT_HTTP_BUFFER_SIZE;
    p_app->http_buffer_len = 0;
    p_app->http_buffer_ptr = (char*)malloc(p_app->http_buffer_size);
    if (!p_app->http_buffer_ptr) {
        rval = -1;
        LOGE("alloc http buffer failed, size:%d", p_app->http_buffer_size);
        goto EXIT;
    }

    p_app->http_body_size = DEFAULT_HTTP_BODY_SIZE;
    p_app->http_body_len = 0;
    p_app->http_body_ptr = (char*)malloc(p_app->http_body_size);
    if (!p_app->http_body_ptr) {
        rval = -1;
        LOGE("alloc http body failed, size:%d", p_app->http_body_size);
        goto EXIT;
    }

    p_app->json_token_size = DEFAULT_JSON_TOKENS_MAX_COUNT;
    p_app->json_token_ptr = (jsmntok_t*)malloc(p_app->json_token_size * sizeof(jsmntok_t));
    if (!p_app->json_token_ptr) {
        rval = -1;
        LOGE("alloc json token failed, size:%d", p_app->json_token_size);
        goto EXIT;
    }

    p_app->result_license_tab_count = 0;
    p_app->result_license_tab_ptr = NULL;

EXIT:
    return rval;
}

static void app_deinit(app_t *p_app)
{
    if (p_app->http_buffer_ptr) {
        free(p_app->http_buffer_ptr);
        p_app->http_buffer_ptr = NULL;
    }
    p_app->http_buffer_size = 0;
    p_app->http_buffer_len = 0;

    if (p_app->http_body_ptr) {
        free(p_app->http_body_ptr);
        p_app->http_body_ptr = NULL;
    }
    p_app->http_body_size = 0;
    p_app->http_body_len = 0;

    if (p_app->json_token_ptr) {
        free(p_app->json_token_ptr);
        p_app->json_token_ptr = NULL;
    }
    p_app->json_token_size = 0;

    if (p_app->result_license_tab_ptr) {
        free(p_app->result_license_tab_ptr);
        p_app->result_license_tab_ptr = NULL;
    }
    p_app->result_license_tab_count = 0;
}

static int32_t app_parse_response(app_t *p_app) {
    jsmntok_t *token_tab_ptr = p_app->json_token_ptr;
    int32_t   token_tab_size = p_app->json_token_size;
    jsmn_init(&p_app->json_parser);
    int32_t rval = jsmn_parse(&p_app->json_parser,
                            p_app->http_body_ptr, p_app->http_body_len,
                            token_tab_ptr, token_tab_size);
    if (rval <= 0) {
        LOGE("%s parse config failed, token_num=%d", TAG_APP, rval);
        goto EXIT;
    }

    int32_t token_num = rval;
    if (token_num < 1 || token_tab_ptr[0].type != JSMN_OBJECT) {
        rval = -1;
        LOGE("%s parse json failed", TAG_APP);
        goto EXIT;
    }

    char str_buffer[MAX_BUF_LEN];
    int32_t str_buffer_len = 0;
    for (int32_t i = 1; i < token_num; i++) {
        if (jsoneq(p_app->http_body_ptr, &token_tab_ptr[i], "totalSize") == 0) {
            i++;
            str_buffer_len = token_tab_ptr[i].end - token_tab_ptr[i].start;
            memcpy(str_buffer, p_app->http_body_ptr + token_tab_ptr[i].start, str_buffer_len);
            str_buffer[str_buffer_len] = '\0';

            p_app->result_license_tab_count = atoi (str_buffer);
            if (p_app->result_license_tab_count == 0) {
                goto EXIT;
            }

            int32_t size = p_app->result_license_tab_count * sizeof(app_license_entry_t);
            p_app->result_license_tab_ptr = (app_license_entry_t*)malloc(size);
            memset(p_app->result_license_tab_ptr, 0, size);
        } else if (jsoneq(p_app->http_body_ptr, &token_tab_ptr[i], "elements") == 0) {
            i++;
            if (token_tab_ptr[i].type != JSMN_ARRAY) {
                continue;
            }

            if (p_app->result_license_tab_count != token_tab_ptr[i].size) {
                rval = -1;
                LOGE("%s inconsistent license count, totalSize:%d elementSize:%d",
                    TAG_APP, p_app->result_license_tab_count, token_tab_ptr[i].size);
                goto EXIT;
            }

            enum {
                OBJ                                         =   0,
                OBJ_CUSTOM_KEY                              =   1,
                OBJ_CUSTOM_VALUE                            =   2,
                OBJ_RECORDS_KEY                             =   3,
                OBJ_RECORDS_ARRAY                           =   4,
                OBJ_RECORDS_ARRAY_OBJ_0                     =   5,
                OBJ_RECORDS_ARRAY_OBJ_0_CREATE_TIME_KEY     =   6,
                OBJ_RECORDS_ARRAY_OBJ_0_CREATE_TIME_VALUE   =   7,
                OBJ_RECORDS_ARRAY_OBJ_0_VALID_PERIOD_KEY    =   8,
                OBJ_RECORDS_ARRAY_OBJ_0_VALID_PERIOD_VALUE  =   9,
                OBJ_RECORDS_ARRAY_OBJ_0_ACTIVATE_TIME_KEY   =   10,
                OBJ_RECORDS_ARRAY_OBJ_0_ACTIVATE_TIME_VALUE =   11,
                OBJ_RECORDS_ARRAY_OBJ_0_ACTIVATED_KEY       =   12,
                OBJ_RECORDS_ARRAY_OBJ_0_ACTIVATED_VALUE     =   13,
                OBJ_COUNT                                   =   14,
            };

            for (int32_t j = 0; j < p_app->result_license_tab_count; j++) {
                app_license_entry_t *p_entry = p_app->result_license_tab_ptr + j;

                // license key
                jsmntok_t *p_token = &token_tab_ptr[i + 1 + j*OBJ_COUNT + OBJ_CUSTOM_VALUE];
                memcpy(p_entry->str_license_key,
                    p_app->http_body_ptr + p_token->start,
                    p_token->end - p_token->start);

                // create time
                p_token = &token_tab_ptr[i + 1 + j*OBJ_COUNT + OBJ_RECORDS_ARRAY_OBJ_0_CREATE_TIME_VALUE];
                str_buffer_len = p_token->end - p_token->start;  // ignore ms
                memcpy(str_buffer, p_app->http_body_ptr + p_token->start, str_buffer_len);
                str_buffer[str_buffer_len] = '\0';
                if (strcmp(str_buffer, "null") != 0) {
                    time_t time = strtol(str_buffer, NULL, 10) / 1000;
                    struct tm  *local = localtime(&time);
                    strftime(p_entry->str_create_time, sizeof(p_entry->str_create_time), "%Y-%m-%d %H:%M:%S", local);
                } else {
                    strcpy(p_entry->str_create_time, str_buffer);
                }

                // valid period
                p_token = &token_tab_ptr[i + 1 + j*OBJ_COUNT + OBJ_RECORDS_ARRAY_OBJ_0_VALID_PERIOD_VALUE];
                str_buffer_len = p_token->end - p_token->start;
                memcpy(p_entry->str_valid_period, p_app->http_body_ptr + p_token->start, str_buffer_len);
                p_entry->str_valid_period[str_buffer_len] = '\0';

                // activate time
                p_token = &token_tab_ptr[i + 1 + j*OBJ_COUNT + OBJ_RECORDS_ARRAY_OBJ_0_ACTIVATE_TIME_VALUE];
                str_buffer_len = p_token->end - p_token->start;
                memcpy(str_buffer, p_app->http_body_ptr + p_token->start, str_buffer_len);
                str_buffer[str_buffer_len] = '\0';
                if (strcmp(str_buffer, "null") != 0) {
                    time_t time = strtol(str_buffer, NULL, 10) / 1000;
                    struct tm *local = localtime(&time);
                    strftime(p_entry->str_activate_time, sizeof(p_entry->str_activate_time), "%Y-%m-%d %H:%M:%S", local);
                } else {
                    strcpy(p_entry->str_activate_time, str_buffer);
                }

                // activated
                p_token = &token_tab_ptr[i + 1 + j*OBJ_COUNT + OBJ_RECORDS_ARRAY_OBJ_0_ACTIVATED_VALUE];
                str_buffer_len = p_token->end - p_token->start;
                memcpy(p_entry->str_activated, p_app->http_body_ptr + p_token->start, str_buffer_len);
                p_entry->str_activated[str_buffer_len] = '\0';
            }

            i += p_app->result_license_tab_count * OBJ_COUNT;
        } else {
            i++;
        }
    }

EXIT:
    return 0;
}

static int32_t _http_on_body(http_parser *p_parser, const char *at, size_t length)
{
    app_t *p_app = (app_t *)p_parser->data;

    if (length > 0) {
        memcpy(p_app->http_body_ptr + p_app->http_body_len, at, length);
        p_app->http_body_len += length;
    }
    return 0;
}

int32_t base64_encode(const char *src, int32_t src_len, char *encoded)
{
    const char basis_64[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

    int i;
    char *p = encoded;
    for (i = 0; i < src_len - 2; i += 3) {
        *p++ = basis_64[(src[i] >> 2) & 0x3F];
        *p++ = basis_64[((src[i] & 0x3) << 4) |
                        ((src[i + 1] & 0xF0) >> 4)];
        *p++ = basis_64[((src[i + 1] & 0xF) << 2) |
                        ((src[i + 2] & 0xC0) >> 6)];
        *p++ = basis_64[src[i + 2] & 0x3F];
    }
    if (i < src_len) {
        *p++ = basis_64[(src[i] >> 2) & 0x3F];
        if (i == (src_len - 1)) {
            *p++ = basis_64[((src[i] & 0x3) << 4)];
            *p++ = '=';
        } else {
            *p++ = basis_64[((src[i] & 0x3) << 4) |
                            ((src[i + 1] & 0xF0) >> 4)];
            *p++ = basis_64[((src[i + 1] & 0xF) << 2)];
        }
        *p++ = '=';
    }

    *p++ = '\0';
    return (p - encoded);
}

int32_t app_query_license(app_t *p_app)
{
    // License Query Flow
    // Reference Link
    // https://docs-preview.agoralab.co/cn/Agora%20Platform/license_mechanism_v3?platform=All%20Platforms

    app_config_t *p_config = &p_app->config;
    struct addrinfo *p_addinfo = NULL;

    struct addrinfo hints;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_CANONNAME;
    hints.ai_protocol = 0;

    char str_hostname[] = "api.agora.io";
    int32_t rval = getaddrinfo(str_hostname, NULL, &hints, &p_addinfo);
    if(rval != 0) {
        LOGE("%s dns parser failed", TAG_APP);
        goto EXIT;
    }

    char str_ip[129] = "";
    for (struct addrinfo *res_p = p_addinfo; res_p != NULL; res_p = res_p->ai_next) {
        rval = getnameinfo(res_p->ai_addr, res_p->ai_addrlen, str_ip, sizeof(str_ip), NULL, 0, NI_NUMERICHOST);
        if(rval == 0) {
            break;
        }
    }

    if (strcmp(str_ip, "") == 0) {
        rval = -1;
        LOGE("%s dns parser can't get valid ip", TAG_APP);
        goto EXIT;
    }

    int32_t sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(80);
    inet_pton(AF_INET, str_ip, &serv_addr.sin_addr);

    rval = connect(sock_fd, (struct sockaddr*) &serv_addr, sizeof(serv_addr));
    if (rval < 0) {
        LOGE("%s connect server failed, hostname:%s ip:%s", TAG_APP, str_hostname, str_ip);
        goto EXIT;
    }

    char str_authorization[MAX_BUF_LEN];
    char str_authorization_base64[MAX_BUF_LEN];
    int32_t str_authorization_base64_len = 0;
    snprintf(str_authorization, MAX_BUF_LEN, "%s:%s", p_app->str_customer_key, p_app->str_customer_secret);
    base64_encode(str_authorization, strlen(str_authorization), str_authorization_base64);

    p_app->http_buffer_len = snprintf(
                                p_app->http_buffer_ptr, p_app->http_buffer_size,
                                "GET /dev/v3/apps/%s/licenses HTTP/1.1\r\n"
                                "Authorization: Basic %s\r\n"
                                "Host: %s\r\n"
                                "Accept: */*\r\n"
                                "\r\n",
                                p_app->str_appid, str_authorization_base64, str_hostname);
    // LOGS("SEND len=%d\n%s", p_app->http_buffer_len, p_app->http_buffer_ptr);
    rval = write(sock_fd, p_app->http_buffer_ptr, p_app->http_buffer_len);
    if (rval <= 0) {
        rval = -1;
        LOGE("%s send license activate request failed", TAG_APP);
        goto EXIT;
    }

    // read loop
    p_app->http_buffer_len = 0;
    while (1) {
        fd_set rd_set;
        FD_ZERO(&rd_set);
        FD_SET(sock_fd, &rd_set);

        struct timeval timeout = { 0 };
        timeout.tv_sec = 1;
        timeout.tv_usec = 0;

        rval = select(sock_fd + 1, &rd_set, NULL, NULL, &timeout);
        if (rval <= 0) {
            break;
        }

        rval = read(sock_fd, p_app->http_buffer_ptr + p_app->http_buffer_len, p_app->http_buffer_size);
        if (rval <= 0) {
            break;
        }
        p_app->http_buffer_len += rval;
    }

    if (p_app->http_buffer_len <= 0) {
        rval = -1;
        LOGE("%s recv response failed", TAG_APP);
        goto EXIT;
    }
    p_app->http_buffer_ptr[p_app->http_buffer_len] = '\0';
    // LOGS("RECV len=%d\n%s", p_app->http_buffer_len, p_app->http_buffer_ptr);

    http_parser_settings http_settings = { 0 };
    http_settings.on_body = _http_on_body;

    p_app->http_parser.data = (void*)p_app;
    http_parser_init(&p_app->http_parser, HTTP_RESPONSE);
    http_parser_execute(&p_app->http_parser, &http_settings, p_app->http_buffer_ptr, p_app->http_buffer_len);

    rval = app_parse_response(p_app);
    if (rval < 0) {
        rval = -1;
        LOGE("%s parse http reponse failed", TAG_APP);
        goto EXIT;
    }

    LOGS("-----------------------------------  License Status  ----------------------------------");
    LOGS("| total: %-5.5d                                                                         |", p_app->result_license_tab_count);
    LOGS("---------------------------------------------------------------------------------------");
    if (p_app->result_license_tab_count > 0) {
        LOGS("| %-16.16s | %-16.16s | %-16.16s | %-8.8s | %-16.16s |", "license key", "create time", "validity (day)", "activated", "activate time");
        LOGS("---------------------------------------------------------------------------------------");
        for (int32_t i = 0; i < p_app->result_license_tab_count; i++) {
            app_license_entry_t *p_entry = p_app->result_license_tab_ptr + i;
            LOGS("| %-16.16s | %-16.16s | %-16.16s | %-8.8s | %-16.16s |",
                p_entry->str_license_key,
                p_entry->str_create_time,
                p_entry->str_valid_period,
                p_entry->str_activated,
                p_entry->str_activate_time);
        }
        LOGS("---------------------------------------------------------------------------------------");
    }

    // rval = app_save_file(p_config->p_output_file, p_app->str_result, &p_app->str_result_len);
    // if (rval != 0) {
    //     LOGE("%s save credential failed, path=%s", TAG_APP, p_config->p_output_file);
    //     goto EXIT;
    // }

EXIT:
    if (p_addinfo) {
        freeaddrinfo(p_addinfo);
    }

    if (sock_fd > 0) {
        close(sock_fd);
        sock_fd = INVALID_FD;
    }
    return rval;
}

int main(int argc, char **argv)
{
    app_t *p_app = app_get_instance();

    // 0. app parse args
    int32_t rval = app_parse_args(&p_app->config, argc, argv);
    if (rval != 0) {
        app_print_usage(argc, argv);
        goto EXIT;
    }

    // 1. app init
    rval = app_init(p_app);
    if (rval < 0) {
        LOGE("%s init failed, rval=%d", TAG_APP, rval);
        goto EXIT;
    }

    // 2. license query
    app_query_license(p_app);

EXIT:
    // 3. app deinit
    app_deinit(p_app);
    return rval;
}
