/*************************************************************
 *
 * This is a part of the Agora RTC Service SDK.
 * Copyright (C) 2020 Agora IO
 * All rights reserved.
 *
 *************************************************************/

#ifndef __AGORA_RTC_API_H__
#define __AGORA_RTC_API_H__

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_MSC_VER)
#if defined(__BUILDING_AGORA_SDK__)
#define __agora_api__ __declspec(dllexport)
#else
#define __agora_api__ __declspec(dllimport)
#endif
#else
#define __agora_api__
#endif

#define AGORA_TOKEN_MAX_LEN             512
#define AGORA_CMD_PARAM_MAX_LEN         256
#define AGORA_CREDENTIAL_MAX_LEN        256
#define AGORA_CERTIFICATE_MAX_LEN       1024

/** Error code. */
typedef enum {
    /** No error. */
    ERR_OK = 0,

    /** General error */
    ERR_FAILED = 1,

    /**
     * Network is unavailable
     */
    ERR_NET_DOWN = 14,

    /**
     * Request to join channel is rejected.
     * It occurs when local user is already in channel and try to join the same channel again.
     */
    ERR_JOIN_CHANNEL_REJECTED = 17,

    /** App ID is invalid. */
    ERR_INVALID_APP_ID = 101,

    /** Channel is invalid. */
    ERR_INVALID_CHANNEL_NAME = 102,

    /**
     * Server rejected request to look up channel.
     */
    ERR_LOOKUP_CHANNEL_REJECTED = 105,

    /**
     * Server rejected request to open channel.
     */
    ERR_OPEN_CHANNEL_REJECTED = 107,

    /**
     * Token expired due to reasons belows:
     * - Authorized Timestamp expired:      The timestamp is represented by the number of
     *                                      seconds elapsed since 1/1/1970. The user can use
     *                                      the Token to access the Agora service within five
     *                                      minutes after the Token is generated. If the user
     *                                      does not access the Agora service after five minutes,
     *                                      this Token will no longer be valid.
     * - Call Expiration Timestamp expired: The timestamp indicates the exact time when a
     *                                      user can no longer use the Agora service (for example,
     *                                      when a user is forced to leave an ongoing call).
     *                                      When the value is set for the Call Expiration Timestamp,
     *                                      it does not mean that the Token will be expired,
     *                                      but that the user will be kicked out of the channel.
     */
    ERR_TOKEN_EXPIRED = 109,

    /**
     * Token is invalid due to reasons belows:
     * - If application certificate is enabled on the Dashboard,
     *   valid token SHOULD be set when invoke.
     *
     * - If uid field is mandatory, and users must set the same uid when setting the
     *   uid parameter when calling `agora_rtc_join_channel`.
     */
    ERR_INVALID_TOKEN = 110,

    /** Switching roles failed.
     *  Please try to rejoin the channel.
     */
    ERR_SET_CLIENT_ROLE_NOT_AUTHORIZED = 119,

    /** Ticket to open channel is invalid */
    ERR_OPEN_CHANNEL_INVALID_TICKET = 121,

    /** Try another server. */
    ERR_OPEN_CHANNEL_TRY_NEXT_VOS = 122,

    /** Client is banned by the server */
    ERR_CLIENT_IS_BANNED_BY_SERVER = 123,
} agora_err_code_e;

/**
 * The definition of the user_offline_reason_e enum.
 */
typedef enum {
    /**
     * 0: Remote user leaves channel actively
     */
    USER_OFFLINE_QUIT     = 0,
    /**
     * 1: Remote user is dropped due to timeout
     */
    USER_OFFLINE_DROPPED  = 1,
} user_offline_reason_e;

/**
 * The definition of the video_codec_e enum.
 */
typedef enum {
    /**
     * 2: h264
     */
    VIDEO_CODEC_H264     = 2,
    /**
     * 6: generic
     */
    VIDEO_CODEC_GENERIC  = 6,
} video_codec_e;

/**
 * The definition of the video_frame_type_e enum.
 */
typedef enum {
    /**
     * 1: key frame
     */
    VIDEO_FRAME_KEY    =  1,
    /*
     * 4: delta frame, e.g: P-Frame
     */
    VIDEO_FRAME_DELTA  =  4,
} video_frame_type_e;

/**
 * The definition of the video_frame_rate_e enum.
 */
