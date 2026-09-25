## what is inline use in function 

The keyword inline is a directive that tells the compiler to replace the function call directly with the function's actual code, right where it is called.
Instead of jumping to a separate memory location to run the function, the compiler inserts the loop logic directly into your workflow. 
This eliminates the overhead of a traditional function call.

When you call a normal function, the CPU must perform several steps:
1) Push the arguments (num and digit) onto the stack.
2) Jump to the memory address of function.
3) Execute the function code.
4) Pop the return value from the stack.
5) Jump back to where it left off.

When you add inline, the compiler removes steps 1, 2, 4, and 5. It turns your code into this at compile time:

<> Normal Function Call (Separate Frames):
|--------------------------|
| main() stack frame       | <-- Paused here
|--------------------------|
| mul_func() stack frame   | <-- CPU jumps here, creates new variables,
|                          |     deletes them, then jumps back.
|--------------------------|

<> Inline Function (Single Blended Frame):

|--------------------------|
| main() stack frame       | <-- Execution never leaves this box.
|   - main variables       |     The inline code runs right here.
|   - inline variables     |     as if you typed it out manually.
|--------------------------|

## how bellow program executes 

#include <stdio.h>

/* this function is for swaping the bits in number at given position */
int swap_bits(int num,int p,int q)
{
	if((num>>p&1)^(num>>q&1)){ /* compare the bits at p and q position is differ or same if differ then swap otherwise dont swap*/
		num=num^1<<p;
		num=num^1<<q;
	}
	return num;
}

/* this function is for printing the bits of given number */
void printbits(int num)
{
	int pos;
	for(pos=31;pos>=0;pos--)
		printf("%d",num>>pos&1);
	printf("\n");
}

int main()
{

	int num=31,ret;
	int p=5,q=6;  /* 5nd and 6th bit  in given number*/
	printbits(num);
	ret=swap_bits(num,p,q);
	printbits(ret);
	
}

PHASE -1:

1. Memory Allocation: The stack pointer moves to carve out space for main's local variables: num, ret, p, and q.Value 
2. Initialization: The values are written into main's frame space.

| main() Stack Frame                |
|   - num = 31  (0x0000001F)        |
|   - p   = 5   (0x00000005)        |
|   - q   = 6   (0x00000006)        |
|   - ret = [Uninitialized/Garbage] |
+-----------------------------------+ <--- Stack Pointer (SP)

PHASE -2: Calling printbits(num)

1. Passing Arguments: On ARM, the value of num (31) is copied directly into a CPU hardware register (usually R0 or X0) instead of being pushed to the stack to save time.
2. Saving the Return Address: The address of the next instruction in main (which is ret=swap_bits(...)) is saved into the CPU's Link Register (LR). This tells the CPU where to return when printbits finishes.
3. Allocation: The CPU jumps to printbits, and a new stack frame is created on top of main. Inside it, a local copy of num and the loop variable pos are allocated.
4. Execution: execute the logic inside function.
5. Teardown: after finish function logic. stack frame is popped (the Stack Pointer simply moves back up, marking that memory as free). The CPU reads the Link Register (LR) and jumps back to main.

| main() Stack Frame                |
|   - num = 31, p = 5, q = 6, ret   |
+-----------------------------------+

| printbits() Stack Frame           |
|   - num = 31  (Copied from R0)    |
|   - pos = 31  (Starts at 31)      |
+-----------------------------------+ <--- Stack Pointer (SP)

PHASE -3: Calling swap_bits(num, p, q)

same as PHASE-2

| main() Stack Frame                |
|   - num = 31, p = 5, q = 6, ret   |
+-----------------------------------+

| swap_bits() Stack Frame           |
|   - num = 31  (Local copy)        |
|   - p   = 5   (Local copy)        |
|   - q   = 6   (Local copy)        |
+-----------------------------------+ <--- Stack Pointer (SP)

PHASE -4:

1. The CPU returns to main().
2. When main() reaches the end of its block, its own stack frame is destroyed, and control passes back to the PetaLinux operating system environment.

| main() Stack Frame                |
|   - num = 31                      |
|   - p   = 5                       |
|   - q   = 6                       |
|   - ret = 31                      |
+-----------------------------------+ <--- Stack Pointer (SP)


## 
catching array in function can be treaed as in many ways :

