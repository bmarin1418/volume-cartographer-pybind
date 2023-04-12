#include <pybind11/pybind11.h>

namespace py = pybind11;
void init_OrderedPointSetMesher(py::module&);

PYBIND11_MODULE(meshing, m)
{
    m.doc() = "Library containing VC meshing functionality";

    // init types
    init_OrderedPointSetMesher(m);
}
