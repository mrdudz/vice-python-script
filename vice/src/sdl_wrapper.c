
#include <stdio.h>

#include <SDL/SDL.h>
#include <dlfcn.h>

#include <Python.h>

typedef unsigned short WORD;
typedef unsigned char BYTE;

void mem_store(WORD addr, BYTE value);
BYTE mem_read(WORD addr);


int (*pSDL_OpenAudio)(SDL_AudioSpec *, SDL_AudioSpec *);
void (*pSDL_CloseAudio)(void);
void (*pSDL_PauseAudio)(int pause_on);
int (*pSDL_GL_SetAttribute)(SDL_GLattr attr, int value);
void (*pSDL_GL_SwapBuffers)(void);
Uint8 (*pSDL_GetAppState)(void);
void (*pSDL_SetError)(const char *fmt, ...);
char* (*pSDL_GetError)(void);
void (*pSDL_ClearError)(void);
Uint32 (*pSDL_GetTicks)(void);
int (*pSDL_Init)(Uint32 flags);
int (*pSDL_InitSubSystem)(Uint32 flags);
SDL_Surface* (*pSDL_CreateRGBSurface)(Uint32 flags, int width, int height, int depth, Uint32 Rmask, Uint32 Gmask, Uint32 Bmask, Uint32 Amask);
SDL_Surface* (*pSDL_CreateRGBSurfaceFrom)(void *pixels, int width, int height, int depth, int pitch, Uint32 Rmask, Uint32 Gmask, Uint32 Bmask, Uint32 Amask);
void (*pSDL_FreeSurface)(SDL_Surface *surface);
int (*pSDL_LockSurface)(SDL_Surface *surface);
void (*pSDL_UnlockSurface)(SDL_Surface *surface);
SDL_GrabMode (*pSDL_WM_GrabInput)(SDL_GrabMode mode);
Uint32 (*pSDL_MapRGB)(const SDL_PixelFormat * const format, const Uint8 r, const Uint8 g, const Uint8 b);
int (*pSDL_PollEvent)(SDL_Event *event);
int (*pSDL_WaitEvent)(SDL_Event *event);
void (*pSDL_Quit)(void);
int (*pSDL_SetColors)(SDL_Surface *surface, SDL_Color *colors, int firstcolor, int ncolors);
SDL_Surface* (*pSDL_SetVideoMode)(int width, int height, int bpp, Uint32 flags);
void (*pSDL_UpdateRects)(SDL_Surface *screen, int numrects, SDL_Rect *rects);
void (*pSDL_UpdateRect)(SDL_Surface *screen, Sint32 x, Sint32 y, Uint32 w, Uint32 h);
int (*pSDL_Flip)(SDL_Surface *screen);
void (*pSDL_SetCursor)(SDL_Cursor *cursor);
SDL_Cursor* (*pSDL_GetCursor)(void);
void (*pSDL_FreeCursor)(SDL_Cursor *cursor);
int (*pSDL_ShowCursor)(int toggle);
void (*pSDL_WM_SetCaption)(const char *title, const char *icon);
SDL_RWops* (*pSDL_RWFromFile)(const char *file, const char *mode);
void (*pSDL_Delay)(Uint32 ms);
char* (*pSDL_GetKeyName)(SDLKey key);
Uint8 (*pSDL_GetMouseState)(int *x, int *y);
int (*pSDL_EnableKeyRepeat)(int delay, int interval);
int (*pSDL_EnableUNICODE)(int enable);
Uint8 (*pSDL_EventState)(Uint8 type, int state);
int (*pSDL_NumJoysticks)(void);
const char* (*pSDL_JoystickName)(int device_index);
SDL_Joystick * (*pSDL_JoystickOpen)(int device_index);
int (*pSDL_JoystickOpened)(int device_index);
int (*pSDL_JoystickIndex)(SDL_Joystick *joystick);
int (*pSDL_JoystickNumAxes)(SDL_Joystick *joystick);
int (*pSDL_JoystickNumBalls)(SDL_Joystick *joystick);
int (*pSDL_JoystickNumHats)(SDL_Joystick *joystick);
int (*pSDL_JoystickNumButtons)(SDL_Joystick *joystick);
void (*pSDL_JoystickUpdate)(void);
int (*pSDL_JoystickEventState)(int state);
Sint16 (*pSDL_JoystickGetAxis)(SDL_Joystick *joystick, int axis);
Uint8 (*pSDL_JoystickGetHat)(SDL_Joystick *joystick, int hat);
int (*pSDL_JoystickGetBall)(SDL_Joystick *joystick, int ball, int *dx, int *dy);
Uint8 (*pSDL_JoystickGetButton)(SDL_Joystick *joystick, int button);
void (*pSDL_JoystickClose)(SDL_Joystick *joystick);
int (*pSDL_PushEvent)(SDL_Event *event);

