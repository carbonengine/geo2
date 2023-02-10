#include "Vector.h"
#include "Util.h"
#ifndef _WIN32
#include <CcpSecureCrt.h>
#endif
#include <algorithm>
#include <limits>
#include <cstdint>

// Allocate memory for the python object
static PyObject* Vector_alloc( PyTypeObject *type, Vector* value, unsigned char size )
{
	PyObject *self;

	self = type->tp_alloc( type, 0 );
	if( self == NULL )
		return NULL;

	( ( Vector_Object* )self )->size = size;
	(( Vector_Object* )self )->value = *value;	
	return self;
}

// object.x == getattr( object, "x" )
// Here we resolve any member variables
PyObject* Vector_getattr( PyObject* self, char* attrname )
{ 
	Vector_Object *object = (Vector_Object *)self;
	int stringLength = (int)strlen( attrname );
	if( stringLength == 1 )
	{
		int index = ((attrname[0] - 'w')+3)%4;
		if( index < 0 || index >= object->size )
		{
			return PyObject_GenericGetAttr( self, PyString_FromString( attrname ) );
		}
		else
		{
			return PyFloat_FromDouble( object->value.u[index] );	
		}		
	}
	else if( stringLength < 5 )
	{
		Vector	vector;
		for( int i = 1; i < stringLength; i++ )
		{
			int index = ((attrname[i] - 'w')+3)%4;
			if( index < 0  || index  >= object->size )
			{
				// If none is found, forward the query to the generic attribute handler
				return PyObject_GenericGetAttr( self, PyString_FromString( attrname ) );
			}
			vector.u[i] = object->value.u[index];
		}
		return Vector_alloc( &Vector_Type, &vector, stringLength );
	}
	else
	{
		// If none is found, forward the query to the generic attribute handler
		return PyObject_GenericGetAttr( self, PyString_FromString( attrname ) );
	}	
}

// component assignment 
int Vector_setattr( PyObject* self, char* attrname, PyObject* value )
{ 
	// if someone calls del vector.x, value will be null. We don't allow that
	if ( value == NULL )
	{
		PyErr_SetString( PyExc_TypeError, "read-only attribute" );
		return -1;
	}

	int stringLength = (int)strlen( attrname );
	// Only check the first character
	if( stringLength == 1 )
	{
		Vector_Object *object = (Vector_Object *)self;
		int index = ((attrname[0] - 'w')+3)%4;
		if( index < 0 || index >= object->size )
		{
			return PyObject_GenericSetAttr( self, PyString_FromString( attrname ), value );
		}
		else
		{
			float fresult;				
			if ( !GetRealValue<float>( value, &fresult ) ) 
			{		
				PyErr_SetString( PyExc_TypeError, "a float is required" );
				return -1;
			}	
			object->value.u[index] = fresult;	
		}	
	}
	else
	{
		return PyObject_GenericSetAttr( self, PyString_FromString( attrname ), value );
	}
	return 0;
}

// For iterators
Py_ssize_t Vector_length( Vector_Object *a )
{
	return a->size;
}

// Get at the vector components by item index
PyObject* Vector_get_item( Vector_Object *a, Py_ssize_t i )
{
	if ( i < 0 || i >= a->size ) {
		PyErr_SetString( PyExc_IndexError, "vector index out of range" );
		return NULL;
	}

	return PyFloat_FromDouble( (a->value).u[(uint32_t)i] );
}

// Set a vector component by index
int Vector_set_item( Vector_Object *a, Py_ssize_t i, PyObject* value )
{
	if ( i < 0 || i >= a->size ) {
		PyErr_SetString( PyExc_IndexError, "vector index out of range" );
		return -1;
	}

	float val;
	if ( !GetRealValue<float>( value, &val ) ) 
	{		
		PyErr_SetString( PyExc_TypeError, "a float is required" );
		return -1;
	}	

	(a->value).u[(uint32_t)i] = val;

	return 0;
}

