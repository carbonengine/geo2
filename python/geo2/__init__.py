import blue
import sys

# This is a hack to allow PyCharm to parse stub files for _geo2. The _geo2_stub stub is located
# in packages/stubgen/stubs and will always generate an ImportError.
try:
    from _geo2_stub import *
except ImportError:
    pass

sys.modules[__name__] = blue.LoadExtension("_geo2")
