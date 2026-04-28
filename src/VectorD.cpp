// Copyright © 2026 CCP ehf.

#include "Vector.h"
#include "Util.h"
#ifndef _WIN32
#include <CcpSecureCrt.h>
#endif
#include <cstdint>
#include <algorithm>
#include <limits>

// Allocate memory for the python object
static PyObject* VectorD_alloc( PyTypeObject *type, VectorD* value, unsigned char size )
{
	PyObject *self;

	self = type->tp_alloc( type, 0 );
	if( self == NULL )
		return NULL;

	( ( VectorD_Object* )self )->size = size;
	(( VectorD_Object* )self )->value = *value;
	return self;
}

// object.x == getattr( object, "x" )
// Here we resolve any member variables
PyObject* VectorD_getattr( PyObject* self, char* attrname )
{
	auto* attr = PyUnicode_FromString( attrname );
	if( attr == nullptr )
	{
		return nullptr;
	}
	VectorD_Object *object = (VectorD_Object *)self;
	int stringLength = (int)strlen( attrname );
	if( stringLength == 1 )
	{
		int index = ((attrname[0] - 'w')+3)%4;
		if( index < 0 || index >= object->size )
		{
			return PyObject_GenericGetAttr( self, attr );
		}
		else
		{
			return PyFloat_FromDouble( object->value.u[index] );
		}
	}
	else if( stringLength < 5 )
	{
		VectorD	vector;
		for( int i = 0; i < stringLength; i++ )
		{
			int index = ((attrname[i] - 'w')+3)%4;
			if( index < 0  || index  >= object->size )
			{
				// If none is found, forward the query to the generic attribute handler
				return PyObject_GenericGetAttr( self, attr );
			}
			vector.u[i] = object->value.u[index];
		}
		return VectorD_alloc( &VectorD_Type, &vector, stringLength );
	}
	else
	{
		// If none is found, forward the query to the generic attribute handler
		return PyObject_GenericGetAttr( self, attr );
	}
}

// component assignment
int VectorD_setattr( PyObject* self, char* attrname, PyObject* value )
{
	// if someone calls del vector.x, value will be null. We don't allow that
	if ( value == NULL )
	{
		PyErr_SetString( PyExc_TypeError, "read-only attribute" );
		return -1;
	}

	auto* attr = PyUnicode_FromString( attrname );
	if( attr == nullptr )
	{
		return 0;
	}

	int stringLength = (int)strlen( attrname );
	// Only check the first character
	if( stringLength == 1 )
	{
		VectorD_Object *object = (VectorD_Object *)self;
		int index = ((attrname[0] - 'w')+3)%4;
		if( index < 0 || index >= object->size )
		{
			return PyObject_GenericSetAttr( self, attr, value );
		}
		else
		{
			double fresult;
			if ( !GetRealValue<double>( value, &fresult ) )
			{
				PyErr_SetString( PyExc_TypeError, "a double is required" );
				return -1;
			}
			object->value.u[index] = fresult;
		}
	}
	else
	{
		return PyObject_GenericSetAttr( self, attr, value );
	}
	return 0;
}

// For iterators
Py_ssize_t VectorD_length( VectorD_Object *a )
{
	return a->size;
}

// Get at the vector components by item index
PyObject* VectorD_get_item( VectorD_Object *a, Py_ssize_t i )
{
	if ( i < 0 || i >= a->size ) {
		PyErr_SetString( PyExc_IndexError, "vector index out of range" );
		return NULL;
	}

	return PyFloat_FromDouble( (a->value).u[(uint32_t)i] );
}

// Set a vector component by index
int VectorD_set_item( VectorD_Object *a, Py_ssize_t i, PyObject* value )
{
	if ( i < 0 || i >= a->size ) {
		PyErr_SetString( PyExc_IndexError, "vector index out of range" );
		return -1;
	}

	double val;
	if ( !GetRealValue<double>( value, &val ) )
	{
		PyErr_SetString( PyExc_TypeError, "a double is required" );
		return -1;
	}

	(a->value).u[(uint32_t)i] = val;

	return 0;
}

// Print a string representation of the object for inspection
PyObject* VectorD_repr( VectorD_Object *v )
{
	char buff[256];

	// Since there are only 3 types of vectors to create,
	// no need to complicate things.
	if( v->size == 3 ) // 3 is the most common size so lets check for that first
	{
		sprintf_s( buff, "(%g, %g, %g)", (v->value).x, (v->value).y, (v->value).z );
	}
	else if ( v->size == 2 )
	{
		sprintf_s( buff, "(%g, %g)", (v->value).x, (v->value).y );
	}
	else
	{
		sprintf_s( buff, "(%g, %g, %g, %g)", (v->value).x, (v->value).y, (v->value).z, (v->value).w );
	}
	return PyUnicode_FromString( buff );
}

