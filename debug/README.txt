1. debug i.mx6ull need list:
    1.1 jtag adapter（add ftdi and j-link）
    1.2 i.mx6ull board (The JTAG pin is connect the extend gpio)
    1.3 interface cfg file
    1.4 target cfg file
    1.5 openocd version -(Open On-Chip Debugger 0.11.0+dev-02177-g898e464d7)
    1.6 guide

2. Reference:
    2.1 https://community.nxp.com/t5/i-MX-Processors/Openocd-on-the-imx6ULL-EVK/m-p/830585
    2.2 https://wiki.dh-electronics.com/index.php/U-Boot_recovery_for_i.MX6_Q/D/DL/S/ULL_via_JTAG (only interface is different)

3. Support target plan:
    3.1 i.mx6ull myir dev board
    3.2 stm32mp157 mini zhengdianyuanzi dev board

4. Guide for myir i.mx6ull:

    4.1 Connect JTAG pin to ESP-Prog
             i.mx6ull                        ESP-Prog
        J3-28---------JTAG_TCK             ---------> TCK
        J3-30---------JTAG_TMS             ---------> TMS
        J3-32---------JTAG_TDI             ---------> TDI
        J3-34---------JTAG_TDO             ---------> TDO
        J3-36---------JTAG_nTRST (NC)
        J3-38---------JTAG_MOD   (NC)
        J3-40---------GND                  ---------> G D

    4.2 Run command in CLI: (Connect ESP-Prog to PC USB Port and power on i.mx6ull befor)
        $sudo openocd -f esp32_devkitj_v1.cfg -f imx6ull.cfg

    4.3 Log for connect success:
        Open On-Chip Debugger 0.11.0+dev-02177-g898e464d7 (2021-12-23-17:02)
        Licensed under GNU GPL v2
        
             .
            / \                     1. Mixing signed and unsigned integers in arithmetic operations
            | |                     2. Overstepping Array Boundaries
            |.|                     3. Missing out the Base Condition in Recursive Function
            |.|                     4. Using character constants instead of string literals, and vice versa
            |:|      __             5. Floating point literals are of type double by default
          ,_|:|_,   /  )            6. Forgetting to free memory
            (Oo    / _I_            7. Adding a semicolon to a #define
             +\ \  || __|           8. Be careful with semicolons
                \ \||___|
                  \ /.:.\-\         9. Mistakenly writing = instead of == when comparing
                   |.:. /-----\     10. Copying too much
                   |___|::oOo::|
                   /   |:<_T_>:|    11. Macros are simple string replacements
                  |_____\ ::: /
                   | |  \ \:/       [http://www.wuqi-tech.com/]
                   | |   | |        Buletooth: Beetle
           [Debug] \ /   | \__      WiFi: Hawk Kiwi
                   / |   \____\
                   `='
                 =================================
        For bug reports, read
                http://openocd.org/doc/doxygen/bugs.html
        Info : auto-selecting first available session transport "jtag". To override use 'transport select <transport>'.
        Warn : imx6ull.sdma: nonstandard IR value
        2182283264
        Info : Listening on port 6666 for tcl connections
        Info : Listening on port 4444 for telnet connections
        Info : clock speed 1000 kHz
        Info : JTAG tap: imx6ull.cpu tap/device found: 0x5ba00477 (mfg: 0x23b (ARM Ltd), part: 0xba00, ver: 0x5)
        Info : TAP imx6ull.sdma does not have valid IDCODE (idcode=0x1118203a)
        Info : JTAG tap: imx6ull.sjc tap/device found: 0x088c101d (mfg: 0x00e (Freescale (Motorola)), part: 0x88c1, ver: 0x0)
        [imx6ull.ahb] Target successfully examined.
        Info : imx6ull.cpu: hardware has 6 breakpoints, 4 watchpoints
        [imx6ull.cpu] Target successfully examined.
        Info : gdb port disabled
        Info : starting gdb server for imx6ull.cpu on 3333
        Info : Listening on port 3333 for gdb connections

    4.4 Use gdb debugging:
        $ arm-poky-linux-gnueabi-gdb -x ../../github/linux-stack/debug/gdbinit_imx6ull
        GNU gdb (GDB) 7.10.1
        Copyright (C) 2015 Free Software Foundation, Inc.
        License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>
        This is free software: you are free to change and redistribute it.
        There is NO WARRANTY, to the extent permitted by law.  Type "show copying"
        and "show warranty" for details.
        This GDB was configured as "--host=x86_64-pokysdk-linux --target=arm-poky-linux-gnueabi".
        Type "show configuration" for configuration details.
        For bug reporting instructions, please see:
        <http://www.gnu.org/software/gdb/bugs/>.
        Find the GDB manual and other documentation resources online at:
        <http://www.gnu.org/software/gdb/documentation/>.
        For help, type "help".
        Type "apropos word" to search for commands related to "word".
        (gdb) con
        0x0000861a in ?? ()
        (gdb) file u-boot
        A program is being debugged already.
        Are you sure you want to change the file? (y or n) y
        Reading symbols from u-boot...done.
        (gdb) monitor reset init
        Can't assert SRST: nSRST signal is not defined
        JTAG tap: imx6ull.cpu tap/device found: 0x5ba00477 (mfg: 0x23b (ARM Ltd), part: 0xba00, ver: 0x5)
        TAP imx6ull.sdma does not have valid IDCODE (idcode=0x1118203a)
        JTAG tap: imx6ull.sjc tap/device found: 0x088c101d (mfg: 0x00e (Freescale (Motorola)), part: 0x88c1, ver: 0x0)
        imx6ull.cpu rev 5, partnum c07, arch f, variant 0, implementor 41
        target halted in Thumb state due to debug-request, current mode: Supervisor
        cpsr: 0x400001f3 pc: 0x00008618
        MMU: disabled, D-Cache: disabled, I-Cache: enabled
        (gdb) load
        Loading section .text, size 0x45734 lma 0x87800000
        Loading section .rodata, size 0xfcfc lma 0x87845738
        Loading section .hash, size 0x2c lma 0x87855434
        Loading section .data, size 0xadfc lma 0x87855460
        Loading section .got.plt, size 0xc lma 0x8786025c
        Loading section .u_boot_list, size 0xbc8 lma 0x87860268
        Loading section .rel.dyn, size 0x93f8 lma 0x87860e30
        Loading section .dynsym, size 0x60 lma 0x8786a228
        Loading section .dynstr, size 0x2a lma 0x8786a288
        Loading section .dynamic, size 0x88 lma 0x8786a2b4
        Loading section .interp, size 0x11 lma 0x8786a33c
        Start address 0x87800000, load size 435015
        Transfer rate: 61 KB/sec, 3815 bytes/write.
        (gdb) b board_init_r
        Breakpoint 1 at 0x87811764: file common/board_r.c, line 992.
        (gdb) c
        Continuing.

    4.5 Use telent debugging:
        $ telnet localhost 4444
        Trying 127.0.0.1...
        Connected to localhost.
        Escape character is '^]'.
        Open On-Chip Debugger
        > reset init
        Can't assert SRST: nSRST signal is not defined
        JTAG tap: imx6ull.cpu tap/device found: 0x5ba00477 (mfg: 0x23b (ARM Ltd), part: 0xba00, ver: 0x5)
        TAP imx6ull.sdma does not have valid IDCODE (idcode=0x1118203a)
        JTAG tap: imx6ull.sjc tap/device found: 0x088c101d (mfg: 0x00e (Freescale (Motorola)), part: 0x88c1, ver: 0x0)
        imx6ull.cpu: ran after reset and before halt ...
        imx6ull.cpu rev 5, partnum c07, arch f, variant 0, implementor 41
        imx6ull.cpu: MPIDR level2 0, cluster 0, core 0, multi core, no SMT
        target halted in Thumb state due to debug-request, current mode: Supervisor
        cpsr: 0x800001f3 pc: 0x00002f88
        MMU: disabled, D-Cache: disabled, I-Cache: enabled
        > load_image /home/macro/imx6ull-devel/uboot/u-boot.bin 0x87800000 bin
        434728 bytes written at address 0x87800000
        downloaded 434728 bytes in 3.137619s (135.306 KiB/s)
        
        > resume 0x87800000

    4.6 Log output:
        U-Boot 2016.03-g04a0d30-dirty (Oct 03 2022 - 15:20:46 +0800)

        CPU:   Freescale i.MX6ULL rev1.1 528 MHz (running at 396 MHz)
        CPU:   Commercial temperature grade (0C to 95C) at 47C
        Reset cause: POR
        Board: MYS-MX6ULL 14x14
        Author: Macro
        I2C:   ready
        DRAM:  256 MiB
        NAND:  256 MiB
        MMC:   FSL_SDHC: 0
        *** Warning - bad CRC, using default environment
        
        Display: TFT43AB (480x272)
        Video: 480x272x24
        In:    serial
        Out:   serial
        Err:   serial
        Net:   FEC0
        Error: FEC0 address not set.
        
        Boot from USB for mfgtools
        Use default environment for                              mfgtools
        Run bootcmd_mfg: run mfgtool_args;bootz ${loadaddr} ${initrd_addr} ${fdt_addr};
        Hit any key to stop autoboot:  0
        Bad Linux ARM zImage magic!
        =>

5.Guide for stm32mp157 mini:
     5.1 Connect JTAG pin to ST-Link V2:

        
