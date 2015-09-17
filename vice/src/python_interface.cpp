
#include <stdio.h>
#include <Python.h>

//typedef unsigned short WORD;
//typedef unsigned char BYTE;

extern "C" {

#include "maincpu.h"

void mem_store(WORD addr, BYTE value);
BYTE mem_read(WORD addr);
//unsigned int maincpu_get_a(void);
void maincpu_mainloop(unsigned int count);
void keyboard_key_pressed(signed long);
void keyboard_key_released(signed long);
//int keyboard_key_pressed_matrix(int row, int column, int shift);
//int keyboard_key_released_matrix(int row, int column, int shift);
void keyboard_rowcol_release(int row, int column);
void keyboard_rowcol_press(int row, int column);

#undef HAVE_NANOSLEEP
#include "attach.h"

}

struct KeyEvent {
	KeyEvent(int r, int c, bool p) : row(r), col(c), pressed(p)  {}
	int8_t row;
	int8_t col;
	bool pressed;
};

// 0 2 false // rt
// 0 2 true // lt
// 0 7 false // dn
// 0 7 true // up
// 6 3 false // home
// 7 7 false // stop

// 0 4 // f1 - f7
// 0 5
// 0 6
// 0 3

KeyEvent a2m[] = {

	{ 0, 4, false },
	{ 0, 4, true },
	{ 0, 5, false },
	{ 0, 5, true },
	{ 0, 6, false },
	{ 0, 6, true },
	{ 0, 3, false },
	{ 0, 3, true },
	{ 0, 0, false },
	{ -1, -1, false },
	{ 0, 1, false },
	{ -1, -1, false },
	{ 6, 3, false },
	{ -1, -1, false },
	{ 7, 7, true },
	{ 7, 7, false },

	{ 0, 2, false },
	{ 0, 2, true },
	{ 0, 7, false },
	{ 0, 7, true },
	{ -1, -1, false },
	{ -1, -1, false },
	{ -1, -1, false },
	{ -1, -1, false },
	{ -1, -1, false },
	{ -1, -1, false },
	{ -1, -1, false },
	{ -1, -1, false },
	{ -1, -1, false },
	{ -1, -1, false },
	{ -1, -1, false },
	{ -1, -1, false },


	{ 7, 4, false }, // SPACE ! " # $ % &
	{ 7, 0, true },
	{ 7, 3, true },
	{ 1, 0, true },
	{ 1, 3, true },
	{ 2, 0, true },
	{ 2, 3, true },

	{ 3, 0, true }, // '()*+,-
	{ 3, 3, true },
	{ 4, 0, true },
	{ 6, 1, false },
	{ 5, 0, false },
	{ 5, 7, false },
	{ 5, 3, false },

	{ 5, 4, false }, // ./0123456789
	{ 6, 7, false },
	{ 4, 3, false },
	{ 7, 0, false },
	{ 7, 3, false },
	{ 1, 0, false },
	{ 1, 3, false },
	{ 2, 0, false },
	{ 2, 3, false },
	{ 3, 0, false },
	{ 3, 3, false },
	{ 4, 0, false },

	{ 5, 5, false }, // :;<=>?@
	{ 6, 2, false },
	{ -1, -1, false },
	{ 6, 5, false },
	{ -1, -1, false },
	{ 6, 7, false },
	{ 5, 6, false },

	{ 1, 2, true }, // A-Z
	{ 3, 4, true },
	{ 2, 4, true },
	{ 2, 2, true },
	{ 1, 6, true },
	{ 2, 5, true },
	{ 3, 2, true },
	{ 3, 5, true },
	{ 4, 1, true },
	{ 4, 2, true },
	{ 4, 5, true },
	{ 5, 2, true },
	{ 4, 4, true },
	{ 4, 7, true },
	{ 4, 6, true },
	{ 5, 1, true },
	{ 7, 6, true },
	{ 2, 1, true },
	{ 1, 5, true },
	{ 2, 6, true },
	{ 3, 6, true },
	{ 3, 7, true },
	{ 1, 1, true },
	{ 2, 7, true },
	{ 3, 1, true },
	{ 1, 4, true },

	{ 5, 5, true }, // [\]^_`
	{ -1, -1, false },
	{ 6, 2, true },
	{ 2, 3, true },
	{ -1, -1, false },
	{ -1, -1, false },

	{ 1, 2, false }, // a-z
	{ 3, 4, false },
	{ 2, 4, false },
	{ 2, 2, false },
	{ 1, 6, false },
	{ 2, 5, false },
	{ 3, 2, false },
	{ 3, 5, false },
	{ 4, 1, false },
	{ 4, 2, false },
	{ 4, 5, false },
	{ 5, 2, false },
	{ 4, 4, false },
	{ 4, 7, false },
	{ 4, 6, false },
	{ 5, 1, false },
	{ 7, 6, false },
	{ 2, 1, false },
	{ 1, 5, false },
	{ 2, 6, false },
	{ 3, 6, false },
	{ 3, 7, false },
	{ 1, 1, false },
	{ 2, 7, false },
	{ 3, 1, false },
	{ 1, 4, false },

	{ -1, -1, false },
	{ -1, -1, false },
	{ -1, -1, false },
	{ 0, 0, false }
};


