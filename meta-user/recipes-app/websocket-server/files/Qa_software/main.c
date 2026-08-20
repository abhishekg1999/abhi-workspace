#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "apis.h"
#include <signal.h>
#include <libwebsockets.h>
#include <pthread.h>

int interrupted = 0;
void sigint_handler(int sig) { 
	interrupted = 1; 
}

/* ==== MAIN ==== */
int main(void) {
    
    signal(SIGINT, sigint_handler);
    signal(SIGTERM, sigint_handler);
    
    /* Load XML + build mapping */ 
    if (load_xml_and_map() != 0) {
        printf("Failed to load controller.xml\n");
    } else {
    	printf("Loaded %d signals from XML\n", signal_count);
    }
	
    struct lws_context_creation_info info;
    memset(&info, 0, sizeof(info));
    info.port = 9000; 
    info.protocols = protocols;

    struct lws_context *context = lws_create_context(&info);
    if (!context) {
        fprintf(stderr, "lws init failed\n");
        return -1;
    }

    printf("WebSocket server listening on ws://localhost:9000\n");

    while (!interrupted) {
        lws_service(context, 1);
    }

    cleanup_mem();
    /* cleanup XML */
    if (g_doc) 
    	xmlFreeDoc(g_doc);
    
    lws_context_destroy(context);
    printf("Server stopped\n");
    return 0;
}
