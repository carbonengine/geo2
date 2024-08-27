
#include <CcpMath.h>
#include <Python.h>
#include "Vector.h"
#include "Util.h"
#include <algorithm>
#include <limits>
#include <cstdint>
#include <cmath>

// Function wrappers.... lots of them
#define VectorFunArgV( XMFUNC, PYFUNC, MINSIZE, RETURNSIZE ) \
PyObject* PYFUNC( PyObject *module, PyObject *args )\
{\
	PyObject* p1 = NULL;\
	XMVECTOR a;\
\
	if( !PyArg_ParseTuple( args, "O", &p1 )\
		|| !ExtractXMVectorFromSequence( p1, a, MINSIZE ))\
	{\
		return NULL;\
	}\
\
	return XMVectorAsPyFloats( XMFUNC( a ), RETURNSIZE );\
}

#define VectorFunArgVV( XMFUNC, PYFUNC, MINSIZE, RETURNSIZE ) \
PyObject* PYFUNC( PyObject *module, PyObject *args )\
{\
	XMVECTOR a, b;\
\
	if( !ExtractXMVectorPairFromArgs( args, a, b, MINSIZE ) )\
	{\
		return NULL;\
	}\
\
	return XMVectorAsPyFloats( XMFUNC( a, b ), RETURNSIZE );\
}

#define VectorFunArgVVV( XMFUNC, PYFUNC, MINSIZE, RETURNSIZE ) \
PyObject* PYFUNC( PyObject *module, PyObject *args )\
{\
	XMVECTOR a,b,c;\
	PyObject* p1 = NULL;\
	PyObject* p2 = NULL;\
	PyObject* p3 = NULL;\
\
	if( !PyArg_ParseTuple( args, "OOO", &p1, &p2, &p3 )\
		|| !ExtractXMVectorFromSequence( p1, a, MINSIZE )\
		|| !ExtractXMVectorFromSequence( p2, b, MINSIZE )\
		|| !ExtractXMVectorFromSequence( p3, c, MINSIZE ) )\
	{\
		return NULL;\
	}\
\
	return XMVectorAsPyFloats( XMFUNC( a, b, c ), RETURNSIZE );\
}

#define VectorFunArgVVF( XMFUNC, PYFUNC, MINSIZE, RETURNSIZE ) \
PyObject* PYFUNC( PyObject *module, PyObject *args )\
{\
	PyObject* p1 = NULL;\
	PyObject* p2 = NULL;\
	XMVECTOR a, b;\
	float s;\
\
	if( !PyArg_ParseTuple( args, "OOf", &p1, &p2, &s )\
		|| !ExtractXMVectorFromSequence( p1, a, MINSIZE )\
		|| !ExtractXMVectorFromSequence( p2, b, MINSIZE ))\
	{\
		return NULL;\
	}\
\
	return XMVectorAsPyFloats( XMFUNC( a, b, s ), RETURNSIZE );\
}

#define VectorFunArgVF( XMFUNC, PYFUNC, MINSIZE, RETURNSIZE ) \
PyObject* PYFUNC( PyObject *module, PyObject *args )\
{\
	PyObject* p1 = NULL;\
	XMVECTOR a;\
	float s;\
\
	if( !PyArg_ParseTuple( args, "Of", &p1, &s )\
		|| !ExtractXMVectorFromSequence( p1, a, MINSIZE ))\
	{\
		return NULL;\
	}\
\
	return XMVectorAsPyFloats( XMFUNC( a, s ), RETURNSIZE );\
}

#define VectorFunArgVVVFF( XMFUNC, PYFUNC, MINSIZE, RETURNSIZE ) \
PyObject* PYFUNC( PyObject *module, PyObject *args )\
{\
	XMVECTOR a,b,c;\
	float f,g;\
	PyObject* p1 = NULL;\
	PyObject* p2 = NULL;\
	PyObject* p3 = NULL;\
\
	if( !PyArg_ParseTuple( args, "OOOff", &p1, &p2, &p3, &f, &g )\
		|| !ExtractXMVectorFromSequence( p1, a, MINSIZE )\
		|| !ExtractXMVectorFromSequence( p2, b, MINSIZE )\
		|| !ExtractXMVectorFromSequence( p3, c, MINSIZE ) )\
	{\
		return NULL;\
	}\
\
	return XMVectorAsPyFloats( XMFUNC( a, b, c, f, g ), RETURNSIZE );\
}

#define VectorFunArgVVVVF( XMFUNC, PYFUNC, MINSIZE, RETURNSIZE ) \
PyObject* PYFUNC( PyObject *module, PyObject *args )\
{\
	XMVECTOR a,b,c,d;\
	float f;\
	PyObject* p1 = NULL;\
	PyObject* p2 = NULL;\
	PyObject* p3 = NULL;\
	PyObject* p4 = NULL;\
\
	if( !PyArg_ParseTuple( args, "OOOOf", &p1, &p2, &p3, &p4, &f )\
		|| !ExtractXMVectorFromSequence( p1, a, MINSIZE )\
		|| !ExtractXMVectorFromSequence( p2, b, MINSIZE )\
		|| !ExtractXMVectorFromSequence( p3, c, MINSIZE )\
		|| !ExtractXMVectorFromSequence( p4, d, MINSIZE ) )\
	{\
		return NULL;\
	}\
\
	return XMVectorAsPyFloats( XMFUNC( a, b, c, d, f ), RETURNSIZE );\
}

#define VectorFunArgVM( XMFUNC, PYFUNC, MINSIZE, RETURNSIZE ) \
PyObject* PYFUNC( PyObject *module, PyObject *args )\
{\
	XMVECTOR b;\
	XMMATRIX m;\
	PyObject* p1 = NULL;\
	PyObject* p2 = NULL;\
\
	if( !PyArg_ParseTuple( args, "OO", &p1, &p2 )\
		|| !ExtractXMVectorFromSequence( p1, b, MINSIZE )\
		|| !ExtractXMMatrixFromSequence( p2, m ) )\
	{\
		return NULL;\
	}\
\
	return XMVectorAsPyFloats( XMFUNC( b, m ), RETURNSIZE );\
}

/*

// If we extract the coefficients for the curve at the start of each segment we
// can save ourselves a lot of calculations

// Lets calculate this and extract s^3, s^2, s
// p1 = y1
// p2 = y2
// dx = x2 - x1
// c1 and c2
// The polynomial works for the unit interval so we need to normalize
// p1*(2s^3 - 3s^2 + 1) + p2*(-2s^3 + 3s^2) + t1(s^3 -2s^2 + s) + t2(s^3 - s^2)
//  = 2s^3p1 - 3s^2p1 + p1 -2s^3p2 + 3s^2p2 + s^3t1 -2s^2t1 + st1 + s^3t2 - s^2t2
//  = s^3(2p1 -2p2 + t1 + t2) + s^2(-3p1 + 3p2 -2t1 - t2 ) + s( t1 ) + p1
//c1:  (2p1 - 2p2 + t1 + t2)/dx^3
//c2:  (-3p1 + 3p2 -2t1 - t2)/dx^2
//c3:  t1
//c4:  p1	

*/
PyObject* CreateHermiteSegment( PyObject *module, PyObject *args )
{

	float startValue, endValue, inTangent, outTangent, timeRange;
	float segment[6];
	if( !PyArg_ParseTuple( args, "fffff", &startValue, &inTangent, &endValue, &outTangent, &timeRange ) )
	{
		return NULL;
	}

	float invlen = 1.0f/(timeRange*timeRange*timeRange);
	float t1 = timeRange*inTangent;
	float t2 = timeRange*outTangent;
	float dt = endValue - startValue;
	segment[0] = (-2.0f*dt + t1 + t2)*invlen;
	segment[1] = (3.0f*dt - 2.0f*t1 - t2)*invlen*timeRange;
	segment[2] = inTangent;
	segment[3] = startValue;
	segment[4] = endValue;
	segment[5] = timeRange;	
	return FloatArrayAsPyFloats( &segment[0], 6 );
}

PyObject* CreateHermiteSegmentD( PyObject *module, PyObject *args )
{

	double startValue, endValue, inTangent, outTangent, timeRange;
	double segment[6];
	if( !PyArg_ParseTuple( args, "ddddd", &startValue,  &inTangent, &endValue, &outTangent, &timeRange ) )
	{
		return NULL;
	}

	double invlen = 1.0/(timeRange*timeRange*timeRange);
	double t1 = timeRange*inTangent;
	double t2 = timeRange*outTangent;
	double dt = endValue - startValue;
	segment[0] = (-2.0*dt + t1 + t2)*invlen;
	segment[1] = (3.0*dt - 2.0*t1 - t2)*invlen*timeRange;
	segment[2] = inTangent;
	segment[3] = startValue;
	segment[4] = endValue;
	segment[5] = timeRange;	
	return DoubleArrayAsPyFloats( &segment[0], 6 );
}

PyObject* EvaluateHermiteSegment( PyObject *module, PyObject *args )
{
	float time;
	float segment[6];
	PyObject* p1 = NULL;
	if( !PyArg_ParseTuple( args, "Of", &p1, &time )
		|| !ExtractFloatArrayFromSequence( p1, &segment[0], 6 ) )
	{
		return NULL;
	}

	if( time < 0.0f )
	{
		return PyFloat_FromDouble( segment[3] );
	}
	else if( time >= segment[5] )
	{
		return PyFloat_FromDouble( segment[4] );
	}
	else
	{
		return PyFloat_FromDouble( time * (time * (time * segment[0] + segment[1]) + segment[2]) + segment[3] );
	}
}

PyObject* EvaluateHermiteSegmentD( PyObject *module, PyObject *args )
{
	double time;
	double segment[6];
	PyObject* p1 = NULL;
	if( !PyArg_ParseTuple( args, "Od", &p1, &time )
		|| !ExtractDoubleArrayFromSequence( p1, &segment[0], 6 ) )
	{
		return NULL;
	}

	if( time < 0.0 )
	{
		return PyFloat_FromDouble( segment[3] );
	}
	else if( time >= segment[5] )
	{
		return PyFloat_FromDouble( segment[4] );
	}
	else
	{
		return PyFloat_FromDouble( time * (time * (time * segment[0] + segment[1]) + segment[2]) + segment[3] );
	}
}

PyObject* VecDirection( PyObject *module, PyObject *args )
{
	XMVECTOR a,b, diff;
	PyObject* p1 = NULL;
	PyObject* p2 = NULL;
	if( !PyArg_ParseTuple( args, "OO", &p1, &p2 )
		|| !ExtractXMVectorFromSequence( p1, a, 2 )
		|| !ExtractXMVectorFromSequence( p2, b, 2 ) )
	{
		return NULL;
	}
	diff = XMVectorSubtract(a, b);
	float length = XMVectorGetX( XMVector3Length( diff ) );
	size_t minsize = std::min( PySequence_Size( p1 ), PySequence_Size( p2 ));

	if( length == 0.0f )
	{
		return XMVectorAsPyFloats( XMVectorZero(), (int)minsize );
	}
	else
	{
		return XMVectorAsPyFloats( XMVector3Normalize( diff ), (int)minsize );
	}
}

// generic vector functions, do not care about the size
PyObject* VecScale( PyObject *module, PyObject *args )
{
	XMVECTOR a;
	float s;
	PyObject* p1 = NULL;
	if( !PyArg_ParseTuple( args, "Of", &p1, &s )
		|| !ExtractXMVectorFromSequence( p1, a, 2 ))
	{
		return NULL;
	}

	return XMVectorAsPyFloats( XMVectorScale( a, s ), (int)PySequence_Size( p1 ) );
}

PyObject* VecLerp( PyObject *module, PyObject *args )
{
	XMVECTOR a, b;
	float s;
	PyObject* p1 = NULL;
	PyObject* p2 = NULL;
	if( !PyArg_ParseTuple( args, "OOf", &p1, &p2, &s )
		|| !ExtractXMVectorFromSequence( p1, a, 2 )
		|| !ExtractXMVectorFromSequence( p2, b, 2 ) )
	{
		return NULL;
	}
	
	size_t minsize = std::min( PySequence_Size( p1 ), PySequence_Size( p2 ));

	return XMVectorAsPyFloats( XMVectorLerp( a, b, s ), (int)minsize );
}

PyObject* VecAdd( PyObject *module, PyObject *args )
{
	XMVECTOR a, b;
	PyObject* p1 = NULL;
	PyObject* p2 = NULL;
	if( !PyArg_ParseTuple( args, "OO", &p1, &p2 )
		|| !ExtractXMVectorFromSequence( p1, a, 2 )
		|| !ExtractXMVectorFromSequence( p2, b, 2 ) )
	{
		return NULL;
	}
	
	size_t minsize = std::min( PySequence_Size( p1 ), PySequence_Size( p2 ));

	return XMVectorAsPyFloats( XMVectorAdd( a, b ), (int)minsize );
}

PyObject* VecSubtract( PyObject *module, PyObject *args )
{
	XMVECTOR a, b;
	PyObject* p1 = NULL;
	PyObject* p2 = NULL;
	if( !PyArg_ParseTuple( args, "OO", &p1, &p2 )
		|| !ExtractXMVectorFromSequence( p1, a, 2 )
		|| !ExtractXMVectorFromSequence( p2, b, 2 ) )
	{
		return NULL;
	}
	
	size_t minsize = std::min( PySequence_Size( p1 ), PySequence_Size( p2 ));

	return XMVectorAsPyFloats( XMVectorSubtract( a, b ), (int)minsize );
}

PyObject* VecMaximize( PyObject *module, PyObject *args )
{
	XMVECTOR a, b;
	PyObject* p1 = NULL;
	PyObject* p2 = NULL;
	if( !PyArg_ParseTuple( args, "OO", &p1, &p2 )
		|| !ExtractXMVectorFromSequence( p1, a, 2 )
		|| !ExtractXMVectorFromSequence( p2, b, 2 ) )
	{
		return NULL;
	}
	
	size_t minsize = std::min( PySequence_Size( p1 ), PySequence_Size( p2 ));

	return XMVectorAsPyFloats( XMVectorMax( a, b ), (int)minsize );
}

PyObject* VecMinimize( PyObject *module, PyObject *args )
{
	XMVECTOR a, b;
	PyObject* p1 = NULL;
	PyObject* p2 = NULL;
	if( !PyArg_ParseTuple( args, "OO", &p1, &p2 )
		|| !ExtractXMVectorFromSequence( p1, a, 2 )
		|| !ExtractXMVectorFromSequence( p2, b, 2 ) )
	{
		return NULL;
	}
	
	size_t minsize = std::min( PySequence_Size( p1 ), PySequence_Size( p2 ));

	return XMVectorAsPyFloats( XMVectorMin( a, b ), (int)minsize );
}

PyObject* VecBaryCentric( PyObject *module, PyObject *args )
{
	XMVECTOR a,b,c;
	float f,g;
	PyObject* p1 = NULL;
	PyObject* p2 = NULL;
	PyObject* p3 = NULL;

	if( !PyArg_ParseTuple( args, "OOOff", &p1, &p2, &p3, &f, &g )
		|| !ExtractXMVectorFromSequence( p1, a, 2 )
		|| !ExtractXMVectorFromSequence( p2, b, 2 )
		|| !ExtractXMVectorFromSequence( p3, c, 2 ) )
	{
		return NULL;
	}

	return XMVectorAsPyFloats( XMVectorBaryCentric( a, b, c, f, g ), 4 );
}

PyObject* VecCatmullRom( PyObject *module, PyObject *args )
{
	XMVECTOR a,b,c,d;
	float f;
	PyObject* p1 = NULL;
	PyObject* p2 = NULL;
	PyObject* p3 = NULL;
	PyObject* p4 = NULL;

	if( !PyArg_ParseTuple( args, "OOOOf", &p1, &p2, &p3, &p4, &f )
		|| !ExtractXMVectorFromSequence( p1, a, 2 )
		|| !ExtractXMVectorFromSequence( p2, b, 2 )
		|| !ExtractXMVectorFromSequence( p3, c, 2 )
		|| !ExtractXMVectorFromSequence( p4, d, 2 ) )
	{
		return NULL;
	}

	return XMVectorAsPyFloats( XMVectorCatmullRom( a, b, c, d, f ), 4 );
}

PyObject* VecHermite( PyObject *module, PyObject *args )
{
	XMVECTOR a,b,c,d;
	float f;
	PyObject* p1 = NULL;
	PyObject* p2 = NULL;
	PyObject* p3 = NULL;
	PyObject* p4 = NULL;

	if( !PyArg_ParseTuple( args, "OOOOf", &p1, &p2, &p3, &p4, &f )
		|| !ExtractXMVectorFromSequence( p1, a, 2 )
		|| !ExtractXMVectorFromSequence( p2, b, 2 )
		|| !ExtractXMVectorFromSequence( p3, c, 2 )
		|| !ExtractXMVectorFromSequence( p4, d, 2 ) )
	{
		return NULL;
	}

	return XMVectorAsPyFloats( XMVectorHermite( a, b, c, d, f ), 4 );
}

// multiply, mat*mat, vec*mat, mat*vec
PyObject* Multiply( PyObject *module, PyObject *args )
{
	XMVECTOR v;
	XMMATRIX matA, matB;
	PyObject* p1 = NULL;
	PyObject* p2 = NULL;
	PyObject* result = NULL;

	bool aisvec = false;
	bool bisvec = false;

	if( !PyArg_ParseTuple( args, "OO", &p1, &p2 )
		|| !PyTuple_Check( p1 )
		|| !PyTuple_Check( p2 ))
	{
		return NULL;
	}

	if ( PyFloat_Check( PySequence_GetItem( p1 , 0 ) ) )
	{
		if ( !ExtractXMVectorFromSequence( p1, v, 3 ) )
		{
			return NULL;			
		}
		aisvec = true;
	}

	if ( PyFloat_Check( PySequence_GetItem( p2 , 0 ) ) )
	{
		if ( !ExtractXMVectorFromSequence( p2, v, 3 ) )
		{
			return NULL;
		}
		bisvec = true;
	}

	if ( bisvec && aisvec )
	{
		PyErr_SetString( PyExc_Exception, "geo2::Multiply: Can't multiply two vectors" );
		return NULL;
	}

	if( !( aisvec || bisvec ) )
	{
		if( !ExtractXMMatrixFromSequence( p1, matA )
			|| !ExtractXMMatrixFromSequence( p2, matB ))
		{
			return NULL;
		}
		else
		{
			return XMMatrixAsTuple( XMMatrixMultiply( matA, matB ) );
		}
	}
	else if ( aisvec ) 
	{
		if ( ExtractXMMatrixFromSequence( p2, matB ) )
		{
			return XMVectorAsPyFloats( XMVector3Transform( v, matB ), 4 );
		}	
	}
	else if ( bisvec )
	{
		if ( ExtractXMMatrixFromSequence( p1, matA ) )
		{
			return XMVectorAsPyFloats( XMVector3Transform( v, matA ), 4 );
		}	
	}		

	return NULL;
}

// 2d vector funcs
VectorFunArgVV( XMVectorAdd, Vec2Add, 2, 2 )
VectorFunArgVV( XMVectorSubtract, Vec2Subtract, 2, 2 )
VectorFunArgVF( XMVectorScale, Vec2Scale, 2, 2 )
VectorFunArgVVF( XMVectorLerp, Vec2Lerp, 2, 2 )
VectorFunArgVV( XMVectorMax, Vec2Maximize, 2, 2 )
VectorFunArgVV( XMVectorMin, Vec2Minimize, 2, 2 )
VectorFunArgVVVFF( XMVectorBaryCentric, Vec2BaryCentric, 2, 2 )
VectorFunArgVVVVF( XMVectorCatmullRom, Vec2CatmullRom, 2, 2 )
VectorFunArgVVVVF( XMVectorHermite, Vec2Hermite, 2, 2 )

VectorFunArgVV( XMVector2Cross, Vec2CCW, 2, 1 )
VectorFunArgVV( XMVector2Dot, Vec2Dot, 2, 1 )
VectorFunArgV( XMVector2Length, Vec2Length, 2, 1 )
VectorFunArgV( XMVector2LengthSq, Vec2LengthSq, 2, 1 )
VectorFunArgV( XMVector2Normalize, Vec2Normalize, 2, 2 )
VectorFunArgV( XMVectorNegate, Vec2Negate, 2, 2 )
VectorFunArgVM( XMVector2Transform, Vec2Transform, 2, 2 )
VectorFunArgVM( XMVector2TransformCoord, Vec2TransformCoord, 2, 2 )
VectorFunArgVM( XMVector2TransformNormal, Vec2TransformNormal, 2, 2 )

PyObject* Vec2TransformArray( PyObject *module, PyObject *args )
{
	XMMATRIX world;
	PyObject* p1 = NULL;
	PyObject* p2 = NULL;
	
	if( !PyArg_ParseTuple( args, "OO", &p1, &p2 ) )
	{
		return NULL;
	}

	if( !ExtractXMMatrixFromSequence( p2, world ) )
	{
		return NULL;
	}

	// Get the size of the tuple
	uint32_t size = (uint32_t)PySequence_Fast_GET_SIZE( p1 );
	XMFLOAT2 *inarray = new XMFLOAT2[size];
	XMFLOAT4 *outarray = new XMFLOAT4[size];

	for ( uint32_t i = 0; i < size; i++ )
	{
		PyObject* t = PySequence_GetItem( p1, i );
		bool ok = ExtractFloat2FromSequence( t, inarray[i]);
		Py_XDECREF( t );
		if( !ok )
		{
			delete [] inarray;
			delete [] outarray;
			return NULL;
		}
	}
	
	XMVector2TransformStream( outarray, sizeof( XMFLOAT4 ), inarray, sizeof( XMFLOAT2 ), size, world );

	PyObject* ret = PyTuple_New( size );

	for ( uint32_t i = 0; i < size; i++ )
	{
		PyObject* t = PyTuple_New( 4 );
		PyTuple_SET_ITEM( t, 0, PyFloat_FromDouble( outarray[i].x ) );
		PyTuple_SET_ITEM( t, 1, PyFloat_FromDouble( outarray[i].y ) );
		PyTuple_SET_ITEM( t, 2, PyFloat_FromDouble( outarray[i].z ) );
		PyTuple_SET_ITEM( t, 3, PyFloat_FromDouble( outarray[i].w ) );
		PyTuple_SET_ITEM( ret, i, t );
	}

	delete [] inarray;
	delete [] outarray;

	return ret;
}

