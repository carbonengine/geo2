"""Due to issues in testing and how non-pythonic geo2 is we have to create this
sub-module. If we were to keep this in __init__.py then the
binbootstrapper wouldn't run correctly and geo2 can't be imported.

Geo2 matrices are all 4x4."""
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