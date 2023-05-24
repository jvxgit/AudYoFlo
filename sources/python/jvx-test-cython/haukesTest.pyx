cimport cython
import numpy as np
cimport numpy as npc
DTYPE = np.float64
ctypedef npc.float_t DTYPE_t

from libc.stdio cimport printf
from cython.view cimport array
from libc.math cimport sqrt, sin, cos, atan2, acos, hypot, isnan, NAN, pi

print("Hello My Code")

# utilities for empty array initialization
cdef inline double[:] _empty1(int n) noexcept:
    return array(shape=(n,), itemsize=sizeof(double), format=b"d")
cdef inline double[:, :] _empty2(int n1, int n2) noexcept :
    return array(shape=(n1, n2), itemsize=sizeof(double), format=b"d")
cdef inline double[:, :, :] _empty3(int n1, int n2, int n3) noexcept:
    return array(shape=(n1, n2, n3), itemsize=sizeof(double), format=b"d")
cdef inline double[:, :] _zeros2(int n1, int n2) noexcept:
    cdef double[:, :] arr = array(shape=(n1, n2),
        itemsize=sizeof(double), format=b"d")
    arr[:, :] = 0
    return arr

# flat implementations of numpy functions
@cython.boundscheck(False)
@cython.wraparound(False)
cdef inline double[:] _cross3(const double[:] a, const double[:] b) noexcept:
    cdef double[:] result = _empty1(3)
    result[0] = a[1]*b[2] - a[2]*b[1]
    result[1] = a[2]*b[0] - a[0]*b[2]
    result[2] = a[0]*b[1] - a[1]*b[0]
    return result

@cython.boundscheck(False)
@cython.wraparound(False)
cdef inline double _dot3(const double[:] a, const double[:] b) noexcept nogil:
    return a[0]*b[0] + a[1]*b[1] + a[2]*b[2]

@cython.boundscheck(False)
@cython.wraparound(False)
cdef inline double _norm3(const double[:] elems) noexcept nogil:
    return sqrt(_dot3(elems, elems))

@cython.boundscheck(False)
def method(
      npc.ndarray[DTYPE_t] inArg):
   print("Hello My Code 2")
   outVal = inArg * 2;
   print(outVal)
   return outVal
   