PyObject* Vec2TransformCoordArray( PyObject *module, PyObject *args )
{
	XMMATRIX world;
	PyObject* p1 = NULL;
	PyObject* p2 = NULL;

	if( !PyArg_ParseTuple( args, "OO", &p1, &p2 ) )
	{
		return NULL;
	}

	if( !ExtractXMMatrixFromSequence( p2, world ) )
	{
		return NULL;
	}

	// Get the size of the tuple
	uint32_t size = (uint32_t)PySequence_Fast_GET_SIZE( p1 );
	XMFLOAT2 *inarray = new XMFLOAT2[size];
	XMFLOAT2 *outarray = new XMFLOAT2[size];

	for ( uint32_t i = 0; i < size; i++ )
	{
		PyObject* t = PySequence_GetItem( p1, i );
		bool ok = ExtractFloat2FromSequence( t, inarray[i] );
		Py_XDECREF( t );
		if( !ok )
		{
			delete [] inarray;
			delete [] outarray;
			return NULL;
		}
	}

	XMVector2TransformCoordStream( outarray, sizeof( XMFLOAT2 ), inarray, sizeof( XMFLOAT2 ), size, world );

	PyObject* ret = PyTuple_New( size );

	for ( uint32_t i = 0; i < size; i++ )
	{
		PyObject* t = PyTuple_New( 2 );
		PyTuple_SET_ITEM( t, 0, PyFloat_FromDouble( outarray[i].x ) );
		PyTuple_SET_ITEM( t, 1, PyFloat_FromDouble( outarray[i].y ) );
		PyTuple_SET_ITEM( ret, i, t );
	}

	delete [] inarray;
	delete [] outarray;

	return ret;
}

PyObject* Vec2TransformNormalArray( PyObject *module, PyObject *args )
{
	XMMATRIX world;
	PyObject* p1 = NULL;
	PyObject* p2 = NULL;

	if( !PyArg_ParseTuple( args, "OO", &p1, &p2 ) )
	{
		return NULL;
	}

	if( !ExtractXMMatrixFromSequence( p2, world ) )
	{
		return NULL;
	}

	// Get the size of the tuple
	uint32_t size = (uint32_t)PySequence_Fast_GET_SIZE( p1 );
	XMFLOAT2 *inarray = new XMFLOAT2[size];
	XMFLOAT2 *outarray = new XMFLOAT2[size];

	for ( uint32_t i = 0; i < size; i++ )
	{
		PyObject* t = PySequence_GetItem( p1, i );
		bool ok = ExtractFloat2FromSequence( t, inarray[i] );
		Py_XDECREF( t );
		if( !ok )
		{
			delete [] inarray;
			delete [] outarray;
			return NULL;
		}
	}

	XMVector2TransformNormalStream( outarray, sizeof( XMFLOAT2 ), inarray, sizeof( XMFLOAT2 ), size, world );

	PyObject* ret = PyTuple_New( size );

	for ( uint32_t i = 0; i < size; i++ )
	{
		PyObject* t = PyTuple_New( 2 );
		PyTuple_SET_ITEM( t, 0, PyFloat_FromDouble( outarray[i].x ) );
		PyTuple_SET_ITEM( t, 1, PyFloat_FromDouble( outarray[i].y ) );
		PyTuple_SET_ITEM( ret, i, t );
	}

	delete [] inarray;
	delete [] outarray;

	return ret;
}

// 3d vector funcs
VectorFunArgVV( XMVectorAdd, Vec3Add, 2, 3 )
VectorFunArgVV( XMVectorSubtract, Vec3Subtract, 2, 3 )
VectorFunArgVF( XMVectorScale, Vec3Scale, 2, 3 )
VectorFunArgVVF( XMVectorLerp, Vec3Lerp, 2, 3 )
VectorFunArgVV( XMVectorMax, Vec3Maximize, 2, 3 )
VectorFunArgVV( XMVectorMin, Vec3Minimize, 2, 3 )
VectorFunArgVVVFF( XMVectorBaryCentric, Vec3BaryCentric, 2, 3 )
VectorFunArgVVVVF( XMVectorCatmullRom, Vec3CatmullRom, 2, 3 )
VectorFunArgVVVVF( XMVectorHermite, Vec3Hermite, 2, 3 )

VectorFunArgVV( XMVector3Cross, Vec3Cross, 3, 3 )
VectorFunArgVV( XMVector3Dot, Vec3Dot, 3, 1 )
VectorFunArgV( XMVector3Length, Vec3Length, 3, 1 )
VectorFunArgV( XMVector3LengthSq, Vec3LengthSq, 3, 1 )
VectorFunArgV( XMVectorNegate, Vec3Negate, 3, 3 )
VectorFunArgVM( XMVector3Transform, Vec3Transform, 3, 3 )
VectorFunArgVM( XMVector3TransformCoord, Vec3TransformCoord, 3, 3 )
VectorFunArgVM( XMVector3TransformNormal, Vec3TransformNormal, 3, 3 )

PyObject* Vec3LerpD( PyObject *module, PyObject *args )
{
	VectorD a,b;
	PyObject* p1 = NULL;
	PyObject* p2 = NULL;
	double t = 0.0;
	if( !PyArg_ParseTuple( args, "OOd", &p1, &p2, &t )
		|| !ExtractVectorDFromSequence( p1, a, 3 )
		|| !ExtractVectorDFromSequence( p2, b, 3 )
		)
	{
		return NULL;
	}
	// V0 + t * (V1 - V0)
	a.x += t*(b.x - a.x);
	a.y += t*(b.y - a.y);
	a.z += t*(b.z - a.z);

	return VectorDAsPyFloats( a, 3 );
}

PyObject* Vec3DirectionD( PyObject *module, PyObject *args )
{
	VectorD a,b;
	if( !ExtractVectorDPairFromArgs( args, a, b, 3 ) )
	{
		return NULL;
	}
	VectorD result;
	result.x = a.x-b.x;
	result.y = a.y-b.y;
	result.z = a.z-b.z;

	double length = sqrt(result.x*result.x+result.y*result.y+result.z*result.z);

	if( length > std::numeric_limits<double>::epsilon() )
	{
		result.x /= length;
		result.y /= length;
		result.z /= length;
		return VectorDAsPyFloats( result, 3 );
	}
	else
	{
		double zero[3] = {0.0, 0.0, 0.0};
		return DoubleArrayAsPyFloats(zero, 3);
	}
	
}

PyObject* Vec3AddD( PyObject *module, PyObject *args )
{
	VectorD a,b;
	if( !ExtractVectorDPairFromArgs( args, a, b, 3 ) )
	{
		return NULL;
	}
	VectorD result;
	result.x = a.x+b.x;
	result.y = a.y+b.y;
	result.z = a.z+b.z;
	return VectorDAsPyFloats( result, 3 );
}

PyObject* Vec3SubtractD( PyObject *module, PyObject *args )
{
	VectorD a,b;
	if( !ExtractVectorDPairFromArgs( args, a, b, 3 ) )
	{
		return NULL;
	}
	VectorD result;
	result.x = a.x-b.x;
	result.y = a.y-b.y;
	result.z = a.z-b.z;
	return VectorDAsPyFloats( result, 3 );
}

PyObject* Vec3ScaleD( PyObject *module, PyObject *args )
{
	VectorD a;
	PyObject* p1 = NULL;
	double scale = 1.0;
	if( !PyArg_ParseTuple( args, "Od", &p1, &scale )
		|| !ExtractVectorDFromSequence( p1, a, 3 )
		)
	{
		return NULL;
	}
	VectorD result;
	result.x = a.x*scale;
	result.y = a.y*scale;
	result.z = a.z*scale;
	return VectorDAsPyFloats( result, 3 );
}

PyObject* Vec3DotD( PyObject *module, PyObject *args )
{
	VectorD a,b;
	if( !ExtractVectorDPairFromArgs( args, a, b, 3 ) )
	{
		return NULL;
	}

	return PyFloat_FromDouble( a.x * b.x + a.y * b.y + a.z * b.z );
}

PyObject* Vec3CrossD( PyObject *module, PyObject *args )
{
	VectorD a,b;
	if( !ExtractVectorDPairFromArgs( args, a, b, 3 ) )
	{
		return NULL;
	}

	VectorD result;
	result.x =  a.y*b.z - a.z*b.y;
	result.y =  a.z*b.x - a.x*b.z;
	result.z =  a.x*b.y - a.y*b.x;

	return VectorDAsPyFloats( result, 3 );
}

PyObject* Vec3Equal( PyObject *module, PyObject *args )
{
	Vector a,b;
	if( !ExtractVectorPairFromArgs( args, a, b, 3 ) )
	{
		return NULL;
	}
	if(a.x != b.x || a.y != b.y || a.z != b.z)
	{
		return PyBool_FromLong( 0 );
	}
	else
	{
		return PyBool_FromLong( 1 );
	}
}

PyObject* Vec3EqualD( PyObject *module, PyObject *args )
{
	VectorD a,b;
	if( !ExtractVectorDPairFromArgs( args, a, b, 3 ) )
	{
		return NULL;
	}

	if(a.x != b.x || a.y != b.y || a.z != b.z)
	{
		return PyBool_FromLong( 0 );
	}
	else
	{
		return PyBool_FromLong( 1 );
	}
}

PyObject* Vec3NearEqual( PyObject *module, PyObject *args )
{
	Vector a,b;
	PyObject* p1 = NULL;
	PyObject* p2 = NULL;
	float epsilon = std::numeric_limits<float>::epsilon();
	if( !PyArg_ParseTuple( args, "OO|f", &p1, &p2, &epsilon )
		|| !ExtractVectorFromSequence( p1, a, 3 )
		|| !ExtractVectorFromSequence( p2, b, 3 )
		)
	{
		return NULL;
	}

	if(fabs(a.x - b.x) > epsilon || fabs(a.y - b.y) > epsilon || fabs(a.z - b.z) > epsilon)
	{
		return PyBool_FromLong( 0 );
	}
	else
	{
		return PyBool_FromLong( 1 );
	}
}

PyObject* Vec3NearEqualD( PyObject *module, PyObject *args )
{
	VectorD a,b;
	PyObject* p1 = NULL;
	PyObject* p2 = NULL;
	double epsilon = std::numeric_limits<double>::epsilon();
	if( !PyArg_ParseTuple( args, "OO|d", &p1, &p2, &epsilon )
		|| !ExtractVectorDFromSequence( p1, a, 3 )
		|| !ExtractVectorDFromSequence( p2, b, 3 )
		)
	{
		return NULL;
	}

	if(fabs(a.x - b.x) > epsilon || fabs(a.y - b.y) > epsilon || fabs(a.z - b.z) > epsilon)
	{
		return PyBool_FromLong( 0 );
	}
	else
	{
		return PyBool_FromLong( 1 );
	}
}

PyObject* Vec3Distance( PyObject *module, PyObject *args )
{
	XMVECTOR a,b;

	if( !ExtractXMVectorPairFromArgs( args, a, b, 3 ) )
	{
		return NULL;
	}

	return XMVectorAsPyFloats( XMVector3Length( (a - b) ), 1 );
}

PyObject* Vec3DistanceSq( PyObject *module, PyObject *args )
{
	XMVECTOR a, b;

	if( !ExtractXMVectorPairFromArgs( args, a, b, 3 ) )
	{
		return NULL;
	}

	return XMVectorAsPyFloats( XMVector3LengthSq( (a - b) ), 1 );
}

PyObject* Vec3DistanceD( PyObject *module, PyObject *args )
{
	VectorD a,b;

	if( !ExtractVectorDPairFromArgs( args, a, b, 3 ) )
	{
		return NULL;
	}
	VectorD delta;
	delta.x =  a.x - b.x;
	delta.y =  a.y - b.y;
	delta.z =  a.z - b.z;
	return PyFloat_FromDouble(sqrt(delta.x*delta.x+delta.y*delta.y+delta.z*delta.z));
}

PyObject* Vec3DistanceSqD( PyObject *module, PyObject *args )
{
	VectorD a,b;

	if( !ExtractVectorDPairFromArgs( args, a, b, 3 ) )
	{
		return NULL;
	}
	VectorD delta;
	delta.x =  a.x - b.x;
	delta.y =  a.y - b.y;
	delta.z =  a.z - b.z;
	return PyFloat_FromDouble(delta.x*delta.x+delta.y*delta.y+delta.z*delta.z);
}

PyObject* Vec3Normalize( PyObject* module, PyObject* args)
{
	XMVECTOR v;

	if( !ExtractXMVectorFromArgs( args, v, 3) )
	{
		return NULL;
	}

	float length = XMVectorGetX( XMVector3Length( v ) );

	if( length == 0.0f )
	{
		return XMVectorAsPyFloats( XMVectorZero(), 3 );
	}
	else
	{
		return XMVectorAsPyFloats( XMVector3Normalize( v ), 3 );
	}
}

PyObject* Vec3NormalizeD( PyObject* module, PyObject* args)
{
	VectorD v;

	if( !ExtractVectorDFromArgs( args, v, 3) )
	{
		return NULL;
	}

	double length = sqrt(v.x*v.x+v.y*v.y+v.z*v.z);

	if( fabs( length ) < std::numeric_limits<double>::epsilon() )
	{	
		VectorD zero = {0.0, 0.0, 0.0, 0.0};
		return VectorDAsPyFloats( zero, 3 );
	}
	else
	{
		VectorD normalized = {v.x/length, v.y/length, v.z/length, 0.0};
		return VectorDAsPyFloats( normalized, 3 );
	}
}

PyObject* Vec3NegateD( PyObject* module, PyObject* args)
{
    VectorD v;

	if( !ExtractVectorDFromArgs( args, v, 3) )
	{
		return NULL;
	}

	v.x = -v.x;
	v.y = -v.y;
	v.z = -v.z;

	return VectorDAsPyFloats(  v, 3 );
}

PyObject* Vec3LengthD( PyObject* module, PyObject* args)
{
	VectorD v;

	if( !ExtractVectorDFromArgs( args, v, 3) )
	{
		return NULL;
	}
	return PyFloat_FromDouble(sqrt(v.x*v.x+v.y*v.y+v.z*v.z));
}

PyObject* Vec3LengthSqD( PyObject* module, PyObject* args)
{
	VectorD v;

	if( !ExtractVectorDFromArgs( args, v, 3) )
	{
		return NULL;
	}
	return PyFloat_FromDouble((v.x*v.x+v.y*v.y+v.z*v.z));
}

PyObject* Vec3Project( PyObject *module, PyObject *args )
{
	XMVECTOR b;
	float X, Y, Width, Height, MinZ, MaxZ;
	XMMATRIX projection, view, world;

	PyObject* p1 = NULL;
	PyObject* p2 = NULL;
	PyObject* p3 = NULL;
	PyObject* p4 = NULL;
	PyObject* p5 = NULL;

	if( !PyArg_ParseTuple( args, "OOOOO", &p1, &p2, &p3, &p4, &p5 )
		|| !ExtractXMVectorFromSequence( p1, b, 3 ) )
	{
		return NULL;
	}

	X = (float)PyLong_AsLong( PySequence_GetItem( p2, 0 ) );
	Y = (float)PyLong_AsLong( PySequence_GetItem( p2, 1 ) );
	Width = (float)PyLong_AsLong( PySequence_GetItem( p2, 2 ) );
	Height = (float)PyLong_AsLong( PySequence_GetItem( p2, 3 ) );
	MinZ = (float)PyFloat_AsDouble( PySequence_GetItem( p2, 4 ) );
	MaxZ = (float)PyFloat_AsDouble( PySequence_GetItem( p2, 5 ) );

	if( !ExtractXMMatrixFromSequence( p3, projection ) )
	{
		return NULL;
	}
	
	if( !ExtractXMMatrixFromSequence( p4, view ) )
	{
		return NULL;
	}
	
	if( !ExtractXMMatrixFromSequence( p5, world ) )
	{
		return NULL;
	}

	return XMVectorAsPyFloats( XMVector3Project( b, X, Y, Width, Height, MinZ, MaxZ, projection, view, world ), 4 );
}

PyObject* Vec3ProjectArray( PyObject *module, PyObject *args )
{
	float X, Y, Width, Height, MinZ, MaxZ;
	XMMATRIX projection, view, world;

	PyObject* p1 = NULL;
	PyObject* p2 = NULL;
	PyObject* p3 = NULL;
	PyObject* p4 = NULL;
	PyObject* p5 = NULL;

	if( !PyArg_ParseTuple( args, "OOOOO", &p1, &p2, &p3, &p4, &p5 ) )
	{
		return NULL;
	}

	X = (float)PyLong_AsLong( PySequence_GetItem( p2, 0 ) );
	Y = (float)PyLong_AsLong( PySequence_GetItem( p2, 1 ) );
	Width = (float)PyLong_AsLong( PySequence_GetItem( p2, 2 ) );
	Height = (float)PyLong_AsLong( PySequence_GetItem( p2, 3 ) );
	MinZ = (float)PyFloat_AsDouble( PySequence_GetItem( p2, 4 ) );
	MaxZ = (float)PyFloat_AsDouble( PySequence_GetItem( p2, 5 ) );

	if( !ExtractXMMatrixFromSequence( p3, projection ) )
	{
		return NULL;
	}

	if( !ExtractXMMatrixFromSequence( p4, view ) )
	{
		return NULL;
	}

	if( !ExtractXMMatrixFromSequence( p5, world ) )
	{
		return NULL;
	}

	// Get the size of the tuple
	uint32_t size = (uint32_t)PySequence_Fast_GET_SIZE( p1 );
	XMFLOAT3 *inarray = new XMFLOAT3[size];
	XMFLOAT3 *outarray = new XMFLOAT3[size];

	for ( uint32_t i = 0; i < size; i++ )
	{
		PyObject* t = PySequence_GetItem( p1, i );
		bool ok = ExtractFloat3FromSequence( t, inarray[i] );
		Py_XDECREF( t );
		if( !ok )
		{
			delete [] inarray;
			delete [] outarray;
			return NULL;
		}
	}

	XMVector3ProjectStream( outarray, sizeof( XMFLOAT3 ), inarray, sizeof( XMFLOAT3 ), size, X, Y, Width, Height, MinZ, MaxZ, projection, view, world );

	PyObject* ret = PyTuple_New( size );

	for ( uint32_t i = 0; i < size; i++ )
	{
		PyObject* t = PyTuple_New( 3 );
		PyTuple_SET_ITEM( t, 0, PyFloat_FromDouble( outarray[i].x ) );
		PyTuple_SET_ITEM( t, 1, PyFloat_FromDouble( outarray[i].y ) );
		PyTuple_SET_ITEM( t, 2, PyFloat_FromDouble( outarray[i].z ) );
		PyTuple_SET_ITEM( ret, i, t );
	}

	delete [] inarray;
	delete [] outarray;

	return ret;
}

PyObject* Vec3TransformArray( PyObject *module, PyObject *args )
{
	XMMATRIX world;
	PyObject* p1 = NULL;
	PyObject* p2 = NULL;

	if( !PyArg_ParseTuple( args, "OO", &p1, &p2 ) )
	{
		return NULL;
	}

	if( !ExtractXMMatrixFromSequence( p2, world ) )
	{
		return NULL;
	}

	// Get the size of the tuple
	uint32_t size = (uint32_t)PySequence_Fast_GET_SIZE( p1 );
	XMFLOAT3 *inarray = new XMFLOAT3[size];
	XMFLOAT4 *outarray = new XMFLOAT4[size];

	for ( uint32_t i = 0; i < size; i++ )
	{
		PyObject* t = PySequence_GetItem( p1, i );
		bool ok = ExtractFloat3FromSequence( t, inarray[i] );
		Py_XDECREF( t );
		if( !ok )
		{
			delete [] inarray;
			delete [] outarray;
			return NULL;
		}
	}

	XMVector3TransformStream( outarray, sizeof( XMFLOAT4 ), inarray, sizeof( XMFLOAT3 ), size, world );

	PyObject* ret = PyTuple_New( size );

	for ( uint32_t i = 0; i < size; i++ )
	{
		PyObject* t = PyTuple_New( 4 );
		PyTuple_SET_ITEM( t, 0, PyFloat_FromDouble( outarray[i].x ) );
		PyTuple_SET_ITEM( t, 1, PyFloat_FromDouble( outarray[i].y ) );
		PyTuple_SET_ITEM( t, 2, PyFloat_FromDouble( outarray[i].z ) );
		PyTuple_SET_ITEM( t, 3, PyFloat_FromDouble( outarray[i].w ) );
		PyTuple_SET_ITEM( ret, i, t );
	}

	delete [] inarray;
	delete [] outarray;

	return ret;
}

PyObject* Vec3DistanceSqFromBoundingBox( PyObject *module, PyObject * args )
{
	PyObject* pyPoint = NULL;
	PyObject* pyMin = NULL;
	PyObject* pyMax = NULL;

	XMVECTOR point;
	XMVECTOR aabbMin;
	XMVECTOR aabbMax;

	if( !PyArg_ParseTuple( args, "OOO", &pyPoint, &pyMin, &pyMax ) 
		|| !ExtractXMVectorFromSequence( pyPoint, point, 3 )
		|| !ExtractXMVectorFromSequence( pyMin, aabbMin, 3 )
		|| !ExtractXMVectorFromSequence( pyMax, aabbMax, 3 ))
	{
		return NULL;
	}

	point -= aabbMin;
	aabbMax -= aabbMin;
	for(int i = 0; i < 3; i++)
	{
		if( XMVectorGetByIndex( point, i ) > 0 )
		{
			point = XMVectorSetByIndex( point, std::max((XMVectorGetByIndex(point,i) - XMVectorGetByIndex(aabbMax, i )), 0.0f), i );
		}
	}
	
	return XMVectorAsPyFloats( XMVector3LengthSq( point ), 1);
}

PyObject* Vec3TransformCoordArray( PyObject *module, PyObject *args )
{
	XMMATRIX world;
	PyObject* p1 = NULL;
	PyObject* p2 = NULL;

	if( !PyArg_ParseTuple( args, "OO", &p1, &p2 ) )
	{
		return NULL;
	}

	if( !ExtractXMMatrixFromSequence( p2, world ) )
	{
		return NULL;
	}

	// Get the size of the tuple
	uint32_t size = (uint32_t)PySequence_Fast_GET_SIZE( p1 );
	XMFLOAT3 *inarray = new XMFLOAT3[size];
	XMFLOAT3 *outarray = new XMFLOAT3[size];

	for ( uint32_t i = 0; i < size; i++ )
	{
		PyObject* t = PySequence_GetItem( p1, i );
		bool ok = ExtractFloat3FromSequence( t, inarray[i] );
		Py_XDECREF( t );
		if( !ok )
		{
			delete [] inarray;
			delete [] outarray;
			return NULL;
		}
	}

	XMVector3TransformCoordStream( outarray, sizeof( XMFLOAT3 ), inarray, sizeof( XMFLOAT3 ), size, world );

	PyObject* ret = PyTuple_New( size );

	for ( uint32_t i = 0; i < size; i++ )
	{
		PyObject* t = PyTuple_New( 3 );
		PyTuple_SET_ITEM( t, 0, PyFloat_FromDouble( outarray[i].x ) );
		PyTuple_SET_ITEM( t, 1, PyFloat_FromDouble( outarray[i].y ) );
		PyTuple_SET_ITEM( t, 2, PyFloat_FromDouble( outarray[i].z ) );
		PyTuple_SET_ITEM( ret, i, t );
	}

	delete [] inarray;
	delete [] outarray;

	return ret;
}

