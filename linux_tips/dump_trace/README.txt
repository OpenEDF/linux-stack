insmod log:
[10276.993945] DUMP STACK: START
[10276.993974] CPU: 0 PID: 21812 Comm: insmod Tainted: P         C O      5.15.92-v7+ #1
[10276.993998] Hardware name: BCM2835
[10276.994007] Backtrace:
[10276.994021] [<80a4bf10>] (dump_backtrace) from [<80a4c154>] (show_stack+0x20/0x24)
[10276.994070]  r7:00000002 r6:80d1b2ec r5:00000000 r4:60000013
[10276.994078] [<80a4c134>] (show_stack) from [<80a50744>] (dump_stack_lvl+0x70/0x94)
[10276.994107] [<80a506d4>] (dump_stack_lvl) from [<80a50780>] (dump_stack+0x18/0x1c)
[10276.994137]  r7:00000002 r6:7f13b000 r5:81005008 r4:7f1e0000
[10276.994144] [<80a50768>] (dump_stack) from [<7f13b024>] (mod_int+0x24/0x1000 [dump_trace])
[10276.994178] [<7f13b000>] (mod_int [dump_trace]) from [<801021c4>] (do_one_initcall+0x50/0x244)
[10276.994213] [<80102174>] (do_one_initcall) from [<801cc1b8>] (do_init_module+0x54/0x23c)
[10276.994248]  r8:7f1e0000 r7:00000002 r6:838cc040 r5:00000002 r4:7f1e0000
[10276.994256] [<801cc164>] (do_init_module) from [<801cea7c>] (load_module+0x255c/0x29c4)
[10276.994287]  r6:8e691980 r5:00000002 r4:86011f30
[10276.994294] [<801cc520>] (load_module) from [<801cf160>] (sys_finit_module+0xc8/0xfc)
[10276.994329]  r10:0000017b r9:86010000 r8:80100244 r7:00000003 r6:0002d064 r5:00000000
[10276.994339]  r4:81005008
[10276.994345] [<801cf098>] (sys_finit_module) from [<80100040>] (ret_fast_syscall+0x0/0x1c)
[10276.994370] Exception stack(0x86011fa8 to 0x86011ff0)
[10276.994387] 1fa0:                   cffc4600 7eb167d4 00000003 0002d064 00000000 00000005
[10276.994405] 1fc0: cffc4600 7eb167d4 0003fce8 0000017b 01a81d00 00000000 00000002 00000000
[10276.994418] 1fe0: 7eb16608 7eb165f8 00022cb8 76c66af0
[10276.994433]  r7:0000017b r6:0003fce8 r5:7eb167d4 r4:cffc4600
[10276.994442] DUMP STACK: END
rmmod log:
[10677.483922] DUMP STACK: GOODBYE
