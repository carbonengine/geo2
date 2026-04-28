# Copyright © 2026 CCP ehf.
# Code in __init__.py is free from the geo2 dependency.
# Bit odd to have a geo2helpers package free of geo2 dependency,
# maybe we should've been named "mathhelpers" or something..?
import itertoolsext


def Lerp(minVal, maxVal, term):
    return (maxVal - minVal) * term + minVal


def CastMatrix16To4x4(matrix16):
    return matrix16[:4], matrix16[4:8], matrix16[8:12], matrix16[12:]


def CastMatrix4x4To16(matrix4x4):
    return list(itertoolsext.flatmap(lambda e: e, matrix4x4))