#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <tuple>
#include <deque>

#include "python_interface.h"

using namespace std;

static FILE *output_fp;
static PyFunc frame_fn = NULL;
static unordered_map<uint16_t, PyFunc> breaks;
static vector<PyMethodDef> vice_methods;
static deque<int> keys;
static int currentKey = -1;

static deque<KeyEvent> matrix;

extern int frameCounter;
extern int frameDelay;

extern "C" {
	PyObject **store_watch_on;
}

extern "C" void do_store_watch(int a, int v)
{
	auto *obj = store_watch_on[a];
	if(obj) {
		PyObject* arglist = Py_BuildValue("(ii)", a, v);
		PyObject_CallObject(obj, arglist);
		Py_DECREF(arglist);
	}
	//printf("%02x => %04x\n", v, a);
}


void scripting_check_traps(int pc)
{
	//printf("%x\n", pc);
	if(breaks.find(pc) != breaks.end()) {
		auto *obj = breaks[pc].func;
		int a = maincpu_get_a();
		int x = maincpu_get_x();
		int y = maincpu_get_y();
		PyObject* arglist = Py_BuildValue("(iii)", a, x, y);
		PyObject_CallObject(obj, arglist);
		Py_DECREF(arglist);
	}
}

void python_mainloop(int cycles)
{
	while(cycles--) {
		maincpu_mainloop(1);
		int pc = maincpu_get_pc();
		scripting_check_traps(pc);
	}
}

static int delay = 0;

extern "C" int onframe_python(int fc) 
{
	if(delay == 0) {
		if(currentKey > 0) {
			keyboard_key_released(currentKey);
			//printf("RELEASE %d\n", currentKey);
			currentKey = -1;
		} else
		if(keys.size()) {
			currentKey = keys.front();
			keys.pop_front();
			//printf("PUSH %d\n", currentKey);
			keyboard_key_pressed(currentKey);
		}
		if(matrix.size() > 0) {
			auto m = matrix.front();
			matrix.pop_front();
			if(m.pressed)
				keyboard_rowcol_press(m.row, m.col);
			else
				keyboard_rowcol_release(m.row, m.col);
		}
		delay = 1;
	} else
		delay--;

	if(frame_fn)
		frame_fn.call();
	return 0;
}

//#define PYFUNC(xxx) PyObject *xxx(PyObject *self, PyObject *args) { return call(args, []
//#define PMDEF(name, doc) { #name, vice_ ## name, METH_VARARGS, doc },

#define ADDFUNC(name, doc) \
PyObject *vice_ ## name(PyObject *self, PyObject *args) { return callf(args, name); } \
struct name ## _struct { \
	name ## _struct() { vice_methods.push_back({ #name, vice_ ## name, METH_VARARGS, doc }); } \
} name ## _impl;


static int attach_disk(std::string fn, int device) {
	if (!(file_system_attach_disk(device, fn.c_str()) < 0)) {
		return -1;
	}
	return device;
};
ADDFUNC(attach_disk, "Attach a disk image to given device");

static int output(std::string text) {
	fputs(text.c_str(), output_fp);
	return 0;
}
ADDFUNC(output, "Print to stdout");

static int run_opcodes(int ops) {
	python_mainloop(ops);
	return ops;
}
ADDFUNC(run_opcodes, "Run for a specific number of opcodes");

static int run_frames(int frames)
{
	uint32_t toFrame = frameCounter + frames;
	while(toFrame > frameCounter)
		python_mainloop(500);
	return frames;
}
ADDFUNC(run_frames, "Run for a specific number of frames");

static int warp_frames(int frames)
{
	uint32_t toFrame = frameCounter + frames;
	frameDelay = 0;
	while(toFrame > frameCounter)
		python_mainloop(500);
	frameDelay = 20;
	return frames;
}
ADDFUNC(warp_frames, "Run for a specific number of frames");

