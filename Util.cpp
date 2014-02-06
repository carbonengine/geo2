
#include "Util.h"
#include <algorithm>

PyObject* GetFastItems( PyObject* seq, size_t minSize, PyObject*** items, size_t* nrofitems )
{
	// We use the Fast functions because when returned by PySequence_Fast we have allready
	// made sure that it is a sequence or a tuple, so we don't have to do all the type checking
	PyObject* tuple = PySequence_Fast( seq, "geo2: expected a sequence" );
	if( !tuple )
	{
		// We don't have to bother with refcount since we did not get a sequence
		return NULL;
	}
	
	// We know this is a sequence so we can safely treat it as one.
	size_t tsize = PySequence_Fast_GET_SIZE( tuple );
	if( tsize < minSize )
	{
		PyErr_SetString( PyExc_Exception, "geo2: sequence is of the wrong size!" );
		Py_DECREF( tuple ); // don't forget to decrease the refcount
		return NULL;
	}

	// Get all the items at once.
	*items = PySequence_Fast_ITEMS( tuple );
	*nrofitems = tsize;
	return tuple;
}

template <class T> bool GetRealsFromFastItems( PyObject** items, size_t size, T* result )
{
	// Get all the floating point items from the GetFastItems result

	// The PyFloat_AsDouble returns you a -1 on failure.... WTF.
	// So if we have the -1.0f float value... we don't know if it was 
	// a failure or success unless we check the error string
	for( size_t i = 0; i < size; ++i )
	{
		// We want to make the type checks for floats ourselves
		if( !GetRealValue<T>( items[i], &result[i] ) )
		{
			PyErr_SetString( PyExc_Exception, "geo2: sequence must contain numeric values only" );
			return false;
		}
	}	
	return true;
}

size_t ExtractDoublesFromSequence( PyObject* seq, VectorD& result, unsigned int minlength )
{
	// Check for a vector object first...
	if( VectorD_Check( seq ) )
	{
		result = ( ( VectorD_Object* ) seq )->value;
		return ( ( VectorD_Object* ) seq )->size;
	}

	// Extract the floats from the sequence
	size_t size;	
	PyObject** items;
	PyObject* fastSeq = GetFastItems( seq, minlength, &items, &size );

	if( fastSeq == NULL || !GetRealsFromFastItems<double>( items, std::min( size, size_t( 4 ) ), &result.u[0] ))
	{
		Py_XDECREF( fastSeq );
		return 0;
	}

	Py_DECREF( fastSeq );
	return size;
}

bool ExtractDoubleArrayFromSequence( PyObject* seq, double* result, unsigned int minlength )
{
	// Extract the floats from the sequence
	size_t size;	
	PyObject** items;
	PyObject* fastSeq = GetFastItems( seq, minlength, &items, &size );

	if( fastSeq == NULL || !GetRealsFromFastItems<double>( items, minlength, &result[0] ))
	{
		Py_XDECREF( fastSeq );
		return false;
	}

	Py_DECREF( fastSeq );
	return true;
}

bool ExtractVectorDFromSequence( PyObject* seq, VectorD& result, unsigned int minlength )
{
	// Check for a vector object first...
	if( VectorD_Check( seq ) )
	{
		if ( ( ( VectorD_Object* ) seq )->size < minlength )
		{
			return false;
		}
		result = ( ( VectorD_Object* ) seq )->value;
		return true;
	}

	// Extract the floats from the sequence
	size_t size;	
	PyObject** items;
	PyObject* fastSeq = GetFastItems( seq, minlength, &items, &size );

	if( fastSeq == NULL || !GetRealsFromFastItems<double>( items, std::min( size, size_t( 4 ) ), &result.u[0] ))
	{
		Py_XDECREF( fastSeq );
		return false;
	}

	Py_DECREF( fastSeq );
	return true;
}

bool ExtractVectorDFromSequence( PyObject* seq, unsigned int minlength, VectorD& result, size_t* size )
{
	// Check for a vector object first...
	if( VectorD_Check( seq ) )
	{
		if ( ( ( VectorD_Object* ) seq )->size < minlength )
		{
			return false;
		}
		result = ( ( VectorD_Object* ) seq )->value;
		return true;
	}

	// Extract the floats from the sequence
	PyObject** items;
	PyObject* fastSeq = GetFastItems( seq, minlength, &items, size );

	if( fastSeq == NULL || !GetRealsFromFastItems<double>( items, std::min( *size, size_t( 4 ) ), &result.u[0] ))
	{
		Py_XDECREF( fastSeq );
		return false;
	}

	Py_DECREF( fastSeq );
	return true;
}

