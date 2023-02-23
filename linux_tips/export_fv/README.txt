compiler:
pi@raspberrypi:~/linux_tips/export_fv$ make
make -C /home/pi/linux/ M=/home/pi/linux_tips/export_fv modules
make[1]: Entering directory '/home/pi/linux'
  CC [M]  /home/pi/linux_tips/export_fv/export_fv_call.o
  CC [M]  /home/pi/linux_tips/export_fv/export_fv.o
  MODPOST /home/pi/linux_tips/export_fv/Module.symvers
  CC [M]  /home/pi/linux_tips/export_fv/export_fv.mod.o
  LD [M]  /home/pi/linux_tips/export_fv/export_fv.ko
  CC [M]  /home/pi/linux_tips/export_fv/export_fv_call.mod.o
  LD [M]  /home/pi/linux_tips/export_fv/export_fv_call.ko
make[1]: Leaving directory '/home/pi/linux'

insmod log:
[53709.211458] export_fv: loading out-of-tree module taints kernel.
[53709.211495] export_fv: module license 'CPL' taints kernel.
[53709.211507] Disabling lock debugging due to kernel taint
[53709.212031] [EXPORT TEST]: Linux export function and variable test:
[53709.212044] [EXPORT TEST]: export function has be called.
[53709.212054] [EXPORT TEST]: export_var = 1
[53753.709243] [EXPORT TEST]: Linux module call export function and variable test:
[53753.709270] [EXPORT TEST]: export function has be called.
[53753.709283] [EXPORT TEST]: export_var = 2

rmmod log:
[54634.805433] [EXPORT TEST]: Linux module call export function and variable test exit!
[54642.614674] [EXPORT TEST]: Linux export function and variable test exit!

check symbol:
pi@raspberrypi:~/linux_tips/export_fv $ cat /proc/kallsyms | grep "export_var"
00000000 r __kstrtab_export_var [export_fv]
00000000 r __kstrtabns_export_var       [export_fv]
00000000 r __ksymtab_export_var [export_fv]
00000000 B export_var   [export_fv]
pi@raspberrypi:~/linux_tips/export_fv $ cat /proc/kallsyms | grep "export_shared_func"
00000000 r __kstrtab_export_shared_func [export_fv]
00000000 r __kstrtabns_export_shared_func       [export_fv]
00000000 r __ksymtab_export_shared_func [export_fv]
00000000 T export_shared_func   [export_fv]

check export symbol:
