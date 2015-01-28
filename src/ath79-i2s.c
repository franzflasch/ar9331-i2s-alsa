/*
 * ath79-i2s.c -- ALSA DAI (i2s) interface for the QCA Wasp based audio interface
 *
 * Copyright (c) 2012 The Linux Foundation. All rights reserved.
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

#include <linux/module.h>
#include <linux/spinlock.h>
#include <sound/core.h>
#include <sound/soc.h>
#include <sound/soc-dai.h>
#include <sound/pcm_params.h>

#include <asm/mach-ath79/ar71xx_regs.h>
#include <asm/mach-ath79/ath79.h>

#include "ath79-i2s.h"

#define DRV_NAME	"ath79-i2s"


/*
 * FIXME: Remove these ugly macros!
 *
 * */
#include <linux/gpio.h>
#include <../arch/mips/ath79/common.h>
#include <../arch/mips/include/asm/mach-ath79/gpio.h>
#include <linux/delay.h>

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

#define AR7240_STEREO_CLK_DIV (0x18000000+0x000B0000+0x1c)
#define AR7240_STEREO_CONFIG (0x18000000+0x000B0000+0x00)
#define AR7240_STEREO_CONFIG_PSEDGE(x)			(0xff&x)

#define I2S_GPIOPIN_MIC					22
#define I2S_GPIOPIN_WS					19
#define I2S_GPIOPIN_SCK					18
#define I2S_GPIOPIN_SD					20
#define I2S_GPIOPIN_OMCLK				21



DEFINE_SPINLOCK(ath79_stereo_lock);

static const struct ath79_snd_clk_config clk_cfg[] = {
       /* freq,        divint,         divfrac,        posedge */
       { 44100,        0x11,           0xB726,         0x2 },
       { 48000,        0x10,           0x46AB,         0x2 },
};

void ath79_stereo_reset(void)
{
	u32 t;
	printk("%s\n", __FUNCTION__);
	spin_lock(&ath79_stereo_lock);
	t = ath79_stereo_rr(AR934X_STEREO_REG_CONFIG);
	t |= AR934X_STEREO_CONFIG_RESET;
	ath79_stereo_wr(AR934X_STEREO_REG_CONFIG, t);
	spin_unlock(&ath79_stereo_lock);
}
EXPORT_SYMBOL(ath79_stereo_reset);

static int ath79_i2s_startup(struct snd_pcm_substream *substream,
			      struct snd_soc_dai *dai)
{
	printk("%s\n", __FUNCTION__);
	/* Enable I2S and SPDIF by default */
	if (!dai->active) {
		ath79_stereo_wr(AR934X_STEREO_REG_CONFIG,
				AR934X_STEREO_CONFIG_SPDIF_ENABLE |
				AR934X_STEREO_CONFIG_I2S_ENABLE |
				AR934X_STEREO_CONFIG_SAMPLE_CNT_CLEAR_TYPE |
				AR934X_STEREO_CONFIG_MASTER);
		ath79_stereo_reset();
	}
	return 0;
}

static void ath79_i2s_shutdown(struct snd_pcm_substream *substream,
				struct snd_soc_dai *dai)
{
	printk("%s\n", __FUNCTION__);
	if (!dai->active) {
		ath79_stereo_wr(AR934X_STEREO_REG_CONFIG, 0);
	}
	return;
}

static int ath79_i2s_trigger(struct snd_pcm_substream *substream, int cmd,
			      struct snd_soc_dai *dai)
{
	printk("%s\n", __FUNCTION__);
	return 0;
}

static int ath79_i2s_hw_params(struct snd_pcm_substream *substream,
				struct snd_pcm_hw_params *params,
				struct snd_soc_dai *dai)
{
	u32 mask = 0, t;
	int clk_cfg_idx = 0;
	int clk_cfg_array_size =sizeof(clk_cfg)/(sizeof(*clk_cfg));
	printk("%s\n", __FUNCTION__);

	/* find right frequency setting: */
	for(clk_cfg_idx=0;clk_cfg_idx<(sizeof(clk_cfg)/(sizeof(struct ath79_snd_clk_config)));clk_cfg_idx++)
	{
		if(params_rate(params) == clk_cfg[clk_cfg_idx].freq)
		{
			printk(KERN_NOTICE "Set clk config: %x-%x-%x for freq: %d\n", clk_cfg[clk_cfg_idx].divint, clk_cfg[clk_cfg_idx].divfrac, clk_cfg[clk_cfg_idx].posedgde, clk_cfg[clk_cfg_idx].freq);
			break;
		}
	}

