import os
from volcart import core
from volcart import meshing


DIR_PATH = os.path.dirname(os.path.realpath(__file__))

def test_mesher_pipeline():
    import pdb; pdb.set_trace()
    path = os.path.join(DIR_PATH, "../test/res/Testing.volpkg/paths/local-reslice-particle-sim/pointset.vcps")
    mode = core.IOMode.BINARY  # Or ASCII
    point_set = core.PointSetIO3d.read_ordered_point_set(path, mode)
    meshing.OrderedPointSetMesher(point_set)
    mesh_out = mesher.compute()

