
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


KeyEvent a2m[] = {

	{ -1, -1, false },
	{ -1, -1, false },
	{ -1, -1, false },
	{ -1, -1, false },
	{ -1, -1, false },
	{ -1, -1, false },
	{ -1, -1, false },
	{ -1, -1, false },
	{ 0, 0, false },
	{ -1, -1, false },
	{ 0, 1, false },
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

static vector<unsigned int> get_regs()
{
	vector<unsigned int> regs { 
		maincpu_get_a(),
		maincpu_get_x(),
		maincpu_get_y(),
		maincpu_get_pc(),
		maincpu_get_sp()
	};

	return regs;
}
ADDFUNC(get_regs, "")

/*


#define PYFN(name) PyObject* vice_ ## name(PyObject *self, PyObject *args)

PyObject *vice_run_cycles(PyObject *self, PyObject *args)
{
	int cycles = 0;
	PyArg_ParseTuple(args, "i", &cycles);
	python_mainloop(cycles);
	return Py_BuildValue("i", 0);
}

extern int frameCounter;
PyObject *vice_run_frames(PyObject *self, PyObject *args)
{
	int frames = 0;
	PyArg_ParseTuple(args, "i", &frames);
	uint32_t toFrame = frameCounter + frames;
	while(toFrame > frameCounter) {
		python_mainloop(500);

	}
	return Py_BuildValue("i", 0);
}

PyObject *vice_break(PyObject *self, PyObject *args)
{
	int break_pc;
	PyArg_ParseTuple(args, "iO", &break_pc, &obj);
	if (!PyCallable_Check(obj)) {
			PyErr_SetString(PyExc_TypeError, "parameter must be callable");
			return NULL;
		}
	Py_XINCREF(obj);
	//printf("Break on %04x", break_pc);
	breaks.insert(break_pc);
	return Py_BuildValue("i", 0);
}

PYFN(putchar)
{
   int key;
   PyArg_ParseTuple(args, "i", &key);
   keyboard_key_pressed(key);
	return Py_BuildValue("i", 0);
 }

PYFN(on_frame)
{
	PyArg_ParseTuple(args, "O", &frame_fn);
	if (!PyCallable_Check(frame_fn)) {
			PyErr_SetString(PyExc_TypeError, "parameter must be callable");
			return NULL;
		}
	Py_XINCREF(frame_fn);
	return Py_BuildValue("i", 0);
}

PYFN(mem_read)
{
	int addr;
	PyArg_ParseTuple(args, "i", &addr);
	return Py_BuildValue("i", mem_read(addr));
}

PYFN(mem_write)
{
	int addr, val;
	PyArg_ParseTuple(args, "ii", &addr, &val);
	int oldv = mem_read(addr);
	mem_store(addr, val);
	return Py_BuildValue("i", oldv);
}

#define PMDEF(name, doc) { #name, vice_ ## name, METH_VARARGS, doc },

PyObject *vice_print(PyObject *self, PyObject *args) { return callf(args, print); }


PyObject *vice_attach_disk(PyObject *self, PyObject *args) { return callf(args, attach_disk); }

static PyMethodDef ViceMethods[] = {
	PMDEF(mem_read, "")
	PMDEF(mem_write, "")
	PMDEF(on_frame, "")
	{"output", vice_print, METH_VARARGS, "Prints text"},
	{"attach_disk", vice_attach_disk, METH_VARARGS, "Attach disk"},
	{"run_cycles", vice_run_cycles, METH_VARARGS, "Run cycles"},
	{"run_frames", vice_run_frames, METH_VARARGS, "Run cycles"},
	{"break_on", vice_break, METH_VARARGS, "Run cycles"},
	{NULL, NULL, 0, NULL}
};
*/

extern "C" void run_python(int argc, char **argv, FILE *out, FILE *in)
{

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
		//return 1;
	}
	

	// Py_Finalize();


		//maincpu_start();
 //   startPython("test.py");
	//while(1)
	  //  maincpu_mainloop(1);

	//return 0;


}