// Print a string representation of the object for inspection
PyObject* Vector_repr( Vector_Object *v )
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
	return PyString_FromString( buff );
}

PyObject* Vector_new( PyTypeObject *type, PyObject *args, PyObject *kwds )
{
	
	Vector vec = {0.0, 0.0, 0.0, 0.0};
	unsigned char size = 2;
	Py_ssize_t tupleSize = PyTuple_Size(args);
	if( tupleSize >= 2 )
	{		
		if( !PyArg_ParseTuple( args, "ff|ff", &vec.u[0], &vec.u[1], &vec.u[2], &vec.u[3] ) )
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
		size_t vecSize = ExtractFloatsFromSequence( p, vec, 2);
		if(vecSize == 0)
		{
			return NULL;
		}
		return Vector_alloc( type, &vec, (unsigned char)vecSize );
	}
	else
	{
		PyErr_SetString( PyExc_Exception, "geo2::Vector: Constructor needs arguments" );
		return NULL;
	}

	return Vector_alloc( type, &vec, (unsigned char)PySequence_Size( args ) );
}

// Addition
PyObject* Vector_add( PyObject *py_left, PyObject *py_right )
{	
	bool rightIsVec = Vector_Check( py_right );
	bool leftIsVec = Vector_Check( py_left );
	if( leftIsVec && rightIsVec ) 
	{
		unsigned char size = std::min( ( ( Vector_Object* ) py_left )->size,( ( Vector_Object* )py_right )->size );
		Vector result;
		for( int i = 0; i < size; i++ )
		{
			result.u[i] = (( Vector_Object* ) py_left )->value.u[i] + 
				(( Vector_Object* ) py_right )->value.u[i];
		}
		return Vector_alloc( &Vector_Type, &result, size );
	}
	else
	{
		// Support for adding a tuple to the vector...
		Vector a;
		size_t size;
		if( ( ExtractVectorFromSequence( py_left, 2, a, &size ) && rightIsVec ) || 
			( ExtractVectorFromSequence( py_right, 2, a, &size ) && leftIsVec ) )
		{
			PyObject* vector = rightIsVec ? py_right : py_left;
			unsigned char minSize = std::min( ( ( Vector_Object* ) vector )->size, (unsigned char)size );
			Vector result;
			for( int i = 0; i < minSize; i++ )
			{
				result.u[i] = a.u[i] + (( Vector_Object* ) vector )->value.u[i];
			}
			return Vector_alloc( &Vector_Type, &result, minSize );
		}
		else
		{
			Py_INCREF( Py_NotImplemented );
			return Py_NotImplemented;
		}
	}
}

// Subtraction
PyObject* Vector_sub( PyObject *py_left, PyObject *py_right )
{
	bool rightIsVec = Vector_Check( py_right );
	bool leftIsVec = Vector_Check( py_left );
	if( leftIsVec && rightIsVec ) 
	{
		unsigned char size = std::min( ( ( Vector_Object* ) py_left )->size,( ( Vector_Object* )py_right )->size );

		Vector result;
		for( int i = 0; i < size; i++ )
		{
			result.u[i] = (( Vector_Object* ) py_left )->value.u[i] - 
				(( Vector_Object* ) py_right )->value.u[i];
		}
		return Vector_alloc( &Vector_Type, &result, size );
	}
	else
	{
		// Support for subtracting a tuple from the vector...
		Vector a;
		size_t size;
		if( rightIsVec && ExtractVectorFromSequence( py_left, 2, a, &size ) )
		{
			unsigned char minSize = std::min( ( ( Vector_Object* ) py_right )->size, (unsigned char)size );
			Vector result;
			for( int i = 0; i < minSize; i++ )
			{
				result.u[i] = a.u[i] - (( Vector_Object* ) py_right )->value.u[i];
			}
			return Vector_alloc( &Vector_Type, &result, minSize );
		}
		else if(ExtractVectorFromSequence( py_right, 2, a, &size ))
		{
			unsigned char minSize = std::min( ( ( Vector_Object* ) py_left )->size, (unsigned char)size );
			Vector result;
			for( int i = 0; i < minSize; i++ )
			{
				result.u[i] = (( Vector_Object* ) py_left )->value.u[i] - a.u[i];
			}
			return Vector_alloc( &Vector_Type, &result, minSize );
		}
		else
		{
			Py_INCREF( Py_NotImplemented );
			return Py_NotImplemented;
		}
	}
}