size_t ExtractFloatsFromSequence( PyObject* seq, Vector& result, unsigned int minlength )
{
	// Check for a vector object first...
	if( Vector_Check( seq ) )
	{
		result = ( ( Vector_Object* ) seq )->value;
		return ( ( VectorD_Object* ) seq )->size;
	}

	// Extract the floats from the sequence
	size_t size = 0;	
	PyObject** items;
	PyObject* fastSeq = GetFastItems( seq, minlength, &items, &size );

	if( fastSeq == NULL || !GetRealsFromFastItems<float>( items, std::min( size, size_t( 4 ) ), &result.u[0] ))
	{
		Py_XDECREF( fastSeq );
		return 0;
	}

	Py_DECREF( fastSeq );
	return size;
}

bool ExtractFloatArrayFromSequence( PyObject* seq, float* result, unsigned int minlength )
{
	// Extract the floats from the sequence
	size_t size;	
	PyObject** items;
	PyObject* fastSeq = GetFastItems( seq, minlength, &items, &size );

	if( fastSeq == NULL || !GetRealsFromFastItems<float>( items, minlength, &result[0] ))
	{
		Py_XDECREF( fastSeq );
		return false;
	}

	Py_DECREF( fastSeq );
	return true;
}

bool ExtractVectorFromSequence( PyObject* seq, Vector& result, unsigned int minlength )
{
	// Check for a vector object first...
	if( Vector_Check( seq ) )
	{
		if ( ( ( Vector_Object* ) seq )->size < minlength )
		{
			return false;
		}
		result = ( ( Vector_Object* ) seq )->value;
		return true;
	}

	// Extract the floats from the sequence
	size_t size;	
	PyObject** items;
	PyObject* fastSeq = GetFastItems( seq, minlength, &items, &size );

	if( fastSeq == NULL || !GetRealsFromFastItems<float>( items, std::min( size, size_t( 4 ) ), &result.u[0] ))
	{
		Py_XDECREF( fastSeq );
		return false;
	}

	Py_DECREF( fastSeq );
	return true;
}

bool ExtractVectorFromSequence( PyObject* seq, unsigned int minlength, Vector& result, size_t* size )
{
	// Check for a vector object first...
	if( Vector_Check( seq ) )
	{
		if ( ( ( Vector_Object* ) seq )->size < minlength )
		{
			return false;
		}
		result = ( ( Vector_Object* ) seq )->value;
		return true;
	}

	// Extract the floats from the sequence
	PyObject** items;
	PyObject* fastSeq = GetFastItems( seq, minlength, &items, size );

	if( fastSeq == NULL || !GetRealsFromFastItems<float>( items, std::min( *size, size_t( 4 ) ), &result.u[0] ))
	{
		Py_XDECREF( fastSeq );
		return false;
	}

	Py_DECREF( fastSeq );
	return true;
}

