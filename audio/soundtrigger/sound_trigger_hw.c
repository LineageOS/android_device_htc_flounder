/*
 * Copyright (C) 2014 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#define LOG_TAG "sound_trigger_hw_flounder"
/*#define LOG_NDEBUG 0*/

#include <errno.h>
#include <fcntl.h>
#include <poll.h>
#include <pthread.h>
#include <sys/ioctl.h>
#include <sys/prctl.h>
#include <cutils/log.h>
#include <cutils/uevent.h>

#include <hardware/hardware.h>
#include <system/sound_trigger.h>
#include <hardware/sound_trigger.h>
#include <tinyalsa/asoundlib.h>

#define FLOUNDER_MIXER_VAD	1
#define FLOUNDER_CTRL_DSP	26
#define FLOUNDER_CTRL_MIC	198
#define UEVENT_MSG_LEN		1024

#define FLOUNDER_VAD_DEV	"/dev/snd/hwC1D0"
#define FLOUNDER_MIC_BUF_SIZE	(64 * 1024)

static const struct sound_trigger_properties hw_properties = {
    "The Android Open Source Project", // implementor
    "Volantis OK Google ", // description
    1, // version
    { 0xe780f240, 0xf034, 0x11e3, 0xb79a, { 0x00, 0x02, 0xa5, 0xd5, 0xc5, 0x1b } }, // uuid
    1, // max_sound_models
    1, // max_key_phrases
    1, // max_users
    RECOGNITION_MODE_VOICE_TRIGGER, // recognition_modes
    false, // capture_transition
    0, // max_capture_ms
// FIXME: workaround for issue 16844445
    // false, // concurrent_capture
    true, // concurrent_capture
    true, // trigger_in_event
    0 // power_consumption_mw
};

struct flounder_sound_trigger_device {
    struct sound_trigger_hw_device device;
    sound_model_handle_t model_handle;
    recognition_callback_t recognition_callback;
    void *recognition_cookie;
    sound_model_callback_t sound_model_callback;
    void *sound_model_cookie;
    pthread_t callback_thread;
    pthread_mutex_t lock;
    int send_sock;
    int term_sock;
    int vad_fd;
    struct mixer *mixer;
    struct mixer_ctl *ctl_dsp;
    struct mixer_ctl *ctl_mic;
    struct sound_trigger_recognition_config *config;
};

struct rt_codec_cmd {
    size_t number;
    int *buf;
};

enum {
    RT_READ_CODEC_DSP_IOCTL = _IOR('R', 0x04, struct rt_codec_cmd),
    RT_WRITE_CODEC_DSP_IOCTL = _IOW('R', 0x04, struct rt_codec_cmd),
};


static int stdev_init_mixer(struct flounder_sound_trigger_device *stdev)
{
    int ret = -1;

    stdev->vad_fd = open(FLOUNDER_VAD_DEV, O_RDWR);
    if (stdev->vad_fd < 0) {
        ALOGE("Error opening vad device");
        return ret;
    }

    stdev->mixer = mixer_open(FLOUNDER_MIXER_VAD);
    if (!stdev->mixer)
        goto err;

    stdev->ctl_mic = mixer_get_ctl(stdev->mixer, FLOUNDER_CTRL_MIC);
    if (!stdev->ctl_mic)
        goto err;

    stdev->ctl_dsp = mixer_get_ctl(stdev->mixer, FLOUNDER_CTRL_DSP);
    if (!stdev->ctl_dsp)
        goto err;

    return 0;

err:
    close(stdev->vad_fd);
    if (stdev->mixer)
        mixer_close(stdev->mixer);
    return ret;
}

static void stdev_close_term_sock(struct flounder_sound_trigger_device *stdev)
{
    if (stdev->send_sock >=0) {
        close(stdev->send_sock);
        stdev->send_sock = -1;
    }
    if (stdev->term_sock >=0) {
        close(stdev->term_sock);
        stdev->term_sock = -1;
    }
}

static void stdev_close_mixer(struct flounder_sound_trigger_device *stdev)
{
    if (stdev) {
        mixer_close(stdev->mixer);
        stdev_close_term_sock(stdev);
        close(stdev->vad_fd);
    }
}

static int vad_load_sound_model(struct flounder_sound_trigger_device *stdev,
                                char *buf, size_t len)
{
    struct rt_codec_cmd cmd;
    int ret = 0;

    if (!buf || (len == 0))
        return ret;

    cmd.number = len / sizeof(int);
    cmd.buf = (int *)buf;

    ret = ioctl(stdev->vad_fd, RT_WRITE_CODEC_DSP_IOCTL, &cmd);
    if (ret)
        ALOGE("Error VAD write ioctl");
    return ret;
}

