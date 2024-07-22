#include <linux/interrupt.h>
#include <linux/workqueue.h>

// Example data structure to pass to bottom half
struct my_data {
    int data;
    struct work_struct work;
};

// Bottom half handler (workqueue)
void my_bottom_half(struct work_struct *work) {
    struct my_data *mydata = container_of(work, struct my_data, work);
    // Process the deferred work
    printk(KERN_INFO "Bottom half processing data: %d\n", mydata->data);
    kfree(mydata); // Free allocated memory
}

// Top half handler (ISR)
irqreturn_t my_isr(int irq, void *dev_id) {
    struct my_data *mydata;

    // Allocate memory for the bottom half
    mydata = kmalloc(sizeof(struct my_data), GFP_ATOMIC);
    if (!mydata)
        return IRQ_HANDLED; // Handle allocation failure

    // Initialize workqueue
    INIT_WORK(&mydata->work, my_bottom_half);

    // Collect necessary data
    mydata->data = read_interrupt_data();

    // Schedule bottom half
    schedule_work(&mydata->work);

    return IRQ_HANDLED; // Interrupt handled successfully
}

static int __init my_module_init(void) {
    // Request IRQ and register ISR
    request_irq(my_irq, my_isr, IRQF_SHARED, "my_isr", dev_id);
    return 0;
}

static void __exit my_module_exit(void) {
    // Free IRQ
    free_irq(my_irq, dev_id);
}

module_init(my_module_init);
module_exit(my_module_exit);
