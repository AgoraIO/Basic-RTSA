/*************************************************************
 * File  :  license_activator.c
 * Module:  Agora SD-RTN SDK RTC C API license activator.
 *
 * This is a tool to activate license Agora RTC Service SDK.
 * Copyright (C) 2020 Agora IO
 * All rights reserved.
 *
 *************************************************************/

#include <stdio.h>
#include <string.h>

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
#include "agora_rtc_api.h"

#define LOGS(fmt, ...)  fprintf(stdout, "" fmt "\n", ##__VA_ARGS__)
#define LOGI(fmt, ...)  fprintf(stdout, "I/ " fmt " at %s:%u\n", ##__VA_ARGS__, __PRETTY_FUNCTION__, __LINE__)
#define LOGD(fmt, ...)  fprintf(stdout, "D/ " fmt " at %s:%u\n", ##__VA_ARGS__, __PRETTY_FUNCTION__, __LINE__)
#define LOGE(fmt, ...)  fprintf(stdout, "E/ " fmt " at %s:%u\n", ##__VA_ARGS__, __PRETTY_FUNCTION__, __LINE__)
#define LOGW(fmt, ...)  fprintf(stdout, "W/ " fmt " at %s:%u\n", ##__VA_ARGS__, __PRETTY_FUNCTION__, __LINE__)

#define TAG_APP     "[app]"

#define CERTIFACTE_FILENAME         "certificate.bin"
#define CREDENTIAL_FILENAME         "deviceID.bin"

#define INVALID_FD                  -1
#define MAX_TOKENS_COUNT            16
#define MAX_BUF_LEN                 1024
#define HTTP_MAX_BUF_LEN            4096
#define APPID_MAX_LEN               64
#define CUSTOMER_KEY_MAX_LEN        64
#define CUSTOMER_SECRET_MAX_LEN     64
#define LICENSE_KEY_MAX_LEN         128

#define CREDENTIAL_MAX_LEN          256
#define CERTIFICATE_MAX_LEN         2048

typedef struct {
    const char  *p_config_file;
    const char  *p_certificate_dir;
} app_config_t;

typedef struct {
    app_config_t    config;
    jsmn_parser     json_parser;
    http_parser     http_parser;

    // input: load from config license.cfg
    char            str_appid[APPID_MAX_LEN];
    char            str_customer_key[CUSTOMER_KEY_MAX_LEN];
    char            str_customer_secret[CUSTOMER_SECRET_MAX_LEN];
    char            str_license_key[LICENSE_KEY_MAX_LEN];

    // output: save certificate & credential
    char            str_credential[CREDENTIAL_MAX_LEN];
    uint32_t        str_credential_len;
    char            str_certificate[CERTIFICATE_MAX_LEN];
    uint32_t        str_certificate_len;

    int32_t         b_activate_success;
    char            str_activate_result[MAX_BUF_LEN];
} app_t;

static app_t g_app_instance = {
    .config = {
        .p_config_file      =   "./license.cfg",
        .p_certificate_dir =   ".",
    },

    .str_credential_len     =   CREDENTIAL_MAX_LEN,
    .str_certificate_len    =   CERTIFICATE_MAX_LEN,
    .b_activate_success     =   0,
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
    LOGS(" -c, --configFile         : config file (intput), default is ./license.cfg");
    LOGS(" -o, --certOutputDir      : certificate directory (output), default is '.'");
    LOGS("\nExample:");
    LOGS("    %s --configFile ./license.cfg --certOutputDir .", argv[0]);
}

