/**
 *
 * Release under GPLv-3.0.
 * 
 * @file    hy_event_listener.h
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    27/04 2023 09:15
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        27/04 2023      create the file
 * 
 *     last modified: 27/04 2023 09:15
 */
#ifndef __LIBHY_UTILS_INCLUDE_HY_EVENT_LISTENER_H_
#define __LIBHY_UTILS_INCLUDE_HY_EVENT_LISTENER_H_

#include <semaphore.h>
#ifdef __cplusplus
extern "C" {
#endif

#include <event2/event.h>
#include <event2/listener.h>

#include <hy_os_type/hy_os_type.h>

typedef void (*HyEventListenerErrorCb_t)(void *args);

typedef struct {
    hy_s32_t            signal;
    event_callback_fn   signal_cb;
    void                *signal_cb_args;
} HyEventListenerSignal_s;

typedef void (*HyEventListenerCb_t)(evutil_socket_t sock,
                                    struct sockaddr *addr,
                                    int socklen,
                                    void *args);

typedef struct {
    HyEventListenerCb_t         listener_cb;
    HyEventListenerErrorCb_t    listener_error_cb;
    void                        *listener_args;


    HyEventListenerSignal_s     *signal_cb;
    hy_u32_t                    signal_cb_num;
} HyEventListenerSaveConfig_s;

typedef struct {
    HyEventListenerSaveConfig_s save_c;

    const char                  *ip;
    hy_u32_t                    port;
} HyEventListenerConfig_s;

typedef struct HyEventListener_s HyEventListener_s;

HyEventListener_s *HyEventListenerCreate(HyEventListenerConfig_s *listener_c);
void HyEventListenerDestroy(HyEventListener_s **handle_pp);

void HyEventListenerLoop(HyEventListener_s *handle);
void HyEventListenerExit(HyEventListener_s *handle);

#ifdef __cplusplus
}
#endif

#endif