// Multiplication
PyObject* Vector_mul( PyObject *py_left, PyObject *py_right )
{
	float scalar;
	bool rightIsVec = Vector_Check( py_right );
	bool leftIsVec = Vector_Check( py_left );
	if ( GetRealValue<float>( py_left, &scalar ) && rightIsVec ) 
	{
		unsigned char size = ( ( Vector_Object* )py_right )->size;
		Vector result;
		for( int i = 0; i < size; i++ )
		{
			result.u[i] = (( Vector_Object* ) py_right )->value.u[i] * scalar;
		}
		return Vector_alloc( &Vector_Type, &result, size );		
	}
	else if ( leftIsVec && GetRealValue<float>( py_right, &scalar ) ) 
	{		
		unsigned char size = ( ( Vector_Object* )py_left )->size;
		Vector result;
		for( int i = 0; i < size; i++ )
		{
			result.u[i] = (( Vector_Object* ) py_left )->value.u[i] * scalar;
		}
		return Vector_alloc( &Vector_Type, &result, size );	
	}
	else
	{
		Py_INCREF( Py_NotImplemented );
		return Py_NotImplemented;
	}
}

// Division
PyObject* Vector_div( PyObject *py_left, PyObject *py_right )
{
	float scalar;
	if( Vector_Check( py_left ) && GetRealValue<float>( py_right, &scalar ) )
	{
		if ( fabs( scalar ) < std::numeric_limits<float>::epsilon() )
		{
			PyErr_SetString( PyExc_Exception, "geo2::Vector: Can't divide by Zero" );
			return NULL;
		}
		scalar = 1.0f/scalar;
		unsigned char size = ( ( Vector_Object* )py_left )->size;
		Vector result;
		for( int i = 0; i < size; i++ )
		{
			result.u[i] = (( Vector_Object* ) py_left )->value.u[i] * scalar;
		}
		return Vector_alloc( &Vector_Type, &result, size );
	}
	else
	{
		Py_INCREF( Py_NotImplemented );
		return Py_NotImplemented;
	}
}

// Negation
PyObject* Vector_neg( Vector_Object *v )
{
	Vector result;
	for( int i = 0; i < v->size; i++ )
	{
		result.u[i] =  -v->value.u[i];
	}
	return Vector_alloc( &Vector_Type, &result, v->size );
}


static PyNumberMethods Vector_as_number = {
	Vector_add,			/* nb_add */
	Vector_sub,			/* nb_subtract */
	Vector_mul,			/* nb_multiply */
	Vector_div,			/* nb_divide */
	0,						/* nb_remainder */
	0,						/* nb_divmod */
	0,						/* nb_power */
	(unaryfunc)Vector_neg	/* nb_negative */
};

static PySequenceMethods Vector_as_sequence = {
	(lenfunc)Vector_length,			/* sq_length */
	0,
	0,
	(ssizeargfunc)Vector_get_item,		/* sq_item */
	0,
	(ssizeobjargproc)Vector_set_item,/* sq_ass_item */
	0,					/* sq_ass_slice */
	0,
};

// Pickle support
PyObject* Vector_getinitargs( Vector_Object *v )
{
	return VectorAsPyFloats( v->value, v->size );
}

PyObject* Vector_getstate( Vector_Object *v )
{
	return VectorAsPyFloats( v->value, v->size );
}

