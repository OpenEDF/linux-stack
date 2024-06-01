per-opeartion:

insmod log:
[362567.118211]  ---> mod_int
[362567.118241] linux own workqueue test...
[362567.118261] module init done!
[362567.118273]  <--- mod_int
[362568.190319]  ---> workqueue_fn
[362568.190349] executing workqueue function! count: 0
[362568.190369]  <--- workqueue_fn
[362573.230195]  ---> workqueue_fn
[362573.230221] executing workqueue function! count: 1
[362573.230240]  <--- workqueue_fn
[362578.270276]  ---> workqueue_fn
[362578.270302] executing workqueue function! count: 2
[362578.270321]  <--- workqueue_fn
[362583.310270]  ---> workqueue_fn
[362583.310299] executing workqueue function! count: 3
[362583.310318]  <--- workqueue_fn
[362588.350287]  ---> workqueue_fn
[362588.350315] executing workqueue function! count: 4
[362588.350334]  <--- workqueue_fn

rmmod log:
[362590.010612]  ---> mod_exit
[362590.010643] linux workqueue test test exit!
[362590.010655]  <--- mod_exit
