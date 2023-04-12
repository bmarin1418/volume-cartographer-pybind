

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <opencv2/core.hpp>
#include "vc/core/io/PointSetIO.hpp"

namespace py = pybind11;
namespace vc = volcart;

template<class T> void declare_class(pybind11::module&, std::string);
void init_PointSetIO(py::module&);

template<typename T>
void declare_class(py::module &m, std::string namestr) {
    using Class = vc::PointSetIO<T>;
    std::string pyclass_name = std::string("PointSetIO") + namestr;
    py::class_<Class>(m, pyclass_name.c_str())
        .def_static("read_ordered_point_set", [](std::string path, vc::IOMode mode) {
   		const std::filesystem::path fs_path = path;
		return Class::ReadOrderedPointSet(fs_path, mode);
	})
        .def_static("read_point_set", [](std::string path, vc::IOMode mode) {
   		const std::filesystem::path fs_path = path;
		return Class::ReadPointSet(fs_path, mode);
	})
        .def_static("write_ordered_point_set", &Class::WriteOrderedPointSet)
        .def_static("write_point_set", &Class::WritePointSet)
        .def_static("make_header", &Class::MakeHeader)
	.def_static("make_ordered_header", &Class::MakeOrderedHeader);
}

void init_PointSetIO(py::module& m) {
    
    // Define the IOMode enum class
    py::enum_<vc::IOMode>(m, "IOMode")
        .value("ASCII", vc::IOMode::ASCII)
        .value("BINARY", vc::IOMode::BINARY);

    // Define the PointSetIO class for a couple types
    declare_class<cv::Vec3i>(m, "3i");
    declare_class<cv::Vec3d>(m, "3d");
    declare_class<cv::Vec6d>(m, "6d");
}
