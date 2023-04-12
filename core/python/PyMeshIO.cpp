#include <pybind11/pybind11.h>
#include "vc/core/io/MeshIO.hpp"

namespace py = pybind11;
namespace vc = volcart;

void init_MeshIO(py::module&);
vc::MeshReaderResult ReadMeshStr(std::string path);

vc::MeshReaderResult ReadMeshStr(std::string path) {
   const std::filesystem::path fs_path = path;
   return vc::ReadMesh(fs_path);
}



void init_MeshIO(py::module& m)
{
    py::module meshio = m.def_submodule("mesh_io", "Mesh reading and writing");
    
    /** Mesh Result **/
    py::class_<vc::MeshReaderResult> c(meshio, "MeshReaderResult");
    c.doc() = "Object containing return from mesh read";
    c.def_readwrite("mesh", &vc::MeshReaderResult::mesh);
    c.def_readwrite("uv", &vc::MeshReaderResult::uv);
    c.def_readwrite("texture", &vc::MeshReaderResult::texture);

    /** Mesh Read */
    meshio.def(
	"read_mesh",
	&ReadMeshStr,
	"Read in a mesh in either .ply or .obj format",
	py::arg("path")
    );
}

