CELL_MK_DIR = $(CELL_SDK)/samples/mk
include $(CELL_MK_DIR)/sdk.makedef.mk

BUILD_TYPE     	= release

LIBSTUB_DIR		= ./lib
PRX_DIR			= .
PEXPORTPICKUP		= ppu-lv2-prx-exportpickup

CRT_HEAD                += $(shell ppu-lv2-gcc -print-file-name'='ecrti.o)
CRT_HEAD                += $(shell ppu-lv2-gcc -print-file-name'='crtbegin.o)
CRT_TAIL                += $(shell ppu-lv2-gcc -print-file-name'='crtend.o)
CRT_HEAD                += $(shell ppu-lv2-gcc -print-file-name'='ecrtn.o)

OBJS_DIR = obj
PPU_SRCS = main.c $(wildcard lua/*.c)
PPU_INCDIRS	= -I vsh
PPU_PRX_TARGET = lua-prx.prx
PPU_PRX_LDFLAGS = -L ./lib -Wl,--strip-unused-data
PPU_PRX_STRIP_FLAGS = -s
PPU_PRX_LDLIBS 	= -lfs_stub -lnet_stub -lrtc_stub -lio_stub -lgcm_sys_stub
PPU_PRX_LDLIBS 	+= -lallocator_export_stub -lstdc_export_stub
PPU_PRX_LDLIBS 	+= -lnetctl_stub -lsysmodule_stub -lhttp_util_stub -lhttp_stub 
PPU_PRX_LDLIBS 	+= -lcrashdump_system_export_stub \
                   -lsysPrxForUser_export_stub \
                   -lvsh_export_stub \
                   -lpaf_export_stub \
                   -lvshmain_export_stub \
                   -lvshtask_export_stub \
                   -lsdk_export_stub \
                   -lpngdec_ppuonly_export_stub \
                   -lxsetting_export_stub \
                   -lvshnet_export_stub \
                   -lnetctl_main_export_stub



PPU_CFLAGS += -Os -ffunction-sections -fdata-sections \
              -fno-builtin-printf -nodefaultlibs -std=gnu99 \
              -Wno-shadow -Wno-unused-parameter
			  

CLEANFILES = $(PRX_DIR)/$(PPU_SPRX_TARGET)

all:
	$(MAKE) $(PPU_OBJS_DEPENDS)
	$(PPU_PRX_STRIP) --strip-debug --strip-section-header $(PPU_PRX_TARGET)
	scetool -0 SELF -1 TRUE -s FALSE -2 04 -3 1070000052000001 -4 01000002 -5 APP -6 0003004000000000 -A 0001000000000000 -e $(PPU_PRX_TARGET) $(PPU_SPRX_TARGET)
	
include $(CELL_MK_DIR)/sdk.target.mk




