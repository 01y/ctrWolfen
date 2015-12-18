#include "include/wl_def.h"
#include <3ds.h>

byte *gfxbuf = NULL;
static unsigned char pal[768];
u16* fb;

extern void keyboard_handler(int code, int press);
extern boolean InternalKeyboard[NumCodes];

u16 d_8to16table[256];
vwidth = 400;
vheight = 240;
vstride = 400;

int main (int argc, char *argv[])
{
	aptInit();
	aptOpenSession();
	Result ret=APT_SetAppCpuTimeLimit(30);
	aptCloseSession();
	fsInit();
	gfxInit(GSP_RGB565_OES,GSP_RGB565_OES,false);
	hidInit();
	gfxSetDoubleBuffering(GFX_TOP, false);
	gfxSetDoubleBuffering(GFX_BOTTOM, false);
	gfxSet3D(false);
	consoleInit(GFX_BOTTOM, NULL);
	return WolfMain(argc, argv);
}

void DisplayTextSplash(byte *text);

/*
==========================
=
= Quit
=
==========================
*/

void exitGame(){
	gfxExit();
	hidExit();
	fsExit();
	aptExit();
	exit(EXIT_FAILURE);
}

void Quit(char *error)
{
	memptr screen = NULL;

	if (!error || !*error) {
		CA_CacheGrChunk(ORDERSCREEN);
		screen = grsegs[ORDERSCREEN];
		WriteConfig();
	} else if (error) {
		CA_CacheGrChunk(ERRORSCREEN);
		screen = grsegs[ERRORSCREEN];
	}

	ShutdownId();

	if (screen) {
		//DisplayTextSplash(screen);
	}
	
	if (error && *error) {
		printf("Fatal Error: %s\nPress A to exit\n\n", error);
		do{hidScanInput();}while(!(hidKeysDown() & KEY_A));				
 	}
	
	exitGame();
}

void VL_WaitVBL(int vbls)
{
	long last = get_TimeCount() + vbls;
	while (last > get_TimeCount()) ;
}

void VW_UpdateScreen()
{
	int x,y;
	for(x=0; x<vwidth; x++){
		for(y=0; y<vheight;y++){
			fb[((x*240) + (239 - y))] = d_8to16table[gfxbuf[y*vwidth + x]];
		}
	}
}

/*
=======================
=
= VL_Startup
=
=======================
*/

void VL_Startup()
{
	printf("Starting rendering...");
	gfxbuf = malloc(vwidth * vheight);
	fb = gfxGetFramebuffer(GFX_TOP, GFX_LEFT, NULL, NULL);
	printf("Done!\n");
}

/*
=======================
=
= VL_Shutdown
=
=======================
*/

void VL_Shutdown()
{
}

/* ======================================================================== */

/*
=================
=
= VL_SetPalette
=
=================
*/

void VL_SetPalette(const byte *palette)
{
	VL_WaitVBL(1);
	memcpy(pal, palette, 768);
	int i;
	u8 *pal = palette;
	u16 *table = d_8to16table;
	unsigned r, g, b;
	for(i=0; i<256; i++){
		r = pal[0];
		g = pal[1];
		b = pal[2];
		table[0] = RGB8_to_565(r,g,b);
		table++;
		pal += 3;
	}
}

/*
=================
=
= VL_GetPalette
=
=================
*/

void VL_GetPalette(byte *palette)
{
	memcpy(palette, pal, 768);
}

/*
=================
=
= INL_Update
=
=================
*/
static int mx = 0;
static int my = 0;

void INL_SetKeys(u32 keys, u32 state){
	if ((keys & KEY_SELECT) && (state == 0)){
		keyboard_handler(sc_1, 0); 
		keyboard_handler(sc_2, 0); 
		keyboard_handler(sc_3, 0); 
		keyboard_handler(sc_4, 0); 
	}else if((keys & KEY_SELECT) && (state == 1)){
		keyboard_handler(sc_Y, 0); 
		keyboard_handler(sc_Control, 0); 
		keyboard_handler(sc_RShift, 0); 
		keyboard_handler(sc_Space, 0); 
		keyboard_handler(sc_BackSpace, 0);
		if( keys & KEY_A){ // Weapon 1
			keyboard_handler(sc_1, state); 
		}
		if( keys & KEY_X){ // Weapon 2
			keyboard_handler(sc_2, state);
		}
		if( keys & KEY_Y){ // Weapon 3
			keyboard_handler(sc_3, state);
		}
		if( keys & KEY_B){ // Weapon 4
			keyboard_handler(sc_4, state);
		}
	}else{
		if( keys & KEY_A){ // Yes button/Fire
			keyboard_handler(sc_Y, state); 
			keyboard_handler(sc_Control, state);
		}
		if( keys & KEY_X){ // Run
			keyboard_handler(sc_RShift, state);
		}
		if( keys & KEY_Y){ // Open/Operate
			keyboard_handler(sc_Space, state);
		}
		if( keys & KEY_B){ // Backspace
			keyboard_handler(sc_BackSpace, state);
		}
	}
	if( keys & KEY_START){ // Open menu
		keyboard_handler(sc_Escape, state);
	}
	if( keys & KEY_DUP){
		keyboard_handler(sc_UpArrow, state);
	}
	if( keys & KEY_DDOWN){
		keyboard_handler(sc_DownArrow, state);
	}
	if( keys & KEY_DLEFT){
		keyboard_handler(sc_LeftArrow, state);
	}
	if( keys & KEY_DRIGHT){
		keyboard_handler(sc_RightArrow, state);
	}
	if( keys & KEY_L){ // Move left
		keyboard_handler(sc_LeftArrow, state);
	}
	if( keys & KEY_R){ // Move right
		keyboard_handler(sc_RightArrow, state);
	}
	if( keys & KEY_ZL){ // Move left
		keyboard_handler(sc_LeftArrow, state);
	}
	if( keys & KEY_ZR){ // Move right
		keyboard_handler(sc_RightArrow, state);
	}
}

void INL_Update()
{
	static u32 buttons = 0;
	static int multiplex = 0;
	u32 new_buttons;
	static int mu = 0;
	static int md = 0;
	
	hidScanInput();
	u32 kDown = hidKeysDown();
	u32 kUp = hidKeysUp();
	if(kDown)
		INL_SetKeys(kDown, 1);
	if(kUp)
		INL_SetKeys(kUp, 0);
	
}

void IN_GetMouseDelta(int *dx, int *dy)
{
	if (dx)
		*dx = mx;
	if (dy)
		*dy = my;
}

byte IN_MouseButtons()
{
	return 0;
}
