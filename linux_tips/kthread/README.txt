per-opeartion:

insmod log:
[20920.696006] --->: mod_int
[20920.696035] linux softirq test...
[20920.696278] etx thread create succefully!
[20920.696294] module init done!
[20920.696305] <---: mod_int
[20920.696319] --->: kthread_fn
[20920.696334] executing kthread function 0
[20920.696334] executing kthread function 1
[20922.812162] executing kthread function 2
[20923.854316] executing kthread function 3
[20924.896432] executing kthread function 4
[20925.938416] executing kthread function 5
[20926.980369] executing kthread function 6
[20928.022269] executing kthread function 7

rmmod log:
[20970.974718] --->: mod_exit
[20971.747265] <---: kthread_fn
[20971.747345] linux softirq test exit!
[20971.747360] <---: mod_exit

