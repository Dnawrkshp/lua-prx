#include <sdk_version.h>
#include <cellstatus.h>
#include <cell/cell_fs.h>
#include <cell/sysmodule.h>
#include <cell/rtc.h>
#include <cell/pad.h>
#include <cell/font.h>
#include <cell/l10n.h>
#include <cell/gcm.h>

#include <sysutil/sysutil_common.h>

#include <sys/prx.h>
#include <sys/return_code.h>
#include <sys/ppu_thread.h>
#include <sys/event.h>
#include <sys/syscall.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/memory.h>
#include <sys/timer.h>

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <math.h>

#include "vsh_exports.h"
#include "lua/lua.h"
#include "lua/lualib.h"
#include "lua/lauxlib.h"

SYS_MODULE_INFO(LUA, 0, 1, 0);
SYS_MODULE_START(lua_start);
SYS_MODULE_STOP(lua_stop);

#define THREAD_NAME "t-lua"
#define STOP_THREAD_NAME "s-lua"

static sys_ppu_thread_t thread_id = 1;

int lua_start(uint64_t arg);
int lua_stop(void);
static void stop_prx_module(void);

lua_State *L = NULL;

static inline void _sys_ppu_thread_exit(uint64_t val)
{
	system_call_1(41, val);
}

static inline sys_prx_id_t prx_get_module_id_by_address(void *addr)
{
	system_call_1(461, (uint64_t)(uint32_t)addr);
	return (int)p1;
}

static void lua_thread(uint64_t arg) {
	// Create lua state
	L = luaL_newstate();
	if (L == NULL) {
		printf("Error creating lua state!\n");
		goto exit;
	}

	// initialize standard libs
	luaL_openlibs(L);

	// load script
	char *filebuf = (char*)malloc(1000);
	FILE *fp = fopen("/dev_usb000/test.lua", "r");
	if (!fp) {
		printf("Error reading /dev_usb000/test.lua!\n");
		goto cleanup;
	}

	int size = fread(filebuf, 1, 1000, fp);
	filebuf[size] = 0;
	fclose(fp);


	// Run script
	printf("Running lua script:\n %s\n\n", filebuf);
	if (luaL_loadbuffer(L, filebuf, strlen(filebuf), "lua") != 0 || lua_pcall(L, 0, 0, 0) != 0) {
		printf("Error loading: %s\n", lua_tostring(L, -1));
	}


	cleanup:;
	free(filebuf);
	lua_close(L);

	exit:;
	stop_prx_module();
	uint64_t exit_code;
	sys_ppu_thread_join(thread_id, &exit_code);

	sys_ppu_thread_exit(0);
}

int lua_start(uint64_t arg)
{
	sys_ppu_thread_create(&thread_id, lua_thread, NULL, 3000, 0x4000, SYS_PPU_THREAD_CREATE_JOINABLE, THREAD_NAME);
	_sys_ppu_thread_exit(0);
	return SYS_PRX_RESIDENT;
}

static void stop_prx_module(void)
{
	uint64_t meminfo[5];

	sys_prx_id_t prx = prx_get_module_id_by_address(stop_prx_module);

	meminfo[0] = 0x28;
	meminfo[1] = 2;
	meminfo[3] = 0;

	system_call_3(482, prx, 0, (uint64_t)(uint32_t)meminfo);
}

static void unload_prx_module(void)
{
	sys_prx_id_t prx = prx_get_module_id_by_address(unload_prx_module);

	{system_call_3(483, prx, 0, NULL);}
}

int lua_stop(void)
{
	uint64_t exit_code;
	sys_ppu_thread_join(thread_id, &exit_code);

	sys_timer_usleep(500000);

	stop_prx_module();
	unload_prx_module();

	_sys_ppu_thread_exit(0);
	return SYS_PRX_STOP_OK;
}
