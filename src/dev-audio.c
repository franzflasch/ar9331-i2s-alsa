/*
 *  Atheros AR71xx Audio driver code
 *
 *  Copyright (c) 2013 The Linux Foundation. All rights reserved.
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#include <linux/kernel.h>
#include <linux/platform_device.h>
#include <linux/module.h>

#include <asm/mach-ath79/ar71xx_regs.h>
#include <asm/mach-ath79/ath79.h>

#include "dev-audio.h"
#include "ath79-i2s.h"

#define AR934X_DMA_BASE		(AR71XX_APB_BASE + 0x000A0000)
#define AR934X_DMA_SIZE		0x6C

void __iomem *ath79_dma_io_base;
EXPORT_SYMBOL_GPL(ath79_dma_io_base);

void __iomem *ath79_stereo_base;
EXPORT_SYMBOL_GPL(ath79_stereo_base);

static struct platform_device ath79_i2s_device = {
	.name		= "ath79-i2s",
	.id		= -1,
};

static struct platform_device ath79_pcm_device = {
	.name		= "ath79-pcm-audio",
	.id		= -1,
};

static struct platform_device wm8727_codec = {
	.name		= "wm8727",
	.id		= -1,
};

void __init ath79_audio_device_register(void)
{
	platform_device_register(&ath79_i2s_device);
	platform_device_register(&ath79_pcm_device);
	platform_device_register(&wm8727_codec);
}

void __init ath79_audio_setup(void)
{
	ath79_dma_io_base = ioremap_nocache(AR934X_DMA_BASE,AR934X_DMA_SIZE);
	ath79_stereo_base = ioremap_nocache(AR934X_STEREO_BASE,AR934X_STEREO_SIZE);
}


static int ath_audio_init(void)
{
   ath79_audio_setup();
   ath79_audio_device_register();
   printk(KERN_ALERT "ath_audio_init\n");
   return 0;
}


static void ath_audio_exit(void)
{
   printk(KERN_ALERT "ath_audio_exit\n");
}


module_init(ath_audio_init);
module_exit(ath_audio_exit);

MODULE_LICENSE("GPL");