function:
void print_array(int b[]) {

        for(int i=0;i<ELE;i++)
           printf("%d ",b[i]);
}


arguments	Actual type inside function
int b[]		int *b
int b[1000]	int *b    //1000 will be ignored
int *b		int *b

note: it is actually a pointer inside the function 



## embedded boot flow :

SOFTWARE FLOW   [ 1. BootROM Code ]  ───►  [ 2. FSBL Code ]    ───►  [ 3. U-Boot ]       ───►  [ 4. Linux Kernel ]
                       │                         │                         │                         │
                       ▼                         ▼                         ▼                         ▼
HARDWARE MEMORY [ BootROM (Silicon) ]      [ OCM (On-Chip RAM) ]     [ Main DDR3 RAM ]         [ Main DDR3 RAM ]
                (Permanent Factory CD)    (Tiny 256KB Workspace)    (Large 512MB Dining Room) (Large 512MB Dining Room)
─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
TRANSFER STEPS   1. Power-on triggers      2. BootROM copies         3. FSBL configures &      4. U-Boot reads 
                    hardcoded BootROM         FSBL from SD Card/        wakes up DDR3 RAM.        Kernel from SD/eMMC,
                    to execute.               QSPI into OCM.            Then loads U-Boot         unpacks it into RAM,
                                              Control jumps to OCM.     into RAM and jumps.       and starts Linux.
   
## Explaination :                                           
Step 1: The Silicon Awakening
	- Software: BootROM Code
	- Hardware Location: BootROM
	- What happens:  You flip the power switch. The processor chip is physically wired to look at the internal BootROM first. It checks your physical board jumpers (JP1/JP2) to find 
	  out where your boot files are stored (like an SD card or QSPI Flash).
	 
Step 2: The First Handoff (To OCM)
	- Software: FSBL (First Stage Boot Loader)
	- Hardware Location: OCM
	- What happens: The BootROM code reaches out to your SD card/Flash, grabs the FSBL file, and copies it directly into the OCM. Once the copy is done, 
	  the BootROM turns off, and the processor starts executing the FSBL directly inside the OCM.
	  
	  what it perform:
	  - Turns on the Main RAM: It wakes up the board's external 512MB DDR3 memory.
	  - Sets up the Hardware Clocks: It configures the CPU speeds and internal timing.
	  - Loads the next stage: It copies U-Boot into the freshly awakened DDR3 RAM and start it.
	  