PyObject* Vec3TransformNormalArray( PyObject *module, PyObject *args )
{
	XMMATRIX world;
	PyObject* p1 = NULL;
	PyObject* p2 = NULL;

	if( !PyArg_ParseTuple( args, "OO", &p1, &p2 ) )
	{
		return NULL;
	}

	if( !ExtractXMMatrixFromSequence( p2, world ) )
	{
		return NULL;
	}

	// Get the size of the tuple
	uint32_t size = (uint32_t)PySequence_Fast_GET_SIZE( p1 );
	XMFLOAT3 *inarray = new XMFLOAT3[size];
	XMFLOAT3 *outarray = new XMFLOAT3[size];

	for ( uint32_t i = 0; i < size; i++ )
	{
		PyObject* t = PySequence_GetItem( p1, i );
		bool ok = ExtractFloat3FromSequence( t, inarray[i] );
		Py_XDECREF( t );
		if( !ok )
		{
			delete [] inarray;
			delete [] outarray;
			return NULL;
		}
	}

	XMVector3TransformNormalStream( outarray, sizeof( XMFLOAT3 ), inarray, sizeof( XMFLOAT3 ), size, world );

	PyObject* ret = PyTuple_New( size );

	for ( uint32_t i = 0; i < size; i++ )
	{
		PyObject* t = PyTuple_New( 3 );
		PyTuple_SET_ITEM( t, 0, PyFloat_FromDouble( outarray[i].x ) );
		PyTuple_SET_ITEM( t, 1, PyFloat_FromDouble( outarray[i].y ) );
		PyTuple_SET_ITEM( t, 2, PyFloat_FromDouble( outarray[i].z ) );
		PyTuple_SET_ITEM( ret, i, t );
	}

	delete [] inarray;
	delete [] outarray;

	return ret;
}

PyObject* Vec3Unproject( PyObject *module, PyObject *args )
{
	XMVECTOR a;
	float X, Y, Width, Height, MinZ, MaxZ;
	XMMATRIX projection, view, world;

	PyObject* p1 = NULL;
	PyObject* p3 = NULL;
	PyObject* p4 = NULL;
	PyObject* p5 = NULL;

	if( !PyArg_ParseTuple( args, "O(ffffff)OOO", &p1, &X, &Y, &Width, &Height, &MinZ, &MaxZ, &p3, &p4, &p5 )
		|| !ExtractXMVectorFromSequence( p1, a, 3 ) )
	{
		return NULL;
	}

	if( !ExtractXMMatrixFromSequence( p3, projection ) )
	{
		return NULL;
	}

	if( !ExtractXMMatrixFromSequence( p4, view ) )
	{
		return NULL;
	}

	if( !ExtractXMMatrixFromSequence( p5, world ) )
	{
		return NULL;
	}

	
	return XMVectorAsPyFloats( XMVector3Unproject( a, X, Y, Width, Height, MinZ, MaxZ, projection, view, world ), 3 );
}

PyObject* Vec3UnprojectArray( PyObject *module, PyObject *args )
{
	float X, Y, Width, Height, MinZ, MaxZ;
	XMMATRIX projection, view, world;

	PyObject* p1 = NULL;
	PyObject* p3 = NULL;
	PyObject* p4 = NULL;
	PyObject* p5 = NULL;

	if(!PyArg_ParseTuple( args, "O(ffffff)OOO", &p1, &X, &Y, &Width, &Height, &MinZ, &MaxZ, &p3, &p4, &p5 ) )
	{
		return NULL;
	}

	if( !ExtractXMMatrixFromSequence( p3, projection ) )
	{
		return NULL;
	}

	if( !ExtractXMMatrixFromSequence( p4, view ) )
	{
		return NULL;
	}

	if( !ExtractXMMatrixFromSequence( p5, world ) )
	{
		return NULL;
	}
	// Get the size of the tuple
	uint32_t size = (uint32_t)PySequence_Fast_GET_SIZE( p1 );
	XMFLOAT3 *inarray = new XMFLOAT3[size];
	XMFLOAT3 *outarray = new XMFLOAT3[size];

	for ( uint32_t i = 0; i < size; i++ )
	{
		PyObject* t = PySequence_GetItem( p1, i );
		bool ok = ExtractFloat3FromSequence( t, inarray[i] );
		Py_XDECREF( t );
		if( !ok )
		{
			delete [] inarray;
			delete [] outarray;
			return NULL;
		}
	}

	XMVector3UnprojectStream( outarray, sizeof( XMFLOAT3 ), inarray, sizeof( XMFLOAT3 ), size, X, Y, Width, Height, MinZ, MaxZ, projection, view, world );

	PyObject* ret = PyTuple_New( size );

	for ( uint32_t i = 0; i < size; i++ )
	{
		PyObject* t = PyTuple_New( 3 );
		PyTuple_SET_ITEM( t, 0, PyFloat_FromDouble( outarray[i].x ) );
		PyTuple_SET_ITEM( t, 1, PyFloat_FromDouble( outarray[i].y ) );
		PyTuple_SET_ITEM( t, 2, PyFloat_FromDouble( outarray[i].z ) );
		PyTuple_SET_ITEM( ret, i, t );
	}

	delete [] inarray;
	delete [] outarray;

	return ret;
}

// 4d vector funcs
VectorFunArgVV( XMVectorAdd, Vec4Add, 2, 4 )
VectorFunArgVV( XMVectorSubtract, Vec4Subtract, 2, 4 )
VectorFunArgVF( XMVectorScale, Vec4Scale, 2, 4 )
VectorFunArgVVF( XMVectorLerp, Vec4Lerp, 2, 4 )
VectorFunArgVV( XMVectorMax, Vec4Maximize, 2, 4 )
VectorFunArgVV( XMVectorMin, Vec4Minimize, 2, 4 )
VectorFunArgVVVFF( XMVectorBaryCentric, Vec4BaryCentric, 2, 4 )
VectorFunArgVVVVF( XMVectorCatmullRom, Vec4CatmullRom, 2, 4 )
VectorFunArgVVVVF( XMVectorHermite, Vec4Hermite, 2, 4 )

VectorFunArgVVV( XMVector4Cross, Vec4Cross, 4, 4 )
VectorFunArgVV( XMVector4Dot, Vec4Dot, 4, 1 )
VectorFunArgV( XMVector4Length, Vec4Length, 4, 1 )
VectorFunArgV( XMVector4LengthSq, Vec4LengthSq, 4, 1 )
VectorFunArgV( XMVectorNegate, Vec4Negate, 4, 4 )
VectorFunArgV( XMVector4Normalize, Vec4Normalize, 4, 4 )
VectorFunArgVM( XMVector4Transform, Vec4Transform, 4, 4 )


PyObject* Vec4TransformArray( PyObject *module, PyObject *args )
{
	XMMATRIX world;
	PyObject* p1 = NULL;
	PyObject* p2 = NULL;

	if( !PyArg_ParseTuple( args, "OO", &p1, &p2 ) )
	{
		return NULL;
	}

	if( !ExtractXMMatrixFromSequence( p2, world ) )
	{
		return NULL;
	}

	// Get the size of the tuple
	uint32_t size = (uint32_t)PySequence_Fast_GET_SIZE( p1 );
	XMFLOAT4 *inarray = new XMFLOAT4[size];
	XMFLOAT4 *outarray = new XMFLOAT4[size];

	for ( uint32_t i = 0; i < size; i++ )
	{
		PyObject* t = PySequence_GetItem( p1, i );
		bool ok = ExtractFloat4FromSequence( t, inarray[i] );
		Py_XDECREF( t );
		if( !ok )
		{
			delete [] inarray;
			delete [] outarray;
			return NULL;
		}
	}

	XMVector4TransformStream( outarray, sizeof( XMFLOAT4 ), inarray, sizeof( XMFLOAT4 ), size, world );

	PyObject* ret = PyTuple_New( size );

	for ( uint32_t i = 0; i < size; i++ )
	{
		PyObject* t = PyTuple_New( 4 );
		PyTuple_SET_ITEM( t, 0, PyFloat_FromDouble( outarray[i].x ) );
		PyTuple_SET_ITEM( t, 1, PyFloat_FromDouble( outarray[i].y ) );
		PyTuple_SET_ITEM( t, 2, PyFloat_FromDouble( outarray[i].z ) );
		PyTuple_SET_ITEM( t, 3, PyFloat_FromDouble( outarray[i].w ) );
		PyTuple_SET_ITEM( ret, i, t );
	}

	delete [] inarray;
	delete [] outarray;

	return ret;
}

// Matrix funcs
PyObject* MatrixAffineTransformation( PyObject *module, PyObject *args )
{
	float scale;
	XMVECTOR rotationCenter, translation;
	XMVECTOR rotation;
	XMVECTOR scaling;

	PyObject* p1 = NULL;
	PyObject* p2 = NULL;
	PyObject* p3 = NULL;

	if( !PyArg_ParseTuple( args, "fOOO", &scale, &p1, &p2, &p3 ) )
	{
		return NULL;
	}

	scaling = XMVectorSet( scale, scale, scale, scale );

	// Null arguments are treated as identities
	if ( p1 != Py_None )
	{
		if( !ExtractXMVectorFromSequence( p1, rotationCenter, 3 ))
		{
			return NULL;
		}
	}

	if ( p2 != Py_None )
	{
		if( !ExtractXMVectorFromSequence( p2, rotation, 4 ) )
		{
			return NULL;
		}
	}

	if ( p3 != Py_None )
	{
		if( !ExtractXMVectorFromSequence( p3, translation, 3 ) )
		{
			return NULL;
		}
	}

	// Create the tuple
	return XMMatrixAsTuple( XMMatrixAffineTransformation( scaling, rotationCenter, rotation, translation ) );
}

PyObject* MatrixDecompose( PyObject *module, PyObject *args )
{
	PyObject* p1 = NULL;
	XMMATRIX mat;

	XMVECTOR scale, translation;
	XMVECTOR rotation;

	if( !PyArg_ParseTuple( args, "O", &p1 )
		|| !ExtractXMMatrixFromSequence( p1, mat )
		 )
	{
		return NULL;
	}

	if( XMMatrixDecompose( &scale, &rotation, &translation, mat ) )
	{

		PyObject* ret = PyTuple_New( 3 );

		PyObject* scaleT = XMVectorAsPyFloats( scale, 3 );
		PyObject* rotationT = XMVectorAsPyFloats( rotation, 4 );
		PyObject* translationT = XMVectorAsPyFloats( translation, 3 );

		PyTuple_SET_ITEM( ret, 0, scaleT );
		PyTuple_SET_ITEM( ret, 1, rotationT );
		PyTuple_SET_ITEM( ret, 2, translationT );

		return ret;
	}
	else
	{
		Py_INCREF(Py_None);
		return Py_None;
	}
}

PyObject* MatrixIdentity( PyObject *module, PyObject *args )
{
	return XMMatrixAsTuple( XMMatrixIdentity() );
}

PyObject* MatrixIsIdentity( PyObject *module, PyObject *args )
{
	PyObject* p1 = NULL;
	XMMATRIX mat;

	if( !PyArg_ParseTuple( args, "O", &p1 )
		|| !ExtractXMMatrixFromSequence( p1, mat )
		)
	{
		return NULL;
	}
	if( XMMatrixIsIdentity( mat ) )
	{
		return PyLong_FromLong(1);
	}
	else
	{
		return PyLong_FromLong(0);
	}	
}

PyObject* MatrixDeterminant( PyObject *module, PyObject *args )
{
	XMMATRIX mat;
	PyObject* p1 = NULL;
	if( !PyArg_ParseTuple( args, "O", &p1 ) 
		|| !ExtractXMMatrixFromSequence( p1, mat ))
	{
		return NULL;
	}

	return XMVectorAsPyFloats( XMMatrixDeterminant( mat ), 1 );
}

PyObject* MatrixInverse( PyObject *module, PyObject *args )
{
	XMMATRIX mat;
	PyObject* p1 = NULL;
	if( !PyArg_ParseTuple( args, "O", &p1 ) 
		|| !ExtractXMMatrixFromSequence( p1, mat ))
	{
		return NULL;
	}
    XMVECTOR det;
	mat = XMMatrixInverse( &det, mat );
	if( !XMMatrixIsInfinite( mat  ) )
	{
		return XMMatrixAsTuple( mat );
	}
	else
	{
		Py_INCREF(Py_None);
		return Py_None;
	}
}

PyObject* MatrixLookAtLH( PyObject *module, PyObject *args )
{
	XMVECTOR eye, at, up;
	PyObject* p1 = NULL;
	PyObject* p2 = NULL;
	PyObject* p3 = NULL;

	if( !PyArg_ParseTuple( args, "OOO", &p1, &p2, &p3 ) 
		|| !ExtractXMVectorFromSequence( p1, eye, 3 )
		|| !ExtractXMVectorFromSequence( p2, at, 3 )
		|| !ExtractXMVectorFromSequence( p3, up, 3 ) )
	{
		return NULL;
	}

	return XMMatrixAsTuple( XMMatrixLookAtLH( eye, at, up ) );
}

PyObject* MatrixLookAtRH( PyObject *module, PyObject *args )
{
	XMVECTOR eye, at, up;
	PyObject* p1 = NULL;
	PyObject* p2 = NULL;
	PyObject* p3 = NULL;

	if( !PyArg_ParseTuple( args, "OOO", &p1, &p2, &p3 ) 
		|| !ExtractXMVectorFromSequence( p1, eye, 3 )
		|| !ExtractXMVectorFromSequence( p2, at, 3 )
		|| !ExtractXMVectorFromSequence( p3, up, 3 ) )
	{
		return NULL;
	}

	return XMMatrixAsTuple( XMMatrixLookAtRH( eye, at, up ) );
}

PyObject* MatrixMultiply( PyObject *module, PyObject *args )
{
	XMMATRIX mat1, mat2;

	PyObject* p1 = NULL;
	PyObject* p2 = NULL;

	if( !PyArg_ParseTuple( args, "OO", &p1, &p2 ) )
	{
		return NULL;
	}

	if( !ExtractXMMatrixFromSequence( p1, mat1 ) )
	{
		return NULL;
	}

	if( !ExtractXMMatrixFromSequence( p2, mat2 ) )
	{
		return NULL;
	}

	return XMMatrixAsTuple( XMMatrixMultiply( mat1, mat2 ) );
}
PyObject* MatrixPerspectiveFovLH( PyObject *module, PyObject *args )
{
	float fovy, aspect, znear, zfar;
	
	if( !PyArg_ParseTuple( args, "ffff", &fovy, &aspect, &znear, &zfar ) )		
	{
		return NULL;
	}

	return XMMatrixAsTuple( XMMatrixPerspectiveFovLH( fovy, aspect, znear, zfar ) );
}

PyObject* MatrixPerspectiveFovRH( PyObject *module, PyObject *args )
{
	float fovy, aspect, znear, zfar;
	
	if( !PyArg_ParseTuple( args, "ffff", &fovy, &aspect, &znear, &zfar ) )		
	{
		return NULL;
	}

	return XMMatrixAsTuple( XMMatrixPerspectiveFovRH( fovy, aspect, znear, zfar ) );
}

PyObject* MatrixOrthoRH( PyObject *module, PyObject *args )
{
	float width, height, nearz, farz;

	PyObject* p1 = NULL;
	PyObject* p2 = NULL;
	PyObject* p3 = NULL;
	PyObject* p4 = NULL;

	if( !PyArg_ParseTuple( args, "ffff", &width, &height, &nearz, &farz ))
	{
		return NULL;
	}

	return XMMatrixAsTuple( XMMatrixOrthographicRH( width, height, nearz, farz ) );
}

PyObject* MatrixOrthoLH( PyObject *module, PyObject *args )
{
	float width, height, nearz, farz;

	PyObject* p1 = NULL;
	PyObject* p2 = NULL;
	PyObject* p3 = NULL;
	PyObject* p4 = NULL;

	if( !PyArg_ParseTuple( args, "ffff", &width, &height, &nearz, &farz ))
	{
		return NULL;
	}

	return XMMatrixAsTuple( XMMatrixOrthographicLH( width, height, nearz, farz ) );
}

PyObject* MatrixRotationQuaternion( PyObject *module, PyObject *args )
{
	XMVECTOR quat;
	PyObject* p1 = NULL;

	if( !PyArg_ParseTuple( args, "O", &p1 )
		|| !ExtractXMVectorFromSequence( p1, quat, 4 ) )
	{
		return NULL;
	}

	return XMMatrixAsTuple( XMMatrixRotationQuaternion( quat ) );
}

PyObject* MatrixRotationYawPitchRoll( PyObject *module, PyObject *args )
{
	float yaw, pitch, roll;

	if( !PyArg_ParseTuple( args, "fff", &yaw, &pitch, &roll ))
	{
		return NULL;
	}

	return XMMatrixAsTuple( XMMatrixRotationRollPitchYaw( pitch, yaw, roll ) );
}

PyObject* MatrixTransformation( PyObject *module, PyObject *args )
{
	XMVECTOR scalingCenter = {0,0,0};
	XMVECTOR scalingRotation = XMQuaternionIdentity();
	XMVECTOR scaling = {1,1,1};
	XMVECTOR rotationCenter = {0,0,0};
	XMVECTOR rotation = XMQuaternionIdentity();
	XMVECTOR translation = {0,0,0};

	PyObject* p1 = NULL;
	PyObject* p2 = NULL;
	PyObject* p3 = NULL;
	PyObject* p4 = NULL;
	PyObject* p5 = NULL;
	PyObject* p6 = NULL; 

	if( !PyArg_ParseTuple( args, "OOOOOO", &p1, &p2, &p3, &p4, &p5, &p6 ) )
	{
		return NULL;
	}

	// Null arguments are treated as identities
	if ( p1 != Py_None )
	{
		if( !ExtractXMVectorFromSequence( p1, scalingCenter, 3 ) )
		{
			return NULL;
		}
	}

	if ( p2 != Py_None )
	{
		if( !ExtractXMVectorFromSequence( p2, scalingRotation, 4 ) )
		{
			return NULL;
		}
	}

	if ( p3 != Py_None )
	{
		if( !ExtractXMVectorFromSequence( p3, scaling, 3 ) )
		{
			return NULL;
		}
	}
	if ( p4 != Py_None )
	{
		if( !ExtractXMVectorFromSequence( p4, rotationCenter, 3 ) )
		{
			return NULL;
		}
	}

	if ( p5 != Py_None )
	{
		if( !ExtractXMVectorFromSequence( p5, rotation, 4 ) )
		{
			return NULL;
		}
	}

	if ( p6 != Py_None )
	{
		if( !ExtractXMVectorFromSequence( p6, translation, 3 ) )
		{
			return NULL;
		}
	}

	// Create the tuple
	return XMMatrixAsTuple( XMMatrixTransformation( scalingCenter, scalingRotation, scaling, rotationCenter, rotation, translation ) );
}

PyObject* MatrixRotationAxis( PyObject *module, PyObject *args )
{
	float angle;
	PyObject* p1 = NULL;
	XMVECTOR pV;

	if( !PyArg_ParseTuple( args, "Of", &p1, &angle )
		|| !ExtractXMVectorFromSequence( p1, pV, 3 )
		)
	{
		return NULL;
	}
	return XMMatrixAsTuple( XMMatrixRotationAxis( pV, angle ) );
}

PyObject* MatrixRotationNormal( PyObject *module, PyObject *args )
{
	float angle;
	PyObject* p1 = NULL;
	XMVECTOR pV;

	if( !PyArg_ParseTuple( args, "Of", &p1, &angle )
		|| !ExtractXMVectorFromSequence( p1, pV, 3 )
		)
	{
		return NULL;
	}
	return XMMatrixAsTuple( XMMatrixRotationNormal( pV, angle ) );
}

PyObject* MatrixScaling( PyObject *module, PyObject *args )
{
	float x, y, z;

	if( !PyArg_ParseTuple( args, "fff", &x, &y, &z))
	{
		return NULL;
	}
	
	return XMMatrixAsTuple( XMMatrixScaling(  x, y, z ) );
}

PyObject* MatrixTranslation( PyObject *module, PyObject *args )
{
	float x, y, z;

	if( !PyArg_ParseTuple( args, "fff", &x, &y, &z))
	{
		return NULL;
	}
	
	return XMMatrixAsTuple( XMMatrixTranslation(  x, y, z ) );
}

PyObject* MatrixTranspose( PyObject *module, PyObject *args )
{
	XMMATRIX mat;
	PyObject* p1 = NULL;
	if( !PyArg_ParseTuple( args, "O", &p1 ) 
		|| !ExtractXMMatrixFromSequence( p1, mat ))
	{
		return NULL;
	}

	return XMMatrixAsTuple( XMMatrixTranspose( mat ) );
}

PyObject* MatrixRotationX( PyObject *module, PyObject *args )
{
	float angle;
	if( !PyArg_ParseTuple( args, "f", &angle ) )
	{
		return NULL;
	}

	return XMMatrixAsTuple( XMMatrixRotationX( angle ) );
}

PyObject* MatrixRotationY( PyObject *module, PyObject *args )
{
	float angle;
	if( !PyArg_ParseTuple( args, "f", &angle ) )
	{
		return NULL;
	}

	return XMMatrixAsTuple( XMMatrixRotationY( angle ) );
}

PyObject* MatrixRotationZ( PyObject *module, PyObject *args )
{
	float angle;
	if( !PyArg_ParseTuple( args, "f", &angle ) )
	{
		return NULL;
	}

	return XMMatrixAsTuple( XMMatrixRotationZ( angle ) );
}

// plane funcs
PyObject* PlaneFromPoints( PyObject *module, PyObject *args )
{
	XMVECTOR v1, v2, v3;

	PyObject* p1 = NULL;
	PyObject* p2 = NULL;
	PyObject* p3 = NULL;

	if( !PyArg_ParseTuple( args, "OOO", &p1, &p2, &p3 ) 
		|| !ExtractXMVectorFromSequence( p1, v1, 3 )
		|| !ExtractXMVectorFromSequence( p2, v2, 3 )
		|| !ExtractXMVectorFromSequence( p3, v3, 3 ) )
	{
		return NULL;
	}
	
	return XMVectorAsPyFloats( XMPlaneFromPoints( v1, v2, v3 ), 4 );
}