static int vad_get_phrase(struct flounder_sound_trigger_device *stdev,
                          char *buf, unsigned int *len)
{
    struct rt_codec_cmd cmd;
    int ret;

    cmd.number = (*len) / sizeof(int);
    cmd.buf = (int *)buf;

    ret = ioctl(stdev->vad_fd, RT_READ_CODEC_DSP_IOCTL, &cmd);
    *len = (unsigned int)cmd.number * sizeof(int);
    if (ret)
        ALOGE("Error VAD read ioctl");
    return ret;
}

static char *sound_trigger_event_alloc(struct flounder_sound_trigger_device *
                                       stdev)
{
    char *data = (char *)calloc(1,
                    sizeof(struct sound_trigger_phrase_recognition_event) +
                    FLOUNDER_MIC_BUF_SIZE);
    struct sound_trigger_phrase_recognition_event *event =
                    (struct sound_trigger_phrase_recognition_event *)data;

    if (!data)
        return NULL;
    event->common.status = RECOGNITION_STATUS_SUCCESS;
    event->common.type = SOUND_MODEL_TYPE_KEYPHRASE;
    event->common.model = stdev->model_handle;

    if (stdev->config) {
        unsigned int i;

        event->num_phrases = stdev->config->num_phrases;
        if (event->num_phrases > SOUND_TRIGGER_MAX_PHRASES)
            event->num_phrases = SOUND_TRIGGER_MAX_PHRASES;
        for (i=0; i < event->num_phrases; i++)
            memcpy(&event->phrase_extras[i], &stdev->config->phrases[i],
                   sizeof(struct sound_trigger_phrase_recognition_extra));
    }

    event->num_phrases = 1;
    event->phrase_extras[0].confidence_level = 100;
    event->phrase_extras[0].num_levels = 1;
    event->phrase_extras[0].levels[0].level = 100;
    event->phrase_extras[0].levels[0].user_id = 0;

    event->common.trigger_in_data = true;
    event->common.audio_config = AUDIO_CONFIG_INITIALIZER;
    event->common.audio_config.sample_rate = 16000;
    event->common.audio_config.channel_mask = AUDIO_CHANNEL_IN_MONO;
    event->common.audio_config.format = AUDIO_FORMAT_PCM_16_BIT;

    event->common.data_offset =
                    sizeof(struct sound_trigger_phrase_recognition_event);
    event->common.data_size = FLOUNDER_MIC_BUF_SIZE;
    vad_get_phrase(stdev, data + event->common.data_offset,
                   &event->common.data_size);
    return data;
}

static void *callback_thread_loop(void *context)
{
    char msg[UEVENT_MSG_LEN];
    struct flounder_sound_trigger_device *stdev =
               (struct flounder_sound_trigger_device *)context;
    struct pollfd fds[2];
    int exit_sockets[2];
    int err = 0;
    int i, n;

    ALOGI("%s", __func__);
    prctl(PR_SET_NAME, (unsigned long)"sound trigger callback", 0, 0, 0);

    pthread_mutex_lock(&stdev->lock);
    if (stdev->recognition_callback == NULL)
        goto exit;

    if (socketpair(AF_UNIX, SOCK_STREAM, 0, exit_sockets) == -1)
        goto exit;

    stdev_close_term_sock(stdev);
    stdev->send_sock = exit_sockets[0];
    stdev->term_sock = exit_sockets[1];

    memset(fds, 0, 2 * sizeof(struct pollfd));
    fds[0].events = POLLIN;
    fds[0].fd = uevent_open_socket(64*1024, true);
    if (fds[0].fd == -1) {
        ALOGE("Error opening socket for hotplug uevent");
        goto exit;
    }
    fds[1].events = POLLIN;
    fds[1].fd = stdev->term_sock;

    mixer_ctl_set_value(stdev->ctl_dsp, 0, 1);
    mixer_ctl_set_value(stdev->ctl_mic, 0, 1);

    pthread_mutex_unlock(&stdev->lock);

    while (1) {
        err = poll(fds, 2, -1);
        pthread_mutex_lock(&stdev->lock);
        if ((err < 0) || (stdev->recognition_callback == NULL)) {
            ALOGE("Error in hotplug CPU poll: %d", errno);
            break;
        }

        if (fds[0].revents & POLLIN) {
            n = uevent_kernel_multicast_recv(fds[0].fd, msg, UEVENT_MSG_LEN);
            if (n <= 0) {
                pthread_mutex_unlock(&stdev->lock);
                continue;
            }
            for (i=0; i < n;) {
                if (strstr(msg + i, "HOTWORD")) {
                    struct sound_trigger_phrase_recognition_event *event;

                    event = (struct sound_trigger_phrase_recognition_event *)
                            sound_trigger_event_alloc(stdev);
                    if (event) {
                        ALOGI("%s send callback model %d", __func__,
                              stdev->model_handle);
                        stdev->recognition_callback(&event->common,
                                                    stdev->recognition_cookie);
                        free(event);
                    }
                    goto found;
                }
                i += strlen(msg + i) + 1;
            }
        } else if (fds[1].revents & POLLIN) {
            read(fds[1].fd, &n, sizeof(n)); /* clear the socket */
            ALOGI("%s: Termination message: %d", __func__, n);
            break;
        } else {
            ALOGI("%s: Message to ignore", __func__);
        }
        pthread_mutex_unlock(&stdev->lock);
    }

found:
    close(fds[0].fd);

exit:
    stdev->recognition_callback = NULL;
    stdev_close_term_sock(stdev);

    mixer_ctl_set_value(stdev->ctl_dsp, 0, 0);
    mixer_ctl_set_value(stdev->ctl_mic, 0, 0);

    pthread_mutex_unlock(&stdev->lock);

    return (void *)(long)err;
}

