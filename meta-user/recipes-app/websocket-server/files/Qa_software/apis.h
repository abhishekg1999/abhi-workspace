#ifndef APIS_H
#define APIS_H

#include <stdint.h>
//#include "fg_reset.h"
#include <signal.h>
#include <libwebsockets.h>
#include <stdbool.h>
#include <pthread.h>
#include <libxml/parser.h>
#include <libxml/tree.h>

typedef char* (*void_func_ptr)(void);
typedef char* (*one_arg_func_ptr)(uint32_t);
typedef char* (*two_arg_func_ptr)(uint32_t, uint32_t);
typedef char* (*three_arg_func_ptr)(uint32_t, uint32_t, uint32_t);

/* One-argument APIs */
typedef struct {
    const char *name;
    one_arg_func_ptr func;
} OneArgAPI;

/* Void APIs */
typedef struct {
    const char *name;
    void_func_ptr func;
} VoidAPI;

/* Two-argument APIs */ 
typedef struct {
    const char *name;
    two_arg_func_ptr func;
} TwoArgAPI;

/* Two-argument APIs */ 
typedef struct {
    const char *name;
    three_arg_func_ptr func;
} ThreeArgAPI;

/* websocket prototypes #### start */
extern int callback_api(struct lws *wsi,enum lws_callback_reasons reason,void *user, void *in, size_t len);
extern struct lws_protocols protocols[];
/* websocket prototypes #### End */

extern void cleanup_mem(void);
extern int load_xml_and_map(void);

/* API arrays */
extern VoidAPI void_apis[]; 
extern OneArgAPI one_arg_apis[];
extern TwoArgAPI two_arg_apis[];
extern ThreeArgAPI three_arg_apis[];
extern uint32_t VoidApi_count;
extern uint32_t OneArgApi_count;
extern uint32_t TwoArgApi_count;
extern uint32_t ThreeArgApi_count;

extern int signal_count;
extern xmlDocPtr g_doc;

#endif // APIS_H
