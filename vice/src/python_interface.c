
#include <stdio.h>
#include <Python.h>

//typedef unsigned short WORD;
//typedef unsigned char BYTE;

#include "maincpu.h"

void mem_store(WORD addr, BYTE value);
BYTE mem_read(WORD addr);
//unsigned int maincpu_get_a(void);
void maincpu_mainloop(unsigned int count);

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

FILE *output;

PyObject *vice_print(PyObject *self, PyObject *args)
{
	char *text;
	PyArg_ParseTuple(args, "s", &text);
	//PyString_AsString(

	//printf("\n ## VICE: %s\n", text);
    fprintf(output, text);
	return Py_BuildValue("i", 0);
}


PyObject *obj;
int break_pc;

void scripting_check_traps(int pc)
{
    //printf("%x\n", pc);
    if(pc == break_pc) {
        int a = maincpu_get_a();
        int x = maincpu_get_x();
        int y = maincpu_get_y();
        //printf("OUT: %x, %02x\n", pc, a);
        //putchar(a);
        //printf("BREAK\n");
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
	PyArg_ParseTuple(args, "io", &break_pc, &obj);
    PyArg_ParseTuple(args, "iO:set_callback", &break_pc, &obj);
    if (!PyCallable_Check(obj)) {
            PyErr_SetString(PyExc_TypeError, "parameter must be callable");
            return NULL;
        }
    Py_XINCREF(obj);
    printf("Break on %04x", break_pc);
    return Py_BuildValue("i", 0);
}


static PyMethodDef ViceMethods[] = {
    {"output", vice_print, METH_VARARGS, "Prints text"},
    {"attach_disk", vice_attach_disk, METH_VARARGS, "Attach disk"},
    {"run_cycles", vice_run_cycles, METH_VARARGS, "Run cycles"},
    {"run_frames", vice_run_frames, METH_VARARGS, "Run cycles"},
    {"break_on", vice_break, METH_VARARGS, "Run cycles"},
    {NULL, NULL, 0, NULL}
};


void run_python(int argc, char **argv, FILE *out, FILE *in)
{

    output = out;

    Py_SetProgramName(argv[0]);
    Py_Initialize();
    PySys_SetArgv(argc, argv);

    const char *pythonScript = argv[0];

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
        //return 1;
    }
    Py_Finalize();


        //maincpu_start();
 //   startPython("test.py");
    //while(1)
      //  maincpu_mainloop(1);

    //return 0;


}
