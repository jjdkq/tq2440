
#include <linux/init.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/gpio.h>
#include <linux/of.h>
#include <linux/of_gpio.h>
#include <linux/interrupt.h>

static irqreturn_t tq2440_key_isr(int irq, void *dev_id)
{
    printk("%s enter, irq: %d, %s\n", __func__, irq, (char *)dev_id);
    return IRQ_HANDLED;
}

static int tq2440_key_probe(struct platform_device *pdev) {
    struct device *dev = &pdev->dev;
    int irq_gpio = -1;
    int irq = -1;

    printk("%s enter.\n", __func__);

    if (!dev->of_node) {
        dev_err(dev, "no platform data.\n");
        return -EINVAL;
    }

    irq = platform_get_irq(pdev, 0);
    printk("%s: get irq %d\n", __func__, irq);
    devm_request_any_context_irq(dev, irq,
        tq2440_key_isr, IRQF_TRIGGER_FALLING, "key-1", "key-1");

    irq = platform_get_irq(pdev, 1);
    printk("%s: get irq %d\n", __func__, irq);
    devm_request_any_context_irq(dev, irq,
        tq2440_key_isr, IRQF_TRIGGER_FALLING, "key-2", "key-2");

    irq_gpio = of_get_named_gpio(dev->of_node, "key_3", 0);
    irq = gpio_to_irq(irq_gpio);
    printk("%s: gpio: %d ---> irq (%d)\n", __func__, irq_gpio, irq);
    devm_request_any_context_irq(dev, irq,
        tq2440_key_isr, IRQF_TRIGGER_FALLING, "key-3", "key-3");

    irq_gpio = of_get_named_gpio(dev->of_node, "key_4", 0);
    irq = gpio_to_irq(irq_gpio);
    printk("%s: gpio: %d ---> irq (%d)\n", __func__, irq_gpio, irq);
    devm_request_any_context_irq(dev, irq,
        tq2440_key_isr, IRQF_TRIGGER_FALLING, "key-4", "key-4");

    irq_gpio = of_get_named_gpio(dev->of_node, "key_8", 0);
    irq = gpio_to_irq(irq_gpio);
    printk("%s: gpio: %d ---> irq (%d)\n", __func__, irq_gpio, irq);
    devm_request_any_context_irq(dev, irq,
        tq2440_key_isr, IRQF_TRIGGER_FALLING, "key-8", "key-8");

    return 0;
}

static int tq2440_key_remove(struct platform_device *pdev) {

    printk("%s enter.\n", __func__);

    return 0;
}

static const struct of_device_id tq2440_key_dt_ids[] = {
    { .compatible = "tq2440,key", },
    {},
};

MODULE_DEVICE_TABLE(of, tq2440_key_dt_ids);

static struct platform_driver tq2440_key_driver = {
    .driver        = {
        .name    = "tq2440_key",
        .of_match_table    = of_match_ptr(tq2440_key_dt_ids),
    },
    .probe        = tq2440_key_probe,
    .remove        = tq2440_key_remove,
};

static int __init tq2440_key_init(void)
{
    int ret;

    ret = platform_driver_register(&tq2440_key_driver);
    if (ret)
        printk(KERN_ERR "tq2440_key: probe failed: %d\n", ret);

    return ret;
}
module_init(tq2440_key_init);

static void __exit tq2440_key_exit(void)
{
    platform_driver_unregister(&tq2440_key_driver);
}
module_exit(tq2440_key_exit);

MODULE_LICENSE("GPL");
