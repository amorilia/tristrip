tristrip is a C++ library for stripifying triangles into strips.

This library is a clean and simple C++ port of RuneBlade's Python port
of NVidia's C++ triangle stripifier. The library handles arbitrary
geometry, including geometry with more than two faces per edge.

Note that the current implementation is not yet cache aware, because
that part of the code has not been yet ported. For now, consider this
library for demonstrative purposes only, unless you really look for an
implementation which aims at producing as long as possible strips.