Step 3: Waking up the Giant (To RAM)
	- Software: U-Boot
	- Hardware Location: Main DDR3 RAM
	- What happens: The FSBL code running inside the OCM contains the settings for your board's big external memory. It initializes the Main DDR3 RAM controller. 
	  Now that the 512MB RAM is awake, the FSBL reads the next file, U-Boot, from your SD card/Flash and copies it directly into the newly awakened Main DDR3 RAM.
	  
	  what it perform:
	  - Finds the Operating System: It scans your storage (like the TF card or eMMC flash) for the Linux kernel files.
	  - Prepares the Kernel: It unpacks the Linux kernel image and the Device Tree (the board's hardware map) into RAM.
	  - Starts the OS: It passes final instructions to the kernel and officially hands control over to Linux.
	  
Step 4: Loading the Operating System
	- Software: Linux Kernel
	- Hardware Location: Main DDR3 RAM
	- What happens: The processor jumps into Main DDR3 RAM to run U-Boot. U-Boot looks at your eMMC or SD card, grabs the heavy Linux Kernel file (uImage), 
	  unpacks it into another section of the Main DDR3 RAM, and launches Linux.
	
## bit stream file loading:
  
Option A: Loaded by the FSBL (Most Common)
In this setup, the bitstream is packaged directly inside your main BOOT.BIN file on your SD card alongside the FSBL and U-Boot.

	-The Flow: BootROM -> FSBL
	- The Load: As soon as the FSBL boots into the OCM, it grabs the .bit file from the BOOT.BIN package.
	- Where it goes: The FSBL pushes the bitstream directly into the FPGA Configuration Logic.
	- Result: Your custom hardware pins and FPGA circuits should be completely alive before U-Boot or Linux even start.
	
Option B: Loaded by U-Boot (Alternative)
In this setup, the bitstream is kept as a separate file on your SD card (often renamed or wrapped inside an image file).

	- The Flow: BootROM -> FSBL -> U-Boot.
	- The Load: U-Boot starts up, reaches out to the SD card, and reads the separate bitstream file into the main DDR3 RAM.
	- Where it goes: U-Boot runs a command (like fpga load) which flushes that file from the RAM into the FPGA Configuration Logic.
	- Result: The FPGA should turns on right before the Linux kernel is unzipped and launched.
	
	  
#### How the Kernel Uses the Device Tree to Handle a Peripheral
	
let s assume this dts..

uart0: serial@10010000 {
    compatible = "sifive,fu540-c000-uart";
    reg = <0x10010000 0x1000>;
    interrupts = <1>;
    clocks = <&tlclk>;
    status = "okay";
};


explanation :

- Where is it physically wired? (reg): 
  The kernel learns that the physical control switches for this serial port live at address 0x10010000 in the chip's silicon.

- Which device driver should I load? (compatible): 
  The kernel looks at the name "sifive,fu540-c000-uart". It searches through its internal code library, finds the matching software driver, and assigns it to this port.
  
- How fast should it run? (clocks): 
  It looks up the clock settings to calculate the correct timing speeds for data transfer.
  
- How do I listen to it? (interrupts): 
  It learns that if a user presses a key on the keyboard, the hardware will ring "Bell Number 1" (Interrupt 1) to wake up the CPU.
  
  
  
## driver s complexity in linux:
  ┌────────────────────────────────────────────────────────────────────────┐
  │  EASY (Few lines of code)                                              │
  │  • GPIO: Just flips a wire between 0 and 1.                            │
  │  • UART / Serial: Just a simple bucket that holds incoming text bytes. │
  ├────────────────────────────────────────────────────────────────────────┤
  │  MEDIUM (Requires strict timing)                                       │
  │  • I2C / SPI: Moves data using strict clock ticks to specific chips.   │
  ├────────────────────────────────────────────────────────────────────────┤
  │  HARD (High speed or complex memory)                                   │
  │  • Display / Audio: Continuous, heavy data streams with no lag allowed.│
  │  • Ethernet: Manages hundreds of thousands of DMA network packets.     │
  ├────────────────────────────────────────────────────────────────────────┤
  │  EXTREME (The Ultimate Boss)                                           │
  │  • USB Host: Manages dynamic speeds, hot-plugging, hubs, and devices.  │
  └────────────────────────────────────────────────────────────────────────┘
  
## A recipe will go thorugh bellow steps 

.bb
 │
 ▼
do_fetch
 │
 │  Download source
 ▼
do_unpack
 │
 │  Extract/checkout
 ▼
do_patch
 │
 │  Apply patches
 ▼
do_prepare_recipe_sysroot
 │
 │  Prepare dependencies
 ▼
do_configure
 │
 │  Configure project
 ▼
do_compile
 │
 │  Build
 ▼
do_install
 │
 │  Install into ${D}
 ▼
do_package
 │
 │  Split into packages
 ▼
do_packagedata
 │
 │  Generate package metadata
 ▼
do_package_write_*
 │
 │  Create RPM/IPK/DEB
 ▼
package
  
  
## bitbake commands 

bitbake -c list websocket-server  //This shows the tasks available for that recipe.
bitbake -c taskexp websocket-server  //can help visualize task dependencies.

bitbake -e websocket-server | grep ^FILE=   //find the path of bb file 

bitbake-layers show-layers   //show layers 

bitbake-layers create-layer ../meta-abhi  //create layer
bitbake-layers add-layer ../meta-abhi     //add layer to build/conf/bblayers.conf
bitbake-layers remove-layer ../meta-abhi  //remove layer from build/conf/bblayers.conf

bitbake core-image-minimal           //build the image
bitbake core-image-minimal -c clean  // clean the image

## petalinux commands

1. build only dtb file 
petalinux-build -c device-tree -x clean
petalinux-build -c device-tree

2. build u-boot with new dtsi
petalinux-build -c device-tree -x clean
petalinux-build -c device-tree
petalinux-package --force --boot --format BIN --fsbl images/linux/zynq_fsbl.elf --u-boot images/linux/u-boot.elf --fpga images/linux/system.bit   //Update BOOT.BIN with the new DTB

3. build kernel with new dtsi
petalinux-build -c device-tree -x clean
petalinux-build -c device-tree
petalinux-build -c kernel -x do_deploy   //Update image.ub with the new DTB

petalinux-build    //build the default image 
petalinux-build -x clean   //clean the image



## yocto directory structure 
vvdn@dpwcs:~/new_disk/clone-diu2/sgri_dpwc$ tree -L 1
.
├── build
├── downloads
├── imx-setup-release.sh -> sources/meta-imx/tools/imx-setup-release.sh
├── README.md -> sources/meta-imx/README.md
├── setup-environment -> sources/base/setup-environment
└── sources

## directory structure of petalinux
vvdn@dpwcs:~/new_disk/abhi/MYD/Project_mydz070$ tree -d -L 3
.
├── components
│   └── yocto
│       ├── cache
│       ├── conf
│       ├── downloads
│       ├── layers
│       ├── sysroots
│       └── workspace
├── project-spec
│   ├── configs
│   │   ├── busybox
│   │   ├── init-ifupdown
│   │   └── systemd-conf
│   ├── hw-description
│   └── meta-user
│       ├── conf
│       ├── meta-xilinx-tools
│       ├── recipes-app
│       ├── recipes-bsp
│       └── recipes-kernel
└── XSA


## what is machine.conf file 

path meta-imx-bsp/conf/machine/imx8mpevk.conf

roles:
1
defines hardware/platform-specific configuration.

2
DEFAULTTUNE = "cortexa53" 
This tells Yocto what CPU tuning to use.

3
PREFERRED_PROVIDER_virtual/kernel = "linux-imx"
It can specify which kernel provider/version should be use.

4
PREFERRED_PROVIDER_virtual/bootloader = "u-boot"
It can select/configure the bootloader and machine-specific bootloader configuration.

5
KERNEL_DEVICETREE = "freescale/my-board.dtb"
The machine configuration can specify which device-tree files should be built.

6
MACHINE_FEATURES += "wifi bluetooth"
Machine configuration can describe hardware capabilities

7
Machine configuration can also influence how the final image is generated.
like .wic or wic.gz

#roadmap:
local.conf
     │
     │ MACHINE="ddiu-imx8mp"
     ▼
machine/ddiu-imx8mp.conf
     │
     ├── CPU/architecture
     ├── kernel
     ├── bootloader
     ├── device tree
     ├── hardware features
     └── machine-specific settings
            │
            ▼
       BitBake metadata
            │
            ▼
          IMAGE
          
          
## petalinux 4 stage work flow

(Command)		(Main purpose)				(Simple meaning)
petalinux-create	Create a project/component		Create
petalinux-config	Configure the project			Configure
petalinux-build		Build recipes/kernel/rootfs		Build
petalinux-package	Create bootable/package artifacts	Package

##### Memory allocation using malloc and kalloc:

Your system has physical RAM:
                 Physical RAM
        ┌─────────────────────────────┐
        │  Physical Address 0x00000000│
        │                             │
        │   Kernel memory             │
        │                             │
        │   User process memory       │
        │                             │
        │   Page cache                │
        │                             │
        │   Free memory               │
        │                             │
        │   Physical Address ...      │
        └─────────────────────────────┘
        
But neither your application nor the kernel normally works directly with physical addresses.
They work with virtual addresses.

### malloc();
int *p = malloc(4096);

malloc() gives to your process a virtual address.
p = 0x7f12345000


User Process 
Virtual Address
0x7f12345000
       │
       │ MMU + Page Tables
          ▼
Physical Address
0x12345000
       │
          ▼
      RAM
 
So    
malloc()
   ↓
User virtual memory
   ↓
MMU / page table
   ↓
Physical RAM pages

#### kalloc();
int *p = kmalloc(4096, GFP_KERNEL);

kmalloc() allocates kernel memory, You get a kernel virtual address:
p = 0xffff800012345000

Kernel
Virtual Address
0xffff800012345000
        │
        │ MMU + Kernel page tables
           ▼
Physical Address
0x12345000
        │
           ▼
       RAM
       
       
So both can ultimately use RAM:
malloc()                  kmalloc()
   │                         │
    ▼                                     ▼
User Virtual Address     Kernel Virtual Address
   │                         │
   └──────────┬──────────────┘
                     ▼
             MMU
              │
                    ▼
       Physical RAM
       
## how /dev/mem works :

- /dev/mem does not contain a list of physical addresses. It is a Linux device interface that lets you access the physical address space.
- Think of /dev/mem as a window into physical address space.
- /dev/mem + mmap() is generally mapping an existing physical address, not allocating new memory.

see this :
fd = open("/dev/mem", O_RDWR | O_SYNC);
ptr = mmap(NULL,
           4096,
           PROT_READ | PROT_WRITE,
           MAP_SHARED,
           fd,
           0xA3106000);

mmap Map the physical address starting at 0xA3106000 into my process's virtual address space.
Linux creates the appropriate page-table mappings.

Process Virtual Address       Physical Address

0x7F12345000  ──────────────►  0xA3106000
                                  │
                                                   ▼
                            Hardware register

Now when your program accesses:
value = *(volatile uint32_t *)ptr;

## processor address space :

Conceptually, a processor has a physical address space:
Processor Physical Address Space
──────────────────────────────────────────────

0x00000000 ────────────────┐
                           │
                           │ RAM
                           │
0x80000000 ────────────────┘

0x80000000 ────────────────┐
                           │ Peripheral registers
                           │ UART
                           │ SPI
                           │ GPIO
                           │ Timer
                           │ etc.
0xA0000000 ────────────────┘

Other ranges ──────────────
                           │ Flash
                           │ PCIe
                           │ reserved
                           │ other devices
                           │
                           
The actual addresses and layout depend on the SoC.

/dev/mem is a Linux character device that provides access to physical memory addresses. It doesn't mean that every address corresponds to RAM.

For example, if a system has:
Physical address 0x10000000 → RAM
Physical address 0x40000000 → UART registers
Physical address 0x50000000 → GPIO registers

then /dev/mem can potentially be used to map those physical ranges.
/dev/mem
   │
   │ physical address = 0x40000000
   ▼
Physical address space
   │
   ▼
UART hardware registers

important points:
Physical address space:	 Addresses the CPU/SoC can generate for physical resources
RAM		      :  One resource occupying some portion of that address space
/dev/mem	      :  Linux interface that can provide userspace access to physical address ranges

##################################################

# can we use volatile and const at same time :

Yes, you can use const and volatile at the same time.
const, is a promise to the compiler that your code will not attempt to modify the variable.
volatile, is a warning to the compiler that the value can change due to factors outside your code, so it must never cache the value.

What is the need? (Real-World Use Cases):

Imagine a microcontroller hooked up to a physical button or a network card. The hardware updates a specific memory address to reflect the current state (e.g., 0 for button released, 1 for button pressed).

Why const? :
Your code should never try to write to a button. Writing to it makes no sense and could cause a hardware error. The compiler will catch it if you accidentally try.

Why volatile? :
If you loop while waiting for the button to be pressed, the compiler might optimize the loop away, assuming the value never changes because your code never modifies it. volatile forces it to check the hardware register on every loop iteration.

example:
// A read-only hardware register at memory address 0x40001000
volatile const unsigned int *button_status_reg = (volatile const unsigned int *)0x40001000;

void wait_for_button_press(void) {
    // Without volatile, this loop becomes infinite because the register is cached!
    while (*button_status_reg == 0) {
        // Do nothing, just wait for hardware to change the value to 1
    }
    printf("Button pressed!\n");
}

## I2C subsystem 

Linux code that connects:
Controller driver
        ↔
Device driver
			Linux kernel 
			  | 
			I2C Subsystem
                    ┌─────┴─────┐     
                    │           │     
        Controller driver    Device driver 

## how I2C device get detected 

supose we added I2C device node:
&i2c2 {
    tca8418@34 {
        compatible = "ti,tca8418";
        reg = <0x34>;
    };
};

Now these things will happen:

1. Kernel sees:
i2c2
   |
   +--- device at address 0x34
           compatible = "ti,tca8418"
           
2. Kernel search 

a) Which driver supports: "ti,tca8418" ?

