#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <stdint.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include "apis.h"
#include <pthread.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include "fg_reset.h"
#include "reg.h"

 /* xml data */ 
#define MAX_SIGNALS 300
#define XML_PATH "/etc/config/controller.xml"
//#define XML_PATH "controller.xml"

typedef struct {
    uint32_t id;
    xmlNodePtr node;   // direct pointer to XML node
} signal_map_t;

signal_map_t signal_map[MAX_SIGNALS];
int signal_count = 0;
xmlDocPtr g_doc = NULL;


#define MAP_SIZE 4096UL
#define MAP_MASK (MAP_SIZE - 1)
#define GET_BIT(v, b) (((v) >> (b)) & 0x1)
#define CHUNK_SIZE 50

typedef enum {
    STATUS_SUCCESS = 0,           // no error
    STATUS_ERR_INVALID_REG = 1,  // reg range not matched
    STATUS_ERR_INVALID_FLAG = 2, // 0 or 1
    STATUS_ERR_INVALID_BIT = 3,  //bit position is not valid
    STATUS_ERR_MISMATCH = 4,   // register rd wr missmatch
    STATUS_ERR_INVALID_SIG = 5,   // invalid signal
    STATUS_ERR_INVALID_FGID = 6,   //Invalid fg id
    STATUS_ERR_INVALID_TYPE = 7,   // invalid reg catagory
    STATUS_ERR_HW_FAILED = -1  //reg map failed
} status_code_t;

typedef struct {
    volatile uint32_t *map_base;
    uint32_t base_addr;
}mem_map_t;

mem_map_t mmap_info = { NULL, 0 };
static int fd = -1;

// Map a base address (if not already mapped or changed)
int init_mem(uint32_t base_addr) {
    if (fd < 0) {
        fd = open("/dev/mem", O_RDWR | O_SYNC);
        if (fd < 0) return -1;
    }

    // If already mapped and same base, do nothing
    if (mmap_info.map_base && mmap_info.base_addr == base_addr)
        return 0;

    // Unmap previous mapping
    if (mmap_info.map_base) {
        munmap((void *)mmap_info.map_base, MAP_SIZE);
        mmap_info.map_base = NULL;
    }

    // New mapping
    mmap_info.map_base = (volatile uint32_t *) mmap(
        0, MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, base_addr & ~MAP_MASK);

    if (mmap_info.map_base == MAP_FAILED) {
        mmap_info.map_base = NULL;
        return -2;
    }

    mmap_info.base_addr = base_addr;
    return 0;
}

// Read using base address + offset
int reg_read(uint32_t base_addr, uint32_t offset, uint32_t *value) {

    if (!(mmap_info.map_base && mmap_info.base_addr == base_addr)) {
        if (init_mem(base_addr) != 0)
            return -1;
    }

    *value = *(mmap_info.map_base + (offset / sizeof(uint32_t)));
    return 0;
}

// Write using base address + offset
int reg_write(uint32_t base_addr, uint32_t offset, uint32_t value) {

    if (!(mmap_info.map_base && mmap_info.base_addr == base_addr)) {
        if (init_mem(base_addr) != 0)
            return -1;
    }
    
    *(mmap_info.map_base + (offset / sizeof(uint32_t))) = value;
    return 0;
}

void cleanup_mem() {
    if (mmap_info.map_base) {
        munmap((void *)mmap_info.map_base, MAP_SIZE);
        mmap_info.map_base = NULL;
    }
    if (fd >= 0) {
        close(fd);
        fd = -1;
    }
}

char* register_update(uint32_t adds, uint32_t user_value){

    static char buf[128];   
    status_code_t fault = STATUS_SUCCESS;
    uint32_t base =adds&0xFFFFF000, offset =adds&0x00000FFF, reg_val=0;

    // ---- Update Register ----
    if (reg_write(base, offset, user_value) == -1) {
    	fault = STATUS_ERR_HW_FAILED;
        goto result;
    }
    
    // ---- Read Back Verification ----
    reg_read(base, offset, &reg_val);
    if (reg_val != user_value) 
    	fault = STATUS_ERR_MISMATCH;
    
    result:
    snprintf(buf, sizeof(buf), 
    	"{ \"status\": %d }", 
    	fault);
             
    return buf;
}

char* Enable_Line_Side(uint32_t choice){
    static char buf[512];

    snprintf(buf, sizeof(buf),
                 "{ \"input\": \"%u\" }\n", choice);
        return buf;
}

/* pentograph registers API*/
char* raise_pan1_vcu2dio2_status(void){

        static char buf[256];
	uint32_t value;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA2020000, 0x004, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"status\": %d, \"value\": \"%d\" }",
                 fault, (value>>25)&1);
        return buf;
}

char* raise_pan2_vcu2dio1_status(void){

        static char buf[256];
	uint32_t value;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA2020000, 0x000, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"status\": %d, \"value\": \"%d\" }",
                 fault, (value>>29)&1);
        return buf;
}

char* panto_disable_vcu2dio1_status(void){

        static char buf[256];
	uint32_t value;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA2020000, 0x000, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"status\": %d, \"value\": \"%d\" }",
                 fault, (value>>13)&1);
        return buf;
}

char* panto_disable_vcu2dio2_status(void){

        static char buf[256];
	uint32_t value;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA2020000, 0x004, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"status\": %d, \"value\": \"%d\" }",
                 fault, (value>>29)&1);
        return buf;
}

/* Cab Activation(FG23) Input/Output Signals */
char* Cab_Active_IO_Signals_status(void){

	static char buf[512];
	status_code_t fault = STATUS_SUCCESS;
	uint8_t a[13] = {4, 1, 4, 1, 0, 1, 22, 18, 2, 3, 4, 0, 1};
	uint32_t result =0;
	uint32_t val1, val2, fail_adds;
	uint8_t miss_flag_cab1 = 0, miss_flag_cab2  = 0;
	
	int ret =0;
	int res_pos =0;
	
	if (reg_read(fg23_IO_regs[0].base, fg23_IO_regs[0].offset, &val1) != 0) { fault = STATUS_ERR_HW_FAILED; fail_adds = 0xA1010000; goto result_lbl; }
	if (reg_read(fg23_IO_regs[1].base, fg23_IO_regs[1].offset, &val2) != 0) { fault = STATUS_ERR_HW_FAILED; fail_adds = 0xA1010004; goto result_lbl; }
	
	result |= (GET_BIT(val1, a[0]) << res_pos++);
	if (GET_BIT(val1, a[0]) != GET_BIT(val2, a[1])) { miss_flag_cab1 =1; }
    	
    	if (reg_read(fg23_IO_regs[2].base, fg23_IO_regs[2].offset, &val1) != 0) { fault = STATUS_ERR_HW_FAILED; fail_adds = 0xA1020000; goto result_lbl; }
	if (reg_read(fg23_IO_regs[3].base, fg23_IO_regs[3].offset, &val2) != 0) { fault = STATUS_ERR_HW_FAILED; fail_adds = 0xA1020004; goto result_lbl; }
	
	result |= (GET_BIT(val1, a[2]) << res_pos++);
	if (GET_BIT(val1, a[2]) != GET_BIT(val2, a[3])) { miss_flag_cab2 =2; }

	for(int i=4; i<13; i++){
		
	     ret = reg_read(fg23_IO_regs[i].base, fg23_IO_regs[i].offset, &val1);
             if (ret != 0) {
            	fault = STATUS_ERR_HW_FAILED;
            	fail_adds = fg23_IO_regs[i].base + fg23_IO_regs[i].offset;
            	goto result_lbl;
             }
              
             result |= (GET_BIT(val1, a[i]) << res_pos++);
        }
        				
    	if (miss_flag_cab1) result |= (1<<14);  //cab1 mismatch set 14th bit
    	if (miss_flag_cab2) result |= (1<<15);   //cab2 mismatch set 15th bit
	
    	result_lbl:
    	if (fault != STATUS_SUCCESS) {
    	    snprintf(buf, sizeof(buf),
        	"{ \"status\": %d, \"failed_reg\": \"0x%08X\" }",
        	fault, fail_adds);
	} else {
    	    snprintf(buf, sizeof(buf),
                "{ \"status\": %d, \"value\": \"0x%04X\" }",
        	fault, result);
	}

    	return buf;
}
	
char* Cab_Active_simulation_sig(uint32_t mode) {

	static char buf[256];
    	const uint32_t base = 0xA2023000, offset = 0x004, mask = (1U<<0);
    	uint32_t reg_val = 0, read_back;
    	status_code_t fault = STATUS_SUCCESS;

	if (mode > 1) {
        	fault = STATUS_ERR_INVALID_FLAG;
        	goto result;
    	}

        /* update */
        if ( reg_read(base, offset, &reg_val) !=0 ) {
       		fault = STATUS_ERR_HW_FAILED;
        	goto result;
    	}
    	
        reg_val = mode ? (reg_val| mask) : (reg_val& ~mask);
        reg_write(base, offset, reg_val);

        /* verify */
        reg_read(base, offset, &reg_val);
        read_back = (reg_val&mask) ? 1U : 0U;
        if(read_back != mode)
        	fault = STATUS_ERR_MISMATCH;
        
    	result:
    	snprintf(buf, sizeof(buf), 
    		"{ \"status\": %d }", 
    		fault);
             
    	return buf;
}
	
char* Cab_Active_param(uint32_t user_value) {
       
    static char buf[128];   
    status_code_t fault = STATUS_SUCCESS;

    uint32_t base =0xA5023000, offset =0x000, reg_val, mask =0x1F, read_back;

    // ---- Update Register ----
    if (reg_read(base, offset, &reg_val) != 0) {
    	fault = STATUS_ERR_HW_FAILED;
        goto result;
    }

    user_value &= mask;
    reg_val = (reg_val & ~mask) | user_value;
    reg_write(base, offset, reg_val);
    
    // ---- Read Back Verification ----
    reg_read(base, offset, &reg_val);
    read_back = reg_val&mask;

    if (read_back != user_value) 
    	fault = STATUS_ERR_MISMATCH;
    
    result:
    snprintf(buf, sizeof(buf), 
    	"{ \"status\": %d }", 
    	fault);
             
    return buf;
}

char* cab1_panelA_lamp_Status(void) {
	
	static char buf[256];
        uint32_t reg_val=0;
        uint32_t base = 0xA2010000;
        uint32_t offset = 0x000;
        status_code_t fault = STATUS_SUCCESS;
        
        //store result, note:[consider for TBD = 0th bit]
        uint8_t result=0, pos[8]= {0, 9, 0, 14, 0, 0, 1, 5};
        
        if( reg_read(base, offset, &reg_val) != 0) {
        	result = 0;
		fault = STATUS_ERR_HW_FAILED;
        }
        
        for(int i=0; i<8; i++) {
        	result |= GET_BIT(reg_val, pos[i]) <<i;
        }
	
	snprintf(buf, sizeof(buf),
                 "{ \"status\": %d, \"value\": \"0x%02x\" }",
                 fault, result);
            
        return buf;
}
	
char* cab2_panelA_lamp_Status(void) {
	
	static char buf[256];
        uint32_t reg_val=0;
        uint32_t base = 0xA2020000;
        uint32_t offset = 0x000;
        status_code_t fault = STATUS_SUCCESS;
        
        //store result, note:[consider for TBD = 0th bit]
        uint8_t result=0, pos[8]= {0, 9, 0, 14, 0, 0, 1, 5};
        
        if( reg_read(base, offset, &reg_val) != 0) {
        	result = 0;
		fault = STATUS_ERR_HW_FAILED;
        }
        
        for(int i=0; i<8; i++) {
        	result |= GET_BIT(reg_val, pos[i]) <<i;
        }
	
	snprintf(buf, sizeof(buf),
                 "{ \"status\": %d, \"value\": \"0x%02x\" }",
                 fault, result);
            
        return buf;
}

char* cab1_panelC_lamp_Status(void) {
	
	static char buf[256];
	uint32_t val1=0, val2=0, fail_adds =0;
        uint8_t miss_flag_cab1=0, miss_flag_cab2=0;
        status_code_t fault = STATUS_SUCCESS;
        
        //store result
        uint8_t result=0;
        uint8_t a[4]= {6, 1, 8, 0};
        int res_pos =0;
        
        if (reg_read(0xA2010000, 0x000, &val1) != 0) { fault = STATUS_ERR_HW_FAILED; fail_adds = 0xA2010000; goto result_lbl; }
	if (reg_read(0xA2010000, 0x004, &val2) != 0) { fault = STATUS_ERR_HW_FAILED; fail_adds = 0xA2010004; goto result_lbl; }
	
	result |= (GET_BIT(val1, a[0]) << res_pos++);
	result |= (GET_BIT(val1, a[2]) << res_pos++);
	
	if (GET_BIT(val1, a[0]) != GET_BIT(val2, a[1])) { miss_flag_cab1 =1; }
	if (GET_BIT(val1, a[2]) != GET_BIT(val2, a[3])) { miss_flag_cab1 =1; }
	
	if (miss_flag_cab1) result |= (1<<5);  //cab1 mismatch set 5th bit
    	if (miss_flag_cab2) result |= (1<<6);   //cab2 mismatch set 6th bit
    	
	result_lbl:
    	if (fault != STATUS_SUCCESS) {
    	    snprintf(buf, sizeof(buf),
        	"{ \"status\": %d, \"failed_reg\": \"0x%08X\" }",
        	fault, fail_adds);
	} else {
    	    snprintf(buf, sizeof(buf),
                "{ \"status\": %d, \"value\": \"0x%02X\" }",
        	fault, result);
	}

    	return buf;
}

char* cab2_panelC_lamp_Status(void) {
	
	static char buf[256];
	uint32_t val1=0, val2=0, fail_adds =0;
        uint8_t miss_flag_cab1=0, miss_flag_cab2=0;
        status_code_t fault = STATUS_SUCCESS;
        
        //store result
        uint8_t result=0;
        uint8_t a[4]= {7, 1, 8, 0};
        int res_pos =0;
        
        if (reg_read(0xA2020000, 0x000, &val1) != 0) { fault = STATUS_ERR_HW_FAILED; fail_adds = 0xA2020000; goto result_lbl; }
	if (reg_read(0xA2020000, 0x004, &val2) != 0) { fault = STATUS_ERR_HW_FAILED; fail_adds = 0xA2020004; goto result_lbl; }
	
	result |= (GET_BIT(val1, a[0]) << res_pos++);
	result |= (GET_BIT(val1, a[2]) << res_pos++);
	
	if (GET_BIT(val1, a[0]) != GET_BIT(val2, a[1])) { miss_flag_cab1 =1; }
	if (GET_BIT(val1, a[2]) != GET_BIT(val2, a[3])) { miss_flag_cab1 =1; }
	
	if (miss_flag_cab1) result |= (1<<5);  //cab1 mismatch set 5th bit
    	if (miss_flag_cab2) result |= (1<<6);   //cab2 mismatch set 6th bit
    	
	result_lbl:
    	if (fault != STATUS_SUCCESS) {
    	    snprintf(buf, sizeof(buf),
        	"{ \"status\": %d, \"failed_reg\": \"0x%08X\" }",
        	fault, fail_adds);
	} else {
    	    snprintf(buf, sizeof(buf),
                "{ \"status\": %d, \"value\": \"0x%02X\" }",
        	fault, result);
	}

    	return buf;
}

char* msc_node_status(void){

        static char buf[256];
	uint32_t value;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA2083000, 0x10C, &value) != 0) {
        	value = 0;
		fault = STATUS_ERR_HW_FAILED;
    	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"status\": %d, \"value\": \"%d\" }",
                 fault, (value>>16)&0xFFFF);
        return buf;
}


char* fault_code_status(void){

        static char buf[256];
	uint32_t value;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA2083000, 0x700, &value) != 0) {
        	value = 0;
		fault = STATUS_ERR_HW_FAILED;
    	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"status\": %d, \"value\": \"%d\" }",
                 fault, (value&0xFFFF));
        return buf;
}

char* tcu1line1_param_get(void) {
    
    static char buf[2048];
    int len = 0;
    uint32_t val = 0;
    uint32_t map_size = sizeof(tcu1_LC1_param_regs)/sizeof(tcu1_LC1_param_regs[0]);
    
    len += snprintf(buf + len, sizeof(buf) - len, "{\"status\": 0,");
    for (int i = 0; i < map_size; i++) {
	
        int ret = reg_read(tcu1_LC1_param_regs[i].base, tcu1_LC1_param_regs[i].offset, &val);
        if ( ret !=0) {
            snprintf(buf, sizeof(buf),
        	"{ \"status\": %d, \"failed_reg\": \"0x%08X\" }",
        	ret, tcu1_LC1_param_regs[i].base+tcu1_LC1_param_regs[i].offset);
                     
            return buf;
        }

        len += snprintf(buf + len, sizeof(buf) - len,
                        "\"%s\":\"0x%08X\"%s",
                        tcu1_LC1_param_regs[i].name,
                        val,
                        (i == map_size - 1) ? "" : ",");
    }

    snprintf(buf + len, sizeof(buf) - len, "}");
    return buf;
}

char* tcu1line2_param_get(void) {
    
    static char buf[2048];
    int len = 0;
    uint32_t val = 0;
    uint32_t map_size = sizeof(tcu1_LC2_param_regs)/sizeof(tcu1_LC2_param_regs[0]);
    
    len += snprintf(buf + len, sizeof(buf) - len, "{\"status\": 0,");
    for (int i = 0; i < map_size; i++) {
	
	
        int ret = reg_read(tcu1_LC2_param_regs[i].base, tcu1_LC2_param_regs[i].offset, &val);
        if ( ret !=0) {
            snprintf(buf, sizeof(buf),
        	"{ \"status\": %d, \"failed_reg\": \"0x%08X\" }",
        	ret, tcu1_LC2_param_regs[i].base+tcu1_LC2_param_regs[i].offset);
                     
            return buf;
        }
	
        len += snprintf(buf + len, sizeof(buf) - len,
                        "\"%s\":\"0x%08X\"%s",
                        tcu1_LC2_param_regs[i].name,
                        val,
                        (i == map_size - 1) ? "" : ",");
                
    }

    snprintf(buf + len, sizeof(buf) - len, "}");
    return buf;
}

char* tcu1motor1_param_get(void) {
    
    static char buf[2048];
    int len = 0;
    uint32_t val = 0;
    uint32_t map_size = sizeof(tcu1_MC1_param_regs)/sizeof(tcu1_MC1_param_regs[0]);
    
    len += snprintf(buf + len, sizeof(buf) - len, "{\"status\": 0,");
    for (int i = 0; i < map_size; i++) {
	
	
        int ret = reg_read(tcu1_MC1_param_regs[i].base, tcu1_MC1_param_regs[i].offset, &val);
        if ( ret !=0) {
            snprintf(buf, sizeof(buf),
        	"{ \"status\": %d, \"failed_reg\": \"0x%08X\" }",
        	ret, tcu1_MC1_param_regs[i].base+tcu1_MC1_param_regs[i].offset);
                     
            return buf;
        }
	
        len += snprintf(buf + len, sizeof(buf) - len,
                        "\"%s\":\"0x%08X\"%s",
                        tcu1_MC1_param_regs[i].name,
                        val,
                        (i == map_size - 1) ? "" : ",");
    }

    snprintf(buf + len, sizeof(buf) - len, "}");
    return buf;
}

char* tcu1motor2_param_get(void) {
    
    static char buf[2048];
    int len = 0;
    uint32_t val = 0;
    uint32_t map_size = sizeof(tcu1_MC2_param_regs)/sizeof(tcu1_MC2_param_regs[0]);
    
    len += snprintf(buf + len, sizeof(buf) - len, "{\"status\": 0,");
    for (int i = 0; i < map_size; i++) {
	
	
        int ret = reg_read(tcu1_MC2_param_regs[i].base, tcu1_MC2_param_regs[i].offset, &val);
        if ( ret !=0) {
            snprintf(buf, sizeof(buf),
        	"{ \"status\": %d, \"failed_reg\": \"0x%08X\" }",
        	ret, tcu1_MC2_param_regs[i].base+tcu1_MC2_param_regs[i].offset);
                     
            return buf;
        }
	
        len += snprintf(buf + len, sizeof(buf) - len,
                        "\"%s\":\"0x%08X\"%s",
                        tcu1_MC2_param_regs[i].name,
                        val,
                        (i == map_size - 1) ? "" : ",");              
    }

    snprintf(buf + len, sizeof(buf) - len, "}");
    return buf;
}

char* tcu1motor3_param_get(void) {
    
    static char buf[2048];
    int len = 0;
    uint32_t val = 0;
    uint32_t map_size = sizeof(tcu1_MC3_param_regs)/sizeof(tcu1_MC3_param_regs[0]);
    
    len += snprintf(buf + len, sizeof(buf) - len, "{\"status\": 0,");
    for (int i = 0; i < map_size; i++) {
	
	
        int ret = reg_read(tcu1_MC3_param_regs[i].base, tcu1_MC3_param_regs[i].offset, &val);
        if ( ret !=0) {
            snprintf(buf, sizeof(buf),
        	"{ \"status\": %d, \"failed_reg\": \"0x%08X\" }",
        	ret, tcu1_MC3_param_regs[i].base+tcu1_MC3_param_regs[i].offset);
                     
            return buf;
        }
	
        len += snprintf(buf + len, sizeof(buf) - len,
                        "\"%s\":\"0x%08X\"%s",
                        tcu1_MC3_param_regs[i].name,
                        val,
                        (i == map_size - 1) ? "" : ",");
    }

    snprintf(buf + len, sizeof(buf) - len, "}");
    return buf;
}

char* tcu1line1_status(void){

        static char buf[256];
	uint32_t value;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA3107000, 0x200, &value) != 0) {
        	value = 0;
		fault = STATUS_ERR_HW_FAILED;
    	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* tcu1line2_status(void){

        static char buf[256];
	uint32_t value;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA3107000, 0x500, &value) != 0) {
        	value = 0;
		fault = STATUS_ERR_HW_FAILED;
    	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* tcu1motor1_status(void){

        static char buf[256];
	uint32_t value;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA3107000, 0x800, &value) != 0) {
        	value = 0;
		fault = STATUS_ERR_HW_FAILED;
    	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* tcu1motor2_status(void){

        static char buf[256];
	uint32_t value;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA3107000, 0xB00, &value) != 0) {
        	value = 0;
		fault = STATUS_ERR_HW_FAILED;
    	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* tcu1motor3_status(void){

        static char buf[256];
	uint32_t value;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA3107000, 0xE00, &value) != 0) {
        	value = 0;
		fault = STATUS_ERR_HW_FAILED;
    	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* tcu1line1_fault(void){

        static char buf[256];
	uint32_t reg1_val, reg2_val;
	status_code_t fault = STATUS_SUCCESS;
	uint64_t combined;
	
	if( reg_read(0xA3107000, 0x300, &reg1_val) !=0 || reg_read(0xA3107000, 0x304, &reg2_val) != 0) {
        	combined = 0;
		fault = STATUS_ERR_HW_FAILED;
		goto result;
        }
    	
    	uint64_t part1 = (uint64_t)reg1_val;
	uint64_t part2 = (uint64_t)((reg2_val >> 21) & 0x7FF);
	combined = part1 | (part2 << 32);
        
        result:
        snprintf(buf, sizeof(buf),
         	"{ \"status\": %d, \"value\": \"0x%016" PRIX64 "\" }",
         	fault, combined);
        return buf;
}

char* tcu1line2_fault(void){

        static char buf[256];
	uint32_t reg1_val, reg2_val;
	status_code_t fault = STATUS_SUCCESS;
	uint64_t combined;
	
	if( reg_read(0xA3107000, 0x600, &reg1_val) != 0 || reg_read(0xA3107000, 0x604, &reg2_val) != 0) {
        	combined = 0;
		fault = STATUS_ERR_HW_FAILED;
		goto result;
        }
    	
    	uint64_t part1 = (uint64_t)reg1_val;
	uint64_t part2 = (uint64_t)((reg2_val >> 21) & 0x7FF);
	combined = part1 | (part2 << 32);
        
        result:
        snprintf(buf, sizeof(buf),
         	"{ \"status\": %d, \"value\": \"0x%016" PRIX64 "\" }",
         	fault, combined);
        return buf;
}

char* tcu1motor1_fault(void){

        static char buf[256];
	uint32_t value;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA3107000, 0x900, &value) != 0) {
        	value = 0;
		fault = STATUS_ERR_HW_FAILED;
    	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* tcu1motor2_fault(void){

        static char buf[256];
	uint32_t value;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA3107000, 0xC00, &value) != 0) {
       	 	value = 0;
		fault = STATUS_ERR_HW_FAILED;
    	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* tcu1motor3_fault(void){

        static char buf[256];
	uint32_t value;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA3107000, 0xF00, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}


int load_xml_and_map(void) {

    g_doc = xmlReadFile(XML_PATH, NULL, 0);
    if (!g_doc)
        return -1;

    xmlNodePtr root = xmlDocGetRootElement(g_doc);
    xmlNodePtr cur;
    signal_count = 0;
    
    for (cur = root->children; cur; cur = cur->next) {

        if (cur->type == XML_ELEMENT_NODE &&
            strcmp((char *)cur->name, "signal") == 0) {

            if (signal_count >= MAX_SIGNALS)
                break;

            signal_map[signal_count].id = signal_count;
            signal_map[signal_count].node = cur;

            signal_count++;
        }
    }
    return 0;
}

char* update_signal_xml(uint32_t signal, uint32_t value) {

    static char buf[128];
    status_code_t fault = STATUS_SUCCESS;

    if (signal >= signal_count) {
        fault = STATUS_ERR_INVALID_SIG;
        goto result;
    }

    xmlNodePtr node = signal_map[signal].node;

    char val_str[16];
    snprintf(val_str, sizeof(val_str), "%u", value);
    xmlSetProp(node, (xmlChar *)"value", (xmlChar *)val_str);

    // save file
    xmlSaveFormatFileEnc(XML_PATH, g_doc, "UTF-8", 1);

result:
    snprintf(buf, sizeof(buf),
             "{ \"status\": %d }",
             fault);

    return buf;
}

/* Fg88 */
char* fg88_motxtemp_uplwlimit(uint32_t upper_flag, uint32_t user_value) {
    
    static char buf[128];   
    status_code_t fault = STATUS_SUCCESS;

    uint32_t base =0xA5100000, offset =0x024, reg_val;
    uint16_t read_16;

    if (upper_flag > 1) {
        fault = STATUS_ERR_INVALID_FLAG;
        goto result;
    }

    // ---- Update Register ----
    if (reg_read(base, offset, &reg_val) != 0) {
    	fault = STATUS_ERR_HW_FAILED;
        goto result;
    }

    user_value &= 0xFFFF;
    if (upper_flag) {
        reg_val = (reg_val & 0x0000FFFF) | (user_value << 16);
    } else {
        reg_val = (reg_val & 0xFFFF0000) | user_value;
    }
    reg_write(base, offset, reg_val);
    
    // ---- Read Back Verification ----
    reg_read(base, offset, &reg_val);
    read_16 = upper_flag ? ((reg_val >> 16) & 0xFFFF) : (reg_val & 0xFFFF);

    if (read_16 != user_value) 
    	fault = STATUS_ERR_MISMATCH;
    
    result:
    snprintf(buf, sizeof(buf), 
    	"{ \"status\": %d }", 
    	fault);
             
    return buf;
}


/* FG85 (PARAMETERS FOR FG) */
char* fg_88878685_reg(uint32_t bit_pos, uint32_t mode) {
    
	static char buf[256];
    	const uint32_t base = 0xA5100000, offset = 0x008, mask = (1U<<bit_pos);
    	uint32_t reg_val = 0, read_back;
    	status_code_t fault = STATUS_SUCCESS;

	if (mode > 1) {
        	fault = STATUS_ERR_INVALID_FLAG;
        	goto result;
    	}

	if (bit_pos > 21 || !((1 << bit_pos) & 0x366000)) {
    		fault = STATUS_ERR_INVALID_BIT;
    		goto result;
	}
    	
        /* update */
        if ( reg_read(base, offset, &reg_val) != 0 ) {
       		fault = STATUS_ERR_HW_FAILED;
        	goto result;
    	}
    	
        reg_val = mode ? (reg_val| mask) : (reg_val& ~mask);
        reg_write(base, offset, reg_val);

        /* verify */
        reg_read(base, offset, &reg_val);
        read_back = (reg_val&mask) ? 1U : 0U;
        if(read_back != mode)
        	fault = STATUS_ERR_MISMATCH;
        
    	result:
    	snprintf(buf, sizeof(buf), 
    		"{ \"status\": %d }", 
    		fault);
             
    	return buf;
}

/* FG87 */
char* fg88_878685_reg(uint32_t bit_pos, uint32_t mode) {

	static char buf[256];
    	const uint32_t base = 0xA5100000, offset = 0x008, mask = (1U<<bit_pos);
    	uint32_t reg_val = 0, read_back;
    	status_code_t fault = STATUS_SUCCESS;

	if (mode > 1) {
        	fault = STATUS_ERR_INVALID_FLAG;
        	goto result;
    	}

	if (bit_pos > 21 || !((1 << bit_pos) & 0x2F000000)) {
    		fault = STATUS_ERR_INVALID_BIT;
    		goto result;
	}
    	
        /* update */
        if ( reg_read(base, offset, &reg_val) != 0 ) {
       		fault = STATUS_ERR_HW_FAILED;
        	goto result;
    	}
    	
        reg_val = mode ? (reg_val| mask) : (reg_val& ~mask);
        reg_write(base, offset, reg_val);

        /* verify */
        reg_read(base, offset, &reg_val);
        read_back = (reg_val&mask) ? 1U : 0U;
        if(read_back != mode)
        	fault = STATUS_ERR_MISMATCH;
        
    	result:
    	snprintf(buf, sizeof(buf), 
    		"{ \"status\": %d }", 
    		fault);
             
    	return buf;
}

char* fg87_dcinterlink_dclink_volt(uint32_t upper_flag, uint32_t user_value){

    static char buf[128];   
    status_code_t fault = STATUS_SUCCESS;

    uint32_t base =0xA5100000, offset =0x114, reg_val;
    uint16_t read_16;

    if (upper_flag > 1) {
        fault = STATUS_ERR_INVALID_FLAG;
        goto result;
    }

    // ---- Update Register ----
    if (reg_read(base, offset, &reg_val) != 0) {
    	fault = STATUS_ERR_HW_FAILED;
        goto result;
    }

    user_value &= 0xFFFF;
    if (upper_flag) {
        reg_val = (reg_val & 0x0000FFFF) | (user_value << 16);
    } else {
        reg_val = (reg_val & 0xFFFF0000) | user_value;
    }
    reg_write(base, offset, reg_val);
    
    // ---- Read Back Verification ----
    reg_read(base, offset, &reg_val);
    read_16 = upper_flag ? ((reg_val >> 16) & 0xFFFF) : (reg_val & 0xFFFF);

    if (read_16 != user_value) 
    	fault = STATUS_ERR_MISMATCH;
    
    result:
    snprintf(buf, sizeof(buf), 
    	"{ \"status\": %d }", 
    	fault);
             
    return buf;
}

char* fg87_dclink_primvolt_minmax(uint32_t upper_flag, uint32_t user_value){
    
    static char buf[128];   
    status_code_t fault = STATUS_SUCCESS;

    uint32_t base =0xA5100000, offset =0x118, reg_val;
    uint16_t read_16;

    if (upper_flag > 1) {
        fault = STATUS_ERR_INVALID_FLAG;
        goto result;
    }

    // ---- Update Register ----
    if (reg_read(base, offset, &reg_val) != 0) {
    	fault = STATUS_ERR_HW_FAILED;
        goto result;
    }

    user_value &= 0xFFFF;
    if (upper_flag) {
        reg_val = (reg_val & 0x0000FFFF) | (user_value << 16);
    } else {
        reg_val = (reg_val & 0xFFFF0000) | user_value;
    }
    reg_write(base, offset, reg_val);
    
    // ---- Read Back Verification ----
    reg_read(base, offset, &reg_val);
    read_16 = upper_flag ? ((reg_val >> 16) & 0xFFFF) : (reg_val & 0xFFFF);

    if (read_16 != user_value) 
    	fault = STATUS_ERR_MISMATCH;
    
    result:
    snprintf(buf, sizeof(buf), 
    	"{ \"status\": %d }", 
    	fault);
             
    return buf;
}

/* FG08 */
char* fg08_asc_deact(uint32_t bit_pos, uint32_t mode){
	
	static char buf[256];
    	const uint32_t base = 0xA5100000, offset = 0x278, mask = (1U<<bit_pos);
    	uint32_t reg_val = 0, read_back;
    	status_code_t fault = STATUS_SUCCESS;

	if (mode > 1) {
        	fault = STATUS_ERR_INVALID_FLAG;
        	goto result;
    	}

	if (bit_pos > 21 || !((1 << bit_pos) & 0x1E)) {
    		fault = STATUS_ERR_INVALID_BIT;
    		goto result;
	}
    	
        /* update */
        if ( reg_read(base, offset, &reg_val) != 0 ) {
       		fault = STATUS_ERR_HW_FAILED;
        	goto result;
    	}
    	
        reg_val = mode ? (reg_val| mask) : (reg_val& ~mask);
        reg_write(base, offset, reg_val);

        /* verify */
        reg_read(base, offset, &reg_val);
        read_back = (reg_val&mask) ? 1U : 0U;
        if(read_back != mode)
        	fault = STATUS_ERR_MISMATCH;
        
    	result:
    	snprintf(buf, sizeof(buf), 
    		"{ \"status\": %d }", 
    		fault);
             
    	return buf;
}

/* FG18 */
char* fg18_870991_reg(uint32_t bit_pos, uint32_t mode){
	
	static char buf[256];
    	const uint32_t base = 0xA5100000, offset = 0x100, mask = (1U<<bit_pos);
    	uint32_t reg_val = 0, read_back;
    	status_code_t fault = STATUS_SUCCESS;

	if (mode > 1) {
        	fault = STATUS_ERR_INVALID_FLAG;
        	goto result;
    	}

	if (bit_pos > 21 || !((1 << bit_pos) & 0x2)) {
    		fault = STATUS_ERR_INVALID_BIT;
    		goto result;
	}
    	
        /* update */
        if ( reg_read(base, offset, &reg_val) != 0 ) {
       		fault = STATUS_ERR_HW_FAILED;
        	goto result;
    	}
    	
        reg_val = mode ? (reg_val| mask) : (reg_val& ~mask);
        reg_write(base, offset, reg_val);

        /* verify */
        reg_read(base, offset, &reg_val);
        read_back = (reg_val&mask) ? 1U : 0U;
        if(read_back != mode)
        	fault = STATUS_ERR_MISMATCH;
        
    	result:
    	snprintf(buf, sizeof(buf), 
    		"{ \"status\": %d }", 
    		fault);
             
    	return buf;
}

/* FG09 */
char* fg09_18870991_reg(uint32_t bit_pos, uint32_t mode){
	
	static char buf[256];
    	const uint32_t base = 0xA5100000, offset = 0x100, mask = (1U<<bit_pos);
    	uint32_t reg_val = 0, read_back;
    	status_code_t fault = STATUS_SUCCESS;

	if (mode > 1) {
        	fault = STATUS_ERR_INVALID_FLAG;
        	goto result;
    	}

	if (bit_pos > 30 || !((1 << bit_pos) & 0x7DE20000)) {
    		fault = STATUS_ERR_INVALID_BIT;
    		goto result;
	}
    	
        /* update */
        if ( reg_read(base, offset, &reg_val) != 0 ) {
       		fault = STATUS_ERR_HW_FAILED;
        	goto result;
    	}
    	
        reg_val = mode ? (reg_val| mask) : (reg_val& ~mask);
        reg_write(base, offset, reg_val);

        /* verify */
        reg_read(base, offset, &reg_val);
        read_back = (reg_val&mask) ? 1U : 0U;
        if(read_back != mode)
        	fault = STATUS_ERR_MISMATCH;
        
    	result:
    	snprintf(buf, sizeof(buf), 
    		"{ \"status\": %d }", 
    		fault);
             
    	return buf;
}

char* fg09_identprojtemp_oillmt(uint32_t upper_flag, uint32_t user_value){
    
    static char buf[128];   
    status_code_t fault = STATUS_SUCCESS;

    uint32_t base =0xA5100000, offset =0x144, reg_val;
    uint16_t read_16;

    if (upper_flag > 1) {
        fault = STATUS_ERR_INVALID_FLAG;
        goto result;
    }

    // ---- Update Register ----
    if (reg_read(base, offset, &reg_val) != 0) {
    	fault = STATUS_ERR_HW_FAILED;
        goto result;
    }

    user_value &= 0xFFFF;
    if (upper_flag) {
        reg_val = (reg_val & 0x0000FFFF) | (user_value << 16);
    } else {
        reg_val = (reg_val & 0xFFFF0000) | user_value;
    }
    reg_write(base, offset, reg_val);
    
    // ---- Read Back Verification ----
    reg_read(base, offset, &reg_val);
    read_16 = upper_flag ? ((reg_val >> 16) & 0xFFFF) : (reg_val & 0xFFFF);

    if (read_16 != user_value) 
    	fault = STATUS_ERR_MISMATCH;
    
    result:
    snprintf(buf, sizeof(buf), 
    	"{ \"status\": %d }", 
    	fault);
             
    return buf;
}

char* fg09_srpwr_rdctn_startend(uint32_t upper_flag, uint32_t user_value){
    
    static char buf[128];   
    status_code_t fault = STATUS_SUCCESS;

    uint32_t base =0xA5100000, offset =0x148, reg_val;
    uint16_t read_16;

    if (upper_flag > 1) {
        fault = STATUS_ERR_INVALID_FLAG;
        goto result;
    }

    // ---- Update Register ----
    if (reg_read(base, offset, &reg_val) != 0) {
    	fault = STATUS_ERR_HW_FAILED;
        goto result;
    }

    user_value &= 0xFFFF;
    if (upper_flag) {
        reg_val = (reg_val & 0x0000FFFF) | (user_value << 16);
    } else {
        reg_val = (reg_val & 0xFFFF0000) | user_value;
    }
    reg_write(base, offset, reg_val);
    
    // ---- Read Back Verification ----
    reg_read(base, offset, &reg_val);
    read_16 = upper_flag ? ((reg_val >> 16) & 0xFFFF) : (reg_val & 0xFFFF);

    if (read_16 != user_value) 
    	fault = STATUS_ERR_MISMATCH;
    
    result:
    snprintf(buf, sizeof(buf), 
    	"{ \"status\": %d }", 
    	fault);
             
    return buf;
}

char* fg09_troiltemp_lmtpwr_rdctn_srt(uint32_t upper_flag, uint32_t user_value){
    
    static char buf[128];   
    status_code_t fault = STATUS_SUCCESS;

    uint32_t base =0xA5100000, offset =0x150, reg_val;
    uint16_t read_16;

    if (upper_flag > 1) {
        fault = STATUS_ERR_INVALID_FLAG;
        goto result;
    }

    // ---- Update Register ----
    if (reg_read(base, offset, &reg_val) != 0) {
    	fault = STATUS_ERR_HW_FAILED;
        goto result;
    }

    user_value &= 0xFFFF;
    if (upper_flag) {
        reg_val = (reg_val & 0x0000FFFF) | (user_value << 16);
    } else {
        reg_val = (reg_val & 0xFFFF0000) | user_value;
    }
    reg_write(base, offset, reg_val);
    
    // ---- Read Back Verification ----
    reg_read(base, offset, &reg_val);
    read_16 = upper_flag ? ((reg_val >> 16) & 0xFFFF) : (reg_val & 0xFFFF);

    if (read_16 != user_value) 
    	fault = STATUS_ERR_MISMATCH;
    
    result:
    snprintf(buf, sizeof(buf), 
    	"{ \"status\": %d }", 
    	fault);
             
    return buf;
}

char* fg09_trpwr_rdctn_oiltemp_endtsk_time1(uint32_t upper_flag, uint32_t user_value){
    static char buf[128];   
    status_code_t fault = STATUS_SUCCESS;

    uint32_t base =0xA5100000, offset =0x154, reg_val;
    uint16_t read_16;

    if (upper_flag > 1) {
        fault = STATUS_ERR_INVALID_FLAG;
        goto result;
    }

    // ---- Update Register ----
    if (reg_read(base, offset, &reg_val) != 0) {
    	fault = STATUS_ERR_HW_FAILED;
        goto result;
    }

    user_value &= 0xFFFF;
    if (upper_flag) {
        reg_val = (reg_val & 0x0000FFFF) | (user_value << 16);
    } else {
        reg_val = (reg_val & 0xFFFF0000) | user_value;
    }
    reg_write(base, offset, reg_val);
    
    // ---- Read Back Verification ----
    reg_read(base, offset, &reg_val);
    read_16 = upper_flag ? ((reg_val >> 16) & 0xFFFF) : (reg_val & 0xFFFF);

    if (read_16 != user_value) 
    	fault = STATUS_ERR_MISMATCH;
    
    result:
    snprintf(buf, sizeof(buf), 
    	"{ \"status\": %d }", 
    	fault);
             
    return buf;
}

char* fg09_trpa_maxlimit_mottemp_lmt(uint32_t upper_flag, uint32_t user_value){
    static char buf[128];   
    status_code_t fault = STATUS_SUCCESS;

    uint32_t base =0xA5100000, offset =0x160, reg_val;
    uint16_t read_16;

    if (upper_flag > 1) {
        fault = STATUS_ERR_INVALID_FLAG;
        goto result;
    }

    // ---- Update Register ----
    if (reg_read(base, offset, &reg_val) != 0) {
    	fault = STATUS_ERR_HW_FAILED;
        goto result;
    }

    user_value &= 0xFFFF;
    if (upper_flag) {
        reg_val = (reg_val & 0x0000FFFF) | (user_value << 16);
    } else {
        reg_val = (reg_val & 0xFFFF0000) | user_value;
    }
    reg_write(base, offset, reg_val);
    
    // ---- Read Back Verification ----
    reg_read(base, offset, &reg_val);
    read_16 = upper_flag ? ((reg_val >> 16) & 0xFFFF) : (reg_val & 0xFFFF);

    if (read_16 != user_value) 
    	fault = STATUS_ERR_MISMATCH;
    
    result:
    snprintf(buf, sizeof(buf), 
    	"{ \"status\": %d }", 
    	fault);
             
    return buf;
}

char* fg09_motlower_temptask_timelimit(uint32_t upper_flag, uint32_t user_value){
    static char buf[128];   
    status_code_t fault = STATUS_SUCCESS;

    uint32_t base =0xA5100000, offset =0x164, reg_val;
    uint16_t read_16;

    if (upper_flag > 1) {
        fault = STATUS_ERR_INVALID_FLAG;
        goto result;
    }

    // ---- Update Register ----
    if (reg_read(base, offset, &reg_val) != 0) {
    	fault = STATUS_ERR_HW_FAILED;
        goto result;
    }

    user_value &= 0xFFFF;
    if (upper_flag) {
        reg_val = (reg_val & 0x0000FFFF) | (user_value << 16);
    } else {
        reg_val = (reg_val & 0xFFFF0000) | user_value;
    }
    reg_write(base, offset, reg_val);
    
    // ---- Read Back Verification ----
    reg_read(base, offset, &reg_val);
    read_16 = upper_flag ? ((reg_val >> 16) & 0xFFFF) : (reg_val & 0xFFFF);

    if (read_16 != user_value) 
    	fault = STATUS_ERR_MISMATCH;
    
    result:
    snprintf(buf, sizeof(buf), 
    	"{ \"status\": %d }", 
    	fault);
             
    return buf;
}
 
char* fg09_mottemp_lvl1levl2(uint32_t upper_flag, uint32_t user_value){
    static char buf[128];   
    status_code_t fault = STATUS_SUCCESS;

    uint32_t base =0xA5100000, offset =0x168, reg_val;
    uint16_t read_16;

    if (upper_flag > 1) {
        fault = STATUS_ERR_INVALID_FLAG;
        goto result;
    }

    // ---- Update Register ----
    if (reg_read(base, offset, &reg_val) != 0) {
    	fault = STATUS_ERR_HW_FAILED;
        goto result;
    }

    user_value &= 0xFFFF;
    if (upper_flag) {
        reg_val = (reg_val & 0x0000FFFF) | (user_value << 16);
    } else {
        reg_val = (reg_val & 0xFFFF0000) | user_value;
    }
    reg_write(base, offset, reg_val);
    
    // ---- Read Back Verification ----
    reg_read(base, offset, &reg_val);
    read_16 = upper_flag ? ((reg_val >> 16) & 0xFFFF) : (reg_val & 0xFFFF);

    if (read_16 != user_value) 
    	fault = STATUS_ERR_MISMATCH;
    
    result:
    snprintf(buf, sizeof(buf), 
    	"{ \"status\": %d }", 
    	fault);
             
    return buf;
}

char* fg09_mottemp_lvl3trtemplvl3(uint32_t upper_flag, uint32_t user_value){
    static char buf[128];   
    status_code_t fault = STATUS_SUCCESS;

    uint32_t base =0xA5100000, offset =0x16C, reg_val;
    uint16_t read_16;

    if (upper_flag > 1) {
        fault = STATUS_ERR_INVALID_FLAG;
        goto result;
    }

    // ---- Update Register ----
    if (reg_read(base, offset, &reg_val) != 0) {
    	fault = STATUS_ERR_HW_FAILED;
        goto result;
    }

    user_value &= 0xFFFF;
    if (upper_flag) {
        reg_val = (reg_val & 0x0000FFFF) | (user_value << 16);
    } else {
        reg_val = (reg_val & 0xFFFF0000) | user_value;
    }
    reg_write(base, offset, reg_val);
    
    // ---- Read Back Verification ----
    reg_read(base, offset, &reg_val);
    read_16 = upper_flag ? ((reg_val >> 16) & 0xFFFF) : (reg_val & 0xFFFF);

    if (read_16 != user_value) 
    	fault = STATUS_ERR_MISMATCH;
    
    result:
    snprintf(buf, sizeof(buf), 
    	"{ \"status\": %d }", 
    	fault);
             
    return buf;
}

char* fg09_trtemp_lvl1levl2(uint32_t upper_flag, uint32_t user_value){
    static char buf[128];   
    status_code_t fault = STATUS_SUCCESS;

    uint32_t base =0xA5100000, offset =0x170, reg_val;
    uint16_t read_16;

    if (upper_flag > 1) {
        fault = STATUS_ERR_INVALID_FLAG;
        goto result;
    }

    // ---- Update Register ----
    if (reg_read(base, offset, &reg_val) != 0) {
    	fault = STATUS_ERR_HW_FAILED;
        goto result;
    }

    user_value &= 0xFFFF;
    if (upper_flag) {
        reg_val = (reg_val & 0x0000FFFF) | (user_value << 16);
    } else {
        reg_val = (reg_val & 0xFFFF0000) | user_value;
    }
    reg_write(base, offset, reg_val);
    
    // ---- Read Back Verification ----
    reg_read(base, offset, &reg_val);
    read_16 = upper_flag ? ((reg_val >> 16) & 0xFFFF) : (reg_val & 0xFFFF);

    if (read_16 != user_value) 
    	fault = STATUS_ERR_MISMATCH;
    
    result:
    snprintf(buf, sizeof(buf), 
    	"{ \"status\": %d }", 
    	fault);
             
    return buf;
}

char* fg09_srtemp_lvl1levl2(uint32_t upper_flag, uint32_t user_value){
    static char buf[128];   
    status_code_t fault = STATUS_SUCCESS;

    uint32_t base =0xA5100000, offset =0x174, reg_val;
    uint16_t read_16;

    if (upper_flag > 1) {
        fault = STATUS_ERR_INVALID_FLAG;
        goto result;
    }

    // ---- Update Register ----
    if (reg_read(base, offset, &reg_val) != 0) {
    	fault = STATUS_ERR_HW_FAILED;
        goto result;
    }

    user_value &= 0xFFFF;
    if (upper_flag) {
        reg_val = (reg_val & 0x0000FFFF) | (user_value << 16);
    } else {
        reg_val = (reg_val & 0xFFFF0000) | user_value;
    }
    reg_write(base, offset, reg_val);
    
    // ---- Read Back Verification ----
    reg_read(base, offset, &reg_val);
    read_16 = upper_flag ? ((reg_val >> 16) & 0xFFFF) : (reg_val & 0xFFFF);

    if (read_16 != user_value) 
    	fault = STATUS_ERR_MISMATCH;
    
    result:
    snprintf(buf, sizeof(buf), 
    	"{ \"status\": %d }", 
    	fault);
             
    return buf;
}

char* fg09_srtemplvl3_swtimefor_lvl2t0lvl3(uint32_t upper_flag, uint32_t user_value){
    static char buf[128];   
    status_code_t fault = STATUS_SUCCESS;

    uint32_t base =0xA5100000, offset =0x178, reg_val;
    uint16_t read_16;

    if (upper_flag > 1) {
        fault = STATUS_ERR_INVALID_FLAG;
        goto result;
    }

    // ---- Update Register ----
    if (reg_read(base, offset, &reg_val) != 0) {
    	fault = STATUS_ERR_HW_FAILED;
        goto result;
    }

    user_value &= 0xFFFF;
    if (upper_flag) {
        reg_val = (reg_val & 0x0000FFFF) | (user_value << 16);
    } else {
        reg_val = (reg_val & 0xFFFF0000) | user_value;
    }
    reg_write(base, offset, reg_val);
    
    // ---- Read Back Verification ----
    reg_read(base, offset, &reg_val);
    read_16 = upper_flag ? ((reg_val >> 16) & 0xFFFF) : (reg_val & 0xFFFF);

    if (read_16 != user_value) 
    	fault = STATUS_ERR_MISMATCH;
    
    result:
    snprintf(buf, sizeof(buf), 
    	"{ \"status\": %d }", 
    	fault);
             
    return buf;
}

char* fg09_pwrconv_templimit_uprvolt_lmt1(uint32_t upper_flag, uint32_t user_value){
    static char buf[128];   
    status_code_t fault = STATUS_SUCCESS;

    uint32_t base =0xA5100000, offset =0x180, reg_val;
    uint16_t read_16;

    if (upper_flag > 1) {
        fault = STATUS_ERR_INVALID_FLAG;
        goto result;
    }

    // ---- Update Register ----
    if (reg_read(base, offset, &reg_val) != 0) {
    	fault = STATUS_ERR_HW_FAILED;
        goto result;
    }

    user_value &= 0xFFFF;
    if (upper_flag) {
        reg_val = (reg_val & 0x0000FFFF) | (user_value << 16);
    } else {
        reg_val = (reg_val & 0xFFFF0000) | user_value;
    }
    reg_write(base, offset, reg_val);
    
    // ---- Read Back Verification ----
    reg_read(base, offset, &reg_val);
    read_16 = upper_flag ? ((reg_val >> 16) & 0xFFFF) : (reg_val & 0xFFFF);

    if (read_16 != user_value) 
    	fault = STATUS_ERR_MISMATCH;
    
    result:
    snprintf(buf, sizeof(buf), 
    	"{ \"status\": %d }", 
    	fault);
             
    return buf;
}

char* fg09_uprvolt_limit2_2bhoge_opcrnt(uint32_t upper_flag, uint32_t user_value){
    static char buf[128];   
    status_code_t fault = STATUS_SUCCESS;

    uint32_t base =0xA5100000, offset =0x184, reg_val;
    uint16_t read_16;

    if (upper_flag > 1) {
        fault = STATUS_ERR_INVALID_FLAG;
        goto result;
    }

    // ---- Update Register ----
    if (reg_read(base, offset, &reg_val) != 0) {
    	fault = STATUS_ERR_HW_FAILED;
        goto result;
    }

    user_value &= 0xFFFF;
    if (upper_flag) {
        reg_val = (reg_val & 0x0000FFFF) | (user_value << 16);
    } else {
        reg_val = (reg_val & 0xFFFF0000) | user_value;
    }
    reg_write(base, offset, reg_val);
    
    // ---- Read Back Verification ----
    reg_read(base, offset, &reg_val);
    read_16 = upper_flag ? ((reg_val >> 16) & 0xFFFF) : (reg_val & 0xFFFF);

    if (read_16 != user_value) 
    	fault = STATUS_ERR_MISMATCH;
    
    result:
    snprintf(buf, sizeof(buf), 
    	"{ \"status\": %d }", 
    	fault);
             
    return buf;
}

char* fg09_pri_voltlimit_minvoltlimit(uint32_t upper_flag, uint32_t user_value){
    static char buf[128];   
    status_code_t fault = STATUS_SUCCESS;

    uint32_t base =0xA5100000, offset =0x188, reg_val;
    uint16_t read_16;

    if (upper_flag > 1) {
        fault = STATUS_ERR_INVALID_FLAG;
        goto result;
    }

    // ---- Update Register ----
    if (reg_read(base, offset, &reg_val) != 0) {
    	fault = STATUS_ERR_HW_FAILED;
        goto result;
    }

    user_value &= 0xFFFF;
    if (upper_flag) {
        reg_val = (reg_val & 0x0000FFFF) | (user_value << 16);
    } else {
        reg_val = (reg_val & 0xFFFF0000) | user_value;
    }
    reg_write(base, offset, reg_val);
    
    // ---- Read Back Verification ----
    reg_read(base, offset, &reg_val);
    read_16 = upper_flag ? ((reg_val >> 16) & 0xFFFF) : (reg_val & 0xFFFF);

    if (read_16 != user_value) 
    	fault = STATUS_ERR_MISMATCH;
    
    result:
    snprintf(buf, sizeof(buf), 
    	"{ \"status\": %d }", 
    	fault);
             
    return buf;
}

char* fg09_inrush_crntlimit_pwrcon_tasktime(uint32_t upper_flag, uint32_t user_value){
    static char buf[128];   
    status_code_t fault = STATUS_SUCCESS;

    uint32_t base =0xA5100000, offset =0x194, reg_val;
    uint16_t read_16;

    if (upper_flag > 1) {
        fault = STATUS_ERR_INVALID_FLAG;
        goto result;
    }

    // ---- Update Register ----
    if (reg_read(base, offset, &reg_val) != 0) {
    	fault = STATUS_ERR_HW_FAILED;
        goto result;
    }

    user_value &= 0xFFFF;
    if (upper_flag) {
        reg_val = (reg_val & 0x0000FFFF) | (user_value << 16);
    } else {
        reg_val = (reg_val & 0xFFFF0000) | user_value;
    }
    reg_write(base, offset, reg_val);
    
    // ---- Read Back Verification ----
    reg_read(base, offset, &reg_val);
    read_16 = upper_flag ? ((reg_val >> 16) & 0xFFFF) : (reg_val & 0xFFFF);

    if (read_16 != user_value) 
    	fault = STATUS_ERR_MISMATCH;
    
    result:
    snprintf(buf, sizeof(buf), 
    	"{ \"status\": %d }", 
    	fault);
             
    return buf;
}

char* fg09_inrush_crnttime_limithotelid_maxlim(uint32_t upper_flag, uint32_t user_value){
    static char buf[128];   
    status_code_t fault = STATUS_SUCCESS;

    uint32_t base =0xA5100000, offset =0x198, reg_val;
    uint16_t read_16;

    if (upper_flag > 1) {
        fault = STATUS_ERR_INVALID_FLAG;
        goto result;
    }

    // ---- Update Register ----
    if (reg_read(base, offset, &reg_val) != 0) {
    	fault = STATUS_ERR_HW_FAILED;
        goto result;
    }

    user_value &= 0xFFFF;
    if (upper_flag) {
        reg_val = (reg_val & 0x0000FFFF) | (user_value << 16);
    } else {
        reg_val = (reg_val & 0xFFFF0000) | user_value;
    }
    reg_write(base, offset, reg_val);
    
    // ---- Read Back Verification ----
    reg_read(base, offset, &reg_val);
    read_16 = upper_flag ? ((reg_val >> 16) & 0xFFFF) : (reg_val & 0xFFFF);

    if (read_16 != user_value) 
    	fault = STATUS_ERR_MISMATCH;
    
    result:
    snprintf(buf, sizeof(buf), 
    	"{ \"status\": %d }", 
    	fault);
             
    return buf;
}

char* fg09_wttime_forvcb0_zsslimit_burmx_crntlim(uint32_t upper_flag, uint32_t user_value){
    static char buf[128];   
    status_code_t fault = STATUS_SUCCESS;

    uint32_t base =0xA5100000, offset =0x19C, reg_val;
    uint16_t read_16;

    if (upper_flag > 1) {
        fault = STATUS_ERR_INVALID_FLAG;
        goto result;
    }

    // ---- Update Register ----
    if (reg_read(base, offset, &reg_val) != 0) {
    	fault = STATUS_ERR_HW_FAILED;
        goto result;
    }

    user_value &= 0xFFFF;
    if (upper_flag) {
        reg_val = (reg_val & 0x0000FFFF) | (user_value << 16);
    } else {
        reg_val = (reg_val & 0xFFFF0000) | user_value;
    }
    reg_write(base, offset, reg_val);
    
    // ---- Read Back Verification ----
    reg_read(base, offset, &reg_val);
    read_16 = upper_flag ? ((reg_val >> 16) & 0xFFFF) : (reg_val & 0xFFFF);

    if (read_16 != user_value) 
    	fault = STATUS_ERR_MISMATCH;
    
    result:
    snprintf(buf, sizeof(buf), 
    	"{ \"status\": %d }", 
    	fault);
             
    return buf;
}

char* fg09_wttimeforbur_vcbonhrmnic_liltermx_crnt(uint32_t upper_flag, uint32_t user_value){
    static char buf[128];   
    status_code_t fault = STATUS_SUCCESS;

    uint32_t base =0xA5100000, offset =0x1A0, reg_val;
    uint16_t read_16;

    if (upper_flag > 1) {
        fault = STATUS_ERR_INVALID_FLAG;
        goto result;
    }

    // ---- Update Register ----
    if (reg_read(base, offset, &reg_val) != 0) {
    	fault = STATUS_ERR_HW_FAILED;
        goto result;
    }

    user_value &= 0xFFFF;
    if (upper_flag) {
        reg_val = (reg_val & 0x0000FFFF) | (user_value << 16);
    } else {
        reg_val = (reg_val & 0xFFFF0000) | user_value;
    }
    reg_write(base, offset, reg_val);
    
    // ---- Read Back Verification ----
    reg_read(base, offset, &reg_val);
    read_16 = upper_flag ? ((reg_val >> 16) & 0xFFFF) : (reg_val & 0xFFFF);

    if (read_16 != user_value) 
    	fault = STATUS_ERR_MISMATCH;
    
    result:
    snprintf(buf, sizeof(buf), 
    	"{ \"status\": %d }", 
    	fault);
             
    return buf;
}

char* fg09_maxdc_intvolt_cocohrmnc_fltrinrsh_crnt(uint32_t upper_flag, uint32_t user_value){
    static char buf[128];   
    status_code_t fault = STATUS_SUCCESS;

    uint32_t base =0xA5100000, offset =0x1A4, reg_val;
    uint16_t read_16;

    if (upper_flag > 1) {
        fault = STATUS_ERR_INVALID_FLAG;
        goto result;
    }

    // ---- Update Register ----
    if (reg_read(base, offset, &reg_val) != 0) {
    	fault = STATUS_ERR_HW_FAILED;
        goto result;
    }

    user_value &= 0xFFFF;
    if (upper_flag) {
        reg_val = (reg_val & 0x0000FFFF) | (user_value << 16);
    } else {
        reg_val = (reg_val & 0xFFFF0000) | user_value;
    }
    reg_write(base, offset, reg_val);
    
    // ---- Read Back Verification ----
    reg_read(base, offset, &reg_val);
    read_16 = upper_flag ? ((reg_val >> 16) & 0xFFFF) : (reg_val & 0xFFFF);

    if (read_16 != user_value) 
    	fault = STATUS_ERR_MISMATCH;
    
    result:
    snprintf(buf, sizeof(buf), 
    	"{ \"status\": %d }", 
    	fault);
             
    return buf;
}

char* fg09_dcinrlnk_lmt1(uint32_t upper_flag, uint32_t user_value){
    static char buf[128];   
    status_code_t fault = STATUS_SUCCESS;

    uint32_t base =0xA5100000, offset =0x1AC, reg_val;
    uint16_t read_16;

    if (upper_flag > 1) {
        fault = STATUS_ERR_INVALID_FLAG;
        goto result;
    }

    // ---- Update Register ----
    if (reg_read(base, offset, &reg_val) != 0) {
    	fault = STATUS_ERR_HW_FAILED;
        goto result;
    }

    user_value &= 0xFFFF;
    if (upper_flag) {
        reg_val = (reg_val & 0x0000FFFF) | (user_value << 16);
    } else {
        reg_val = (reg_val & 0xFFFF0000) | user_value;
    }
    reg_write(base, offset, reg_val);
    
    // ---- Read Back Verification ----
    reg_read(base, offset, &reg_val);
    read_16 = upper_flag ? ((reg_val >> 16) & 0xFFFF) : (reg_val & 0xFFFF);

    if (read_16 != user_value) 
    	fault = STATUS_ERR_MISMATCH;
    
    result:
    snprintf(buf, sizeof(buf), 
    	"{ \"status\": %d }", 
    	fault);
             
    return buf;
}

char* fg09_dcinrlnk_lmt2(uint32_t upper_flag, uint32_t user_value){
    static char buf[128];   
    status_code_t fault = STATUS_SUCCESS;

    uint32_t base =0xA5100000, offset =0x1B0, reg_val;
    uint16_t read_16;

    if (upper_flag > 1) {
        fault = STATUS_ERR_INVALID_FLAG;
        goto result;
    }

    // ---- Update Register ----
    if (reg_read(base, offset, &reg_val) != 0) {
    	fault = STATUS_ERR_HW_FAILED;
        goto result;
    }

    user_value &= 0xFFFF;
    if (upper_flag) {
        reg_val = (reg_val & 0x0000FFFF) | (user_value << 16);
    } else {
        reg_val = (reg_val & 0xFFFF0000) | user_value;
    }
    reg_write(base, offset, reg_val);
    
    // ---- Read Back Verification ----
    reg_read(base, offset, &reg_val);
    read_16 = upper_flag ? ((reg_val >> 16) & 0xFFFF) : (reg_val & 0xFFFF);

    if (read_16 != user_value) 
    	fault = STATUS_ERR_MISMATCH;
    
    result:
    snprintf(buf, sizeof(buf), 
    	"{ \"status\": %d }", 
    	fault);
             
    return buf;
}

char* fg09_dcinrlnk_lmt3(uint32_t upper_flag, uint32_t user_value){
    static char buf[128];   
    status_code_t fault = STATUS_SUCCESS;

    uint32_t base =0xA5100000, offset =0x1B4, reg_val;
    uint16_t read_16;

    if (upper_flag > 1) {
        fault = STATUS_ERR_INVALID_FLAG;
        goto result;
    }

    // ---- Update Register ----
    if (reg_read(base, offset, &reg_val) != 0) {
    	fault = STATUS_ERR_HW_FAILED;
        goto result;
    }

    user_value &= 0xFFFF;
    if (upper_flag) {
        reg_val = (reg_val & 0x0000FFFF) | (user_value << 16);
    } else {
        reg_val = (reg_val & 0xFFFF0000) | user_value;
    }
    reg_write(base, offset, reg_val);
    
    // ---- Read Back Verification ----
    reg_read(base, offset, &reg_val);
    read_16 = upper_flag ? ((reg_val >> 16) & 0xFFFF) : (reg_val & 0xFFFF);

    if (read_16 != user_value) 
    	fault = STATUS_ERR_MISMATCH;
    
    result:
    snprintf(buf, sizeof(buf), 
    	"{ \"status\": %d }", 
    	fault);
             
    return buf;
}

char* fg09_grndtimelmt_intrmddclink_chrgiglmt(uint32_t upper_flag, uint32_t user_value){
    static char buf[128];   
    status_code_t fault = STATUS_SUCCESS;

    uint32_t base =0xA5100000, offset =0x1B8, reg_val;
    uint16_t read_16;

    if (upper_flag > 1) {
        fault = STATUS_ERR_INVALID_FLAG;
        goto result;
    }

    // ---- Update Register ----
    if (reg_read(base, offset, &reg_val) != 0) {
    	fault = STATUS_ERR_HW_FAILED;
        goto result;
    }

    user_value &= 0xFFFF;
    if (upper_flag) {
        reg_val = (reg_val & 0x0000FFFF) | (user_value << 16);
    } else {
        reg_val = (reg_val & 0xFFFF0000) | user_value;
    }
    reg_write(base, offset, reg_val);
    
    // ---- Read Back Verification ----
    reg_read(base, offset, &reg_val);
    read_16 = upper_flag ? ((reg_val >> 16) & 0xFFFF) : (reg_val & 0xFFFF);

    if (read_16 != user_value) 
    	fault = STATUS_ERR_MISMATCH;
    
    result:
    snprintf(buf, sizeof(buf), 
    	"{ \"status\": %d }", 
    	fault);
             
    return buf;
}

char* fg09_tnfrratio_prmyvolt_factlmt(uint32_t upper_flag, uint32_t user_value){
    static char buf[128];   
    status_code_t fault = STATUS_SUCCESS;

    uint32_t base =0xA5100000, offset =0x1C0, reg_val;
    uint16_t read_16;

    if (upper_flag > 1) {
        fault = STATUS_ERR_INVALID_FLAG;
        goto result;
    }

    // ---- Update Register ----
    if (reg_read(base, offset, &reg_val) != 0) {
    	fault = STATUS_ERR_HW_FAILED;
        goto result;
    }

    user_value &= 0xFFFF;
    if (upper_flag) {
        reg_val = (reg_val & 0x0000FFFF) | (user_value << 16);
    } else {
        reg_val = (reg_val & 0xFFFF0000) | user_value;
    }
    reg_write(base, offset, reg_val);
    
    // ---- Read Back Verification ----
    reg_read(base, offset, &reg_val);
    read_16 = upper_flag ? ((reg_val >> 16) & 0xFFFF) : (reg_val & 0xFFFF);

    if (read_16 != user_value) 
    	fault = STATUS_ERR_MISMATCH;
    
    result:
    snprintf(buf, sizeof(buf), 
    	"{ \"status\": %d }", 
    	fault);
             
    return buf;
}

char* fg09_spdsnrp7_grratio_avgwhl_dia(uint32_t upper_flag, uint32_t user_value){
    static char buf[128];   
    status_code_t fault = STATUS_SUCCESS;

    uint32_t base =0xA5100000, offset =0x0F8, reg_val;
    uint16_t read_16;

    if (upper_flag > 1) {
        fault = STATUS_ERR_INVALID_FLAG;
        goto result;
    }

    // ---- Update Register ----
    if (reg_read(base, offset, &reg_val) != 0) {
    	fault = STATUS_ERR_HW_FAILED;
        goto result;
    }

    user_value &= 0xFFFF;
    if (upper_flag) {
        reg_val = (reg_val & 0x0000FFFF) | (user_value << 16);
    } else {
        reg_val = (reg_val & 0xFFFF0000) | user_value;
    }
    reg_write(base, offset, reg_val);
    
    // ---- Read Back Verification ----
    reg_read(base, offset, &reg_val);
    read_16 = upper_flag ? ((reg_val >> 16) & 0xFFFF) : (reg_val & 0xFFFF);

    if (read_16 != user_value) 
    	fault = STATUS_ERR_MISMATCH;
    
    result:
    snprintf(buf, sizeof(buf), 
    	"{ \"status\": %d }", 
    	fault);
             
    return buf;
}

char* fg09_mxspeed_lmtp5g9(uint32_t upper_flag, uint32_t user_value){
    static char buf[128];   
    status_code_t fault = STATUS_SUCCESS;

    uint32_t base =0xA5100000, offset =0x1CC, reg_val;
    uint16_t read_16;

    if (upper_flag > 1) {
        fault = STATUS_ERR_INVALID_FLAG;
        goto result;
    }

    // ---- Update Register ----
    if (reg_read(base, offset, &reg_val) != 0) {
    	fault = STATUS_ERR_HW_FAILED;
        goto result;
    }

    user_value &= 0xFFFF;
    if (upper_flag) {
        reg_val = (reg_val & 0x0000FFFF) | (user_value << 16);
    } else {
        reg_val = (reg_val & 0xFFFF0000) | user_value;
    }
    reg_write(base, offset, reg_val);
    
    // ---- Read Back Verification ----
    reg_read(base, offset, &reg_val);
    read_16 = upper_flag ? ((reg_val >> 16) & 0xFFFF) : (reg_val & 0xFFFF);

    if (read_16 != user_value) 
    	fault = STATUS_ERR_MISMATCH;
    
    result:
    snprintf(buf, sizeof(buf), 
    	"{ \"status\": %d }", 
    	fault);
             
    return buf;
}

char* fg09_mxtime_lmtfr_maxspd_GWvmaxCoP7(uint32_t upper_flag, uint32_t user_value){
    static char buf[128];   
    status_code_t fault = STATUS_SUCCESS;

    uint32_t base =0xA5100000, offset =0x1D0, reg_val;
    uint16_t read_16;

    if (upper_flag > 1) {
        fault = STATUS_ERR_INVALID_FLAG;
        goto result;
    }

    // ---- Update Register ----
    if (reg_read(base, offset, &reg_val) != 0) {
    	fault = STATUS_ERR_HW_FAILED;
        goto result;
    }

    user_value &= 0xFFFF;
    if (upper_flag) {
        reg_val = (reg_val & 0x0000FFFF) | (user_value << 16);
    } else {
        reg_val = (reg_val & 0xFFFF0000) | user_value;
    }
    reg_write(base, offset, reg_val);
    
    // ---- Read Back Verification ----
    reg_read(base, offset, &reg_val);
    read_16 = upper_flag ? ((reg_val >> 16) & 0xFFFF) : (reg_val & 0xFFFF);

    if (read_16 != user_value) 
    	fault = STATUS_ERR_MISMATCH;
    
    result:
    snprintf(buf, sizeof(buf), 
    	"{ \"status\": %d }", 
    	fault);
             
    return buf;
}

char* fg09_tebe_motg9_mot1lmt(uint32_t upper_flag, uint32_t user_value){
    static char buf[128];   
    status_code_t fault = STATUS_SUCCESS;

    uint32_t base =0xA5100000, offset =0x1D4, reg_val;
    uint16_t read_16;

    if (upper_flag > 1) {
        fault = STATUS_ERR_INVALID_FLAG;
        goto result;
    }

    // ---- Update Register ----
    if (reg_read(base, offset, &reg_val) != 0) {
    	fault = STATUS_ERR_HW_FAILED;
        goto result;
    }

    user_value &= 0xFFFF;
    if (upper_flag) {
        reg_val = (reg_val & 0x0000FFFF) | (user_value << 16);
    } else {
        reg_val = (reg_val & 0xFFFF0000) | user_value;
    }
    reg_write(base, offset, reg_val);
    
    // ---- Read Back Verification ----
    reg_read(base, offset, &reg_val);
    read_16 = upper_flag ? ((reg_val >> 16) & 0xFFFF) : (reg_val & 0xFFFF);

    if (read_16 != user_value) 
    	fault = STATUS_ERR_MISMATCH;
    
    result:
    snprintf(buf, sizeof(buf), 
    	"{ \"status\": %d }", 
    	fault);
             
    return buf;
}

char* fg09_tebe_loco_p7lmtg9(uint32_t upper_flag, uint32_t user_value){
    static char buf[128];   
    status_code_t fault = STATUS_SUCCESS;

    uint32_t base =0xA5100000, offset =0x1DC, reg_val;
    uint16_t read_16;

    if (upper_flag > 1) {
        fault = STATUS_ERR_INVALID_FLAG;
        goto result;
    }

    // ---- Update Register ----
    if (reg_read(base, offset, &reg_val) != 0) {
    	fault = STATUS_ERR_HW_FAILED;
        goto result;
    }

    user_value &= 0xFFFF;
    if (upper_flag) {
        reg_val = (reg_val & 0x0000FFFF) | (user_value << 16);
    } else {
        reg_val = (reg_val & 0xFFFF0000) | user_value;
    }
    reg_write(base, offset, reg_val);
    
    // ---- Read Back Verification ----
    reg_read(base, offset, &reg_val);
    read_16 = upper_flag ? ((reg_val >> 16) & 0xFFFF) : (reg_val & 0xFFFF);

    if (read_16 != user_value) 
    	fault = STATUS_ERR_MISMATCH;
    
    result:
    snprintf(buf, sizeof(buf), 
    	"{ \"status\": %d }", 
    	fault);
             
    return buf;
}

char* fg09_spdventi_onhylmt(uint32_t upper_flag, uint32_t user_value){
    static char buf[128];   
    status_code_t fault = STATUS_SUCCESS;

    uint32_t base =0xA5100000, offset =0x200, reg_val;
    uint16_t read_16;

    if (upper_flag > 1) {
        fault = STATUS_ERR_INVALID_FLAG;
        goto result;
    }

    // ---- Update Register ----
    if (reg_read(base, offset, &reg_val) != 0) {
    	fault = STATUS_ERR_HW_FAILED;
        goto result;
    }

    user_value &= 0xFFFF;
    if (upper_flag) {
        reg_val = (reg_val & 0x0000FFFF) | (user_value << 16);
    } else {
        reg_val = (reg_val & 0xFFFF0000) | user_value;
    }
    reg_write(base, offset, reg_val);
    
    // ---- Read Back Verification ----
    reg_read(base, offset, &reg_val);
    read_16 = upper_flag ? ((reg_val >> 16) & 0xFFFF) : (reg_val & 0xFFFF);

    if (read_16 != user_value) 
    	fault = STATUS_ERR_MISMATCH;
    
    result:
    snprintf(buf, sizeof(buf), 
    	"{ \"status\": %d }", 
    	fault);
             
    return buf;
}

char* fg09_pwrrdn_hyventi_lmt(uint32_t upper_flag, uint32_t user_value){
    static char buf[128];   
    status_code_t fault = STATUS_SUCCESS;

    uint32_t base =0xA5100000, offset =0x204, reg_val;
    uint16_t read_16;

    if (upper_flag > 1) {
        fault = STATUS_ERR_INVALID_FLAG;
        goto result;
    }

    // ---- Update Register ----
    if (reg_read(base, offset, &reg_val) != 0) {
    	fault = STATUS_ERR_HW_FAILED;
        goto result;
    }

    user_value &= 0xFFFF;
    if (upper_flag) {
        reg_val = (reg_val & 0x0000FFFF) | (user_value << 16);
    } else {
        reg_val = (reg_val & 0xFFFF0000) | user_value;
    }
    reg_write(base, offset, reg_val);
    
    // ---- Read Back Verification ----
    reg_read(base, offset, &reg_val);
    read_16 = upper_flag ? ((reg_val >> 16) & 0xFFFF) : (reg_val & 0xFFFF);

    if (read_16 != user_value) 
    	fault = STATUS_ERR_MISMATCH;
    
    result:
    snprintf(buf, sizeof(buf), 
    	"{ \"status\": %d }", 
    	fault);
             
    return buf;
}

char* fg08_09_reg(uint32_t bit_pos, uint32_t mode) {
    
	static char buf[256];
    	const uint32_t base = 0xA5100000, offset = 0x268, mask = (1U<<bit_pos);
    	uint32_t reg_val = 0, read_back;
    	status_code_t fault = STATUS_SUCCESS;

	if (mode > 1) {
        	fault = STATUS_ERR_INVALID_FLAG;
        	goto result;
    	}

	if (bit_pos > 23 || !((1 << bit_pos) & 0xE00007)) {
    		fault = STATUS_ERR_INVALID_BIT;
    		goto result;
	}
    	
        /* update */
        if ( reg_read(base, offset, &reg_val) != 0 ) {
       		fault = STATUS_ERR_HW_FAILED;
        	goto result;
    	}
    	
        reg_val = mode ? (reg_val| mask) : (reg_val& ~mask);
        reg_write(base, offset, reg_val);

        /* verify */
        reg_read(base, offset, &reg_val);
        read_back = (reg_val&mask) ? 1U : 0U;
        if(read_back != mode)
        	fault = STATUS_ERR_MISMATCH;
        
    	result:
    	snprintf(buf, sizeof(buf), 
    		"{ \"status\": %d }", 
    		fault);
             
    	return buf;
}

char* fg09_mot1mot2_crntlmt(uint32_t upper_flag, uint32_t user_value){
    static char buf[128];   
    status_code_t fault = STATUS_SUCCESS;

    uint32_t base =0xA5100000, offset =0x708, reg_val;
    uint16_t read_16;

    if (upper_flag > 1) {
        fault = STATUS_ERR_INVALID_FLAG;
        goto result;
    }

    // ---- Update Register ----
    if (reg_read(base, offset, &reg_val) != 0) {
    	fault = STATUS_ERR_HW_FAILED;
        goto result;
    }

    user_value &= 0xFFFF;
    if (upper_flag) {
        reg_val = (reg_val & 0x0000FFFF) | (user_value << 16);
    } else {
        reg_val = (reg_val & 0xFFFF0000) | user_value;
    }
    reg_write(base, offset, reg_val);
    
    // ---- Read Back Verification ----
    reg_read(base, offset, &reg_val);
    read_16 = upper_flag ? ((reg_val >> 16) & 0xFFFF) : (reg_val & 0xFFFF);

    if (read_16 != user_value) 
    	fault = STATUS_ERR_MISMATCH;
    
    result:
    snprintf(buf, sizeof(buf), 
    	"{ \"status\": %d }", 
    	fault);
             
    return buf;
}

char* fg09_mot3_crntlmt_igbttmp(uint32_t upper_flag, uint32_t user_value){
    static char buf[128];   
    status_code_t fault = STATUS_SUCCESS;

    uint32_t base =0xA5100000, offset =0x70C, reg_val;
    uint16_t read_16;

    if (upper_flag > 1) {
        fault = STATUS_ERR_INVALID_FLAG;
        goto result;
    }

    // ---- Update Register ----
    if (reg_read(base, offset, &reg_val) != 0) {
    	fault = STATUS_ERR_HW_FAILED;
        goto result;
    }

    user_value &= 0xFFFF;
    if (upper_flag) {
        reg_val = (reg_val & 0x0000FFFF) | (user_value << 16);
    } else {
        reg_val = (reg_val & 0xFFFF0000) | user_value;
    }
    reg_write(base, offset, reg_val);
    
    // ---- Read Back Verification ----
    reg_read(base, offset, &reg_val);
    read_16 = upper_flag ? ((reg_val >> 16) & 0xFFFF) : (reg_val & 0xFFFF);

    if (read_16 != user_value) 
    	fault = STATUS_ERR_MISMATCH;
    
    result:
    snprintf(buf, sizeof(buf), 
    	"{ \"status\": %d }", 
    	fault);
             
    return buf;
}

char* igbt_temp_lmt1(uint32_t upper_flag, uint32_t user_value){
    static char buf[128];   
    status_code_t fault = STATUS_SUCCESS;

    uint32_t base =0xA5100000, offset =0x710, reg_val;
    uint16_t read_16;

    if (upper_flag > 1) {
        fault = STATUS_ERR_INVALID_FLAG;
        goto result;
    }

    // ---- Update Register ----
    if (reg_read(base, offset, &reg_val) != 0) {
    	fault = STATUS_ERR_HW_FAILED;
        goto result;
    }

    user_value &= 0xFFFF;
    if (upper_flag) {
        reg_val = (reg_val & 0x0000FFFF) | (user_value << 16);
    } else {
        reg_val = (reg_val & 0xFFFF0000) | user_value;
    }
    reg_write(base, offset, reg_val);
    
    // ---- Read Back Verification ----
    reg_read(base, offset, &reg_val);
    read_16 = upper_flag ? ((reg_val >> 16) & 0xFFFF) : (reg_val & 0xFFFF);

    if (read_16 != user_value) 
    	fault = STATUS_ERR_MISMATCH;
    
    result:
    snprintf(buf, sizeof(buf), 
    	"{ \"status\": %d }", 
    	fault);
             
    return buf;
}

char* igbt_temp_lmt2(uint32_t upper_flag, uint32_t user_value){
    static char buf[128];   
    status_code_t fault = STATUS_SUCCESS;

    uint32_t base =0xA5100000, offset =0x714, reg_val;
    uint16_t read_16;

    if (upper_flag > 1) {
        fault = STATUS_ERR_INVALID_FLAG;
        goto result;
    }

    // ---- Update Register ----
    if (reg_read(base, offset, &reg_val) != 0) {
    	fault = STATUS_ERR_HW_FAILED;
        goto result;
    }

    user_value &= 0xFFFF;
    if (upper_flag) {
        reg_val = (reg_val & 0x0000FFFF) | (user_value << 16);
    } else {
        reg_val = (reg_val & 0xFFFF0000) | user_value;
    }
    reg_write(base, offset, reg_val);
    
    // ---- Read Back Verification ----
    reg_read(base, offset, &reg_val);
    read_16 = upper_flag ? ((reg_val >> 16) & 0xFFFF) : (reg_val & 0xFFFF);

    if (read_16 != user_value) 
    	fault = STATUS_ERR_MISMATCH;
    
    result:
    snprintf(buf, sizeof(buf), 
    	"{ \"status\": %d }", 
    	fault);
             
    return buf;
}

char* igbt_temp_lmt3(uint32_t upper_flag, uint32_t user_value){
    static char buf[128];   
    status_code_t fault = STATUS_SUCCESS;

    uint32_t base =0xA5100000, offset =0x718, reg_val;
    uint16_t read_16;

    if (upper_flag > 1) {
        fault = STATUS_ERR_INVALID_FLAG;
        goto result;
    }

    // ---- Update Register ----
    if (reg_read(base, offset, &reg_val) != 0) {
    	fault = STATUS_ERR_HW_FAILED;
        goto result;
    }

    user_value &= 0xFFFF;
    if (upper_flag) {
        reg_val = (reg_val & 0x0000FFFF) | (user_value << 16);
    } else {
        reg_val = (reg_val & 0xFFFF0000) | user_value;
    }
    reg_write(base, offset, reg_val);
    
    // ---- Read Back Verification ----
    reg_read(base, offset, &reg_val);
    read_16 = upper_flag ? ((reg_val >> 16) & 0xFFFF) : (reg_val & 0xFFFF);

    if (read_16 != user_value) 
    	fault = STATUS_ERR_MISMATCH;
    
    result:
    snprintf(buf, sizeof(buf), 
    	"{ \"status\": %d }", 
    	fault);
             
    return buf;
}

char* igbt_temp_lmt4(uint32_t upper_flag, uint32_t user_value){
    static char buf[128];   
    status_code_t fault = STATUS_SUCCESS;

    uint32_t base =0xA5100000, offset =0x71C, reg_val;
    uint16_t read_16;

    if (upper_flag > 1) {
        fault = STATUS_ERR_INVALID_FLAG;
        goto result;
    }

    // ---- Update Register ----
    if (reg_read(base, offset, &reg_val) != 0) {
    	fault = STATUS_ERR_HW_FAILED;
        goto result;
    }

    user_value &= 0xFFFF;
    if (upper_flag) {
        reg_val = (reg_val & 0x0000FFFF) | (user_value << 16);
    } else {
        reg_val = (reg_val & 0xFFFF0000) | user_value;
    }
    reg_write(base, offset, reg_val);
    
    // ---- Read Back Verification ----
    reg_read(base, offset, &reg_val);
    read_16 = upper_flag ? ((reg_val >> 16) & 0xFFFF) : (reg_val & 0xFFFF);

    if (read_16 != user_value) 
    	fault = STATUS_ERR_MISMATCH;
    
    result:
    snprintf(buf, sizeof(buf), 
    	"{ \"status\": %d }", 
    	fault);
             
    return buf;
}

/* FG91 */
char* fg91_pwradjust_fctr(uint32_t upper_flag, uint32_t user_value){
    static char buf[128];   
    status_code_t fault = STATUS_SUCCESS;

    uint32_t base =0xA5100000, offset =0x098, reg_val;
    uint16_t read_16;

    if (upper_flag > 1) {
        fault = STATUS_ERR_INVALID_FLAG;
        goto result;
    }

    // ---- Update Register ----
    if (reg_read(base, offset, &reg_val) != 0) {
    	fault = STATUS_ERR_HW_FAILED;
        goto result;
    }

    user_value &= 0xFFFF;
    if (upper_flag) {
        reg_val = (reg_val & 0x0000FFFF) | (user_value << 16);
    } else {
        reg_val = (reg_val & 0xFFFF0000) | user_value;
    }
    reg_write(base, offset, reg_val);
    
    // ---- Read Back Verification ----
    reg_read(base, offset, &reg_val);
    read_16 = upper_flag ? ((reg_val >> 16) & 0xFFFF) : (reg_val & 0xFFFF);

    if (read_16 != user_value) 
    	fault = STATUS_ERR_MISMATCH;
    
    result:
    snprintf(buf, sizeof(buf), 
    	"{ \"status\": %d }", 
    	fault);
             
    return buf;
}

char* fg91_DGDGC_max_brkeffort(uint32_t upper_flag, uint32_t user_value){
    static char buf[128];   
    status_code_t fault = STATUS_SUCCESS;

    uint32_t base =0xA5100000, offset =0x09C, reg_val;
    uint16_t read_16;

    if (upper_flag > 1) {
        fault = STATUS_ERR_INVALID_FLAG;
        goto result;
    }

    // ---- Update Register ----
    if (reg_read(base, offset, &reg_val) != 0) {
    	fault = STATUS_ERR_HW_FAILED;
        goto result;
    }

    user_value &= 0xFFFF;
    if (upper_flag) {
        reg_val = (reg_val & 0x0000FFFF) | (user_value << 16);
    } else {
        reg_val = (reg_val & 0xFFFF0000) | user_value;
    }
    reg_write(base, offset, reg_val);
    
    // ---- Read Back Verification ----
    reg_read(base, offset, &reg_val);
    read_16 = upper_flag ? ((reg_val >> 16) & 0xFFFF) : (reg_val & 0xFFFF);

    if (read_16 != user_value) 
    	fault = STATUS_ERR_MISMATCH;
    
    result:
    snprintf(buf, sizeof(buf), 
    	"{ \"status\": %d }", 
    	fault);
             
    return buf;
}

char* fg91_080591_reg(uint32_t bit_pos, uint32_t mode) {
    
	static char buf[256];
    	const uint32_t base = 0xA5100000, offset = 0x278, mask = (1U<<bit_pos);
    	uint32_t reg_val = 0, read_back;
    	status_code_t fault = STATUS_SUCCESS;

	if (mode > 1) {
        	fault = STATUS_ERR_INVALID_FLAG;
        	goto result;
    	}

	if (bit_pos > 21 || !((1 << bit_pos) & 0xE000000)) {
    		fault = STATUS_ERR_INVALID_BIT;
    		goto result;
	}
    	
        /* update */
        if ( reg_read(base, offset, &reg_val) != 0 ) {
       		fault = STATUS_ERR_HW_FAILED;
        	goto result;
    	}
    	
        reg_val = mode ? (reg_val| mask) : (reg_val& ~mask);
        reg_write(base, offset, reg_val);

        /* verify */
        reg_read(base, offset, &reg_val);
        read_back = (reg_val&mask) ? 1U : 0U;
        if(read_back != mode)
        	fault = STATUS_ERR_MISMATCH;
        
    	result:
    	snprintf(buf, sizeof(buf), 
    		"{ \"status\": %d }", 
    		fault);
             
    	return buf;
}

char* fg91_18870991_reg(uint32_t bit_pos, uint32_t mode) {
    
	static char buf[256];
    	const uint32_t base = 0xA5100000, offset = 0x100, mask = (1U<<bit_pos);
    	uint32_t reg_val = 0, read_back;
    	status_code_t fault = STATUS_SUCCESS;

	if (mode > 1) {
        	fault = STATUS_ERR_INVALID_FLAG;
        	goto result;
    	}

	if (bit_pos > 21 || !((1 << bit_pos) & 0x1000)) {
    		fault = STATUS_ERR_INVALID_BIT;
    		goto result;
	}
    	
        /* update */
        if ( reg_read(base, offset, &reg_val) != 0 ) {
       		fault = STATUS_ERR_HW_FAILED;
        	goto result;
    	}
    	
        reg_val = mode ? (reg_val| mask) : (reg_val& ~mask);
        reg_write(base, offset, reg_val);

        /* verify */
        reg_read(base, offset, &reg_val);
        read_back = (reg_val&mask) ? 1U : 0U;
        if(read_back != mode)
        	fault = STATUS_ERR_MISMATCH;
        
    	result:
    	snprintf(buf, sizeof(buf), 
    		"{ \"status\": %d }", 
    		fault);
             
    	return buf;
}

char* reg_dump(uint32_t fg_no, uint32_t type, uint32_t chunk) {
    
    static char buf[2048];
    int len = 0;
    uint32_t val = 0;
    const Register *reg_ptr = NULL;
    int i;
    
    for(i=0; i<FG_MAP_SIZE; i++) {
    	if(fg_no == fg_map[i].fg_id) {
    	   break;
	}	   
    }
    
    if (i >= FG_MAP_SIZE) { snprintf(buf, sizeof(buf), "{ \"status\": 7, \"msg\": \"Incorrect fg_no\" }"); return buf; }
    
    uint16_t reg_size =0;
    uint16_t input_count = fg_map[i].reg_count[0];     //reg count for INPUT
    uint16_t output_count = fg_map[i].reg_count[1];    //reg count for OUTPUT
    uint16_t internal_count = fg_map[i].reg_count[2];  //reg count for INTERNAL
    uint16_t param_count = fg_map[i].reg_count[3];     //reg count for PARAMETER
    
    if (type == 0) { reg_ptr = fg_map[i].registers; reg_size = input_count; }  //pointer to input regsiters
    else if (type == 1) { reg_ptr = fg_map[i].registers + input_count; reg_size = output_count; }  //pointer to output register
    else if (type == 2) { reg_ptr = fg_map[i].registers + (input_count+output_count); reg_size = internal_count; }  //pointer to internal register
    else if (type == 3) { reg_ptr = fg_map[i].registers + (input_count+output_count+internal_count); reg_size = param_count; }  // pointer to parameter register
    else { snprintf(buf, sizeof(buf), "{ \"status\": 7, \"msg\": \"Incorrect type\" }"); return buf; }   
    
    uint32_t max_chunks = (reg_size/CHUNK_SIZE) +1;
    if( !(chunk < max_chunks)) { snprintf(buf, sizeof(buf), "{ \"status\": 7, \"msg\": \"Incorrect chunk\" }"); return buf; }
    
    int start = chunk * CHUNK_SIZE;
    int end   = start + CHUNK_SIZE;
    if(end > reg_size) { end = reg_size; }
    
    //printf("start =%d end =%d reg_size=%d\n", start, end, reg_size);
    if (start >= reg_size) {
        snprintf(buf, sizeof(buf), "{ \"status\": 7, \"msg\": \"No data\" }");
        return buf;
    }
     
    len += snprintf(buf + len, sizeof(buf) - len, "{\"status\":0,\"fg_no\":%u,\"type\":%u,\"chunk\":%u,\"reg\":[", fg_no, type, chunk);
    for (int j = start;  j< end; j++) {
	
        int ret = reg_read(reg_ptr[j].base, reg_ptr[j].offset, &val);
        if ( ret !=0) {
            snprintf(buf, sizeof(buf),
        	"{ \"status\": %d, \"failed_reg\": \"0x%08X\" }",
        	ret, reg_ptr[j].base+reg_ptr[j].offset);
                     
            return buf;
        }
        
        len += snprintf(buf + len, sizeof(buf) - len,
                           "[0x%08X,0x%08X]%s",
                           reg_ptr[j].base+reg_ptr[j].offset,
                           val,
                           (j < end-1) ? "," : "");  
                                                                  
    }

    snprintf(buf + len, sizeof(buf) - len, "]}");
    return buf;
}


/* DDU signals FLG_MVB_OUT */
char* MVBOut_DDU3100_1024msReg0_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA2083000, 0x100, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* MVBOut_DDU3104_1024msReg1_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA2083000, 0x104, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* MVBOut_DDU3100_1024msReg2_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA2083000, 0x108, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* MVBOut_DDU3100_1024msReg3_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA2083000, 0x10C, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* MVBOut_DDU3100_1024msReg4_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA2083000, 0x110, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* MVBOut_DDU3100_1024msReg5_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA2083000, 0x114, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* MVBOut_DDU3100_1024msReg6_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA2083000, 0x118, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* MVBOut_DDU3100_1024msReg7_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA2083000, 0x11C, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* MVBOut_DDU3110_1024msReg0_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA2083000, 0x120, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* MVBOut_DDU3110_1024msReg1_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA2083000, 0x124, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* MVBOut_DDU3110_1024msReg2_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA2083000, 0x128, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* MVBOut_DDU3110_1024msReg3_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA2083000, 0x12C, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* MVBOut_DDU3110_1024msReg4_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA2083000, 0x130, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* MVBOut_DDU3110_1024msReg5_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA2083000, 0x134, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* MVBOut_DDU3110_1024msReg6_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA2083000, 0x138, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* MVBOut_DDU3300_1024msReg0_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA2083000, 0x300, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* MVBOut_DDU3300_1024msReg1_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA2083000, 0x304, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* MVBOut_DDU3400_1024msReg0_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA2083000, 0x400, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* MVBOut_DDU3400_1024msReg1_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA2083000, 0x404, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* MVBOut_DDU3500_1024msReg0_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA2083000, 0x500, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* MVBOut_DDU3500_1024msReg1_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA2083000, 0x504, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* MVBOut_DDU3600_1024msReg0_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA2083000, 0x600, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* MVBOut_DDU3600_1024msReg1_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA2083000, 0x604, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* MVBOut_DDUFaultCodeReg_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA2083000, 0x700, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

/* DDU signals BUR_MVB_OUT */
char* MVBout_BUR1DDU1_1024msReg0_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA2202000, 0x000, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* MVBout_BUR1DDU1_1024msReg1_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA2202000, 0x004, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* MVBout_BUR1DDU1_1024msReg2_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA2202000, 0x008, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* MVBout_BUR1DDU1_1024msReg3_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA2202000, 0x00C, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* MVBout_BUR1DDU1_1024msReg4_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA2202000, 0x010, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* MVBout_BUR2DDU1_1024msReg0_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA2202000, 0x100, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* MVBout_BUR2DDU1_1024msReg1_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA2202000, 0x104, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* MVBout_BUR2DDU1_1024msReg2_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA2202000, 0x108, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* MVBout_BUR2DDU1_1024msReg3_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA2202000, 0x10C, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* MVBout_BUR2DDU1_1024msReg4_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA2202000, 0x110, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* MVBout_BUR2DDU1_1024msReg5_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA2202000, 0x114, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* MVBout_BUR2DDU1_1024msReg6_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA2202000, 0x118, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* MVBout_BUR2DDU1_1024msReg7_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA2202000, 0x11C, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* MVBout_BUR3DDU1_1024msReg0_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA2202000, 0x200, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* MVBout_BUR3DDU1_1024msReg1_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA2202000, 0x204, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* MVBout_BUR3DDU1_1024msReg2_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA2202000, 0x208, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* MVBout_BUR3DDU1_1024msReg3_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA2202000, 0x20C, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* MVBout_BUR3DDU1_1024msReg4_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA2202000, 0x210, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* MVBout_BUR3DDU1_1024msReg5_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA2202000, 0x214, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* MVBout_BUR3DDU1_1024msReg6_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA2202000, 0x218, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* MVBout_BUR3DDU1_1024msReg7_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA2202000, 0x21C, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

/* DDU signals SLG_MVB_OUT */
char* MVBOut_TCU1Proc_T1000ms_DDU1Reg87_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA2100000, 0x15C, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* MVBOut_TCU1Proc_T1000ms_DDU1Reg94_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA2100000, 0x178, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* MVBOut_TCU1Proc_T1000ms_DDU1Reg95_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA2100000, 0x17C, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* MVBOut_TCU1Proc_T1000ms_DDU1Reg96_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA2100000, 0x180, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* MVBOut_TCU1Proc_T1000ms_DDU1Reg97_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA2100000, 0x184, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* MVBOut_TCU1Proc_T1000ms_DDU1Reg98_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA2100000, 0x188, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* MVBOut_TCU1Proc_T200ms_DDU1Reg101_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA2100000, 0x194, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* MVBOut_TCU1Proc_T100ms_DDU1Reg102_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA2100000, 0x198, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* MVBOut_TCU1Proc_T100ms_DDU1Reg103_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA2100000, 0x19C, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* MVBOut_TCU1Proc_T1000ms_DDU1Reg108_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA2100000, 0x1B0, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* MVBOut_TCU1Proc_T1000ms_DDU1Reg109_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA2100000, 0x1B4, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* MVBOut_TCU1Proc_T1000ms_DDU1Reg110_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA2100000, 0x1B8, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* MVBOut_TCU1Proc_T1000ms_DDU1Reg111_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA2100000, 0x1BC, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* MVBOut_TCU1Proc_T1000ms_DDU1Reg112_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA2100000, 0x1C0, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* MVBOut_TCU1Proc_T1000ms_DDU1Reg113_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA2100000, 0x1C4, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* MVBOut_TCU1Proc_T1000ms_DDU1Reg114_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA2100000, 0x1C8, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* MVBOut_TCU1Proc_T1000ms_DDU1Reg115_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA2100000, 0x1CC, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* MVBOut_TCU1Proc_T1000ms_DDU1Reg117_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA2100000, 0x1D4, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* MVBOut_TCU1Proc_T1000ms_DDU1Reg118_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA2100000, 0x1D8, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* MVBOut_TCU1Proc_T1000ms_DDU1Reg119_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA2100000, 0x1DC, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

/* line motor writable parameter */
char* CANout_tcu1lc1_wrpara_reg0(uint32_t upper_flag, uint32_t user_value){

    static char buf[128];   
    status_code_t fault = STATUS_SUCCESS;

    uint32_t base =0xA4107000, offset =0x100, reg_val;
    uint16_t read_16;

    if (upper_flag > 1) {
        fault = STATUS_ERR_INVALID_FLAG;
        goto result;
    }

    // ---- Update Register ----
    if (reg_read(base, offset, &reg_val) != 0) {
    	fault = STATUS_ERR_HW_FAILED;
        goto result;
    }

    user_value &= 0xFFFF;
    if (upper_flag) {
        reg_val = (reg_val & 0x0000FFFF) | (user_value << 16);
    } else {
        reg_val = (reg_val & 0xFFFF0000) | user_value;
    }
    reg_write(base, offset, reg_val);
    
    // ---- Read Back Verification ----
    reg_read(base, offset, &reg_val);
    read_16 = upper_flag ? ((reg_val >> 16) & 0xFFFF) : (reg_val & 0xFFFF);

    if (read_16 != user_value) 
    	fault = STATUS_ERR_MISMATCH;
    
    result:
    snprintf(buf, sizeof(buf), 
    	"{ \"status\": %d }", 
    	fault);
             
    return buf;
}

char* CANout_tcu1lc1_wrpara_reg1(uint32_t upper_flag, uint32_t user_value){

    static char buf[128];   
    status_code_t fault = STATUS_SUCCESS;

    uint32_t base =0xA4107000, offset =0x104, reg_val;
    uint16_t read_16;

    if (upper_flag > 1) {
        fault = STATUS_ERR_INVALID_FLAG;
        goto result;
    }

    // ---- Update Register ----
    if (reg_read(base, offset, &reg_val) != 0) {
    	fault = STATUS_ERR_HW_FAILED;
        goto result;
    }

    user_value &= 0xFFFF;
    if (upper_flag) {
        reg_val = (reg_val & 0x0000FFFF) | (user_value << 16);
    } else {
        reg_val = (reg_val & 0xFFFF0000) | user_value;
    }
    reg_write(base, offset, reg_val);
    
    // ---- Read Back Verification ----
    reg_read(base, offset, &reg_val);
    read_16 = upper_flag ? ((reg_val >> 16) & 0xFFFF) : (reg_val & 0xFFFF);

    if (read_16 != user_value) 
    	fault = STATUS_ERR_MISMATCH;
    
    result:
    snprintf(buf, sizeof(buf), 
    	"{ \"status\": %d }", 
    	fault);
             
    return buf;
}

char* CANout_tcu1lc1_wrpara_reg2(uint32_t upper_flag, uint32_t user_value){

    static char buf[128];   
    status_code_t fault = STATUS_SUCCESS;

    uint32_t base =0xA4107000, offset =0x108, reg_val;
    uint16_t read_16;

    if (upper_flag > 1) {
        fault = STATUS_ERR_INVALID_FLAG;
        goto result;
    }

    // ---- Update Register ----
    if (reg_read(base, offset, &reg_val) != 0) {
    	fault = STATUS_ERR_HW_FAILED;
        goto result;
    }

    user_value &= 0xFFFF;
    if (upper_flag) {
        reg_val = (reg_val & 0x0000FFFF) | (user_value << 16);
    } else {
        reg_val = (reg_val & 0xFFFF0000) | user_value;
    }
    reg_write(base, offset, reg_val);
    
    // ---- Read Back Verification ----
    reg_read(base, offset, &reg_val);
    read_16 = upper_flag ? ((reg_val >> 16) & 0xFFFF) : (reg_val & 0xFFFF);

    if (read_16 != user_value) 
    	fault = STATUS_ERR_MISMATCH;
    
    result:
    snprintf(buf, sizeof(buf), 
    	"{ \"status\": %d }", 
    	fault);
             
    return buf;
}

char* CANout_tcu1lc1_wrpara_reg3(uint32_t upper_flag, uint32_t user_value){

    static char buf[128];   
    status_code_t fault = STATUS_SUCCESS;

    uint32_t base =0xA4107000, offset =0x10C, reg_val;
    uint16_t read_16;

    if (upper_flag > 1) {
        fault = STATUS_ERR_INVALID_FLAG;
        goto result;
    }

    // ---- Update Register ----
    if (reg_read(base, offset, &reg_val) != 0) {
    	fault = STATUS_ERR_HW_FAILED;
        goto result;
    }

    user_value &= 0xFFFF;
    if (upper_flag) {
        reg_val = (reg_val & 0x0000FFFF) | (user_value << 16);
    } else {
        reg_val = (reg_val & 0xFFFF0000) | user_value;
    }
    reg_write(base, offset, reg_val);
    
    // ---- Read Back Verification ----
    reg_read(base, offset, &reg_val);
    read_16 = upper_flag ? ((reg_val >> 16) & 0xFFFF) : (reg_val & 0xFFFF);

    if (read_16 != user_value) 
    	fault = STATUS_ERR_MISMATCH;
    
    result:
    snprintf(buf, sizeof(buf), 
    	"{ \"status\": %d }", 
    	fault);
             
    return buf;
}

char* CANout_tcu1lc1_wrpara_reg4(uint32_t upper_flag, uint32_t user_value){

    static char buf[128];   
    status_code_t fault = STATUS_SUCCESS;

    uint32_t base =0xA4107000, offset =0x110, reg_val;
    uint16_t read_16;

    if (upper_flag > 1) {
        fault = STATUS_ERR_INVALID_FLAG;
        goto result;
    }

    // ---- Update Register ----
    if (reg_read(base, offset, &reg_val) != 0) {
    	fault = STATUS_ERR_HW_FAILED;
        goto result;
    }

    user_value &= 0xFFFF;
    if (upper_flag) {
        reg_val = (reg_val & 0x0000FFFF) | (user_value << 16);
    } else {
        reg_val = (reg_val & 0xFFFF0000) | user_value;
    }
    reg_write(base, offset, reg_val);
    
    // ---- Read Back Verification ----
    reg_read(base, offset, &reg_val);
    read_16 = upper_flag ? ((reg_val >> 16) & 0xFFFF) : (reg_val & 0xFFFF);

    if (read_16 != user_value) 
    	fault = STATUS_ERR_MISMATCH;
    
    result:
    snprintf(buf, sizeof(buf), 
    	"{ \"status\": %d }", 
    	fault);
             
    return buf;
}

char* CANout_tcu1lc1_wrpara_reg5(uint32_t upper_flag, uint32_t user_value){

    static char buf[128];   
    status_code_t fault = STATUS_SUCCESS;

    uint32_t base =0xA4107000, offset =0x114, reg_val;
    uint16_t read_16;

    if (upper_flag > 1) {
        fault = STATUS_ERR_INVALID_FLAG;
        goto result;
    }

    // ---- Update Register ----
    if (reg_read(base, offset, &reg_val) != 0) {
    	fault = STATUS_ERR_HW_FAILED;
        goto result;
    }

    user_value &= 0xFFFF;
    if (upper_flag) {
        reg_val = (reg_val & 0x0000FFFF) | (user_value << 16);
    } else {
        reg_val = (reg_val & 0xFFFF0000) | user_value;
    }
    reg_write(base, offset, reg_val);
    
    // ---- Read Back Verification ----
    reg_read(base, offset, &reg_val);
    read_16 = upper_flag ? ((reg_val >> 16) & 0xFFFF) : (reg_val & 0xFFFF);

    if (read_16 != user_value) 
    	fault = STATUS_ERR_MISMATCH;
    
    result:
    snprintf(buf, sizeof(buf), 
    	"{ \"status\": %d }", 
    	fault);
             
    return buf;
}

char* CANout_tcu1lc1_wrpara_reg6(uint32_t upper_flag, uint32_t user_value){

    static char buf[128];   
    status_code_t fault = STATUS_SUCCESS;

    uint32_t base =0xA4107000, offset =0x118, reg_val;
    uint16_t read_16;

    if (upper_flag > 1) {
        fault = STATUS_ERR_INVALID_FLAG;
        goto result;
    }

    // ---- Update Register ----
    if (reg_read(base, offset, &reg_val) != 0) {
    	fault = STATUS_ERR_HW_FAILED;
        goto result;
    }

    user_value &= 0xFFFF;
    if (upper_flag) {
        reg_val = (reg_val & 0x0000FFFF) | (user_value << 16);
    } else {
        reg_val = (reg_val & 0xFFFF0000) | user_value;
    }
    reg_write(base, offset, reg_val);
    
    // ---- Read Back Verification ----
    reg_read(base, offset, &reg_val);
    read_16 = upper_flag ? ((reg_val >> 16) & 0xFFFF) : (reg_val & 0xFFFF);

    if (read_16 != user_value) 
    	fault = STATUS_ERR_MISMATCH;
    
    result:
    snprintf(buf, sizeof(buf), 
    	"{ \"status\": %d }", 
    	fault);
             
    return buf;
}

char* CANout_tcu1lc1_wrpara_reg7(uint32_t upper_flag, uint32_t user_value){

    static char buf[128];   
    status_code_t fault = STATUS_SUCCESS;

    uint32_t base =0xA4107000, offset =0x11C, reg_val;
    uint16_t read_16;

    if (upper_flag > 1) {
        fault = STATUS_ERR_INVALID_FLAG;
        goto result;
    }

    // ---- Update Register ----
    if (reg_read(base, offset, &reg_val) != 0) {
    	fault = STATUS_ERR_HW_FAILED;
        goto result;
    }

    user_value &= 0xFFFF;
    if (upper_flag) {
        reg_val = (reg_val & 0x0000FFFF) | (user_value << 16);
    } else {
        reg_val = (reg_val & 0xFFFF0000) | user_value;
    }
    reg_write(base, offset, reg_val);
    
    // ---- Read Back Verification ----
    reg_read(base, offset, &reg_val);
    read_16 = upper_flag ? ((reg_val >> 16) & 0xFFFF) : (reg_val & 0xFFFF);

    if (read_16 != user_value) 
    	fault = STATUS_ERR_MISMATCH;
    
    result:
    snprintf(buf, sizeof(buf), 
    	"{ \"status\": %d }", 
    	fault);
             
    return buf;
}

char* CANout_tcu1lc1_wrpara_reg8(uint32_t upper_flag, uint32_t user_value){

    static char buf[128];   
    status_code_t fault = STATUS_SUCCESS;

    uint32_t base =0xA4107000, offset =0x120, reg_val;
    uint16_t read_16;

    if (upper_flag > 1) {
        fault = STATUS_ERR_INVALID_FLAG;
        goto result;
    }

    // ---- Update Register ----
    if (reg_read(base, offset, &reg_val) != 0) {
    	fault = STATUS_ERR_HW_FAILED;
        goto result;
    }

    user_value &= 0xFFFF;
    if (upper_flag) {
        reg_val = (reg_val & 0x0000FFFF) | (user_value << 16);
    } else {
        reg_val = (reg_val & 0xFFFF0000) | user_value;
    }
    reg_write(base, offset, reg_val);
    
    // ---- Read Back Verification ----
    reg_read(base, offset, &reg_val);
    read_16 = upper_flag ? ((reg_val >> 16) & 0xFFFF) : (reg_val & 0xFFFF);

    if (read_16 != user_value) 
    	fault = STATUS_ERR_MISMATCH;
    
    result:
    snprintf(buf, sizeof(buf), 
    	"{ \"status\": %d }", 
    	fault);
             
    return buf;
}

char* CANout_tcu1lc1_wrpara_reg9(uint32_t upper_flag, uint32_t user_value){

    static char buf[128];   
    status_code_t fault = STATUS_SUCCESS;

    uint32_t base =0xA4107000, offset =0x124, reg_val;
    uint16_t read_16;

    if (upper_flag > 1) {
        fault = STATUS_ERR_INVALID_FLAG;
        goto result;
    }

    // ---- Update Register ----
    if (reg_read(base, offset, &reg_val) != 0) {
    	fault = STATUS_ERR_HW_FAILED;
        goto result;
    }

    user_value &= 0xFFFF;
    if (upper_flag) {
        reg_val = (reg_val & 0x0000FFFF) | (user_value << 16);
    } else {
        reg_val = (reg_val & 0xFFFF0000) | user_value;
    }
    reg_write(base, offset, reg_val);
    
    // ---- Read Back Verification ----
    reg_read(base, offset, &reg_val);
    read_16 = upper_flag ? ((reg_val >> 16) & 0xFFFF) : (reg_val & 0xFFFF);

    if (read_16 != user_value) 
    	fault = STATUS_ERR_MISMATCH;
    
    result:
    snprintf(buf, sizeof(buf), 
    	"{ \"status\": %d }", 
    	fault);
             
    return buf;
}

char* CANout_tcu1lc1_wrpara_reg10(uint32_t upper_flag, uint32_t user_value){

    static char buf[128];   
    status_code_t fault = STATUS_SUCCESS;

    uint32_t base =0xA4107000, offset =0x128, reg_val;
    uint16_t read_16;

    if (upper_flag > 1) {
        fault = STATUS_ERR_INVALID_FLAG;
        goto result;
    }

    // ---- Update Register ----
    if (reg_read(base, offset, &reg_val) != 0) {
    	fault = STATUS_ERR_HW_FAILED;
        goto result;
    }

    user_value &= 0xFFFF;
    if (upper_flag) {
        reg_val = (reg_val & 0x0000FFFF) | (user_value << 16);
    } else {
        reg_val = (reg_val & 0xFFFF0000) | user_value;
    }
    reg_write(base, offset, reg_val);
    
    // ---- Read Back Verification ----
    reg_read(base, offset, &reg_val);
    read_16 = upper_flag ? ((reg_val >> 16) & 0xFFFF) : (reg_val & 0xFFFF);

    if (read_16 != user_value) 
    	fault = STATUS_ERR_MISMATCH;
    
    result:
    snprintf(buf, sizeof(buf), 
    	"{ \"status\": %d }", 
    	fault);
             
    return buf;
}

char* CANout_tcu1lc1_wrpara_reg11(uint32_t upper_flag, uint32_t user_value){

    static char buf[128];   
    status_code_t fault = STATUS_SUCCESS;

    uint32_t base =0xA4107000, offset =0x12C, reg_val;
    uint16_t read_16;

    if (upper_flag > 1) {
        fault = STATUS_ERR_INVALID_FLAG;
        goto result;
    }

    // ---- Update Register ----
    if (reg_read(base, offset, &reg_val) != 0) {
    	fault = STATUS_ERR_HW_FAILED;
        goto result;
    }

    user_value &= 0xFFFF;
    if (upper_flag) {
        reg_val = (reg_val & 0x0000FFFF) | (user_value << 16);
    } else {
        reg_val = (reg_val & 0xFFFF0000) | user_value;
    }
    reg_write(base, offset, reg_val);
    
    // ---- Read Back Verification ----
    reg_read(base, offset, &reg_val);
    read_16 = upper_flag ? ((reg_val >> 16) & 0xFFFF) : (reg_val & 0xFFFF);

    if (read_16 != user_value) 
    	fault = STATUS_ERR_MISMATCH;
    
    result:
    snprintf(buf, sizeof(buf), 
    	"{ \"status\": %d }", 
    	fault);
             
    return buf;
}

char* CANout_tcu1lc1_wrpara_reg12(uint32_t upper_flag, uint32_t user_value){

    static char buf[128];   
    status_code_t fault = STATUS_SUCCESS;

    uint32_t base =0xA4107000, offset =0x130, reg_val;
    uint16_t read_16;

    if (upper_flag > 1) {
        fault = STATUS_ERR_INVALID_FLAG;
        goto result;
    }

    // ---- Update Register ----
    if (reg_read(base, offset, &reg_val) != 0) {
    	fault = STATUS_ERR_HW_FAILED;
        goto result;
    }

    user_value &= 0xFFFF;
    if (upper_flag) {
        reg_val = (reg_val & 0x0000FFFF) | (user_value << 16);
    } else {
        reg_val = (reg_val & 0xFFFF0000) | user_value;
    }
    reg_write(base, offset, reg_val);
    
    // ---- Read Back Verification ----
    reg_read(base, offset, &reg_val);
    read_16 = upper_flag ? ((reg_val >> 16) & 0xFFFF) : (reg_val & 0xFFFF);

    if (read_16 != user_value) 
    	fault = STATUS_ERR_MISMATCH;
    
    result:
    snprintf(buf, sizeof(buf), 
    	"{ \"status\": %d }", 
    	fault);
             
    return buf;
}

char* CANout_tcu1lc1_wrpara_reg13(uint32_t upper_flag, uint32_t user_value){

    static char buf[128];   
    status_code_t fault = STATUS_SUCCESS;

    uint32_t base =0xA4107000, offset =0x134, reg_val;
    uint16_t read_16;

    if (upper_flag > 1) {
        fault = STATUS_ERR_INVALID_FLAG;
        goto result;
    }

    // ---- Update Register ----
    if (reg_read(base, offset, &reg_val) != 0) {
    	fault = STATUS_ERR_HW_FAILED;
        goto result;
    }

    user_value &= 0xFFFF;
    if (upper_flag) {
        reg_val = (reg_val & 0x0000FFFF) | (user_value << 16);
    } else {
        reg_val = (reg_val & 0xFFFF0000) | user_value;
    }
    reg_write(base, offset, reg_val);
    
    // ---- Read Back Verification ----
    reg_read(base, offset, &reg_val);
    read_16 = upper_flag ? ((reg_val >> 16) & 0xFFFF) : (reg_val & 0xFFFF);

    if (read_16 != user_value) 
    	fault = STATUS_ERR_MISMATCH;
    
    result:
    snprintf(buf, sizeof(buf), 
    	"{ \"status\": %d }", 
    	fault);
             
    return buf;
}

char* CANout_tcu1lc1_wrpara_reg14(uint32_t upper_flag, uint32_t user_value){

    static char buf[128];   
    status_code_t fault = STATUS_SUCCESS;

    uint32_t base =0xA4107000, offset =0x138, reg_val;
    uint16_t read_16;

    if (upper_flag > 1) {
        fault = STATUS_ERR_INVALID_FLAG;
        goto result;
    }

    // ---- Update Register ----
    if (reg_read(base, offset, &reg_val) != 0) {
    	fault = STATUS_ERR_HW_FAILED;
        goto result;
    }

    user_value &= 0xFFFF;
    if (upper_flag) {
        reg_val = (reg_val & 0x0000FFFF) | (user_value << 16);
    } else {
        reg_val = (reg_val & 0xFFFF0000) | user_value;
    }
    reg_write(base, offset, reg_val);
    
    // ---- Read Back Verification ----
    reg_read(base, offset, &reg_val);
    read_16 = upper_flag ? ((reg_val >> 16) & 0xFFFF) : (reg_val & 0xFFFF);

    if (read_16 != user_value) 
    	fault = STATUS_ERR_MISMATCH;
    
    result:
    snprintf(buf, sizeof(buf), 
    	"{ \"status\": %d }", 
    	fault);
             
    return buf;
}

char* CANout_tcu1lc1_wrpara_reg15(uint32_t upper_flag, uint32_t user_value){

    static char buf[128];   
    status_code_t fault = STATUS_SUCCESS;

    uint32_t base =0xA4107000, offset =0x13C, reg_val;
    uint16_t read_16;

    if (upper_flag > 1) {
        fault = STATUS_ERR_INVALID_FLAG;
        goto result;
    }

    // ---- Update Register ----
    if (reg_read(base, offset, &reg_val) != 0) {
    	fault = STATUS_ERR_HW_FAILED;
        goto result;
    }

    user_value &= 0xFFFF;
    if (upper_flag) {
        reg_val = (reg_val & 0x0000FFFF) | (user_value << 16);
    } else {
        reg_val = (reg_val & 0xFFFF0000) | user_value;
    }
    reg_write(base, offset, reg_val);
    
    // ---- Read Back Verification ----
    reg_read(base, offset, &reg_val);
    read_16 = upper_flag ? ((reg_val >> 16) & 0xFFFF) : (reg_val & 0xFFFF);

    if (read_16 != user_value) 
    	fault = STATUS_ERR_MISMATCH;
    
    result:
    snprintf(buf, sizeof(buf), 
    	"{ \"status\": %d }", 
    	fault);
             
    return buf;
}

char* CANout_tcu1lc1_wrpara_reg16(uint32_t upper_flag, uint32_t user_value){

    static char buf[128];   
    status_code_t fault = STATUS_SUCCESS;

    uint32_t base =0xA4107000, offset =0x140, reg_val;
    uint16_t read_16;

    if (upper_flag > 1) {
        fault = STATUS_ERR_INVALID_FLAG;
        goto result;
    }

    // ---- Update Register ----
    if (reg_read(base, offset, &reg_val) != 0) {
    	fault = STATUS_ERR_HW_FAILED;
        goto result;
    }

    user_value &= 0xFFFF;
    if (upper_flag) {
        reg_val = (reg_val & 0x0000FFFF) | (user_value << 16);
    } else {
        reg_val = (reg_val & 0xFFFF0000) | user_value;
    }
    reg_write(base, offset, reg_val);
    
    // ---- Read Back Verification ----
    reg_read(base, offset, &reg_val);
    read_16 = upper_flag ? ((reg_val >> 16) & 0xFFFF) : (reg_val & 0xFFFF);

    if (read_16 != user_value) 
    	fault = STATUS_ERR_MISMATCH;
    
    result:
    snprintf(buf, sizeof(buf), 
    	"{ \"status\": %d }", 
    	fault);
             
    return buf;
}

char* CANout_tcu1lc1_wrpara_reg17(uint32_t upper_flag, uint32_t user_value){

    static char buf[128];   
    status_code_t fault = STATUS_SUCCESS;

    uint32_t base =0xA4107000, offset =0x144, reg_val;
    uint16_t read_16;

    if (upper_flag > 1) {
        fault = STATUS_ERR_INVALID_FLAG;
        goto result;
    }

    // ---- Update Register ----
    if (reg_read(base, offset, &reg_val) != 0) {
    	fault = STATUS_ERR_HW_FAILED;
        goto result;
    }

    user_value &= 0xFFFF;
    if (upper_flag) {
        reg_val = (reg_val & 0x0000FFFF) | (user_value << 16);
    } else {
        reg_val = (reg_val & 0xFFFF0000) | user_value;
    }
    reg_write(base, offset, reg_val);
    
    // ---- Read Back Verification ----
    reg_read(base, offset, &reg_val);
    read_16 = upper_flag ? ((reg_val >> 16) & 0xFFFF) : (reg_val & 0xFFFF);

    if (read_16 != user_value) 
    	fault = STATUS_ERR_MISMATCH;
    
    result:
    snprintf(buf, sizeof(buf), 
    	"{ \"status\": %d }", 
    	fault);
             
    return buf;
}

char* CANout_tcu1lc1_wrpara_reg18(uint32_t upper_flag, uint32_t user_value){

    static char buf[128];   
    status_code_t fault = STATUS_SUCCESS;

    uint32_t base =0xA4107000, offset =0x148, reg_val;
    uint16_t read_16;

    if (upper_flag > 1) {
        fault = STATUS_ERR_INVALID_FLAG;
        goto result;
    }

    // ---- Update Register ----
    if (reg_read(base, offset, &reg_val) != 0) {
    	fault = STATUS_ERR_HW_FAILED;
        goto result;
    }

    user_value &= 0xFFFF;
    if (upper_flag) {
        reg_val = (reg_val & 0x0000FFFF) | (user_value << 16);
    } else {
        reg_val = (reg_val & 0xFFFF0000) | user_value;
    }
    reg_write(base, offset, reg_val);
    
    // ---- Read Back Verification ----
    reg_read(base, offset, &reg_val);
    read_16 = upper_flag ? ((reg_val >> 16) & 0xFFFF) : (reg_val & 0xFFFF);

    if (read_16 != user_value) 
    	fault = STATUS_ERR_MISMATCH;
    
    result:
    snprintf(buf, sizeof(buf), 
    	"{ \"status\": %d }", 
    	fault);
             
    return buf;
}

char* CANout_tcu1lc1_wrpara_reg19(uint32_t upper_flag, uint32_t user_value){

    static char buf[128];   
    status_code_t fault = STATUS_SUCCESS;

    uint32_t base =0xA4107000, offset =0x14C, reg_val;
    uint16_t read_16;

    if (upper_flag > 1) {
        fault = STATUS_ERR_INVALID_FLAG;
        goto result;
    }

    // ---- Update Register ----
    if (reg_read(base, offset, &reg_val) != 0) {
    	fault = STATUS_ERR_HW_FAILED;
        goto result;
    }

    user_value &= 0xFFFF;
    if (upper_flag) {
        reg_val = (reg_val & 0x0000FFFF) | (user_value << 16);
    } else {
        reg_val = (reg_val & 0xFFFF0000) | user_value;
    }
    reg_write(base, offset, reg_val);
    
    // ---- Read Back Verification ----
    reg_read(base, offset, &reg_val);
    read_16 = upper_flag ? ((reg_val >> 16) & 0xFFFF) : (reg_val & 0xFFFF);

    if (read_16 != user_value) 
    	fault = STATUS_ERR_MISMATCH;
    
    result:
    snprintf(buf, sizeof(buf), 
    	"{ \"status\": %d }", 
    	fault);
             
    return buf;
}

char* CANout_tcu1lc1_wrpara_reg20(uint32_t upper_flag, uint32_t user_value){

    static char buf[128];   
    status_code_t fault = STATUS_SUCCESS;

    uint32_t base =0xA4107000, offset =0x150, reg_val;
    uint16_t read_16;

    if (upper_flag > 1) {
        fault = STATUS_ERR_INVALID_FLAG;
        goto result;
    }

    // ---- Update Register ----
    if (reg_read(base, offset, &reg_val) != 0) {
    	fault = STATUS_ERR_HW_FAILED;
        goto result;
    }

    user_value &= 0xFFFF;
    if (upper_flag) {
        reg_val = (reg_val & 0x0000FFFF) | (user_value << 16);
    } else {
        reg_val = (reg_val & 0xFFFF0000) | user_value;
    }
    reg_write(base, offset, reg_val);
    
    // ---- Read Back Verification ----
    reg_read(base, offset, &reg_val);
    read_16 = upper_flag ? ((reg_val >> 16) & 0xFFFF) : (reg_val & 0xFFFF);

    if (read_16 != user_value) 
    	fault = STATUS_ERR_MISMATCH;
    
    result:
    snprintf(buf, sizeof(buf), 
    	"{ \"status\": %d }", 
    	fault);
             
    return buf;
}

char* CANout_tcu1lc1_wrpara_reg21(uint32_t upper_flag, uint32_t user_value){

    static char buf[128];   
    status_code_t fault = STATUS_SUCCESS;

    uint32_t base =0xA4107000, offset =0x154, reg_val;
    uint16_t read_16;

    if (upper_flag > 1) {
        fault = STATUS_ERR_INVALID_FLAG;
        goto result;
    }

    // ---- Update Register ----
    if (reg_read(base, offset, &reg_val) != 0) {
    	fault = STATUS_ERR_HW_FAILED;
        goto result;
    }

    user_value &= 0xFFFF;
    if (upper_flag) {
        reg_val = (reg_val & 0x0000FFFF) | (user_value << 16);
    } else {
        reg_val = (reg_val & 0xFFFF0000) | user_value;
    }
    reg_write(base, offset, reg_val);
    
    // ---- Read Back Verification ----
    reg_read(base, offset, &reg_val);
    read_16 = upper_flag ? ((reg_val >> 16) & 0xFFFF) : (reg_val & 0xFFFF);

    if (read_16 != user_value) 
    	fault = STATUS_ERR_MISMATCH;
    
    result:
    snprintf(buf, sizeof(buf), 
    	"{ \"status\": %d }", 
    	fault);
             
    return buf;
}

char* CANout_tcu1lc1_wrpara_reg22(uint32_t upper_flag, uint32_t user_value){

    static char buf[128];   
    status_code_t fault = STATUS_SUCCESS;

    uint32_t base =0xA4107000, offset =0x158, reg_val;
    uint16_t read_16;

    if (upper_flag > 1) {
        fault = STATUS_ERR_INVALID_FLAG;
        goto result;
    }

    // ---- Update Register ----
    if (reg_read(base, offset, &reg_val) != 0) {
    	fault = STATUS_ERR_HW_FAILED;
        goto result;
    }

    user_value &= 0xFFFF;
    if (upper_flag) {
        reg_val = (reg_val & 0x0000FFFF) | (user_value << 16);
    } else {
        reg_val = (reg_val & 0xFFFF0000) | user_value;
    }
    reg_write(base, offset, reg_val);
    
    // ---- Read Back Verification ----
    reg_read(base, offset, &reg_val);
    read_16 = upper_flag ? ((reg_val >> 16) & 0xFFFF) : (reg_val & 0xFFFF);

    if (read_16 != user_value) 
    	fault = STATUS_ERR_MISMATCH;
    
    result:
    snprintf(buf, sizeof(buf), 
    	"{ \"status\": %d }", 
    	fault);
             
    return buf;
}

char* CANout_tcu1lc1_wrpara_reg23(uint32_t upper_flag, uint32_t user_value){

    static char buf[128];   
    status_code_t fault = STATUS_SUCCESS;

    uint32_t base =0xA4107000, offset =0x15C, reg_val;
    uint16_t read_16;

    if (upper_flag > 1) {
        fault = STATUS_ERR_INVALID_FLAG;
        goto result;
    }

    // ---- Update Register ----
    if (reg_read(base, offset, &reg_val) != 0) {
    	fault = STATUS_ERR_HW_FAILED;
        goto result;
    }

    user_value &= 0xFFFF;
    if (upper_flag) {
        reg_val = (reg_val & 0x0000FFFF) | (user_value << 16);
    } else {
        reg_val = (reg_val & 0xFFFF0000) | user_value;
    }
    reg_write(base, offset, reg_val);
    
    // ---- Read Back Verification ----
    reg_read(base, offset, &reg_val);
    read_16 = upper_flag ? ((reg_val >> 16) & 0xFFFF) : (reg_val & 0xFFFF);

    if (read_16 != user_value) 
    	fault = STATUS_ERR_MISMATCH;
    
    result:
    snprintf(buf, sizeof(buf), 
    	"{ \"status\": %d }", 
    	fault);
             
    return buf;
}

char* CANout_tcu1lc1_wrpara_reg24(uint32_t upper_flag, uint32_t user_value){

    static char buf[128];   
    status_code_t fault = STATUS_SUCCESS;

    uint32_t base =0xA4107000, offset =0x160, reg_val;
    uint16_t read_16;

    if (upper_flag > 1) {
        fault = STATUS_ERR_INVALID_FLAG;
        goto result;
    }

    // ---- Update Register ----
    if (reg_read(base, offset, &reg_val) != 0) {
    	fault = STATUS_ERR_HW_FAILED;
        goto result;
    }

    user_value &= 0xFFFF;
    if (upper_flag) {
        reg_val = (reg_val & 0x0000FFFF) | (user_value << 16);
    } else {
        reg_val = (reg_val & 0xFFFF0000) | user_value;
    }
    reg_write(base, offset, reg_val);
    
    // ---- Read Back Verification ----
    reg_read(base, offset, &reg_val);
    read_16 = upper_flag ? ((reg_val >> 16) & 0xFFFF) : (reg_val & 0xFFFF);

    if (read_16 != user_value) 
    	fault = STATUS_ERR_MISMATCH;
    
    result:
    snprintf(buf, sizeof(buf), 
    	"{ \"status\": %d }", 
    	fault);
             
    return buf;
}

char* CANout_tcu1lc1_wrpara_reg25(uint32_t upper_flag, uint32_t user_value){

    static char buf[128];   
    status_code_t fault = STATUS_SUCCESS;

    uint32_t base =0xA4107000, offset =0x164, reg_val;
    uint16_t read_16;

    if (upper_flag > 1) {
        fault = STATUS_ERR_INVALID_FLAG;
        goto result;
    }

    // ---- Update Register ----
    if (reg_read(base, offset, &reg_val) != 0) {
    	fault = STATUS_ERR_HW_FAILED;
        goto result;
    }

    user_value &= 0xFFFF;
    if (upper_flag) {
        reg_val = (reg_val & 0x0000FFFF) | (user_value << 16);
    } else {
        reg_val = (reg_val & 0xFFFF0000) | user_value;
    }
    reg_write(base, offset, reg_val);
    
    // ---- Read Back Verification ----
    reg_read(base, offset, &reg_val);
    read_16 = upper_flag ? ((reg_val >> 16) & 0xFFFF) : (reg_val & 0xFFFF);

    if (read_16 != user_value) 
    	fault = STATUS_ERR_MISMATCH;
    
    result:
    snprintf(buf, sizeof(buf), 
    	"{ \"status\": %d }", 
    	fault);
             
    return buf;
}

char* CANout_tcu1lc2_wrpara_reg0(uint32_t upper_flag, uint32_t user_value){

    static char buf[128];   
    status_code_t fault = STATUS_SUCCESS;

    uint32_t base =0xA4107000, offset =0x400, reg_val;
    uint16_t read_16;

    if (upper_flag > 1) {
        fault = STATUS_ERR_INVALID_FLAG;
        goto result;
    }

    // ---- Update Register ----
    if (reg_read(base, offset, &reg_val) != 0) {
    	fault = STATUS_ERR_HW_FAILED;
        goto result;
    }

    user_value &= 0xFFFF;
    if (upper_flag) {
        reg_val = (reg_val & 0x0000FFFF) | (user_value << 16);
    } else {
        reg_val = (reg_val & 0xFFFF0000) | user_value;
    }
    reg_write(base, offset, reg_val);
    
    // ---- Read Back Verification ----
    reg_read(base, offset, &reg_val);
    read_16 = upper_flag ? ((reg_val >> 16) & 0xFFFF) : (reg_val & 0xFFFF);

    if (read_16 != user_value) 
    	fault = STATUS_ERR_MISMATCH;
    
    result:
    snprintf(buf, sizeof(buf), 
    	"{ \"status\": %d }", 
    	fault);
             
    return buf;
}

char* CANout_tcu1lc2_wrpara_reg1(uint32_t upper_flag, uint32_t user_value){

    static char buf[128];   
    status_code_t fault = STATUS_SUCCESS;

    uint32_t base =0xA4107000, offset =0x404, reg_val;
    uint16_t read_16;

    if (upper_flag > 1) {
        fault = STATUS_ERR_INVALID_FLAG;
        goto result;
    }

    // ---- Update Register ----
    if (reg_read(base, offset, &reg_val) != 0) {
    	fault = STATUS_ERR_HW_FAILED;
        goto result;
    }

    user_value &= 0xFFFF;
    if (upper_flag) {
        reg_val = (reg_val & 0x0000FFFF) | (user_value << 16);
    } else {
        reg_val = (reg_val & 0xFFFF0000) | user_value;
    }
    reg_write(base, offset, reg_val);
    
    // ---- Read Back Verification ----
    reg_read(base, offset, &reg_val);
    read_16 = upper_flag ? ((reg_val >> 16) & 0xFFFF) : (reg_val & 0xFFFF);

    if (read_16 != user_value) 
    	fault = STATUS_ERR_MISMATCH;
    
    result:
    snprintf(buf, sizeof(buf), 
    	"{ \"status\": %d }", 
    	fault);
             
    return buf;
}

char* CANout_tcu1lc2_wrpara_reg2(uint32_t upper_flag, uint32_t user_value){

    static char buf[128];   
    status_code_t fault = STATUS_SUCCESS;

    uint32_t base =0xA4107000, offset =0x408, reg_val;
    uint16_t read_16;

    if (upper_flag > 1) {
        fault = STATUS_ERR_INVALID_FLAG;
        goto result;
    }

    // ---- Update Register ----
    if (reg_read(base, offset, &reg_val) != 0) {
    	fault = STATUS_ERR_HW_FAILED;
        goto result;
    }

    user_value &= 0xFFFF;
    if (upper_flag) {
        reg_val = (reg_val & 0x0000FFFF) | (user_value << 16);
    } else {
        reg_val = (reg_val & 0xFFFF0000) | user_value;
    }
    reg_write(base, offset, reg_val);
    
    // ---- Read Back Verification ----
    reg_read(base, offset, &reg_val);
    read_16 = upper_flag ? ((reg_val >> 16) & 0xFFFF) : (reg_val & 0xFFFF);

    if (read_16 != user_value) 
    	fault = STATUS_ERR_MISMATCH;
    
    result:
    snprintf(buf, sizeof(buf), 
    	"{ \"status\": %d }", 
    	fault);
             
    return buf;
}

char* CANout_tcu1lc2_wrpara_reg3(uint32_t upper_flag, uint32_t user_value){

    static char buf[128];   
    status_code_t fault = STATUS_SUCCESS;

    uint32_t base =0xA4107000, offset =0x40C, reg_val;
    uint16_t read_16;

    if (upper_flag > 1) {
        fault = STATUS_ERR_INVALID_FLAG;
        goto result;
    }

    // ---- Update Register ----
    if (reg_read(base, offset, &reg_val) != 0) {
    	fault = STATUS_ERR_HW_FAILED;
        goto result;
    }

    user_value &= 0xFFFF;
    if (upper_flag) {
        reg_val = (reg_val & 0x0000FFFF) | (user_value << 16);
    } else {
        reg_val = (reg_val & 0xFFFF0000) | user_value;
    }
    reg_write(base, offset, reg_val);
    
    // ---- Read Back Verification ----
    reg_read(base, offset, &reg_val);
    read_16 = upper_flag ? ((reg_val >> 16) & 0xFFFF) : (reg_val & 0xFFFF);

    if (read_16 != user_value) 
    	fault = STATUS_ERR_MISMATCH;
    
    result:
    snprintf(buf, sizeof(buf), 
    	"{ \"status\": %d }", 
    	fault);
             
    return buf;
}

char* CANout_tcu1lc2_wrpara_reg4(uint32_t upper_flag, uint32_t user_value){

    static char buf[128];   
    status_code_t fault = STATUS_SUCCESS;

    uint32_t base =0xA4107000, offset =0x410, reg_val;
    uint16_t read_16;

    if (upper_flag > 1) {
        fault = STATUS_ERR_INVALID_FLAG;
        goto result;
    }

    // ---- Update Register ----
    if (reg_read(base, offset, &reg_val) != 0) {
    	fault = STATUS_ERR_HW_FAILED;
        goto result;
    }

    user_value &= 0xFFFF;
    if (upper_flag) {
        reg_val = (reg_val & 0x0000FFFF) | (user_value << 16);
    } else {
        reg_val = (reg_val & 0xFFFF0000) | user_value;
    }
    reg_write(base, offset, reg_val);
    
    // ---- Read Back Verification ----
    reg_read(base, offset, &reg_val);
    read_16 = upper_flag ? ((reg_val >> 16) & 0xFFFF) : (reg_val & 0xFFFF);

    if (read_16 != user_value) 
    	fault = STATUS_ERR_MISMATCH;
    
    result:
    snprintf(buf, sizeof(buf), 
    	"{ \"status\": %d }", 
    	fault);
             
    return buf;
}

char* CANout_tcu1lc2_wrpara_reg5(uint32_t upper_flag, uint32_t user_value){

    static char buf[128];   
    status_code_t fault = STATUS_SUCCESS;

    uint32_t base =0xA4107000, offset =0x414, reg_val;
    uint16_t read_16;

    if (upper_flag > 1) {
        fault = STATUS_ERR_INVALID_FLAG;
        goto result;
    }

    // ---- Update Register ----
    if (reg_read(base, offset, &reg_val) != 0) {
    	fault = STATUS_ERR_HW_FAILED;
        goto result;
    }

    user_value &= 0xFFFF;
    if (upper_flag) {
        reg_val = (reg_val & 0x0000FFFF) | (user_value << 16);
    } else {
        reg_val = (reg_val & 0xFFFF0000) | user_value;
    }
    reg_write(base, offset, reg_val);
    
    // ---- Read Back Verification ----
    reg_read(base, offset, &reg_val);
    read_16 = upper_flag ? ((reg_val >> 16) & 0xFFFF) : (reg_val & 0xFFFF);

    if (read_16 != user_value) 
    	fault = STATUS_ERR_MISMATCH;
    
    result:
    snprintf(buf, sizeof(buf), 
    	"{ \"status\": %d }", 
    	fault);
             
    return buf;
}

char* CANout_tcu1lc2_wrpara_reg6(uint32_t upper_flag, uint32_t user_value){

    static char buf[128];   
    status_code_t fault = STATUS_SUCCESS;

    uint32_t base =0xA4107000, offset =0x418, reg_val;
    uint16_t read_16;

    if (upper_flag > 1) {
        fault = STATUS_ERR_INVALID_FLAG;
        goto result;
    }

    // ---- Update Register ----
    if (reg_read(base, offset, &reg_val) != 0) {
    	fault = STATUS_ERR_HW_FAILED;
        goto result;
    }

    user_value &= 0xFFFF;
    if (upper_flag) {
        reg_val = (reg_val & 0x0000FFFF) | (user_value << 16);
    } else {
        reg_val = (reg_val & 0xFFFF0000) | user_value;
    }
    reg_write(base, offset, reg_val);
    
    // ---- Read Back Verification ----
    reg_read(base, offset, &reg_val);
    read_16 = upper_flag ? ((reg_val >> 16) & 0xFFFF) : (reg_val & 0xFFFF);

    if (read_16 != user_value) 
    	fault = STATUS_ERR_MISMATCH;
    
    result:
    snprintf(buf, sizeof(buf), 
    	"{ \"status\": %d }", 
    	fault);
             
    return buf;
}

char* CANout_tcu1lc2_wrpara_reg7(uint32_t upper_flag, uint32_t user_value){

    static char buf[128];   
    status_code_t fault = STATUS_SUCCESS;

    uint32_t base =0xA4107000, offset =0x41C, reg_val;
    uint16_t read_16;

    if (upper_flag > 1) {
        fault = STATUS_ERR_INVALID_FLAG;
        goto result;
    }

    // ---- Update Register ----
    if (reg_read(base, offset, &reg_val) != 0) {
    	fault = STATUS_ERR_HW_FAILED;
        goto result;
    }

    user_value &= 0xFFFF;
    if (upper_flag) {
        reg_val = (reg_val & 0x0000FFFF) | (user_value << 16);
    } else {
        reg_val = (reg_val & 0xFFFF0000) | user_value;
    }
    reg_write(base, offset, reg_val);
    
    // ---- Read Back Verification ----
    reg_read(base, offset, &reg_val);
    read_16 = upper_flag ? ((reg_val >> 16) & 0xFFFF) : (reg_val & 0xFFFF);

    if (read_16 != user_value) 
    	fault = STATUS_ERR_MISMATCH;
    
    result:
    snprintf(buf, sizeof(buf), 
    	"{ \"status\": %d }", 
    	fault);
             
    return buf;
}

char* CANout_tcu1lc2_wrpara_reg8(uint32_t upper_flag, uint32_t user_value){

    static char buf[128];   
    status_code_t fault = STATUS_SUCCESS;

    uint32_t base =0xA4107000, offset =0x420, reg_val;
    uint16_t read_16;

    if (upper_flag > 1) {
        fault = STATUS_ERR_INVALID_FLAG;
        goto result;
    }

    // ---- Update Register ----
    if (reg_read(base, offset, &reg_val) != 0) {
    	fault = STATUS_ERR_HW_FAILED;
        goto result;
    }

    user_value &= 0xFFFF;
    if (upper_flag) {
        reg_val = (reg_val & 0x0000FFFF) | (user_value << 16);
    } else {
        reg_val = (reg_val & 0xFFFF0000) | user_value;
    }
    reg_write(base, offset, reg_val);
    
    // ---- Read Back Verification ----
    reg_read(base, offset, &reg_val);
    read_16 = upper_flag ? ((reg_val >> 16) & 0xFFFF) : (reg_val & 0xFFFF);

    if (read_16 != user_value) 
    	fault = STATUS_ERR_MISMATCH;
    
    result:
    snprintf(buf, sizeof(buf), 
    	"{ \"status\": %d }", 
    	fault);
             
    return buf;
}

char* CANout_tcu1lc2_wrpara_reg9(uint32_t upper_flag, uint32_t user_value){

    static char buf[128];   
    status_code_t fault = STATUS_SUCCESS;

    uint32_t base =0xA4107000, offset =0x424, reg_val;
    uint16_t read_16;

    if (upper_flag > 1) {
        fault = STATUS_ERR_INVALID_FLAG;
        goto result;
    }

    // ---- Update Register ----
    if (reg_read(base, offset, &reg_val) != 0) {
    	fault = STATUS_ERR_HW_FAILED;
        goto result;
    }

    user_value &= 0xFFFF;
    if (upper_flag) {
        reg_val = (reg_val & 0x0000FFFF) | (user_value << 16);
    } else {
        reg_val = (reg_val & 0xFFFF0000) | user_value;
    }
    reg_write(base, offset, reg_val);
    
    // ---- Read Back Verification ----
    reg_read(base, offset, &reg_val);
    read_16 = upper_flag ? ((reg_val >> 16) & 0xFFFF) : (reg_val & 0xFFFF);

    if (read_16 != user_value) 
    	fault = STATUS_ERR_MISMATCH;
    
    result:
    snprintf(buf, sizeof(buf), 
    	"{ \"status\": %d }", 
    	fault);
             
    return buf;
}

char* CANout_tcu1lc2_wrpara_reg10(uint32_t upper_flag, uint32_t user_value){

    static char buf[128];   
    status_code_t fault = STATUS_SUCCESS;

    uint32_t base =0xA4107000, offset =0x428, reg_val;
    uint16_t read_16;

    if (upper_flag > 1) {
        fault = STATUS_ERR_INVALID_FLAG;
        goto result;
    }

    // ---- Update Register ----
    if (reg_read(base, offset, &reg_val) != 0) {
    	fault = STATUS_ERR_HW_FAILED;
        goto result;
    }

    user_value &= 0xFFFF;
    if (upper_flag) {
        reg_val = (reg_val & 0x0000FFFF) | (user_value << 16);
    } else {
        reg_val = (reg_val & 0xFFFF0000) | user_value;
    }
    reg_write(base, offset, reg_val);
    
    // ---- Read Back Verification ----
    reg_read(base, offset, &reg_val);
    read_16 = upper_flag ? ((reg_val >> 16) & 0xFFFF) : (reg_val & 0xFFFF);

    if (read_16 != user_value) 
    	fault = STATUS_ERR_MISMATCH;
    
    result:
    snprintf(buf, sizeof(buf), 
    	"{ \"status\": %d }", 
    	fault);
             
    return buf;
}

char* CANout_tcu1lc2_wrpara_reg11(uint32_t upper_flag, uint32_t user_value){

    static char buf[128];   
    status_code_t fault = STATUS_SUCCESS;

    uint32_t base =0xA4107000, offset =0x42C, reg_val;
    uint16_t read_16;

    if (upper_flag > 1) {
        fault = STATUS_ERR_INVALID_FLAG;
        goto result;
    }

    // ---- Update Register ----
    if (reg_read(base, offset, &reg_val) != 0) {
    	fault = STATUS_ERR_HW_FAILED;
        goto result;
    }

    user_value &= 0xFFFF;
    if (upper_flag) {
        reg_val = (reg_val & 0x0000FFFF) | (user_value << 16);
    } else {
        reg_val = (reg_val & 0xFFFF0000) | user_value;
    }
    reg_write(base, offset, reg_val);
    
    // ---- Read Back Verification ----
    reg_read(base, offset, &reg_val);
    read_16 = upper_flag ? ((reg_val >> 16) & 0xFFFF) : (reg_val & 0xFFFF);

    if (read_16 != user_value) 
    	fault = STATUS_ERR_MISMATCH;
    
    result:
    snprintf(buf, sizeof(buf), 
    	"{ \"status\": %d }", 
    	fault);
             
    return buf;
}

char* CANout_tcu1lc2_wrpara_reg12(uint32_t upper_flag, uint32_t user_value){

    static char buf[128];   
    status_code_t fault = STATUS_SUCCESS;

    uint32_t base =0xA4107000, offset =0x430, reg_val;
    uint16_t read_16;

    if (upper_flag > 1) {
        fault = STATUS_ERR_INVALID_FLAG;
        goto result;
    }

    // ---- Update Register ----
    if (reg_read(base, offset, &reg_val) != 0) {
    	fault = STATUS_ERR_HW_FAILED;
        goto result;
    }

    user_value &= 0xFFFF;
    if (upper_flag) {
        reg_val = (reg_val & 0x0000FFFF) | (user_value << 16);
    } else {
        reg_val = (reg_val & 0xFFFF0000) | user_value;
    }
    reg_write(base, offset, reg_val);
    
    // ---- Read Back Verification ----
    reg_read(base, offset, &reg_val);
    read_16 = upper_flag ? ((reg_val >> 16) & 0xFFFF) : (reg_val & 0xFFFF);

    if (read_16 != user_value) 
    	fault = STATUS_ERR_MISMATCH;
    
    result:
    snprintf(buf, sizeof(buf), 
    	"{ \"status\": %d }", 
    	fault);
             
    return buf;
}

char* CANout_tcu1lc2_wrpara_reg13(uint32_t upper_flag, uint32_t user_value){

    static char buf[128];   
    status_code_t fault = STATUS_SUCCESS;

    uint32_t base =0xA4107000, offset =0x434, reg_val;
    uint16_t read_16;

    if (upper_flag > 1) {
        fault = STATUS_ERR_INVALID_FLAG;
        goto result;
    }

    // ---- Update Register ----
    if (reg_read(base, offset, &reg_val) != 0) {
    	fault = STATUS_ERR_HW_FAILED;
        goto result;
    }

    user_value &= 0xFFFF;
    if (upper_flag) {
        reg_val = (reg_val & 0x0000FFFF) | (user_value << 16);
    } else {
        reg_val = (reg_val & 0xFFFF0000) | user_value;
    }
    reg_write(base, offset, reg_val);
    
    // ---- Read Back Verification ----
    reg_read(base, offset, &reg_val);
    read_16 = upper_flag ? ((reg_val >> 16) & 0xFFFF) : (reg_val & 0xFFFF);

    if (read_16 != user_value) 
    	fault = STATUS_ERR_MISMATCH;
    
    result:
    snprintf(buf, sizeof(buf), 
    	"{ \"status\": %d }", 
    	fault);
             
    return buf;
}

char* CANout_tcu1lc2_wrpara_reg14(uint32_t upper_flag, uint32_t user_value){

    static char buf[128];   
    status_code_t fault = STATUS_SUCCESS;

    uint32_t base =0xA4107000, offset =0x438, reg_val;
    uint16_t read_16;

    if (upper_flag > 1) {
        fault = STATUS_ERR_INVALID_FLAG;
        goto result;
    }

    // ---- Update Register ----
    if (reg_read(base, offset, &reg_val) != 0) {
    	fault = STATUS_ERR_HW_FAILED;
        goto result;
    }

    user_value &= 0xFFFF;
    if (upper_flag) {
        reg_val = (reg_val & 0x0000FFFF) | (user_value << 16);
    } else {
        reg_val = (reg_val & 0xFFFF0000) | user_value;
    }
    reg_write(base, offset, reg_val);
    
    // ---- Read Back Verification ----
    reg_read(base, offset, &reg_val);
    read_16 = upper_flag ? ((reg_val >> 16) & 0xFFFF) : (reg_val & 0xFFFF);

    if (read_16 != user_value) 
    	fault = STATUS_ERR_MISMATCH;
    
    result:
    snprintf(buf, sizeof(buf), 
    	"{ \"status\": %d }", 
    	fault);
             
    return buf;
}

char* CANout_tcu1lc2_wrpara_reg15(uint32_t upper_flag, uint32_t user_value){

    static char buf[128];   
    status_code_t fault = STATUS_SUCCESS;

    uint32_t base =0xA4107000, offset =0x43C, reg_val;
    uint16_t read_16;

    if (upper_flag > 1) {
        fault = STATUS_ERR_INVALID_FLAG;
        goto result;
    }

    // ---- Update Register ----
    if (reg_read(base, offset, &reg_val) != 0) {
    	fault = STATUS_ERR_HW_FAILED;
        goto result;
    }

    user_value &= 0xFFFF;
    if (upper_flag) {
        reg_val = (reg_val & 0x0000FFFF) | (user_value << 16);
    } else {
        reg_val = (reg_val & 0xFFFF0000) | user_value;
    }
    reg_write(base, offset, reg_val);
    
    // ---- Read Back Verification ----
    reg_read(base, offset, &reg_val);
    read_16 = upper_flag ? ((reg_val >> 16) & 0xFFFF) : (reg_val & 0xFFFF);

    if (read_16 != user_value) 
    	fault = STATUS_ERR_MISMATCH;
    
    result:
    snprintf(buf, sizeof(buf), 
    	"{ \"status\": %d }", 
    	fault);
             
    return buf;
}

char* CANout_tcu1lc2_wrpara_reg16(uint32_t upper_flag, uint32_t user_value){

    static char buf[128];   
    status_code_t fault = STATUS_SUCCESS;

    uint32_t base =0xA4107000, offset =0x440, reg_val;
    uint16_t read_16;

    if (upper_flag > 1) {
        fault = STATUS_ERR_INVALID_FLAG;
        goto result;
    }

    // ---- Update Register ----
    if (reg_read(base, offset, &reg_val) != 0) {
    	fault = STATUS_ERR_HW_FAILED;
        goto result;
    }

    user_value &= 0xFFFF;
    if (upper_flag) {
        reg_val = (reg_val & 0x0000FFFF) | (user_value << 16);
    } else {
        reg_val = (reg_val & 0xFFFF0000) | user_value;
    }
    reg_write(base, offset, reg_val);
    
    // ---- Read Back Verification ----
    reg_read(base, offset, &reg_val);
    read_16 = upper_flag ? ((reg_val >> 16) & 0xFFFF) : (reg_val & 0xFFFF);

    if (read_16 != user_value) 
    	fault = STATUS_ERR_MISMATCH;
    
    result:
    snprintf(buf, sizeof(buf), 
    	"{ \"status\": %d }", 
    	fault);
             
    return buf;
}

char* CANout_tcu1lc2_wrpara_reg17(uint32_t upper_flag, uint32_t user_value){

    static char buf[128];   
    status_code_t fault = STATUS_SUCCESS;

    uint32_t base =0xA4107000, offset =0x444, reg_val;
    uint16_t read_16;

    if (upper_flag > 1) {
        fault = STATUS_ERR_INVALID_FLAG;
        goto result;
    }

    // ---- Update Register ----
    if (reg_read(base, offset, &reg_val) != 0) {
    	fault = STATUS_ERR_HW_FAILED;
        goto result;
    }

    user_value &= 0xFFFF;
    if (upper_flag) {
        reg_val = (reg_val & 0x0000FFFF) | (user_value << 16);
    } else {
        reg_val = (reg_val & 0xFFFF0000) | user_value;
    }
    reg_write(base, offset, reg_val);
    
    // ---- Read Back Verification ----
    reg_read(base, offset, &reg_val);
    read_16 = upper_flag ? ((reg_val >> 16) & 0xFFFF) : (reg_val & 0xFFFF);

    if (read_16 != user_value) 
    	fault = STATUS_ERR_MISMATCH;
    
    result:
    snprintf(buf, sizeof(buf), 
    	"{ \"status\": %d }", 
    	fault);
             
    return buf;
}

char* CANout_tcu1lc2_wrpara_reg18(uint32_t upper_flag, uint32_t user_value){

    static char buf[128];   
    status_code_t fault = STATUS_SUCCESS;

    uint32_t base =0xA4107000, offset =0x448, reg_val;
    uint16_t read_16;

    if (upper_flag > 1) {
        fault = STATUS_ERR_INVALID_FLAG;
        goto result;
    }

    // ---- Update Register ----
    if (reg_read(base, offset, &reg_val) != 0) {
    	fault = STATUS_ERR_HW_FAILED;
        goto result;
    }

    user_value &= 0xFFFF;
    if (upper_flag) {
        reg_val = (reg_val & 0x0000FFFF) | (user_value << 16);
    } else {
        reg_val = (reg_val & 0xFFFF0000) | user_value;
    }
    reg_write(base, offset, reg_val);
    
    // ---- Read Back Verification ----
    reg_read(base, offset, &reg_val);
    read_16 = upper_flag ? ((reg_val >> 16) & 0xFFFF) : (reg_val & 0xFFFF);

    if (read_16 != user_value) 
    	fault = STATUS_ERR_MISMATCH;
    
    result:
    snprintf(buf, sizeof(buf), 
    	"{ \"status\": %d }", 
    	fault);
             
    return buf;
}

char* CANout_tcu1lc2_wrpara_reg19(uint32_t upper_flag, uint32_t user_value){

    static char buf[128];   
    status_code_t fault = STATUS_SUCCESS;

    uint32_t base =0xA4107000, offset =0x44C, reg_val;
    uint16_t read_16;

    if (upper_flag > 1) {
        fault = STATUS_ERR_INVALID_FLAG;
        goto result;
    }

    // ---- Update Register ----
    if (reg_read(base, offset, &reg_val) != 0) {
    	fault = STATUS_ERR_HW_FAILED;
        goto result;
    }

    user_value &= 0xFFFF;
    if (upper_flag) {
        reg_val = (reg_val & 0x0000FFFF) | (user_value << 16);
    } else {
        reg_val = (reg_val & 0xFFFF0000) | user_value;
    }
    reg_write(base, offset, reg_val);
    
    // ---- Read Back Verification ----
    reg_read(base, offset, &reg_val);
    read_16 = upper_flag ? ((reg_val >> 16) & 0xFFFF) : (reg_val & 0xFFFF);

    if (read_16 != user_value) 
    	fault = STATUS_ERR_MISMATCH;
    
    result:
    snprintf(buf, sizeof(buf), 
    	"{ \"status\": %d }", 
    	fault);
             
    return buf;
}

char* CANout_tcu1lc2_wrpara_reg20(uint32_t upper_flag, uint32_t user_value){

    static char buf[128];   
    status_code_t fault = STATUS_SUCCESS;

    uint32_t base =0xA4107000, offset =0x450, reg_val;
    uint16_t read_16;

    if (upper_flag > 1) {
        fault = STATUS_ERR_INVALID_FLAG;
        goto result;
    }

    // ---- Update Register ----
    if (reg_read(base, offset, &reg_val) != 0) {
    	fault = STATUS_ERR_HW_FAILED;
        goto result;
    }

    user_value &= 0xFFFF;
    if (upper_flag) {
        reg_val = (reg_val & 0x0000FFFF) | (user_value << 16);
    } else {
        reg_val = (reg_val & 0xFFFF0000) | user_value;
    }
    reg_write(base, offset, reg_val);
    
    // ---- Read Back Verification ----
    reg_read(base, offset, &reg_val);
    read_16 = upper_flag ? ((reg_val >> 16) & 0xFFFF) : (reg_val & 0xFFFF);

    if (read_16 != user_value) 
    	fault = STATUS_ERR_MISMATCH;
    
    result:
    snprintf(buf, sizeof(buf), 
    	"{ \"status\": %d }", 
    	fault);
             
    return buf;
}

char* CANout_tcu1lc2_wrpara_reg21(uint32_t upper_flag, uint32_t user_value){

    static char buf[128];   
    status_code_t fault = STATUS_SUCCESS;

    uint32_t base =0xA4107000, offset =0x454, reg_val;
    uint16_t read_16;

    if (upper_flag > 1) {
        fault = STATUS_ERR_INVALID_FLAG;
        goto result;
    }

    // ---- Update Register ----
    if (reg_read(base, offset, &reg_val) != 0) {
    	fault = STATUS_ERR_HW_FAILED;
        goto result;
    }

    user_value &= 0xFFFF;
    if (upper_flag) {
        reg_val = (reg_val & 0x0000FFFF) | (user_value << 16);
    } else {
        reg_val = (reg_val & 0xFFFF0000) | user_value;
    }
    reg_write(base, offset, reg_val);
    
    // ---- Read Back Verification ----
    reg_read(base, offset, &reg_val);
    read_16 = upper_flag ? ((reg_val >> 16) & 0xFFFF) : (reg_val & 0xFFFF);

    if (read_16 != user_value) 
    	fault = STATUS_ERR_MISMATCH;
    
    result:
    snprintf(buf, sizeof(buf), 
    	"{ \"status\": %d }", 
    	fault);
             
    return buf;
}

char* CANout_tcu1lc2_wrpara_reg22(uint32_t upper_flag, uint32_t user_value){

    static char buf[128];   
    status_code_t fault = STATUS_SUCCESS;

    uint32_t base =0xA4107000, offset =0x458, reg_val;
    uint16_t read_16;

    if (upper_flag > 1) {
        fault = STATUS_ERR_INVALID_FLAG;
        goto result;
    }

    // ---- Update Register ----
    if (reg_read(base, offset, &reg_val) != 0) {
    	fault = STATUS_ERR_HW_FAILED;
        goto result;
    }

    user_value &= 0xFFFF;
    if (upper_flag) {
        reg_val = (reg_val & 0x0000FFFF) | (user_value << 16);
    } else {
        reg_val = (reg_val & 0xFFFF0000) | user_value;
    }
    reg_write(base, offset, reg_val);
    
    // ---- Read Back Verification ----
    reg_read(base, offset, &reg_val);
    read_16 = upper_flag ? ((reg_val >> 16) & 0xFFFF) : (reg_val & 0xFFFF);

    if (read_16 != user_value) 
    	fault = STATUS_ERR_MISMATCH;
    
    result:
    snprintf(buf, sizeof(buf), 
    	"{ \"status\": %d }", 
    	fault);
             
    return buf;
}

char* CANout_tcu1lc2_wrpara_reg23(uint32_t upper_flag, uint32_t user_value){

    static char buf[128];   
    status_code_t fault = STATUS_SUCCESS;

    uint32_t base =0xA4107000, offset =0x45C, reg_val;
    uint16_t read_16;

    if (upper_flag > 1) {
        fault = STATUS_ERR_INVALID_FLAG;
        goto result;
    }

    // ---- Update Register ----
    if (reg_read(base, offset, &reg_val) != 0) {
    	fault = STATUS_ERR_HW_FAILED;
        goto result;
    }

    user_value &= 0xFFFF;
    if (upper_flag) {
        reg_val = (reg_val & 0x0000FFFF) | (user_value << 16);
    } else {
        reg_val = (reg_val & 0xFFFF0000) | user_value;
    }
    reg_write(base, offset, reg_val);
    
    // ---- Read Back Verification ----
    reg_read(base, offset, &reg_val);
    read_16 = upper_flag ? ((reg_val >> 16) & 0xFFFF) : (reg_val & 0xFFFF);

    if (read_16 != user_value) 
    	fault = STATUS_ERR_MISMATCH;
    
    result:
    snprintf(buf, sizeof(buf), 
    	"{ \"status\": %d }", 
    	fault);
             
    return buf;
}

char* CANout_tcu1lc2_wrpara_reg24(uint32_t upper_flag, uint32_t user_value){

    static char buf[128];   
    status_code_t fault = STATUS_SUCCESS;

    uint32_t base =0xA4107000, offset =0x460, reg_val;
    uint16_t read_16;

    if (upper_flag > 1) {
        fault = STATUS_ERR_INVALID_FLAG;
        goto result;
    }

    // ---- Update Register ----
    if (reg_read(base, offset, &reg_val) != 0) {
    	fault = STATUS_ERR_HW_FAILED;
        goto result;
    }

    user_value &= 0xFFFF;
    if (upper_flag) {
        reg_val = (reg_val & 0x0000FFFF) | (user_value << 16);
    } else {
        reg_val = (reg_val & 0xFFFF0000) | user_value;
    }
    reg_write(base, offset, reg_val);
    
    // ---- Read Back Verification ----
    reg_read(base, offset, &reg_val);
    read_16 = upper_flag ? ((reg_val >> 16) & 0xFFFF) : (reg_val & 0xFFFF);

    if (read_16 != user_value) 
    	fault = STATUS_ERR_MISMATCH;
    
    result:
    snprintf(buf, sizeof(buf), 
    	"{ \"status\": %d }", 
    	fault);
             
    return buf;
}

char* CANout_tcu1lc2_wrpara_reg25(uint32_t upper_flag, uint32_t user_value){

    static char buf[128];   
    status_code_t fault = STATUS_SUCCESS;

    uint32_t base =0xA4107000, offset =0x464, reg_val;
    uint16_t read_16;

    if (upper_flag > 1) {
        fault = STATUS_ERR_INVALID_FLAG;
        goto result;
    }

    // ---- Update Register ----
    if (reg_read(base, offset, &reg_val) != 0) {
    	fault = STATUS_ERR_HW_FAILED;
        goto result;
    }

    user_value &= 0xFFFF;
    if (upper_flag) {
        reg_val = (reg_val & 0x0000FFFF) | (user_value << 16);
    } else {
        reg_val = (reg_val & 0xFFFF0000) | user_value;
    }
    reg_write(base, offset, reg_val);
    
    // ---- Read Back Verification ----
    reg_read(base, offset, &reg_val);
    read_16 = upper_flag ? ((reg_val >> 16) & 0xFFFF) : (reg_val & 0xFFFF);

    if (read_16 != user_value) 
    	fault = STATUS_ERR_MISMATCH;
    
    result:
    snprintf(buf, sizeof(buf), 
    	"{ \"status\": %d }", 
    	fault);
             
    return buf;
}

char* CANout_tcu1mc1_wrpara_reg0(uint32_t upper_flag, uint32_t user_value){

    static char buf[128];   
    status_code_t fault = STATUS_SUCCESS;

    uint32_t base =0xA4107000, offset =0x700, reg_val;
    uint16_t read_16;

    if (upper_flag > 1) {
        fault = STATUS_ERR_INVALID_FLAG;
        goto result;
    }

    // ---- Update Register ----
    if (reg_read(base, offset, &reg_val) != 0) {
    	fault = STATUS_ERR_HW_FAILED;
        goto result;
    }

    user_value &= 0xFFFF;
    if (upper_flag) {
        reg_val = (reg_val & 0x0000FFFF) | (user_value << 16);
    } else {
        reg_val = (reg_val & 0xFFFF0000) | user_value;
    }
    reg_write(base, offset, reg_val);
    
    // ---- Read Back Verification ----
    reg_read(base, offset, &reg_val);
    read_16 = upper_flag ? ((reg_val >> 16) & 0xFFFF) : (reg_val & 0xFFFF);

    if (read_16 != user_value) 
    	fault = STATUS_ERR_MISMATCH;
    
    result:
    snprintf(buf, sizeof(buf), 
    	"{ \"status\": %d }", 
    	fault);
             
    return buf;
}

char* CANout_tcu1mc1_wrpara_reg1(uint32_t upper_flag, uint32_t user_value){

    static char buf[128];   
    status_code_t fault = STATUS_SUCCESS;

    uint32_t base =0xA4107000, offset =0x704, reg_val;
    uint16_t read_16;

    if (upper_flag > 1) {
        fault = STATUS_ERR_INVALID_FLAG;
        goto result;
    }

    // ---- Update Register ----
    if (reg_read(base, offset, &reg_val) != 0) {
    	fault = STATUS_ERR_HW_FAILED;
        goto result;
    }

    user_value &= 0xFFFF;
    if (upper_flag) {
        reg_val = (reg_val & 0x0000FFFF) | (user_value << 16);
    } else {
        reg_val = (reg_val & 0xFFFF0000) | user_value;
    }
    reg_write(base, offset, reg_val);
    
    // ---- Read Back Verification ----
    reg_read(base, offset, &reg_val);
    read_16 = upper_flag ? ((reg_val >> 16) & 0xFFFF) : (reg_val & 0xFFFF);

    if (read_16 != user_value) 
    	fault = STATUS_ERR_MISMATCH;
    
    result:
    snprintf(buf, sizeof(buf), 
    	"{ \"status\": %d }", 
    	fault);
             
    return buf;
}

char* CANout_tcu1mc1_wrpara_reg2(uint32_t upper_flag, uint32_t user_value){

    static char buf[128];   
    status_code_t fault = STATUS_SUCCESS;

    uint32_t base =0xA4107000, offset =0x708, reg_val;
    uint16_t read_16;

    if (upper_flag > 1) {
        fault = STATUS_ERR_INVALID_FLAG;
        goto result;
    }

    // ---- Update Register ----
    if (reg_read(base, offset, &reg_val) != 0) {
    	fault = STATUS_ERR_HW_FAILED;
        goto result;
    }

    user_value &= 0xFFFF;
    if (upper_flag) {
        reg_val = (reg_val & 0x0000FFFF) | (user_value << 16);
    } else {
        reg_val = (reg_val & 0xFFFF0000) | user_value;
    }
    reg_write(base, offset, reg_val);
    
    // ---- Read Back Verification ----
    reg_read(base, offset, &reg_val);
    read_16 = upper_flag ? ((reg_val >> 16) & 0xFFFF) : (reg_val & 0xFFFF);

    if (read_16 != user_value) 
    	fault = STATUS_ERR_MISMATCH;
    
    result:
    snprintf(buf, sizeof(buf), 
    	"{ \"status\": %d }", 
    	fault);
             
    return buf;
}

char* CANout_tcu1mc1_wrpara_reg3(uint32_t upper_flag, uint32_t user_value){

    static char buf[128];   
    status_code_t fault = STATUS_SUCCESS;

    uint32_t base =0xA4107000, offset =0x70C, reg_val;
    uint16_t read_16;

    if (upper_flag > 1) {
        fault = STATUS_ERR_INVALID_FLAG;
        goto result;
    }

    // ---- Update Register ----
    if (reg_read(base, offset, &reg_val) != 0) {
    	fault = STATUS_ERR_HW_FAILED;
        goto result;
    }

    user_value &= 0xFFFF;
    if (upper_flag) {
        reg_val = (reg_val & 0x0000FFFF) | (user_value << 16);
    } else {
        reg_val = (reg_val & 0xFFFF0000) | user_value;
    }
    reg_write(base, offset, reg_val);
    
    // ---- Read Back Verification ----
    reg_read(base, offset, &reg_val);
    read_16 = upper_flag ? ((reg_val >> 16) & 0xFFFF) : (reg_val & 0xFFFF);

    if (read_16 != user_value) 
    	fault = STATUS_ERR_MISMATCH;
    
    result:
    snprintf(buf, sizeof(buf), 
    	"{ \"status\": %d }", 
    	fault);
             
    return buf;
}

char* CANout_tcu1mc1_wrpara_reg4(uint32_t upper_flag, uint32_t user_value){

    static char buf[128];   
    status_code_t fault = STATUS_SUCCESS;

    uint32_t base =0xA4107000, offset =0x710, reg_val;
    uint16_t read_16;

    if (upper_flag > 1) {
        fault = STATUS_ERR_INVALID_FLAG;
        goto result;
    }

    // ---- Update Register ----
    if (reg_read(base, offset, &reg_val) != 0) {
    	fault = STATUS_ERR_HW_FAILED;
        goto result;
    }

    user_value &= 0xFFFF;
    if (upper_flag) {
        reg_val = (reg_val & 0x0000FFFF) | (user_value << 16);
    } else {
        reg_val = (reg_val & 0xFFFF0000) | user_value;
    }
    reg_write(base, offset, reg_val);
    
    // ---- Read Back Verification ----
    reg_read(base, offset, &reg_val);
    read_16 = upper_flag ? ((reg_val >> 16) & 0xFFFF) : (reg_val & 0xFFFF);

    if (read_16 != user_value) 
    	fault = STATUS_ERR_MISMATCH;
    
    result:
    snprintf(buf, sizeof(buf), 
    	"{ \"status\": %d }", 
    	fault);
             
    return buf;
}

char* CANout_tcu1mc1_wrpara_reg5(uint32_t upper_flag, uint32_t user_value){

    static char buf[128];   
    status_code_t fault = STATUS_SUCCESS;

    uint32_t base =0xA4107000, offset =0x714, reg_val;
    uint16_t read_16;

    if (upper_flag > 1) {
        fault = STATUS_ERR_INVALID_FLAG;
        goto result;
    }

    // ---- Update Register ----
    if (reg_read(base, offset, &reg_val) != 0) {
    	fault = STATUS_ERR_HW_FAILED;
        goto result;
    }

    user_value &= 0xFFFF;
    if (upper_flag) {
        reg_val = (reg_val & 0x0000FFFF) | (user_value << 16);
    } else {
        reg_val = (reg_val & 0xFFFF0000) | user_value;
    }
    reg_write(base, offset, reg_val);
    
    // ---- Read Back Verification ----
    reg_read(base, offset, &reg_val);
    read_16 = upper_flag ? ((reg_val >> 16) & 0xFFFF) : (reg_val & 0xFFFF);

    if (read_16 != user_value) 
    	fault = STATUS_ERR_MISMATCH;
    
    result:
    snprintf(buf, sizeof(buf), 
    	"{ \"status\": %d }", 
    	fault);
             
    return buf;
}

char* CANout_tcu1mc1_wrpara_reg6(uint32_t upper_flag, uint32_t user_value){

    static char buf[128];   
    status_code_t fault = STATUS_SUCCESS;

    uint32_t base =0xA4107000, offset =0x718, reg_val;
    uint16_t read_16;

    if (upper_flag > 1) {
        fault = STATUS_ERR_INVALID_FLAG;
        goto result;
    }

    // ---- Update Register ----
    if (reg_read(base, offset, &reg_val) != 0) {
    	fault = STATUS_ERR_HW_FAILED;
        goto result;
    }

    user_value &= 0xFFFF;
    if (upper_flag) {
        reg_val = (reg_val & 0x0000FFFF) | (user_value << 16);
    } else {
        reg_val = (reg_val & 0xFFFF0000) | user_value;
    }
    reg_write(base, offset, reg_val);
    
    // ---- Read Back Verification ----
    reg_read(base, offset, &reg_val);
    read_16 = upper_flag ? ((reg_val >> 16) & 0xFFFF) : (reg_val & 0xFFFF);

    if (read_16 != user_value) 
    	fault = STATUS_ERR_MISMATCH;
    
    result:
    snprintf(buf, sizeof(buf), 
    	"{ \"status\": %d }", 
    	fault);
             
    return buf;
}

char* CANout_tcu1mc1_wrpara_reg7(uint32_t upper_flag, uint32_t user_value){

    static char buf[128];   
    status_code_t fault = STATUS_SUCCESS;

    uint32_t base =0xA4107000, offset =0x71C, reg_val;
    uint16_t read_16;

    if (upper_flag > 1) {
        fault = STATUS_ERR_INVALID_FLAG;
        goto result;
    }

    // ---- Update Register ----
    if (reg_read(base, offset, &reg_val) != 0) {
    	fault = STATUS_ERR_HW_FAILED;
        goto result;
    }

    user_value &= 0xFFFF;
    if (upper_flag) {
        reg_val = (reg_val & 0x0000FFFF) | (user_value << 16);
    } else {
        reg_val = (reg_val & 0xFFFF0000) | user_value;
    }
    reg_write(base, offset, reg_val);
    
    // ---- Read Back Verification ----
    reg_read(base, offset, &reg_val);
    read_16 = upper_flag ? ((reg_val >> 16) & 0xFFFF) : (reg_val & 0xFFFF);

    if (read_16 != user_value) 
    	fault = STATUS_ERR_MISMATCH;
    
    result:
    snprintf(buf, sizeof(buf), 
    	"{ \"status\": %d }", 
    	fault);
             
    return buf;
}

char* CANout_tcu1mc1_wrpara_reg8(uint32_t upper_flag, uint32_t user_value){

    static char buf[128];   
    status_code_t fault = STATUS_SUCCESS;

    uint32_t base =0xA4107000, offset =0x720, reg_val;
    uint16_t read_16;

    if (upper_flag > 1) {
        fault = STATUS_ERR_INVALID_FLAG;
        goto result;
    }

    // ---- Update Register ----
    if (reg_read(base, offset, &reg_val) != 0) {
    	fault = STATUS_ERR_HW_FAILED;
        goto result;
    }

    user_value &= 0xFFFF;
    if (upper_flag) {
        reg_val = (reg_val & 0x0000FFFF) | (user_value << 16);
    } else {
        reg_val = (reg_val & 0xFFFF0000) | user_value;
    }
    reg_write(base, offset, reg_val);
    
    // ---- Read Back Verification ----
    reg_read(base, offset, &reg_val);
    read_16 = upper_flag ? ((reg_val >> 16) & 0xFFFF) : (reg_val & 0xFFFF);

    if (read_16 != user_value) 
    	fault = STATUS_ERR_MISMATCH;
    
    result:
    snprintf(buf, sizeof(buf), 
    	"{ \"status\": %d }", 
    	fault);
             
    return buf;
}

char* CANout_tcu1mc1_wrpara_reg9(uint32_t upper_flag, uint32_t user_value){

    static char buf[128];   
    status_code_t fault = STATUS_SUCCESS;

    uint32_t base =0xA4107000, offset =0x724, reg_val;
    uint16_t read_16;

    if (upper_flag > 1) {
        fault = STATUS_ERR_INVALID_FLAG;
        goto result;
    }

    // ---- Update Register ----
    if (reg_read(base, offset, &reg_val) != 0) {
    	fault = STATUS_ERR_HW_FAILED;
        goto result;
    }

    user_value &= 0xFFFF;
    if (upper_flag) {
        reg_val = (reg_val & 0x0000FFFF) | (user_value << 16);
    } else {
        reg_val = (reg_val & 0xFFFF0000) | user_value;
    }
    reg_write(base, offset, reg_val);
    
    // ---- Read Back Verification ----
    reg_read(base, offset, &reg_val);
    read_16 = upper_flag ? ((reg_val >> 16) & 0xFFFF) : (reg_val & 0xFFFF);

    if (read_16 != user_value) 
    	fault = STATUS_ERR_MISMATCH;
    
    result:
    snprintf(buf, sizeof(buf), 
    	"{ \"status\": %d }", 
    	fault);
             
    return buf;
}

char* CANout_tcu1mc1_wrpara_reg10(uint32_t upper_flag, uint32_t user_value){

    static char buf[128];   
    status_code_t fault = STATUS_SUCCESS;

    uint32_t base =0xA4107000, offset =0x728, reg_val;
    uint16_t read_16;

    if (upper_flag > 1) {
        fault = STATUS_ERR_INVALID_FLAG;
        goto result;
    }

    // ---- Update Register ----
    if (reg_read(base, offset, &reg_val) != 0) {
    	fault = STATUS_ERR_HW_FAILED;
        goto result;
    }

    user_value &= 0xFFFF;
    if (upper_flag) {
        reg_val = (reg_val & 0x0000FFFF) | (user_value << 16);
    } else {
        reg_val = (reg_val & 0xFFFF0000) | user_value;
    }
    reg_write(base, offset, reg_val);
    
    // ---- Read Back Verification ----
    reg_read(base, offset, &reg_val);
    read_16 = upper_flag ? ((reg_val >> 16) & 0xFFFF) : (reg_val & 0xFFFF);

    if (read_16 != user_value) 
    	fault = STATUS_ERR_MISMATCH;
    
    result:
    snprintf(buf, sizeof(buf), 
    	"{ \"status\": %d }", 
    	fault);
             
    return buf;
}

char* CANout_tcu1mc1_wrpara_reg11(uint32_t upper_flag, uint32_t user_value){

    static char buf[128];   
    status_code_t fault = STATUS_SUCCESS;

    uint32_t base =0xA4107000, offset =0x72C, reg_val;
    uint16_t read_16;

    if (upper_flag > 1) {
        fault = STATUS_ERR_INVALID_FLAG;
        goto result;
    }

    // ---- Update Register ----
    if (reg_read(base, offset, &reg_val) != 0) {
    	fault = STATUS_ERR_HW_FAILED;
        goto result;
    }

    user_value &= 0xFFFF;
    if (upper_flag) {
        reg_val = (reg_val & 0x0000FFFF) | (user_value << 16);
    } else {
        reg_val = (reg_val & 0xFFFF0000) | user_value;
    }
    reg_write(base, offset, reg_val);
    
    // ---- Read Back Verification ----
    reg_read(base, offset, &reg_val);
    read_16 = upper_flag ? ((reg_val >> 16) & 0xFFFF) : (reg_val & 0xFFFF);

    if (read_16 != user_value) 
    	fault = STATUS_ERR_MISMATCH;
    
    result:
    snprintf(buf, sizeof(buf), 
    	"{ \"status\": %d }", 
    	fault);
             
    return buf;
}

char* CANout_tcu1mc1_wrpara_reg12(uint32_t upper_flag, uint32_t user_value){

    static char buf[128];   
    status_code_t fault = STATUS_SUCCESS;

    uint32_t base =0xA4107000, offset =0x730, reg_val;
    uint16_t read_16;

    if (upper_flag > 1) {
        fault = STATUS_ERR_INVALID_FLAG;
        goto result;
    }

    // ---- Update Register ----
    if (reg_read(base, offset, &reg_val) != 0) {
    	fault = STATUS_ERR_HW_FAILED;
        goto result;
    }

    user_value &= 0xFFFF;
    if (upper_flag) {
        reg_val = (reg_val & 0x0000FFFF) | (user_value << 16);
    } else {
        reg_val = (reg_val & 0xFFFF0000) | user_value;
    }
    reg_write(base, offset, reg_val);
    
    // ---- Read Back Verification ----
    reg_read(base, offset, &reg_val);
    read_16 = upper_flag ? ((reg_val >> 16) & 0xFFFF) : (reg_val & 0xFFFF);

    if (read_16 != user_value) 
    	fault = STATUS_ERR_MISMATCH;
    
    result:
    snprintf(buf, sizeof(buf), 
    	"{ \"status\": %d }", 
    	fault);
             
    return buf;
}

char* CANout_tcu1mc1_wrpara_reg13(uint32_t upper_flag, uint32_t user_value){

    static char buf[128];   
    status_code_t fault = STATUS_SUCCESS;

    uint32_t base =0xA4107000, offset =0x734, reg_val;
    uint16_t read_16;

    if (upper_flag > 1) {
        fault = STATUS_ERR_INVALID_FLAG;
        goto result;
    }

    // ---- Update Register ----
    if (reg_read(base, offset, &reg_val) != 0) {
    	fault = STATUS_ERR_HW_FAILED;
        goto result;
    }

    user_value &= 0xFFFF;
    if (upper_flag) {
        reg_val = (reg_val & 0x0000FFFF) | (user_value << 16);
    } else {
        reg_val = (reg_val & 0xFFFF0000) | user_value;
    }
    reg_write(base, offset, reg_val);
    
    // ---- Read Back Verification ----
    reg_read(base, offset, &reg_val);
    read_16 = upper_flag ? ((reg_val >> 16) & 0xFFFF) : (reg_val & 0xFFFF);

    if (read_16 != user_value) 
    	fault = STATUS_ERR_MISMATCH;
    
    result:
    snprintf(buf, sizeof(buf), 
    	"{ \"status\": %d }", 
    	fault);
             
    return buf;
}

char* CANout_tcu1mc1_wrpara_reg14(uint32_t upper_flag, uint32_t user_value){

    static char buf[128];   
    status_code_t fault = STATUS_SUCCESS;

    uint32_t base =0xA4107000, offset =0x738, reg_val;
    uint16_t read_16;

    if (upper_flag > 1) {
        fault = STATUS_ERR_INVALID_FLAG;
        goto result;
    }

    // ---- Update Register ----
    if (reg_read(base, offset, &reg_val) != 0) {
    	fault = STATUS_ERR_HW_FAILED;
        goto result;
    }

    user_value &= 0xFFFF;
    if (upper_flag) {
        reg_val = (reg_val & 0x0000FFFF) | (user_value << 16);
    } else {
        reg_val = (reg_val & 0xFFFF0000) | user_value;
    }
    reg_write(base, offset, reg_val);
    
    // ---- Read Back Verification ----
    reg_read(base, offset, &reg_val);
    read_16 = upper_flag ? ((reg_val >> 16) & 0xFFFF) : (reg_val & 0xFFFF);

    if (read_16 != user_value) 
    	fault = STATUS_ERR_MISMATCH;
    
    result:
    snprintf(buf, sizeof(buf), 
    	"{ \"status\": %d }", 
    	fault);
             
    return buf;
}

char* CANout_tcu1mc1_wrpara_reg15(uint32_t upper_flag, uint32_t user_value){

    static char buf[128];   
    status_code_t fault = STATUS_SUCCESS;

    uint32_t base =0xA4107000, offset =0x73C, reg_val;
    uint16_t read_16;

    if (upper_flag > 1) {
        fault = STATUS_ERR_INVALID_FLAG;
        goto result;
    }

    // ---- Update Register ----
    if (reg_read(base, offset, &reg_val) != 0) {
    	fault = STATUS_ERR_HW_FAILED;
        goto result;
    }

    user_value &= 0xFFFF;
    if (upper_flag) {
        reg_val = (reg_val & 0x0000FFFF) | (user_value << 16);
    } else {
        reg_val = (reg_val & 0xFFFF0000) | user_value;
    }
    reg_write(base, offset, reg_val);
    
    // ---- Read Back Verification ----
    reg_read(base, offset, &reg_val);
    read_16 = upper_flag ? ((reg_val >> 16) & 0xFFFF) : (reg_val & 0xFFFF);

    if (read_16 != user_value) 
    	fault = STATUS_ERR_MISMATCH;
    
    result:
    snprintf(buf, sizeof(buf), 
    	"{ \"status\": %d }", 
    	fault);
             
    return buf;
}

char* CANout_tcu1mc1_wrpara_reg16(uint32_t upper_flag, uint32_t user_value){

    static char buf[128];   
    status_code_t fault = STATUS_SUCCESS;

    uint32_t base =0xA4107000, offset =0x740, reg_val;
    uint16_t read_16;

    if (upper_flag > 1) {
        fault = STATUS_ERR_INVALID_FLAG;
        goto result;
    }

    // ---- Update Register ----
    if (reg_read(base, offset, &reg_val) != 0) {
    	fault = STATUS_ERR_HW_FAILED;
        goto result;
    }

    user_value &= 0xFFFF;
    if (upper_flag) {
        reg_val = (reg_val & 0x0000FFFF) | (user_value << 16);
    } else {
        reg_val = (reg_val & 0xFFFF0000) | user_value;
    }
    reg_write(base, offset, reg_val);
    
    // ---- Read Back Verification ----
    reg_read(base, offset, &reg_val);
    read_16 = upper_flag ? ((reg_val >> 16) & 0xFFFF) : (reg_val & 0xFFFF);

    if (read_16 != user_value) 
    	fault = STATUS_ERR_MISMATCH;
    
    result:
    snprintf(buf, sizeof(buf), 
    	"{ \"status\": %d }", 
    	fault);
             
    return buf;
}

char* CANout_tcu1mc1_wrpara_reg17(uint32_t upper_flag, uint32_t user_value){

    static char buf[128];   
    status_code_t fault = STATUS_SUCCESS;

    uint32_t base =0xA4107000, offset =0x744, reg_val;
    uint16_t read_16;

    if (upper_flag > 1) {
        fault = STATUS_ERR_INVALID_FLAG;
        goto result;
    }

    // ---- Update Register ----
    if (reg_read(base, offset, &reg_val) != 0) {
    	fault = STATUS_ERR_HW_FAILED;
        goto result;
    }

    user_value &= 0xFFFF;
    if (upper_flag) {
        reg_val = (reg_val & 0x0000FFFF) | (user_value << 16);
    } else {
        reg_val = (reg_val & 0xFFFF0000) | user_value;
    }
    reg_write(base, offset, reg_val);
    
    // ---- Read Back Verification ----
    reg_read(base, offset, &reg_val);
    read_16 = upper_flag ? ((reg_val >> 16) & 0xFFFF) : (reg_val & 0xFFFF);

    if (read_16 != user_value) 
    	fault = STATUS_ERR_MISMATCH;
    
    result:
    snprintf(buf, sizeof(buf), 
    	"{ \"status\": %d }", 
    	fault);
             
    return buf;
}

char* CANout_tcu1mc1_wrpara_reg18(uint32_t upper_flag, uint32_t user_value){

    static char buf[128];   
    status_code_t fault = STATUS_SUCCESS;

    uint32_t base =0xA4107000, offset =0x748, reg_val;
    uint16_t read_16;

    if (upper_flag > 1) {
        fault = STATUS_ERR_INVALID_FLAG;
        goto result;
    }

    // ---- Update Register ----
    if (reg_read(base, offset, &reg_val) != 0) {
    	fault = STATUS_ERR_HW_FAILED;
        goto result;
    }

    user_value &= 0xFFFF;
    if (upper_flag) {
        reg_val = (reg_val & 0x0000FFFF) | (user_value << 16);
    } else {
        reg_val = (reg_val & 0xFFFF0000) | user_value;
    }
    reg_write(base, offset, reg_val);
    
    // ---- Read Back Verification ----
    reg_read(base, offset, &reg_val);
    read_16 = upper_flag ? ((reg_val >> 16) & 0xFFFF) : (reg_val & 0xFFFF);

    if (read_16 != user_value) 
    	fault = STATUS_ERR_MISMATCH;
    
    result:
    snprintf(buf, sizeof(buf), 
    	"{ \"status\": %d }", 
    	fault);
             
    return buf;
}

char* CANout_tcu1mc1_wrpara_reg19(uint32_t upper_flag, uint32_t user_value){

    static char buf[128];   
    status_code_t fault = STATUS_SUCCESS;

    uint32_t base =0xA4107000, offset =0x74C, reg_val;
    uint16_t read_16;

    if (upper_flag > 1) {
        fault = STATUS_ERR_INVALID_FLAG;
        goto result;
    }

    // ---- Update Register ----
    if (reg_read(base, offset, &reg_val) != 0) {
    	fault = STATUS_ERR_HW_FAILED;
        goto result;
    }

    user_value &= 0xFFFF;
    if (upper_flag) {
        reg_val = (reg_val & 0x0000FFFF) | (user_value << 16);
    } else {
        reg_val = (reg_val & 0xFFFF0000) | user_value;
    }
    reg_write(base, offset, reg_val);
    
    // ---- Read Back Verification ----
    reg_read(base, offset, &reg_val);
    read_16 = upper_flag ? ((reg_val >> 16) & 0xFFFF) : (reg_val & 0xFFFF);

    if (read_16 != user_value) 
    	fault = STATUS_ERR_MISMATCH;
    
    result:
    snprintf(buf, sizeof(buf), 
    	"{ \"status\": %d }", 
    	fault);
             
    return buf;
}

char* CANout_tcu1mc1_wrpara_reg20(uint32_t upper_flag, uint32_t user_value){

    static char buf[128];   
    status_code_t fault = STATUS_SUCCESS;

    uint32_t base =0xA4107000, offset =0x750, reg_val;
    uint16_t read_16;

    if (upper_flag > 1) {
        fault = STATUS_ERR_INVALID_FLAG;
        goto result;
    }

    // ---- Update Register ----
    if (reg_read(base, offset, &reg_val) != 0) {
    	fault = STATUS_ERR_HW_FAILED;
        goto result;
    }

    user_value &= 0xFFFF;
    if (upper_flag) {
        reg_val = (reg_val & 0x0000FFFF) | (user_value << 16);
    } else {
        reg_val = (reg_val & 0xFFFF0000) | user_value;
    }
    reg_write(base, offset, reg_val);
    
    // ---- Read Back Verification ----
    reg_read(base, offset, &reg_val);
    read_16 = upper_flag ? ((reg_val >> 16) & 0xFFFF) : (reg_val & 0xFFFF);

    if (read_16 != user_value) 
    	fault = STATUS_ERR_MISMATCH;
    
    result:
    snprintf(buf, sizeof(buf), 
    	"{ \"status\": %d }", 
    	fault);
             
    return buf;
}

char* CANout_tcu1mc1_wrpara_reg21(uint32_t upper_flag, uint32_t user_value){

    static char buf[128];   
    status_code_t fault = STATUS_SUCCESS;

    uint32_t base =0xA4107000, offset =0x754, reg_val;
    uint16_t read_16;

    if (upper_flag > 1) {
        fault = STATUS_ERR_INVALID_FLAG;
        goto result;
    }

    // ---- Update Register ----
    if (reg_read(base, offset, &reg_val) != 0) {
    	fault = STATUS_ERR_HW_FAILED;
        goto result;
    }

    user_value &= 0xFFFF;
    if (upper_flag) {
        reg_val = (reg_val & 0x0000FFFF) | (user_value << 16);
    } else {
        reg_val = (reg_val & 0xFFFF0000) | user_value;
    }
    reg_write(base, offset, reg_val);
    
    // ---- Read Back Verification ----
    reg_read(base, offset, &reg_val);
    read_16 = upper_flag ? ((reg_val >> 16) & 0xFFFF) : (reg_val & 0xFFFF);

    if (read_16 != user_value) 
    	fault = STATUS_ERR_MISMATCH;
    
    result:
    snprintf(buf, sizeof(buf), 
    	"{ \"status\": %d }", 
    	fault);
             
    return buf;
}

char* CANout_tcu1mc1_wrpara_reg22(uint32_t upper_flag, uint32_t user_value){

    static char buf[128];   
    status_code_t fault = STATUS_SUCCESS;

    uint32_t base =0xA4107000, offset =0x758, reg_val;
    uint16_t read_16;

    if (upper_flag > 1) {
        fault = STATUS_ERR_INVALID_FLAG;
        goto result;
    }

    // ---- Update Register ----
    if (reg_read(base, offset, &reg_val) != 0) {
    	fault = STATUS_ERR_HW_FAILED;
        goto result;
    }

    user_value &= 0xFFFF;
    if (upper_flag) {
        reg_val = (reg_val & 0x0000FFFF) | (user_value << 16);
    } else {
        reg_val = (reg_val & 0xFFFF0000) | user_value;
    }
    reg_write(base, offset, reg_val);
    
    // ---- Read Back Verification ----
    reg_read(base, offset, &reg_val);
    read_16 = upper_flag ? ((reg_val >> 16) & 0xFFFF) : (reg_val & 0xFFFF);

    if (read_16 != user_value) 
    	fault = STATUS_ERR_MISMATCH;
    
    result:
    snprintf(buf, sizeof(buf), 
    	"{ \"status\": %d }", 
    	fault);
             
    return buf;
}

char* CANout_tcu1mc1_wrpara_reg23(uint32_t upper_flag, uint32_t user_value){

    static char buf[128];   
    status_code_t fault = STATUS_SUCCESS;

    uint32_t base =0xA4107000, offset =0x75C, reg_val;
    uint16_t read_16;

    if (upper_flag > 1) {
        fault = STATUS_ERR_INVALID_FLAG;
        goto result;
    }

    // ---- Update Register ----
    if (reg_read(base, offset, &reg_val) != 0) {
    	fault = STATUS_ERR_HW_FAILED;
        goto result;
    }

    user_value &= 0xFFFF;
    if (upper_flag) {
        reg_val = (reg_val & 0x0000FFFF) | (user_value << 16);
    } else {
        reg_val = (reg_val & 0xFFFF0000) | user_value;
    }
    reg_write(base, offset, reg_val);
    
    // ---- Read Back Verification ----
    reg_read(base, offset, &reg_val);
    read_16 = upper_flag ? ((reg_val >> 16) & 0xFFFF) : (reg_val & 0xFFFF);

    if (read_16 != user_value) 
    	fault = STATUS_ERR_MISMATCH;
    
    result:
    snprintf(buf, sizeof(buf), 
    	"{ \"status\": %d }", 
    	fault);
             
    return buf;
}

char* CANout_tcu1mc1_wrpara_reg24(uint32_t upper_flag, uint32_t user_value){

    static char buf[128];   
    status_code_t fault = STATUS_SUCCESS;

    uint32_t base =0xA4107000, offset =0x760, reg_val;
    uint16_t read_16;

    if (upper_flag > 1) {
        fault = STATUS_ERR_INVALID_FLAG;
        goto result;
    }

    // ---- Update Register ----
    if (reg_read(base, offset, &reg_val) != 0) {
    	fault = STATUS_ERR_HW_FAILED;
        goto result;
    }

    user_value &= 0xFFFF;
    if (upper_flag) {
        reg_val = (reg_val & 0x0000FFFF) | (user_value << 16);
    } else {
        reg_val = (reg_val & 0xFFFF0000) | user_value;
    }
    reg_write(base, offset, reg_val);
    
    // ---- Read Back Verification ----
    reg_read(base, offset, &reg_val);
    read_16 = upper_flag ? ((reg_val >> 16) & 0xFFFF) : (reg_val & 0xFFFF);

    if (read_16 != user_value) 
    	fault = STATUS_ERR_MISMATCH;
    
    result:
    snprintf(buf, sizeof(buf), 
    	"{ \"status\": %d }", 
    	fault);
             
    return buf;
}

char* CANout_tcu1mc1_wrpara_reg25(uint32_t upper_flag, uint32_t user_value){

    static char buf[128];   
    status_code_t fault = STATUS_SUCCESS;

    uint32_t base =0xA4107000, offset =0x764, reg_val;
    uint16_t read_16;

    if (upper_flag > 1) {
        fault = STATUS_ERR_INVALID_FLAG;
        goto result;
    }

    // ---- Update Register ----
    if (reg_read(base, offset, &reg_val) != 0) {
    	fault = STATUS_ERR_HW_FAILED;
        goto result;
    }

    user_value &= 0xFFFF;
    if (upper_flag) {
        reg_val = (reg_val & 0x0000FFFF) | (user_value << 16);
    } else {
        reg_val = (reg_val & 0xFFFF0000) | user_value;
    }
    reg_write(base, offset, reg_val);
    
    // ---- Read Back Verification ----
    reg_read(base, offset, &reg_val);
    read_16 = upper_flag ? ((reg_val >> 16) & 0xFFFF) : (reg_val & 0xFFFF);

    if (read_16 != user_value) 
    	fault = STATUS_ERR_MISMATCH;
    
    result:
    snprintf(buf, sizeof(buf), 
    	"{ \"status\": %d }", 
    	fault);
             
    return buf;
}

char* CANout_tcu1mc2_wrpara_reg0(uint32_t upper_flag, uint32_t user_value){

    static char buf[128];   
    status_code_t fault = STATUS_SUCCESS;

    uint32_t base =0xA4107000, offset =0xA00, reg_val;
    uint16_t read_16;

    if (upper_flag > 1) {
        fault = STATUS_ERR_INVALID_FLAG;
        goto result;
    }

    // ---- Update Register ----
    if (reg_read(base, offset, &reg_val) != 0) {
    	fault = STATUS_ERR_HW_FAILED;
        goto result;
    }

    user_value &= 0xFFFF;
    if (upper_flag) {
        reg_val = (reg_val & 0x0000FFFF) | (user_value << 16);
    } else {
        reg_val = (reg_val & 0xFFFF0000) | user_value;
    }
    reg_write(base, offset, reg_val);
    
    // ---- Read Back Verification ----
    reg_read(base, offset, &reg_val);
    read_16 = upper_flag ? ((reg_val >> 16) & 0xFFFF) : (reg_val & 0xFFFF);

    if (read_16 != user_value) 
    	fault = STATUS_ERR_MISMATCH;
    
    result:
    snprintf(buf, sizeof(buf), 
    	"{ \"status\": %d }", 
    	fault);
             
    return buf;
}

char* CANout_tcu1mc2_wrpara_reg1(uint32_t upper_flag, uint32_t user_value){

    static char buf[128];   
    status_code_t fault = STATUS_SUCCESS;

    uint32_t base =0xA4107000, offset =0xA04, reg_val;
    uint16_t read_16;

    if (upper_flag > 1) {
        fault = STATUS_ERR_INVALID_FLAG;
        goto result;
    }

    // ---- Update Register ----
    if (reg_read(base, offset, &reg_val) != 0) {
    	fault = STATUS_ERR_HW_FAILED;
        goto result;
    }

    user_value &= 0xFFFF;
    if (upper_flag) {
        reg_val = (reg_val & 0x0000FFFF) | (user_value << 16);
    } else {
        reg_val = (reg_val & 0xFFFF0000) | user_value;
    }
    reg_write(base, offset, reg_val);
    
    // ---- Read Back Verification ----
    reg_read(base, offset, &reg_val);
    read_16 = upper_flag ? ((reg_val >> 16) & 0xFFFF) : (reg_val & 0xFFFF);

    if (read_16 != user_value) 
    	fault = STATUS_ERR_MISMATCH;
    
    result:
    snprintf(buf, sizeof(buf), 
    	"{ \"status\": %d }", 
    	fault);
             
    return buf;
}

char* CANout_tcu1mc2_wrpara_reg2(uint32_t upper_flag, uint32_t user_value){

    static char buf[128];   
    status_code_t fault = STATUS_SUCCESS;

    uint32_t base =0xA4107000, offset =0xA08, reg_val;
    uint16_t read_16;

    if (upper_flag > 1) {
        fault = STATUS_ERR_INVALID_FLAG;
        goto result;
    }

    // ---- Update Register ----
    if (reg_read(base, offset, &reg_val) != 0) {
    	fault = STATUS_ERR_HW_FAILED;
        goto result;
    }

    user_value &= 0xFFFF;
    if (upper_flag) {
        reg_val = (reg_val & 0x0000FFFF) | (user_value << 16);
    } else {
        reg_val = (reg_val & 0xFFFF0000) | user_value;
    }
    reg_write(base, offset, reg_val);
    
    // ---- Read Back Verification ----
    reg_read(base, offset, &reg_val);
    read_16 = upper_flag ? ((reg_val >> 16) & 0xFFFF) : (reg_val & 0xFFFF);

    if (read_16 != user_value) 
    	fault = STATUS_ERR_MISMATCH;
    
    result:
    snprintf(buf, sizeof(buf), 
    	"{ \"status\": %d }", 
    	fault);
             
    return buf;
}

char* CANout_tcu1mc2_wrpara_reg3(uint32_t upper_flag, uint32_t user_value){

    static char buf[128];   
    status_code_t fault = STATUS_SUCCESS;

    uint32_t base =0xA4107000, offset =0xA0C, reg_val;
    uint16_t read_16;

    if (upper_flag > 1) {
        fault = STATUS_ERR_INVALID_FLAG;
        goto result;
    }

    // ---- Update Register ----
    if (reg_read(base, offset, &reg_val) != 0) {
    	fault = STATUS_ERR_HW_FAILED;
        goto result;
    }

    user_value &= 0xFFFF;
    if (upper_flag) {
        reg_val = (reg_val & 0x0000FFFF) | (user_value << 16);
    } else {
        reg_val = (reg_val & 0xFFFF0000) | user_value;
    }
    reg_write(base, offset, reg_val);
    
    // ---- Read Back Verification ----
    reg_read(base, offset, &reg_val);
    read_16 = upper_flag ? ((reg_val >> 16) & 0xFFFF) : (reg_val & 0xFFFF);

    if (read_16 != user_value) 
    	fault = STATUS_ERR_MISMATCH;
    
    result:
    snprintf(buf, sizeof(buf), 
    	"{ \"status\": %d }", 
    	fault);
             
    return buf;
}

char* CANout_tcu1mc2_wrpara_reg4(uint32_t upper_flag, uint32_t user_value){

    static char buf[128];   
    status_code_t fault = STATUS_SUCCESS;

    uint32_t base =0xA4107000, offset =0xA10, reg_val;
    uint16_t read_16;

    if (upper_flag > 1) {
        fault = STATUS_ERR_INVALID_FLAG;
        goto result;
    }

    // ---- Update Register ----
    if (reg_read(base, offset, &reg_val) != 0) {
    	fault = STATUS_ERR_HW_FAILED;
        goto result;
    }

    user_value &= 0xFFFF;
    if (upper_flag) {
        reg_val = (reg_val & 0x0000FFFF) | (user_value << 16);
    } else {
        reg_val = (reg_val & 0xFFFF0000) | user_value;
    }
    reg_write(base, offset, reg_val);
    
    // ---- Read Back Verification ----
    reg_read(base, offset, &reg_val);
    read_16 = upper_flag ? ((reg_val >> 16) & 0xFFFF) : (reg_val & 0xFFFF);

    if (read_16 != user_value) 
    	fault = STATUS_ERR_MISMATCH;
    
    result:
    snprintf(buf, sizeof(buf), 
    	"{ \"status\": %d }", 
    	fault);
             
    return buf;
}

char* CANout_tcu1mc2_wrpara_reg5(uint32_t upper_flag, uint32_t user_value){

    static char buf[128];   
    status_code_t fault = STATUS_SUCCESS;

    uint32_t base =0xA4107000, offset =0xA14, reg_val;
    uint16_t read_16;

    if (upper_flag > 1) {
        fault = STATUS_ERR_INVALID_FLAG;
        goto result;
    }

    // ---- Update Register ----
    if (reg_read(base, offset, &reg_val) != 0) {
    	fault = STATUS_ERR_HW_FAILED;
        goto result;
    }

    user_value &= 0xFFFF;
    if (upper_flag) {
        reg_val = (reg_val & 0x0000FFFF) | (user_value << 16);
    } else {
        reg_val = (reg_val & 0xFFFF0000) | user_value;
    }
    reg_write(base, offset, reg_val);
    
    // ---- Read Back Verification ----
    reg_read(base, offset, &reg_val);
    read_16 = upper_flag ? ((reg_val >> 16) & 0xFFFF) : (reg_val & 0xFFFF);

    if (read_16 != user_value) 
    	fault = STATUS_ERR_MISMATCH;
    
    result:
    snprintf(buf, sizeof(buf), 
    	"{ \"status\": %d }", 
    	fault);
             
    return buf;
}

char* CANout_tcu1mc2_wrpara_reg6(uint32_t upper_flag, uint32_t user_value){

    static char buf[128];   
    status_code_t fault = STATUS_SUCCESS;

    uint32_t base =0xA4107000, offset =0xA18, reg_val;
    uint16_t read_16;

    if (upper_flag > 1) {
        fault = STATUS_ERR_INVALID_FLAG;
        goto result;
    }

    // ---- Update Register ----
    if (reg_read(base, offset, &reg_val) != 0) {
    	fault = STATUS_ERR_HW_FAILED;
        goto result;
    }

    user_value &= 0xFFFF;
    if (upper_flag) {
        reg_val = (reg_val & 0x0000FFFF) | (user_value << 16);
    } else {
        reg_val = (reg_val & 0xFFFF0000) | user_value;
    }
    reg_write(base, offset, reg_val);
    
    // ---- Read Back Verification ----
    reg_read(base, offset, &reg_val);
    read_16 = upper_flag ? ((reg_val >> 16) & 0xFFFF) : (reg_val & 0xFFFF);

    if (read_16 != user_value) 
    	fault = STATUS_ERR_MISMATCH;
    
    result:
    snprintf(buf, sizeof(buf), 
    	"{ \"status\": %d }", 
    	fault);
             
    return buf;
}

char* CANout_tcu1mc2_wrpara_reg7(uint32_t upper_flag, uint32_t user_value){

    static char buf[128];   
    status_code_t fault = STATUS_SUCCESS;

    uint32_t base =0xA4107000, offset =0xA1C, reg_val;
    uint16_t read_16;

    if (upper_flag > 1) {
        fault = STATUS_ERR_INVALID_FLAG;
        goto result;
    }

    // ---- Update Register ----
    if (reg_read(base, offset, &reg_val) != 0) {
    	fault = STATUS_ERR_HW_FAILED;
        goto result;
    }

    user_value &= 0xFFFF;
    if (upper_flag) {
        reg_val = (reg_val & 0x0000FFFF) | (user_value << 16);
    } else {
        reg_val = (reg_val & 0xFFFF0000) | user_value;
    }
    reg_write(base, offset, reg_val);
    
    // ---- Read Back Verification ----
    reg_read(base, offset, &reg_val);
    read_16 = upper_flag ? ((reg_val >> 16) & 0xFFFF) : (reg_val & 0xFFFF);

    if (read_16 != user_value) 
    	fault = STATUS_ERR_MISMATCH;
    
    result:
    snprintf(buf, sizeof(buf), 
    	"{ \"status\": %d }", 
    	fault);
             
    return buf;
}

char* CANout_tcu1mc2_wrpara_reg8(uint32_t upper_flag, uint32_t user_value){

    static char buf[128];   
    status_code_t fault = STATUS_SUCCESS;

    uint32_t base =0xA4107000, offset =0xA20, reg_val;
    uint16_t read_16;

    if (upper_flag > 1) {
        fault = STATUS_ERR_INVALID_FLAG;
        goto result;
    }

    // ---- Update Register ----
    if (reg_read(base, offset, &reg_val) != 0) {
    	fault = STATUS_ERR_HW_FAILED;
        goto result;
    }

    user_value &= 0xFFFF;
    if (upper_flag) {
        reg_val = (reg_val & 0x0000FFFF) | (user_value << 16);
    } else {
        reg_val = (reg_val & 0xFFFF0000) | user_value;
    }
    reg_write(base, offset, reg_val);
    
    // ---- Read Back Verification ----
    reg_read(base, offset, &reg_val);
    read_16 = upper_flag ? ((reg_val >> 16) & 0xFFFF) : (reg_val & 0xFFFF);

    if (read_16 != user_value) 
    	fault = STATUS_ERR_MISMATCH;
    
    result:
    snprintf(buf, sizeof(buf), 
    	"{ \"status\": %d }", 
    	fault);
             
    return buf;
}

char* CANout_tcu1mc2_wrpara_reg9(uint32_t upper_flag, uint32_t user_value){

    static char buf[128];   
    status_code_t fault = STATUS_SUCCESS;

    uint32_t base =0xA4107000, offset =0xA24, reg_val;
    uint16_t read_16;

    if (upper_flag > 1) {
        fault = STATUS_ERR_INVALID_FLAG;
        goto result;
    }

    // ---- Update Register ----
    if (reg_read(base, offset, &reg_val) != 0) {
    	fault = STATUS_ERR_HW_FAILED;
        goto result;
    }

    user_value &= 0xFFFF;
    if (upper_flag) {
        reg_val = (reg_val & 0x0000FFFF) | (user_value << 16);
    } else {
        reg_val = (reg_val & 0xFFFF0000) | user_value;
    }
    reg_write(base, offset, reg_val);
    
    // ---- Read Back Verification ----
    reg_read(base, offset, &reg_val);
    read_16 = upper_flag ? ((reg_val >> 16) & 0xFFFF) : (reg_val & 0xFFFF);

    if (read_16 != user_value) 
    	fault = STATUS_ERR_MISMATCH;
    
    result:
    snprintf(buf, sizeof(buf), 
    	"{ \"status\": %d }", 
    	fault);
             
    return buf;
}

char* CANout_tcu1mc2_wrpara_reg10(uint32_t upper_flag, uint32_t user_value){

    static char buf[128];   
    status_code_t fault = STATUS_SUCCESS;

    uint32_t base =0xA4107000, offset =0xA28, reg_val;
    uint16_t read_16;

    if (upper_flag > 1) {
        fault = STATUS_ERR_INVALID_FLAG;
        goto result;
    }

    // ---- Update Register ----
    if (reg_read(base, offset, &reg_val) != 0) {
    	fault = STATUS_ERR_HW_FAILED;
        goto result;
    }

    user_value &= 0xFFFF;
    if (upper_flag) {
        reg_val = (reg_val & 0x0000FFFF) | (user_value << 16);
    } else {
        reg_val = (reg_val & 0xFFFF0000) | user_value;
    }
    reg_write(base, offset, reg_val);
    
    // ---- Read Back Verification ----
    reg_read(base, offset, &reg_val);
    read_16 = upper_flag ? ((reg_val >> 16) & 0xFFFF) : (reg_val & 0xFFFF);

    if (read_16 != user_value) 
    	fault = STATUS_ERR_MISMATCH;
    
    result:
    snprintf(buf, sizeof(buf), 
    	"{ \"status\": %d }", 
    	fault);
             
    return buf;
}

char* CANout_tcu1mc2_wrpara_reg11(uint32_t upper_flag, uint32_t user_value){

    static char buf[128];   
    status_code_t fault = STATUS_SUCCESS;

    uint32_t base =0xA4107000, offset =0xA2C, reg_val;
    uint16_t read_16;

    if (upper_flag > 1) {
        fault = STATUS_ERR_INVALID_FLAG;
        goto result;
    }

    // ---- Update Register ----
    if (reg_read(base, offset, &reg_val) != 0) {
    	fault = STATUS_ERR_HW_FAILED;
        goto result;
    }

    user_value &= 0xFFFF;
    if (upper_flag) {
        reg_val = (reg_val & 0x0000FFFF) | (user_value << 16);
    } else {
        reg_val = (reg_val & 0xFFFF0000) | user_value;
    }
    reg_write(base, offset, reg_val);
    
    // ---- Read Back Verification ----
    reg_read(base, offset, &reg_val);
    read_16 = upper_flag ? ((reg_val >> 16) & 0xFFFF) : (reg_val & 0xFFFF);

    if (read_16 != user_value) 
    	fault = STATUS_ERR_MISMATCH;
    
    result:
    snprintf(buf, sizeof(buf), 
    	"{ \"status\": %d }", 
    	fault);
             
    return buf;
}

char* CANout_tcu1mc2_wrpara_reg12(uint32_t upper_flag, uint32_t user_value){

    static char buf[128];   
    status_code_t fault = STATUS_SUCCESS;

    uint32_t base =0xA4107000, offset =0xA30, reg_val;
    uint16_t read_16;

    if (upper_flag > 1) {
        fault = STATUS_ERR_INVALID_FLAG;
        goto result;
    }

    // ---- Update Register ----
    if (reg_read(base, offset, &reg_val) != 0) {
    	fault = STATUS_ERR_HW_FAILED;
        goto result;
    }

    user_value &= 0xFFFF;
    if (upper_flag) {
        reg_val = (reg_val & 0x0000FFFF) | (user_value << 16);
    } else {
        reg_val = (reg_val & 0xFFFF0000) | user_value;
    }
    reg_write(base, offset, reg_val);
    
    // ---- Read Back Verification ----
    reg_read(base, offset, &reg_val);
    read_16 = upper_flag ? ((reg_val >> 16) & 0xFFFF) : (reg_val & 0xFFFF);

    if (read_16 != user_value) 
    	fault = STATUS_ERR_MISMATCH;
    
    result:
    snprintf(buf, sizeof(buf), 
    	"{ \"status\": %d }", 
    	fault);
             
    return buf;
}

char* CANout_tcu1mc2_wrpara_reg13(uint32_t upper_flag, uint32_t user_value){

    static char buf[128];   
    status_code_t fault = STATUS_SUCCESS;

    uint32_t base =0xA4107000, offset =0xA34, reg_val;
    uint16_t read_16;

    if (upper_flag > 1) {
        fault = STATUS_ERR_INVALID_FLAG;
        goto result;
    }

    // ---- Update Register ----
    if (reg_read(base, offset, &reg_val) != 0) {
    	fault = STATUS_ERR_HW_FAILED;
        goto result;
    }

    user_value &= 0xFFFF;
    if (upper_flag) {
        reg_val = (reg_val & 0x0000FFFF) | (user_value << 16);
    } else {
        reg_val = (reg_val & 0xFFFF0000) | user_value;
    }
    reg_write(base, offset, reg_val);
    
    // ---- Read Back Verification ----
    reg_read(base, offset, &reg_val);
    read_16 = upper_flag ? ((reg_val >> 16) & 0xFFFF) : (reg_val & 0xFFFF);

    if (read_16 != user_value) 
    	fault = STATUS_ERR_MISMATCH;
    
    result:
    snprintf(buf, sizeof(buf), 
    	"{ \"status\": %d }", 
    	fault);
             
    return buf;
}

char* CANout_tcu1mc2_wrpara_reg14(uint32_t upper_flag, uint32_t user_value){

    static char buf[128];   
    status_code_t fault = STATUS_SUCCESS;

    uint32_t base =0xA4107000, offset =0xA38, reg_val;
    uint16_t read_16;

    if (upper_flag > 1) {
        fault = STATUS_ERR_INVALID_FLAG;
        goto result;
    }

    // ---- Update Register ----
    if (reg_read(base, offset, &reg_val) != 0) {
    	fault = STATUS_ERR_HW_FAILED;
        goto result;
    }

    user_value &= 0xFFFF;
    if (upper_flag) {
        reg_val = (reg_val & 0x0000FFFF) | (user_value << 16);
    } else {
        reg_val = (reg_val & 0xFFFF0000) | user_value;
    }
    reg_write(base, offset, reg_val);
    
    // ---- Read Back Verification ----
    reg_read(base, offset, &reg_val);
    read_16 = upper_flag ? ((reg_val >> 16) & 0xFFFF) : (reg_val & 0xFFFF);

    if (read_16 != user_value) 
    	fault = STATUS_ERR_MISMATCH;
    
    result:
    snprintf(buf, sizeof(buf), 
    	"{ \"status\": %d }", 
    	fault);
             
    return buf;
}

char* CANout_tcu1mc2_wrpara_reg15(uint32_t upper_flag, uint32_t user_value){

    static char buf[128];   
    status_code_t fault = STATUS_SUCCESS;

    uint32_t base =0xA4107000, offset =0xA3C, reg_val;
    uint16_t read_16;

    if (upper_flag > 1) {
        fault = STATUS_ERR_INVALID_FLAG;
        goto result;
    }

    // ---- Update Register ----
    if (reg_read(base, offset, &reg_val) != 0) {
    	fault = STATUS_ERR_HW_FAILED;
        goto result;
    }

    user_value &= 0xFFFF;
    if (upper_flag) {
        reg_val = (reg_val & 0x0000FFFF) | (user_value << 16);
    } else {
        reg_val = (reg_val & 0xFFFF0000) | user_value;
    }
    reg_write(base, offset, reg_val);
    
    // ---- Read Back Verification ----
    reg_read(base, offset, &reg_val);
    read_16 = upper_flag ? ((reg_val >> 16) & 0xFFFF) : (reg_val & 0xFFFF);

    if (read_16 != user_value) 
    	fault = STATUS_ERR_MISMATCH;
    
    result:
    snprintf(buf, sizeof(buf), 
    	"{ \"status\": %d }", 
    	fault);
             
    return buf;
}

char* CANout_tcu1mc2_wrpara_reg16(uint32_t upper_flag, uint32_t user_value){

    static char buf[128];   
    status_code_t fault = STATUS_SUCCESS;

    uint32_t base =0xA4107000, offset =0xA40, reg_val;
    uint16_t read_16;

    if (upper_flag > 1) {
        fault = STATUS_ERR_INVALID_FLAG;
        goto result;
    }

    // ---- Update Register ----
    if (reg_read(base, offset, &reg_val) != 0) {
    	fault = STATUS_ERR_HW_FAILED;
        goto result;
    }

    user_value &= 0xFFFF;
    if (upper_flag) {
        reg_val = (reg_val & 0x0000FFFF) | (user_value << 16);
    } else {
        reg_val = (reg_val & 0xFFFF0000) | user_value;
    }
    reg_write(base, offset, reg_val);
    
    // ---- Read Back Verification ----
    reg_read(base, offset, &reg_val);
    read_16 = upper_flag ? ((reg_val >> 16) & 0xFFFF) : (reg_val & 0xFFFF);

    if (read_16 != user_value) 
    	fault = STATUS_ERR_MISMATCH;
    
    result:
    snprintf(buf, sizeof(buf), 
    	"{ \"status\": %d }", 
    	fault);
             
    return buf;
}

char* CANout_tcu1mc2_wrpara_reg17(uint32_t upper_flag, uint32_t user_value){

    static char buf[128];   
    status_code_t fault = STATUS_SUCCESS;

    uint32_t base =0xA4107000, offset =0xA44, reg_val;
    uint16_t read_16;

    if (upper_flag > 1) {
        fault = STATUS_ERR_INVALID_FLAG;
        goto result;
    }

    // ---- Update Register ----
    if (reg_read(base, offset, &reg_val) != 0) {
    	fault = STATUS_ERR_HW_FAILED;
        goto result;
    }

    user_value &= 0xFFFF;
    if (upper_flag) {
        reg_val = (reg_val & 0x0000FFFF) | (user_value << 16);
    } else {
        reg_val = (reg_val & 0xFFFF0000) | user_value;
    }
    reg_write(base, offset, reg_val);
    
    // ---- Read Back Verification ----
    reg_read(base, offset, &reg_val);
    read_16 = upper_flag ? ((reg_val >> 16) & 0xFFFF) : (reg_val & 0xFFFF);

    if (read_16 != user_value) 
    	fault = STATUS_ERR_MISMATCH;
    
    result:
    snprintf(buf, sizeof(buf), 
    	"{ \"status\": %d }", 
    	fault);
             
    return buf;
}

char* CANout_tcu1mc2_wrpara_reg18(uint32_t upper_flag, uint32_t user_value){

    static char buf[128];   
    status_code_t fault = STATUS_SUCCESS;

    uint32_t base =0xA4107000, offset =0xA48, reg_val;
    uint16_t read_16;

    if (upper_flag > 1) {
        fault = STATUS_ERR_INVALID_FLAG;
        goto result;
    }

    // ---- Update Register ----
    if (reg_read(base, offset, &reg_val) != 0) {
    	fault = STATUS_ERR_HW_FAILED;
        goto result;
    }

    user_value &= 0xFFFF;
    if (upper_flag) {
        reg_val = (reg_val & 0x0000FFFF) | (user_value << 16);
    } else {
        reg_val = (reg_val & 0xFFFF0000) | user_value;
    }
    reg_write(base, offset, reg_val);
    
    // ---- Read Back Verification ----
    reg_read(base, offset, &reg_val);
    read_16 = upper_flag ? ((reg_val >> 16) & 0xFFFF) : (reg_val & 0xFFFF);

    if (read_16 != user_value) 
    	fault = STATUS_ERR_MISMATCH;
    
    result:
    snprintf(buf, sizeof(buf), 
    	"{ \"status\": %d }", 
    	fault);
             
    return buf;
}

char* CANout_tcu1mc2_wrpara_reg19(uint32_t upper_flag, uint32_t user_value){

    static char buf[128];   
    status_code_t fault = STATUS_SUCCESS;

    uint32_t base =0xA4107000, offset =0xA4C, reg_val;
    uint16_t read_16;

    if (upper_flag > 1) {
        fault = STATUS_ERR_INVALID_FLAG;
        goto result;
    }

    // ---- Update Register ----
    if (reg_read(base, offset, &reg_val) != 0) {
    	fault = STATUS_ERR_HW_FAILED;
        goto result;
    }

    user_value &= 0xFFFF;
    if (upper_flag) {
        reg_val = (reg_val & 0x0000FFFF) | (user_value << 16);
    } else {
        reg_val = (reg_val & 0xFFFF0000) | user_value;
    }
    reg_write(base, offset, reg_val);
    
    // ---- Read Back Verification ----
    reg_read(base, offset, &reg_val);
    read_16 = upper_flag ? ((reg_val >> 16) & 0xFFFF) : (reg_val & 0xFFFF);

    if (read_16 != user_value) 
    	fault = STATUS_ERR_MISMATCH;
    
    result:
    snprintf(buf, sizeof(buf), 
    	"{ \"status\": %d }", 
    	fault);
             
    return buf;
}

char* CANout_tcu1mc2_wrpara_reg20(uint32_t upper_flag, uint32_t user_value){

    static char buf[128];   
    status_code_t fault = STATUS_SUCCESS;

    uint32_t base =0xA4107000, offset =0xA50, reg_val;
    uint16_t read_16;

    if (upper_flag > 1) {
        fault = STATUS_ERR_INVALID_FLAG;
        goto result;
    }

    // ---- Update Register ----
    if (reg_read(base, offset, &reg_val) != 0) {
    	fault = STATUS_ERR_HW_FAILED;
        goto result;
    }

    user_value &= 0xFFFF;
    if (upper_flag) {
        reg_val = (reg_val & 0x0000FFFF) | (user_value << 16);
    } else {
        reg_val = (reg_val & 0xFFFF0000) | user_value;
    }
    reg_write(base, offset, reg_val);
    
    // ---- Read Back Verification ----
    reg_read(base, offset, &reg_val);
    read_16 = upper_flag ? ((reg_val >> 16) & 0xFFFF) : (reg_val & 0xFFFF);

    if (read_16 != user_value) 
    	fault = STATUS_ERR_MISMATCH;
    
    result:
    snprintf(buf, sizeof(buf), 
    	"{ \"status\": %d }", 
    	fault);
             
    return buf;
}

char* CANout_tcu1mc2_wrpara_reg21(uint32_t upper_flag, uint32_t user_value){

    static char buf[128];   
    status_code_t fault = STATUS_SUCCESS;

    uint32_t base =0xA4107000, offset =0xA54, reg_val;
    uint16_t read_16;

    if (upper_flag > 1) {
        fault = STATUS_ERR_INVALID_FLAG;
        goto result;
    }

    // ---- Update Register ----
    if (reg_read(base, offset, &reg_val) != 0) {
    	fault = STATUS_ERR_HW_FAILED;
        goto result;
    }

    user_value &= 0xFFFF;
    if (upper_flag) {
        reg_val = (reg_val & 0x0000FFFF) | (user_value << 16);
    } else {
        reg_val = (reg_val & 0xFFFF0000) | user_value;
    }
    reg_write(base, offset, reg_val);
    
    // ---- Read Back Verification ----
    reg_read(base, offset, &reg_val);
    read_16 = upper_flag ? ((reg_val >> 16) & 0xFFFF) : (reg_val & 0xFFFF);

    if (read_16 != user_value) 
    	fault = STATUS_ERR_MISMATCH;
    
    result:
    snprintf(buf, sizeof(buf), 
    	"{ \"status\": %d }", 
    	fault);
             
    return buf;
}

char* CANout_tcu1mc2_wrpara_reg22(uint32_t upper_flag, uint32_t user_value){

    static char buf[128];   
    status_code_t fault = STATUS_SUCCESS;

    uint32_t base =0xA4107000, offset =0xA58, reg_val;
    uint16_t read_16;

    if (upper_flag > 1) {
        fault = STATUS_ERR_INVALID_FLAG;
        goto result;
    }

    // ---- Update Register ----
    if (reg_read(base, offset, &reg_val) != 0) {
    	fault = STATUS_ERR_HW_FAILED;
        goto result;
    }

    user_value &= 0xFFFF;
    if (upper_flag) {
        reg_val = (reg_val & 0x0000FFFF) | (user_value << 16);
    } else {
        reg_val = (reg_val & 0xFFFF0000) | user_value;
    }
    reg_write(base, offset, reg_val);
    
    // ---- Read Back Verification ----
    reg_read(base, offset, &reg_val);
    read_16 = upper_flag ? ((reg_val >> 16) & 0xFFFF) : (reg_val & 0xFFFF);

    if (read_16 != user_value) 
    	fault = STATUS_ERR_MISMATCH;
    
    result:
    snprintf(buf, sizeof(buf), 
    	"{ \"status\": %d }", 
    	fault);
             
    return buf;
}

char* CANout_tcu1mc2_wrpara_reg23(uint32_t upper_flag, uint32_t user_value){

    static char buf[128];   
    status_code_t fault = STATUS_SUCCESS;

    uint32_t base =0xA4107000, offset =0xA5C, reg_val;
    uint16_t read_16;

    if (upper_flag > 1) {
        fault = STATUS_ERR_INVALID_FLAG;
        goto result;
    }

    // ---- Update Register ----
    if (reg_read(base, offset, &reg_val) != 0) {
    	fault = STATUS_ERR_HW_FAILED;
        goto result;
    }

    user_value &= 0xFFFF;
    if (upper_flag) {
        reg_val = (reg_val & 0x0000FFFF) | (user_value << 16);
    } else {
        reg_val = (reg_val & 0xFFFF0000) | user_value;
    }
    reg_write(base, offset, reg_val);
    
    // ---- Read Back Verification ----
    reg_read(base, offset, &reg_val);
    read_16 = upper_flag ? ((reg_val >> 16) & 0xFFFF) : (reg_val & 0xFFFF);

    if (read_16 != user_value) 
    	fault = STATUS_ERR_MISMATCH;
    
    result:
    snprintf(buf, sizeof(buf), 
    	"{ \"status\": %d }", 
    	fault);
             
    return buf;
}

char* CANout_tcu1mc2_wrpara_reg24(uint32_t upper_flag, uint32_t user_value){

    static char buf[128];   
    status_code_t fault = STATUS_SUCCESS;

    uint32_t base =0xA4107000, offset =0xA60, reg_val;
    uint16_t read_16;

    if (upper_flag > 1) {
        fault = STATUS_ERR_INVALID_FLAG;
        goto result;
    }

    // ---- Update Register ----
    if (reg_read(base, offset, &reg_val) != 0) {
    	fault = STATUS_ERR_HW_FAILED;
        goto result;
    }

    user_value &= 0xFFFF;
    if (upper_flag) {
        reg_val = (reg_val & 0x0000FFFF) | (user_value << 16);
    } else {
        reg_val = (reg_val & 0xFFFF0000) | user_value;
    }
    reg_write(base, offset, reg_val);
    
    // ---- Read Back Verification ----
    reg_read(base, offset, &reg_val);
    read_16 = upper_flag ? ((reg_val >> 16) & 0xFFFF) : (reg_val & 0xFFFF);

    if (read_16 != user_value) 
    	fault = STATUS_ERR_MISMATCH;
    
    result:
    snprintf(buf, sizeof(buf), 
    	"{ \"status\": %d }", 
    	fault);
             
    return buf;
}

char* CANout_tcu1mc2_wrpara_reg25(uint32_t upper_flag, uint32_t user_value){

    static char buf[128];   
    status_code_t fault = STATUS_SUCCESS;

    uint32_t base =0xA4107000, offset =0xA64, reg_val;
    uint16_t read_16;

    if (upper_flag > 1) {
        fault = STATUS_ERR_INVALID_FLAG;
        goto result;
    }

    // ---- Update Register ----
    if (reg_read(base, offset, &reg_val) != 0) {
    	fault = STATUS_ERR_HW_FAILED;
        goto result;
    }

    user_value &= 0xFFFF;
    if (upper_flag) {
        reg_val = (reg_val & 0x0000FFFF) | (user_value << 16);
    } else {
        reg_val = (reg_val & 0xFFFF0000) | user_value;
    }
    reg_write(base, offset, reg_val);
    
    // ---- Read Back Verification ----
    reg_read(base, offset, &reg_val);
    read_16 = upper_flag ? ((reg_val >> 16) & 0xFFFF) : (reg_val & 0xFFFF);

    if (read_16 != user_value) 
    	fault = STATUS_ERR_MISMATCH;
    
    result:
    snprintf(buf, sizeof(buf), 
    	"{ \"status\": %d }", 
    	fault);
             
    return buf;
}

char* CANout_tcu1mc3_wrpara_reg0(uint32_t upper_flag, uint32_t user_value){

    static char buf[128];   
    status_code_t fault = STATUS_SUCCESS;

    uint32_t base =0xA4107000, offset =0xD00, reg_val;
    uint16_t read_16;

    if (upper_flag > 1) {
        fault = STATUS_ERR_INVALID_FLAG;
        goto result;
    }

    // ---- Update Register ----
    if (reg_read(base, offset, &reg_val) != 0) {
    	fault = STATUS_ERR_HW_FAILED;
        goto result;
    }

    user_value &= 0xFFFF;
    if (upper_flag) {
        reg_val = (reg_val & 0x0000FFFF) | (user_value << 16);
    } else {
        reg_val = (reg_val & 0xFFFF0000) | user_value;
    }
    reg_write(base, offset, reg_val);
    
    // ---- Read Back Verification ----
    reg_read(base, offset, &reg_val);
    read_16 = upper_flag ? ((reg_val >> 16) & 0xFFFF) : (reg_val & 0xFFFF);

    if (read_16 != user_value) 
    	fault = STATUS_ERR_MISMATCH;
    
    result:
    snprintf(buf, sizeof(buf), 
    	"{ \"status\": %d }", 
    	fault);
             
    return buf;
}

char* CANout_tcu1mc3_wrpara_reg1(uint32_t upper_flag, uint32_t user_value){

    static char buf[128];   
    status_code_t fault = STATUS_SUCCESS;

    uint32_t base =0xA4107000, offset =0xD04, reg_val;
    uint16_t read_16;

    if (upper_flag > 1) {
        fault = STATUS_ERR_INVALID_FLAG;
        goto result;
    }

    // ---- Update Register ----
    if (reg_read(base, offset, &reg_val) != 0) {
    	fault = STATUS_ERR_HW_FAILED;
        goto result;
    }

    user_value &= 0xFFFF;
    if (upper_flag) {
        reg_val = (reg_val & 0x0000FFFF) | (user_value << 16);
    } else {
        reg_val = (reg_val & 0xFFFF0000) | user_value;
    }
    reg_write(base, offset, reg_val);
    
    // ---- Read Back Verification ----
    reg_read(base, offset, &reg_val);
    read_16 = upper_flag ? ((reg_val >> 16) & 0xFFFF) : (reg_val & 0xFFFF);

    if (read_16 != user_value) 
    	fault = STATUS_ERR_MISMATCH;
    
    result:
    snprintf(buf, sizeof(buf), 
    	"{ \"status\": %d }", 
    	fault);
             
    return buf;
}

char* CANout_tcu1mc3_wrpara_reg2(uint32_t upper_flag, uint32_t user_value){

    static char buf[128];   
    status_code_t fault = STATUS_SUCCESS;

    uint32_t base =0xA4107000, offset =0xD08, reg_val;
    uint16_t read_16;

    if (upper_flag > 1) {
        fault = STATUS_ERR_INVALID_FLAG;
        goto result;
    }

    // ---- Update Register ----
    if (reg_read(base, offset, &reg_val) != 0) {
    	fault = STATUS_ERR_HW_FAILED;
        goto result;
    }

    user_value &= 0xFFFF;
    if (upper_flag) {
        reg_val = (reg_val & 0x0000FFFF) | (user_value << 16);
    } else {
        reg_val = (reg_val & 0xFFFF0000) | user_value;
    }
    reg_write(base, offset, reg_val);
    
    // ---- Read Back Verification ----
    reg_read(base, offset, &reg_val);
    read_16 = upper_flag ? ((reg_val >> 16) & 0xFFFF) : (reg_val & 0xFFFF);

    if (read_16 != user_value) 
    	fault = STATUS_ERR_MISMATCH;
    
    result:
    snprintf(buf, sizeof(buf), 
    	"{ \"status\": %d }", 
    	fault);
             
    return buf;
}

char* CANout_tcu1mc3_wrpara_reg3(uint32_t upper_flag, uint32_t user_value){

    static char buf[128];   
    status_code_t fault = STATUS_SUCCESS;

    uint32_t base =0xA4107000, offset =0xD0C, reg_val;
    uint16_t read_16;

    if (upper_flag > 1) {
        fault = STATUS_ERR_INVALID_FLAG;
        goto result;
    }

    // ---- Update Register ----
    if (reg_read(base, offset, &reg_val) != 0) {
    	fault = STATUS_ERR_HW_FAILED;
        goto result;
    }

    user_value &= 0xFFFF;
    if (upper_flag) {
        reg_val = (reg_val & 0x0000FFFF) | (user_value << 16);
    } else {
        reg_val = (reg_val & 0xFFFF0000) | user_value;
    }
    reg_write(base, offset, reg_val);
    
    // ---- Read Back Verification ----
    reg_read(base, offset, &reg_val);
    read_16 = upper_flag ? ((reg_val >> 16) & 0xFFFF) : (reg_val & 0xFFFF);

    if (read_16 != user_value) 
    	fault = STATUS_ERR_MISMATCH;
    
    result:
    snprintf(buf, sizeof(buf), 
    	"{ \"status\": %d }", 
    	fault);
             
    return buf;
}

char* CANout_tcu1mc3_wrpara_reg4(uint32_t upper_flag, uint32_t user_value){

    static char buf[128];   
    status_code_t fault = STATUS_SUCCESS;

    uint32_t base =0xA4107000, offset =0xD10, reg_val;
    uint16_t read_16;

    if (upper_flag > 1) {
        fault = STATUS_ERR_INVALID_FLAG;
        goto result;
    }

    // ---- Update Register ----
    if (reg_read(base, offset, &reg_val) != 0) {
    	fault = STATUS_ERR_HW_FAILED;
        goto result;
    }

    user_value &= 0xFFFF;
    if (upper_flag) {
        reg_val = (reg_val & 0x0000FFFF) | (user_value << 16);
    } else {
        reg_val = (reg_val & 0xFFFF0000) | user_value;
    }
    reg_write(base, offset, reg_val);
    
    // ---- Read Back Verification ----
    reg_read(base, offset, &reg_val);
    read_16 = upper_flag ? ((reg_val >> 16) & 0xFFFF) : (reg_val & 0xFFFF);

    if (read_16 != user_value) 
    	fault = STATUS_ERR_MISMATCH;
    
    result:
    snprintf(buf, sizeof(buf), 
    	"{ \"status\": %d }", 
    	fault);
             
    return buf;
}

char* CANout_tcu1mc3_wrpara_reg5(uint32_t upper_flag, uint32_t user_value){

    static char buf[128];   
    status_code_t fault = STATUS_SUCCESS;

    uint32_t base =0xA4107000, offset =0xD14, reg_val;
    uint16_t read_16;

    if (upper_flag > 1) {
        fault = STATUS_ERR_INVALID_FLAG;
        goto result;
    }

    // ---- Update Register ----
    if (reg_read(base, offset, &reg_val) != 0) {
    	fault = STATUS_ERR_HW_FAILED;
        goto result;
    }

    user_value &= 0xFFFF;
    if (upper_flag) {
        reg_val = (reg_val & 0x0000FFFF) | (user_value << 16);
    } else {
        reg_val = (reg_val & 0xFFFF0000) | user_value;
    }
    reg_write(base, offset, reg_val);
    
    // ---- Read Back Verification ----
    reg_read(base, offset, &reg_val);
    read_16 = upper_flag ? ((reg_val >> 16) & 0xFFFF) : (reg_val & 0xFFFF);

    if (read_16 != user_value) 
    	fault = STATUS_ERR_MISMATCH;
    
    result:
    snprintf(buf, sizeof(buf), 
    	"{ \"status\": %d }", 
    	fault);
             
    return buf;
}

char* CANout_tcu1mc3_wrpara_reg6(uint32_t upper_flag, uint32_t user_value){

    static char buf[128];   
    status_code_t fault = STATUS_SUCCESS;

    uint32_t base =0xA4107000, offset =0xD18, reg_val;
    uint16_t read_16;

    if (upper_flag > 1) {
        fault = STATUS_ERR_INVALID_FLAG;
        goto result;
    }

    // ---- Update Register ----
    if (reg_read(base, offset, &reg_val) != 0) {
    	fault = STATUS_ERR_HW_FAILED;
        goto result;
    }

    user_value &= 0xFFFF;
    if (upper_flag) {
        reg_val = (reg_val & 0x0000FFFF) | (user_value << 16);
    } else {
        reg_val = (reg_val & 0xFFFF0000) | user_value;
    }
    reg_write(base, offset, reg_val);
    
    // ---- Read Back Verification ----
    reg_read(base, offset, &reg_val);
    read_16 = upper_flag ? ((reg_val >> 16) & 0xFFFF) : (reg_val & 0xFFFF);

    if (read_16 != user_value) 
    	fault = STATUS_ERR_MISMATCH;
    
    result:
    snprintf(buf, sizeof(buf), 
    	"{ \"status\": %d }", 
    	fault);
             
    return buf;
}

char* CANout_tcu1mc3_wrpara_reg7(uint32_t upper_flag, uint32_t user_value){

    static char buf[128];   
    status_code_t fault = STATUS_SUCCESS;

    uint32_t base =0xA4107000, offset =0xD1C, reg_val;
    uint16_t read_16;

    if (upper_flag > 1) {
        fault = STATUS_ERR_INVALID_FLAG;
        goto result;
    }

    // ---- Update Register ----
    if (reg_read(base, offset, &reg_val) != 0) {
    	fault = STATUS_ERR_HW_FAILED;
        goto result;
    }

    user_value &= 0xFFFF;
    if (upper_flag) {
        reg_val = (reg_val & 0x0000FFFF) | (user_value << 16);
    } else {
        reg_val = (reg_val & 0xFFFF0000) | user_value;
    }
    reg_write(base, offset, reg_val);
    
    // ---- Read Back Verification ----
    reg_read(base, offset, &reg_val);
    read_16 = upper_flag ? ((reg_val >> 16) & 0xFFFF) : (reg_val & 0xFFFF);

    if (read_16 != user_value) 
    	fault = STATUS_ERR_MISMATCH;
    
    result:
    snprintf(buf, sizeof(buf), 
    	"{ \"status\": %d }", 
    	fault);
             
    return buf;
}

char* CANout_tcu1mc3_wrpara_reg8(uint32_t upper_flag, uint32_t user_value){

    static char buf[128];   
    status_code_t fault = STATUS_SUCCESS;

    uint32_t base =0xA4107000, offset =0xD20, reg_val;
    uint16_t read_16;

    if (upper_flag > 1) {
        fault = STATUS_ERR_INVALID_FLAG;
        goto result;
    }

    // ---- Update Register ----
    if (reg_read(base, offset, &reg_val) != 0) {
    	fault = STATUS_ERR_HW_FAILED;
        goto result;
    }

    user_value &= 0xFFFF;
    if (upper_flag) {
        reg_val = (reg_val & 0x0000FFFF) | (user_value << 16);
    } else {
        reg_val = (reg_val & 0xFFFF0000) | user_value;
    }
    reg_write(base, offset, reg_val);
    
    // ---- Read Back Verification ----
    reg_read(base, offset, &reg_val);
    read_16 = upper_flag ? ((reg_val >> 16) & 0xFFFF) : (reg_val & 0xFFFF);

    if (read_16 != user_value) 
    	fault = STATUS_ERR_MISMATCH;
    
    result:
    snprintf(buf, sizeof(buf), 
    	"{ \"status\": %d }", 
    	fault);
             
    return buf;
}

char* CANout_tcu1mc3_wrpara_reg9(uint32_t upper_flag, uint32_t user_value){

    static char buf[128];   
    status_code_t fault = STATUS_SUCCESS;

    uint32_t base =0xA4107000, offset =0xD24, reg_val;
    uint16_t read_16;

    if (upper_flag > 1) {
        fault = STATUS_ERR_INVALID_FLAG;
        goto result;
    }

    // ---- Update Register ----
    if (reg_read(base, offset, &reg_val) != 0) {
    	fault = STATUS_ERR_HW_FAILED;
        goto result;
    }

    user_value &= 0xFFFF;
    if (upper_flag) {
        reg_val = (reg_val & 0x0000FFFF) | (user_value << 16);
    } else {
        reg_val = (reg_val & 0xFFFF0000) | user_value;
    }
    reg_write(base, offset, reg_val);
    
    // ---- Read Back Verification ----
    reg_read(base, offset, &reg_val);
    read_16 = upper_flag ? ((reg_val >> 16) & 0xFFFF) : (reg_val & 0xFFFF);

    if (read_16 != user_value) 
    	fault = STATUS_ERR_MISMATCH;
    
    result:
    snprintf(buf, sizeof(buf), 
    	"{ \"status\": %d }", 
    	fault);
             
    return buf;
}

char* CANout_tcu1mc3_wrpara_reg10(uint32_t upper_flag, uint32_t user_value){

    static char buf[128];   
    status_code_t fault = STATUS_SUCCESS;

    uint32_t base =0xA4107000, offset =0xD28, reg_val;
    uint16_t read_16;

    if (upper_flag > 1) {
        fault = STATUS_ERR_INVALID_FLAG;
        goto result;
    }

    // ---- Update Register ----
    if (reg_read(base, offset, &reg_val) != 0) {
    	fault = STATUS_ERR_HW_FAILED;
        goto result;
    }

    user_value &= 0xFFFF;
    if (upper_flag) {
        reg_val = (reg_val & 0x0000FFFF) | (user_value << 16);
    } else {
        reg_val = (reg_val & 0xFFFF0000) | user_value;
    }
    reg_write(base, offset, reg_val);
    
    // ---- Read Back Verification ----
    reg_read(base, offset, &reg_val);
    read_16 = upper_flag ? ((reg_val >> 16) & 0xFFFF) : (reg_val & 0xFFFF);

    if (read_16 != user_value) 
    	fault = STATUS_ERR_MISMATCH;
    
    result:
    snprintf(buf, sizeof(buf), 
    	"{ \"status\": %d }", 
    	fault);
             
    return buf;
}

char* CANout_tcu1mc3_wrpara_reg11(uint32_t upper_flag, uint32_t user_value){

    static char buf[128];   
    status_code_t fault = STATUS_SUCCESS;

    uint32_t base =0xA4107000, offset =0xD2C, reg_val;
    uint16_t read_16;

    if (upper_flag > 1) {
        fault = STATUS_ERR_INVALID_FLAG;
        goto result;
    }

    // ---- Update Register ----
    if (reg_read(base, offset, &reg_val) != 0) {
    	fault = STATUS_ERR_HW_FAILED;
        goto result;
    }

    user_value &= 0xFFFF;
    if (upper_flag) {
        reg_val = (reg_val & 0x0000FFFF) | (user_value << 16);
    } else {
        reg_val = (reg_val & 0xFFFF0000) | user_value;
    }
    reg_write(base, offset, reg_val);
    
    // ---- Read Back Verification ----
    reg_read(base, offset, &reg_val);
    read_16 = upper_flag ? ((reg_val >> 16) & 0xFFFF) : (reg_val & 0xFFFF);

    if (read_16 != user_value) 
    	fault = STATUS_ERR_MISMATCH;
    
    result:
    snprintf(buf, sizeof(buf), 
    	"{ \"status\": %d }", 
    	fault);
             
    return buf;
}

char* CANout_tcu1mc3_wrpara_reg12(uint32_t upper_flag, uint32_t user_value){

    static char buf[128];   
    status_code_t fault = STATUS_SUCCESS;

    uint32_t base =0xA4107000, offset =0xD30, reg_val;
    uint16_t read_16;

    if (upper_flag > 1) {
        fault = STATUS_ERR_INVALID_FLAG;
        goto result;
    }

    // ---- Update Register ----
    if (reg_read(base, offset, &reg_val) != 0) {
    	fault = STATUS_ERR_HW_FAILED;
        goto result;
    }

    user_value &= 0xFFFF;
    if (upper_flag) {
        reg_val = (reg_val & 0x0000FFFF) | (user_value << 16);
    } else {
        reg_val = (reg_val & 0xFFFF0000) | user_value;
    }
    reg_write(base, offset, reg_val);
    
    // ---- Read Back Verification ----
    reg_read(base, offset, &reg_val);
    read_16 = upper_flag ? ((reg_val >> 16) & 0xFFFF) : (reg_val & 0xFFFF);

    if (read_16 != user_value) 
    	fault = STATUS_ERR_MISMATCH;
    
    result:
    snprintf(buf, sizeof(buf), 
    	"{ \"status\": %d }", 
    	fault);
             
    return buf;
}

char* CANout_tcu1mc3_wrpara_reg13(uint32_t upper_flag, uint32_t user_value){

    static char buf[128];   
    status_code_t fault = STATUS_SUCCESS;

    uint32_t base =0xA4107000, offset =0xD34, reg_val;
    uint16_t read_16;

    if (upper_flag > 1) {
        fault = STATUS_ERR_INVALID_FLAG;
        goto result;
    }

    // ---- Update Register ----
    if (reg_read(base, offset, &reg_val) != 0) {
    	fault = STATUS_ERR_HW_FAILED;
        goto result;
    }

    user_value &= 0xFFFF;
    if (upper_flag) {
        reg_val = (reg_val & 0x0000FFFF) | (user_value << 16);
    } else {
        reg_val = (reg_val & 0xFFFF0000) | user_value;
    }
    reg_write(base, offset, reg_val);
    
    // ---- Read Back Verification ----
    reg_read(base, offset, &reg_val);
    read_16 = upper_flag ? ((reg_val >> 16) & 0xFFFF) : (reg_val & 0xFFFF);

    if (read_16 != user_value) 
    	fault = STATUS_ERR_MISMATCH;
    
    result:
    snprintf(buf, sizeof(buf), 
    	"{ \"status\": %d }", 
    	fault);
             
    return buf;
}

char* CANout_tcu1mc3_wrpara_reg14(uint32_t upper_flag, uint32_t user_value){

    static char buf[128];   
    status_code_t fault = STATUS_SUCCESS;

    uint32_t base =0xA4107000, offset =0xD38, reg_val;
    uint16_t read_16;

    if (upper_flag > 1) {
        fault = STATUS_ERR_INVALID_FLAG;
        goto result;
    }

    // ---- Update Register ----
    if (reg_read(base, offset, &reg_val) != 0) {
    	fault = STATUS_ERR_HW_FAILED;
        goto result;
    }

    user_value &= 0xFFFF;
    if (upper_flag) {
        reg_val = (reg_val & 0x0000FFFF) | (user_value << 16);
    } else {
        reg_val = (reg_val & 0xFFFF0000) | user_value;
    }
    reg_write(base, offset, reg_val);
    
    // ---- Read Back Verification ----
    reg_read(base, offset, &reg_val);
    read_16 = upper_flag ? ((reg_val >> 16) & 0xFFFF) : (reg_val & 0xFFFF);

    if (read_16 != user_value) 
    	fault = STATUS_ERR_MISMATCH;
    
    result:
    snprintf(buf, sizeof(buf), 
    	"{ \"status\": %d }", 
    	fault);
             
    return buf;
}

char* CANout_tcu1mc3_wrpara_reg15(uint32_t upper_flag, uint32_t user_value){

    static char buf[128];   
    status_code_t fault = STATUS_SUCCESS;

    uint32_t base =0xA4107000, offset =0xD3C, reg_val;
    uint16_t read_16;

    if (upper_flag > 1) {
        fault = STATUS_ERR_INVALID_FLAG;
        goto result;
    }

    // ---- Update Register ----
    if (reg_read(base, offset, &reg_val) != 0) {
    	fault = STATUS_ERR_HW_FAILED;
        goto result;
    }

    user_value &= 0xFFFF;
    if (upper_flag) {
        reg_val = (reg_val & 0x0000FFFF) | (user_value << 16);
    } else {
        reg_val = (reg_val & 0xFFFF0000) | user_value;
    }
    reg_write(base, offset, reg_val);
    
    // ---- Read Back Verification ----
    reg_read(base, offset, &reg_val);
    read_16 = upper_flag ? ((reg_val >> 16) & 0xFFFF) : (reg_val & 0xFFFF);

    if (read_16 != user_value) 
    	fault = STATUS_ERR_MISMATCH;
    
    result:
    snprintf(buf, sizeof(buf), 
    	"{ \"status\": %d }", 
    	fault);
             
    return buf;
}

char* CANout_tcu1mc3_wrpara_reg16(uint32_t upper_flag, uint32_t user_value){

    static char buf[128];   
    status_code_t fault = STATUS_SUCCESS;

    uint32_t base =0xA4107000, offset =0xD40, reg_val;
    uint16_t read_16;

    if (upper_flag > 1) {
        fault = STATUS_ERR_INVALID_FLAG;
        goto result;
    }

    // ---- Update Register ----
    if (reg_read(base, offset, &reg_val) != 0) {
    	fault = STATUS_ERR_HW_FAILED;
        goto result;
    }

    user_value &= 0xFFFF;
    if (upper_flag) {
        reg_val = (reg_val & 0x0000FFFF) | (user_value << 16);
    } else {
        reg_val = (reg_val & 0xFFFF0000) | user_value;
    }
    reg_write(base, offset, reg_val);
    
    // ---- Read Back Verification ----
    reg_read(base, offset, &reg_val);
    read_16 = upper_flag ? ((reg_val >> 16) & 0xFFFF) : (reg_val & 0xFFFF);

    if (read_16 != user_value) 
    	fault = STATUS_ERR_MISMATCH;
    
    result:
    snprintf(buf, sizeof(buf), 
    	"{ \"status\": %d }", 
    	fault);
             
    return buf;
}

char* CANout_tcu1mc3_wrpara_reg17(uint32_t upper_flag, uint32_t user_value){

    static char buf[128];   
    status_code_t fault = STATUS_SUCCESS;

    uint32_t base =0xA4107000, offset =0xD44, reg_val;
    uint16_t read_16;

    if (upper_flag > 1) {
        fault = STATUS_ERR_INVALID_FLAG;
        goto result;
    }

    // ---- Update Register ----
    if (reg_read(base, offset, &reg_val) != 0) {
    	fault = STATUS_ERR_HW_FAILED;
        goto result;
    }

    user_value &= 0xFFFF;
    if (upper_flag) {
        reg_val = (reg_val & 0x0000FFFF) | (user_value << 16);
    } else {
        reg_val = (reg_val & 0xFFFF0000) | user_value;
    }
    reg_write(base, offset, reg_val);
    
    // ---- Read Back Verification ----
    reg_read(base, offset, &reg_val);
    read_16 = upper_flag ? ((reg_val >> 16) & 0xFFFF) : (reg_val & 0xFFFF);

    if (read_16 != user_value) 
    	fault = STATUS_ERR_MISMATCH;
    
    result:
    snprintf(buf, sizeof(buf), 
    	"{ \"status\": %d }", 
    	fault);
             
    return buf;
}

char* CANout_tcu1mc3_wrpara_reg18(uint32_t upper_flag, uint32_t user_value){

    static char buf[128];   
    status_code_t fault = STATUS_SUCCESS;

    uint32_t base =0xA4107000, offset =0xD48, reg_val;
    uint16_t read_16;

    if (upper_flag > 1) {
        fault = STATUS_ERR_INVALID_FLAG;
        goto result;
    }

    // ---- Update Register ----
    if (reg_read(base, offset, &reg_val) != 0) {
    	fault = STATUS_ERR_HW_FAILED;
        goto result;
    }

    user_value &= 0xFFFF;
    if (upper_flag) {
        reg_val = (reg_val & 0x0000FFFF) | (user_value << 16);
    } else {
        reg_val = (reg_val & 0xFFFF0000) | user_value;
    }
    reg_write(base, offset, reg_val);
    
    // ---- Read Back Verification ----
    reg_read(base, offset, &reg_val);
    read_16 = upper_flag ? ((reg_val >> 16) & 0xFFFF) : (reg_val & 0xFFFF);

    if (read_16 != user_value) 
    	fault = STATUS_ERR_MISMATCH;
    
    result:
    snprintf(buf, sizeof(buf), 
    	"{ \"status\": %d }", 
    	fault);
             
    return buf;
}

char* CANout_tcu1mc3_wrpara_reg19(uint32_t upper_flag, uint32_t user_value){

    static char buf[128];   
    status_code_t fault = STATUS_SUCCESS;

    uint32_t base =0xA4107000, offset =0xD4C, reg_val;
    uint16_t read_16;

    if (upper_flag > 1) {
        fault = STATUS_ERR_INVALID_FLAG;
        goto result;
    }

    // ---- Update Register ----
    if (reg_read(base, offset, &reg_val) != 0) {
    	fault = STATUS_ERR_HW_FAILED;
        goto result;
    }

    user_value &= 0xFFFF;
    if (upper_flag) {
        reg_val = (reg_val & 0x0000FFFF) | (user_value << 16);
    } else {
        reg_val = (reg_val & 0xFFFF0000) | user_value;
    }
    reg_write(base, offset, reg_val);
    
    // ---- Read Back Verification ----
    reg_read(base, offset, &reg_val);
    read_16 = upper_flag ? ((reg_val >> 16) & 0xFFFF) : (reg_val & 0xFFFF);

    if (read_16 != user_value) 
    	fault = STATUS_ERR_MISMATCH;
    
    result:
    snprintf(buf, sizeof(buf), 
    	"{ \"status\": %d }", 
    	fault);
             
    return buf;
}

char* CANout_tcu1mc3_wrpara_reg20(uint32_t upper_flag, uint32_t user_value){

    static char buf[128];   
    status_code_t fault = STATUS_SUCCESS;

    uint32_t base =0xA4107000, offset =0xD50, reg_val;
    uint16_t read_16;

    if (upper_flag > 1) {
        fault = STATUS_ERR_INVALID_FLAG;
        goto result;
    }

    // ---- Update Register ----
    if (reg_read(base, offset, &reg_val) != 0) {
    	fault = STATUS_ERR_HW_FAILED;
        goto result;
    }

    user_value &= 0xFFFF;
    if (upper_flag) {
        reg_val = (reg_val & 0x0000FFFF) | (user_value << 16);
    } else {
        reg_val = (reg_val & 0xFFFF0000) | user_value;
    }
    reg_write(base, offset, reg_val);
    
    // ---- Read Back Verification ----
    reg_read(base, offset, &reg_val);
    read_16 = upper_flag ? ((reg_val >> 16) & 0xFFFF) : (reg_val & 0xFFFF);

    if (read_16 != user_value) 
    	fault = STATUS_ERR_MISMATCH;
    
    result:
    snprintf(buf, sizeof(buf), 
    	"{ \"status\": %d }", 
    	fault);
             
    return buf;
}

char* CANout_tcu1mc3_wrpara_reg21(uint32_t upper_flag, uint32_t user_value){

    static char buf[128];   
    status_code_t fault = STATUS_SUCCESS;

    uint32_t base =0xA4107000, offset =0xD54, reg_val;
    uint16_t read_16;

    if (upper_flag > 1) {
        fault = STATUS_ERR_INVALID_FLAG;
        goto result;
    }

    // ---- Update Register ----
    if (reg_read(base, offset, &reg_val) != 0) {
    	fault = STATUS_ERR_HW_FAILED;
        goto result;
    }

    user_value &= 0xFFFF;
    if (upper_flag) {
        reg_val = (reg_val & 0x0000FFFF) | (user_value << 16);
    } else {
        reg_val = (reg_val & 0xFFFF0000) | user_value;
    }
    reg_write(base, offset, reg_val);
    
    // ---- Read Back Verification ----
    reg_read(base, offset, &reg_val);
    read_16 = upper_flag ? ((reg_val >> 16) & 0xFFFF) : (reg_val & 0xFFFF);

    if (read_16 != user_value) 
    	fault = STATUS_ERR_MISMATCH;
    
    result:
    snprintf(buf, sizeof(buf), 
    	"{ \"status\": %d }", 
    	fault);
             
    return buf;
}

char* CANout_tcu1mc3_wrpara_reg22(uint32_t upper_flag, uint32_t user_value){

    static char buf[128];   
    status_code_t fault = STATUS_SUCCESS;

    uint32_t base =0xA4107000, offset =0xD58, reg_val;
    uint16_t read_16;

    if (upper_flag > 1) {
        fault = STATUS_ERR_INVALID_FLAG;
        goto result;
    }

    // ---- Update Register ----
    if (reg_read(base, offset, &reg_val) != 0) {
    	fault = STATUS_ERR_HW_FAILED;
        goto result;
    }

    user_value &= 0xFFFF;
    if (upper_flag) {
        reg_val = (reg_val & 0x0000FFFF) | (user_value << 16);
    } else {
        reg_val = (reg_val & 0xFFFF0000) | user_value;
    }
    reg_write(base, offset, reg_val);
    
    // ---- Read Back Verification ----
    reg_read(base, offset, &reg_val);
    read_16 = upper_flag ? ((reg_val >> 16) & 0xFFFF) : (reg_val & 0xFFFF);

    if (read_16 != user_value) 
    	fault = STATUS_ERR_MISMATCH;
    
    result:
    snprintf(buf, sizeof(buf), 
    	"{ \"status\": %d }", 
    	fault);
             
    return buf;
}

char* CANout_tcu1mc3_wrpara_reg23(uint32_t upper_flag, uint32_t user_value){

    static char buf[128];   
    status_code_t fault = STATUS_SUCCESS;

    uint32_t base =0xA4107000, offset =0xD5C, reg_val;
    uint16_t read_16;

    if (upper_flag > 1) {
        fault = STATUS_ERR_INVALID_FLAG;
        goto result;
    }

    // ---- Update Register ----
    if (reg_read(base, offset, &reg_val) != 0) {
    	fault = STATUS_ERR_HW_FAILED;
        goto result;
    }

    user_value &= 0xFFFF;
    if (upper_flag) {
        reg_val = (reg_val & 0x0000FFFF) | (user_value << 16);
    } else {
        reg_val = (reg_val & 0xFFFF0000) | user_value;
    }
    reg_write(base, offset, reg_val);
    
    // ---- Read Back Verification ----
    reg_read(base, offset, &reg_val);
    read_16 = upper_flag ? ((reg_val >> 16) & 0xFFFF) : (reg_val & 0xFFFF);

    if (read_16 != user_value) 
    	fault = STATUS_ERR_MISMATCH;
    
    result:
    snprintf(buf, sizeof(buf), 
    	"{ \"status\": %d }", 
    	fault);
             
    return buf;
}

char* CANout_tcu1mc3_wrpara_reg24(uint32_t upper_flag, uint32_t user_value){

    static char buf[128];   
    status_code_t fault = STATUS_SUCCESS;

    uint32_t base =0xA4107000, offset =0xD60, reg_val;
    uint16_t read_16;

    if (upper_flag > 1) {
        fault = STATUS_ERR_INVALID_FLAG;
        goto result;
    }

    // ---- Update Register ----
    if (reg_read(base, offset, &reg_val) != 0) {
    	fault = STATUS_ERR_HW_FAILED;
        goto result;
    }

    user_value &= 0xFFFF;
    if (upper_flag) {
        reg_val = (reg_val & 0x0000FFFF) | (user_value << 16);
    } else {
        reg_val = (reg_val & 0xFFFF0000) | user_value;
    }
    reg_write(base, offset, reg_val);
    
    // ---- Read Back Verification ----
    reg_read(base, offset, &reg_val);
    read_16 = upper_flag ? ((reg_val >> 16) & 0xFFFF) : (reg_val & 0xFFFF);

    if (read_16 != user_value) 
    	fault = STATUS_ERR_MISMATCH;
    
    result:
    snprintf(buf, sizeof(buf), 
    	"{ \"status\": %d }", 
    	fault);
             
    return buf;
}

char* CANout_tcu1mc3_wrpara_reg25(uint32_t upper_flag, uint32_t user_value){

    static char buf[128];   
    status_code_t fault = STATUS_SUCCESS;

    uint32_t base =0xA4107000, offset =0xD64, reg_val;
    uint16_t read_16;

    if (upper_flag > 1) {
        fault = STATUS_ERR_INVALID_FLAG;
        goto result;
    }

    // ---- Update Register ----
    if (reg_read(base, offset, &reg_val) != 0) {
    	fault = STATUS_ERR_HW_FAILED;
        goto result;
    }

    user_value &= 0xFFFF;
    if (upper_flag) {
        reg_val = (reg_val & 0x0000FFFF) | (user_value << 16);
    } else {
        reg_val = (reg_val & 0xFFFF0000) | user_value;
    }
    reg_write(base, offset, reg_val);
    
    // ---- Read Back Verification ----
    reg_read(base, offset, &reg_val);
    read_16 = upper_flag ? ((reg_val >> 16) & 0xFFFF) : (reg_val & 0xFFFF);

    if (read_16 != user_value) 
    	fault = STATUS_ERR_MISMATCH;
    
    result:
    snprintf(buf, sizeof(buf), 
    	"{ \"status\": %d }", 
    	fault);
             
    return buf;
}

char* CANIn_tcu1lc1_wrpara_reg0_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA3106000, 0x100, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"id\": 0, \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* CANIn_tcu1lc1_wrpara_reg1_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA3106000, 0x104, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"id\": 1, \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* CANIn_tcu1lc1_wrpara_reg2_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA3106000, 0x108, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"id\": 2, \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* CANIn_tcu1lc1_wrpara_reg3_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA3106000, 0x10C, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"id\": 3, \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* CANIn_tcu1lc1_wrpara_reg4_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA3106000, 0x110, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"id\": 4, \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* CANIn_tcu1lc1_wrpara_reg5_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA3106000, 0x114, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"id\": 5, \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* CANIn_tcu1lc1_wrpara_reg6_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA3106000, 0x118, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"id\": 6, \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* CANIn_tcu1lc1_wrpara_reg7_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA3106000, 0x11C, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"id\": 7, \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* CANIn_tcu1lc1_wrpara_reg8_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA3106000, 0x120, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"id\": 8, \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* CANIn_tcu1lc1_wrpara_reg9_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA3106000, 0x124, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"id\": 9, \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* CANIn_tcu1lc1_wrpara_reg10_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA3106000, 0x128, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"id\": 10, \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* CANIn_tcu1lc1_wrpara_reg11_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA3106000, 0x12C, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"id\": 11, \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* CANIn_tcu1lc1_wrpara_reg12_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA3106000, 0x130, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"id\": 12, \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* CANIn_tcu1lc1_wrpara_reg13_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA3106000, 0x134, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"id\": 13, \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* CANIn_tcu1lc1_wrpara_reg14_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA3106000, 0x138, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"id\": 14, \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* CANIn_tcu1lc1_wrpara_reg15_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA3106000, 0x13C, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"id\": 15, \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* CANIn_tcu1lc1_wrpara_reg16_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA3106000, 0x140, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"id\": 16, \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* CANIn_tcu1lc1_wrpara_reg17_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA3106000, 0x144, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"id\": 17, \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* CANIn_tcu1lc1_wrpara_reg18_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA3106000, 0x148, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"id\": 18, \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* CANIn_tcu1lc1_wrpara_reg19_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA3106000, 0x14C, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"id\": 19, \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* CANIn_tcu1lc1_wrpara_reg20_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA3106000, 0x150, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"id\": 20, \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* CANIn_tcu1lc1_wrpara_reg21_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA3106000, 0x154, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"id\": 21, \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* CANIn_tcu1lc1_wrpara_reg22_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA3106000, 0x158, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"id\": 22, \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* CANIn_tcu1lc1_wrpara_reg23_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA3106000, 0x15C, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"id\": 23, \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* CANIn_tcu1lc1_wrpara_reg24_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA3106000, 0x160, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"id\": 24, \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* CANIn_tcu1lc1_wrpara_reg25_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA3106000, 0x164, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"id\": 25, \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* CANIn_tcu1lc2_wrpara_reg0_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA3106000, 0x400, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"id\": 0, \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* CANIn_tcu1lc2_wrpara_reg1_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA3106000, 0x404, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"id\": 1, \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* CANIn_tcu1lc2_wrpara_reg2_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA3106000, 0x408, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"id\": 2, \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* CANIn_tcu1lc2_wrpara_reg3_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA3106000, 0x40C, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"id\": 3, \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* CANIn_tcu1lc2_wrpara_reg4_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA3106000, 0x410, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"id\": 4, \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* CANIn_tcu1lc2_wrpara_reg5_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA3106000, 0x414, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"id\": 5, \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* CANIn_tcu1lc2_wrpara_reg6_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA3106000, 0x418, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"id\": 6, \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* CANIn_tcu1lc2_wrpara_reg7_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA3106000, 0x41C, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"id\": 7, \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* CANIn_tcu1lc2_wrpara_reg8_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA3106000, 0x420, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"id\": 8, \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* CANIn_tcu1lc2_wrpara_reg9_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA3106000, 0x424, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"id\": 9, \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* CANIn_tcu1lc2_wrpara_reg10_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA3106000, 0x428, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"id\": 10, \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* CANIn_tcu1lc2_wrpara_reg11_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA3106000, 0x42C, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"id\": 11, \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* CANIn_tcu1lc2_wrpara_reg12_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA3106000, 0x430, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"id\": 12, \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* CANIn_tcu1lc2_wrpara_reg13_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA3106000, 0x434, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"id\": 13, \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* CANIn_tcu1lc2_wrpara_reg14_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA3106000, 0x438, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"id\": 14, \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* CANIn_tcu1lc2_wrpara_reg15_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA3106000, 0x43C, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"id\": 15, \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* CANIn_tcu1lc2_wrpara_reg16_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA3106000, 0x440, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"id\": 16, \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* CANIn_tcu1lc2_wrpara_reg17_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA3106000, 0x444, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"id\": 17, \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* CANIn_tcu1lc2_wrpara_reg18_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA3106000, 0x448, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"id\": 18, \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* CANIn_tcu1lc2_wrpara_reg19_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA3106000, 0x44C, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"id\": 19, \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* CANIn_tcu1lc2_wrpara_reg20_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA3106000, 0x450, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"id\": 20, \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* CANIn_tcu1lc2_wrpara_reg21_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA3106000, 0x454, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"id\": 21, \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* CANIn_tcu1lc2_wrpara_reg22_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA3106000, 0x458, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"id\": 22, \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* CANIn_tcu1lc2_wrpara_reg23_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA3106000, 0x45C, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"id\": 23, \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* CANIn_tcu1lc2_wrpara_reg24_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA3106000, 0x460, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"id\": 24, \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* CANIn_tcu1lc2_wrpara_reg25_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA3106000, 0x464, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"id\": 25, \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* CANIn_tcu1mc1_wrpara_reg0_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA3106000, 0x700, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"id\": 0, \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* CANIn_tcu1mc1_wrpara_reg1_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA3106000, 0x704, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"id\": 1, \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* CANIn_tcu1mc1_wrpara_reg2_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA3106000, 0x708, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"id\": 2, \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* CANIn_tcu1mc1_wrpara_reg3_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA3106000, 0x70C, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"id\": 3, \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* CANIn_tcu1mc1_wrpara_reg4_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA3106000, 0x710, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"id\": 4, \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* CANIn_tcu1mc1_wrpara_reg5_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA3106000, 0x714, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"id\": 5, \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* CANIn_tcu1mc1_wrpara_reg6_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA3106000, 0x718, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"id\": 6, \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* CANIn_tcu1mc1_wrpara_reg7_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA3106000, 0x71C, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"id\": 7, \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* CANIn_tcu1mc1_wrpara_reg8_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA3106000, 0x720, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"id\": 8, \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* CANIn_tcu1mc1_wrpara_reg9_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA3106000, 0x724, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"id\": 9, \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* CANIn_tcu1mc1_wrpara_reg10_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA3106000, 0x728, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"id\": 10, \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* CANIn_tcu1mc1_wrpara_reg11_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA3106000, 0x72C, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"id\": 11, \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* CANIn_tcu1mc1_wrpara_reg12_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA3106000, 0x730, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"id\": 12, \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* CANIn_tcu1mc1_wrpara_reg13_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA3106000, 0x734, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"id\": 13, \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* CANIn_tcu1mc1_wrpara_reg14_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA3106000, 0x738, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"id\": 14, \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* CANIn_tcu1mc1_wrpara_reg15_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA3106000, 0x73C, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"id\": 15, \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* CANIn_tcu1mc1_wrpara_reg16_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA3106000, 0x740, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"id\": 16, \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* CANIn_tcu1mc1_wrpara_reg17_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA3106000, 0x744, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"id\": 17, \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* CANIn_tcu1mc1_wrpara_reg18_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA3106000, 0x748, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"id\": 18, \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* CANIn_tcu1mc1_wrpara_reg19_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA3106000, 0x74C, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"id\": 19, \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* CANIn_tcu1mc1_wrpara_reg20_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA3106000, 0x750, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"id\": 20, \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* CANIn_tcu1mc1_wrpara_reg21_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA3106000, 0x754, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"id\": 21, \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* CANIn_tcu1mc1_wrpara_reg22_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA3106000, 0x758, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"id\": 22, \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* CANIn_tcu1mc1_wrpara_reg23_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA3106000, 0x75C, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"id\": 23, \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* CANIn_tcu1mc1_wrpara_reg24_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA3106000, 0x760, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"id\": 24, \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* CANIn_tcu1mc1_wrpara_reg25_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA3106000, 0x764, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"id\": 25, \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}


char* CANIn_tcu1mc2_wrpara_reg0_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA3106000, 0xA00, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"id\": 0, \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* CANIn_tcu1mc2_wrpara_reg1_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA3106000, 0xA04, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"id\": 1, \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* CANIn_tcu1mc2_wrpara_reg2_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA3106000, 0xA08, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"id\": 2, \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* CANIn_tcu1mc2_wrpara_reg3_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA3106000, 0xA0C, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"id\": 3, \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* CANIn_tcu1mc2_wrpara_reg4_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA3106000, 0xA10, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"id\": 4, \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* CANIn_tcu1mc2_wrpara_reg5_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA3106000, 0xA14, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"id\": 5, \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* CANIn_tcu1mc2_wrpara_reg6_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA3106000, 0xA18, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"id\": 6, \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* CANIn_tcu1mc2_wrpara_reg7_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA3106000, 0xA1C, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"id\": 7, \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* CANIn_tcu1mc2_wrpara_reg8_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA3106000, 0xA20, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"id\": 8, \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* CANIn_tcu1mc2_wrpara_reg9_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA3106000, 0xA24, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"id\": 9, \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* CANIn_tcu1mc2_wrpara_reg10_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA3106000, 0xA28, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"id\": 10, \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* CANIn_tcu1mc2_wrpara_reg11_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA3106000, 0xA2C, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"id\": 11, \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* CANIn_tcu1mc2_wrpara_reg12_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA3106000, 0xA30, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"id\": 12, \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* CANIn_tcu1mc2_wrpara_reg13_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA3106000, 0xA34, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"id\": 13, \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* CANIn_tcu1mc2_wrpara_reg14_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA3106000, 0xA38, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"id\": 14, \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* CANIn_tcu1mc2_wrpara_reg15_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA3106000, 0xA3C, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"id\": 15, \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* CANIn_tcu1mc2_wrpara_reg16_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA3106000, 0xA40, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"id\": 16, \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* CANIn_tcu1mc2_wrpara_reg17_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA3106000, 0xA44, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"id\": 17, \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* CANIn_tcu1mc2_wrpara_reg18_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA3106000, 0xA48, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"id\": 18, \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* CANIn_tcu1mc2_wrpara_reg19_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA3106000, 0xA4C, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"id\": 19, \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* CANIn_tcu1mc2_wrpara_reg20_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA3106000, 0xA50, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"id\": 20, \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* CANIn_tcu1mc2_wrpara_reg21_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA3106000, 0xA54, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"id\": 21, \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* CANIn_tcu1mc2_wrpara_reg22_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA3106000, 0xA58, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"id\": 22, \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* CANIn_tcu1mc2_wrpara_reg23_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA3106000, 0xA5C, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"id\": 23, \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* CANIn_tcu1mc2_wrpara_reg24_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA3106000, 0xA60, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"id\": 24, \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* CANIn_tcu1mc2_wrpara_reg25_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA3106000, 0xA64, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"id\": 25, \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}


char* CANIn_tcu1mc3_wrpara_reg0_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA3106000, 0xD00, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"id\": 0, \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* CANIn_tcu1mc3_wrpara_reg1_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA3106000, 0xD04, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"id\": 1, \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* CANIn_tcu1mc3_wrpara_reg2_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA3106000, 0xD08, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"id\": 2, \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* CANIn_tcu1mc3_wrpara_reg3_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA3106000, 0xD0C, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"id\": 3, \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* CANIn_tcu1mc3_wrpara_reg4_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA3106000, 0xD10, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"id\": 4, \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* CANIn_tcu1mc3_wrpara_reg5_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA3106000, 0xD14, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"id\": 5, \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* CANIn_tcu1mc3_wrpara_reg6_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA3106000, 0xD18, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"id\": 6, \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* CANIn_tcu1mc3_wrpara_reg7_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA3106000, 0xD1C, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"id\": 7, \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* CANIn_tcu1mc3_wrpara_reg8_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA3106000, 0xD20, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"id\": 8, \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* CANIn_tcu1mc3_wrpara_reg9_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA3106000, 0xD24, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"id\": 9, \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* CANIn_tcu1mc3_wrpara_reg10_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA3106000, 0xD28, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"id\": 10, \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* CANIn_tcu1mc3_wrpara_reg11_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA3106000, 0xD2C, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"id\": 11, \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* CANIn_tcu1mc3_wrpara_reg12_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA3106000, 0xD30, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"id\": 12, \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* CANIn_tcu1mc3_wrpara_reg13_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA3106000, 0xD34, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"id\": 13, \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* CANIn_tcu1mc3_wrpara_reg14_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA3106000, 0xD38, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"id\": 14, \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* CANIn_tcu1mc3_wrpara_reg15_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA3106000, 0xD3C, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"id\": 15, \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* CANIn_tcu1mc3_wrpara_reg16_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA3106000, 0xD40, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"id\": 16, \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* CANIn_tcu1mc3_wrpara_reg17_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA3106000, 0xD44, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"id\": 17, \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* CANIn_tcu1mc3_wrpara_reg18_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA3106000, 0xD48, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"id\": 18, \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* CANIn_tcu1mc3_wrpara_reg19_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA3106000, 0xD4C, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"id\": 19, \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* CANIn_tcu1mc3_wrpara_reg20_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA3106000, 0xD50, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"id\": 20, \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* CANIn_tcu1mc3_wrpara_reg21_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA3106000, 0xD54, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"id\": 21, \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* CANIn_tcu1mc3_wrpara_reg22_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA3106000, 0xD58, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"id\": 22, \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* CANIn_tcu1mc3_wrpara_reg23_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA3106000, 0xD5C, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"id\": 23, \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* CANIn_tcu1mc3_wrpara_reg24_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA3106000, 0xD60, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"id\": 24, \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

char* CANIn_tcu1mc3_wrpara_reg25_status(void) {

        static char buf[256];
	uint32_t value =0;
	status_code_t fault = STATUS_SUCCESS;
	
	if ( reg_read(0xA3106000, 0xD64, &value) != 0) {
		value = 0;
		fault = STATUS_ERR_HW_FAILED;
	}
    	
        snprintf(buf, sizeof(buf),
                 "{ \"id\": 25, \"status\": %d, \"value\": \"0x%08X\" }",
                 fault, value);
        return buf;
}

/* one arg API Array */
OneArgAPI one_arg_apis[] = {
	{"Enable_Line_Side", Enable_Line_Side},
	{"Cab_Active_simulation_sig", Cab_Active_simulation_sig},
	{"Cab_Active_param", Cab_Active_param}
};

/* void API Array */
VoidAPI void_apis[] = {
	{"raise_pan1_vcu2dio2_status", raise_pan1_vcu2dio2_status},
	{"raise_pan2_vcu2dio1_status", raise_pan2_vcu2dio1_status},
	{"panto_disable_vcu2dio1_status", panto_disable_vcu2dio1_status},
	{"panto_disable_vcu2dio2_status", panto_disable_vcu2dio2_status},
	{"Cab_Active_IO_Signals_status", Cab_Active_IO_Signals_status},
	{"cab1_panelA_lamp_Status", cab1_panelA_lamp_Status},
	{"cab2_panelA_lamp_Status", cab2_panelA_lamp_Status},
	{"cab1_panelC_lamp_Status", cab1_panelC_lamp_Status},
	{"cab2_panelC_lamp_Status", cab2_panelC_lamp_Status},
	{"msc_node_status", msc_node_status},
	{"fault_code_status", fault_code_status},
	{"tcu1line1_status", tcu1line1_status},
	{"tcu1line2_status", tcu1line2_status},
	{"tcu1motor1_status", tcu1motor1_status},
	{"tcu1motor2_status", tcu1motor2_status},
	{"tcu1motor3_status", tcu1motor3_status},
	{"tcu1line1_fault", tcu1line1_fault},
	{"tcu1line2_fault", tcu1line2_fault},
	{"tcu1motor1_fault", tcu1motor1_fault},
	{"tcu1motor2_fault", tcu1motor2_fault},
	{"tcu1motor3_fault", tcu1motor3_fault},
	{"tcu1line1_param_get", tcu1line1_param_get},
	{"tcu1line2_param_get", tcu1line2_param_get},
	{"tcu1motor1_param_get", tcu1motor1_param_get},
	{"tcu1motor2_param_get", tcu1motor2_param_get},
	{"tcu1motor3_param_get", tcu1motor3_param_get},
	{"MVBOut_DDU3100_1024msReg0_status", MVBOut_DDU3100_1024msReg0_status},
	{"MVBOut_DDU3104_1024msReg1_status", MVBOut_DDU3104_1024msReg1_status},
	{"MVBOut_DDU3100_1024msReg2_status", MVBOut_DDU3100_1024msReg2_status},
	{"MVBOut_DDU3100_1024msReg3_status", MVBOut_DDU3100_1024msReg3_status},
	{"MVBOut_DDU3100_1024msReg4_status", MVBOut_DDU3100_1024msReg4_status},
	{"MVBOut_DDU3100_1024msReg5_status", MVBOut_DDU3100_1024msReg5_status},
	{"MVBOut_DDU3100_1024msReg6_status", MVBOut_DDU3100_1024msReg6_status},
	{"MVBOut_DDU3100_1024msReg7_status", MVBOut_DDU3100_1024msReg7_status},
	{"MVBOut_DDU3110_1024msReg0_status", MVBOut_DDU3110_1024msReg0_status},
	{"MVBOut_DDU3110_1024msReg1_status", MVBOut_DDU3110_1024msReg1_status},
	{"MVBOut_DDU3110_1024msReg2_status", MVBOut_DDU3110_1024msReg2_status},
	{"MVBOut_DDU3110_1024msReg3_status", MVBOut_DDU3110_1024msReg3_status},
	{"MVBOut_DDU3110_1024msReg4_status", MVBOut_DDU3110_1024msReg4_status},
	{"MVBOut_DDU3110_1024msReg5_status", MVBOut_DDU3110_1024msReg5_status},
	{"MVBOut_DDU3110_1024msReg6_status", MVBOut_DDU3110_1024msReg6_status},
	{"MVBOut_DDU3300_1024msReg0_status", MVBOut_DDU3300_1024msReg0_status},
	{"MVBOut_DDU3300_1024msReg1_status", MVBOut_DDU3300_1024msReg1_status},
	{"MVBOut_DDU3400_1024msReg0_status", MVBOut_DDU3400_1024msReg0_status},
	{"MVBOut_DDU3400_1024msReg1_status", MVBOut_DDU3400_1024msReg1_status},
	{"MVBOut_DDU3500_1024msReg0_status", MVBOut_DDU3500_1024msReg0_status},
	{"MVBOut_DDU3500_1024msReg1_status", MVBOut_DDU3500_1024msReg1_status},
	{"MVBOut_DDU3600_1024msReg0_status", MVBOut_DDU3600_1024msReg0_status},
	{"MVBOut_DDU3600_1024msReg1_status", MVBOut_DDU3600_1024msReg1_status},
	{"MVBOut_DDUFaultCodeReg_status", MVBOut_DDUFaultCodeReg_status},
	{"MVBout_BUR1DDU1_1024msReg0_status", MVBout_BUR1DDU1_1024msReg0_status},
	{"MVBout_BUR1DDU1_1024msReg1_status", MVBout_BUR1DDU1_1024msReg1_status},
	{"MVBout_BUR1DDU1_1024msReg2_status", MVBout_BUR1DDU1_1024msReg2_status},
	{"MVBout_BUR1DDU1_1024msReg3_status", MVBout_BUR1DDU1_1024msReg3_status},
	{"MVBout_BUR1DDU1_1024msReg4_status", MVBout_BUR1DDU1_1024msReg4_status},
	{"MVBout_BUR2DDU1_1024msReg0_status", MVBout_BUR2DDU1_1024msReg0_status},
	{"MVBout_BUR2DDU1_1024msReg1_status", MVBout_BUR2DDU1_1024msReg1_status},
	{"MVBout_BUR2DDU1_1024msReg2_status", MVBout_BUR2DDU1_1024msReg2_status},
	{"MVBout_BUR2DDU1_1024msReg3_status", MVBout_BUR2DDU1_1024msReg3_status},
	{"MVBout_BUR2DDU1_1024msReg4_status", MVBout_BUR2DDU1_1024msReg4_status},
	{"MVBout_BUR2DDU1_1024msReg5_status", MVBout_BUR2DDU1_1024msReg5_status},
	{"MVBout_BUR2DDU1_1024msReg6_status", MVBout_BUR2DDU1_1024msReg6_status},
	{"MVBout_BUR2DDU1_1024msReg7_status", MVBout_BUR2DDU1_1024msReg7_status},
	{"MVBout_BUR3DDU1_1024msReg0_status", MVBout_BUR3DDU1_1024msReg0_status},
	{"MVBout_BUR3DDU1_1024msReg1_status", MVBout_BUR3DDU1_1024msReg1_status},
	{"MVBout_BUR3DDU1_1024msReg2_status", MVBout_BUR3DDU1_1024msReg2_status},
	{"MVBout_BUR3DDU1_1024msReg3_status", MVBout_BUR3DDU1_1024msReg3_status},
	{"MVBout_BUR3DDU1_1024msReg4_status", MVBout_BUR3DDU1_1024msReg4_status},
	{"MVBout_BUR3DDU1_1024msReg5_status", MVBout_BUR3DDU1_1024msReg5_status},
	{"MVBout_BUR3DDU1_1024msReg6_status", MVBout_BUR3DDU1_1024msReg6_status},
	{"MVBout_BUR3DDU1_1024msReg7_status", MVBout_BUR3DDU1_1024msReg7_status},
	{"MVBOut_TCU1Proc_T1000ms_DDU1Reg87_status", MVBOut_TCU1Proc_T1000ms_DDU1Reg87_status},
	{"MVBOut_TCU1Proc_T1000ms_DDU1Reg94_status", MVBOut_TCU1Proc_T1000ms_DDU1Reg94_status},
	{"MVBOut_TCU1Proc_T1000ms_DDU1Reg95_status", MVBOut_TCU1Proc_T1000ms_DDU1Reg95_status},
	{"MVBOut_TCU1Proc_T1000ms_DDU1Reg96_status", MVBOut_TCU1Proc_T1000ms_DDU1Reg96_status},
	{"MVBOut_TCU1Proc_T1000ms_DDU1Reg97_status", MVBOut_TCU1Proc_T1000ms_DDU1Reg97_status},
	{"MVBOut_TCU1Proc_T1000ms_DDU1Reg98_status", MVBOut_TCU1Proc_T1000ms_DDU1Reg98_status},
	{"MVBOut_TCU1Proc_T200ms_DDU1Reg101_status", MVBOut_TCU1Proc_T200ms_DDU1Reg101_status},
	{"MVBOut_TCU1Proc_T100ms_DDU1Reg102_status", MVBOut_TCU1Proc_T100ms_DDU1Reg102_status},
	{"MVBOut_TCU1Proc_T100ms_DDU1Reg103_status", MVBOut_TCU1Proc_T100ms_DDU1Reg103_status},
	{"MVBOut_TCU1Proc_T1000ms_DDU1Reg108_status", MVBOut_TCU1Proc_T1000ms_DDU1Reg108_status},
	{"MVBOut_TCU1Proc_T1000ms_DDU1Reg109_status", MVBOut_TCU1Proc_T1000ms_DDU1Reg109_status},
	{"MVBOut_TCU1Proc_T1000ms_DDU1Reg110_status", MVBOut_TCU1Proc_T1000ms_DDU1Reg110_status},
	{"MVBOut_TCU1Proc_T1000ms_DDU1Reg111_status", MVBOut_TCU1Proc_T1000ms_DDU1Reg111_status},
	{"MVBOut_TCU1Proc_T1000ms_DDU1Reg112_status", MVBOut_TCU1Proc_T1000ms_DDU1Reg112_status},
	{"MVBOut_TCU1Proc_T1000ms_DDU1Reg113_status", MVBOut_TCU1Proc_T1000ms_DDU1Reg113_status},
	{"MVBOut_TCU1Proc_T1000ms_DDU1Reg114_status", MVBOut_TCU1Proc_T1000ms_DDU1Reg114_status},
	{"MVBOut_TCU1Proc_T1000ms_DDU1Reg115_status", MVBOut_TCU1Proc_T1000ms_DDU1Reg115_status},
	{"MVBOut_TCU1Proc_T1000ms_DDU1Reg117_status", MVBOut_TCU1Proc_T1000ms_DDU1Reg117_status},
	{"MVBOut_TCU1Proc_T1000ms_DDU1Reg118_status", MVBOut_TCU1Proc_T1000ms_DDU1Reg118_status},
	{"MVBOut_TCU1Proc_T1000ms_DDU1Reg119_status", MVBOut_TCU1Proc_T1000ms_DDU1Reg119_status},
	{"CANIn_tcu1lc1_wrpara_reg0_status", CANIn_tcu1lc1_wrpara_reg0_status},
	{"CANIn_tcu1lc1_wrpara_reg1_status", CANIn_tcu1lc1_wrpara_reg1_status},
	{"CANIn_tcu1lc1_wrpara_reg2_status", CANIn_tcu1lc1_wrpara_reg2_status},
	{"CANIn_tcu1lc1_wrpara_reg3_status", CANIn_tcu1lc1_wrpara_reg3_status},
	{"CANIn_tcu1lc1_wrpara_reg4_status", CANIn_tcu1lc1_wrpara_reg4_status},
	{"CANIn_tcu1lc1_wrpara_reg5_status", CANIn_tcu1lc1_wrpara_reg5_status},
	{"CANIn_tcu1lc1_wrpara_reg6_status", CANIn_tcu1lc1_wrpara_reg6_status},
	{"CANIn_tcu1lc1_wrpara_reg7_status", CANIn_tcu1lc1_wrpara_reg7_status},
	{"CANIn_tcu1lc1_wrpara_reg8_status", CANIn_tcu1lc1_wrpara_reg8_status},
	{"CANIn_tcu1lc1_wrpara_reg9_status", CANIn_tcu1lc1_wrpara_reg9_status},
	{"CANIn_tcu1lc1_wrpara_reg10_status", CANIn_tcu1lc1_wrpara_reg10_status},
	{"CANIn_tcu1lc1_wrpara_reg11_status", CANIn_tcu1lc1_wrpara_reg11_status},
	{"CANIn_tcu1lc1_wrpara_reg12_status", CANIn_tcu1lc1_wrpara_reg12_status},
	{"CANIn_tcu1lc1_wrpara_reg13_status", CANIn_tcu1lc1_wrpara_reg13_status},
	{"CANIn_tcu1lc1_wrpara_reg14_status", CANIn_tcu1lc1_wrpara_reg14_status},
	{"CANIn_tcu1lc1_wrpara_reg15_status", CANIn_tcu1lc1_wrpara_reg15_status},
	{"CANIn_tcu1lc1_wrpara_reg16_status", CANIn_tcu1lc1_wrpara_reg16_status},
	{"CANIn_tcu1lc1_wrpara_reg17_status", CANIn_tcu1lc1_wrpara_reg17_status},
	{"CANIn_tcu1lc1_wrpara_reg18_status", CANIn_tcu1lc1_wrpara_reg18_status},
	{"CANIn_tcu1lc1_wrpara_reg19_status", CANIn_tcu1lc1_wrpara_reg19_status},
	{"CANIn_tcu1lc1_wrpara_reg20_status", CANIn_tcu1lc1_wrpara_reg20_status},
	{"CANIn_tcu1lc1_wrpara_reg21_status", CANIn_tcu1lc1_wrpara_reg21_status},
	{"CANIn_tcu1lc1_wrpara_reg22_status", CANIn_tcu1lc1_wrpara_reg22_status},
	{"CANIn_tcu1lc1_wrpara_reg23_status", CANIn_tcu1lc1_wrpara_reg23_status},
	{"CANIn_tcu1lc1_wrpara_reg24_status", CANIn_tcu1lc1_wrpara_reg24_status},
	{"CANIn_tcu1lc1_wrpara_reg25_status", CANIn_tcu1lc1_wrpara_reg25_status},
	{"CANIn_tcu1lc2_wrpara_reg0_status", CANIn_tcu1lc2_wrpara_reg0_status},
	{"CANIn_tcu1lc2_wrpara_reg1_status", CANIn_tcu1lc2_wrpara_reg1_status},
	{"CANIn_tcu1lc2_wrpara_reg2_status", CANIn_tcu1lc2_wrpara_reg2_status},
	{"CANIn_tcu1lc2_wrpara_reg3_status", CANIn_tcu1lc2_wrpara_reg3_status},
	{"CANIn_tcu1lc2_wrpara_reg4_status", CANIn_tcu1lc2_wrpara_reg4_status},
	{"CANIn_tcu1lc2_wrpara_reg5_status", CANIn_tcu1lc2_wrpara_reg5_status},
	{"CANIn_tcu1lc2_wrpara_reg6_status", CANIn_tcu1lc2_wrpara_reg6_status},
	{"CANIn_tcu1lc2_wrpara_reg7_status", CANIn_tcu1lc2_wrpara_reg7_status},
	{"CANIn_tcu1lc2_wrpara_reg8_status", CANIn_tcu1lc2_wrpara_reg8_status},
	{"CANIn_tcu1lc2_wrpara_reg9_status", CANIn_tcu1lc2_wrpara_reg9_status},
	{"CANIn_tcu1lc2_wrpara_reg10_status", CANIn_tcu1lc2_wrpara_reg10_status},
	{"CANIn_tcu1lc2_wrpara_reg11_status", CANIn_tcu1lc2_wrpara_reg11_status},
	{"CANIn_tcu1lc2_wrpara_reg12_status", CANIn_tcu1lc2_wrpara_reg12_status},
	{"CANIn_tcu1lc2_wrpara_reg13_status", CANIn_tcu1lc2_wrpara_reg13_status},
	{"CANIn_tcu1lc2_wrpara_reg14_status", CANIn_tcu1lc2_wrpara_reg14_status},
	{"CANIn_tcu1lc2_wrpara_reg15_status", CANIn_tcu1lc2_wrpara_reg15_status},
	{"CANIn_tcu1lc2_wrpara_reg16_status", CANIn_tcu1lc2_wrpara_reg16_status},
	{"CANIn_tcu1lc2_wrpara_reg17_status", CANIn_tcu1lc2_wrpara_reg17_status},
	{"CANIn_tcu1lc2_wrpara_reg18_status", CANIn_tcu1lc2_wrpara_reg18_status},
	{"CANIn_tcu1lc2_wrpara_reg19_status", CANIn_tcu1lc2_wrpara_reg19_status},
	{"CANIn_tcu1lc2_wrpara_reg20_status", CANIn_tcu1lc2_wrpara_reg20_status},
	{"CANIn_tcu1lc2_wrpara_reg21_status", CANIn_tcu1lc2_wrpara_reg21_status},
	{"CANIn_tcu1lc2_wrpara_reg22_status", CANIn_tcu1lc2_wrpara_reg22_status},
	{"CANIn_tcu1lc2_wrpara_reg23_status", CANIn_tcu1lc2_wrpara_reg23_status},
	{"CANIn_tcu1lc2_wrpara_reg24_status", CANIn_tcu1lc2_wrpara_reg24_status},
	{"CANIn_tcu1lc2_wrpara_reg25_status", CANIn_tcu1lc2_wrpara_reg25_status},
	{"CANIn_tcu1mc1_wrpara_reg0_status", CANIn_tcu1mc1_wrpara_reg0_status},
	{"CANIn_tcu1mc1_wrpara_reg1_status", CANIn_tcu1mc1_wrpara_reg1_status},
	{"CANIn_tcu1mc1_wrpara_reg2_status", CANIn_tcu1mc1_wrpara_reg2_status},
	{"CANIn_tcu1mc1_wrpara_reg3_status", CANIn_tcu1mc1_wrpara_reg3_status},
	{"CANIn_tcu1mc1_wrpara_reg4_status", CANIn_tcu1mc1_wrpara_reg4_status},
	{"CANIn_tcu1mc1_wrpara_reg5_status", CANIn_tcu1mc1_wrpara_reg5_status},
	{"CANIn_tcu1mc1_wrpara_reg6_status", CANIn_tcu1mc1_wrpara_reg6_status},
	{"CANIn_tcu1mc1_wrpara_reg7_status", CANIn_tcu1mc1_wrpara_reg7_status},
	{"CANIn_tcu1mc1_wrpara_reg8_status", CANIn_tcu1mc2_wrpara_reg8_status},
	{"CANIn_tcu1mc1_wrpara_reg9_status", CANIn_tcu1mc1_wrpara_reg9_status},
	{"CANIn_tcu1mc1_wrpara_reg10_status", CANIn_tcu1mc1_wrpara_reg10_status},
	{"CANIn_tcu1mc1_wrpara_reg11_status", CANIn_tcu1mc1_wrpara_reg11_status},
	{"CANIn_tcu1mc1_wrpara_reg12_status", CANIn_tcu1mc1_wrpara_reg12_status},
	{"CANIn_tcu1mc1_wrpara_reg13_status", CANIn_tcu1mc1_wrpara_reg13_status},
	{"CANIn_tcu1mc1_wrpara_reg14_status", CANIn_tcu1mc1_wrpara_reg14_status},
	{"CANIn_tcu1mc1_wrpara_reg15_status", CANIn_tcu1mc1_wrpara_reg15_status},
	{"CANIn_tcu1mc1_wrpara_reg16_status", CANIn_tcu1mc1_wrpara_reg16_status},
	{"CANIn_tcu1mc1_wrpara_reg17_status", CANIn_tcu1mc1_wrpara_reg17_status},
	{"CANIn_tcu1mc1_wrpara_reg18_status", CANIn_tcu1mc1_wrpara_reg18_status},
	{"CANIn_tcu1mc1_wrpara_reg19_status", CANIn_tcu1mc1_wrpara_reg19_status},
	{"CANIn_tcu1mc1_wrpara_reg20_status", CANIn_tcu1mc1_wrpara_reg20_status},
	{"CANIn_tcu1mc1_wrpara_reg21_status", CANIn_tcu1mc1_wrpara_reg21_status},
	{"CANIn_tcu1mc1_wrpara_reg22_status", CANIn_tcu1mc1_wrpara_reg22_status},
	{"CANIn_tcu1mc1_wrpara_reg23_status", CANIn_tcu1mc1_wrpara_reg23_status},
	{"CANIn_tcu1mc1_wrpara_reg24_status", CANIn_tcu1mc1_wrpara_reg24_status},
	{"CANIn_tcu1mc1_wrpara_reg25_status", CANIn_tcu1mc1_wrpara_reg25_status},
	{"CANIn_tcu1mc2_wrpara_reg0_status", CANIn_tcu1mc2_wrpara_reg0_status},
	{"CANIn_tcu1mc2_wrpara_reg1_status", CANIn_tcu1mc2_wrpara_reg1_status},
	{"CANIn_tcu1mc2_wrpara_reg2_status", CANIn_tcu1mc2_wrpara_reg2_status},
	{"CANIn_tcu1mc2_wrpara_reg3_status", CANIn_tcu1mc2_wrpara_reg3_status},
	{"CANIn_tcu1mc2_wrpara_reg4_status", CANIn_tcu1mc2_wrpara_reg4_status},
	{"CANIn_tcu1mc2_wrpara_reg5_status", CANIn_tcu1mc2_wrpara_reg5_status},
	{"CANIn_tcu1mc2_wrpara_reg6_status", CANIn_tcu1mc2_wrpara_reg6_status},
	{"CANIn_tcu1mc2_wrpara_reg7_status", CANIn_tcu1mc2_wrpara_reg7_status},
	{"CANIn_tcu1mc2_wrpara_reg8_status", CANIn_tcu1mc2_wrpara_reg8_status},
	{"CANIn_tcu1mc2_wrpara_reg9_status", CANIn_tcu1mc2_wrpara_reg9_status},
	{"CANIn_tcu1mc2_wrpara_reg10_status", CANIn_tcu1mc2_wrpara_reg10_status},
	{"CANIn_tcu1mc2_wrpara_reg11_status", CANIn_tcu1mc2_wrpara_reg11_status},
	{"CANIn_tcu1mc2_wrpara_reg12_status", CANIn_tcu1mc2_wrpara_reg12_status},
	{"CANIn_tcu1mc2_wrpara_reg13_status", CANIn_tcu1mc2_wrpara_reg13_status},
	{"CANIn_tcu1mc2_wrpara_reg14_status", CANIn_tcu1mc2_wrpara_reg14_status},
	{"CANIn_tcu1mc2_wrpara_reg15_status", CANIn_tcu1mc2_wrpara_reg15_status},
	{"CANIn_tcu1mc2_wrpara_reg16_status", CANIn_tcu1mc2_wrpara_reg16_status},
	{"CANIn_tcu1mc2_wrpara_reg17_status", CANIn_tcu1mc2_wrpara_reg17_status},
	{"CANIn_tcu1mc2_wrpara_reg18_status", CANIn_tcu1mc2_wrpara_reg18_status},
	{"CANIn_tcu1mc2_wrpara_reg19_status", CANIn_tcu1mc2_wrpara_reg19_status},
	{"CANIn_tcu1mc2_wrpara_reg20_status", CANIn_tcu1mc2_wrpara_reg20_status},
	{"CANIn_tcu1mc2_wrpara_reg21_status", CANIn_tcu1mc2_wrpara_reg21_status},
	{"CANIn_tcu1mc2_wrpara_reg22_status", CANIn_tcu1mc2_wrpara_reg22_status},
	{"CANIn_tcu1mc2_wrpara_reg23_status", CANIn_tcu1mc2_wrpara_reg23_status},
	{"CANIn_tcu1mc2_wrpara_reg24_status", CANIn_tcu1mc2_wrpara_reg24_status},
	{"CANIn_tcu1mc2_wrpara_reg25_status", CANIn_tcu1mc2_wrpara_reg25_status},
	{"CANIn_tcu1mc3_wrpara_reg0_status", CANIn_tcu1mc3_wrpara_reg0_status},
	{"CANIn_tcu1mc3_wrpara_reg1_status", CANIn_tcu1mc3_wrpara_reg1_status},
	{"CANIn_tcu1mc3_wrpara_reg2_status", CANIn_tcu1mc3_wrpara_reg2_status},
	{"CANIn_tcu1mc3_wrpara_reg3_status", CANIn_tcu1mc3_wrpara_reg3_status},
	{"CANIn_tcu1mc3_wrpara_reg4_status", CANIn_tcu1mc3_wrpara_reg4_status},
	{"CANIn_tcu1mc3_wrpara_reg5_status", CANIn_tcu1mc3_wrpara_reg5_status},
	{"CANIn_tcu1mc3_wrpara_reg6_status", CANIn_tcu1mc3_wrpara_reg6_status},
	{"CANIn_tcu1mc3_wrpara_reg7_status", CANIn_tcu1mc3_wrpara_reg7_status},
	{"CANIn_tcu1mc3_wrpara_reg8_status", CANIn_tcu1mc3_wrpara_reg8_status},
	{"CANIn_tcu1mc3_wrpara_reg9_status", CANIn_tcu1mc3_wrpara_reg9_status},
	{"CANIn_tcu1mc3_wrpara_reg10_status", CANIn_tcu1mc3_wrpara_reg10_status},
	{"CANIn_tcu1mc3_wrpara_reg11_status", CANIn_tcu1mc3_wrpara_reg11_status},
	{"CANIn_tcu1mc3_wrpara_reg12_status", CANIn_tcu1mc3_wrpara_reg12_status},
	{"CANIn_tcu1mc3_wrpara_reg13_status", CANIn_tcu1mc3_wrpara_reg13_status},
	{"CANIn_tcu1mc3_wrpara_reg14_status", CANIn_tcu1mc3_wrpara_reg14_status},
	{"CANIn_tcu1mc3_wrpara_reg15_status", CANIn_tcu1mc3_wrpara_reg15_status},
	{"CANIn_tcu1mc3_wrpara_reg16_status", CANIn_tcu1mc3_wrpara_reg16_status},
	{"CANIn_tcu1mc3_wrpara_reg17_status", CANIn_tcu1mc3_wrpara_reg17_status},
	{"CANIn_tcu1mc3_wrpara_reg18_status", CANIn_tcu1mc3_wrpara_reg18_status},
	{"CANIn_tcu1mc3_wrpara_reg19_status", CANIn_tcu1mc3_wrpara_reg19_status},
	{"CANIn_tcu1mc3_wrpara_reg20_status", CANIn_tcu1mc3_wrpara_reg20_status},
	{"CANIn_tcu1mc3_wrpara_reg21_status", CANIn_tcu1mc3_wrpara_reg21_status},
	{"CANIn_tcu1mc3_wrpara_reg22_status", CANIn_tcu1mc3_wrpara_reg22_status},
	{"CANIn_tcu1mc3_wrpara_reg23_status", CANIn_tcu1mc3_wrpara_reg23_status},
	{"CANIn_tcu1mc3_wrpara_reg24_status", CANIn_tcu1mc3_wrpara_reg24_status},
	{"CANIn_tcu1mc3_wrpara_reg25_status", CANIn_tcu1mc3_wrpara_reg25_status}
	
};

/* two arg API Array */ 
TwoArgAPI two_arg_apis[] = {
	{"update_signal_xml", update_signal_xml},
	{"register_update", register_update},
	{"fg88_motxtemp_uplwlimit", fg88_motxtemp_uplwlimit},
	{"fg_88878685_reg", fg_88878685_reg},
	{"fg88_878685_reg", fg88_878685_reg},
	{"fg87_dcinterlink_dclink_volt", fg87_dcinterlink_dclink_volt},
	{"fg87_dclink_primvolt_minmax", fg87_dclink_primvolt_minmax},
	{"fg08_asc_deact", fg08_asc_deact},
	{"fg18_870991_reg", fg18_870991_reg},
	{"fg09_18870991_reg", fg09_18870991_reg},
	{"fg09_identprojtemp_oillmt", fg09_identprojtemp_oillmt},
	{"fg09_srpwr_rdctn_startend", fg09_srpwr_rdctn_startend},
	{"fg09_troiltemp_lmtpwr_rdctn_srt", fg09_troiltemp_lmtpwr_rdctn_srt},
	{"fg09_trpwr_rdctn_oiltemp_endtsk_time1", fg09_trpwr_rdctn_oiltemp_endtsk_time1},
	{"fg09_trpa_maxlimit_mottemp_lmt", fg09_trpa_maxlimit_mottemp_lmt},
	{"fg09_motlower_temptask_timelimit", fg09_motlower_temptask_timelimit},
	{"fg09_mottemp_lvl1levl2", fg09_mottemp_lvl1levl2},
	{"fg09_mottemp_lvl3trtemplvl3", fg09_mottemp_lvl3trtemplvl3},
	{"fg09_trtemp_lvl1levl2", fg09_trtemp_lvl1levl2},
	{"fg09_srtemp_lvl1levl2", fg09_srtemp_lvl1levl2},
	{"fg09_srtemplvl3_swtimefor_lvl2t0lvl3", fg09_srtemplvl3_swtimefor_lvl2t0lvl3},
	{"fg09_pwrconv_templimit_uprvolt_lmt1", fg09_pwrconv_templimit_uprvolt_lmt1},
	{"fg09_uprvolt_limit2_2bhoge_opcrnt", fg09_uprvolt_limit2_2bhoge_opcrnt},
	{"fg09_pri_voltlimit_minvoltlimit", fg09_pri_voltlimit_minvoltlimit},
	{"fg09_inrush_crntlimit_pwrcon_tasktime", fg09_inrush_crntlimit_pwrcon_tasktime},
	{"fg09_inrush_crnttime_limithotelid_maxlim", fg09_inrush_crnttime_limithotelid_maxlim},
	{"fg09_wttime_forvcb0_zsslimit_burmx_crntlim", fg09_wttime_forvcb0_zsslimit_burmx_crntlim},
	{"fg09_wttimeforbur_vcbonhrmnic_liltermx_crnt", fg09_wttimeforbur_vcbonhrmnic_liltermx_crnt},
	{"fg09_maxdc_intvolt_cocohrmnc_fltrinrsh_crnt", fg09_maxdc_intvolt_cocohrmnc_fltrinrsh_crnt},
	{"fg09_dcinrlnk_lmt1", fg09_dcinrlnk_lmt1},
	{"fg09_dcinrlnk_lmt2", fg09_dcinrlnk_lmt2},
	{"fg09_dcinrlnk_lmt3", fg09_dcinrlnk_lmt3},
	{"fg09_grndtimelmt_intrmddclink_chrgiglmt", fg09_grndtimelmt_intrmddclink_chrgiglmt},
	{"fg09_tnfrratio_prmyvolt_factlmt", fg09_tnfrratio_prmyvolt_factlmt},
	{"fg09_spdsnrp7_grratio_avgwhl_dia", fg09_spdsnrp7_grratio_avgwhl_dia},
	{"fg09_mxspeed_lmtp5g9", fg09_mxspeed_lmtp5g9},
	{"fg09_mxtime_lmtfr_maxspd_GWvmaxCoP7", fg09_mxtime_lmtfr_maxspd_GWvmaxCoP7},
	{"fg09_tebe_motg9_mot1lmt", fg09_tebe_motg9_mot1lmt},
	{"fg09_tebe_loco_p7lmtg9", fg09_tebe_loco_p7lmtg9},
	{"fg09_spdventi_onhylmt", fg09_spdventi_onhylmt},
	{"fg09_pwrrdn_hyventi_lmt", fg09_pwrrdn_hyventi_lmt},
	{"fg08_09_reg", fg08_09_reg},
	{"fg09_mot1mot2_crntlmt", fg09_mot1mot2_crntlmt},
	{"fg09_mot3_crntlmt_igbttmp", fg09_mot3_crntlmt_igbttmp},
	{"igbt_temp_lmt1", igbt_temp_lmt1},
	{"igbt_temp_lmt2", igbt_temp_lmt2},
	{"igbt_temp_lmt3", igbt_temp_lmt3},
	{"igbt_temp_lmt4", igbt_temp_lmt4},
	{"fg91_pwradjust_fctr", fg91_pwradjust_fctr},
	{"fg91_DGDGC_max_brkeffort", fg91_DGDGC_max_brkeffort},
	{"fg91_080591_reg", fg91_080591_reg},
	{"fg91_18870991_reg", fg91_18870991_reg},
	{"CANout_tcu1lc1_wrpara_reg0", CANout_tcu1lc1_wrpara_reg0},
	{"CANout_tcu1lc1_wrpara_reg1", CANout_tcu1lc1_wrpara_reg1},
	{"CANout_tcu1lc1_wrpara_reg2", CANout_tcu1lc1_wrpara_reg2},
	{"CANout_tcu1lc1_wrpara_reg3", CANout_tcu1lc1_wrpara_reg3},
	{"CANout_tcu1lc1_wrpara_reg4", CANout_tcu1lc1_wrpara_reg4},
	{"CANout_tcu1lc1_wrpara_reg5", CANout_tcu1lc1_wrpara_reg5},
	{"CANout_tcu1lc1_wrpara_reg6", CANout_tcu1lc1_wrpara_reg6},
	{"CANout_tcu1lc1_wrpara_reg7", CANout_tcu1lc1_wrpara_reg7},
	{"CANout_tcu1lc1_wrpara_reg8", CANout_tcu1lc1_wrpara_reg8},
	{"CANout_tcu1lc1_wrpara_reg9", CANout_tcu1lc1_wrpara_reg9},
	{"CANout_tcu1lc1_wrpara_reg10", CANout_tcu1lc1_wrpara_reg10},
	{"CANout_tcu1lc1_wrpara_reg11", CANout_tcu1lc1_wrpara_reg11},
	{"CANout_tcu1lc1_wrpara_reg12", CANout_tcu1lc1_wrpara_reg12},
	{"CANout_tcu1lc1_wrpara_reg13", CANout_tcu1lc1_wrpara_reg13},
	{"CANout_tcu1lc1_wrpara_reg14", CANout_tcu1lc1_wrpara_reg14},
	{"CANout_tcu1lc1_wrpara_reg15", CANout_tcu1lc1_wrpara_reg15},
	{"CANout_tcu1lc1_wrpara_reg16", CANout_tcu1lc1_wrpara_reg16},
	{"CANout_tcu1lc1_wrpara_reg17", CANout_tcu1lc1_wrpara_reg17},
	{"CANout_tcu1lc1_wrpara_reg18", CANout_tcu1lc1_wrpara_reg18},
	{"CANout_tcu1lc1_wrpara_reg19", CANout_tcu1lc1_wrpara_reg19},
	{"CANout_tcu1lc1_wrpara_reg20", CANout_tcu1lc1_wrpara_reg20},
	{"CANout_tcu1lc1_wrpara_reg21", CANout_tcu1lc1_wrpara_reg21},
	{"CANout_tcu1lc1_wrpara_reg22", CANout_tcu1lc1_wrpara_reg22},
	{"CANout_tcu1lc1_wrpara_reg23", CANout_tcu1lc1_wrpara_reg23},
	{"CANout_tcu1lc1_wrpara_reg24", CANout_tcu1lc1_wrpara_reg24},
	{"CANout_tcu1lc1_wrpara_reg25", CANout_tcu1lc1_wrpara_reg25},
	{"CANout_tcu1lc2_wrpara_reg0", CANout_tcu1lc2_wrpara_reg0},
	{"CANout_tcu1lc2_wrpara_reg1", CANout_tcu1lc2_wrpara_reg1},
	{"CANout_tcu1lc2_wrpara_reg2", CANout_tcu1lc2_wrpara_reg2},
	{"CANout_tcu1lc2_wrpara_reg3", CANout_tcu1lc2_wrpara_reg3},
	{"CANout_tcu1lc2_wrpara_reg4", CANout_tcu1lc2_wrpara_reg4},
	{"CANout_tcu1lc2_wrpara_reg5", CANout_tcu1lc2_wrpara_reg5},
	{"CANout_tcu1lc2_wrpara_reg6", CANout_tcu1lc2_wrpara_reg6},
	{"CANout_tcu1lc2_wrpara_reg7", CANout_tcu1lc2_wrpara_reg7},
	{"CANout_tcu1lc2_wrpara_reg8", CANout_tcu1lc2_wrpara_reg8},
	{"CANout_tcu1lc2_wrpara_reg9", CANout_tcu1lc2_wrpara_reg9},
	{"CANout_tcu1lc2_wrpara_reg10", CANout_tcu1lc2_wrpara_reg10},
	{"CANout_tcu1lc2_wrpara_reg11", CANout_tcu1lc2_wrpara_reg11},
	{"CANout_tcu1lc2_wrpara_reg12", CANout_tcu1lc2_wrpara_reg12},
	{"CANout_tcu1lc2_wrpara_reg13", CANout_tcu1lc2_wrpara_reg13},
	{"CANout_tcu1lc2_wrpara_reg14", CANout_tcu1lc2_wrpara_reg14},
	{"CANout_tcu1lc2_wrpara_reg15", CANout_tcu1lc2_wrpara_reg15},
	{"CANout_tcu1lc2_wrpara_reg16", CANout_tcu1lc2_wrpara_reg16},
	{"CANout_tcu1lc2_wrpara_reg17", CANout_tcu1lc2_wrpara_reg17},
	{"CANout_tcu1lc2_wrpara_reg18", CANout_tcu1lc2_wrpara_reg18},
	{"CANout_tcu1lc2_wrpara_reg19", CANout_tcu1lc2_wrpara_reg19},
	{"CANout_tcu1lc2_wrpara_reg20", CANout_tcu1lc2_wrpara_reg20},
	{"CANout_tcu1lc2_wrpara_reg21", CANout_tcu1lc2_wrpara_reg21},
	{"CANout_tcu1lc2_wrpara_reg22", CANout_tcu1lc2_wrpara_reg22},
	{"CANout_tcu1lc2_wrpara_reg23", CANout_tcu1lc2_wrpara_reg23},
	{"CANout_tcu1lc2_wrpara_reg24", CANout_tcu1lc2_wrpara_reg24},
	{"CANout_tcu1lc2_wrpara_reg25", CANout_tcu1lc2_wrpara_reg25},
	{"CANout_tcu1mc1_wrpara_reg0", CANout_tcu1mc1_wrpara_reg0},
	{"CANout_tcu1mc1_wrpara_reg1", CANout_tcu1mc1_wrpara_reg1},
	{"CANout_tcu1mc1_wrpara_reg2", CANout_tcu1mc1_wrpara_reg2},
	{"CANout_tcu1mc1_wrpara_reg3", CANout_tcu1mc1_wrpara_reg3},
	{"CANout_tcu1mc1_wrpara_reg4", CANout_tcu1mc1_wrpara_reg4},
	{"CANout_tcu1mc1_wrpara_reg5", CANout_tcu1mc1_wrpara_reg5},
	{"CANout_tcu1mc1_wrpara_reg6", CANout_tcu1mc1_wrpara_reg6},
	{"CANout_tcu1mc1_wrpara_reg7", CANout_tcu1mc1_wrpara_reg7},
	{"CANout_tcu1mc1_wrpara_reg8", CANout_tcu1mc1_wrpara_reg8},
	{"CANout_tcu1mc1_wrpara_reg9", CANout_tcu1mc1_wrpara_reg9},
	{"CANout_tcu1mc1_wrpara_reg10", CANout_tcu1mc1_wrpara_reg10},
	{"CANout_tcu1mc1_wrpara_reg11", CANout_tcu1mc1_wrpara_reg11},
	{"CANout_tcu1mc1_wrpara_reg12", CANout_tcu1mc1_wrpara_reg12},
	{"CANout_tcu1mc1_wrpara_reg13", CANout_tcu1mc1_wrpara_reg13},
	{"CANout_tcu1mc1_wrpara_reg14", CANout_tcu1mc1_wrpara_reg14},
	{"CANout_tcu1mc1_wrpara_reg15", CANout_tcu1mc1_wrpara_reg15},
	{"CANout_tcu1mc1_wrpara_reg16", CANout_tcu1mc1_wrpara_reg16},
	{"CANout_tcu1mc1_wrpara_reg17", CANout_tcu1mc1_wrpara_reg17},
	{"CANout_tcu1mc1_wrpara_reg18", CANout_tcu1mc1_wrpara_reg18},
	{"CANout_tcu1mc1_wrpara_reg19", CANout_tcu1mc1_wrpara_reg19},
	{"CANout_tcu1mc1_wrpara_reg20", CANout_tcu1mc1_wrpara_reg20},
	{"CANout_tcu1mc1_wrpara_reg21", CANout_tcu1mc1_wrpara_reg21},
	{"CANout_tcu1mc1_wrpara_reg22", CANout_tcu1mc1_wrpara_reg22},
	{"CANout_tcu1mc1_wrpara_reg23", CANout_tcu1mc1_wrpara_reg23},
	{"CANout_tcu1mc1_wrpara_reg24", CANout_tcu1mc1_wrpara_reg24},
	{"CANout_tcu1mc1_wrpara_reg25", CANout_tcu1mc1_wrpara_reg25},
	{"CANout_tcu1mc2_wrpara_reg0", CANout_tcu1mc2_wrpara_reg0},
	{"CANout_tcu1mc2_wrpara_reg1", CANout_tcu1mc2_wrpara_reg1},
	{"CANout_tcu1mc2_wrpara_reg2", CANout_tcu1mc2_wrpara_reg2},
	{"CANout_tcu1mc2_wrpara_reg3", CANout_tcu1mc2_wrpara_reg3},
	{"CANout_tcu1mc2_wrpara_reg4", CANout_tcu1mc2_wrpara_reg4},
	{"CANout_tcu1mc2_wrpara_reg5", CANout_tcu1mc2_wrpara_reg5},
	{"CANout_tcu1mc2_wrpara_reg6", CANout_tcu1mc2_wrpara_reg6},
	{"CANout_tcu1mc2_wrpara_reg7", CANout_tcu1mc2_wrpara_reg7},
	{"CANout_tcu1mc2_wrpara_reg8", CANout_tcu1mc2_wrpara_reg8},
	{"CANout_tcu1mc2_wrpara_reg9", CANout_tcu1mc2_wrpara_reg9},
	{"CANout_tcu1mc2_wrpara_reg10", CANout_tcu1mc2_wrpara_reg10},
	{"CANout_tcu1mc2_wrpara_reg11", CANout_tcu1mc2_wrpara_reg11},
	{"CANout_tcu1mc2_wrpara_reg12", CANout_tcu1mc2_wrpara_reg12},
	{"CANout_tcu1mc2_wrpara_reg13", CANout_tcu1mc2_wrpara_reg13},
	{"CANout_tcu1mc2_wrpara_reg14", CANout_tcu1mc2_wrpara_reg14},
	{"CANout_tcu1mc2_wrpara_reg15", CANout_tcu1mc2_wrpara_reg15},
	{"CANout_tcu1mc2_wrpara_reg16", CANout_tcu1mc2_wrpara_reg16},
	{"CANout_tcu1mc2_wrpara_reg17", CANout_tcu1mc2_wrpara_reg17},
	{"CANout_tcu1mc2_wrpara_reg18", CANout_tcu1mc2_wrpara_reg18},
	{"CANout_tcu1mc2_wrpara_reg19", CANout_tcu1mc2_wrpara_reg19},
	{"CANout_tcu1mc2_wrpara_reg20", CANout_tcu1mc2_wrpara_reg20},
	{"CANout_tcu1mc2_wrpara_reg21", CANout_tcu1mc2_wrpara_reg21},
	{"CANout_tcu1mc2_wrpara_reg22", CANout_tcu1mc2_wrpara_reg22},
	{"CANout_tcu1mc2_wrpara_reg23", CANout_tcu1mc2_wrpara_reg23},
	{"CANout_tcu1mc2_wrpara_reg24", CANout_tcu1mc2_wrpara_reg24},
	{"CANout_tcu1mc2_wrpara_reg25", CANout_tcu1mc2_wrpara_reg25},
	{"CANout_tcu1mc3_wrpara_reg0", CANout_tcu1mc3_wrpara_reg0},
	{"CANout_tcu1mc3_wrpara_reg1", CANout_tcu1mc3_wrpara_reg1},
	{"CANout_tcu1mc3_wrpara_reg2", CANout_tcu1mc3_wrpara_reg2},
	{"CANout_tcu1mc3_wrpara_reg3", CANout_tcu1mc3_wrpara_reg3},
	{"CANout_tcu1mc3_wrpara_reg4", CANout_tcu1mc3_wrpara_reg4},
	{"CANout_tcu1mc3_wrpara_reg5", CANout_tcu1mc3_wrpara_reg5},
	{"CANout_tcu1mc3_wrpara_reg6", CANout_tcu1mc3_wrpara_reg6},
	{"CANout_tcu1mc3_wrpara_reg7", CANout_tcu1mc3_wrpara_reg7},
	{"CANout_tcu1mc3_wrpara_reg8", CANout_tcu1mc3_wrpara_reg8},
	{"CANout_tcu1mc3_wrpara_reg9", CANout_tcu1mc3_wrpara_reg9},
	{"CANout_tcu1mc3_wrpara_reg10", CANout_tcu1mc3_wrpara_reg10},
	{"CANout_tcu1mc3_wrpara_reg11", CANout_tcu1mc3_wrpara_reg11},
	{"CANout_tcu1mc3_wrpara_reg12", CANout_tcu1mc3_wrpara_reg12},
	{"CANout_tcu1mc3_wrpara_reg13", CANout_tcu1mc3_wrpara_reg13},
	{"CANout_tcu1mc3_wrpara_reg14", CANout_tcu1mc3_wrpara_reg14},
	{"CANout_tcu1mc3_wrpara_reg15", CANout_tcu1mc3_wrpara_reg15},
	{"CANout_tcu1mc3_wrpara_reg16", CANout_tcu1mc3_wrpara_reg16},
	{"CANout_tcu1mc3_wrpara_reg17", CANout_tcu1mc3_wrpara_reg17},
	{"CANout_tcu1mc3_wrpara_reg18", CANout_tcu1mc3_wrpara_reg18},
	{"CANout_tcu1mc3_wrpara_reg19", CANout_tcu1mc3_wrpara_reg19},
	{"CANout_tcu1mc3_wrpara_reg20", CANout_tcu1mc3_wrpara_reg20},
	{"CANout_tcu1mc3_wrpara_reg21", CANout_tcu1mc3_wrpara_reg21},
	{"CANout_tcu1mc3_wrpara_reg22", CANout_tcu1mc3_wrpara_reg22},
	{"CANout_tcu1mc3_wrpara_reg23", CANout_tcu1mc3_wrpara_reg23},
	{"CANout_tcu1mc3_wrpara_reg24", CANout_tcu1mc3_wrpara_reg24},
	{"CANout_tcu1mc3_wrpara_reg25", CANout_tcu1mc3_wrpara_reg25}
};

/* three arg API Array */ 
ThreeArgAPI three_arg_apis[] = {
	{"reg_dump", reg_dump}
};

uint32_t VoidApi_count = sizeof(void_apis)/sizeof(VoidAPI);
uint32_t OneArgApi_count = sizeof(one_arg_apis)/sizeof(OneArgAPI);
uint32_t TwoArgApi_count = sizeof(two_arg_apis)/sizeof(TwoArgAPI);
uint32_t ThreeArgApi_count = sizeof(three_arg_apis)/sizeof(ThreeArgAPI);
