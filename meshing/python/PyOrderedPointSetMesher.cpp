#include <pybind11/pybind11.h>
#include <opencv2/core.hpp>
#include "vc/meshing/OrderedPointSetMesher.hpp"

namespace py = pybind11;
namespace vc = volcart;
//using namespace volcart::meshing;

void init_OrderedPointSetMesher(py::module&);
void init_OrderedPointSetMesher(py::module& m) {
   using PointSet = vc::OrderedPointSet<cv::Vec3d>;
   py::class_<vc::meshing::OrderedPointSetMesher>(m, "OrderedPointSetMesher")
        .def(py::init<>())
	.def(py::init<PointSet>())
        .def(py::init<PointSet>())
	.def("compute", &vc::meshing::OrderedPointSetMesher::compute)
	.def("get_output_mesh", &vc::meshing::OrderedPointSetMesher::getOutputMesh)
	.def("set_compute_triangulation", &vc::meshing::OrderedPointSetMesher::setComputeTriangulation);
}
