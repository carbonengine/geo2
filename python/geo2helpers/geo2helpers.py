# Copyright © 2026 CCP ehf.
# Geo2 matrices are all 4x4.
import geo2


def MatrixResetScale(matrix):
    """Decompose matrix, reset scale and return recomposed matrix."""
    scale, rotation, position = geo2.MatrixDecompose(matrix)
    return geo2.MatrixTransformation(None, None, None, None, rotation, position)


def ComputeBoundingBoxMinMax(points):
    center, extends = geo2.ComputeBoundingBox(points)
    minBounds = geo2.Vec3Add(center, geo2.Vec3Negate(extends))
    maxBounds = geo2.Vec3Add(center, extends)
    return minBounds, maxBounds