int32_t app_parse_args(app_config_t *p_config, int32_t argc, char **argv)
{
    const char* av_short_option = "hc:o:";
    const struct option av_long_option[] = {
        {"help",            0, NULL, 'h'},
        {"configFile",      1, NULL, 'c'},
        {"certOutputDir",   1, NULL, 'o'},
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
            p_config->p_certificate_dir = optarg;
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

int32_t app_save_license_file(const char *path, char *buffer, uint32_t *buf_len)
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
    jsmntok_t tokens[MAX_TOKENS_COUNT];
    jsmn_init(&p_app->json_parser);
    rval = jsmn_parse(&p_app->json_parser, buffer, buffer_len, tokens, MAX_TOKENS_COUNT);
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
        } else if (jsoneq(buffer, &tokens[i], "licenseKey") == 0) {
            i++;
            str_len = tokens[i].end - tokens[i].start;
            memcpy(p_app->str_license_key, buffer + tokens[i].start, str_len);
            p_app->str_license_key[str_len] = '\0';
        } else {
            rval = -1;
            LOGE("Unexpected key: %.*s\n", tokens[i].end - tokens[i].start, buffer + tokens[i].start);
            goto EXIT;
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

    if (strcmp(p_app->str_license_key, "") == 0
     || strstr(p_app->str_license_key, "YOUR") != NULL
     || strstr(p_app->str_license_key, "KEY") != NULL) {
        rval = -1;
        LOGE("invalid licnese key:%s", p_app->str_license_key);
        goto EXIT;
    }

EXIT:
    return rval;
}

static int32_t _http_on_body(http_parser *p_parser, const char *at, size_t length)
{
    app_t *p_app = (app_t *)p_parser->data;

    jsmntok_t tokens[MAX_TOKENS_COUNT];
    jsmn_init(&p_app->json_parser);
    int32_t rval = jsmn_parse(&p_app->json_parser, at, length, tokens, MAX_TOKENS_COUNT);
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
        if (jsoneq(at, &tokens[i], "cert") == 0) {
            i++;
            str_len = tokens[i].end - tokens[i].start;
            memcpy(p_app->str_certificate, at + tokens[i].start, str_len);
            p_app->str_certificate[str_len] = '\0';
            p_app->str_certificate_len = str_len;
            p_app->b_activate_success = 1;
        } else {
            p_app->b_activate_success = 0;
            memcpy(p_app->str_activate_result, at, length);
            p_app->str_activate_result[length] = '\0';
            break;
        }
    }

EXIT:
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

int32_t app_activate_license(app_t *p_app)
{
    // License Activation Flow
    // Reference Link
    // https://docs-preview.agoralab.co/cn/Agora%20Platform/license_mechanism_v3?platform=All%20Platforms

    app_config_t *p_config = &p_app->config;
    struct addrinfo *p_addinfo = NULL;

    int32_t rval = agora_rtc_license_gen_credential(p_app->str_credential, &p_app->str_credential_len);
    if (rval != 0) {
        rval = -1;
        LOGE("generate credential failed, license_key=%s", p_app->str_license_key);
        LOGS("!!! please check whether your SDK supports license feature");
        goto EXIT;
    }
    LOGS("%s generate credential\nlen:%d\nstr:%s", TAG_APP, p_app->str_credential_len, p_app->str_credential);

    struct addrinfo hints;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_CANONNAME;
    hints.ai_protocol = 0;

    char str_hostname[] = "api.agora.io";
    rval = getaddrinfo(str_hostname, NULL, &hints, &p_addinfo);
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

    char str_page[MAX_BUF_LEN];
    snprintf(str_page, MAX_BUF_LEN, "/dev/v3/apps/%s/licenses/activation", p_app->str_appid);

    char str_post[MAX_BUF_LEN];
    int32_t str_post_len = snprintf(str_post, MAX_BUF_LEN,
                                    "{ \"credential\": \"%s\", \"custom\": \"%s\" }",
                                    p_app->str_credential, p_app->str_license_key);

    char str_authorization[MAX_BUF_LEN];
    char str_authorization_base64[MAX_BUF_LEN];
    int32_t str_authorization_base64_len = 0;
    snprintf(str_authorization, MAX_BUF_LEN, "%s:%s", p_app->str_customer_key, p_app->str_customer_secret);
    base64_encode(str_authorization, strlen(str_authorization), str_authorization_base64);

    char http_buffer[HTTP_MAX_BUF_LEN];
    int32_t http_buffer_len = snprintf(http_buffer, HTTP_MAX_BUF_LEN,
                            "POST %s HTTP/1.1\r\n"
                            "Authorization: Basic %s\r\n"
                            "Host: %s\r\n"
                            "Content-type: application/json\r\n"
                            "Accept: */*\r\n"
                            "Content-length: %d\r\n\r\n"
                            "%s",
                            str_page, str_authorization_base64, str_hostname, str_post_len, str_post);
    // LOGS("SEND len=%d\n%s", http_buffer_len, http_buffer);

    rval = write(sock_fd, http_buffer, http_buffer_len);
    if (rval <= 0) {
        rval = -1;
        LOGE("%s send license activate request failed", TAG_APP);
        goto EXIT;
    }

    fd_set rd_set;
    FD_ZERO(&rd_set);
    FD_SET(sock_fd, &rd_set);

    struct timeval timeout = { 0 };
    timeout.tv_sec = 1;
    timeout.tv_usec = 0;

    rval = select(sock_fd + 1, &rd_set, NULL, NULL, &timeout);
    if (rval == -1) {
        LOGE("%s wait http response timeout, rval=%d", TAG_APP, rval);
        goto EXIT;
    }

    rval = read(sock_fd, http_buffer, HTTP_MAX_BUF_LEN);
    if (rval <= 0) {
        rval = -1;
        LOGE("%s recv license activate request failed", TAG_APP);
        goto EXIT;
    }
    http_buffer_len = rval;
    http_buffer[http_buffer_len] = '\0';
    // LOGS("RECV len=%d\n%s", http_buffer_len, http_buffer);

    http_parser_settings http_activation_settings = { 0 };
    http_activation_settings.on_body = _http_on_body;

    p_app->http_parser.data = (void*)p_app;
    http_parser_init(&p_app->http_parser, HTTP_RESPONSE);
    http_parser_execute(&p_app->http_parser, &http_activation_settings, http_buffer, http_buffer_len);
    if (!p_app->b_activate_success) {
        rval = -1;
        LOGE("%s activate failed", TAG_APP);
        LOGE("%s result: %s", TAG_APP, p_app->str_activate_result);
        goto EXIT;
    }
    LOGS("%s activate success", TAG_APP);

    char str_file[MAX_BUF_LEN];
    snprintf(str_file, MAX_BUF_LEN, "%s/%s", p_config->p_certificate_dir, CREDENTIAL_FILENAME);
    rval = app_save_license_file(str_file, p_app->str_credential, &p_app->str_credential_len);
    if (rval != 0) {
        LOGE("%s save credential failed, path=%s", TAG_APP, str_file);
        goto EXIT;
    }
    LOGS("Generated credential is saved at:  %s", str_file);

    snprintf(str_file, MAX_BUF_LEN, "%s/%s", p_config->p_certificate_dir, CERTIFACTE_FILENAME);
    rval = app_save_license_file(str_file, p_app->str_certificate, &p_app->str_certificate_len);
    if (rval != 0) {
        LOGE("%s save certificate failed, path=%s", TAG_APP, str_file);
        goto EXIT;
    }
    LOGS("Generated certificate is saved at: %s", str_file);

    LOGS("\n--------------  IMPORTANT HINT  ---------------");
    LOGS("!!! Please keep your own unique credential");
    LOGS("!!! If credential is lost, it cannot be retrieved");
    LOGS("-----------------------------------------------");

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

    // 2. license activate
    app_activate_license(p_app);

EXIT:
    return rval;
}