PyObject* PlaneFromPointNormal( PyObject *module, PyObject *args )
{
	XMVECTOR v1, v2;

	PyObject* p1 = NULL;
	PyObject* p2 = NULL;

	if( !PyArg_ParseTuple( args, "OO", &p1, &p2 ) 
		|| !ExtractXMVectorFromSequence( p1, v1, 3 )
		|| !ExtractXMVectorFromSequence( p2, v2, 3 ))
	{
		return NULL;
	}

	return XMVectorAsPyFloats( XMPlaneFromPointNormal( v1, v2 ), 4 );
}

PyObject* PlaneIntersectLine( PyObject *module, PyObject *args )
{
	XMVECTOR v1, v2, v3, v4;

	PyObject* p1 = NULL;
	PyObject* p2 = NULL;
	PyObject* p3 = NULL;

	if( !PyArg_ParseTuple( args, "OOO", &p1, &p2, &p3 ) 
		|| !ExtractXMVectorFromSequence( p1, v1, 4 )
		|| !ExtractXMVectorFromSequence( p2, v2, 3 )
		|| !ExtractXMVectorFromSequence( p3, v3, 3 ) )
	{
		return NULL;
	}
	v4 = XMPlaneIntersectLine( v1, v2, v3 );
	if ( XMPlaneIsNaN( v4 ) )
	{
		Py_INCREF(Py_None);
		return Py_None;		
	}
	
	return XMVectorAsPyFloats( v4, 3 );
}

// Quaternion funcs
PyObject* QuaternionIdentity( PyObject *module, PyObject *args )
{	
	return XMVectorAsPyFloats( XMQuaternionIdentity(), 4 );
}

PyObject* QuaternionInverse( PyObject *module, PyObject *args )
{
	XMVECTOR  q;
	PyObject* p1 = NULL;

	if( !PyArg_ParseTuple( args, "O", &p1 ) 
		|| !ExtractXMVectorFromSequence( p1, q, 4 ) )
	{
		return NULL;
	}

	return XMVectorAsPyFloats( XMQuaternionInverse( q ), 4 );
}

PyObject* QuaternionLengthSq( PyObject *module, PyObject *args )
{
	XMVECTOR q;

	PyObject* p1 = NULL;

	if( !PyArg_ParseTuple( args, "O", &p1 ) 
		|| !ExtractXMVectorFromSequence( p1, q, 4 ) )
	{
		return NULL;
	}

	return XMVectorAsPyFloats( XMQuaternionLengthSq( q ), 1 );
}

PyObject* QuaternionMultiply( PyObject *module, PyObject *args )
{
	XMVECTOR q1, q2;
	PyObject* p1 = NULL;
	PyObject* p2 = NULL;

	if( !PyArg_ParseTuple( args, "OO", &p1, &p2 ) 
		|| !ExtractXMVectorFromSequence( p1, q1, 4 )
		|| !ExtractXMVectorFromSequence( p2, q2, 4 ) )
	{
		return NULL;
	}

	return XMVectorAsPyFloats( XMQuaternionMultiply( q1, q2 ), 4 );
}

PyObject* QuaternionNormalize( PyObject *module, PyObject *args )
{
	XMVECTOR q1;

	PyObject* p1 = NULL;

	if( !PyArg_ParseTuple( args, "O", &p1 ) 
		|| !ExtractXMVectorFromSequence( p1, q1, 4 ) )
	{
		return NULL;
	}

	return XMVectorAsPyFloats( XMQuaternionNormalize( q1 ), 4 );
}

PyObject* QuaternionRotationAxis( PyObject *module, PyObject *args )
{
	XMVECTOR axis;
	float angle = 0.0f;
	PyObject* p1 = NULL;

	if( !PyArg_ParseTuple( args, "Of", &p1, &angle ) 
		|| !ExtractXMVectorFromSequence( p1, axis, 3 ) )
	{
		return NULL;
	}

	XMVECTOR result = XMQuaternionRotationAxis( axis, angle );
	if( XMVector4IsNaN( result ) )
	{
		return XMVectorAsPyFloats( XMQuaternionIdentity(), 4 );
	}
	else
	{
		return XMVectorAsPyFloats( result, 4 );
	}
}

PyObject* QuaternionRotationMatrix( PyObject *module, PyObject *args )
{
	XMMATRIX mat;
	PyObject* p1 = NULL;
	if( !PyArg_ParseTuple( args, "O", &p1 ) 
		|| !ExtractXMMatrixFromSequence( p1, mat ))
	{
		return NULL;
	}

	return XMVectorAsPyFloats( XMQuaternionRotationMatrix( mat ), 4 );
}

PyObject* QuaternionRotationSetYawPitchRoll( PyObject *module, PyObject *args )
{
	float yaw, pitch, roll;
	if( !PyArg_ParseTuple( args, "fff", &yaw, &pitch, &roll ) )
	{
		return NULL;
	}

	return XMVectorAsPyFloats( RotationQuaternion( yaw, pitch, roll ), 4 );
}

PyObject* QuaternionTransformVector( PyObject *module, PyObject *args )
{
	XMVECTOR q;
	XMVECTOR in;

	PyObject* p1 = NULL;
	PyObject* p2 = NULL;

	if( !PyArg_ParseTuple( args, "OO", &p1, &p2 ) 
		|| !ExtractXMVectorFromSequence( p1, q, 4 )
		|| !ExtractXMVectorFromSequence( p2, in, 3 ) )
	{
		return NULL;
	}

	return XMVectorAsPyFloats( XMVector3TransformCoord( in, XMMatrixRotationQuaternion( q ) ), 3 );
}

PyObject* QuaternionRotationArc( PyObject *module, PyObject *args )
{
	XMVECTOR v1, v2;

	PyObject* p1 = NULL;
	PyObject* p2 = NULL;

	if( !PyArg_ParseTuple( args, "OO", &p1, &p2 ) 
		|| !ExtractXMVectorFromSequence( p1, v1, 3 )
		|| !ExtractXMVectorFromSequence( p2, v2, 3 ) )
	{
		return NULL;
	}
	
	float dot = XMVectorGetByIndex( XMVector3Dot( v1, v2 ), 0 );
	auto axis = XMVector3Cross( v1, v2 );
	if( XMVector3Equal( axis, XMVectorZero() ) )
	{
		return XMVectorAsPyFloats( XMQuaternionIdentity(), 4 );
	}
	XMVECTOR result = XMQuaternionRotationAxis( axis, acos( dot ) );
	if( XMVector4IsNaN( result ) )
	{
		return XMVectorAsPyFloats( XMQuaternionIdentity(), 4 );
	}
	else
	{
		return XMVectorAsPyFloats( result, 4 );
	}
}

PyObject* QuaternionRotationGetYawPitchRoll( PyObject *module, PyObject *args )
{
	PyObject* p1 = NULL;

	XMVECTOR q;

	if( !PyArg_ParseTuple( args, "O", &p1 ) 
		|| !ExtractXMVectorFromSequence( p1, q, 4 ) )
	{
		return NULL;
	}

	// Extract vector to float array (since it makes no 
	// sence to use SSE in this function) and change 
	// axes to comply with old function behavior
	float q1[4];
	q1[0] = -XMVectorGetZ( q );
	q1[1] = XMVectorGetY( q );
	q1[2] = XMVectorGetX( q );
	q1[3] = XMVectorGetW( q );

	float yaw, pitch, roll;
	const float pi = 3.141592654f;

	float test = q1[0] * q1[1] + q1[2] * q1[3];
	if( test > 0.499999999f ) 
	{ 
		// singularity at north pole
		yaw = 2 * atan2( q1[0], q1[3] );
		pitch = pi / 2;
		roll = 0;
	}
	else if( test < -0.499999999f ) 
	{ 
		// singularity at south pole
		yaw = -2 * atan2( q1[0], q1[3] );
		pitch = -pi / 2;
		roll = 0;
	}
	else
	{
		float sqx = q1[0] * q1[0];
		float sqy = q1[1] * q1[1];
		float sqz = q1[2] * q1[2];
		yaw = atan2( 2 * q1[1] * q1[3] - 2 * q1[0] * q1[2], 1 - 2 * sqy - 2 * sqz );
		pitch = asin( 2 * test );
		roll = atan2( 2 * q1[0] * q1[3] - 2 * q1[1] * q1[2], 1 - 2 * sqx - 2 * sqz );
	}

	roll = -roll;

	// Fix yaw to be always positive. This is to comply with the
	// old function.
	if( yaw < 0 )
	{
		yaw += pi * 2.f;
	}

	PyObject* ret = PyTuple_New( 3 );

	PyTuple_SET_ITEM( ret, 0, PyFloat_FromDouble( yaw ) );
	PyTuple_SET_ITEM( ret, 1, PyFloat_FromDouble( pitch ) );
	PyTuple_SET_ITEM( ret, 2, PyFloat_FromDouble( roll ) );

	return ret;
}

//-----------------------------------------------------------------
// xnacollision... taken from the dx sample
static const XMVECTOR g_UnitVectorEpsilon =
{
    1.0e-4f, 1.0e-4f, 1.0e-4f, 1.0e-4f
};
//-----------------------------------------------------------------------------
// Return TRUE if the vector is a unit vector (length == 1).
//-----------------------------------------------------------------------------
static inline int32_t XMVector3IsUnit( FXMVECTOR V )
{
    XMVECTOR Difference = XMVector3Length( V ) - XMVectorSplatOne();

    return XMVector4Less( XMVectorAbs( Difference ), g_UnitVectorEpsilon );
}
//-----------------------------------------------------------------------------
// Compute the intersection of a ray (Origin, Direction) with a triangle
// (V0, V1, V2).  Return TRUE if there is an intersection and also set *pDist 
// to the distance along the ray to the intersection.
// 
// The algorithm is based on Moller, Tomas and Trumbore, "Fast, Minimum Storage 
// Ray-Triangle Intersection", Journal of Graphics Tools, vol. 2, no. 1, 
// pp 21-28, 1997.
//-----------------------------------------------------------------------------
int32_t IntersectRayTriangle( FXMVECTOR Origin, FXMVECTOR Direction, FXMVECTOR V0, CXMVECTOR V1, CXMVECTOR V2,
                           float* pDist )
{
    assert( pDist );
    assert( XMVector3IsUnit( Direction ) );

    static const XMVECTOR Epsilon =
    {
        1e-20f, 1e-20f, 1e-20f, 1e-20f
    };

    XMVECTOR Zero = XMVectorZero();

    XMVECTOR e1 = V1 - V0;
    XMVECTOR e2 = V2 - V0;

    // p = Direction ^ e2;
    XMVECTOR p = XMVector3Cross( Direction, e2 );

    // det = e1 * p;
    XMVECTOR det = XMVector3Dot( e1, p );

    XMVECTOR u, v, t;

    if( XMVector3GreaterOrEqual( det, Epsilon ) )
    {
        // Determinate is positive (front side of the triangle).
        XMVECTOR s = Origin - V0;

        // u = s * p;
        u = XMVector3Dot( s, p );

        XMVECTOR NoIntersection = XMVectorLess( u, Zero );
        NoIntersection = XMVectorOrInt( NoIntersection, XMVectorGreater( u, det ) );

        // q = s ^ e1;
        XMVECTOR q = XMVector3Cross( s, e1 );

        // v = Direction * q;
        v = XMVector3Dot( Direction, q );

        NoIntersection = XMVectorOrInt( NoIntersection, XMVectorLess( v, Zero ) );
        NoIntersection = XMVectorOrInt( NoIntersection, XMVectorGreater( u + v, det ) );

        // t = e2 * q;
        t = XMVector3Dot( e2, q );

        NoIntersection = XMVectorOrInt( NoIntersection, XMVectorLess( t, Zero ) );

        if( XMVector4EqualInt( NoIntersection, XMVectorTrueInt() ) )
            return 0;
    }
    else if( XMVector3LessOrEqual( det, -Epsilon ) )
    {
        // Determinate is negative (back side of the triangle).
        XMVECTOR s = Origin - V0;

        // u = s * p;
        u = XMVector3Dot( s, p );

        XMVECTOR NoIntersection = XMVectorGreater( u, Zero );
        NoIntersection = XMVectorOrInt( NoIntersection, XMVectorLess( u, det ) );

        // q = s ^ e1;
        XMVECTOR q = XMVector3Cross( s, e1 );

        // v = Direction * q;
        v = XMVector3Dot( Direction, q );

        NoIntersection = XMVectorOrInt( NoIntersection, XMVectorGreater( v, Zero ) );
        NoIntersection = XMVectorOrInt( NoIntersection, XMVectorLess( u + v, det ) );

        // t = e2 * q;
        t = XMVector3Dot( e2, q );

        NoIntersection = XMVectorOrInt( NoIntersection, XMVectorGreater( t, Zero ) );

        if ( XMVector4EqualInt( NoIntersection, XMVectorTrueInt() ) )
            return 0;
    }
    else
    {
        // Parallel ray.
        return 0;
    }

    XMVECTOR inv_det = XMVectorReciprocal( det );

    t *= inv_det;

    // u * inv_det and v * inv_det are the barycentric cooridinates of the intersection.

    // Store the x-component to *pDist
    XMStoreFloat( pDist, t );

    return 1;
}
//-----------------------------------------------------------------------------
// Compute the intersection of a ray (Origin, Direction) with a sphere.
//-----------------------------------------------------------------------------
int32_t IntersectRaySphere( FXMVECTOR Origin, FXMVECTOR Direction, const XMFLOAT3* spCenter, const float* spRadius, float* pDist )
{
    assert( spCenter );
    assert( pDist );
    assert( XMVector3IsUnit( Direction ) );

    XMVECTOR Center = XMLoadFloat3( spCenter );
    XMVECTOR Radius = XMVectorReplicatePtr( spRadius );

    // l is the vector from the ray origin to the center of the sphere.
    XMVECTOR l = Center - Origin;

    // s is the projection of the l onto the ray direction.
    XMVECTOR s = XMVector3Dot( l, Direction );

    XMVECTOR l2 = XMVector3Dot( l, l );

    XMVECTOR r2 = Radius * Radius;

    // m2 is squared distance from the center of the sphere to the projection.
    XMVECTOR m2 = l2 - s * s;

    XMVECTOR NoIntersection;

    // If the ray origin is outside the sphere and the center of the sphere is 
    // behind the ray origin there is no intersection.
    NoIntersection = XMVectorAndInt( XMVectorLess( s, XMVectorZero() ), XMVectorGreater( l2, r2 ) );

    // If the squared distance from the center of the sphere to the projection
    // is greater than the radius squared the ray will miss the sphere.
    NoIntersection = XMVectorOrInt( NoIntersection, XMVectorGreater( m2, r2 ) );

    // The ray hits the sphere, compute the nearest intersection point.
    XMVECTOR q = XMVectorSqrt( r2 - m2 );
    XMVECTOR t1 = s - q;
    XMVECTOR t2 = s + q;

    XMVECTOR OriginInside = XMVectorLessOrEqual( l2, r2 );
    XMVECTOR t = XMVectorSelect( t1, t2, OriginInside );

    if( XMVector4NotEqualInt( NoIntersection, XMVectorTrueInt() ) )
    {
        // Store the x-component to *pDist.
        XMStoreFloat( pDist, t );
        return 1;
    }

    return 0;
}


//-----------------------------------------------------------------------------
// Return TRUE if any of the elements of a 3 vector are equal to 0xffffffff.
// Slightly more efficient than using XMVector3EqualInt.
//-----------------------------------------------------------------------------
static inline int32_t XMVector3AnyTrue( FXMVECTOR V )
{
    XMVECTOR C;

    // Duplicate the fourth element from the first element.
    C = XMVectorSwizzle( V, 0, 1, 2, 0 );

    return XMComparisonAnyTrue( XMVector4EqualIntR( C, XMVectorTrueInt() ) );
}


//-----------------------------------------------------------------------------
// Compute the intersection of a ray (Origin, Direction) with an axis aligned 
// box using the slabs method.
//-----------------------------------------------------------------------------
int32_t IntersectRayAxisAlignedBox( FXMVECTOR Origin, FXMVECTOR Direction, const XMFLOAT3* center, const XMFLOAT3* extents, float* pDist )
{
    assert( center );
    assert( pDist );
    assert( XMVector3IsUnit( Direction ) );

    static const XMVECTOR Epsilon =
    {
        1e-20f, 1e-20f, 1e-20f, 1e-20f
    };
    static const XMVECTOR FltMin =
    {
        -std::numeric_limits<float>::max(),
        -std::numeric_limits<float>::max(),
        -std::numeric_limits<float>::max(),
        -std::numeric_limits<float>::max()
    };
    static const XMVECTOR FltMax =
    {
        std::numeric_limits<float>::max(),
        std::numeric_limits<float>::max(),
        std::numeric_limits<float>::max(),
        std::numeric_limits<float>::max()
    };

    // Load the box.
    XMVECTOR Center = XMLoadFloat3( center );
    XMVECTOR Extents = XMLoadFloat3( extents );

    // Adjust ray origin to be relative to center of the box.
    XMVECTOR TOrigin = Center - Origin;

    // Compute the dot product againt each axis of the box.
    // Since the axii are (1,0,0), (0,1,0), (0,0,1) no computation is necessary.
    XMVECTOR AxisDotOrigin = TOrigin;
    XMVECTOR AxisDotDirection = Direction;

    // if (fabs(AxisDotDirection) <= Epsilon) the ray is nearly parallel to the slab.
    XMVECTOR IsParallel = XMVectorLessOrEqual( XMVectorAbs( AxisDotDirection ), Epsilon );

    // Test against all three axii simultaneously.
    XMVECTOR InverseAxisDotDirection = XMVectorReciprocal( AxisDotDirection );
    XMVECTOR t1 = ( AxisDotOrigin - Extents ) * InverseAxisDotDirection;
    XMVECTOR t2 = ( AxisDotOrigin + Extents ) * InverseAxisDotDirection;

    // Compute the max of min(t1,t2) and the min of max(t1,t2) ensuring we don't
    // use the results from any directions parallel to the slab.
    XMVECTOR t_min = XMVectorSelect( XMVectorMin( t1, t2 ), FltMin, IsParallel );
    XMVECTOR t_max = XMVectorSelect( XMVectorMax( t1, t2 ), FltMax, IsParallel );

    // t_min.x = maximum( t_min.x, t_min.y, t_min.z );
    // t_max.x = minimum( t_max.x, t_max.y, t_max.z );
    t_min = XMVectorMax( t_min, XMVectorSplatY( t_min ) );  // x = max(x,y)
    t_min = XMVectorMax( t_min, XMVectorSplatZ( t_min ) );  // x = max(max(x,y),z)
    t_max = XMVectorMin( t_max, XMVectorSplatY( t_max ) );  // x = min(x,y)
    t_max = XMVectorMin( t_max, XMVectorSplatZ( t_max ) );  // x = min(min(x,y),z)

    // if ( t_min > t_max ) return FALSE;
    XMVECTOR NoIntersection = XMVectorGreater( XMVectorSplatX( t_min ), XMVectorSplatX( t_max ) );

    // if ( t_max < 0.0f ) return FALSE;
    NoIntersection = XMVectorOrInt( NoIntersection, XMVectorLess( XMVectorSplatX( t_max ), XMVectorZero() ) );

    // if (IsParallel && (-Extents > AxisDotOrigin || Extents < AxisDotOrigin)) return FALSE;
    XMVECTOR ParallelOverlap = XMVectorInBounds( AxisDotOrigin, Extents );
    NoIntersection = XMVectorOrInt( NoIntersection, XMVectorAndCInt( IsParallel, ParallelOverlap ) );

    if( !XMVector3AnyTrue( NoIntersection ) )
    {
        // Store the x-component to *pDist
        XMStoreFloat( pDist, t_min );
        return 1;
    }

    return 0;
}

//-----------------------------------------------------------------------------
// Find the approximate smallest enclosing bounding sphere for a set of 
// points. Exact computation of the smallest enclosing bounding sphere is 
// possible but is slower and requires a more complex algorithm.
// The algorithm is based on  Jack Ritter, "An Efficient Bounding Sphere", 
// Graphics Gems.
//-----------------------------------------------------------------------------
void ComputeBoundingSphereFromPoints( XMFLOAT3* center, float* radius, uint32_t Count, const XMFLOAT3* pPoints, uint32_t Stride )
{
    assert( center );
    assert( Count > 0 );
    assert( pPoints );

    // Find the points with minimum and maximum x, y, and z
    XMVECTOR MinX, MaxX, MinY, MaxY, MinZ, MaxZ;

    MinX = MaxX = MinY = MaxY = MinZ = MaxZ = XMLoadFloat3( pPoints );

    for( uint32_t i = 1; i < Count; i++ )
    {
        XMVECTOR Point = XMLoadFloat3( ( XMFLOAT3* )( ( uint8_t* )pPoints + i * Stride ) );

        float px = XMVectorGetX( Point );
        float py = XMVectorGetY( Point );
        float pz = XMVectorGetZ( Point );

        if( px < XMVectorGetX( MinX ) )
            MinX = Point;

        if( px > XMVectorGetX( MaxX ) )
            MaxX = Point;

        if( py < XMVectorGetY( MinY ) )
            MinY = Point;

        if( py > XMVectorGetY( MaxY ) )
            MaxY = Point;

        if( pz < XMVectorGetZ( MinZ ) )
            MinZ = Point;

        if( pz > XMVectorGetZ( MaxZ ) )
            MaxZ = Point;
    }

    // Use the min/max pair that are farthest apart to form the initial sphere.
    XMVECTOR DeltaX = MaxX - MinX;
    XMVECTOR DistX = XMVector3Length( DeltaX );

    XMVECTOR DeltaY = MaxY - MinY;
    XMVECTOR DistY = XMVector3Length( DeltaY );

    XMVECTOR DeltaZ = MaxZ - MinZ;
    XMVECTOR DistZ = XMVector3Length( DeltaZ );

    XMVECTOR Center;
    XMVECTOR Radius;

    if( XMVector3Greater( DistX, DistY ) )
    {
        if( XMVector3Greater( DistX, DistZ ) )
        {
            // Use min/max x.
            Center = ( MaxX + MinX ) * 0.5f;
            Radius = DistX * 0.5f;
        }
        else
        {
            // Use min/max z.
            Center = ( MaxZ + MinZ ) * 0.5f;
            Radius = DistZ * 0.5f;
        }
    }
    else // Y >= X
    {
        if( XMVector3Greater( DistY, DistZ ) )
        {
            // Use min/max y.
            Center = ( MaxY + MinY ) * 0.5f;
            Radius = DistY * 0.5f;
        }
        else
        {
            // Use min/max z.
            Center = ( MaxZ + MinZ ) * 0.5f;
            Radius = DistZ * 0.5f;
        }
    }

    // Add any points not inside the sphere.
    for( uint32_t i = 0; i < Count; i++ )
    {
        XMVECTOR Point = XMLoadFloat3( ( XMFLOAT3* )( ( uint8_t* )pPoints + i * Stride ) );

        XMVECTOR Delta = Point - Center;

        XMVECTOR Dist = XMVector3Length( Delta );

        if( XMVector3Greater( Dist, Radius ) )
        {
            // Adjust sphere to include the new point.
            Radius = ( Radius + Dist ) * 0.5f;
            Center += ( XMVectorReplicate( 1.0f ) - Radius * XMVectorReciprocal( Dist ) ) * Delta;
        }
    }

    XMStoreFloat3( center, Center );
    XMStoreFloat( radius, Radius );

    return;
}



