# Copyright © 2021 CCP ehf.
import sys
import os

# This is a hack to allow PyCharm to parse stub files for _geo2. The _geo2_stub stub is located
# in packages/stubgen/stubs and will always generate an ImportError.
try:
    from _geo2_stub import *
except ImportError:
    pass

flavor = os.environ.get("BUILDFLAVOR", "release")

if flavor == 'release':
    import _geo2 as mod
elif flavor == 'debug':
    import _geo2_debug as mod
elif flavor == 'trinitydev':
    import _geo2_trinitydev as mod
elif flavor == 'internal':
    import _geo2_internal as mod
else:
    raise RuntimeError("Unknown build flavor: {}".format(flavor))

sys.modules[__name__] = mod
