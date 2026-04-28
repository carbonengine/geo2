# Copyright © 2026 CCP ehf.
import unittest

import geo2helpers

from geo2helpers.test import MATRIX_16, MATRIX_4x4


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