	if(clk_cfg_idx == clk_cfg_array_size)
	{
		printk(KERN_ERR "No valid clock config found for frequency %d\n", params_rate(params));
		return -ENOTSUPP;
	}
	ar7240_reg_wr(AR7240_STEREO_CLK_DIV, ((clk_cfg[clk_cfg_idx].divint << 16) + clk_cfg[clk_cfg_idx].divfrac));

	switch(params_format(params)) 
	{
		case SNDRV_PCM_FORMAT_S8:
				mask |= AR934X_STEREO_CONFIG_DATA_WORD_8 << AR934X_STEREO_CONFIG_DATA_WORD_SIZE_SHIFT;
				break;
		case SNDRV_PCM_FORMAT_S16_LE:
				mask |= AR934X_STEREO_CONFIG_PCM_SWAP;
		case SNDRV_PCM_FORMAT_S16_BE:
				mask |= AR934X_STEREO_CONFIG_DATA_WORD_16 << AR934X_STEREO_CONFIG_DATA_WORD_SIZE_SHIFT;
				break;
		default:
				printk(KERN_ERR "%s: Format %d not supported\n",__FUNCTION__, params_format(params));
				return -ENOTSUPP;
	}

	spin_lock(&ath79_stereo_lock);
	t = ath79_stereo_rr(AR934X_STEREO_REG_CONFIG);
	t &= ~(AR934X_STEREO_CONFIG_DATA_WORD_SIZE_MASK
		<< AR934X_STEREO_CONFIG_DATA_WORD_SIZE_SHIFT);
	t &= ~(AR934X_STEREO_CONFIG_I2S_WORD_SIZE);
	t |= mask;
	t |= AR7240_STEREO_CONFIG_PSEDGE(2);
	ath79_stereo_wr(AR934X_STEREO_REG_CONFIG, t);
	spin_unlock(&ath79_stereo_lock);

	ath79_stereo_reset();
	return 0;
}

static struct snd_soc_dai_ops ath79_i2s_dai_ops = {
	.startup	= ath79_i2s_startup,
	.shutdown	= ath79_i2s_shutdown,
	.trigger	= ath79_i2s_trigger,
	.hw_params	= ath79_i2s_hw_params,
};

static struct snd_soc_dai_driver ath79_i2s_dai = {
	.name = "ath79-i2s",
	.id = 0,
	.playback = {
		.channels_min = 2,
		.channels_max = 2,
		.rates = SNDRV_PCM_RATE_44100 | SNDRV_PCM_RATE_48000,
/* For now, we'll just support 8 and 16bits as 32 bits is really noisy
 * for some reason */
		.formats = SNDRV_PCM_FMTBIT_S8 | SNDRV_PCM_FMTBIT_S16_BE | SNDRV_PCM_FMTBIT_S16_LE,
		},
	.capture = {
		.channels_min = 1,
		.channels_max = 2,
		.rates = SNDRV_PCM_RATE_44100 |	SNDRV_PCM_RATE_48000,
/* For now, we'll just support 8 and 16bits as 32 bits is really noisy
 * for some reason */
		.formats = SNDRV_PCM_FMTBIT_S8 | SNDRV_PCM_FMTBIT_S16_BE | SNDRV_PCM_FMTBIT_S16_LE,
		},
	.ops = &ath79_i2s_dai_ops,
};

static const struct snd_soc_component_driver ath79_i2s_component = {
        .name           = "ath79-i2s",
};

static int ath79_i2s_drv_probe(struct platform_device *pdev)
{
	printk("%s\n", __FUNCTION__);
	spin_lock_init(&ath79_stereo_lock);

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


	ath79_stereo_reset();
	return snd_soc_register_component(&pdev->dev, &ath79_i2s_component, &ath79_i2s_dai, 1);
}

static int ath79_i2s_drv_remove(struct platform_device *pdev)
{
	printk("%s\n", __FUNCTION__);
	snd_soc_unregister_component(&pdev->dev);
	return 0;
}

static struct platform_driver ath79_i2s_driver = {
	.probe = ath79_i2s_drv_probe,
	.remove = ath79_i2s_drv_remove,

	.driver = {
		.name = DRV_NAME,
		.owner = THIS_MODULE,
	},
};

module_platform_driver(ath79_i2s_driver);

MODULE_AUTHOR("Qualcomm-Atheros Inc.");
MODULE_AUTHOR("Mathieu Olivari <mathieu@qca.qualcomm.com>");
MODULE_AUTHOR("Franz Flasch <franz.flasch@gmx.at>");
MODULE_DESCRIPTION("QCA Audio DAI (i2s) module");
MODULE_LICENSE("Dual BSD/GPL");
MODULE_ALIAS("platform:" DRV_NAME);