typedef enum {
    /**
     * 1: 1 fps.
     */
    VIDEO_FRAME_RATE_FPS_1 = 1,
    /**
     * 7: 7 fps.
     */
    VIDEO_FRAME_RATE_FPS_7 = 7,
    /**
     * 10: 10 fps.
     */
    VIDEO_FRAME_RATE_FPS_10 = 10,
    /**
     * 15: 15 fps.
     */
    VIDEO_FRAME_RATE_FPS_15 = 15,
    /**
     * 24: 24 fps.
     */
    VIDEO_FRAME_RATE_FPS_24 = 24,
    /**
     * 30: 30 fps.
     */
    VIDEO_FRAME_RATE_FPS_30 = 30,
    /**
     * 60: 60 fps. Applies to Windows and macOS only.
     */
    VIDEO_FRAME_RATE_FPS_60 = 60,
} video_frame_rate_e;

/**
 * The definition of the video_frame_info_t struct.
 */
typedef struct {
    /**
     * The video codec: #video_codec_e.
     */
    video_codec_e  codec;
    /**
     * The frame type of the encoded video frame: #video_frame_type_e.
     */
    video_frame_type_e  type;
    /**
     * The number of video frames per second.
     * -This value will be used for calculating timestamps of the encoded image.
     * - If frame_per_sec equals zero, then real timestamp will be used.
     * - Otherwise, timestamp will be adjusted to the value of frame_per_sec set.
     */
    video_frame_rate_e frames_per_sec;
} video_frame_info_t;


/**
 * Audio codec type list.
 */
typedef enum {
    /**
     * 1: OPUS
     */
    AUDIO_CODEC_OPUS     = 1,
    /**
     * 5: G722
     */
    AUDIO_CODEC_G722     = 5,
    /**
     * 8: AACLC
     */
    AUDIO_CODEC_AACLC    = 8,
    /**
     * 9: HEAAC
     */
    AUDIO_CODEC_HEAAC    = 9,
    /**
     * 253: GENERIC
     */
     AUDIO_CODEC_GENERIC = 253,
} audio_codec_e;

/**
 * The definition of the audio_frame_info_t struct.
 */
typedef struct {
    /**
     * Audio codec type, reference #audio_codec_e.
     */
    audio_codec_e  codec;
} audio_frame_info_t;

/**
 * Agora RTC SDK event handler
 */