static int break_on(int pc, PyFunc pf)
{
	//printf("Break on %04x", pc);
	breaks[pc] = pf;
	return 0;
}
ADDFUNC(break_on, "");

static int on_frame(PyFunc pf)
{
	frame_fn = pf;
	return 0;
}
ADDFUNC(on_frame, "");

static int put_key(int key) {
	//printf("Put key %d\n", key);

	if(key > 0x7f)
		return -1;
	auto m = a2m[key];

	if(m.row < 0)
		return -1;

	//printf("Put key %d -> %d %d\n", m.row, m.col);

	if(m.pressed)
		matrix.emplace_back(1, 7, true);
	matrix.emplace_back(m.row, m.col, true);
	if(m.pressed)
		matrix.emplace_back(1, 7, false);
	matrix.emplace_back(m.row, m.col, false);

	return 0;
}
ADDFUNC(put_key, "Simulate typing a key code");


static int put_raw_key(int row, int col, int press)
{
	matrix.emplace_back(row, col, press != 0);
	return 0;
}
ADDFUNC(put_raw_key, "");


static vector<uint8_t> dump;

static vector<uint8_t> read_memory(int start, int len)
{
	dump.resize(len);
	for(int i=0; i<len; i++) {
		dump[i] = mem_read(start+i);
	}
	return dump;
}
ADDFUNC(read_memory, "");

static int write_memory(int start, vector<uint8_t> data)
{
	for(auto &d : data)
		mem_store(start++, d);
	return 0;
}
ADDFUNC(write_memory, "");

static int get_mem(int start) { return mem_read(start); }
ADDFUNC(get_mem, "");

static void put_mem(int start, int v) { mem_store(start, v); }
ADDFUNC(put_mem, "");

static vector<unsigned int> get_regs()
{
	vector<unsigned int> regs { 
		maincpu_get_a(),
		maincpu_get_x(),
		maincpu_get_y(),
		maincpu_get_pc(),
		maincpu_get_sp(),
		maincpu_get_sr()
	};

	return regs;
}
ADDFUNC(get_regs, "");

static int get_a() { return maincpu_get_a(); }
ADDFUNC(get_a, "");

static int get_x() { return maincpu_get_x(); }
ADDFUNC(get_x, "");

static int get_y() { return maincpu_get_y(); }
ADDFUNC(get_y, "");

static int get_pc() { return maincpu_get_pc(); }
ADDFUNC(get_pc, "");

static int get_sp() { return maincpu_get_sp(); }
ADDFUNC(get_sp, "");

static int get_sr() { return maincpu_get_sr(); }
ADDFUNC(get_sr, "");

static void set_a(int v) { maincpu_set_a(v); }
ADDFUNC(set_a, "");

static void set_x(int v) { maincpu_set_x(v); }
ADDFUNC(set_x, "");

static void set_y(int v) { maincpu_set_y(v); }
ADDFUNC(set_y, "");

static void set_pc(int v) { maincpu_set_pc(v); }
ADDFUNC(set_pc, "");

static void set_sp(int v) { maincpu_set_sp(v); }
ADDFUNC(set_sp, "");

static void set_sr(int v) { maincpu_set_sr(v); }
ADDFUNC(set_sr, "");

static void on_write(int adr, int len, PyFunc fn)
{
	for(int i=0; i<len; i++) {
		store_watch_on[adr+i] = fn.func;
	}
}
ADDFUNC	(on_write, "");

extern "C" void run_python(int argc, char **argv, FILE *out, FILE *in)
{

	store_watch_on = new PyObject* [65536];
	memset(&store_watch_on[0], 0, 65536*sizeof(void*));

	//store_watch_on[0xd020] = 1;

	output_fp = out;


	Py_SetProgramName(argv[0]);
	Py_Initialize();

	PySys_SetArgv(argc, argv);


	const char *pythonScript = strdup(argv[0]);
	char *dot = strrchr(pythonScript, '.');
	if(dot && (dot != pythonScript)) *dot = 0;

	PyObject *pName, *pModule;//, *pFunc, *pValue;

	//printf("METHODS:%d\n", vice_methods.size());
	vice_methods.push_back({NULL, NULL, 0, NULL});
	Py_InitModule("vice", &vice_methods[0]);



	pName = PyString_FromString(pythonScript);
	/* Error checking of pName left out */

	pModule = PyImport_Import(pName);
	Py_DECREF(pName);

	free((void*)pythonScript);

	if (pModule == NULL) {
		PyErr_Print();
		fprintf(stderr, "Failed to load\n");
		//return 1;
	}
	
}