b) It finds: TCA8418 driver
note: (because inside the driver : .compatible = "ti,tca8418")

c) TCA8418 driver probe will get called.


3. read write data 

TCA8418 Device Driver
        |
        | "Write 0x80 to register 0x01" or "read register 0x10"
        ↓
I²C Controller Driver
        |
        | Generates I²C transaction: (read/write)
        | START
        | Address 0x34  (7/10bit slave address)
        | Register 0x01 (8bit memory address inside slave)
        | Data 0x80     (8bit data)
        | STOP
        ↓
   SDA / SCL
        ↓
     TCA8418

remarks:
The I²C controller driver handles the I²C bus/protocol operation, while the device driver knows which registers of the particular device need to be read or written.

## USB subystem 

* Common USB host controller drivers:

USB Host Controller	Linux driver
EHCI — USB 2.0		ehci-hcd
OHCI — USB 1.1		ohci-hcd
UHCI — USB 1.1		uhci-hcd
xHCI — USB 3.x		xhci-hcd

DWC3 — common in embedded SoCs dwc3

* USB device-class driver:
USB pendrive  → usb-storage
USB keyboard  → usbhid
USB mouse     → usbhid
USB Ethernet  → usbnet / specific driver
USB camera    → uvcvideo

* For a modern embedded Linux board, you will very commonly see:

