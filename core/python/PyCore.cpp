#include <pybind11/pybind11.h>

namespace py = pybind11;

void init_PerPixelMap(py::module&);
void init_Reslice(py::module&);
void init_Volume(py::module&);
void init_VolumePkg(py::module&);
void init_MeshIO(py::module&);
void init_PointSetIO(py::module&);

PYBIND11_MODULE(core, m)
{
    m.doc() = "Library containing fundamental VC data types and operations.";

    // init types
    init_PerPixelMap(m);
    init_Reslice(m);
    init_Volume(m);
    init_VolumePkg(m);
    init_MeshIO(m);
    init_PointSetIO(m);
}
