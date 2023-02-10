#pragma once
#ifndef Vector_h
#define Vector_h

#include <Python.h>

template <typename T>
union TVector{
	struct{
		T  x;
		T  y;
		T  z;
		T  w;
	};
	T u[4];
};

template <typename T>
struct TVector_Object{
	PyObject_HEAD
	TVector<T> value;
	unsigned char size;
};

typedef union TVector<float> Vector;
typedef union TVector<double> VectorD;

typedef struct TVector_Object<float> Vector_Object;
typedef struct TVector_Object<double> VectorD_Object;

extern PyTypeObject Vector_Type;
extern PyTypeObject VectorD_Type;

#define VectorD_Check( op ) PyObject_TypeCheck( op, &VectorD_Type )
#define Vector_Check( op ) PyObject_TypeCheck( op, &Vector_Type )

#endif