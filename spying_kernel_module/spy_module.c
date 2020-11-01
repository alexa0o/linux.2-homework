#include <linux/module.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/types.h>
#include <linux/timer.h>
#include <linux/kernel.h>
#include <asm/atomic.h>
#include <asm/io.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Alexandr Khatironov");
MODULE_DESCRIPTION("Count how many time PS/2 keys were passed in a minute");
MODULE_VERSION("0.01");

#define PERIOD 60

static int irq = 1;
static int dev_id;
static atomic_t spy_counter = ATOMIC_INIT(0);

static irqreturn_t interrupt_handler(int irq, void* dev_id) {
    unsigned char scancode;
    scancode = inb(0x60);
    if (!(scancode & 0x80)) {
        atomic_inc(&spy_counter);
    }
    return IRQ_NONE;
}

static struct timer_list spy_timer;

void spy_callback(struct timer_list* timer) {
    printk(KERN_INFO "Keys were passed %d times", atomic_xchg(&spy_counter, 0));
    mod_timer(&spy_timer, jiffies + msecs_to_jiffies(PERIOD * 1000));
}

///////////////////////////////////////////////////////////////////////////////

static int __init spy_init(void) {
    if (request_irq(irq, interrupt_handler, IRQF_SHARED, "spy_interrupt", &dev_id))
        return -1;
    timer_setup(&spy_timer, spy_callback, 0);
    mod_timer(&spy_timer, jiffies + msecs_to_jiffies(PERIOD * 1000));
    printk(KERN_INFO "Successfully loading spying kernel module on IRQ: %d\n", irq);
    return 0;
}

static void __exit spy_exit(void) {
    synchronize_irq(irq);
    free_irq(irq, &dev_id);
    del_timer(&spy_timer);
    printk(KERN_INFO "Successfully unloading spying from IRQ: %d\n", irq);
}

module_init(spy_init);
module_exit(spy_exit);