static int stdev_get_properties(const struct sound_trigger_hw_device *dev,
                                struct sound_trigger_properties *properties)
{
    struct flounder_sound_trigger_device *stdev =
                               (struct flounder_sound_trigger_device *)dev;

    ALOGI("%s", __func__);
    if (properties == NULL)
        return -EINVAL;
    memcpy(properties, &hw_properties, sizeof(struct sound_trigger_properties));
    return 0;
}

static int stdev_load_sound_model(const struct sound_trigger_hw_device *dev,
                                  struct sound_trigger_sound_model *sound_model,
                                  sound_model_callback_t callback,
                                  void *cookie,
                                  sound_model_handle_t *handle)
{
    struct flounder_sound_trigger_device *stdev =
                                 (struct flounder_sound_trigger_device *)dev;
    int ret = 0;

    ALOGI("%s", __func__);
    pthread_mutex_lock(&stdev->lock);
    if (handle == NULL || sound_model == NULL) {
        ret = -EINVAL;
        goto exit;
    }

    if (stdev->model_handle == 1) {
        ret = -ENOSYS;
        goto exit;
    }

    ret = vad_load_sound_model(stdev,
                               (char *)sound_model + sound_model->data_offset,
                               sound_model->data_size);
    stdev->model_handle = 1;
    stdev->sound_model_callback = callback;
    stdev->sound_model_cookie = cookie;
    *handle = stdev->model_handle;

exit:
    pthread_mutex_unlock(&stdev->lock);
    return ret;
}

static int stdev_unload_sound_model(const struct sound_trigger_hw_device *dev,
                                    sound_model_handle_t handle)
{
    struct flounder_sound_trigger_device *stdev =
                                   (struct flounder_sound_trigger_device *)dev;
    int status = 0;

    ALOGI("%s handle %d", __func__, handle);
    pthread_mutex_lock(&stdev->lock);
    if (handle != 1) {
        status = -EINVAL;
        goto exit;
    }
    if (stdev->model_handle == 0) {
        status = -ENOSYS;
        goto exit;
    }
    stdev->model_handle = 0;
    free(stdev->config);
    stdev->config = NULL;
    if (stdev->recognition_callback != NULL) {
        stdev->recognition_callback = NULL;
        ALOGI("%s: Sending T", __func__);
        if (stdev->send_sock >=0)
            write(stdev->send_sock, "T", 1);
        pthread_mutex_unlock(&stdev->lock);

        pthread_join(stdev->callback_thread, (void **)NULL);

        pthread_mutex_lock(&stdev->lock);
    }

exit:
    pthread_mutex_unlock(&stdev->lock);
    return status;
}

