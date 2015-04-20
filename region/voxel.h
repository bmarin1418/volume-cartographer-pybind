#ifndef _VOXEL_
#define _VOXEL_

#include <iostream>
#include <opencv2/opencv.hpp>

typedef cv::Vec3f Vector;

class Voxel {
 public:
  Voxel(Vector, Vector, float);
  void project();
  friend bool connector(Voxel, Voxel);
  Vector pos();
  Vector norm();
  float eig();
  friend bool operator<(const Voxel&, const Voxel&);
  friend Vector offset(Voxel, Voxel);
  friend std::ostream& operator<<(std::ostream& os, const Voxel& v);

 private:
  Vector position;
  Vector normal;
  Vector gravity;
  Vector slice_intersect;
  float eigen;
};

#endif // _VOXEL_
