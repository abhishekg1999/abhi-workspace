## What is the difference between a CPU core, a processor, and a SoC?

cpu core:
A CPU core is an independent execution unit that can execute instructions. It typically contains:

CPU Core
├── ALU
├── Registers
├── Control/Decode logic
├── Instruction execution units
├── PC
├── Branch/exception logic
└── Cache (depending on architecture/design)

processor:
its a collection of cpu core s , A multicore processor contains multiple CPU cores, where each core can independently execute instructions.

Processor
├── Core 0
├── Core 1
├── Core 2
└── Core 3

soc:
An SoC integrates one or more CPU cores along with memory controllers, interconnects, and peripherals such as UART, SPI, I2C, GPIO, USB, Ethernet, timers, etc., onto a single chip.

                 SoC
 ┌─────────────────────────────────────┐
 │                                     │
 │  CPU Cores                          │
 │  ┌─────┐ ┌─────┐                    │
 │  │Core0│ │Core1│                    │
 │  └─────┘ └─────┘                    │
 │                                     │
 │  Memory Controller                  │
 │  Cache                              │
 │  Interrupt Controller               │
 │  DMA Controller                     │
 │                                     │
 │  UART   SPI   I2C   GPIO            │
 │  USB    Ethernet   CAN              │
 │                                     │
 │  Timers   PWM   ADC ...             │
 │                                     │
 └─────────────────────────────────────┘


## SoC Cache Hierarchy

Here is exactly where cache is distributed across a typical SoC:

1. Inside the CPU Core (Dedicated Cache):
These caches are physically built directly inside each individual CPU core. They are extremely fast but small, operating at the exact same clock speed as the core itself.

	# L1 Cache (Level 1): 
	Located deep inside each core. It is split into two parts: L1i (for instructions) and L1d (for data). It provides the lowest latency access for the CPU.
	# L2 Cache (Level 2):
	Also located inside (or tightly coupled to) each core. It is slightly larger and a bit slower than L1, acting as a secondary buffer for that specific core.

2. Outside the CPU Cores, Inside the CPU Cluster (Shared Cache):
	# L3 Cache (Level 3): 
	Located outside the individual cores but still within the CPU cluster block.
	All the CPU cores within that cluster share this cache to communicate and share data efficiently without needing to go to main memory.

3. Outside the CPU Entirely (System-Level Cache):
	# SLC (System-Level Cache) / LLC (Last Level Cache): 
	This sits entirely outside the CPU cluster on the SoC fabric. It is shared by all components of the SoC, 
	including the GPU, NPU (Neural Processing Unit), Image Signal Processor (ISP), and the CPU. When the GPU or CPU needs data, 
	it checks the SLC before spending power to access the external RAM (DRAM).

Quick Comparison:

Cache Level:		Location:				Who Can Access It?:	Speed:		Capacity:
L1 Cache		Inside the CPU Core			Single Core Only	Blazing 	FastVery Small (KB)
L2 Cache		Inside the CPU Core			Single Core Only	Very Fast	Small (KB to MB)
L3 Cache		Outside Cores,Inside CPU Cluster	All CPU Cores		Fast		Medium (MB)
System Cache (SLC)	On SoC Fabric, Outside CPU		CPU, GPU, NPU, etc.	Moderate	Large (MB)

#1 Cache is on the MYD-Z020 Board:

1. L1 Cache (Level 1) — Inside the CPU Cores
	Amount: 64 KB per core (Totaling 128 KB across the two cores).
	Structure: Each of the two Arm Cortex-A9 cores has its own dedicated L1 cache, split into:
		- 32 KB L1 Instruction cache (L1i)
		- 32 KB L1 Data cache (L1d)

2. L2 Cache (Level 2) — Shared Within the CPU Cluster
	Amount: 512 KB total.
	Structure: This is a uniform, unified secondary cache that is outside the individual cores but inside the processing cluster. 
	Both CPU cores share this 512 KB pool to easily pass data back and forth without hitting external memory.

