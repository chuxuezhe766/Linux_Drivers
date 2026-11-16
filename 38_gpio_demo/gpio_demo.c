#include <linux/gpio.h>
#include <linux/module.h>

#define LED_GPIO 36  /* GPIO0_D0: 0*32 + 3*8 + 0 = 24 */

static int __init gpio_legacy_init(void)
{
    int ret = gpio_request(LED_GPIO, "legacy_led");
    if (ret) {
        pr_err("gpio request failed\n");
        return ret;
    }
    gpio_direction_output(LED_GPIO, 1);  /* 输出高电平 */
    pr_info("legacy GPIO init success\n");
    return 0;
}

static void __exit gpio_legacy_exit(void)
{
    gpio_set_value(LED_GPIO, 0);
    gpio_free(LED_GPIO);
    pr_info("legacy GPIO exit\n");
}

module_init(gpio_legacy_init);
module_exit(gpio_legacy_exit);
MODULE_LICENSE("GPL");