bool ExtractXMVectorFromSequence( PyObject* seq, XMVECTOR& result, unsigned int minlength )
{
	// Check for a vector object first...
	if( Vector_Check( seq ) )
	{
		if ( ( ( Vector_Object* ) seq )->size < minlength )
		{
			return false;
		}
		result = XMLoadFloat4( (XMFLOAT4*) &( ( Vector_Object* ) seq )->value.u[0] );
		return true;
	}
	
	// Extract the floats from the sequence
	size_t size;
	float fresult[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	PyObject** items;
	PyObject* fastSeq = GetFastItems( seq, minlength, &items, &size );
	
	if( fastSeq == NULL || !GetRealsFromFastItems<float>( items, std::min( size, size_t( 4 ) ), &fresult[0] ))
	{
		Py_XDECREF( fastSeq );
		return false;
	}
	
	result = XMLoadFloat4( (XMFLOAT4*) fresult );
	Py_DECREF( fastSeq );
	return true;
}

bool ExtractXMVectorFromSequence( PyObject* seq, unsigned int minlength, XMVECTOR& result, size_t* size )
{
	// Check for a vector object first...
	if( Vector_Check( seq ) )
	{
		if ( ( ( Vector_Object* ) seq )->size < minlength )
		{
			return false;
		}
		result = XMLoadFloat4( (XMFLOAT4*) &( ( Vector_Object* ) seq )->value.u[0] );
		return true;
	}
	
	// Extract the floats from the sequence
	float fresult[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	PyObject** items;
	PyObject* fastSeq = GetFastItems( seq, minlength, &items, size );
	
	if( fastSeq == NULL || !GetRealsFromFastItems<float>( items, std::min( *size, size_t( 4 ) ), &fresult[0] ))
	{
		Py_XDECREF( fastSeq );
		return false;
	}
	
	result = XMLoadFloat4( (XMFLOAT4*) fresult );
	Py_DECREF( fastSeq );
	return true;
}
//-------------------------------------------------------------------
// Floats
bool ExtractFloat2FromSequence( PyObject* seq, XMFLOAT2& result )
{
	size_t size;
	PyObject** items;
	PyObject* fastSeq = GetFastItems( seq, 2, &items, &size );
	
	if( fastSeq == NULL || !GetRealsFromFastItems<float>( items, std::min( size, size_t( 2 ) ), &result.x ))
	{
		Py_XDECREF( fastSeq );
		return false;
	}
	Py_DECREF( fastSeq );

	return true;
}

bool ExtractFloat3FromSequence( PyObject* seq, XMFLOAT3& result )
{
	size_t size;
	PyObject** items;
	PyObject* fastSeq = GetFastItems( seq, 3, &items, &size );
	
	if( fastSeq == NULL || !GetRealsFromFastItems<float>( items, std::min( size, size_t( 3 ) ), &result.x ))
	{
		Py_XDECREF( fastSeq );
		return false;
	}
	Py_DECREF( fastSeq );

	return true;
}

bool ExtractFloat4FromSequence( PyObject* seq, XMFLOAT4& result )
{
	size_t size;
	PyObject** items;
	PyObject* fastSeq = GetFastItems( seq, 4, &items, &size );
	
	if( fastSeq == NULL || !GetRealsFromFastItems<float>( items, std::min( size, size_t( 4 ) ), &result.x ))
	{
		Py_XDECREF( fastSeq );
		return false;
	}
	Py_DECREF( fastSeq );

	return true;
}

bool ExtractXMMatrixFromSequence( PyObject* seq, XMMATRIX& mat )
{

	size_t size;
	PyObject** items;
	PyObject* fastSeq = GetFastItems( seq, 4, &items, &size );
	if( fastSeq == NULL )
	{
		return false;
	}

	for( int i = 0; i < 4; ++i )
	{	
		if( !ExtractXMVectorFromSequence( items[i], mat.r[i], 4 ) )
		{
			Py_DECREF( fastSeq );
			return false;
		}
	}
	Py_DECREF( fastSeq );
	return true;
}

bool ExtractXMVectorPairFromArgs( PyObject* args, XMVECTOR& result1, XMVECTOR& result2, int minsize  )
{
	PyObject* p1 = NULL;
	PyObject* p2 = NULL;

	if(    !PyArg_ParseTuple( args, "OO", &p1, &p2 ) 
		|| !ExtractXMVectorFromSequence( p1, result1, minsize )
		|| !ExtractXMVectorFromSequence( p2, result2, minsize ) )
	{
		return false;
	}

	return true;
}

bool ExtractVectorDPairFromArgs( PyObject* args, VectorD& result1, VectorD& result2, int minsize  )
{
	PyObject* p1 = NULL;
	PyObject* p2 = NULL;

	if(    !PyArg_ParseTuple( args, "OO", &p1, &p2 ) 
		|| !ExtractVectorDFromSequence( p1, result1, minsize )
		|| !ExtractVectorDFromSequence( p2, result2, minsize ) )
	{
		return false;
	}

	return true;
}

bool ExtractVectorPairFromArgs( PyObject* args, Vector& result1, Vector& result2, int minsize  )
{
	PyObject* p1 = NULL;
	PyObject* p2 = NULL;

	if(    !PyArg_ParseTuple( args, "OO", &p1, &p2 ) 
		|| !ExtractVectorFromSequence( p1, result1, minsize )
		|| !ExtractVectorFromSequence( p2, result2, minsize ) )
	{
		return false;
	}

	return true;
}

bool ExtractXMVectorFromArgs( PyObject* args, XMVECTOR& result, int minsize )
{
	PyObject* p = NULL;

	if(    !PyArg_ParseTuple( args, "O", &p ) 
		|| !ExtractXMVectorFromSequence( p, result, minsize ) )
	{
		return false;
	}

	return true;
}

bool ExtractVectorDFromArgs( PyObject* args, VectorD& result, int minsize )
{
	PyObject* p = NULL;

	if(    !PyArg_ParseTuple( args, "O", &p ) 
		|| !ExtractVectorDFromSequence( p, result, minsize ) )
	{
		return false;
	}

	return true;
}

bool ExtractVectorFromArgs( PyObject* args, Vector& result, int minsize )
{
	PyObject* p = NULL;

	if(    !PyArg_ParseTuple( args, "O", &p ) 
		|| !ExtractVectorFromSequence( p, result, minsize ) )
	{
		return false;
	}

	return true;
}

// Conversion
PyObject* XMVectorAsPyFloats( XMVECTOR v, int length )
{
	if ( length > 1 )
	{
		PyObject* ret = PyTuple_New( length );

		for ( int i = 0; i < length; ++i )
		{
			PyTuple_SET_ITEM( ret, i, PyFloat_FromDouble( XMVectorGetByIndex( v, i ) ) );
		}

		return ret;
	}
	else
	{
		return PyFloat_FromDouble( XMVectorGetX( v ) );
	}
}

// Conversion
PyObject* VectorDAsPyFloats( VectorD& v, int length )
{
	if ( length > 1 )
	{
		PyObject* ret = PyTuple_New( length );

		for ( int i = 0; i < length; ++i )
		{
			PyTuple_SET_ITEM( ret, i, PyFloat_FromDouble(  v.u[i] ) );
		}

		return ret;
	}
	else
	{
		return PyFloat_FromDouble( v.u[0] );
	}
}

// Conversion
PyObject* VectorAsPyFloats( Vector& v, int length )
{
	if ( length > 1 )
	{
		PyObject* ret = PyTuple_New( length );

		for ( int i = 0; i < length; ++i )
		{
			PyTuple_SET_ITEM( ret, i, PyFloat_FromDouble(  v.u[i] ) );
		}

		return ret;
	}
	else
	{
		return PyFloat_FromDouble( v.u[0] );
	}
}

// Conversion
PyObject* FloatArrayAsPyFloats( float* v, int length )
{
	if ( length > 1 )
	{
		PyObject* ret = PyTuple_New( length );

		for ( int i = 0; i < length; ++i )
		{
			PyTuple_SET_ITEM( ret, i, PyFloat_FromDouble(  v[i] ) );
		}

		return ret;
	}
	else
	{
		return PyFloat_FromDouble( v[0] );
	}
}

PyObject* DoubleArrayAsPyFloats( double* v, int length )
{
	if ( length > 1 )
	{
		PyObject* ret = PyTuple_New( length );

		for ( int i = 0; i < length; ++i )
		{
			PyTuple_SET_ITEM( ret, i, PyFloat_FromDouble(  v[i] ) );
		}

		return ret;
	}
	else
	{
		return PyFloat_FromDouble( v[0] );
	}
}

PyObject* XMMatrixAsTuple( const XMMATRIX& m )
{
	PyObject* ret = PyTuple_New( 4 );
	PyObject* row1 = PyTuple_New( 4 );
	PyObject* row2 = PyTuple_New( 4 );
	PyObject* row3 = PyTuple_New( 4 );
	PyObject* row4 = PyTuple_New( 4 );

	for ( int i = 0; i < 4; ++i )
	{
		PyTuple_SET_ITEM( row1, i, PyFloat_FromDouble( *(&m._11 + i) ) );
		PyTuple_SET_ITEM( row2, i, PyFloat_FromDouble( *(&m._21 + i) ) );
		PyTuple_SET_ITEM( row3, i, PyFloat_FromDouble( *(&m._31 + i) ) );
		PyTuple_SET_ITEM( row4, i, PyFloat_FromDouble( *(&m._41 + i) ) );
	}

	PyTuple_SET_ITEM( ret, 0, row1 );	
	PyTuple_SET_ITEM( ret, 1, row2 );	
	PyTuple_SET_ITEM( ret, 2, row3 );	
	PyTuple_SET_ITEM( ret, 3, row4 );	
	return ret;
}