Cache Component			Capacity 			Location		Type
Core 0	- L1			64 KB (32KB i / 32KB d)		Inside Core 0		Dedicated
Core 1 - L1			64 KB (32KB i / 32KB d)		Inside Core 1		Dedicated
Shared L2			512 KB				Inside CPU Cluster	Shared by Core 0 & 1Total Cache576 KB

┌────────────────────────────────────────────────────────────────────────┐
│                      ZYNQ-7020 PROCESSING SYSTEM (PS)                  │
│                                                                        │
│  ┌────────────────────────────────────────────────────────----- ─┐          │
│  │            APPLICATION PROCESSING UNIT (APU)                  │          │
│  │                                                               │          │
│  │  ┌───────────────────────────┐ ┌───────────────────────────┐  │          │
│  │  │        CORE 0             │ │        CORE 1             │  │          │
│  │  │ ┌───────┐ ┌─────────────┐ │ │ ┌───────┐ ┌─────────────┐ │  │          │
│  │  │ │  ALU  │ │     MMU     │ │ │ │  ALU  │ │     MMU     │ │  │          │
│  │  │ └───┬───┘ └──────┬──────┘ │ │ └───┬───┘ └──────┬──────┘ │  │          │
│  │  │     │ Virtual    │ Page   │ │     │ Virtual    │ Page   │  │          │
│  │  │     ▼ Address    ▼ Table  │ │     ▼ Address    ▼ Table│  │          │
│  │  │ ┌───────────────┐ Walk    │ │ ┌───────────────┐ Walk    │  │          │
│  │  │ │ L1 Data/Inst  ├─────────┤ │ │ L1 Data/Inst  ├─────────┤  │          │
│  │  │ │ Cache (64KB)  │         │ │ │ Cache (64KB)  │         │  │          │
│  │  │ └───────┬───────┘         │ │ └───────┬───────┘         │  │          │
│  │  └─────────┼─────────────────┘ └─────────┼─────────────────┘  │          │
│  │            │ Physical                    │ Physical           │          │
│  │            │ Address                     │ Address            │          │
│  │            ▼                             ▼                    		    │          │
│  │       ┌───────────────────────────────────────┐               │          │
│  │       │       SNOOP CONTROL UNIT (SCU)        │               │          │
│  │       └───────────────────┬───────────────────┘               │          │
│  │                           ▼                                   │          │
│  │       ┌───────────────────────────────────────┐               │          │
│  │       │       SHARED L2 CACHE (512 KB)        │               │          │
│  │       └───────────────────┬───────────────────┘               │          │
│  └───────────────────────────┼─────────────────────────────────────┘          │
│                              │ Physical Memory Bus                            │
│                              ▼                                                │
│        ┌───────────────────────────────────────────┐                          │
│        │      CENTRAL INTERCONNECT (AXI MATRIX)     │                          │
│        └──────┬───────────────────┬────────────────┘                          │
│               │                   │                                           │
│               ▼                   ▼                                           │
│       ┌───────────────┐   ┌───────────────────────────────┐                   │
│       │  DDR3 MEMORY  │   │     I/O PERIPHERALS (IOP)     │                   │
│       │  CONTROLLER   │   │ (UART, SPI, I2C, USB, GigE)   │                   │
│       └───────┬───────┘   └───────────────┬───────────────┘                   │
└───────────────┼───────────────────────────┼───────────────────────────────────┘
                │                           │
  Outside Chip  ▼                           ▼ Pins on the Board
        ┌───────────────┐           ┌───────────────────────────────┐
        │  1GB DDR3 RAM │           │ RS232, Pins, SD Card, etc.     │
        └───────────────┘           └───────────────────────────────┘

	
## how data flows in soc from RAM to till registers (myd-z020):


