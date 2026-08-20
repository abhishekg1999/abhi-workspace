#include <libwebsockets.h>
#include "apis.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct {
    int stream_enabled;
    int stream_interval_ms;
    int no_data_sent;
    int stream_api_index;
} per_session_data;

struct lws_protocols protocols[] = {
    {
        .name = "api-protocol",
        .callback = callback_api,
        .per_session_data_size = sizeof(per_session_data),
        .rx_buffer_size = 0,
    },
    { NULL, NULL, 0, 0 } /* terminator */
};

unsigned char buf[LWS_PRE + 2048];
unsigned char *p = &buf[LWS_PRE];
    
/* ==== WEBSOCKET CALLBACK ==== */
int callback_api(struct lws *wsi,enum lws_callback_reasons reason,void *user, void *in, size_t len) {
    
    per_session_data *pss = (per_session_data *)user;
    
    switch (reason) {
    	case LWS_CALLBACK_ESTABLISHED:
        	lwsl_user("Client connected\n");
        	pss->stream_enabled = 0;
    		pss->stream_api_index = -1;
    		pss->no_data_sent = 0;
    		pss->stream_interval_ms = 100;
        	break;

        case LWS_CALLBACK_RECEIVE:
        	/* incoming plain-text command */
        	char *msg = strndup((const char *)in, len);
        	lwsl_user("Received: %s\n", msg);
        	int n = 0;

        	/* split command into api_ptr + optional arg */
        	char *api_ptr = strtok(msg, " \t\r\n");
        	char *arg1 = strtok(NULL, " \t\r\n");
        	char *arg2 = strtok(NULL, " \t\r\n");
        	char *arg3 = strtok(NULL, " \t\r\n");
		
        	if (!api_ptr) { 
        		n = snprintf((char *)p, 512,"{ \"error\":\"Empty command\" }");
        		goto send;
        	}
        	
            	/* search void APIs stream + non stream */
            	int stream_mode = (arg1 && strcmp(arg1, "start_stream") == 0);
            	size_t i;
            	for (i = 0; i < VoidApi_count; i++) {
                    if (strcmp(api_ptr, void_apis[i].name) == 0) {
                	
                	if (stream_mode) {
                		/* ----- START STREAM ----- */
            			pss->stream_enabled = 1;
            			pss->stream_api_index = (int)i;
				pss->stream_interval_ms = 5;  // or 2, 5, etc
				lws_set_timer_usecs(wsi, pss->stream_interval_ms * 1000);
            			n = snprintf((char *)p, sizeof(buf) - LWS_PRE, 
                         		"{\"status\":\"Streaming %s started\"}", void_apis[i].name);
                        } else {
                        	/* non stream void api call */
                    	    	char *resp = void_apis[i].func();
                    	    	size_t buf_size = sizeof(buf) - LWS_PRE;
                    	    	n = snprintf((char *)p, buf_size, "%s", resp);
                    	    	
                    	    	if ((size_t)n >= buf_size) {
    		    	           lwsl_warn("Response truncated!\n");
    		     	           n = buf_size - 1; // ensure valid length
    		                }  
                    	}
                    	
                    	goto send;
                    }
                }

		/* ----- STOP STREAM ----- */
		if (strcmp(api_ptr, "stop_stream") == 0) {
    			pss->stream_enabled = 0;
    			pss->stream_api_index = -1; // Reset the index
    			pss->stream_interval_ms = 0; //reset interval
    			
    			//cancel a scheduled timer in libwebsockets
    			lws_set_timer_usecs(wsi, -1);
    			n = snprintf((char *)p, 512, "{ \"status\":\"CAN stream stopped\" }");
    			goto send;
		}
		
		/* search one-arg APIs */
                for (i = 0; i < OneArgApi_count; i++) {
    		    if (strcmp(api_ptr, one_arg_apis[i].name) == 0) {
       		       if (!arg1) {
            	       /* no argument provided */
           	       n = snprintf((char *)p, 512,
                          "{ \"Error\":\"Missing argument for %s\" }",
                          one_arg_apis[i].name);
                    
                    	} else {
                       	   uint32_t mode = strtoul(arg1, NULL, 0);
                           char *resp = one_arg_apis[i].func(mode);
                           size_t buf_size = sizeof(buf) - LWS_PRE;
                           n = snprintf((char *)p, buf_size, "%s", resp);
                    
                             if ((size_t)n >= buf_size) {
    		    	        lwsl_warn("Response truncated!\n");
    		     	        n = buf_size - 1; // ensure valid length
    		             }    		    	
                        }
               	    
               	    	goto send;
                    }
                }
                
                /* search two-arg APIs */
    		for (i = 0; i < TwoArgApi_count; i++) {
        	    if (strcmp(api_ptr, two_arg_apis[i].name) == 0) {
		       if (!arg1 || !arg2) {
                	  n = snprintf((char *)p, 512,
                              "{ \"Error\":\"Missing arguments for %s. Need 2 args\" }",
                               two_arg_apis[i].name);
                       } else {
                         uint32_t a1 = strtoul(arg1, NULL, 0);
                         uint32_t a2 = strtoul(arg2, NULL, 0);
                         char *resp = two_arg_apis[i].func(a1, a2);
                         size_t buf_size = sizeof(buf) - LWS_PRE;
                         n = snprintf((char *)p, buf_size, "%s", resp);
                      }
            	     
            	     	goto send;
                   }
                }
                
                
                /* search three-arg APIs */
    		for (i = 0; i < ThreeArgApi_count; i++) {
        	    if (strcmp(api_ptr, three_arg_apis[i].name) == 0) {
		       if (!arg1 || !arg2 || !arg3) {
                	  n = snprintf((char *)p, 512,
                              "{ \"Error\":\"Missing arguments for %s. Need 3 args\" }",
                               three_arg_apis[i].name);
                       } else {
                         uint32_t a1 = strtoul(arg1, NULL, 0);
                         uint32_t a2 = strtoul(arg2, NULL, 0);
                         uint32_t a3 = strtoul(arg3, NULL, 0);
                         char *resp = three_arg_apis[i].func(a1, a2, a3);
                         size_t buf_size = sizeof(buf) - LWS_PRE;
                         n = snprintf((char *)p, buf_size, "%s", resp);
                      }
            	     
            	     	goto send;
                   }
                }
                
		/* ---------- NOT FOUND ---------- */
    		n = snprintf((char *)p, 512, "{ \"error\":\"Unrecognized API Name\" }");
    	send:
        	free(msg);
        	lws_write(wsi, p, n, LWS_WRITE_TEXT);
       	 	break;
       	 	
       	case LWS_CALLBACK_TIMER:
    		if (pss->stream_enabled) 
    			lws_callback_on_writable(wsi);
    		break;
    		
	case LWS_CALLBACK_SERVER_WRITEABLE:
    	
		if (!pss->stream_enabled)
		break;
        		
        	char *resp = void_apis[pss->stream_api_index].func();   //call API
        	if (resp) {
        		int n = snprintf((char *)p, sizeof(buf) - LWS_PRE, "%s", resp);
			lws_write(wsi, p, n, LWS_WRITE_TEXT);
	     		
		    // Stop stream if status != 0	    		 
    		    if (strstr(resp, "\"status\": 0") == NULL){
                	pss->stream_enabled = 0;
                	break;
            	    }
    		 }
    		
    		// Continue only if still enabled
        	lws_set_timer_usecs(wsi, pss->stream_interval_ms * 1000);
		break;
		
        case LWS_CALLBACK_CLOSED:
        	lwsl_user("Client disconnected\n");
        	break;

        default:
        	break;
    }  /* switch close */
    
        return 0;
}