void CustomApplicationMain (int argc, char **argv);
int XX_main (int argc, char **argv);

#define MAP(x) p ## x = dlsym(libhandle, #x) ; if(!p ## x) fprintf(stderr, "Could not map %s\n", #x);

#undef main
int main(int argc, char *argv[])
{
	printf("In main\n");
	void *libhandle;
	libhandle = dlopen("/Library/Frameworks/SDL.framework/SDL", RTLD_LAZY);

	printf("Loaded SDL %p'n", libhandle);

	MAP(SDL_OpenAudio);
	MAP(SDL_CloseAudio);
	MAP(SDL_PauseAudio);
	MAP(SDL_GL_SetAttribute);
	MAP(SDL_GL_SwapBuffers);
	MAP(SDL_GetAppState);
	MAP(SDL_SetError);
	MAP(SDL_GetError);
	MAP(SDL_ClearError);
	MAP(SDL_GetTicks);
	MAP(SDL_Init);
	MAP(SDL_InitSubSystem);
	MAP(SDL_CreateRGBSurface);
	MAP(SDL_CreateRGBSurfaceFrom);
	MAP(SDL_FreeSurface);
	MAP(SDL_LockSurface);
	MAP(SDL_UnlockSurface);
	MAP(SDL_WM_GrabInput);
	MAP(SDL_MapRGB);
	MAP(SDL_PollEvent);
	MAP(SDL_WaitEvent);
	MAP(SDL_Quit);
	MAP(SDL_SetColors);
	MAP(SDL_SetVideoMode);
	MAP(SDL_UpdateRects);
	MAP(SDL_UpdateRect);
	MAP(SDL_Flip);
	MAP(SDL_SetCursor);
	MAP(SDL_GetCursor);
	MAP(SDL_FreeCursor);
	MAP(SDL_ShowCursor);
	MAP(SDL_WM_SetCaption);
	MAP(SDL_RWFromFile);
	MAP(SDL_Delay);
	MAP(SDL_GetKeyName);
	MAP(SDL_GetMouseState);
	MAP(SDL_EnableKeyRepeat);
	MAP(SDL_EnableUNICODE);
	MAP(SDL_EventState);
	MAP(SDL_NumJoysticks);
	MAP(SDL_JoystickName);
	MAP(SDL_JoystickOpen);
	MAP(SDL_JoystickOpened);
	MAP(SDL_JoystickIndex);
	MAP(SDL_JoystickNumAxes);
	MAP(SDL_JoystickNumBalls);
	MAP(SDL_JoystickNumHats);
	MAP(SDL_JoystickNumButtons);
	MAP(SDL_JoystickUpdate);
	MAP(SDL_JoystickEventState);
	MAP(SDL_JoystickGetAxis);
	MAP(SDL_JoystickGetHat);
	MAP(SDL_JoystickGetBall);
	MAP(SDL_JoystickGetButton);
	MAP(SDL_JoystickClose);
	MAP(SDL_PushEvent);


	Py_SetProgramName(argv[0]);
    Py_Initialize();
    PySys_SetArgv(argc, argv);

	//startPython("test");

	XX_main(argc, argv);
	//return SDL_main(argc, argv);
	return 0;
}