PyObject* VectorD_new( PyTypeObject *type, PyObject *args, PyObject *kwds )
{
	VectorD vec = {0.0, 0.0, 0.0, 0.0};
	Py_ssize_t tupleSize = PyTuple_Size(args);
	if( tupleSize >= 2 )
	{
		if( !PyArg_ParseTuple( args, "dd|dd", &vec.u[0], &vec.u[1], &vec.u[2], &vec.u[3] ) )
		{
			return NULL;
		}
	}
	else if( tupleSize == 1 )
	{
		PyObject* p = NULL;

		if( !PyArg_ParseTuple( args, "O", &p ) )
		{
			return NULL;
		}
		size_t vecSize = ExtractDoublesFromSequence( p, vec, 2);
		if(vecSize == 0)
		{
			return NULL;
		}
		return VectorD_alloc( type, &vec, (unsigned char)vecSize );
	}
	else
	{
		PyErr_SetString( PyExc_Exception, "geo2::VectorD: Constructor needs arguments" );
		return NULL;
	}

	return VectorD_alloc( type, &vec, (unsigned char)PySequence_Size( args ) );
}

// Addition
PyObject* VectorD_add( PyObject *py_left, PyObject *py_right )
{
	bool rightIsVec = VectorD_Check( py_right );
	bool leftIsVec = VectorD_Check( py_left );
	if( leftIsVec && rightIsVec )
	{
		unsigned char size = std::min( ( ( VectorD_Object* ) py_left )->size,( ( VectorD_Object* )py_right )->size );
		VectorD result;
		for( int i = 0; i < size; i++ )
		{
			result.u[i] = (( VectorD_Object* ) py_left )->value.u[i] +
						  (( VectorD_Object* ) py_right )->value.u[i];
		}
		return VectorD_alloc( &VectorD_Type, &result, size );
	}
	else
	{
		// Support for adding a tuple to the vector...
		VectorD a;
		size_t size;
		if( ( ExtractVectorDFromSequence( py_left, 2, a, &size ) && rightIsVec ) ||
			( ExtractVectorDFromSequence( py_right, 2, a, &size ) && leftIsVec ) )
		{
			PyObject* vector = rightIsVec ? py_right : py_left;
			unsigned char minSize = std::min( ( ( VectorD_Object* ) vector )->size, (unsigned char)size );
			VectorD result;
			for( int i = 0; i < minSize; i++ )
			{
				result.u[i] = a.u[i] + (( VectorD_Object* ) vector )->value.u[i];
			}
			return VectorD_alloc( &VectorD_Type, &result, minSize );
		}
		else
		{
			Py_INCREF( Py_NotImplemented );
			return Py_NotImplemented;
		}
	}
}

// Subtraction
PyObject* VectorD_sub( PyObject *py_left, PyObject *py_right )
{
	bool rightIsVec = VectorD_Check( py_right );
	bool leftIsVec = VectorD_Check( py_left );
	if( leftIsVec && rightIsVec )
	{
		unsigned char size = std::min( ( ( VectorD_Object* ) py_left )->size,( ( VectorD_Object* )py_right )->size );

		VectorD result;
		for( int i = 0; i < size; i++ )
		{
			result.u[i] = (( VectorD_Object* ) py_left )->value.u[i] -
				(( VectorD_Object* ) py_right )->value.u[i];
		}
		return VectorD_alloc( &VectorD_Type, &result, size );
	}
	else
	{
		// Support for subtracting a tuple from the vector...
		VectorD a;
		size_t size;
		if( rightIsVec && ExtractVectorDFromSequence( py_left, 2, a, &size ) )
		{
			unsigned char minSize = std::min( ( ( VectorD_Object* ) py_right )->size, (unsigned char)size );
			VectorD result;
			for( int i = 0; i < minSize; i++ )
			{
				result.u[i] = a.u[i] - (( VectorD_Object* ) py_right )->value.u[i];
			}
			return VectorD_alloc( &VectorD_Type, &result, minSize );
		}
		else if(ExtractVectorDFromSequence( py_right, 2, a, &size ))
		{
			unsigned char minSize = std::min( ( ( VectorD_Object* ) py_left )->size, (unsigned char)size );
			VectorD result;
			for( int i = 0; i < minSize; i++ )
			{
				result.u[i] = (( VectorD_Object* ) py_left )->value.u[i] - a.u[i];
			}
			return VectorD_alloc( &VectorD_Type, &result, minSize );
		}
		else
		{
			Py_INCREF( Py_NotImplemented );
			return Py_NotImplemented;
		}
	}
}

