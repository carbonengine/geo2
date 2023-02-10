import unittest

import sys
sys.path.append(r"..\..\common\script\util")
sys.path.append(r"..\..\autobuild\geo2\python_27\win32")
import Vector3
import geo2
import math

class TestVector3(object):
    def test__init__(self):
        v = self.vector3(1.0, 2.0, 3.0)
        self.assertEqual(v.x, 1.0)
        self.assertEqual(v.y, 2.0)
        self.assertEqual(v.z, 3.0)

    def test__getstate__(self):
        v = self.vector3(1.0, 2.0, 3.0)
        self.assertEqual(v.__getstate__(), (1.0, 2.0, 3.0))

    def test__setstate__(self):
        v = self.vector3(1.0, 2.0, 3.0)
        v.__setstate__((3.0, 4.0, 5.0))
        self.assertEqual(v.__getstate__(), (3.0, 4.0, 5.0))

    def test__setattr__(self):
        v = self.vector3(1.0, 2.0, 3.0)
        v.x = 3.0
        v.y = 4.0
        v.z = 5.0
        self.assertEqual(v.x, 3.0)
        self.assertEqual(v.y, 4.0)
        self.assertEqual(v.z, 5.0)

    def test__len__(self):
        v = self.vector3(1.0, 2.0, 3.0)
        self.assertEqual(len(v), 3)

    def test__getitem__(self):
        v = self.vector3(1.0, 2.0, 3.0)
        self.assertEqual(v[0], 1.0)
        self.assertEqual(v[1], 2.0)
        self.assertEqual(v[2], 3.0)

    def test__setitem__(self):
        v = self.vector3(1.0, 2.0, 3.0)
        v[0] = 4.0
        v[1] = 5.0
        v[2] = 6.0
        self.assertEqual(v[0], 4.0)
        self.assertEqual(v[1], 5.0)
        self.assertEqual(v[2], 6.0)

    def test__add__(self):
        a = self.vector3(1.0, 2.0, 3.0)
        b = self.vector3(2.0, 3.0, 4.0)
        c = a+b
        self.assertEqual(c.x, 3.0)
        self.assertEqual(c.y, 5.0)
        self.assertEqual(c.z, 7.0)

    def test__sub__(self):
        a = self.vector3(1.0, 2.0, 3.0)
        b = self.vector3(2.0, 3.0, 4.0)
        c = a-b
        self.assertEqual(c.x, -1.0)
        self.assertEqual(c.y, -1.0)
        self.assertEqual(c.z, -1.0)

    def test__div__(self):
        a = self.vector3(1.0, 2.0, 3.0)
        c = a/2.0
        self.assertEqual(c.x, 0.5)
        self.assertEqual(c.y, 1.0)
        self.assertEqual(c.z, 1.5)

    def test__mul__vs(self):
        a = self.vector3(1.0, 2.0, 3.0)
        a = a*2.0
        self.assertEqual(a.x, 2.0)
        self.assertEqual(a.y, 4.0)
        self.assertEqual(a.z, 6.0)

    def test__mul__vv(self):
        a = self.vector3(1.0, 2.0, 3.0)
        b = self.vector3(2.0, 3.0, 4.0)
        dot = a*b
        self.assertEqual(dot, 20.0)

    def test__rmul__sv(self):
        a = self.vector3(1.0, 2.0, 3.0)
        a = 2.0*a
        self.assertEqual(a.x, 2.0)
        self.assertEqual(a.y, 4.0)
        self.assertEqual(a.z, 6.0)

    def test__xor__(self):
        a = self.vector3(1.0, 2.0, 3.0)
        b = self.vector3(2.0, 3.0, 4.0)
        c = a^b
        self.assertEqual(c.x, -1.0)
        self.assertEqual(c.y, 2.0)
        self.assertEqual(c.z, -1.0)

    def test__neg__(self):
        a = self.vector3(1.0, 2.0, 3.0)
        a = -a
        self.assertEqual(a.x, -1.0)
        self.assertEqual(a.y, -2.0)
        self.assertEqual(a.z, -3.0)

    def test__abs__(self):
        v = self.vector3(1.0, 2.0, 3.0)
        self.assertAlmostEqual(abs(v), 3.7416573867739413, 11)

    def testRandomize(self):
        # Testing random function is problematic. But I did make sure that
        # both the c and python version produce the same results when 
        # fed the same constants in place or random values
        pass

class TestPythonVector3(unittest.TestCase,TestVector3):
    vector3 = Vector3.Vector3

    def testNormalize(self):
        v = self.vector3(1.0, 2.0, 3.0)
        v.Normalize()
        self.assertEqual(v[0], 0.2672612419124244)
        self.assertEqual(v[1], 0.5345224838248488)
        self.assertEqual(v[2], 0.8017837257372732)

    def testLength(self):
        v = self.vector3(1.0, 2.0, 3.0)
        self.assertAlmostEqual(v.Length(), 3.7416573867739413, 11)

    def testLength2(self):
        v = self.vector3(1.0, 2.0, 3.0)
        self.assertEqual(v.Length2(), 14.0)

    def testUnit(self):
        v = self.vector3(1.0, 2.0, 3.0)
        v = v.Unit()
        self.assertEqual(v[0], 0.2672612419124244)
        self.assertEqual(v[1], 0.5345224838248488)
        self.assertEqual(v[2], 0.8017837257372732)

    def test__eq__(self):
        a = self.vector3(1.0, 2.0, 3.0)
        b = self.vector3(1.0, 2.0, 3.0)
        self.assertEqual(a==b, True)

    def test__ne__(self):
        a = self.vector3(1.0, 2.0, 3.0)
        b = self.vector3(2.0, 3.0, 3.0)
        self.assertEqual(a!=b, True)

    def testAsTuple(self):
        a = self.vector3(1.0, 2.0, 3.0)
        t = a.AsTuple()
        self.assertEqual(t, (1.0, 2.0, 3.0))

class TestCVector3(unittest.TestCase, TestVector3):
    vector3 = geo2.VectorD

    def testNormalize(self):
        v = (1.0, 2.0, 3.0)
        n = geo2.Vec3NormalizeD(v)
        self.assertEqual(n[0], 0.2672612419124244)
        self.assertEqual(n[1], 0.5345224838248488)
        self.assertEqual(n[2], 0.8017837257372732)

    def testLength(self):
        v = self.vector3(1.0, 2.0, 3.0)
        self.assertAlmostEqual(geo2.Vec3LengthD(v), 3.7416573867739413, 11)

    def testLength2(self):
        v = self.vector3(1.0, 2.0, 3.0)
        self.assertEqual(geo2.Vec3LengthSqD(v), 14.0)

    def testUnit(self):
        v = (1.0, 2.0, 3.0)
        n = geo2.Vec3NormalizeD(v)
        self.assertEqual(n[0], 0.2672612419124244)
        self.assertEqual(n[1], 0.5345224838248488)
        self.assertEqual(n[2], 0.8017837257372732)

    def test__eq__(self):
        a = self.vector3(1.0, 2.0, 3.0)
        b = self.vector3(1.0, 2.0, 3.0)
        self.assertEqual(geo2.Vec3EqualD(a,b), True)

    def test__ne__(self):
        a = self.vector3(1.0, 2.0, 3.0)
        b = self.vector3(2.0, 3.0, 3.0)
        self.assertEqual(not geo2.Vec3EqualD(a,b), True)

    def testAsTuple(self):
        a = self.vector3(1.0, 2.0, 3.0)
        t = tuple(a)
        self.assertEqual(t, (1.0, 2.0, 3.0))

unittest.main()