#include <pspkernel.h>
#include <pspdisplay.h>
#include <stdlib.h>
#include <pspthreadman.h>
#include <stdio.h>
#include <string.h>
#include <pspmath.h>
#include <psploadcore.h>
#include <pspiofilemgr.h>
#include <pspctrl.h>
#include <malloc.h>
#include "include/luaPlayer.h"

// Needed libs
#include "extralibs.c"

#define MAX_THREAD 64
/* Define the module info section */
PSP_MODULE_INFO("LuaPlayer Plus", 1, 1, 1);
PSP_MAIN_THREAD_ATTR(0);

// Important Plugins definitions
static thid1 = -1;

int go=0;

int debugOutput(const char *format, ...)
{
        va_list opt;
        char buffer[2048];
        int bufsz;
        va_start(opt, format);
        bufsz = vsnprintf( buffer, (size_t) sizeof(buffer), format, opt);
        return pspDebugScreenPrintData(buffer, bufsz);
}



int main_thread(SceSize args, void *argp) {
sceKernelDelayThread(3000000);
                    SceCtrlData pad;
    u32 oldButtons = 0;
	u32 keycombination = 0;
	 
	 keycombination = PSP_CTRL_LTRIGGER; //Button to start interpreter
	
	while(1){
            oldButtons = pad.Buttons;
            if (go==0){
             sceCtrlPeekBufferPositive(&pad, 1);
                          if(oldButtons != pad.Buttons)
                          {
            if(pad.Buttons & keycombination)
            {
                pauseGame(thid1);
                    go=1;
					pspDebugScreenInit();
                    pspDebugScreenClear();
                    oldButtons = pad.Buttons;
            }
            }
            }
                if (go ==1){
    pspDebugScreenSetXY(0,0);
    pspDebugScreenSetTextColor(0xffffff);
	
	int go2=1;
	SceUID id = sceIoDopen("ms0:/seplugins/script");
				 SceIoDirent entry;
				 int script_files = -2;
				 memset(&entry, 0, sizeof(SceIoDirent)); 
			     while (sceIoDread(id, &entry) > 0)
				{
					script_files = script_files+1;
					memset(&entry, 0, sizeof(SceIoDirent)); 
				}
				sceIoDclose(id); 
				char *script = "ms0:/seplugins/script/index.lua";
        while(go2==1)
        {
				 
				const char *errMsg;
				if (script_files>1){
				errMsg = runScript(extralibs, true);
				}else{ 
                errMsg = runScript(script, false);
				}
				// Temp replacing for loadfile/dofile functions: System.protodofile
				if (strstr(errMsg, "lpp_open")){ 
				int i;
				for(i = 0; i < 40; i++) sceDisplayWaitVblankStart();
				char dum1[20], dum2[20], dum3[20], script_path[200];
				sscanf( errMsg, "%s %s %s %s", dum1, dum2, dum3, script_path );
				strcpy(script,script_path);
				script_files=1;
				// End System.protodofile sources
				}else if (strstr(errMsg, "resumeThread")){
				go=0;
				go2=0;
				}else{
                if (errMsg != NULL);
                {
						pspDebugScreenClear();
						pspDebugScreenSetTextColor(0xffffff);
                        debugOutput("\nError: %s\n", errMsg);
                }
                debugOutput("\nPress start to restart\nPress select to resume thread\n");

                SceCtrlData pad; int i;
                
                for(i = 0; i < 40; i++) sceDisplayWaitVblankStart();
				int restore = 0;
                while(restore==0){
				sceCtrlPeekBufferPositive(&pad, 1);
				if (pad.Buttons&PSP_CTRL_START){
				restore=1;
				go2=0;
				}
				if (pad.Buttons&PSP_CTRL_SELECT){
				resumeGame(thid1);
				restore=1;
				go=0;
				go2=0;
				}
				}
				}
        }


}
sceDisplayWaitVblankStart();
        }
	
	sceKernelSleepThread();
return 0;
}

int module_start(SceSize args, void *argp) {

	int thid;

	/* Create a high priority thread */
	thid = sceKernelCreateThread("LPP", main_thread, 0x18, 0x1000, 0, NULL);//8, 64*1024, PSP_THREAD_ATTR_USER, NULL);
	if(thid >= 0) sceKernelStartThread(thid, args, argp);

	return 0;
}