//-----------------------------------------------------------------------------
// Find the minimum axis aligned bounding box containing a set of points.
//-----------------------------------------------------------------------------
void ComputeBoundingAxisAlignedBoxFromPoints( XMFLOAT3* center, XMFLOAT3* extents, uint32_t Count, const XMFLOAT3* pPoints, uint32_t Stride )
{
    assert( center );
    assert( Count > 0 );
    assert( pPoints );

    // Find the minimum and maximum x, y, and z
    XMVECTOR vMin, vMax;

    vMin = vMax = XMLoadFloat3( pPoints );

    for( uint32_t i = 1; i < Count; i++ )
    {
        XMVECTOR Point = XMLoadFloat3( ( XMFLOAT3* )( ( uint8_t* )pPoints + i * Stride ) );

        vMin = XMVectorMin( vMin, Point );
        vMax = XMVectorMax( vMax, Point );
    }

    // Store center and extents.
    XMStoreFloat3( center, ( vMin + vMax ) * 0.5f );
    XMStoreFloat3( extents, ( vMax - vMin ) * 0.5f );

    return;
}

PyObject* RayToPlaneIntersection( PyObject* module, PyObject* args )
{
	XMVECTOR p0;
	XMVECTOR p1;
	XMVECTOR p2;
	XMVECTOR p3;

	PyObject* pyp0 = NULL;
	PyObject* pyp1 = NULL;
	PyObject* pyp2 = NULL;
	PyObject* pyp3 = NULL;

	if( !PyArg_ParseTuple( args, "OOOO", &pyp0, &pyp1, &pyp2, &pyp3 ) 
		|| !ExtractXMVectorFromSequence( pyp0, p0, 3 )
		|| !ExtractXMVectorFromSequence( pyp1, p1, 3 )
		|| !ExtractXMVectorFromSequence( pyp2, p2, 3 )
		|| !ExtractXMVectorFromSequence( pyp3, p3, 3 )		
		)
	{
		return NULL;
	}

	float denom = XMVectorGetX( XMVector3Dot( p3, p1 ) );
	if( std::abs( denom ) < std::numeric_limits<float>::epsilon() )
	{
		Py_INCREF(Py_None);
		return Py_None;
	}
	else
	{
		float distance = -XMVectorGetX( XMVector3Dot( p2, p3 ) );
		float t = -( XMVectorGetX( XMVector3Dot( p3, p0 ) ) + distance ) / denom;
		return XMVectorAsPyFloats( XMVectorAdd( XMVectorScale( p1, t ), p0 ), 3 );
	}
}

PyObject* IntersectTri( PyObject *module, PyObject *args )
{
	XMVECTOR p0;
	XMVECTOR p1;
	XMVECTOR p2;
	XMVECTOR pRayPos;
	XMVECTOR pRayDir;
	PyObject* pyp0 = NULL;
	PyObject* pyp1 = NULL;
	PyObject* pyp2 = NULL;
	PyObject* pypRayPos = NULL;
	PyObject* pypRayDir = NULL;

	if( !PyArg_ParseTuple( args, "OOOOO", &pyp0, &pyp1, &pyp2, &pypRayPos, &pypRayDir ) 
		|| !ExtractXMVectorFromSequence( pyp0, p0, 3 )
		|| !ExtractXMVectorFromSequence( pyp1, p1, 3 )
		|| !ExtractXMVectorFromSequence( pyp2, p2, 3 )
		|| !ExtractXMVectorFromSequence( pypRayPos, pRayPos, 3 )
		|| !ExtractXMVectorFromSequence( pypRayDir, pRayDir, 3 )
		)
	{
		return NULL;
	}

	float dist;
	if( IntersectRayTriangle( pRayPos, pRayDir, p0, p1, p2, &dist ) )
	{
		return PyFloat_FromDouble( dist );
	}
	else
	{
		Py_INCREF(Py_None);
		return Py_None;
	}
}

PyObject* SphereBoundProbe( PyObject *module, PyObject *args )
{
	XMFLOAT3 pCenter;
	float Radius;
	XMVECTOR pRayPosition;
	XMVECTOR pRayDirection;
	PyObject* pypCenter = NULL;
	PyObject* pypRayPosition = NULL;
	PyObject* pypRayDirection = NULL;

	if( !PyArg_ParseTuple( args, "OfOO", &pypCenter, &Radius, &pypRayPosition, &pypRayDirection ) 
		|| !ExtractFloat3FromSequence( pypCenter, pCenter )
		|| !ExtractXMVectorFromSequence( pypRayPosition, pRayPosition, 3 )
		|| !ExtractXMVectorFromSequence( pypRayDirection, pRayDirection, 3 )
		)
	{
		return NULL;
	}
	float distance;
	if( IntersectRaySphere(  pRayPosition, pRayDirection, &pCenter, &Radius, &distance ) )
	{
		return PyFloat_FromDouble( distance );
	}
	else
	{
		Py_INCREF(Py_None);
		return Py_None;
	}
}

PyObject* BoxBoundProbe( PyObject *module, PyObject *args )
{
	XMFLOAT3 center;
	XMFLOAT3 extents;
	XMVECTOR rayPosition;
	XMVECTOR rayDirection;

	PyObject* pypCenter = NULL;
	PyObject* pypExtends = NULL;
	PyObject* pypRayPosition = NULL;
	PyObject* pypRayDirection = NULL;

	if( !PyArg_ParseTuple( args, "OOOO", &pypCenter, &pypExtends, &pypRayPosition, &pypRayDirection ) 
		|| !ExtractXMVectorFromSequence( pypRayPosition, rayPosition, 3 )
		|| !ExtractXMVectorFromSequence( pypRayDirection, rayDirection, 3 )
		|| !ExtractFloat3FromSequence( pypCenter, center )
		|| !ExtractFloat3FromSequence( pypExtends, extents )
		)
	{
		return NULL;
	}
	float distance;
	if( IntersectRayAxisAlignedBox(  rayPosition, rayDirection, &center, &extents, &distance ) )
	{
		return PyFloat_FromDouble( distance );
	}
	else
	{
		Py_INCREF(Py_None);
		return Py_None;
	}
}

PyObject* ComputeBoundingBox( PyObject *module, PyObject *args )
{
	XMFLOAT3 center;
	XMFLOAT3 extents;
	PyObject* p1 = NULL;

	if( !PyArg_ParseTuple( args, "O", &p1 ) )
	{
		return NULL;
	}

	// Get the size of the tuple
	uint32_t size = (uint32_t)PySequence_Fast_GET_SIZE( p1 );
	XMFLOAT3 *inarray = new XMFLOAT3[size];

	for ( uint32_t i = 0; i < size; i++ )
	{
		PyObject* t = PySequence_GetItem( p1, i );
		bool ok = ExtractFloat3FromSequence( t, inarray[i] );
		Py_XDECREF( t );
		if( !ok )
		{
			delete [] inarray;
			return NULL;
		}
	}

	ComputeBoundingAxisAlignedBoxFromPoints( &center, &extents, (uint32_t)size, inarray, (uint32_t)(sizeof(XMFLOAT3)) );

	delete [] inarray;

	PyObject* ret = PyTuple_New( 2 );

	PyObject* pCenter = XMVectorAsPyFloats( XMLoadFloat3(&center), 3 );
	PyObject* pExtents = XMVectorAsPyFloats( XMLoadFloat3(&extents), 3 );

	PyTuple_SET_ITEM( ret, 0, pCenter );
	PyTuple_SET_ITEM( ret, 1, pExtents );

	return ret;
}

PyObject* ComputeBoundingSphere( PyObject *module, PyObject *args )
{
	XMFLOAT3 center;
	float radius;
	PyObject* p1 = NULL;

	if( !PyArg_ParseTuple( args, "O", &p1 ) )
	{
		return NULL;
	}

	// Get the size of the tuple
	uint32_t size = (uint32_t)PySequence_Fast_GET_SIZE( p1 );
	XMFLOAT3 *inarray = new XMFLOAT3[size];

	for ( uint32_t i = 0; i < size; i++ )
	{
		PyObject* t = PySequence_GetItem( p1, i );
		bool ok = ExtractFloat3FromSequence( t, inarray[i] );
		Py_XDECREF( t );
		if( !ok )
		{
			delete [] inarray;
			return NULL;
		}
	}
	ComputeBoundingSphereFromPoints(&center, &radius, (uint32_t)size, inarray, (uint32_t)(sizeof(XMFLOAT3)) );

	delete [] inarray;

	PyObject* ret = PyTuple_New( 2 );

	PyObject* pCenter = XMVectorAsPyFloats( XMLoadFloat3(&center), 3 );

	PyTuple_SET_ITEM( ret, 0, pCenter );
	PyTuple_SET_ITEM( ret, 1, PyFloat_FromDouble( radius ) );

	return ret;
}