#Step 1: The CPU Requests Data (Virtual Address) :
The CPU executes an instruction (like LDR R0, [R1]) to load data. The address inside the register R1 is a Virtual Address.

- The CPU splits this address into two parts: a Virtual Page Number (VPN) and an Offset (the exact byte location inside that 4 KB page).

#Step 2: Address Translation (Page to Frame) :
Before the SoC can look at the caches or RAM, it must translate the Virtual Address into a Physical Address.

* The TLB Check: 
The CPU first looks inside the TLB (Translation Lookaside Buffer), which is a super-fast hardware cache for page translations.

* Page Table Walk (If TLB Misses): 
If the translation isn't in the TLB, the hardware MMU (Memory Management Unit) walks through the Page Tables stored in RAM. It finds the matching Physical Frame Number (PFN) and combines it with the original offset to get the exact physical address in RAM.

* The translation is saved in the TLB for next time.

#Step 3: Checking the Cache Hierarchy (The Data Flow)

Now that the MMU knows the physical address, the SoC checks the cache hierarchy. On the Cortex-A9, the L1 and L2 caches are Physically Indexed, Physically Tagged (PIPT). This means the cache lookup happens after the address is translated into physical coordinates.

The data flows up the chain like a ladder:
[ CPU Registers ] 
       ▲
       │ (1 to 8 bytes loaded via instruction)
[ L1 Data Cache ]  ── (32 KB per core; handles 32-byte cache lines)
       ▲
       │ (If L1 Miss: Fetches 32-byte line)
[ Shared L2 Cache ] ── (512 KB total; handles 32-byte cache lines)
       ▲
       │ (If L2 Miss: Requests data from RAM controller)
[ Physical RAM (DDR3) ] ── (Stored in 4 KB Physical Frames)

1. L1 Cache Lookup: The MMU looks for the target data inside the core's L1 Data Cache. If it is a Hit, the data is instantly passed to the CPU Register.
2. L2 Cache Lookup (On L1 Miss): If the data isn't in L1, a request goes to the Shared L2 Cache. If it's an L2 Hit, a 32-byte chunk (a cache line) containing the needed data is copied into the L1 cache, and the specific target byte/word goes to the register.

3. RAM Fetch (On L2 Miss): If the data isn't in the caches, the L2 cache controller initiates an external memory access to the DDR3 RAM.

#Key Concept: Cache Lines vs. Entire Pages

It is highly efficient to note that the SoC never copies an entire 4 KB frame into the cache all at once.
- RAM thinks in 4 KB Frames (for access control and mapping).
- Caches think in 32-Byte Cache Lines (for speed).

When an L2 cache miss occurs, the DDR3 controller goes to the specific 4 KB physical frame in RAM, extracts just the 32-byte cache line containing your requested data, and passes it up to the L2, then L1, and finally to the CPU Register.

#What Happens if the Page is Not in RAM? (Page Fault):
If the MMU checks the Page Table and finds the page's "Valid Bit" is 0, the page isn't even in the DDR3 RAM (it might be swapped to storage, or not allocated yet).

1. The MMU triggers a Page Fault Interrupt to the operating system.
2. The OS kernel stops the program, allocates a blank 4 KB Physical Frame in the DDR3 RAM, and loads the data into it from storage.
3. The OS updates the Page Table (sets the valid bit to 1).
4. The CPU retries the instruction, and the standard cache pipeline described above takes over.

## why need cache memory 

We use cache because RAM is too slow for a modern CPU.
If the CPU had to talk directly to the RAM for every single instruction, your high-speed board would slow down to a crawl. This performance gap is known in computer architecture as the "Memory Wall."

Here is exactly why cache is necessary:
1. The Speed Gap (The Main Reason) :
A modern CPU core is incredibly fast, while RAM is relatively slow.	
The CPU Core processes data in a fraction of a nanosecond (less than 0.5 ns).
DDR3 RAM takes about 50 to 100 nanoseconds to find and return data.