//#define CALL(x, ...) (printf("################ %s(%s)\n", #x, #__VA_ARGS__), p ## x(__VA_ARGS__))
#define CALL(x, ...) p ## x(__VA_ARGS__)


int SDLCALL SDL_OpenAudio(SDL_AudioSpec *desired, SDL_AudioSpec *obtained)
{
	return CALL(SDL_OpenAudio, desired, obtained);
}


void SDLCALL SDL_CloseAudio(void)
{
	CALL(SDL_CloseAudio);
}

void SDLCALL SDL_PauseAudio(int pause_on)
{
	CALL(SDL_PauseAudio, pause_on);
}

int SDLCALL SDL_GL_SetAttribute(SDL_GLattr attr, int value)
{
	return CALL(SDL_GL_SetAttribute, attr, value);
}

uint32_t frameCounter = 0;

void SDLCALL SDL_GL_SwapBuffers(void)
{
	CALL(SDL_GL_SwapBuffers);
}

Uint8 SDLCALL SDL_GetAppState(void)
{
	return CALL(SDL_GetAppState);
}

void SDLCALL SDL_SetError(const char *fmt, ...)
{
	CALL(SDL_SetError, fmt);
}

char * SDLCALL SDL_GetError(void)
{
	return CALL(SDL_GetError);
}

void SDLCALL SDL_ClearError(void)
{
	return CALL(SDL_ClearError);
}

int SDLCALL SDL_Init(Uint32 flags)
{
	return CALL(SDL_Init, flags);
}

int SDLCALL SDL_InitSubSystem(Uint32 flags)
{
	return CALL(SDL_InitSubSystem, flags);
}

SDL_Surface * SDLCALL SDL_CreateRGBSurface(Uint32 flags, int width, int height, int depth, Uint32 Rmask, Uint32 Gmask, Uint32 Bmask, Uint32 Amask)
{
	return CALL(SDL_CreateRGBSurface, flags, width, height, depth, Rmask, Gmask, Bmask, Amask);
}

SDL_Surface * SDLCALL SDL_CreateRGBSurfaceFrom(void *pixels, int width, int height, int depth, int pitch, Uint32 Rmask, Uint32 Gmask, Uint32 Bmask, Uint32 Amask)
{
	return CALL(SDL_CreateRGBSurfaceFrom, pixels, width, height, depth, pitch, Rmask, Gmask, Bmask, Amask);
}

void SDLCALL SDL_FreeSurface(SDL_Surface *surface)
{
	CALL(SDL_FreeSurface, surface);
}

int SDLCALL SDL_LockSurface(SDL_Surface *surface)
{
	return CALL(SDL_LockSurface, surface);
}

void SDLCALL SDL_UnlockSurface(SDL_Surface *surface)
{
	CALL(SDL_UnlockSurface, surface);
}

SDL_GrabMode SDLCALL SDL_WM_GrabInput(SDL_GrabMode mode)
{
	return CALL(SDL_WM_GrabInput, mode);
}

Uint32 SDLCALL SDL_MapRGB(const SDL_PixelFormat * const format, const Uint8 r, const Uint8 g, const Uint8 b)
{
	return CALL(SDL_MapRGB, format, r, g, b);
}

int SDLCALL SDL_PollEvent(SDL_Event *event)
{
	int e = CALL(SDL_PollEvent, event);
	if(e == 1) {
		if(event->type == SDL_KEYDOWN) {
			//printf("KEY %x %x %x\n", event->key.keysym.scancode, event->key.keysym.sym, event->key.keysym.mod);
		}
	}
	return e;
}

int SDLCALL SDL_WaitEvent(SDL_Event *event)
{
	int e = CALL(SDL_WaitEvent, event);
	return e;
}

 int SDLCALL SDL_PushEvent(SDL_Event *event)
 {
 	return CALL(SDL_PushEvent, event);
 }

void SDLCALL SDL_Quit(void)
{
	CALL(SDL_Quit);
}

int SDLCALL SDL_SetColors(SDL_Surface *surface, SDL_Color *colors, int firstcolor, int ncolors)
{
	return CALL(SDL_SetColors, surface, colors, firstcolor, ncolors);
}