typedef struct {
    /**
     * Occur when local user joins channel successfully.
     *
     * @param[in] channel    Channel name
     * @param[in] elapsed_ms Time elapsed (ms) since channel is established
     */
    void (*on_join_channel_success)(const char *channel, int elapsed_ms);

    /**
     * Occur when channel is disconnected with server.
     *
     * @param[in] channel Channel name
     */
    void (*on_connection_lost)(const char *channel);

    /**
     * Occur when user rejoins channel successfully after disconnect
     *
     * When channel loses connection with server due to network problems,
     * SDK will retry to connect automatically. If success, it will be triggered.
     *
     * @param[in] channel    Channel name
     * @param[in] elapsed_ms Time elapsed (ms) since rejoin due to network
     */
    void (*on_rejoin_channel_success)(const char *channel, int elapsed_ms);

    /**
     * Report error message during runtime.
     *
     * In most cases, it means SDK can't fix the issue and application should take action.
     *
     * @param[in] channel Channel name
     * @param[in] code    Error code, see #agora_err_code_e
     * @param[in] msg     Error message
     */
    void (*on_error)(const char *channel, int code, const char *msg);

    /**
     * Occur when remote user joins channel successfully.
     *
     * @param[in] channel    Channel name
     * @param[in] uid        Remote user ID
     * @param[in] elapsed_ms Time elapsed (ms) since channel is established
     */
    void (*on_user_joined)(const char *channel, uint32_t uid, int elapsed_ms);

    /**
     * Occur when remote user leaves the channel.
     *
     * @param[in] channel Channel name
     * @param[in] uid     Remote user ID
     * @param[in] reason  Reason, see #user_offline_reason_e
     */
    void (*on_user_offline)(const char *channel, uint32_t uid, int reason);

    /**
     * Occur when a remote user sends notification before enable/disable sending audio.
     *
     * @param[in] channel Channel name
     * @param[in] uid     Remote user ID
     * @param[in] muted   Mute status:
     *                    - 0:        unmuted
     *                    - non-ZERO: muted
     */
    void (*on_user_mute_audio)(const char *channel, uint32_t uid, int muted);

    /**
     * Occur when a remote user sends notification before enable/disable sending video.
     *
     * @param[in] channel Channel name.
     * @param[in] uid     Remote user ID
     * @param[in] muted   Mute status:
     *                    - 0:        unmuted
     *                    - non-ZERO: muted
     */
    void (*on_user_mute_video)(const char *channel, uint32_t uid, int muted);

    /**
     * Occur when a remote user requests a keyframe.
     *
     * This callback notifies the sender to generate a new keyframe.
     *
     * @param[in] channel    Channel name
     * @param[in] uid        Remote user ID
     * @param[in] stream_id  Stream ID for which a keyframe is requested
     */
    void (*on_key_frame_gen_req)(const char *channel, uint32_t uid, uint8_t stream_id);

    /**
     * Occur when receiving the audio frame of a remote user in the channel.
     *
     * @param[in] channel    Channel name
     * @param[in] uid        Remote user ID to which data is sent
     * @param[in] sent_ts    Timestamp (ms) for sending data
     * @param[in] codec      Audio codec type
     * @param[in] data_ptr   Audio frame buffer
     * @param[in] data_len   Audio frame buffer length (bytes)
     */
    void (*on_audio_data)(const char *channel, uint32_t uid, uint16_t sent_ts, uint8_t codec,
                          const void *data_ptr, size_t data_len);

    /**
     * Occur when receiving the video frame of a remote user in the channel.
     *
     * @param[in] channel      Channel name
     * @param[in] uid          Remote user ID to which data is sent
     * @param[in] sent_ts      Timestamp (ms) for sending data
     * @param[in] codec        Video codec type
     * @param[in] stream_id    Video stream ID. Range is [0, 15]
     * @param[in] is_key_frame Frame type:
     *                          - 0:        non-keyframe
     *                          - non-ZERO: keyframe
     * @param[in] data_ptr     Video frame buffer
     * @param[in] data_len     Video frame buffer lenth (bytes)
     */
    void (*on_video_data)(const char *channel, uint32_t uid, uint16_t sent_ts, uint8_t codec,
                          uint8_t stream_id, int is_key_frame, const void *data_ptr, size_t data_len);

    /**
     * Occur when RDT(reliable data tunnel) availability changed.
     *
     * @note As an E2E tunnel, RDT is available only if there are just 2 users in a channel.
     *
     * @param[in] channel          Channel name
     * @param[in] is_available     Available status:
     *                             - 0:         unavailable
     *                             - non-ZERO:  available
     */
    void (*on_rdt_availability_changed)(const char *channel, int is_available);

    /**
     * Occur when command comes from reliable data channel.
     *
     * @param[in] channel    Channel name
     * @param[in] uid        Remote user ID
     * @param[in] cmd        Command value
     * @param[in] param_ptr  Parameter buffer, set NULL if non-exist
     * @param[in] param_len  Parameter buffer length in bytes if exist
     */
    void (*on_cmd)(const char *channel, uint32_t uid, int cmd, const void *param_ptr,
                   size_t param_len);

    /**
     * Occurs when data comes from reliable data channel.
     *
     * @param[in] channel      Channel name
     * @param[in] uid          Remote user ID
     * @param[in] data_ptr     Data buffer
     * @param[in] data_len     Data buffer length
     */
    void (*on_data_from_rdt)(const char *channel, uint32_t uid, const void *data_ptr, size_t data_len);

    /**
     * Advise application to update encoder bitrate.
     *
     * @param[in] channel    Channel name
     * @param[in] target_bps Target value (bps) by which the bitrate should update
     */
    void (*on_target_bitrate_changed)(const char *channel, uint32_t target_bps);

    /**
     * Occur when initialization completes with a given user name.
     *
     * @param[in] uname    Local user name provided on initialization
     * @param[in] uid      Local user ID allocated by server
     */
    void (*on_local_user_registered)(const char *uname, uint32_t uid);

    /**
     * Occur when remote user registered.
     *
     * @param[in] uname Remote user name
     * @param[in] uid   Remote user ID
     */
    void (*on_remote_user_registered)(const char *uname, uint32_t uid);
} agora_rtc_event_handler_t;