If the CPU requests a variable and has to wait for RAM, it must sit completely idle for 100 to 200 clock cycles doing absolutely nothing. Cache bridges this gap by sitting right next to the core, acting as a super-fast buffer:

L1 Cache responds in ~1 to 2 clock cycles (less than 1 ns).
L2 Cache responds in ~10 clock cycles (~3–5 ns).

2. The Laws of Physics and Cost
# Why don't we just make the entire 1 GB of RAM as fast as the cache?

Cache is made of SRAM (Static RAM):
It uses 6 transistors per bit. It is blazingly fast, uses less power when active, but is physically huge and extremely expensive. Putting 1 GB of SRAM on a chip would make the chip massive, hot, and cost thousands of dollars.
 
RAM is made of DRAM (Dynamic RAM):
It uses only 1 transistor and 1 capacitor per bit. It is incredibly cheap and can pack gigabytes of data into a tiny space, but it is fundamentally slower because the capacitors constantly need to be refreshed with electricity.

3. Predictable Human Behavior (Locality)
Cache works incredibly well because software naturally follows the Principle of Locality:

Temporal Locality: 
If your code uses a variable once (like a loop counter i), it is highly likely to use it again almost immediately. Cache keeps it close by.

Spatial Locality: 
If your code accesses an array at index [0], it is highly likely to access [1] and [2] next. When the CPU misses cache, it doesn't fetch just 1 byte from RAM; it grabs a whole 32-byte cache line (the neighboring data) because it anticipates the CPU will need it next.

## why we use NOR and NAND flash memory 

#1. The Core Differences :
The primary difference lies in how their internal memory cells are wired together, which completely changes how the CPU can access them.

NOR Flash (Random Access): 
NOR flash is wired like RAM. It allows Byte-Level Random Access. This means the CPU can read any single byte of data directly using an address bus

NAND Flash (Page/Block Access): 
NAND flash is wired like a grid of storage sectors. It does not allow random access. Data must be read and written in larger chunks called Pages (e.g., 2 KB or 4 KB) and erased in even larger chunks called Blocks.

* Direct Comparison Table:
Feature				NOR Flash				NAND Flash
Read Speed			Fast for small/random bytes		Fast for large sequential files
Write/Erase Speed		Very Slow				Fast
Random Access?			Yes (XIP - Execute In Place)		No (Must copy to RAM first)
Storage Capacity		Low (Typically 1 MB to 512 MB)		High (Gigabytes to Terabytes)
Cost per Megabyte		High					Very Low

#2. Which One Should You Use Where?:

Because of these differences, they have two completely distinct jobs in an embedded system like your MYD-Z020 board:

# Where to use NOR Flash: "The Bootloader & Code Storage"
Use NOR flash when your primary goal is to boot the system safely and immediately.

Used In: 
Storing the First Stage Bootloader (FSBL), BIOS, or critical microcode.

Reason: 
Because NOR allows random access, it supports a feature called XIP (Execute In Place). The CPU core can point its execution pointer directly at the NOR flash chip and start running instructions instantly when you flip the power switch—no RAM setup required.

# Where to use NAND Flash: "The Operating System & User Data"
Use NAND flash when your primary goal is bulk storage of files, images, and data.

Used In: 
Storing the PetaLinux Kernel, root filesystems, application code, logs, and database files. It is the technology inside SD cards, eMMC chips, and Solid-State Drives (SSDs).

Reason: 
NAND is incredibly cheap and can scale to gigabytes easily. When your system boots, the bootloader reads the massive PetaLinux kernel out of the NAND flash, uncompresses it, copies it into the DDR3 RAM, and the CPU runs it from RAM.

#3 qspi(nor flash) & emmc(nand flash) on your MYD-Z020 Board :

Your development board typically incorporates both options or gives you a switch to choose between them:

QSPI NOR Flash (Onboard): 
Usually 16 MB or 32 MB. You flash your Zynq bootloader (BOOT.BIN) here so the board wakes up instantly on power-on.