SDL_Surface * SDLCALL SDL_SetVideoMode(int width, int height, int bpp, Uint32 flags)
{
	SDL_Surface *s = CALL(SDL_SetVideoMode, width, height,  bpp, flags);
	return s;
}

void SDLCALL SDL_UpdateRects(SDL_Surface *screen, int numrects, SDL_Rect *rects)
{
	CALL(SDL_UpdateRects, screen, numrects, rects);
}

void SDLCALL SDL_UpdateRect(SDL_Surface *screen, Sint32 x, Sint32 y, Uint32 w, Uint32 h)
{
	CALL(SDL_UpdateRect, screen, x, y, w, h);
}

int SDLCALL SDL_Flip(SDL_Surface *screen)
{
	return CALL(SDL_Flip, screen);
}

// CURSOR

void SDLCALL SDL_SetCursor(SDL_Cursor *cursor)
{
	CALL(SDL_SetCursor, cursor);
}

SDL_Cursor * SDLCALL SDL_GetCursor(void)
{
	return CALL(SDL_GetCursor);
}

void SDLCALL SDL_FreeCursor(SDL_Cursor *cursor)
{
	CALL(SDL_FreeCursor, cursor);
}

int SDLCALL SDL_ShowCursor(int toggle)
{
	return CALL(SDL_ShowCursor, toggle);
}

void SDLCALL SDL_WM_SetCaption(const char *title, const char *icon)
{
	CALL(SDL_WM_SetCaption, title, icon);
}

SDL_RWops * SDLCALL SDL_RWFromFile(const char *file, const char *mode)
{
	return CALL(SDL_RWFromFile, file, mode);
}

Uint32 SDLCALL SDL_GetTicks(void)
{
	return CALL(SDL_GetTicks);
}

void SDLCALL SDL_Delay(Uint32 ms)
{
	frameCounter++;
	//printf("Delay %d\n", ms);
	if(frameCounter > 50)
		CALL(SDL_Delay, ms);
	//mem_store(0x400, 'A');
}

// Keys

char * SDLCALL SDL_GetKeyName(SDLKey key)
{
	return CALL(SDL_GetKeyName, key);
}

Uint8 SDLCALL SDL_GetMouseState(int *x, int *y)
{
	return CALL(SDL_GetMouseState, x, y);
}




int SDLCALL SDL_EnableKeyRepeat(int delay, int interval)
{
	return CALL(SDL_EnableKeyRepeat, delay, interval);
}

int SDLCALL SDL_EnableUNICODE(int enable)
{
	return CALL(SDL_EnableUNICODE, enable);
}

Uint8 SDLCALL SDL_EventState(Uint8 type, int state)
{
	return SDL_EventState(type, state);
}


// JOYSTICK


int SDLCALL SDL_NumJoysticks(void)
{
	return 0;
}

const char * SDLCALL SDL_JoystickName(int device_index)
{
	return "";
}

SDL_Joystick * SDLCALL SDL_JoystickOpen(int device_index)
{
	return NULL;
}

int SDLCALL SDL_JoystickOpened(int device_index)
{
	return 0;
}

int SDLCALL SDL_JoystickIndex(SDL_Joystick *joystick)
{
	return 0;
}

int SDLCALL SDL_JoystickNumAxes(SDL_Joystick *joystick)
{
	return 0;
}

int SDLCALL SDL_JoystickNumBalls(SDL_Joystick *joystick)
{
	return 0;
}

int SDLCALL SDL_JoystickNumHats(SDL_Joystick *joystick)
{
	return 0;
}

int SDLCALL SDL_JoystickNumButtons(SDL_Joystick *joystick)
{
	return 0;
}

void SDLCALL SDL_JoystickUpdate(void)
{
}

int SDLCALL SDL_JoystickEventState(int state)
{
	return 0;
}

Sint16 SDLCALL SDL_JoystickGetAxis(SDL_Joystick *joystick, int axis)
{
	return 0;
}


Uint8 SDLCALL SDL_JoystickGetHat(SDL_Joystick *joystick, int hat)
{
	return 0;
}