/**
 * Get SDK version.
 *
 * @return A const static string describes the SDK version
 */
extern const char *agora_rtc_get_version(void);

/**
 * Convert error code to const static string.
 *
 * @note You do not have to release the string after use.
 *
 * @param[in] err Error code
 *
 * @return Const static error string
 */
extern __agora_api__ const char *agora_rtc_err_2_str(int err);

/**
 * Generate a credential which is a unique device identifier.
 *
 * @note It's authorizing smart devices license.
 *       You can disregard it if license isn't used.
 *
 * @param[out]    credential        Credential buffer holding the generated data
 * @param[in,out] credential_len    Credential buffer length (bytes), which should be larger than AGORA_CREDENTIAL_MAX_LEN
 *
 * @return
 * - = 0: Success
 * - < 0: Failure
 */
extern __agora_api__ int agora_rtc_license_gen_credential(char *credential, unsigned int *credential_len);

/**
 * Authenticate the SDK licence.
 *
 * @note
 * - It's authorizing smart devices license.
 *   You can disregard it if you do not use a license.
 *   Once the license is enabled, only the authenticated SDK can be used.
 * - This API should be invoked before agora_rtc_init
 *
 * @param[in] certificate     Certificate buffer
 * @param[in] certificate_len Certificate buffer length
 * @param[in] credential      Credential buffer
 * @param[in] credential_len  Credential buffer length
 *
 * @return
 * - = 0: Success
 * - < 0: Failure
 */
extern __agora_api__ int agora_rtc_license_verify(const char *certificate, int certificate_len,
                                                  const char *credential, int credential_len);

/**
 * Initialize the Agora RTSA service.
 *
 * @note Each process can only be initialized once.
 *
 * @param[in] app_id          Application ID
 * @param[in] uid             Local user ID
 *                            If 'uid' is set as 0, SDK will assign a valid ID to the user
 * @param[in] event_handler   A set of callback that handles Agora SDK events
 * @param[in] sdk_log_dir     Directory for storing SDK log
 *                            If `sdk_log_dir` is NULL, the log directory is located in:
 *                            - iOS:         Application’s documents directory
 *                            - macOS:       ~/Documents/
 *                            - Android:     /sdcard/
 *                            - Ubuntu:      Application’s current directory
 *                            - Embedded:    Application’s current directory
 *
 * @return
 * - = 0: Success
 * - < 0: Failure
 */
extern __agora_api__ int agora_rtc_init(const char *app_id, uint32_t uid,
                                        const agora_rtc_event_handler_t *event_handler,
                                        const char *sdk_log_dir);

/**
 * Initialize the Agora RTSA service.
 *
 * @note Each process can only be initialized once.
 *
 * @param[in] app_id          Application ID
 * @param[in] uname           Local user name
 * @param[in] event_handler   A set of callback that handles Agora SDK events
 * @param[in] sdk_log_dir     Directory for storing SDK log
 *                            If `sdk_log_dir` is NULL, the log directory is located in:
 *                            - iOS:         Application’s documents directory
 *                            - macOS:       ~/Documents/
 *                            - Android:     /sdcard/
 *                            - Ubuntu:      Application’s current directory
 *                            - Embedded:    Application’s current directory
 *
 * @return
 * - = 0: Success
 * - < 0: Failure
 */
extern __agora_api__ int agora_rtc_init_with_name(const char *app_id, const char *uname,
                                                  const agora_rtc_event_handler_t *event_handler,
                                                  const char *sdk_log_dir);

/**
 * Set the log level.
 *
 * Select a level from 0 to 6.
 *
 * @param[in] level Log level. Range is [0, 6]
 *
 * @return
 * - = 0: Success
 * - < 0: Failure
 */
extern __agora_api__ int agora_rtc_set_log_level(int level);

/**
 * Set the log file configuration.
 *
 * @param[in] size_per_file   Each log file size (bytes). Range is [10000, 1000000]
 * @param[in] max_file_count  Maxium log file count for rollback. Range is [1, 100]
 *
 * @return
 * - = 0: Success
 * - < 0: Failure
 */
extern __agora_api__ int agora_rtc_config_log(int size_per_file, int max_file_count);