SD Card / eMMC (NAND Flash): 
Usually 4 GB to 32 GB. You place your heavy PetaLinux root filesystem and applications here because they won't fit on the tiny NOR chip.

## Quick Comparison of memory in MYD-Z020 Board :

Memory:				Where does it sit?:			Technology:		Capacity on Zynq-7020:
L1 cache			Inside the CPU each core(Core0,core1)	SRAM			64 KB
L2 Cache			Inside the CPU Cluster(outside core)	SRAM			512 KB
OCM (On-Chip Memory)		Inside the SoC, Outside the CPU		SRAM			256 KB
DDR3 RAM			Outside the SoC				DRAM (Dynamic RAM)	1 GB
QSPI NOR Flash			Outside the SoC 			NOR Flash		16 MB or 32 MB
eMMC				Outside the SoC 			NAND Flash		4 GB to 8 GB

=============================================================================================
                                     MYD-Z020 PHYSICAL BOARD (PCB)
=============================================================================================
┌────────────────────────────────────────────────────────────────────────┐
│                      ZYNQ-7020 SoC SILICON CHIP                        │
│                                                                        │
│   ┌─────────────────────────────────────────────────────────┐          │
│   │            APPLICATION PROCESSING UNIT (APU)            │          │
│   │                                                         │          │
│   │   ┌───────────────────────────┐ ┌───────────────────────────┐  │          │
│   │   │        CORE 0             │ │        CORE 1             │  │          │
│   │   │ [Registers] ◄──► [MMU]    │ │ [Registers] ◄──► [MMU]    │  │          │
│   │   │              │            │ │              │            │  │          │
│   │   │              ▼            │ │              ▼            │  │          │
│   │   │     ┌─────────────────┐   │ │     ┌─────────────────┐   │  │          │
│   │   │     │  L1 CACHE (64K) │   │ │     │  L1 CACHE (64K) │   │  │          │
│   │   │     └────────┬────────┘   │ │     └────────┬────────┘   │  │          │
│   │   └──────────────┼────────────┘ └──────────────┼────────────┘  │          │
│   │                  ▼                             ▼               │          │
│   │             ┌───────────────────────────────────────┐          │          │
│   │             │       SNOOP CONTROL UNIT (SCU)        │          │          │
│   │             └───────────────────┬───────────────────┘          │          │
│   │                                 ▼                              │          │
│   │             ┌───────────────────────────────────────┐          │          │
│   │             │         SHARED L2 CACHE (512K)        │          │          │
│   │             └───────────────────┬───────────────────┘          │          │
│   └─────────────────────────────────┼──────────────────────────────┘          │
│                                     │ Internal Silicon AXI Highway            │
│                                     ▼                                         │
│   ┌───────────────────────────────────────────────────────────────────────┐   │
│   │                     CENTRAL INTERCONNECT MATRIX                       │   │
│   └────────┬────────────────────────┬───────────────┬──────────────────┬──┘   │
│            │                        │               │                  │      │
│            ▼                        ▼               ▼                  ▼      │
│   ┌────────────────┐       ┌────────────────┐┌──────────────┐┌──────────────┐ │
│   │  ON-CHIP RAM   │       │ DDR CONTROLLER ││  QSPI FLASH  ││  SD/SDIO     │ │
│   │  (256KB OCM)   │       └────────┬───────┘│  CONTROLLER  ││  CONTROLLER  │ │
│   └────────────────┘                │        └──────┬───────┘└────────┬─────┘ │
│                                     │               │                 │       │
└─────────────────────────────────────┼───────────────┼─────────────────┼───────┘
                                      │               │                 │
                                      │ Physical Pins │ MIO Pins        │ MIO Pins
  ────────────────────────────────────┼───────────────┼─────────────────┼───────────────────
  PHYSICAL PCB CHIPS                  ▼               ▼                 ▼
                              ┌───────────────┐┌───────────────┐┌───────────────┐
                              │  DDR3 RAM     ││ QSPI NOR      ││  ONBOARD eMMC │
                              │  (1 GB)       ││ (16MB - 32MB) ││  (4GB - 8GB)  │
                              └───────────────┘└───────────────┘└───────────────┘

