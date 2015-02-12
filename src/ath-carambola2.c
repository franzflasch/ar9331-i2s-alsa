/*
 * ath-carambola2.c -- ALSA machine code for CARAMBOLA2
 *
 * Copyright (c) 2013 The Linux Foundation. All rights reserved.
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <asm/delay.h>
#include <linux/types.h>
#include <sound/core.h>
#include <sound/soc.h>
#include <linux/module.h>

/* Driver include */
#include <asm/mach-ath79/ar71xx_regs.h>
#include <asm/mach-ath79/ath79.h>
#include "ath79-i2s.h"
#include "ath79-pcm.h"


typedef unsigned int ar7240_reg_t;

#define ar7240_reg_rd(_phys)	(*(volatile ar7240_reg_t *)KSEG1ADDR(_phys))
#define ar7240_reg_wr_nf(_phys, _val) \
	((*(volatile ar7240_reg_t *)KSEG1ADDR(_phys)) = (_val))

#define ar7240_reg_wr(_phys, _val) do {	\
	ar7240_reg_wr_nf(_phys, _val);	\
	ar7240_reg_rd(_phys);		\
} while(0)

#define ar7240_reg_rmw_set(_reg, _mask)	do {				\
	ar7240_reg_wr((_reg), (ar7240_reg_rd((_reg)) | (_mask)));	\
	ar7240_reg_rd((_reg));						\
} while(0)

#define ar7240_reg_rmw_clear(_reg, _mask) do {				\
	ar7240_reg_wr((_reg), (ar7240_reg_rd((_reg)) & ~(_mask)));	\
	ar7240_reg_rd((_reg));						\
} while(0)

#define AR7240_GPIO_FUNCTION_I2S_GPIO_18_22_EN		(1<<29)
#define AR7240_GPIO_FUNCTION_I2S_REFCLKEN		(1<<28)
#define AR7240_GPIO_FUNCTION_I2S_MCKEN			(1<<27)
#define AR7240_GPIO_FUNCTION_I2S0_EN			(1<<26)

#define AR7240_GPIO_FUNCTIONS (0x18000000+0x00040000+0x28)
#define AR7240_GPIO_FUNCTION_2 (0x18000000+0x00040000+0x30)
#define AR7240_GPIO_OE (0x18000000+0x00040000+0x00)


#define I2S_GPIOPIN_MIC					22
#define I2S_GPIOPIN_WS					19
#define I2S_GPIOPIN_SCK					18
#define I2S_GPIOPIN_SD					20
#define I2S_GPIOPIN_OMCLK				21


static struct platform_device *carambola_snd_device;

static struct snd_soc_dai_link carambola_dai = {
	.name = "Carambola2 audio",
	.stream_name = "Carambola2 audio",
	.cpu_dai_name = "ath79-i2s",
	.codec_dai_name = "wm8727-hifi",
	.platform_name = "ath79-pcm-audio",
	.codec_name = "wm8727",
	/* use ops to check startup state */
};

static struct snd_soc_card snd_soc_carambola = {
	.name = "ATH Carambola2",
	.long_name = "Carambola2 - ath79-pcm/ath79-i2s/wm8727",
	.dai_link = &carambola_dai,
	.num_links = 1,
};

static void prepare_carambola2_i2s(void)
{
	/*
	 * FIXME: Beautify this!
	 *
	 * */
    ar7240_reg_rmw_set(AR7240_GPIO_FUNCTIONS, (AR7240_GPIO_FUNCTION_I2S_GPIO_18_22_EN |
    										   AR7240_GPIO_FUNCTION_I2S_MCKEN |
											   AR7240_GPIO_FUNCTION_I2S0_EN
											   /* | AR7240_GPIO_FUNCTION_JTAG_DISABLE */));

	/* Enable the SPDIF output on GPIO23 */
    ar7240_reg_rmw_set(AR7240_GPIO_FUNCTION_2, (1<<2));
	// Set GPIO_OE
    ar7240_reg_rmw_set(AR7240_GPIO_OE, (1<<I2S_GPIOPIN_SCK) |
    								   (1<<I2S_GPIOPIN_WS) |
									   (1<<I2S_GPIOPIN_SD) |
									   (1<<I2S_GPIOPIN_OMCLK));
    ar7240_reg_rmw_clear(AR7240_GPIO_OE, (1<<I2S_GPIOPIN_MIC));
}

static int __init carmabola2_init(void)
{
	int ret;
	prepare_carambola2_i2s();

	carambola_snd_device = platform_device_alloc("soc-audio", -1);
	if(!carambola_snd_device)
		return -ENOMEM;

	platform_set_drvdata(carambola_snd_device, &snd_soc_carambola);
	ret = platform_device_add(carambola_snd_device);

	if (ret) {
		platform_device_put(carambola_snd_device);
	}

	return ret;
}

static void __exit carmabola2_exit(void)
{
	platform_device_unregister(carambola_snd_device);
}

module_init(carmabola2_init);
module_exit(carmabola2_exit);

MODULE_AUTHOR("Franz Flasch <franz.flasch@gmx.at>");
MODULE_DESCRIPTION("Carambola2 Audio Machine module");
MODULE_LICENSE("Dual BSD/GPL");
