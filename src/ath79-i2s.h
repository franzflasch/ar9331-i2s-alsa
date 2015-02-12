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