Boot Order: 
Power On ──► Internal BootROM launches ──► Reads BOOT.BIN from QSPI NOR (or SD Card) ──► Loads FSBL into internal OCM ──► FSBL wakes up external DDR3 RAM ──► U-Boot launches in RAM ──► Loads the heavy Linux kernel and rootfs from eMMC into DDR3 RAM.

Execution Loop: 
Once Linux is running, instructions move from DDR3 RAM into the L2 Cache, then to the L1 Cache, and finally execute directly inside the CPU Registers.

## NOR(qspi flash) advantage over NAND(sd/emmc) :

1. File System Corruption Risk (No Controller / No FTL):
The eMMC Weakness: 
Inside every eMMC chip (regardless of size) sits a micro-controller running a complex firmware layer called the Flash Translation Layer (FTL). The FTL handles block mapping and wear leveling. If a machine experiences a sudden power loss while the eMMC is doing background tasks or managing its file allocation table, the FTL firmware can get corrupted. If the FTL dies, the eMMC chip becomes completely unresponsive ("bricked").

The NOR Advantage: 
QSPI NOR flash contains no controller and no background FTL firmware. It is just raw, simple memory cells. You talk to it directly using basic SPI commands. If the power gets cut mid-operation, a written file might be incomplete, but the chip itself can never be bricked or corrupted at a hardware level.

2. Physical Layout and Pin Efficiency (4 Pins vs. 30+ Pins):
eMMC:
Even a small eMMC requires an 8-bit parallel data bus, plus clock, command, and multiple power rails (usually around 30 to 100 physical solder balls underneath the chip). Routing these high-speed parallel lines on a circuit board takes up valuable space, demands a thicker circuit board (more layers), and consumes many hardware pins on the MPSoC.

QSPI NOR Flash:
As the name implies (Quad SPI), it requires only 4 data pins (plus clock and chip select). It sits in a tiny, simple 8-pin package that takes up virtually no space on the board and leaves the MPSoC's pins free to be used for your actual hardware peripherals (like sensors, motors, or ethernet).

3. Driver Complexity for the BootROM:
eMMC Driver: To talk to an eMMC chip, a processor's built-in BootROM must send a complex initialization sequence to negotiate voltages, bus widths, and timings. If a specific eMMC vendor requires a slightly non-standard handshake, the hardcoded chip BootROM might fail to read it.

NOR Driver: 
QSPI NOR flash is essentially standard SPI. The initialization sequence is almost non-existent. It acts like an extension of the CPU's memory. The BootROM can read from it instantly using a tiny fraction of the code space.

4. Raw Reliability and Data Retention:
eMMC (NAND): 
NAND flash cells slowly leak electrons over time. If a machine sits turned off in a warehouse or on a shelf for 5 to 10 years, an eMMC chip can actually lose its data due to "charge loss."

QSPI (NOR): 
NOR flash has massive isolation gates that lock electrons in place securely. A bootloader written to a QSPI NOR flash can sit unpowered for 20 to 25 years in extreme temperatures and still boot perfectly the second it receives power.

## Comparison Summary wrt transister

Memory Feature			Cache (SRAM)		Main RAM (DRAM)			NOR Flash						NAND Flash
Transistors Per Bit		6 Transistors		1 Transistor + 1 Capacitor	1 Transistor (Parallel wiring takes extra space)	1 Transistor (Series wiring allows max density)
Volatile?(data gone on poweoff)	yes			yes				No							No
Access Granularity		Individual Byte		Individual Byte			Individual Byte						Large Blocks Only