USB controller hardware
        ↓
Host Controller Driver (HCD)
        ↓
USB Core
        ↓
USB device driver
        ↓
USB device

* USB subsystem 

USB subsystem = the complete USB framework in the Linux kernel
USB core = the central/common part of that USB subsystem

                     Linux Kernel
                          │
                    USB Subsystem
                          │
          ┌───────────────┼────────────────┐
          │               │                │
      USB Core       Host Controller     Device
                      Drivers (HCD)      Drivers
                          │                │
                    ┌─────┴─────┐     ┌────┴─────┐
                    │           │     │          │
                  xHCI        DWC3  usb-storage usbhid
                  EHCI        etc.     │          │
                                      Pendrive  Keyboard
                                      
1. USB core:
	USB device registration
	USB bus management
	Device enumeration
	USB descriptors
	Configuration/interface management
	Matching USB devices with drivers
	USB transfers/interface handling
	Communication between HCD and USB device drivers
	
2. host controller driver:
   	Controller-specific implementation
   	Registers
   	Rings/descriptors
   	DMA
   	Interrupts
   	
3. device driver:
	registers the device with the SCSI/storage subsystem
	read from USB device
	write to USB device

note : the block layer ultimately exposes the device as /dev/sda


## how usb detected 

USB has a built-in device-detection/enumeration mechanism.

