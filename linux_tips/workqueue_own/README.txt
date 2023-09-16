per-opeartion:

insmod log:
[  210.583251]  ---> mod_int
[  210.583278] linux own workqueue test...
[  210.583560] module init done!
[  210.583571]  <--- mod_int
[  210.583597]  ---> workqueue_fn
[  210.583607] executing workqueue function! count: 0
[  210.583622]  <--- workqueue_fn

rmmod log:
[  219.302454]  ---> mod_exit
[  219.302516]  ---> workqueue_fn
[  219.302528] executing workqueue function! count: 1
[  219.302539]  <--- workqueue_fn
[  219.302673] linux workqueue test test exit!
[  219.302684]  <--- mod_exit