// Multiplication
PyObject* VectorD_mul( PyObject *py_left, PyObject *py_right )
{
	double scalar;
	bool rightIsVec = VectorD_Check( py_right );
	bool leftIsVec = VectorD_Check( py_left );
	if ( leftIsVec && GetRealValue<double>( py_right, &scalar ) )
	{
		unsigned char size = ( ( VectorD_Object* )py_left )->size;
		VectorD result;
		for( int i = 0; i < size; i++ )
		{
			result.u[i] = (( VectorD_Object* ) py_left )->value.u[i] * scalar;
		}
		return VectorD_alloc( &VectorD_Type, &result, size );
	}
	else if ( GetRealValue<double>( py_left, &scalar ) && rightIsVec )
	{
		unsigned char size = ( ( VectorD_Object* )py_right )->size;
		VectorD result;
		for( int i = 0; i < size; i++ )
		{
			result.u[i] = (( VectorD_Object* ) py_right )->value.u[i] * scalar;
		}
		return VectorD_alloc( &VectorD_Type, &result, size );
	}
	else
	{
		Py_INCREF( Py_NotImplemented );
		return Py_NotImplemented;
	}
}

// Division
PyObject* VectorD_div( PyObject *py_left, PyObject *py_right )
{
	double scalar;
	if( VectorD_Check( py_left ) && GetRealValue<double>( py_right, &scalar ) )
	{
		if ( fabs( scalar ) < std::numeric_limits<double>::epsilon() )
		{
			PyErr_SetString( PyExc_Exception, "geo2::VectorD: Can't divide by Zero" );
			return NULL;
		}
		scalar = 1.0/scalar;
		unsigned char size = ( ( VectorD_Object* )py_left )->size;
		VectorD result;
		for( int i = 0; i < size; i++ )
		{
			result.u[i] = (( VectorD_Object* ) py_left )->value.u[i] * scalar;
		}
		return VectorD_alloc( &VectorD_Type, &result, size );
	}
	else
	{
		Py_INCREF( Py_NotImplemented );
		return Py_NotImplemented;
	}
}
// Negation
PyObject* VectorD_neg( VectorD_Object *v )
{
	VectorD result;
	for( int i = 0; i < v->size; i++ )
	{
		result.u[i] =  -v->value.u[i];
	}
	return VectorD_alloc( &VectorD_Type, &result, v->size );
}

static PyNumberMethods VectorD_as_number = {
	VectorD_add,			/* nb_add */
	VectorD_sub,			/* nb_subtract */
	VectorD_mul,			/* nb_multiply */
	nullptr,						/* nb_remainder */
	nullptr,						/* nb_divmod */
	nullptr,						/* nb_power */
	(unaryfunc)VectorD_neg,	/* nb_negative */
	nullptr, /* nb_positive */
	nullptr, /* nb_absolute */
	nullptr, /* nb_nonzero */
	nullptr, /* nb_invert */
	nullptr, /* nb_lshift */
	nullptr, /* nb_rshift */
	nullptr, /* nb_and */
	nullptr, /* nb_xor */
	nullptr, /* nb_or */
	nullptr, /* nb_coerce */
	nullptr, /* nb_int */
	nullptr, /* nb_unused */
	nullptr, /* nb_float */
	nullptr, /* nb_inplace_add */
	nullptr, /* nb_inplace_subtract */
	nullptr, /* nb_inplace_multiply */
	nullptr, /* nb_inplace_remainder */
	nullptr, /* nb_inplace_power */
	nullptr, /* nb_inplace_lshift */
	nullptr, /* nb_inplace_rshift */
	nullptr, /* nb_inplace_and */
	nullptr, /* nb_inplace_xor */
	nullptr, /* nb_inplace_or */
	nullptr, /* nb_floor_divide */
	VectorD_div, /* nb_true_divide */
	nullptr, /* nb_inplace_floor_divide */
	nullptr, /* nb_inplace_true_divide */
	nullptr /* nb_index */
};

static PySequenceMethods VectorD_as_sequence = {
	(lenfunc)VectorD_length,			/* sq_length */
	0,
	0,
	(ssizeargfunc)VectorD_get_item,		/* sq_item */
	0,
	(ssizeobjargproc)VectorD_set_item,/* sq_ass_item */
	0,					/* sq_ass_slice */
	0,
};