PyObject* Vector_setstate( Vector_Object *v, PyObject* state )
{
	if (!PyTuple_Check(state) || Py_SIZE(state) > 4 || Py_SIZE(state) < 2) {
		PyErr_Format(PyExc_TypeError,
			"%.200s.__setstate__ argument should be (2-4)-tuple, got %.200s",
			Py_TYPE(v)->tp_name, Py_TYPE(state)->tp_name);
		return NULL;
	}

	Vector a;
	if(!ExtractVectorFromSequence( state, a, 2 ))
	{
		return NULL;
	}

	v->value = a;
	v->size = (unsigned char)Py_SIZE( (PyObject*)state );
	Py_RETURN_NONE;
}

PyObject* Vector_reduce( Vector_Object *v )
{
	PyObject* ret = PyTuple_New( 3 );
	PyTuple_SET_ITEM( ret, 0, PyObject_GetAttrString( (PyObject*)v, "__class__" ) );	
	PyTuple_SET_ITEM( ret, 1, Vector_getinitargs( v ) );
	PyTuple_SET_ITEM( ret, 2, Vector_getstate( v ) );
	return ret;
}

static PyMethodDef Vector_methods[] = {
	{"__add__", Vector_add, METH_O | METH_COEXIST,
	"Add two Vectors."},
	{"__sub__", Vector_sub, METH_O | METH_COEXIST,
	"Subtract two Vectors."},
	{"__mul__", Vector_mul, METH_O | METH_COEXIST,
	"Scale a Vector."},
	{"__div__", Vector_div, METH_O | METH_COEXIST,
	"Scale a Vector."},
	// Pickle support
	{"__getinitargs__", (PyCFunction)Vector_getinitargs, METH_NOARGS,
	"Get init arguments"},
	{"__getstate__", (PyCFunction)Vector_getstate, METH_NOARGS,
	"Get internal state"},
	{"__setstate__", (PyCFunction)Vector_setstate, METH_O,
	"Set internal state"},
	{"__reduce__", (PyCFunction)Vector_reduce, METH_NOARGS,
	"Pickle support"},	
	{NULL, NULL, 0, NULL}
};

static const char* doc_str =	"Create a VectorD object"
	"\n"
	"\nArguments:"
	"\nx       : x component"
	"\ny       : y component"
	"\nz       : (optional)z component"
	"\nw       : (optional)w component";

PyTypeObject Vector_Type = {
	PyObject_HEAD_INIT( NULL )
	0,                          /* ob_size */
	"geo2.Vector",				/* tp_name */
	sizeof( Vector_Object ),	/* tp_basicsize */
	0,                          /* tp_itemsize */
	0,							/* tp_dealloc */
	0,                          /* tp_print */
	Vector_getattr,				/* tp_getattr */
	Vector_setattr,             /* tp_setattr */
	0,                          /* tp_compare */
	(reprfunc)Vector_repr,		/* tp_repr */
	&Vector_as_number,			/* tp_as_number */
	&Vector_as_sequence,       /* tp_as_sequence */
	0,                          /* tp_as_mapping */
	0,                          /* tp_hash */
	0,                          /* tp_call */
	0,                          /* tp_str */
	0,                          /* tp_getattro */
	0,                          /* tp_setattro */
	0,                          /* tp_as_buffer */
	Py_TPFLAGS_DEFAULT |        /* tp_flags */
	Py_TPFLAGS_CHECKTYPES |     /* PyNumberMethods do their own coercion */
	Py_TPFLAGS_BASETYPE,        /* Vector3 allows subclassing*/
	doc_str,	/* tp_doc */
	0,                          /* tp_traverse */
	0,                          /* tp_clear */
	0,                          /* tp_richcompare */
	0,                          /* tp_weaklistoffset */
	0,                          /* tp_iter */
	0,                          /* tp_iternext */
	Vector_methods,				/* tp_methods */
	0,							/* tp_members */
	0,                          /* tp_getset */
	0,                          /* tp_base */
	0,                          /* tp_dict */
	0,                          /* tp_descr_get */
	0,                          /* tp_descr_set */
	0,                          /* tp_dictoffset */
	0,                          /* tp_init */
	0,                          /* tp_alloc */
	Vector_new,					/* tp_new */
	0,                          /* tp_free */
};
