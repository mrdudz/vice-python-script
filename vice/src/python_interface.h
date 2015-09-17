#include <tuple>

namespace std {

template <std::size_t...> struct index_sequence {};

template <std::size_t N, std::size_t... Is>
struct make_index_sequence : make_index_sequence<N - 1, N - 1, Is...> {};

template <std::size_t... Is>
struct make_index_sequence<0u, Is...> : index_sequence<Is...> { using type = index_sequence<Is...>; };

}

template<class F>
struct function_traits;
 
// function pointer
template<class R, class... Args>
struct function_traits<R(*)(Args...)> : public function_traits<R(Args...)>
{};
 
template<class R, class... Args>
struct function_traits<R(Args...)>
{
    using return_type = R;
 
    static constexpr std::size_t arity = sizeof...(Args);
 
    template <std::size_t N>
    struct argument
    {
        static_assert(N < arity, "error: invalid parameter index.");
        using type = typename std::tuple_element<N,std::tuple<Args...>>::type;
    };

    using tuple_type = std::tuple<Args...>;
};


struct PyFunc {
	PyFunc(PyObject *o = nullptr) : func(o) {}

	operator bool() { return func != nullptr; }

	void call() {
		PyObject_CallObject(func, nullptr);
	}
	PyObject *func;
};


//    PyTuple_GetItem(args, pos);

namespace tp {
	template <class T> T getArg(PyObject *pt, int index);
	template <> int getArg(PyObject *pt, int index) {
		auto *obj = PyTuple_GetItem(pt, index);
		Py_XINCREF(obj);
		return PyInt_AsLong(obj);
	}
	template <> std::string getArg(PyObject *pt, int index) {
		return std::string(PyString_AsString(PyTuple_GetItem(pt, index)));
	}

	template <> std::vector<uint8_t> getArg(PyObject *pt, int index) {
		char *buf;
		Py_ssize_t size;
		PyString_AsStringAndSize(PyTuple_GetItem(pt, index), &buf, &size);
		std::vector<uint8_t> data(size);
		memcpy(&data[0], buf, size);
		return data;
	}

	template <> PyFunc getArg(PyObject *pt, int index) {
		auto *obj = PyTuple_GetItem(pt, index);
		if (!PyCallable_Check(obj)) {
				PyErr_SetString(PyExc_TypeError, "parameter must be callable");
				return PyFunc(nullptr);
			}
		Py_XINCREF(obj);
		return PyFunc(obj);
	}

	PyObject *makeRet(const int &i) {
		return Py_BuildValue("i", i);
	}

	PyObject *makeRet(void) {
		return Py_BuildValue("i", 0);
	}


	PyObject *makeRet(const std::string &s) {
		return Py_BuildValue("s", s.c_str());
	}

	PyObject *makeRet(const std::vector<uint8_t> &v) {
		return Py_BuildValue("z#", (char*)&v[0], (int)v.size());
	}

	template <typename T> PyObject *makeRet(const std::vector<T> &v) {

		auto *pt = PyTuple_New(v.size());
		for(int i=0; i<v.size(); i++) {
			PyTuple_SetItem(pt, i, makeRet(v[i]));
		}
		return pt;
	}


}

template <typename ... ARGS> struct TupleParser {

	template <size_t ... A> static std::tuple<ARGS...> make(PyObject *pt, std::index_sequence<A...>) {
		return make_tuple(tp::getArg<ARGS>(pt, A)...);
	}
	   
	static std::tuple<ARGS...> parse(PyObject *pt) {
		return make(pt, std::make_index_sequence<sizeof...(ARGS)>());
	}
};

template<typename... X> struct FCaller;
template <class FX, class... ARGS> struct FCaller<FX,void (FX::*)(ARGS...) const> {

	template <size_t ... A> static void call2(PyObject *pt, FX f, std::index_sequence<A...>) {
		f(tp::getArg<ARGS>(pt, A)...);
	}

	static void call(PyObject *args, FX f) {
			call2(args, f, std::make_index_sequence<sizeof...(ARGS)>());
	}
};

template<typename... X> struct TFCaller;
template <class FX, class R, class... ARGS> struct TFCaller<FX, R, std::tuple<ARGS...>> {

	template <size_t ... A> static PyObject* call2(PyObject *pt, FX f, std::index_sequence<A...>) {
		return tp::makeRet(f(tp::getArg<ARGS>(pt, A)...));
	}

	static PyObject* call(PyObject *args, FX f) {
		return call2(args, f, std::make_index_sequence<sizeof...(ARGS)>());
	}
};

template <class FX, class... ARGS> struct TFCaller<FX, void, std::tuple<ARGS...>> {

	template <size_t ... A> static PyObject* call2(PyObject *pt, FX f, std::index_sequence<A...>) {
		f(tp::getArg<ARGS>(pt, A)...);
		return tp::makeRet();
	}

	static PyObject* call(PyObject *args, FX f) {
		return call2(args, f, std::make_index_sequence<sizeof...(ARGS)>());
	}
};

template <class FX> PyObject* call(PyObject *args, FX f) {

	FCaller<FX,decltype(&FX::operator()) >::call(args, f);
	return nullptr;
}

template <class FX> PyObject* callf(PyObject *args, FX f) {
	using Traits = function_traits<FX>;
	return TFCaller<FX, typename Traits::return_type, typename Traits::tuple_type>::call(args, f);
}
