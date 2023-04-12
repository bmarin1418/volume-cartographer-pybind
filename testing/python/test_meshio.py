import os 

from volcart.core import mesh_io

DIR_PATH = os.path.dirname(os.path.realpath(__file__))

def test_mesh_read():
   mesh_obj = mesh_io.read_mesh(os.path.join(DIR_PATH, "../test/res/OBJWriterPlaneData.obj"))