static PyMethodDef native_methods[] = {
	//Vector2
	{"Vec2Length",				(PyCFunction) Vec2Length, METH_VARARGS, 
		"Returns the length of a 2D vector."
		"\n:param v1: a source vector"
		"\n:type v1: (float, float)"
		"\n:returns: The vector's length."
		"\n:rtype: float"
	},
	{"Vec2LengthSq",			(PyCFunction) Vec2LengthSq, METH_VARARGS, 
		"Returns the square of the length of a 2D vector."
		"\n:param v1: a source vector"
		"\n:type v1: (float, float)"
		"\n:returns: The vectors squared length."
		"\n:rtype: float"
	},
	{"Vec2Lerp",				(PyCFunction) Vec2Lerp, METH_VARARGS, 
		"Performs a linear interpolation between two 2D vectors."
		"\n:param v1: a source vector"
		"\n:type v1: (float, float)"
		"\n:param v2: a source vector"
		"\n:type v2: (float, float)"
		"\n:param s: scaling factor that linearly interpolates between the vectors."
		"\n:type s: float"
		"\n:returns: a vector that is the result of the linear interpolation"
		"\n:rtype: (float, float)"
	},
	{"Vec2Maximize",			(PyCFunction) Vec2Maximize, METH_VARARGS, 
		"Returns a 2D vector that is made up of the largest components of two 2D vectors."
		"\n:param v1: a source vector"
		"\n:type v1: (float, float)"
		"\n:param v2: a source vector"
		"\n:type v2: (float, float)"
		"\n:returns: a vector that is made up of the largest components of the two vectors."
		"\n:rtype: (float, float)"
	},
	{"Vec2Minimize",			(PyCFunction) Vec2Minimize, METH_VARARGS, 
		"Returns a 2D vector that is made up of the smallest components of two 2D vectors."
		"\n:param v1: a source vector"
		"\n:type v1: (float, float)"
		"\n:param v2: a source vector"
		"\n:type v2: (float, float)"
		"\n:returns: a vector that is made up of the smallest components of the two vectors."
		"\n:rtype: (float, float)"
	},
	{"Vec2CCW",					(PyCFunction) Vec2CCW, METH_VARARGS, 
		"Returns the z-component by taking the cross product of two 2D vectors."
		"\n:param v1: a source vector"
		"\n:type v1: (float, float)"
		"\n:param v2: a source vector"
		"\n:type v2: (float, float)"
		"\n:returns: vector that is the cross product"
		"\n:rtype: float"
	},
	{"Vec2Dot",					(PyCFunction) Vec2Dot, METH_VARARGS, 
		"Determines the dot product of two 2D vectors."
		"\n:param v1: a source vector"
		"\n:type v1: (float, float)"
		"\n:param v2: a source vector"
		"\n:type v2: (float, float)"
		"\n:returns: The dot product."
		"\n:rtype: float"
	},
	{"Vec2Add",					(PyCFunction) Vec2Add, METH_VARARGS, 
		"Adds two 2D vectors"
		"\n:param v1: a source vector"
		"\n:type v1: (float, float)"
		"\n:param v2: a source vector"
		"\n:type v2: (float, float)"
		"\n:returns: the sum of two vectors"
		"\n:rtype: (float, float)"
	},
	{"Vec2Subtract",			(PyCFunction) Vec2Subtract, METH_VARARGS, 
		"Subtracts two 2D vectors."
		"\n:param v1: a source vector"
		"\n:type v1: (float, float)"
		"\n:param v2: a source vector"
		"\n:type v2: (float, float)"
		"\n:returns: the difference of two vectors"
		"\n:rtype: (float, float)"
	},
	{"Vec2BaryCentric",			(PyCFunction) Vec2BaryCentric, METH_VARARGS,
		"Returns a point in Barycentric coordinates, using the specified 2D vectors."
		"\nThe Vec2BaryCentric function provides a way to understand points in and around a triangle," 
		"\nindependent of where the triangle is actually located. This function returns the resulting point"
		"\nby using the following equation: V1 + f(V2-V1) + g(V3-V1). "
		"\n"
		"\nAny point in the plane V1V2V3 can be represented by the Barycentric coordinate (f,g).The parameter"
		"\nf controls how much V2 gets weighted into the result and the parameter g controls how much V3 gets"
		"\nweighted into the result. Lastly, 1-f-g controls how much V1 gets weighted into the result."
		"\n"
		"\nNote the following relations."
		"\n"
		"\nIf (f>=0 &, & g>=0 &, & 1-f-g>=0), the point is inside the triangle V1V2V3."
		"\nIf (f==0 &, & g>=0 &, & 1-f-g>=0), the point is on the line V1V3."
		"\nIf (f>=0 &, & g==0 &, & 1-f-g>=0), the point is on the line V1V2."
		"\nIf (f>=0 &, & g>=0 &, & 1-f-g==0), the point is on the line V2V3."
		"\nBarycentric coordinates are a form of general coordinates. In this context, using Barycentric"
		"\ncoordinates represents a change in coordinate systems. What holds true for Cartesian coordinates"
		"\nholds true for Barycentric coordinates."
		"\n:param v1: a position vector"
		"\n:type v1: (float, float)"
		"\n:param v2: a position vector"
		"\n:type v2: (float, float)"
		"\n:param v3: a position vector"
		"\n:type v3: (float, float)"
		"\n:param f: Weighting factor"
		"\n:type f: float"
		"\n:param g: Weighting factor"
		"\n:type g: float"
		"\n:returns: vector in Barycentric coordinates."
		"\n:rtype: (float, float)"
	},
	{"Vec2CatmullRom",			(PyCFunction) Vec2CatmullRom, METH_VARARGS, 
		"Performs a Catmull-Rom interpolation, using the specified 2D vectors."
		"\n:param pos1: a position vector"
		"\n:type pos1: (float, float)"
		"\n:param tan1: a position vector"
		"\n:type tan1: (float, float)"
		"\n:param pos2: a position vector"
		"\n:type pos2: (float, float)"
		"\n:param tan2: a position vector"
		"\n:type tan2: (float, float)"
		"\n:param t: the interpolant"
		"\n:type t: float"
		"\n:returns: vector that is the result of the Catmull-Rom interpolation"
		"\n:rtype: (float, float)"
	},
	{"Vec2Hermite",				(PyCFunction) Vec2Hermite, METH_VARARGS, 
		"Performs a Hermite spline interpolation, using the specified 2D vectors."
		"\n:param pos1: a position vector"
		"\n:type pos1: (float, float)"
		"\n:param tan1: a position vector"
		"\n:type tan1: (float, float)"
		"\n:param pos2: a position vector"
		"\n:type pos2: (float, float)"
		"\n:param tan2: a position vector"
		"\n:type tan2: (float, float)"
		"\n:param t: the interpolant"
		"\n:type t: float"
		"\n:returns: vector that is the result of the Hermite spline interpolation"
		"\n:rtype: (float, float)"
	},
	{"Vec2Normalize",			(PyCFunction) Vec2Normalize, METH_VARARGS, 
		"Returns the normalized version of a 2D vector"
		"\n:param vec: the source vector"
		"\n:type vec: (float, float)"
		"\n:returns: a normalized vector"
		"\n:rtype: (float, float)"
	},
	{"Vec2Negate",			(PyCFunction) Vec2Negate, METH_VARARGS, 
		"Returns the negated version of a 2D vector"
		"\n:param vec: the source vector"
		"\n:type vec: (float, float)"
		"\n:returns: a negated vector"
		"\n:rtype: (float, float)"
	},
	{"Vec2Scale",				(PyCFunction) Vec2Scale, METH_VARARGS, 
		"Scales a 2D vector."
		"\n:param v: a source vector"
		"\n:type v: (float, float)"
		"\n:param scale: scaling factor"
		"\n:type scale: float"
		"\n:returns: the scaled vector."
		"\n:rtype: (float, float)"
	},
	{"Vec2Transform",			(PyCFunction) Vec2Transform, METH_VARARGS, 
		"Transforms a 2D vector by a given matrix."
		"\n:param vec: the source vector"
		"\n:type vec: (float, float)"
		"\n:param mat: the source matrix"
		"\n:type mat: tuple[tuple[float]]"
		"\n:returns: vector transformed by the source matrix"
		"\n:rtype: (float, float)"
	},
	{"Vec2TransformArray",		(PyCFunction) Vec2TransformArray, METH_VARARGS, 
		"Transforms an array of 2D vectors by a given matrix."
		"\n:param array: a sequence of points"
		"\n:type array: list[(float, float)]"
		"\n:param mat: the source matrix"
		"\n:type mat: tuple[tuple[float]]"
		"\n:returns: a sequence of points transformed by the source matrix"
		"\n:rtype: tuple[(float, float)]"
	},
	{"Vec2TransformCoord",		(PyCFunction) Vec2TransformCoord, METH_VARARGS, 
		"Transforms a 2D vector by a given matrix, projecting the result back into w = 1."
		"\n:param vec: the source vector"
		"\n:type vec: (float, float)"
		"\n:param mat: the source matrix"
		"\n:type mat: tuple[tuple[float]]"
		"\n:returns: vector transformed by the source matrix"
		"\n:rtype: (float, float)"
	},
	{"Vec2TransformCoordArray",	(PyCFunction) Vec2TransformCoordArray, METH_VARARGS, 
		"Transforms an array of 2D vectors by a given matrix, and projects the result back into w = 1."
		"\n:param array: a sequence of points"
		"\n:type array: list[(float, float)]"
		"\n:param mat: the source matrix"
		"\n:type mat: tuple[tuple[float]]"
		"\n:returns: a sequence of points transformed by the source matrix"
		"\n:rtype: tuple[(float, float)]"
	},
	{"Vec2TransformNormal",		(PyCFunction) Vec2TransformNormal, METH_VARARGS, 
		"Transforms the 2D vector normal by the given matrix"
		"\n:param vec: the source vector"
		"\n:type vec: (float, float)"
		"\n:param mat: the source matrix"
		"\n:type mat: tuple[tuple[float]]"
		"\n:returns: vector transformed by the source matrix"
		"\n:rtype: (float, float)"
	},
	{"Vec2TransformNormalArray",(PyCFunction) Vec2TransformNormalArray, METH_VARARGS, 
		"Transforms an array of 2D vectors by a given matrix."
		"\n:param array: a sequence of points"
		"\n:type array: list[(float, float)]"
		"\n:param mat: the source matrix"
		"\n:type mat: tuple[tuple[float]]"
		"\n:returns: a sequence of points transformed by the source matrix"
		"\n:rtype: tuple[(float, float)]"
	},
	
	// Vector3
	{"Vec3Equal",	(PyCFunction) Vec3Equal, METH_VARARGS, 
		"Compares the values of two vectors"
		"\n:param v1: a source vector"
		"\n:type v1: (float, float, float)"
		"\n:param v2: a source vector"
		"\n:type v2: (float, float, float)"
		"\n:rtype: bool"
	},
	{"Vec3EqualD",	(PyCFunction) Vec3EqualD, METH_VARARGS, 
		"Compares the values of two vectors with double precision"
		"\n:param v1: a source vector"
		"\n:type v1: (float, float, float)"
		"\n:param v2: a source vector"
		"\n:type v2: (float, float, float)"
		"\n:rtype: bool"
	},
	{"Vec3NearEqual",	(PyCFunction) Vec3NearEqual, METH_VARARGS, 
		"Compares the values of two vectors"
		"\n:param v1: a source vector"
		"\n:type v1: (float, float, float)"
		"\n:param v2: a source vector"
		"\n:type v2: (float, float, float)"
		"\n:param epsilon: optional precision, defaults to FLT_EPSILON"
		"\n:type epsilon: Optional[float]"
		"\n:rtype: bool"
	},
	{"Vec3NearEqualD",	(PyCFunction) Vec3NearEqualD, METH_VARARGS, 
		"Compares the values of two vectors with double precision"
		"\n:param v1: a source vector"
		"\n:type v1: (float, float, float)"
		"\n:param v2: a source vector"
		"\n:type v2: (float, float, float)"
		"\n:param epsilon: optional precision, defaults to FLT_EPSILON"
		"\n:type epsilon: Optional[float]"
		"\n:rtype: bool"
	},
	{"Vec3Add",				(PyCFunction) Vec3Add, METH_VARARGS, 
		"Adds two 3D vectors."
		"\n:param v1: a source vector"
		"\n:type v1: (float, float, float)"
		"\n:param v2: a source vector"
		"\n:type v2: (float, float, float)"
		"\n:returns: the sum of two vectors"
		"\n:rtype: (float, float, float)"
	},
	{"Vec3AddD",				(PyCFunction) Vec3AddD, METH_VARARGS, 
		"Adds two 3D vectors with double precision."
		"\n:param v1: a source vector"
		"\n:type v1: (float, float, float)"
		"\n:param v2: a source vector"
		"\n:type v2: (float, float, float)"
		"\n:returns: the sum of two vectors"
		"\n:rtype: (float, float, float)"
	},
	{"Vec3Subtract",		(PyCFunction) Vec3Subtract, METH_VARARGS, 
		"Subtracts two 3D vectors."
		"\n:param v1: a source vector"
		"\n:type v1: (float, float, float)"
		"\n:param v2: a source vector"
		"\n:type v2: (float, float, float)"
		"\n:returns: the difference of two vectors"
		"\n:rtype: (float, float, float)"
	},
	{"Vec3SubtractD",		(PyCFunction) Vec3SubtractD, METH_VARARGS, 
		"Subtracts two 3D vectors with double precision."
		"\n:param v1: a source vector"
		"\n:type v1: (float, float, float)"
		"\n:param v2: a source vector"
		"\n:type v2: (float, float, float)"
		"\n:returns: the difference of two vectors"
		"\n:rtype: (float, float, float)"
	},
	{"Vec3Cross",			(PyCFunction) Vec3Cross, METH_VARARGS, 
		"Determines the cross-product of two 3D vectors."
		"\n:param v1: a source vector"
		"\n:type v1: (float, float, float)"
		"\n:param v2: a source vector"
		"\n:type v2: (float, float, float)"
		"\n:returns: vector that is the cross product"
		"\n:rtype: (float, float, float)"
	},
	{"Vec3CrossD",			(PyCFunction) Vec3CrossD, METH_VARARGS, 
		"Determines the cross-product of two 3D vectors with double precision."
		"\n:param v1: a source vector"
		"\n:type v1: (float, float, float)"
		"\n:param v2: a source vector"
		"\n:type v2: (float, float, float)"
		"\n:returns: vector that is the cross product"
		"\n:rtype: (float, float, float)"
	},
	{"Vec3Dot",				(PyCFunction) Vec3Dot, METH_VARARGS, 
		"Determines the dot product of two 3D vectors."
		"\n:param v1: a source vector"
		"\n:type v1: (float, float, float)"
		"\n:param v2: a source vector"
		"\n:type v2: (float, float, float)"
		"\n:returns: The dot product."
		"\n:rtype: float"
	},
	{"Vec3DotD",				(PyCFunction) Vec3DotD, METH_VARARGS, 
		"Determines the dot product of two 3D vectors with double precision."
		"\n:param v1: a source vector"
		"\n:type v1: (float, float, float)"
		"\n:param v2: a source vector"
		"\n:type v2: (float, float, float)"
		"\n:returns: The dot product."
		"\n:rtype: float"
	},
	{"Vec3Length",			(PyCFunction) Vec3Length, METH_VARARGS, 
		"Returns the length of a 3D vector."
		"\n:param v1: a source vector"
		"\n:type v1: (float, float, float)"
		"\n:returns: The vector's length."
		"\n:rtype: float"
	},
	{"Vec3LengthD",			(PyCFunction) Vec3LengthD, METH_VARARGS, 
		"Returns the length of a 3D vector with double precision."
		"\n:param v1: a source vector"
		"\n:type v1: (float, float, float)"
		"\n:returns: The vector's length."
		"\n:rtype: float"
	},
	{"Vec3LengthSq",		(PyCFunction) Vec3LengthSq, METH_VARARGS, 
		"Returns the square of the length of a 3D vector."
		"\n:param v1: a source vector"
		"\n:type v1: (float, float, float)"
		"\n:returns: The vector's squared length."
		"\n:rtype: float"
	},
	{"Vec3LengthSqD",		(PyCFunction) Vec3LengthSqD, METH_VARARGS, 
		"Returns the square of the length of a 3D vector with double precision."
		"\n:param v1: a source vector"
		"\n:type v1: (float, float, float)"
		"\n:returns: The vector's squared length."
		"\n:rtype: float"
	},
	{"Vec3Distance",		(PyCFunction) Vec3Distance, METH_VARARGS, 
		"Returns the distance between two 3D vector."
		"\n:param v1: a source vector"
		"\n:type v1: (float, float, float)"
		"\n:param v2: a source vector"
		"\n:type v2: (float, float, float)"
		"\n:returns: The distance between the two vectors."
		"\n:rtype: float"
	},
	{"Vec3DistanceSq",		(PyCFunction) Vec3DistanceSq, METH_VARARGS, 
		"Returns the square of the distance between two 3D vector."
		"\n:param v1: a source vector"
		"\n:type v1: (float, float, float)"
		"\n:param v2: a source vector"
		"\n:type v2: (float, float, float)"
		"\n:returns: The squared distance between the two vectors."
		"\n:rtype: float"
	},
	{"Vec3DistanceD",		(PyCFunction) Vec3DistanceD, METH_VARARGS, 
		"Returns the distance between two 3D vector with double precision."
		"\n:param v1: a source vector"
		"\n:type v1: (float, float, float)"
		"\n:param v2: a source vector"
		"\n:type v2: (float, float, float)"
		"\n:returns: The distance between the two vectors."
		"\n:rtype: float"
	},
	{"Vec3DistanceSqD",		(PyCFunction) Vec3DistanceSqD, METH_VARARGS, 
		"Returns the square of the distance between two 3D vector with double precision."
		"\n:param v1: a source vector"
		"\n:type v1: (float, float, float)"
		"\n:param v2: a source vector"
		"\n:type v2: (float, float, float)"
		"\n:returns: The squared distance between the two vectors."
		"\n:rtype: float"
	},
	{"Vec3Lerp",			(PyCFunction) Vec3Lerp, METH_VARARGS, 
		"Performs a linear interpolation between two 3D vectors."
		"\n:param v1: a source vector"
		"\n:type v1: (float, float, float)"
		"\n:param v2: a source vector"
		"\n:type v2: (float, float, float)"
		"\n:param s: scaling factor that linearly interpolates between the vectors."
		"\n:type s: float"
		"\n:returns: a vector that is the result of the linear interpolation"
		"\n:rtype: (float, float, float)"
	},
	{"Vec3LerpD",			(PyCFunction) Vec3LerpD, METH_VARARGS, 
		"Performs a linear interpolation between two double precision 3D vectors."
		"\n:param v1: a source vector"
		"\n:type v1: (float, float, float)"
		"\n:param v2: a source vector"
		"\n:type v2: (float, float, float)"
		"\n:param s: scaling factor that linearly interpolates between the vectors."
		"\n:type s: float"
		"\n:returns: a vector that is the result of the linear interpolation"
		"\n:rtype: (float, float, float)"
	},
	{"Vec3Maximize",		(PyCFunction) Vec3Maximize, METH_VARARGS, 
		"Returns a 3D vector that is made up of the largest components of two 3D vectors."
		"\n:param v1: a source vector"
		"\n:type v1: (float, float, float)"
		"\n:param v2: a source vector"
		"\n:type v2: (float, float, float)"
		"\n:returns: a vector that is made up of the largest components of the two vectors."
		"\n:rtype: (float, float, float)"
	},
	{"Vec3Minimize",		(PyCFunction) Vec3Minimize, METH_VARARGS, 
		"Returns a 3D vector that is made up of the smallest components of two 3D vectors"
		"\n:param v1: a source vector"
		"\n:type v1: (float, float, float)"
		"\n:param v2: a source vector"
		"\n:type v2: (float, float, float)"
		"\n:returns: a vector that is made up of the smallest components of the two vectors."
		"\n:rtype: (float, float, float)"
	},
	{"Vec3Normalize",		(PyCFunction) Vec3Normalize, METH_VARARGS, 
		"Returns the normalization of the input source vector.  In the case of a 0-length vector, "
		"it returns (0,0,0) like old Geo did."
		"\n:param v: source vector to normalize"
		"\n:type v: (float, float, float)"
		"\n:returns: the normalization of the input vector, or (0,0,0) for a 0-length input vector"
		"\n:rtype: (float, float, float)"
	},
	{"Vec3Negate",			(PyCFunction) Vec3Negate, METH_VARARGS, 
		"Returns the negated version of a 3D vector"
		"\n:param vec: the source vector"
		"\n:type vec: (float, float, float)"
		"\n:returns: a negated vector"
		"\n:rtype: (float, float, float)"
	},
	{"Vec3NegateD",			(PyCFunction) Vec3NegateD, METH_VARARGS, 
		"Returns the negated version of a 3D vector with double precision"
		"\n:param vec: the source vector"
		"\n:type vec: (float, float, float)"
		"\n:returns: a negated vector"
		"\n:rtype: (float, float, float)"
	},
	{"Vec3NormalizeD",		(PyCFunction) Vec3NormalizeD, METH_VARARGS, 
		"Returns the normalization of the input source vector with double precision.  In the case of a 0-length vector, "
		"it returns (0,0,0) like old Geo did."
		"\n:param v: source vector to normalize"
		"\n:type v: (float, float, float)"
		"\n:returns: the normalization of the input vector, or (0,0,0) for a 0-length input vector"
		"\n:rtype: (float, float, float)"
	},
	{"Vec3ScaleD",			(PyCFunction) Vec3ScaleD, METH_VARARGS, 
		"Scales a 3D vector with double precision."
		"\n:param v: a source vector"
		"\n:type v: (float, float, float)"
		"\n:param scale: scaling factor"
		"\n:type scale: float"
		"\n:returns: the scaled vector."
		"\n:rtype: (float, float, float)"
	},
	{"Vec3Scale",			(PyCFunction) Vec3Scale, METH_VARARGS, 
		"Scales a 3D vector."
		"\n:param v: a source vector"
		"\n:type v: (float, float, float)"
		"\n:param scale: scaling factor"
		"\n:type scale: float"
		"\n:returns: the scaled vector."
		"\n:rtype: (float, float, float)"
	},
	{"Vec3DirectionD",	(PyCFunction) Vec3DirectionD, METH_VARARGS, 
		"Normalize the difference between two vectors with double precision."
		"\n:param v1: a source vector"
		"\n:type v1: (float, float, float)"
		"\n:param v2: a source vector"
		"\n:type v2: (float, float, float)"
		"\n:returns: the normalized difference of two vectors"
		"\n:rtype: (float, float, float)"
	},
	{"Vec3Direction",	(PyCFunction) VecDirection, METH_VARARGS, 
		"Normalize the difference between two vectors."
		"\n:param v1: a source vector"
		"\n:type v1: (float, float, float)"
		"\n:param v2: a source vector"
		"\n:type v2: (float, float, float)"
		"\n:returns: the normalized difference of two vectors"
		"\n:rtype: (float, float, float)"
	},
	{"Vec3BaryCentric",		(PyCFunction) Vec3BaryCentric, METH_VARARGS, 
		"Returns a point in Barycentric coordinates, using the specified 3D vectors."
		"\nThe Vec3BaryCentric function provides a way to understand points in and around a triangle," 
		"\nindependent of where the triangle is actually located. This function returns the resulting point"
		"\nby using the following equation: V1 + f(V2-V1) + g(V3-V1). "
		"\n"
		"\nAny point in the plane V1V2V3 can be represented by the Barycentric coordinate (f,g).The parameter"
		"\nf controls how much V2 gets weighted into the result and the parameter g controls how much V3 gets"
		"\nweighted into the result. Lastly, 1-f-g controls how much V1 gets weighted into the result."
		"\n"
		"\nNote the following relations."
		"\n"
		"\nIf (f>=0 &, & g>=0 &, & 1-f-g>=0), the point is inside the triangle V1V2V3."
		"\nIf (f==0 &, & g>=0 &, & 1-f-g>=0), the point is on the line V1V3."
		"\nIf (f>=0 &, & g==0 &, & 1-f-g>=0), the point is on the line V1V2."
		"\nIf (f>=0 &, & g>=0 &, & 1-f-g==0), the point is on the line V2V3."
		"\nBarycentric coordinates are a form of general coordinates. In this context, using Barycentric"
		"\ncoordinates represents a change in coordinate systems. What holds true for Cartesian coordinates"
		"\nholds true for Barycentric coordinates."
		"\n:param v1: a position vector"
		"\n:type v1: (float, float, float)"
		"\n:param v2: a position vector"
		"\n:type v2: (float, float, float)"
		"\n:param v3: a position vector"
		"\n:type v3: (float, float, float)"
		"\n:param f: Weighting factor"
		"\n:type f: float"
		"\n:param g: Weighting factor"
		"\n:type g: float"
		"\n:returns: vector in Barycentric coordinates."
		"\n:rtype: (float, float, float)"
	},
	{"Vec3Hermite",			(PyCFunction) Vec3Hermite, METH_VARARGS, 
		"Performs a Hermite spline interpolation, using the specified 3D vectors."
		"\n:param pos1: a position vector"
		"\n:type pos1: (float, float, float)"
		"\n:param tan1: a tangent vector"
		"\n:type tan1: (float, float, float)"
		"\n:param pos2: a position vector"
		"\n:type pos2: (float, float, float)"
		"\n:param tan2: a tangent vector"
		"\n:type tan2: (float, float, float)"
		"\n:param t: the interpolant"
		"\n:type t: float"
		"\n:returns: vector that is the result of the Hermite spline interpolation"
		"\n:rtype: (float, float, float)"
	},
	{"Vec3CatmullRom",		(PyCFunction) Vec3CatmullRom, METH_VARARGS, 
		"Performs a Catmull-Rom interpolation, using the specified 3D vectors."
		"\n:param pos1: a position vector"
		"\n:type pos1: (float, float, float)"
		"\n:param tan1: a tangent vector"
		"\n:type tan1: (float, float, float)"
		"\n:param pos2: a position vector"
		"\n:type pos2: (float, float, float)"
		"\n:param tan2: a tangent vector"
		"\n:type tan2: (float, float, float)"
		"\n:param t: the interpolant"
		"\n:type t: float"
		"\n:returns: vector that is the result of the Catmull-Rom interpolation"
		"\n:rtype: (float, float, float)"
	},
	{"Vec3Project",			(PyCFunction) Vec3Project, METH_VARARGS, 
		"Projects a 3D vector from object space into screen space."
		"\n:param v1: source vector"
		"\n:type v1: (float, float, float)"
		"\n:param viewport: the viewport(topLeftX, topLeftY, width, height, minDepth, maxDepth)"
		"\n:type viewport: (int, int, int, int, float, float)"
		"\n:param proj: the projection matrix"
		"\n:type proj: tuple[tuple[float]]"
		"\n:param view: the view matrix"
		"\n:type view: tuple[tuple[float]]"
		"\n:param world: the world matrix"
		"\n:type world: tuple[tuple[float]]"
		"\n:returns: vector projected from object space to screen space"
		"\n:rtype: (float, float, float, float)"
	},
	{"Vec3ProjectArray",	(PyCFunction) Vec3ProjectArray, METH_VARARGS, 
		"Projects an array of 3D vectors from object space into screen space."
		"\n:param array: a sequence of points"
		"\n:type array: list[(float, float, float)]"
		"\n:param viewport: the viewport(topLeftX, topLeftY, width, height, minDepth, maxDepth)"
		"\n:type viewport: (int, int, int, int, float, float)"
		"\n:param proj: the projection matrix"
		"\n:type proj: tuple[tuple[float]]"
		"\n:param view: the view matrix"
		"\n:type view: tuple[tuple[float]]"
		"\n:param world: the world matrix"
		"\n:type world: tuple[tuple[float]]"
		"\n:returns: a sequence of vectors projected from object space to screen space"
		"\n:rtype: tuple[(float, float, float, float)]"
	},
	{"Vec3Transform",		(PyCFunction) Vec3Transform, METH_VARARGS, 
		"Transforms vector (x, y, z, 1) by a given matrix."
		"\n:param vec: source vector"
		"\n:type vec: (float, float, float)"
		"\n:param mat: the source matrix"
		"\n:type mat: tuple[tuple[float]]"
		"\n:returns: vector transformed by the source matrix"
		"\n:rtype: (float, float, float)"
	},
	{"Vec3TransformArray",  (PyCFunction) Vec3TransformArray, METH_VARARGS, 
		"Transforms an array (x, y, z, 1) by a given matrix."
		"\n:param array: a sequence of points"
		"\n:type array: list[(float, float, float)]"
		"\n:param mat: the source matrix"
		"\n:type mat: tuple[tuple[float]]"
		"\n:returns: a sequence of points transformed by the source matrix"
		"\n:rtype: tuple[(float, float, float, float)]"
	},
	{"Vec3TransformCoord",  (PyCFunction) Vec3TransformCoord, METH_VARARGS, 
		"Transforms a 3D vector by a given matrix, projecting the result back into w = 1."
		"\n:param vec: source vector"
		"\n:type vec: (float, float, float)"
		"\n:param mat: the source matrix"
		"\n:type mat: tuple[tuple[float]]"
		"\n:returns: the transformed vector"
		"\n:rtype: (float, float, float)"
	},
	{"Vec3TransformCoordArray", (PyCFunction) Vec3TransformCoordArray, METH_VARARGS, 
		"Transforms an array (x, y, z, 1) by a given matrix, and projects the result back into w = 1"
		"\n:param array: a sequence of points"
		"\n:type array: list[(float, float, float)]"
		"\n:param mat: the source matrix"
		"\n:type mat: tuple[tuple[float]]"
		"\n:returns: sequence of transformed vectors"
		"\n:rtype: tuple[(float, float, float)]"
	},
	{"Vec3TransformNormal",		(PyCFunction) Vec3TransformNormal, METH_VARARGS, 
		"Transforms the 3D vector normal by the given matrix."
		"\n:param vec: source vector"
		"\n:type vec: (float, float, float)"
		"\n:param mat: the source matrix"
		"\n:type mat: tuple[float]"
		"\n:returns: the transformed vector"
		"\n:rtype: (float, float, float)"
	},
	{"Vec3TransformNormalArray",(PyCFunction) Vec3TransformNormalArray, METH_VARARGS, 
		"Transforms an array (x, y, z, 0) by a given matrix."
		"\n:param array: a sequence of points"
		"\n:type array: list[(float, float, float)]"
		"\n:param mat: the source matrix"
		"\n:type mat: tuple[tuple[float]]"
		"\n:returns: sequence of transformed vectors"
		"\n:rtype: tuple[(float, float, float)]"
	},
	{"Vec3Unproject",		(PyCFunction) Vec3Unproject, METH_VARARGS, 
		"Projects a vector from screen space into object space."
		"\n:param v1: source vector"
		"\n:type v1: (float, float, float)"
		"\n:param viewport: the viewport(topLeftX, topLeftY, width, height, minDepth, maxDepth)"
		"\n:type viewport: (int, int, int, int, float, float)"
		"\n:param proj: the projection matrix"
		"\n:type proj: tuple[tuple[float]]"
		"\n:param view: the view matrix"
		"\n:type view: tuple[tuple[float]]"
		"\n:param world: the world matrix"
		"\n:type world: tuple[tuple[float]]"
		"\n:returns: vector projected from screen space to object space"
		"\n:rtype: (float, float, float)"
	},
	{"Vec3UnprojectArray",	(PyCFunction) Vec3UnprojectArray, METH_VARARGS, 
		"Projects an array (x, y, z, 0) from screen space into object space."
		"\n:param array: a sequence of points"
		"\n:type array: list[(float, float, float)]"
		"\n:param viewport: the viewport(topLeftX, topLeftY, width, height, minDepth, maxDepth)"
		"\n:type viewport: (int, int, int, int, float, float)"
		"\n:param proj: the projection matrix"
		"\n:type proj: tuple[tuple[float]]"
		"\n:param view: the view matrix"
		"\n:type view: tuple[tuple[float]]"
		"\n:param world: the world matrix"
		"\n:type world: tuple[tuple[float]]"
		"\n:returns: a sequence of vectors projected from screen space to object space"
		"\n:rtype: tuple[(float, float, float)]"
	},
	{"Vec3DistanceSqFromBoundingBox",	(PyCFunction) Vec3DistanceSqFromBoundingBox, METH_VARARGS, 
		"Calculates the distance squared from a point to a bounding box."
		"\n:param point: point to check"
		"\n:type point: (float, float, float)"
		"\n:param boundingBoxMin: bounding box minimum point"
		"\n:type boundingBoxMin: (float, float, float)"
		"\n:param boundingBoxMax: bounding box maximum point"
		"\n:type boundingBoxMax: (float, float, float)"
		"\n:returns: distance squared from the point to the bounding box. zero if inside."
		"\n:rtype: float"
	},

	// Vector4
	{"Vec4Add",				(PyCFunction) Vec4Add, METH_VARARGS, 
		"Adds two 4D vectors."
		"\n:param v1: a source vector"
		"\n:type v1: (float, float, float, float)"
		"\n:param v2: a source vector"
		"\n:type v2: (float, float, float, float)"
		"\n:returns: the sum of two vectors"
		"\n:rtype: (float, float, float, float)"
	},
	{"Vec4Dot",				(PyCFunction) Vec4Dot, METH_VARARGS, 
		"Determines the dot product of two 4D vectors."
		"\n:param v1: a source vector"
		"\n:type v1: (float, float, float, float)"
		"\n:param v2: a source vector"
		"\n:type v2: (float, float, float, float)"
		"\n:returns: The dot product."
		"\n:rtype: float"
	},
	{"Vec4Length",			(PyCFunction) Vec4Length, METH_VARARGS, 
		"Returns the length of a 4D vector."
		"\n:param v1: a source vector"
		"\n:type v1: (float, float, float, float)"
		"\n:returns: The vector's length."
		"\n:rtype: float"
	},
	{"Vec4LengthSq",		(PyCFunction) Vec4LengthSq, METH_VARARGS, 
		"Returns the square of the length of a 4D vector."
		"\n:param v1: a source vector"
		"\n:type v1: (float, float, float, float)"
		"\n:returns: The vector's squared length."
		"\n:rtype: float"
	},
	{"Vec4Lerp",			(PyCFunction) Vec4Lerp, METH_VARARGS, 
		"Performs a linear interpolation between two 4D vectors."
		"\n:param v1: a source vector"
		"\n:type v1: (float, float, float, float)"
		"\n:param v2: a source vector"
		"\n:type v2: (float, float, float, float)"
		"\n:param s: scaling factor that linearly interpolates between the vectors."
		"\n:type s: float"
		"\n:returns: a vector that is the result of the linear interpolation"
		"\n:rtype: (float, float, float, float)"
	},
	{"Vec4Maximize",		(PyCFunction) Vec4Maximize, METH_VARARGS, 
		"Returns a 4D vector that is made up of the largest components of two 4D vectors."
		"\n:param v1: a source vector"
		"\n:type v1: (float, float, float, float)"
		"\n:param v2: a source vector"
		"\n:type v2: (float, float, float, float)"
		"\n:returns: a vector that is made up of the largest components of the two vectors."
		"\n:rtype: (float, float, float, float)"
	},
	{"Vec4Minimize",		(PyCFunction) Vec4Minimize, METH_VARARGS, 
		"Returns a 4D vector that is made up of the smallest components of two 4D vectors."
		"\n:param v1: a source vector"
		"\n:type v1: (float, float, float, float)"
		"\n:param v2: a source vector"
		"\n:type v2: (float, float, float, float)"
		"\n:returns: a vector that is made up of the smallest components of the two vectors."
		"\n:rtype: (float, float, float, float)"
	},
	{"Vec4Scale",			(PyCFunction) Vec4Scale, METH_VARARGS, 
		"Scales a 4D vector."
		"\n:param v: a source vector"
		"\n:type v: (float, float, float, float)"
		"\n:param scale: scaling factor"
		"\n:type scale: float"
		"\n:returns: the scaled vector."
		"\n:rtype: (float, float, float, float)"
	},
	{"Vec4Subtract",		(PyCFunction) Vec4Subtract, METH_VARARGS, 
		"Subtracts two 4D vectors."
		"\n:param v1: a source vector"
		"\n:type v1: (float, float, float, float)"
		"\n:param v2: a source vector"
		"\n:type v2: (float, float, float, float)"
		"\n:returns: the difference of two vectors"
		"\n:rtype: (float, float, float, float)"
	},
	{"Vec4Cross",			(PyCFunction) Vec4Cross, METH_VARARGS, 
		"Determines the cross-product in four dimensions."
		"\n:param v1: a source vector"
		"\n:type v1: (float, float, float, float)"
		"\n:param v2: a source vector"
		"\n:type v2: (float, float, float, float)"
		"\n:param v3: a source vector"
		"\n:type v3: (float, float, float, float)"
		"\n:returns: vector that is the cross product"
		"\n:rtype: (float, float, float, float)"
	},
	{"Vec4BaryCentric",		(PyCFunction) Vec4BaryCentric, METH_VARARGS, 
		"Returns a point in Barycentric coordinates, using the specified 4D vectors."
		"\nThe Vec4BaryCentric function provides a way to understand points in and around a triangle," 
		"\nindependent of where the triangle is actually located. This function returns the resulting point"
		"\nby using the following equation: V1 + f(V2-V1) + g(V3-V1). "
		"\n"
		"\nAny point in the plane V1V2V3 can be represented by the Barycentric coordinate (f,g).The parameter"
		"\nf controls how much V2 gets weighted into the result and the parameter g controls how much V3 gets"
		"\nweighted into the result. Lastly, 1-f-g controls how much V1 gets weighted into the result."
		"\n"
		"\nNote the following relations."
		"\n"
		"\nIf (f>=0 &, & g>=0 &, & 1-f-g>=0), the point is inside the triangle V1V2V3."
		"\nIf (f==0 &, & g>=0 &, & 1-f-g>=0), the point is on the line V1V3."
		"\nIf (f>=0 &, & g==0 &, & 1-f-g>=0), the point is on the line V1V2."
		"\nIf (f>=0 &, & g>=0 &, & 1-f-g==0), the point is on the line V2V3."
		"\nBarycentric coordinates are a form of general coordinates. In this context, using Barycentric"
		"\ncoordinates represents a change in coordinate systems. What holds true for Cartesian coordinates"
		"\nholds true for Barycentric coordinates."
		"\n:param v1: a position vector"
		"\n:type v1: (float, float, float, float)"
		"\n:param v2: a position vector"
		"\n:type v2: (float, float, float, float)"
		"\n:param v3: a position vector"
		"\n:type v3: (float, float, float, float)"
		"\n:param f: Weighting factor"
		"\n:type f: float"
		"\n:param g: Weighting factor"
		"\n:type g: float"
		"\n:returns: vector in Barycentric coordinates."
		"\n:rtype: (float, float, float, float)"
	},
	{"Vec4CatmullRom",		(PyCFunction) Vec4CatmullRom, METH_VARARGS, 
		"Performs a Catmull-Rom interpolation, using the specified 4D vectors."
		"\n"
		"\nArguments:"
		"\n:param pos1: a position vector"
		"\n:type pos1: (float, float, float, float)"
		"\n:param tan1: a position vector"
		"\n:type tan1: (float, float, float, float)"
		"\n:param pos2: a position vector"
		"\n:type pos2: (float, float, float, float)"
		"\n:param tan2: a position vector"
		"\n:type tan2: (float, float, float, float)"
		"\n:param t: the interpolant"
		"\n:type t: float"
		"\n:returns: vector that is the result of the Catmull-Rom interpolation"
		"\n:rtype: (float, float, float, float)"
	},
	{"Vec4Hermite",			(PyCFunction) Vec4Hermite, METH_VARARGS, 
		"Performs a Hermite spline interpolation, using the specified 4D vectors."
		"\n:param pos1: a position vector"
		"\n:type pos1: (float, float, float, float)"
		"\n:param tan1: a position vector"
		"\n:type tan1: (float, float, float, float)"
		"\n:param pos2: a position vector"
		"\n:type pos2: (float, float, float, float)"
		"\n:param tan2: a position vector"
		"\n:type tan2: (float, float, float, float)"
		"\n:param t: the interpolant"
		"\n:type t: float"
		"\n:returns: vector that is the result of the Hermite spline interpolation"
		"\n:rtype: (float, float, float, float)"
	},
	{"Vec4Normalize",		(PyCFunction) Vec4Normalize, METH_VARARGS, 
		"Returns the normalized version of a 4D vector."
		"\n:param vec: the source vector"
		"\n:type vec: (float, float, float, float)"
		"\n:returns: a normalized vector"
		"\n:rtype: (float, float, float, float)"
	},
	{"Vec4Negate",			(PyCFunction) Vec4Negate, METH_VARARGS, 
		"Returns the negated version of a 4D vector"
		"\n:param vec: the source vector"
		"\n:type vec: (float, float, float, float)"
		"\n:returns: a negated vector"
		"\n:rtype: (float, float, float, float)"
	},
	{"Vec4Transform",		(PyCFunction) Vec4Transform, METH_VARARGS, 
		"Transforms a 4D vector by a given matrix."
		"\n:param vec: the source vector"
		"\n:type vec: (float, float, float, float)"
		"\n:param mat: the source matrix"
		"\n:type mat: tuple[tuple[float]]"
		"\n:returns: vector transformed by the source matrix"
		"\n:rtype: (float, float, float, float)"
	},
	{"Vec4TransformArray",	(PyCFunction) Vec4TransformArray, METH_VARARGS, 
		"Transforms an array (x, y, 0, 1) by a given matrix."
		"\n:param array: a sequence of points"
		"\n:type array: list[(float, float, float, float)]"
		"\n:param mat: the source matrix"
		"\n:type mat: tuple[tuple[float]]"
		"\n:returns: a sequence of points transformed by the source matrix"
		"\n:rtype: tuple[(float, float, float, float)]"
	},
	
	// Quaternions	
	//{"QuaternionIsIdentity",	(PyCFunction) QuaternionIsIdentity, METH_VARARGS, "QuaternionIsIdentity( a, b )"},
	//{"QuaternionLength",	(PyCFunction) QuaternionLength, METH_VARARGS, "QuaternionLength( a, b )"},
	{"QuaternionLengthSq",	(PyCFunction) QuaternionLengthSq, METH_VARARGS, 
		"Returns the square of the length of a quaternion."
		"\n:param quat: the source quaternion"
		"\n:type quat: (float, float, float, float)"
		"\n:returns: The quaternion's squared length."
		"\n:rtype: float"
	},
	//{"QuaternionConjugate",	(PyCFunction) QuaternionConjugate, METH_VARARGS, "QuaternionConjugate( a, b )"},
	//{"QuaternionDot",	(PyCFunction) QuaternionDot, METH_VARARGS, "QuaternionDot( a, b )"},
	{"QuaternionIdentity",	(PyCFunction) QuaternionIdentity, METH_NOARGS, 
		"Returns the identity quaternion."
		"\n:rtype: (float, float, float, float)"
	},
	//{"QuaternionBaryCentric",	(PyCFunction) QuaternionBaryCentric, METH_VARARGS, "QuaternionBaryCentric( a, b )"},
	//{"QuaternionExp",	(PyCFunction) QuaternionExp, METH_VARARGS, "QuaternionExp( a, b )"},
	{"QuaternionInverse",	(PyCFunction) QuaternionInverse, METH_VARARGS, 
		"Conjugates and renormalizes a quaternion."
		"\n:param quat: the source quaternion"
		"\n:type quat: (float, float, float, float)"
		"\n:returns: inverse quaternion of the quaternion."
		"\n:rtype: (float, float, float, float)"
	},
	//{"QuaternionLn",	(PyCFunction) QuaternionLn, METH_VARARGS, "QuaternionLn( a, b )"},
	{"QuaternionMultiply",	(PyCFunction) QuaternionMultiply, METH_VARARGS, 
		"Multiplies two quaternions."
		"\n:param quat1: the source quaternion"
		"\n:type quat1: (float, float, float, float)"
		"\n:param quat2: the source quaternion"
		"\n:type quat2: (float, float, float, float)"
		"\n:returns: the product of two quaternions."
		"\n:rtype: (float, float, float, float)"
	},
	{"QuaternionNormalize",	(PyCFunction) QuaternionNormalize, METH_VARARGS, 
		"Computes a unit length quaternion."
		"\n:param quat: the source quaternion"
		"\n:type quat: (float, float, float, float)"
		"\n:returns: normalized quaternion"
		"\n:rtype: (float, float, float, float)"
	},
	{"QuaternionRotationAxis",	(PyCFunction) QuaternionRotationAxis, METH_VARARGS, 
		"Rotates a quaternion about an arbitrary axis."
		"\n"
		"\nArguments:"
		"\n:param vec: the axis about which to rotate the quaternion"
		"\n:type vec: (float, float, float)"
		"\n:param angle: Angle of rotation, in radians. Angles are measured clockwise when looking along the rotation axis toward the origin."
		"\n:type angle: float"
		"\n:returns: quaternion rotated around the specified axis"
		"\n:rtype: (float, float, float, float)"
	},
	{"QuaternionRotationMatrix",	(PyCFunction) QuaternionRotationMatrix, METH_VARARGS, 
		"Builds a quaternion from a rotation matrix."
		"\n:param mat: the source matrix"
		"\n:type mat: tuple[tuple[float]]"
		"\n:returns: quaternion built from a rotation matrix"
		"\n:rtype: (float, float, float, float)"
	},
	{"QuaternionRotationSetYawPitchRoll",	(PyCFunction) QuaternionRotationSetYawPitchRoll, METH_VARARGS, 
		"Builds a quaternion with the given yaw, pitch, and roll."
		"\n:param yaw: Yaw around the y-axis, in radians."
		"\n:type yaw: float"
		"\n:param pitch: Pitch around the x-axis, in radians"
		"\n:type pitch: float"
		"\n:param roll: Roll around the z-axis, in radians"
		"\n:type roll: float"
		"\n:returns: the quaternion with the specified yaw, pitch and roll"
		"\n:rtype: (float, float, float, float)"
	},
	{"QuaternionRotationGetYawPitchRoll",	(PyCFunction) QuaternionRotationGetYawPitchRoll, METH_VARARGS, 
		"Extracts the yaw, pitch and roll from the given quaternion."
		"\n:param quat: the source quaternion"
		"\n:type quat: (float, float, float, float)"
		"\n:returns: the yaw, pitch and roll"
		"\n:rtype: (float, float, float)"
	},
	//{"QuaternionSlerp",	(PyCFunction) QuaternionSlerp, METH_VARARGS, "QuaternionSlerp( a, b )"},
	//{"QuaternionSquad",	(PyCFunction) QuaternionSquad, METH_VARARGS, "QuaternionSquad( a, b )"},
	//{"QuaternionSquadSetup",	(PyCFunction) QuaternionSquadSetup, METH_VARARGS, "QuaternionSquadSetup( a, b )"},
	//{"QuaternionToAxisAngle",	(PyCFunction) QuaternionToAxisAngle, METH_VARARGS, "QuaternionToAxisAngle( a, b )"},
	{"QuaternionRotationArc",	(PyCFunction) QuaternionRotationArc, METH_VARARGS, 
		"Computes the shortest rotation arc between two vectors."
		"\n:param vec1: the start vector"
		"\n:type vec1: (float, float, float)"
		"\n:param vec3: the end vector"		
		"\n:type vec3: (float, float, float)"
		"\n:returns: the shortest rotation quaternion"
		"\n:rtype: (float, float, float, float)"
	},
	{"QuaternionTransformVector",	(PyCFunction) QuaternionTransformVector, METH_VARARGS, 
		"Rotates the given vector by the given quaternion."
		"\n:param quat: the source quaternion"
		"\n:type quat: (float, float, float, float)"
		"\n:param vec: the vector to rotate"
		"\n:type vec: (float, float, float)"
		"\n:returns: the rotated vector"
		"\n:rtype: (float, float, float)"
	},
	
	// Plane	
	//{"PlaneScale",	(PyCFunction) PlaneScale, METH_VARARGS, "PlaneScale( a, b )"},
	//{"PlaneDot",	(PyCFunction) PlaneDot, METH_VARARGS, "PlaneDot( a, b )"},
	//{"PlaneDotCoord",	(PyCFunction) PlaneDotCoord, METH_VARARGS, "PlaneDotCoord( a, b )"},
	//{"PlaneDotNormal",	(PyCFunction) PlaneDotNormal, METH_VARARGS, "PlaneDotNormal( a, b )"},
	{"PlaneFromPointNormal",	(PyCFunction) PlaneFromPointNormal, METH_VARARGS, 
		"Constructs a plane from a point and a normal vector."
		"\n:param p1: the point on the plane"
		"\n:type p1: (float, float, float)"
		"\n:param p2: the normal of the plane"
		"\n:type p2: (float, float, float)"
		"\n:returns: plane that takes the form of the general plane equation. ax + by + cz + dw = 0."
		"\n:rtype: (float, float, float, float)"
	},
	{"PlaneFromPoints",	(PyCFunction) PlaneFromPoints, METH_VARARGS, 
		"Constructs a plane from three points."
		"\n:param p1: one of the points used to construct the plane"
		"\n:type p1: (float, float, float)"
		"\n:param p2: one of the points used to construct the plane"
		"\n:type p2: (float, float, float)"
		"\n:param p3: one of the points used to construct the plane"
		"\n:type p3: (float, float, float)"
		"\n:returns: plane that takes the form of the general plane equation. ax + by + cz + dw = 0."
		"\n:rtype: (float, float, float, float)"
	},
	{"PlaneIntersectLine",	(PyCFunction) PlaneIntersectLine, METH_VARARGS, 
		"Finds the intersection between a plane and a line."
		"\n:param plane: The source plane that takes the form of the general plane equation. ax + by + cz + dw = 0."
		"\n:type plane: (float, float, float, float)"
		"\n:param start: defining a line starting point"
		"\n:type start: (float, float, float)"
		"\n:param end: defining a line end point"
		"\n:type end: (float, float, float)"
		"\n:returns: the intersection between the specified plane and line"
		"\n:rtype: (float, float, float)"
	},
	//{"PlaneNormalize",	(PyCFunction) PlaneNormalize, METH_VARARGS, "PlaneNormalize( a, b )"},
	//{"PlaneTransform",	(PyCFunction) PlaneTransform, METH_VARARGS, "PlaneTransform( a, b )"},
	//{"PlaneTransformArray",	(PyCFunction) PlaneTransformArray, METH_VARARGS, "PlaneTransformArray( a, b )"},
	
	// Matrix
	{"MatrixIdentity",	(PyCFunction) MatrixIdentity, METH_NOARGS, 
		"Creates an identity matrix."
		"\n:rtype: tuple[tuple[float]]"
	},
	{"MatrixIsIdentity",	(PyCFunction) MatrixIsIdentity, METH_VARARGS, 
		"Is the matrix an identity matrix"
		"\n:param mat: the source matrix"
		"\n:type mat: tuple[tuple[float]]"
		"\n:rtype: bool"
	},	
	{"MatrixAffineTransformation",	(PyCFunction) MatrixAffineTransformation, METH_VARARGS, 
		"Builds a 3D affine transformation matrix. NULL arguments are treated as identity transformations."
		"\n:param scaling: scaling factor"
		"\n:type scaling: float"
		"\n:param rotationCenter: (3tuple)the center of rotation"
		"\n:type rotationCenter: (float, float, float)"
		"\n:param rotation: (4tuple)the rotation as a quaternion"
		"\n:type rotation: (float, float, float, float)"
		"\n:param translation: (3tuple)the translation"
		"\n:type translation: (float, float, float)"
		"\n:returns: A matrix that is an affine transformation matrix."
		"\n:rtype: tuple[tuple[float]]"
	},
	//{"MatrixAffineTransformation2D",	(PyCFunction) MatrixAffineTransformation2D, METH_VARARGS, "MatrixAffineTransformation2D( a, b )"},
	{"MatrixDecompose",	(PyCFunction) MatrixDecompose, METH_VARARGS, 
		"Breaks down a general 3D transformation matrix into its scalar, rotational, and translational components."
		"\n:param mat: the source matrix"
		"\n:type mat: tuple[tuple[float]]"
		"\n:returns: (3tuple,4tuple,3tuple), scaling factors applied along the x, y, and z-axes. Rotation and translation. If the function fails it returns None"
		"\n:rtype: ((float, float, float), (float, float, float, float), (float, float, float))"
	},
	{"MatrixDeterminant",	(PyCFunction) MatrixDeterminant, METH_VARARGS, 
		"Computes the determinant of a matrix"
		"\n:param mat: the source matrix"
		"\n:type mat: tuple[tuple[float]]"
		"\n:rtype: float"
	},	
	{"MatrixInverse",	(PyCFunction) MatrixInverse, METH_VARARGS, 
		"Calculates the inverse of a matrix."
		"\n:param mat: the source matrix"
		"\n:type mat: tuple[tuple[float]]"
		"\n:returns: A matrix that is the inverse of the matrix. If matrix inversion fails, None is returned."
		"\n:rtype: tuple[tuple[float]] | None"
	},
	{"MatrixLookAtLH",	(PyCFunction) MatrixLookAtLH, METH_VARARGS, 
		"Builds a left-handed, look-at matrix."
		"\n:param eye: (3tuple) defines the eye point. This value is used in translation."
		"\n:type eye: (float, float, float)"
		"\n:param at: (3tuple) defines the camera look-at target"
		"\n:type at: (float, float, float)"
		"\n:param up: (3tuple) defines the current world's up, usually [0, 1, 0]."
		"\n:type up: (float, float, float)"
		"\n:returns: A matrix that is a left-handed, look-at matrix."
		"\n:rtype: tuple[tuple[float]]"
	},
	{"MatrixLookAtRH",	(PyCFunction) MatrixLookAtRH, METH_VARARGS, 
		"Builds a right-handed, look-at matrix."
		"\n:param eye: (3tuple) defines the eye point. This value is used in translation."
		"\n:type eye: (float, float, float)"
		"\n:param at: (3tuple) defines the camera look-at target"
		"\n:type at: (float, float, float)"
		"\n:param up: (3tuple) defines the current world's up, usually [0, 1, 0]."
		"\n:type up: (float, float, float)"
		"\n:returns: A matrix that is a left-handed, look-at matrix."
		"\n:rtype: tuple[tuple[float]]"
	},
	{"MatrixMultiply",	(PyCFunction) MatrixMultiply, METH_VARARGS, 
		"Determines the product of two matrices."
		"\n:param mat1: left hand side matrix"
		"\n:type mat1: tuple[tuple[float]]"
		"\n:param mat2: right hand side matrix"
		"\n:type mat2: tuple[tuple[float]]"
		"\n:returns: A matrix that is the product of two matrices"
		"\n:rtype: tuple[tuple[float]]"
	},
	//{"MatrixMultiplyTranspose",	(PyCFunction) MatrixMultiplyTranspose, METH_VARARGS, "MatrixMultiplyTranspose( a, b )"},
	//{"MatrixOrthoOffCenterLH",	(PyCFunction) MatrixOrthoOffCenterLH, METH_VARARGS, "MatrixOrthoOffCenterLH( a, b )"},
	//{"MatrixOrthoOffCenterRH",	(PyCFunction) MatrixOrthoOffCenterRH, METH_VARARGS, "MatrixOrthoOffCenterRH( a, b )"},
	{"MatrixOrthoRH",	(PyCFunction) MatrixOrthoRH, METH_VARARGS,
	    "Build a right-handed, orthographic projection matrix."
		"\n:param width: width in meters"
		"\n:type width: float"
	    "\n:param height: heightin meters"
		"\n:type height: float"
	    "\n:param near: near depth in meters"
		"\n:type near: float"
	    "\n:param far: far depth in meters"
		"\n:type far: float"
		"\n:returns: A matrix that is a right-handed, orthograptic projection matrix."
		"\n:rtype: tuple[tuple[float]]"
    },	
	{"MatrixOrthoLH",	(PyCFunction) MatrixOrthoLH, METH_VARARGS,
	    "Build a left-handed, orthographic projection matrix."
		"\n:param width: width in meters"
		"\n:type width: float"
	    "\n:param height: heightin meters"
		"\n:type height: float"
	    "\n:param near: near depth in meters"
		"\n:type near: float"
	    "\n:param far: far depth in meters"
		"\n:type far: float"
		"\n:returns: A matrix that is a left-handed, orthograptic projection matrix."
		"\n:rtype: tuple[tuple[float]]"
    },	
	{"MatrixPerspectiveFovLH",	(PyCFunction) MatrixPerspectiveFovLH, METH_VARARGS, 
		"Builds a matrix for a perspective projection (left handed)"
		"\n:param fovy: vertical field of view"
		"\n:type fovy: float"
		"\n:param aspect: aspect ratio"
		"\n:type aspect: float"
		"\n:param zn: znear value"
		"\n:type zn: float"
		"\n:param zf: zfar value"
		"\n:type zf: float"
		"\n:returns: Perspective projection matrix (LH)"		
		"\n:rtype: tuple[tuple[float]]"
	},
	{"MatrixPerspectiveFovRH",	(PyCFunction) MatrixPerspectiveFovRH, METH_VARARGS, 
		"Builds a matrix for a perspective projection (right handed)"
		"\n:param fovy: vertical field of view"
		"\n:type fovy: float"
		"\n:param aspect: aspect ratio"
		"\n:type aspect: float"
		"\n:param zn: znear value"
		"\n:type zn: float"
		"\n:param zf: zfar value"
		"\n:type zf: float"
		"\n:returns: Perspective projection matrix (RH)"		
		"\n:rtype: tuple[tuple[float]]"
	},	
	//{"MatrixPerspectiveLH",	(PyCFunction) MatrixPerspectiveLH, METH_VARARGS, "MatrixPerspectiveLH( a, b )"},
	//{"MatrixPerspectiveRH",	(PyCFunction) MatrixPerspectiveRH, METH_VARARGS, "MatrixPerspectiveRH( a, b )"},
	//{"MatrixPerspectiveOffCenterLH",	(PyCFunction) MatrixPerspectiveOffCenterLH, METH_VARARGS, "MatrixPerspectiveOffCenterLH( a, b )"},
	//{"MatrixPerspectiveOffCenterRH",	(PyCFunction) MatrixPerspectiveOffCenterRH, METH_VARARGS, "MatrixPerspectiveOffCenterRH( a, b )"},
	//{"MatrixReflect",	(PyCFunction) MatrixReflect, METH_VARARGS, "MatrixReflect( a, b )"},
	{"MatrixRotationNormal",	(PyCFunction) MatrixRotationNormal, METH_VARARGS, 
		"Builds a rotation matrix from an axis and an angle in radians"
		"\n:param axis: (3tuple) the axis the matrix is rotating about"
		"\n:type axis: (float, float, float)"
		"\n:param angle: the angle in radians"
		"\n:type angle: float"
		"\n:returns: A matrix built from the axis and angle."
		"\n:rtype: tuple[tuple[float]]"
	},
	{"MatrixRotationAxis",	(PyCFunction) MatrixRotationAxis, METH_VARARGS, 
		"Builds a rotation matrix from an axis and an angle in radians"
		"\n:param axis: (3tuple) the axis the matrix is rotating about"
		"\n:type axis: (float, float, float)"
		"\n:param angle: the angle in radians"
		"\n:type angle: float"
		"\n:returns: A matrix built from the axis and angle."
		"\n:rtype: tuple[tuple[float]]"
	},
	{"MatrixRotationQuaternion",	(PyCFunction) MatrixRotationQuaternion, METH_VARARGS, 
		"Builds a rotation matrix from a quaternion."
		"\n:param quat: (4tuple) source quaternion"
		"\n:type quat: (float, float, float, float)"
		"\n:returns: A matrix built from the source quaternion."
		"\n:rtype: tuple[tuple[float]]"
	},	
	{"MatrixRotationX",	(PyCFunction) MatrixRotationX, METH_VARARGS, 
		"Builds a matrix with a rotation around the x axis."
		"\n:param rotation: Rotation of the x axis in radians."
		"\n:type rotation: float"
		"\n:returns: A matrix with the specified x rotation"
		"\n:rtype: tuple[tuple[float]]"
	},
	{"MatrixRotationY",	(PyCFunction) MatrixRotationY, METH_VARARGS, 
		"Builds a matrix with a rotation around the y axis."
		"\n:param rotation: Rotation of the y axis in radians."
		"\n:type rotation: float"
		"\n:returns: A matrix with the specified y rotation"
		"\n:rtype: tuple[tuple[float]]"
	},
	{"MatrixRotationZ",	(PyCFunction) MatrixRotationZ, METH_VARARGS, 
		"Builds a matrix with a rotation around the z axis."
		"\n:param rotation: Rotation of the z axis in radians."
		"\n:type rotation: float"
		"\n:returns: A matrix with the specified z rotation"
		"\n:rtype: tuple[tuple[float]]"
	},
	{"MatrixRotationYawPitchRoll",	(PyCFunction) MatrixRotationYawPitchRoll, METH_VARARGS, 
		"Builds a matrix with a specified yaw, pitch, and roll."
		"\n:param yaw: Yaw around the y-axis, in radians."
		"\n:type yaw: float"
		"\n:param pitch: Pitch around the x-axis, in radians"
		"\n:type pitch: float"
		"\n:param roll: Roll around the z-axis, in radians"
		"\n:type roll: float"
		"\n:returns: A matrix with the specified yaw, pitch, and roll."
		"\n:rtype: tuple[tuple[float]]"
	},
	{"MatrixScaling",	(PyCFunction) MatrixScaling, METH_VARARGS, 
		"Builds a matrix using the specified scale"
		"\n:param x: X-coordinate scale"
		"\n:type x: float"
		"\n:param y: Y-coordinate scale"
		"\n:type y: float"
		"\n:param z: Z-coordinate scale"
		"\n:type z: float"
		"\n:returns: The scaled transformation matrix."
		"\n:rtype: tuple[tuple[float]]"
	},
	//{"MatrixShadow",	(PyCFunction) MatrixShadow, METH_VARARGS, "MatrixShadow( a, b )"},
	{"MatrixTransformation",	(PyCFunction) MatrixTransformation, METH_VARARGS, 
		"Builds a transformation matrix. NULL arguments are treated as identity transformations."
		"\n:param scalingCenter: (3tuple) identifying the scaling center point"
		"\n:type scalingCenter: (float, float, float) | None"
		"\n:param scalingRotation: (4tuple) the quaternion that specifies the scaling rotation"
		"\n:type scalingRotation: (float, float, float, float) | None"
		"\n:param scaling: (3tuple) the scaling vector"
		"\n:type scaling: (float, float, float) | None"
		"\n:param rotationCenter: (3tuple) point that identifies the center of rotation"
		"\n:type rotationCenter: (float, float, float) | None"
		"\n:param rotation: (4tuple) the quaternion that specifies the center of rotation"
		"\n:type rotation: (float, float, float, float) | None"
		"\n:param translation: (3tuple) representing the translation"
		"\n:type translation: (float, float, float) | None"
		"\n:rtype: tuple[tuple[float]]"
	},
	//{"MatrixTransformation2D",	(PyCFunction) MatrixTransformation2D, METH_VARARGS, "MatrixTransformation2D( a, b )"},
	{"MatrixTranslation",	(PyCFunction) MatrixTranslation, METH_VARARGS, 
		"Builds a matrix using the specified offsets"
		"\n:param x: X-coordinate offset"
		"\n:type x: float"
		"\n:param y: Y-coordinate offset"
		"\n:type y: float"
		"\n:param z: Z-coordinate offset"
		"\n:type z: float"
		"\n"
		"\n:returns: The translated transformation matrix."
		"\n:rtype: tuple[tuple[float]]"
	},
	{"MatrixTranspose",	(PyCFunction) MatrixTranspose, METH_VARARGS, 
		"Returns the matrix transpose of a matrix"
		"\n:param mat: (4tuple,4tuple,4tuple,4tuple)The matrix to transpose"
		"\n:type mat: tuple[tuple[float]]"
		"\n:returns: The mat transposed"
		"\n:rtype: tuple[tuple[float]]"
	},

	// Color	
	//{"ColorLerp",	(PyCFunction) ColorLerp, METH_VARARGS, "ColorLerp( a, b )"},
	//{"ColorModulate",	(PyCFunction) ColorModulate, METH_VARARGS, "ColorModulate( a, b )"},
	//{"ColorNegative",	(PyCFunction) ColorNegative, METH_VARARGS, "ColorNegative( a, b )"},
	//{"ColorScale",	(PyCFunction) ColorScale, METH_VARARGS, "ColorScale( a, b )"},
	//{"ColorSubtract",	(PyCFunction) ColorSubtract, METH_VARARGS, "ColorSubtract( a, b )"},
	//{"ColorAdd",	(PyCFunction) ColorAdd, METH_VARARGS, "ColorAdd( a, b )"},
	//{"ColorAdjustContrast",	(PyCFunction) ColorAdjustContrast, METH_VARARGS, "ColorAdjustContrast( a, b )"},
	//{"ColorAdjustSaturation",	(PyCFunction) ColorAdjustSaturation, METH_VARARGS, "ColorAdjustSaturation( a, b )"},
	
	// Misc
	//{"FresnelTerm",	(PyCFunction) FresnelTerm, METH_VARARGS, "FresnelTerm( a, b )"},
	{"RayToPlaneIntersection",	(PyCFunction) RayToPlaneIntersection, METH_VARARGS, 
		"Computes the intersection of a ray and a plane"
		"\n:param point1: (3tuple) The point from where the ray starts"
		"\n:type point1: (float, float, float)"
		"\n:param ray: (3tuple) The direction of the ray"
		"\n:type ray: (float, float, float)"
		"\n:param point2: (3tuple) The point on the target plane"
		"\n:type point2: (float, float, float)"
		"\n:param normal: (3tuple) The normal of the target plane"			
		"\n:type normal: (float, float, float)"
		"\n:returns: The position in world coordinates where the ray intersects the plane or None on Failure"
		"\n:rtype: (float, float, float) | None"
	
	},	
	{"IntersectTri",	(PyCFunction) IntersectTri, METH_VARARGS, 
		"Computes the intersection of a ray and a triangle"
		"\n"
		"\nArguments:"
		"\n:param point1: (3tuple) The first point on the triangle"
		"\n:type point1: (float, float, float)"
		"\n:param point2: (3tuple) The second point on the triangle"
		"\n:type point2: (float, float, float)"
		"\n:param point3: (3tuple) The third point on the triangle"
		"\n:type point3: (float, float, float)"
		"\n:param rayPos: (3tuple) The origin coordinate of the ray"
		"\n:type rayPos: (float, float, float)"
		"\n:param rayDir: (3tuple) The direction of the ray. This vector should not be (0,0,0) but does not need to be normalized."
		"\n:type rayDir: (float, float, float)"
		"\n:returns: (u, v, dist) : Barycentric hit coordinates, U and V. Distance to the hit point from rayPos or None on failure"
		"\n:rtype: (float, float, float) | None"

	},
	{"SphereBoundProbe",	(PyCFunction) SphereBoundProbe, METH_VARARGS, 
		"Determines if a ray intersects the volume of a sphere's bounding box"
		"\n:param center: (3tuple) The center coordinates of the sphere"
		"\n:type center: (float, float, float)"
		"\n:param radius: (3tuple) The radius of the sphere"
		"\n:type radius: (float, float, float)"
		"\n:param rayPos: (3tuple) The origin coordinate of the ray"
		"\n:type rayPos: (float, float, float)"
		"\n:param rayDir: (3tuple) The direction of the ray. This vector should not be (0,0,0) but does not need to be normalized."
		"\n:type rayDir: (float, float, float)"
		"\n:returns: Did we intersect or not"
		"\n:rtype: bool"
	},
	{"BoxBoundProbe",	(PyCFunction) BoxBoundProbe, METH_VARARGS, 
		"Determines whether a ray intersects the volume of a box's bounding box"
		"\n:param center: (3tuple) Describing the center of the bounding box"
		"\n:type center: (float, float, float)"
		"\n:param extends: (3tuple) Describing the extend to the min and max point of the bounding box"
		"\n:type extends: (float, float, float)"
		"\n:param rayPos: (3tuple) The origin coordinate of the ray"
		"\n:type rayPos: (float, float, float)"
		"\n:param rayDir: (3tuple) The direction of the ray. This vector should not be (0,0,0) but does not need to be normalized."
		"\n:type rayDir: (float, float, float)"
		"\n:returns: Did we intersect or not"
		"\n:rtype: bool"
	},
	{"ComputeBoundingBox",	(PyCFunction) ComputeBoundingBox, METH_VARARGS, 
		"Computes a coordinate-axis oriented bounding box"
		"\n:param points: A sequence of 3d points"
		"\n:type points: list[(float, float, float)]"
		"\n:returns: (center, extends), describing the center and the distance to the min and max points"		
		"\n:rtype: ((float, float, float), (float, float, float))"
	},
	{"ComputeBoundingSphere",	(PyCFunction) ComputeBoundingSphere, METH_VARARGS, 
		"Computes a bounding sphere for the points"
		"\n:param points: A sequence of 3d points"
		"\n:type points: list[(float, float, float)]"
		"\n:returns: (center, radius), describing the center and radius of the sphere"	
		"\n:rtype: ((float, float, float), float)"
	},

	{"Multiply",	(PyCFunction) Multiply, METH_VARARGS, 
		"Determines the product of two matrices."
		"\n:param mat1 : matrix or a vector"
		"\n:type mat1: tuple[tuple[float]] | tuple[float]"
		"\n:param mat2 : matrix or a vector"
		"\n:type mat2: tuple[tuple[float]] | tuple[float]"
		"\n:returns: A matrix that is the product of two matrices, or a 4tuple that is the product of a matrix and a vector"
		"\n:rtype: tuple[tuple[float]] | tuple[float]"
	},
	{"Direction",	(PyCFunction) VecDirection, METH_VARARGS, 
		"Normalize the difference between two vectors."
		"\n:param v1: a source vector"
		"\n:type v1: tuple[float]"
		"\n:param v2 : a source vector"
		"\n:type v2: tuple[float]"
		"\n:returns: the normalized difference of two vectors"
		"\n:rtype: tuple[float]"
	},
	{"Add",	(PyCFunction) VecAdd, METH_VARARGS, 
		"Adds two vectors."
		"\n:param v1: a source vector"
		"\n:type v1: tuple[float]"
		"\n:param v2 : a source vector"
		"\n:type v2: tuple[float]"
		"\n:returns: the sum of two vectors"
		"\n:rtype: tuple[float]"
	},
	{"Subtract",		(PyCFunction) VecSubtract, METH_VARARGS, 
		"Subtracts two vectors."
		"\n:param v1: a source vector"
		"\n:type v1: tuple[float]"
		"\n:param v2 : a source vector"
		"\n:type v2: tuple[float]"
		"\n:returns: the difference of two vectors"
		"\n:rtype: tuple[float]"
	},
	{"Lerp",			(PyCFunction) VecLerp, METH_VARARGS, 
		"Performs a linear interpolation between two vectors."
		"\n:param v1: a source vector"
		"\n:type v1: tuple[float]"
		"\n:param v2 : a source vector"
		"\n:type v2: tuple[float]"
		"\n:param s: scaling factor that linearly interpolates between the vectors."
		"\n:type s: float"
		"\n:returns: a vector that is the result of the linear interpolation"
		"\n:rtype: tuple[float]"
	},
	{"Max",		(PyCFunction) VecMaximize, METH_VARARGS, 
		"Returns a vector that is made up of the largest components of two vectors."
		"\n:param v1: a source vector"
		"\n:type v1: tuple[float]"
		"\n:param v2 : a source vector"
		"\n:type v2: tuple[float]"
		"\n:returns: a vector that is made up of the largest components of the two vectors."
		"\n:rtype: tuple[float]"
	},
	{"Min",		(PyCFunction) VecMinimize, METH_VARARGS, 
		"Returns a vector that is made up of the smallest components of two vectors"
		"\n:param v1: a source vector"
		"\n:type v1: tuple[float]"
		"\n:param v2 : a source vector"
		"\n:type v2: tuple[float]"
		"\n:returns: a vector that is made up of the smallest components of the two vectors."
		"\n:rtype: tuple[float]"
	},
	{"Scale",			(PyCFunction) VecScale, METH_VARARGS, 
		"Scales a vector."
		"\n:param v: a source vector"
		"\n:type v: tuple[float]"
		"\n:param scale: scaling factor"
		"\n:type scale: float"
		"\n:returns: the scaled vector."
		"\n:rtype: tuple[float]"
	},
	{"BaryCentric",		(PyCFunction) VecBaryCentric, METH_VARARGS, 
		"Returns a point in Barycentric coordinates, using the specified vectors."
		"\nThe Vec3BaryCentric function provides a way to understand points in and around a triangle," 
		"\nindependent of where the triangle is actually located. This function returns the resulting point"
		"\nby using the following equation: V1 + f(V2-V1) + g(V3-V1). "
		"\n"
		"\nAny point in the plane V1V2V3 can be represented by the Barycentric coordinate (f,g).The parameter"
		"\nf controls how much V2 gets weighted into the result and the parameter g controls how much V3 gets"
		"\nweighted into the result. Lastly, 1-f-g controls how much V1 gets weighted into the result."
		"\n"
		"\nNote the following relations."
		"\n"
		"\nIf (f>=0 &, & g>=0 &, & 1-f-g>=0), the point is inside the triangle V1V2V3."
		"\nIf (f==0 &, & g>=0 &, & 1-f-g>=0), the point is on the line V1V3."
		"\nIf (f>=0 &, & g==0 &, & 1-f-g>=0), the point is on the line V1V2."
		"\nIf (f>=0 &, & g>=0 &, & 1-f-g==0), the point is on the line V2V3."
		"\nBarycentric coordinates are a form of general coordinates. In this context, using Barycentric"
		"\ncoordinates represents a change in coordinate systems. What holds true for Cartesian coordinates"
		"\nholds true for Barycentric coordinates."
		"\n:param v1: a position vector"
		"\n:type v1 tuple[float]"
		"\n:param v2: a position vector"
		"\n:type v2: tuple[float]"
		"\n:param v3: a position vector"
		"\n:type v3: tuple[float]"
		"\n:param f: Weighting factor"
		"\n:type f: float"
		"\n:param g: Weighting factor"
		"\n:type g: float"
		"\n:returns: vector in Barycentric coordinates."
		"\n:rtype: tuple[float]"
	},
	{"Hermite",			(PyCFunction) VecHermite, METH_VARARGS, 
		"Performs a Hermite spline interpolation, using the specified vectors."
		"\n:param pos1: a position vector"
		"\n:type pos1: tuple[float]"
		"\n:param tan1: a position vector"
		"\n:type tan1: tuple[float]"
		"\n:param pos2: a position vector"
		"\n:type pos2: tuple[float]"
		"\n:param tan2: a position vector"
		"\n:type tan2: tuple[float]"
		"\n:param t: the interpolant"
		"\n:type t: float"
		"\n:returns: vector that is the result of the Hermite spline interpolation"
		"\n:rtype: tuple[float]"
	},
	{"CatmullRom",		(PyCFunction) VecCatmullRom, METH_VARARGS, 
		"Performs a Catmull-Rom interpolation, using the specified vectors."
		"\n:param pos1: a position vector"
		"\n:type pos1: tuple[float]"
		"\n:param tan1: a position vector"
		"\n:type tan1: tuple[float]"
		"\n:param pos2: a position vector"
		"\n:type pos2: tuple[float]"
		"\n:param tan2: a position vector"
		"\n:type tan2: tuple[float]"
		"\n:param t: the interpolant"
		"\n:type t: float"
		"\n:returns: vector that is the result of the Catmull-Rom interpolation"
		"\n:rtype: tuple[float]"
	},
	{"CreateHermiteSegment",		(PyCFunction) CreateHermiteSegment, METH_VARARGS, 
		"Create a 6-tuple coefficient vector of a Hermite polynomial"
		"\n:param startPoint: The start y value of the segment"
		"\n:type startPoint: float"
		"\n:param inTangent: The tangent at the start point"
		"\n:type inTangent: float"
		"\n:param endValue: The end y value of the segment"
		"\n:type endValue: float"
		"\n:param outTangent: The tangent at the end point"
		"\n:type outTangent: float"
		"\n:param timeRange: The length of the segment"
		"\n:type timeRange: float"
		"\n:returns: 6-tuples (c0,c1,c2,c4,endValue,timeRange)"
		"\n:rtype: tuple[float]"
	},
	{"CreateHermiteSegmentD",		(PyCFunction) CreateHermiteSegmentD, METH_VARARGS, 
		"Create a 6-tuple coefficient vector of a Hermite polynomial with double precision"
		"\n:param startPoint: The start y value of the segment"
		"\n:type startPoint: float"
		"\n:param inTangent: The tangent at the start point"
		"\n:type inTangent: float"
		"\n:param endValue: The end y value of the segment"
		"\n:type endValue: float"
		"\n:param outTangent: The tangent at the end point"
		"\n:type outTangent: float"
		"\n:param timeRange: The length of the segment"
		"\n:type timeRange: float"
		"\n:returns: 6-tuples (c0,c1,c2,c4,endValue,timeRange)"
		"\n:rtype: (float, float, float, float, float, float)"
	},
	{"EvaluateHermiteSegment",		(PyCFunction) EvaluateHermiteSegment, METH_VARARGS, 
		"Performs a Hermite evaluation of the segment and time"
		"\n:param hermiteSegment: 6-tuple containing the 4 coefficiants, endValue and timeRange"
		"\n:type hermiteSegment: (float, float, float, float, float, float)"
		"\n:param time: the time evaluate the segment on."
		"\n:type time: float"
		"\n:returns: scalar value. If time < 0 it will return the startValue"
		"\n  time > timeRange will return the end value."
		"\n  otherwise the scalar value of the evaluated segment"
		"\n:rtype: float"
	},
	{"EvaluateHermiteSegmentD",		(PyCFunction) EvaluateHermiteSegmentD, METH_VARARGS, 
		"Performs a Hermite evaluation of the segment and time with double precision"
		"\n:param hermiteSegment: 6-tuple containing the 4 coefficiants, endValue and timeRange"
		"\n:type hermiteSegment: (float, float, float, float, float, float)"
		"\n:param time: the time evaluate the segment on."
		"\n:type time: float"
		"\n:returns: scalar value. If time < 0 it will return the startValue"
		"\n  time > timeRange will return the end value."
		"\n  otherwise the scalar value of the evaluated segment"
		"\n:rtype: float"
	},
	{NULL,      NULL}      
};