/**
 * Local user joins channel.
 *
 * @note Users in the same channel with the same App ID can send data to each other.
 *       You can join more than one channel at the same time. All channels that
 *       you join will receive the audio/video data stream that you send unless
 *       you stop sending the audio/video data stream in a specific channel.
 *
 * @param[in] channel   Channel name
 *                      Length should be less than 64 bytes
 *                      Supported character scopes are:
 *                      - The 26 lowercase English letters: a to z
 *                      - The 26 uppercase English letters: A to Z
 *                      - The 10 numbers: 0 to 9
 *                      - The space
 *                      - "!", "#", "$", "%", "&", "(", ")", "+", "-", ":", ";", "<",
 *                        "=", ".", ">", "?", "@", "[", "]", "^", "_", " {", "}", "|",
 *                        "~", ","
 *
 * @param[in] token_ptr Token buffer generated by the server
 *                      - if token authorization is enabled on developer website, it should be set correctly
 *                      - else token can be set as `NULL`
 * @param[in] token_len Token buffer len (bytes), Range is [32, 512]
 *                      If token is set NULL, token_len should be set 0
 *
 * @return
 * - = 0: Success
 * - < 0: Failure
 */
extern __agora_api__ int agora_rtc_join_channel(const char *channel, const void *token_ptr, size_t token_len);

/**
 * Renew token for specific channel OR all channels.
 *
 * @note Token should be renewed when valid duration reached expiration.
 *
 * @param[in] channel   Channel name. For all channels, set `channel` as NULL
 * @param[in] token_ptr Token buffer
 * @param[in] token_len Token buffer length (bytes). Range is [32, 512]
 *
 * @return
 * - = 0: Success
 * - < 0: Failure
 */
extern __agora_api__ int agora_rtc_renew_token(const char *channel, const void *token_ptr, size_t token_len);

/**
 * Decide whether to enable/disable sending local audio data to specific channel OR all channels.
 *
 * @param[in] channel   Channel name
 *                      - if `channel` is set NULL, it means all channels
 *                      - else it means specific channel
 * @param[in] mute      Toggle sending local audio
 *                      - 0:        enable (default)
 *                      - non-ZERO: disable
 * @return
 * - = 0: Success
 * - < 0: Failure
 */
extern __agora_api__ int agora_rtc_mute_local_audio(const char *channel, int mute);

/**
 * Decide whether to enable/disable sending local video data to specific channel OR all channels.
 *
 * @param[in] channel   Channel name
 *                      - if `channel` is set NULL, it means all channels
 *                      - else it means specific channel
 * @param[in] mute      Toggle sending local video
 *                      - 0:        enable (default)
 *                      - non-ZERO: disable
 *
 * @return
 * - = 0: Success
 * - < 0: Failure
 */
extern __agora_api__ int agora_rtc_mute_local_video(const char *channel, int mute);

/**
 * Decide whether to enable/disable receiving remote audio data from specific channel OR all channels.
 *
 * @param[in] channel       Channel name
 *                          - NULL for all channels
 *                          - non-NULL for specific channel
 * @param[in] remote_uid    Remote user ID
 *                          - if `remote_uid` is set 0, it's for all users
 *                          - else it's for specific user
 * @param[in] mute          Toggle receiving remote audio
 *                          - 0:        enable (default)
 *                          - non-ZERO: disable
 *
 * @return
 * - = 0: Success
 * - < 0: Failure
 */
extern __agora_api__ int agora_rtc_mute_remote_audio(const char *channel, uint32_t remote_uid,
                                                     int mute);

/**
 * Decide whether to enable/disable receiving remote video data from specific channel OR all channels.
 *
 * @param[in] channel       Channel name
 *                          - NULL for all channels
 *                          - non-NULL for specific channel
 * @param[in] remote_uid    Remote user ID
 *                          - if `remote_uid` is set 0, it's for all users
 *                          - else it's for specific user
 * @param[in] mute          Toggle receiving remote audio
 *                          - 0:        enable (default)
 *                          - non-ZERO: disable
 *
 * @return
 * - = 0: Success
 * - < 0: Failure
 */
extern __agora_api__ int agora_rtc_mute_remote_video(const char *channel, uint32_t remote_uid,
                                                     int mute);