static int stdev_start_recognition(const struct sound_trigger_hw_device *dev,
                                   sound_model_handle_t sound_model_handle,
                                   const struct sound_trigger_recognition_config *config,
                                   recognition_callback_t callback,
                                   void *cookie)
{
    struct flounder_sound_trigger_device *stdev =
                                  (struct flounder_sound_trigger_device *)dev;
    int status = 0;

    ALOGI("%s sound model %d", __func__, sound_model_handle);
    pthread_mutex_lock(&stdev->lock);
    if (stdev->model_handle != sound_model_handle) {
        status = -ENOSYS;
        goto exit;
    }
    if (stdev->recognition_callback != NULL) {
        status = -ENOSYS;
        goto exit;
    }

    free(stdev->config);
    stdev->config = NULL;
    if (config) {
        stdev->config = malloc(sizeof(*config));
        if (!stdev->config) {
            status = -ENOMEM;
            goto exit;
        }
        memcpy(stdev->config, config, sizeof(*config));
    }

    stdev->recognition_callback = callback;
    stdev->recognition_cookie = cookie;
    pthread_create(&stdev->callback_thread, (const pthread_attr_t *) NULL,
                        callback_thread_loop, stdev);
exit:
    pthread_mutex_unlock(&stdev->lock);
    return status;
}

static int stdev_stop_recognition(const struct sound_trigger_hw_device *dev,
                                  sound_model_handle_t sound_model_handle)
{
    struct flounder_sound_trigger_device *stdev =
                                 (struct flounder_sound_trigger_device *)dev;
    int status = 0;

    ALOGI("%s sound model %d", __func__, sound_model_handle);
    pthread_mutex_lock(&stdev->lock);
    if (stdev->model_handle != sound_model_handle) {
        status = -ENOSYS;
        goto exit;
    }
    if (stdev->recognition_callback == NULL) {
        status = -ENOSYS;
        goto exit;
    }
    free(stdev->config);
    stdev->config = NULL;
    stdev->recognition_callback = NULL;
    ALOGI("%s: Sending T", __func__);
    if (stdev->send_sock >=0)
        write(stdev->send_sock, "T", 1);
    pthread_mutex_unlock(&stdev->lock);

    pthread_join(stdev->callback_thread, (void **)NULL);

    pthread_mutex_lock(&stdev->lock);

exit:
    pthread_mutex_unlock(&stdev->lock);
    return status;
}


static int stdev_close(hw_device_t *device)
{
    struct flounder_sound_trigger_device *stdev =
                                (struct flounder_sound_trigger_device *)device;

    stdev_close_mixer(stdev);
    free(device);
    return 0;
}

static int stdev_open(const hw_module_t *module, const char *name,
                      hw_device_t **device)
{
    struct flounder_sound_trigger_device *stdev;
    int ret;

    if (strcmp(name, SOUND_TRIGGER_HARDWARE_INTERFACE) != 0)
        return -EINVAL;

    stdev = calloc(1, sizeof(struct flounder_sound_trigger_device));
    if (!stdev)
        return -ENOMEM;

    stdev->device.common.tag = HARDWARE_DEVICE_TAG;
    stdev->device.common.version = SOUND_TRIGGER_DEVICE_API_VERSION_1_0;
    stdev->device.common.module = (struct hw_module_t *)module;
    stdev->device.common.close = stdev_close;
    stdev->device.get_properties = stdev_get_properties;
    stdev->device.load_sound_model = stdev_load_sound_model;
    stdev->device.unload_sound_model = stdev_unload_sound_model;
    stdev->device.start_recognition = stdev_start_recognition;
    stdev->device.stop_recognition = stdev_stop_recognition;

    pthread_mutex_init(&stdev->lock, (const pthread_mutexattr_t *)NULL);

    stdev->send_sock = stdev->term_sock = -1;

    ret = stdev_init_mixer(stdev);
    if (ret) {
        ALOGE("Error mixer init");
        free(stdev);
    } else {
        *device = &stdev->device.common; /* same address as stdev */
    }
    return ret;
}

static struct hw_module_methods_t hal_module_methods = {
    .open = stdev_open,
};

struct sound_trigger_module HAL_MODULE_INFO_SYM = {
    .common = {
        .tag = HARDWARE_MODULE_TAG,
        .module_api_version = SOUND_TRIGGER_MODULE_API_VERSION_1_0,
        .hal_api_version = HARDWARE_HAL_API_VERSION,
        .id = SOUND_TRIGGER_HARDWARE_MODULE_ID,
        .name = "Default sound trigger HAL",
        .author = "The Android Open Source Project",
        .methods = &hal_module_methods,
    },
};
