1. debug i.mx6ull need list:
    1.1 jtag adapter
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

5.Guide for stm32mp157 mini:

        
