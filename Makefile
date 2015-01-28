#
# Copyright (C) 2013 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=ath79-i2s
PKG_RELEASE:=1

include $(INCLUDE_DIR)/package.mk

define KernelPackage/ath79-i2s
	SUBMENU:=i2s alsa modules
	TITLE:=Atheros 933X I2S module
	DEPENDS:=@AUDIO_SUPPORT +kmod-sound-soc-core +kmod-ath79-i2s-dev
	FILES:=$(PKG_BUILD_DIR)/ath79-i2s.ko
	KCONFIG:=
endef

define KernelPackage/ath79-i2s/description
	Kernel module for atheros ar933x i2s support
endef

define KernelPackage/ath79-pcm
	SUBMENU:=i2s alsa modules
	TITLE:=Atheros 933X PCM module
	DEPENDS:=@AUDIO_SUPPORT +kmod-sound-soc-core +kmod-ath79-i2s-dev
	FILES:=$(PKG_BUILD_DIR)/ath79-pcm-mbox.ko
	KCONFIG:=
endef

define KernelPackage/ath79-pcm/description
	Kernel module for atheros ar933x pcm support
endef

define KernelPackage/ath79-i2s-dev
	SUBMENU:=i2s alsa modules
	TITLE:=Atheros 933X I2S DEV module
	DEPENDS:=@AUDIO_SUPPORT +kmod-ath79-i2s
	FILES:=$(PKG_BUILD_DIR)/dev-audio.ko
	KCONFIG:=
endef

define KernelPackage/ath79-i2s/description
	Kernel module for atheros ar933x i2s dev support
endef

define KernelPackage/ath79-wm8727
	SUBMENU:=i2s alsa modules
	TITLE:=Atheros 933X wm8727 codec
	DEPENDS:=@AUDIO_SUPPORT +kmod-ath79-i2s +kmod-ath79-pcm
	FILES:=$(PKG_BUILD_DIR)/wm8727.ko
	KCONFIG:=
endef

define KernelPackage/ath79-wm8727/description
	Kernel module for atheros ar933x wm8727 codec support
endef

define KernelPackage/ath79-carambola2
	SUBMENU:=i2s alsa modules
	TITLE:=Atheros 933X carambola 2 clue code
	DEPENDS:=@AUDIO_SUPPORT +kmod-ath79-i2s +kmod-ath79-pcm +kmod-ath79-wm8727
	FILES:=$(PKG_BUILD_DIR)/ath-carambola2.ko
	KCONFIG:=
endef

define KernelPackage/ath79-ath-carambola2/description
	Kernel module for atheros ar933x ath-carambola2 clue code
endef



EXTRA_KCONFIG:= \
	CONFIG_AR9331_PCM=m \
	CONFIG_AR9331_I2S=m \
	CONFIG_AR9331_I2S_DEV=m \
	CONFIG_AR9331_WM8727=m \
	CONFIG_AR9331_CARAMBOLA2=m

EXTRA_CFLAGS:= \
	$(patsubst CONFIG_%, -DCONFIG_%=1, $(patsubst %=m,%,$(filter %=m,$(EXTRA_KCONFIG)))) \
	$(patsubst CONFIG_%, -DCONFIG_%=1, $(patsubst %=y,%,$(filter %=y,$(EXTRA_KCONFIG)))) \

MAKE_OPTS:= \
	ARCH="$(LINUX_KARCH)" \
	CROSS_COMPILE="$(TARGET_CROSS)" \
	SUBDIRS="$(PKG_BUILD_DIR)" \
	EXTRA_CFLAGS="$(EXTRA_CFLAGS)" \
	$(EXTRA_KCONFIG)

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
endef

define Build/Compile
	$(MAKE) -C "$(LINUX_DIR)" \
	$(MAKE_OPTS) \
	modules
endef

$(eval $(call KernelPackage,ath79-pcm))
$(eval $(call KernelPackage,ath79-i2s))
$(eval $(call KernelPackage,ath79-i2s-dev))
$(eval $(call KernelPackage,ath79-wm8727))
$(eval $(call KernelPackage,ath79-carambola2))