/**
 * Request remote user to generate a keyframe for all video streams OR specific video stream.
 *
 * @param[in] channel      Channel name
 *                         - NULL for all channels
 *                         - non-NULL for specific channel
 * @param[in] remote_uid   Remote user ID
 *                         - if `remote_uid` is set 0, it's for all users
 *                         - else it's for specific user
 * @param[in] stream_id    Stream ID
 *                         - if `stream_id` is set 0, it's for all video streams
 *                         - else it's for specific video stream
 *
 * @return
 * - = 0: Success
 * - < 0: Failure
 */
extern __agora_api__ int agora_rtc_request_video_key_frame(const char *channel, uint32_t remote_uid,
                                                           uint8_t stream_id);

/**
 * Send an audio frame to all channels OR specific channel.
 *
 * All remote users in this channel will receive the audio frame.
 *
 * @note All channels that you joined will receive the audio frame that you send
 *       unless you stop sending the local audio to a specific channel.
 *
 * @param[in] channel   Channel name
 *                      - NULL for all channels
 *                      - non-NULL for specific channel
 * @param[in] data_ptr  Audio frame buffer
 * @param[in] data_len  Audio frame buffer length (bytes)
 * @param[in] info_ptr  Audio frame info, see #audio_frame_info_t
 *
 * @return
 * - = 0: Success
 * - < 0: Failure
 */
extern __agora_api__ int agora_rtc_send_audio_data(const char *channel, const void *data_ptr,
                                       size_t data_len, audio_frame_info_t *info_ptr);

/**
 * Send a video frame to all channels OR specific channel.
 *
 * All remote users in the channel will receive the video frame.
 *
 * @note All channels that you join will receive the video frame that you send
 *       unless you stop sending the local video to a specific channel.
 *
 * @param[in] channel   Channel name
 *                      - NULL     for all channels
 *                      - non-NULL for specific channel
 * @param[in] stream_id Stream ID
 * @param[in] data_ptr  Video frame buffer
 * @param[in] data_len  Video frame buffer length (bytes)
 * @param[in] info_ptr  Video frame info
 *
 * @return
 * - = 0: Success
 * - < 0: Failure
 */
extern __agora_api__ int agora_rtc_send_video_data(const char *channel, uint8_t stream_id,
                                        const void *data_ptr, size_t data_len, video_frame_info_t *info_ptr);

/**
 * Send command through reliable data channel (RDT)
 *
 * @note Reliable data channel is not available by default, unless callback
 *      `on_rdt_availability_changed` is triggered.
 *
 * @param[in] channel    Channel name
 * @param[in] cmd        Command code
 * @param[in] param_ptr  Parameter buffer
 * @param[in] param_len  Parameter buffer length
 *
 * @return
 * - = 0: Success
 * - < 0: Failure
 */
extern __agora_api__ int agora_rtc_send_cmd(const char *channel, int cmd, const void *param_ptr,
                                            size_t param_len);

/**
 * Send data through reliable data channel (RDT).
 *
 * @note Reliable data channel is not available by default, unless callback
 *      `on_rdt_availability_changed` occurs.
 *
 * @param[in] channel   Channel name
 * @param[in] data_ptr  Data buffer
 * @param[in] data_len  Data buffer length (bytes)
 *
 * @return
 * - = 0: Success
 * - < 0: Failure
 */
extern __agora_api__ int agora_rtc_send_through_rdt(const char *channel, const void *data_ptr,
                                                    size_t data_len);

/**
 * Set network bandwidth estimation (BWE) param
 *
 * @param[in] min_bps   bwe min bps
 * @param[in] max_bps   bwe max bps
 * @param[in] start_bps bwe start bps
 *
 * @return:
 * - = 0: Success
 * - < 0: Failure
 */
extern __agora_api__ int agora_rtc_set_bwe_param(uint32_t min_bps, uint32_t max_bps, uint32_t start_bps);

/**
 * Allow Local user leaves channel.
 *
 * @note Local user should leave channel when data transmission is stopped
 *
 * @param[in] channel   Channel name
 *                      - NULL for all channels
 *                      - non-NULL for specific channel
 *
 * @return
 * - = 0: Success
 * - < 0: Failure
 */
extern __agora_api__ int agora_rtc_leave_channel(const char *channel);

/**
 * Release all resource allocated by Agora RTSA SDK
 *
 * @return
 * - = 0: Success
 * - < 0: Failure
 */
extern __agora_api__ int agora_rtc_fini(void);

#ifdef __cplusplus
}
#endif

#endif /* __AGORA_RTC_API_H__ */