#define GEO2_CONCAT( a, b ) _GEO2_CONCAT( a, b )
#define _GEO2_CONCAT( a, b ) a##b
#define GEO2_STRINGIZE( x ) _GEO2_STRINGIZE( x )
#define _GEO2_STRINGIZE( s ) #s

static struct PyModuleDef moduleDef =
{
	PyModuleDef_HEAD_INIT,
	GEO2_STRINGIZE( GEO2_CONCAT( _geo2, CCP_BUILD_FLAVOR ) ),
	"",
	-1,
	native_methods
};
PyMODINIT_FUNC
#ifndef _WIN32
__attribute__((visibility("default")))
#endif
	GEO2_CONCAT( PyInit__geo2, CCP_BUILD_FLAVOR )()
{
	PyObject *module;

	if ( PyType_Ready( &Vector_Type ) < 0 )
		return nullptr;

	if ( PyType_Ready( &VectorD_Type ) < 0 )
		return nullptr;

	module = PyModule_Create( &moduleDef );

	if ( module == nullptr )
		return nullptr;

	Py_INCREF( &Vector_Type );	
	PyModule_AddObject( module, "Vector", (PyObject *)&Vector_Type );
	Py_INCREF( &VectorD_Type );
	PyModule_AddObject( module, "VectorD", (PyObject *)&VectorD_Type );

	return module;
}