int SDLCALL SDL_JoystickGetBall(SDL_Joystick *joystick, int ball, int *dx, int *dy)
{
	return 0;
}

Uint8 SDLCALL SDL_JoystickGetButton(SDL_Joystick *joystick, int button)
{
	return 0;
}

void SDLCALL SDL_JoystickClose(SDL_Joystick *joystick)
{
}

#undef HAVE_NANOSLEEP
#include "attach.h"

PyObject *vice_attach_disk(PyObject *self, PyObject *args)
{
	char *fileName;
	PyArg_ParseTuple(args, "s", &fileName);
	if (!(file_system_attach_disk(8, fileName) < 0)) {
		return Py_BuildValue("i", -1);
	}
	return Py_BuildValue("i", 8);
}


PyObject *vice_print(PyObject *self, PyObject *args)
{
	char *text;
	PyArg_ParseTuple(args, "s", &text);
	//PyString_AsString(

	printf("\n ## VICE: %s\n", text);
	return Py_BuildValue("i", 0);
}

void maincpu_mainloop(void);
void maincpu_start(void);


PyObject *vice_run_cycles(PyObject *self, PyObject *args)
{
	int cycles = 0;
	PyArg_ParseTuple(args, "i", &cycles);
	while(cycles--) {
	    maincpu_mainloop();
	}
	return Py_BuildValue("i", 0);
}

PyObject *vice_run_frames(PyObject *self, PyObject *args)
{
	int frames = 0;
	PyArg_ParseTuple(args, "i", &frames);
	uint32_t toFrame = frameCounter + frames;
	while(toFrame > frameCounter) {
	    maincpu_mainloop();

	}
	return Py_BuildValue("i", 0);
}

PyObject *vice_break(PyObject *self, PyObject *args)
{
	
}


static PyMethodDef ViceMethods[] = {
    {"output", vice_print, METH_VARARGS, "Prints text"},
    {"attach_disk", vice_attach_disk, METH_VARARGS, "Attach disk"},
    {"run_cycles", vice_run_cycles, METH_VARARGS, "Run cycles"},
    {"run_frames", vice_run_frames, METH_VARARGS, "Run cycles"},
    {"break_on", vice_break, METH_VARARGS, "Run cycles"},
    {NULL, NULL, 0, NULL}
};

int startPython(const char *pythonScript);

unsigned int maincpu_get_a(void);

void scripting_check_traps(int pc)
{
	//printf("%x\n", pc);
	if((pc & 0xffd0) == 0xffd0) {
		int a = maincpu_get_a();
		//printf("OUT: %x, %02x\n", pc, a);
		putchar(a);
	}

}

void start_scripting(void)
{
	//maincpu_start();
	startPython("test.py");
	while(true)
		maincpu_mainloop();
}

int startPython(const char *pythonScript)
{
    PyObject *pName, *pModule;//, *pFunc, *pValue;

    Py_InitModule("vice", ViceMethods);

    pName = PyString_FromString(pythonScript);
    /* Error checking of pName left out */

    pModule = PyImport_Import(pName);
    Py_DECREF(pName);

    if (pModule != NULL) {
#if 0    	
        pFunc = PyObject_GetAttrString(pModule, "start");
        /* pFunc is a new reference */

        if (pFunc && PyCallable_Check(pFunc)) 
        {
            pValue = PyObject_CallObject(pFunc, NULL);
            if (pValue != NULL) {
                printf("Result of call: %ld\n", PyInt_AsLong(pValue));
                Py_DECREF(pValue);
            }
            else {
                Py_DECREF(pFunc);
                Py_DECREF(pModule);
                PyErr_Print();
                fprintf(stderr,"Call failed\n");
                return 1;
            }
        }
        else {
            if (PyErr_Occurred())
                PyErr_Print();
            fprintf(stderr, "Cannot find function\n");
        }
        Py_XDECREF(pFunc);
        Py_DECREF(pModule);
#endif
    }
    else {
        PyErr_Print();
        fprintf(stderr, "Failed to load\n");
        return 1;
    }
    Py_Finalize();
    return 0;
}