The USB host controller driver (HCD) continuously monitors the USB ports.

You insert the pendrive.
USB port
   |
   +---- D+
   +---- D-
   
A USB device has pull-up signaling on one of the USB data lines.
 |
 V
For example, a USB 2.0 device indicates its presence through the state of D+ or D-.
 |
 V
The USB host controller driver (HCD) detects a change in the port state.
 |
 V
Host controller driver generates an event/interrupt and report to USB core
 |
 V
USB core will start enumeration/discovery 
 |
 	get the device info (get descriptor)
 	find appropriate driver and call the probe
 V
USB device driver 
 |
 	registers the device with the SCSI/storage subsystem
 	the block layer ultimately exposes the device as /dev/sda

 
The Device Descriptor contains information such as:
VID          → Who manufactured it
PID          → Which product it is
USB version
Device class
Manufacturer string
Product string
Serial number
Number of configurations

For example:
VID = 0x0781
PID = 0x5567
Manufacturer = SanDisk
Product = USB Flash Drive

* how to read and write 

1. write:
usb-storage (device driver)
     │
     │ "I need to send this storage command"
       ↓
USB Core (code driver)
     │
     │ "Here's a USB transfer"
       ↓
HCD    (controller driver)
     │
     │ "I'll program the controller"
       ↓
Host Controller
     │
       ↓
USB device	

2. read:
Pendrive
   ↓
Host Controller
   ↓
HCD
   ↓
USB Core
   ↓
usb-storage
   ↓
SCSI
   ↓
Block layer
   ↓
Filesystem / Application

# During enumeration

When the pendrive is connected:
1. Host controller detects device connection
                 ↓
2. USB Core starts enumeration
                 ↓
3. USB Core asks for Device Descriptor
                 ↓
4. Request goes through USB controller driver
                 ↓
5. Controller hardware sends USB request
                 ↓
6. Pendrive receives request
                 ↓
7. Pendrive sends descriptor data back
                 ↓
8. Controller hardware receives it
                 ↓
9. USB controller driver gives data to USB Core