// Pickle support
PyObject* VectorD_getinitargs( VectorD_Object *v )
{
	return VectorDAsPyFloats( v->value, v->size );
}

PyObject* VectorD_getstate( VectorD_Object *v )
{
	return VectorDAsPyFloats( v->value, v->size );
}

PyObject* VectorD_setstate( VectorD_Object *v, PyObject* state )
{
	if (!PyTuple_Check(state) || Py_SIZE(state) > 4 || Py_SIZE(state) < 2) {
		PyErr_Format(PyExc_TypeError,
			"%.200s.__setstate__ argument should be (2-4)-tuple, got %.200s",
			Py_TYPE(v)->tp_name, Py_TYPE(state)->tp_name);
		return NULL;
	}

	VectorD a;
	if(!ExtractVectorDFromSequence( state, a, 2 ))
	{
		return NULL;
	}

	v->value = a;
	v->size = (unsigned char)Py_SIZE( (PyObject*)state );
	Py_RETURN_NONE;
}

PyObject* VectorD_reduce( VectorD_Object *v )
{
	PyObject* ret = PyTuple_New( 3 );
	PyTuple_SET_ITEM( ret, 0, PyObject_GetAttrString( (PyObject*)v, "__class__" ) );
	PyTuple_SET_ITEM( ret, 1, VectorD_getinitargs( v ) );
	PyTuple_SET_ITEM( ret, 2, VectorD_getstate( v ) );
	return ret;
}

static PyMethodDef VectorD_methods[] = {
	{"__add__", VectorD_add, METH_O | METH_COEXIST,
	"Add two Vectors."},
	{"__sub__", VectorD_sub, METH_O | METH_COEXIST,
	"Subtract two Vectors."},
	{"__mul__", VectorD_mul, METH_O | METH_COEXIST,
	"Scale a Vector."},
	{"__div__", VectorD_div, METH_O | METH_COEXIST,
	"Scale a Vector."},
	// Pickle support
	{"__getinitargs__", (PyCFunction)VectorD_getinitargs, METH_NOARGS,
	"Get init arguments"},
	{"__getstate__", (PyCFunction)VectorD_getstate, METH_NOARGS,
	"Get internal state"},
	{"__setstate__", (PyCFunction)VectorD_setstate, METH_O,
	"Set internal state"},
	{"__reduce__", (PyCFunction)VectorD_reduce, METH_NOARGS,
	"Pickle support"},
	{NULL, NULL, 0, NULL}
};

static const char* docD_str =	"Create a VectorD object"
	"\n"
	"\nArguments:"
	"\nx       : x component"
	"\ny       : y component"
	"\nz       : (optional)z component"
	"\nw       : (optional)w component";

PyTypeObject VectorD_Type = {
	PyVarObject_HEAD_INIT( NULL, 0 )
	"geo2.VectorD",				/* tp_name */
	sizeof( VectorD_Object ),	/* tp_basicsize */
	0,                          /* tp_itemsize */
	0,							/* tp_dealloc */
	0,                          /* tp_print */
	VectorD_getattr,				/* tp_getattr */
	VectorD_setattr,             /* tp_setattr */
	0,                          /* tp_compare */
	(reprfunc)VectorD_repr,		/* tp_repr */
	&VectorD_as_number,			/* tp_as_number */
	&VectorD_as_sequence,       /* tp_as_sequence */
	0,                          /* tp_as_mapping */
	0,                          /* tp_hash */
	0,                          /* tp_call */
	0,                          /* tp_str */
	0,                          /* tp_getattro */
	0,                          /* tp_setattro */
	0,                          /* tp_as_buffer */
	Py_TPFLAGS_DEFAULT |        /* tp_flags */
	Py_TPFLAGS_BASETYPE,        /* Vector3 allows subclassing*/
	docD_str,	/* tp_doc */
	0,                          /* tp_traverse */
	0,                          /* tp_clear */
	0,                          /* tp_richcompare */
	0,                          /* tp_weaklistoffset */
	0,                          /* tp_iter */
	0,                          /* tp_iternext */
	VectorD_methods,				/* tp_methods */
	0,							/* tp_members */
	0,                          /* tp_getset */
	0,                          /* tp_base */
	0,                          /* tp_dict */
	0,                          /* tp_descr_get */
	0,                          /* tp_descr_set */
	0,                          /* tp_dictoffset */
	0,                          /* tp_init */
	0,                          /* tp_alloc */
	VectorD_new,					/* tp_new */
	0,                          /* tp_free */
};
