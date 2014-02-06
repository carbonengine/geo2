#pragma once
#ifndef Util_h
#define Util_h
#include "Vector.h"

#ifdef _WIN32
#define NOMINMAX
#include <Windows.h>
#include <xnamath.h>
#else
#include "CcpMath/include/xnamath.h"
#include "CcpMath/include/Plane.h"
#endif

template <class T> bool GetRealValue( PyObject* item, T* result )
{
	// We want to make the type checks for floats ourselves
	if( PyFloat_Check( item ) )
	{
		*result = (T)PyFloat_AS_DOUBLE( item );
	}
	else if( PyInt_CheckExact( item ) )
	{
		*result = (T)PyInt_AS_LONG( item );
	}
	else
	{		
		return false;
	}
	return true;
}

// From Python Extraction

// doubles
size_t ExtractDoublesFromSequence( PyObject* seq, VectorD& result, unsigned int minlength );
bool ExtractVectorDFromSequence( PyObject* seq, VectorD& result, unsigned int minlength );
bool ExtractDoubleArrayFromSequence( PyObject* seq, double* result, unsigned int minlength );
bool ExtractVectorDFromSequence( PyObject* seq, unsigned int minlength, VectorD& result, size_t* size );
bool ExtractVectorDFromArgs( PyObject* args, VectorD& result, int minsize );
bool ExtractVectorDPairFromArgs( PyObject* args, VectorD& result1, VectorD& result2, int minsize  );

// floats
size_t ExtractFloatsFromSequence( PyObject* seq, Vector& result, unsigned int minlength );
bool ExtractVectorFromSequence( PyObject* seq, Vector& result, unsigned int minlength );
bool ExtractFloatArrayFromSequence( PyObject* seq, float* result, unsigned int minlength );
bool ExtractVectorFromSequence( PyObject* seq, unsigned int minlength, Vector& result, size_t* size );
bool ExtractVectorPairFromArgs( PyObject* args, Vector& result1, Vector& result2, int minsize  );
bool ExtractVectorFromArgs( PyObject* args, Vector& result, int minsize );

bool ExtractXMVectorFromSequence( PyObject* seq, XMVECTOR& result, unsigned int minlength );
bool ExtractXMVectorFromSequence( PyObject* seq, unsigned int minlength, XMVECTOR& result, size_t* size );
bool ExtractFloat2FromSequence( PyObject* seq, XMFLOAT2& result );
bool ExtractFloat3FromSequence( PyObject* seq, XMFLOAT3& result );
bool ExtractFloat4FromSequence( PyObject* seq, XMFLOAT4& result );
bool ExtractXMMatrixFromSequence( PyObject* seq, XMMATRIX& mat );
bool ExtractXMVectorPairFromArgs( PyObject* args, XMVECTOR& result1, XMVECTOR& result2, int minsize  );
bool ExtractXMVectorFromArgs( PyObject* args, XMVECTOR& result, int minsize );


// To Python Conversion
// double
PyObject* VectorDAsPyFloats( VectorD& v, int length );
PyObject* VectorAsPyFloats( Vector& v, int length );
PyObject* FloatArrayAsPyFloats( float* v, int length );
PyObject* DoubleArrayAsPyFloats( double* v, int length );

// single
PyObject* XMVectorAsPyFloats( XMVECTOR v, int length );
PyObject* XMMatrixAsTuple( const XMMATRIX& m );

#endif // Util_h