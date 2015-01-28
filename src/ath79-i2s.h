/*
 * ath-i2s.h -- ALSA DAI (i2s) interface for the QCA Wasp based audio interface
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

#ifndef _ATH_I2S_H_
#define _ATH_I2S_H_

#include <linux/spinlock.h>
#include <sound/core.h>
#include <sound/soc.h>
#include <sound/soc-dai.h>

#define AR934X_STEREO_BASE	(AR71XX_APB_BASE + 0x000B0000)
#define AR934X_STEREO_SIZE	0x18

#define	AR934X_STEREO_REG_CONFIG			0x00
#define	AR934X_STEREO_CONFIG_SPDIF_ENABLE		BIT(23)
#define AR934X_STEREO_CONFIG_I2S_ENABLE			BIT(21)
#define AR934X_STEREO_CONFIG_MIC_RESET			BIT(20)
#define AR934X_STEREO_CONFIG_RESET			BIT(19)
#define AR934X_STEREO_CONFIG_I2S_DELAY			BIT(18)
#define AR934X_STEREO_CONFIG_PCM_SWAP			BIT(17)
#define AR934X_STEREO_CONFIG_MIC_WORD_SIZE		BIT(16)
#define AR934X_STEREO_CONFIG_STEREO_MONO_SHIFT		14
#define AR934X_STEREO_CONFIG_STEREO_MONO_MASK		0x03
#define AR934X_STEREO_CONFIG_STEREO_MONO_STEREO		0
#define AR934X_STEREO_CONFIG_STEREO_MONO_CH0		1
#define AR934X_STEREO_CONFIG_STEREO_MONO_CH1		2

#define AR934X_STEREO_CONFIG_DATA_WORD_SIZE_SHIFT	12
#define AR934X_STEREO_CONFIG_DATA_WORD_SIZE_MASK	0x03
#define AR934X_STEREO_CONFIG_DATA_WORD_8		0
#define AR934X_STEREO_CONFIG_DATA_WORD_16		1
#define AR934X_STEREO_CONFIG_DATA_WORD_24		2
#define AR934X_STEREO_CONFIG_DATA_WORD_32		3

#define AR934X_STEREO_CONFIG_I2S_WORD_SIZE		BIT(11)
#define AR934X_STEREO_CONFIG_MCK_SEL			BIT(10)
#define AR934X_STEREO_CONFIG_SAMPLE_CNT_CLEAR_TYPE	BIT(9)
#define AR934X_STEREO_CONFIG_MASTER			BIT(8)
#define AR934X_STEREO_CONFIG_POSEDGE_SHIFT		0
#define AR934X_STEREO_CONFIG_POSEDGE_MASK		0xff

#define	AR934X_STEREO_REG_VOLUME			0x04

#define AR934X_STEREO_VOLUME_CH0			8
#define AR934X_STEREO_VOLUME_CH1			0
#define AR934X_STEREO_VOLUME_MASK			0x1f

#define	AR934X_STEREO_REG_MASTER_CLOCK			0x08
#define	AR934X_STEREO_REG_TX_SAMPLE_CNT_LSB		0x0c
#define	AR934X_STEREO_REG_TX_SAMPLE_CNT_MSB		0x10
#define	AR934X_STEREO_REG_RX_SAMPLE_CNT_LSB		0x14
#define	AR934X_STEREO_REG_RX_SAMPLE_CNT_MSB 		0x18

/* PLL */
struct ath79_snd_clk_config
{
	u32 freq;
	u16 divint;
	u16 divfrac;
	u16 posedgde;
};

extern void __iomem *ath79_stereo_base;
extern spinlock_t ath79_stereo_lock;

static inline void ath79_stereo_wr(unsigned reg, u32 val)
{
	__raw_writel(val, ath79_stereo_base + reg);
}

static inline u32 ath79_stereo_rr(unsigned reg)
{
	return __raw_readl(ath79_stereo_base + reg);
}
void ath79_stereo_reset(void);

#endif
