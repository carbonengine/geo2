# Copyright © 2014 CCP ehf.
import unittest

from geo2helpers import geo2helpers

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

    def testMatrixResetScale(self):
        mat = ([0.5, 0.0, 0.0, 0.0],
               [0.0, 0.5, 0.0, 0.0],
               [0.0, 0.0, 0.5, 0.0],
               [0.0, 0.0, 0.0, 1.0])
        result = geo2helpers.MatrixResetScale(mat)

        # MatrixResetScale returns all tuples,
        # but MATRIX_4x4 is a tuple of lists.
        # So we convert the result.
        ideal = tuple([tuple(e) for e in MATRIX_4x4])

        self.assertEqual(ideal, result)

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