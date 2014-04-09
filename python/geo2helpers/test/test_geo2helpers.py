import unittest

import binbootstrapper
binbootstrapper.update_binaries(__file__, binbootstrapper.DLL_GEO2)

import testhelpers

from .. import geo2helpers

MATRIX_16 = [1.0, 0.0, 0.0, 0.0,
             0.0, 1.0, 0.0, 0.0,
             0.0, 0.0, 1.0, 0.0,
             0.0, 0.0, 0.0, 1.0]
MATRIX_4x4 = ([1.0, 0.0, 0.0, 0.0],
              [0.0, 1.0, 0.0, 0.0],
              [0.0, 0.0, 1.0, 0.0],
              [0.0, 0.0, 0.0, 1.0])

class TestGeo2(unittest.TestCase):
    """Tests. Our main test is just that it imports."""
    def testLerp(self):
        self.assertEqual(geo2helpers.Lerp(-1, 1, .5), 0)
        self.assertEqual(geo2helpers.Lerp(-2, -1, 0), -2)
        self.assertEqual(geo2helpers.Lerp(1, 2, 1), 2)

    def testCast16To4x4(self):
        result = geo2helpers.CastMatrix16To4x4(MATRIX_16)
        self.assertEqual(MATRIX_4x4, result)

    def testCast4x4to16(self):
        result = geo2helpers.CastMatrix4x4To16(MATRIX_4x4)
        self.assertEqual(MATRIX_16, result)

    def testMatrixResetScale(self):
        mat = ([0.5, 0.0, 0.0, 0.0],
               [0.0, 0.5, 0.0, 0.0],
               [0.0, 0.0, 0.5, 0.0],
               [0.0, 0.0, 0.0, 1.0])
        result = geo2helpers.MatrixResetScale(mat)

        # MatrixResetScale returns all tuples,
        # but MATRIX_4x4 is a tuple of lists.
        # So we convert the result.
        ideal = tuple(map(lambda e: tuple(e), MATRIX_4x4))

        testhelpers.assertEqualPretty(self, ideal, result)

    def testComputeBoundingBoxMinMax(self):
        verts = [
            (0.0, -10, 0.0),
            (0.0, 10, 0.0),
            (-10, 0, 0.0),
            (10, 0, 0.0),
            (0, 0, -10),
            (0, 0, 10)
        ]
        ideal = ((-10, -10, -10), (10, 10, 10))
        self.assertEqual(geo2helpers.ComputeBoundingBoxMinMax(verts), ideal)

    def testComputeBoundingBoxMinMaxGeoNotMirroredAcrossOrigin(self):
        verts = [
            (0.0, -4, 0.0),
            (0.0, 10, 0.0),
            (-10, 0, 0.0),
            (4, 0, 0.0),
            (0, 0, -4),
            (0, 0, 4)
        ]
        ideal = ((-10, -4, -4), (4, 10, 4))
        self.assertEqual(geo2helpers.ComputeBoundingBoxMinMax(verts), ideal)