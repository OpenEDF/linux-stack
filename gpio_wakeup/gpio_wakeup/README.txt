per-opeartion:

insmod log:
[  175.697890] gpio_wakeup: loading out-of-tree module taints kernel.
[  175.698703] --->: etx_driver_init
[  175.698720] Major = 239 Minor = 0
[  175.699206] GPIO_irqNumber = 199
[  175.699304] my_device: register IRQ as wakrup, ret = -22
[  175.699316] Device Driver Insert...Done!!!
[  175.699326] <---: etx_driver_init
[  188.766139] --->: gpio_irq_handler
[  188.766161] Interrupt Occurred : GPIO_21_OUT : 1
[  188.766169] <---: gpio_irq_handler
[  188.766805] --->: gpio_irq_handler
[  188.807323] --->: gpio_irq_handler
[  188.988265] --->: gpio_irq_handler
[  188.988281] Interrupt Occurred : GPIO_21_OUT : 0
[  188.988287] <---: gpio_irq_handler

rmmod log:

