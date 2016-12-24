// VC Texture
// Object to store texture information generated by the algorithms in
// vc_texturing
// Created by Seth Parker on 10/20/15.
#pragma once

#include <opencv2/opencv.hpp>

#include "core/types/Metadata.h"
#include "core/types/PerPixelMap.h"
#include "core/types/UVMap.h"
#include "core/vc_defines.h"

namespace volcart
{
class Texture
{
public:
    Texture();
    Texture(std::string path);

    // Get metadata
    volcart::Metadata metadata() const { return _metadata; }

    std::string id() const { return _metadata.get<std::string>("id"); };
    int width() const { return _width; };
    int height() const { return _height; };
    size_t numberOfImages() const { return _images.size(); };
    bool hasImages() const { return _images.size() > 0; };
    bool hasMap() const { return _ppm.initialized(); };

    // PPM
    void setPPM(PerPixelMap m) { _ppm = m; };
    const PerPixelMap& ppm() const { return _ppm; };
    PerPixelMap& ppm() { return _ppm; };

    // Get UV Map
    const volcart::UVMap& uvMap() const { return _ppm.uvMap(); };
    volcart::UVMap& uvMap() { return _ppm.uvMap(); };

    // Get/Add Texture Image
    cv::Mat image(int id) const { return _images[id]; };
    void addImage(cv::Mat image);

    // Return the intensity for a Point ID
    double intensity(int point_ID, int image_ID = 0);

    // Extra Metadata
    cv::Mat mask() { return _ppm.mask(); };

private:
    volcart::Metadata _metadata;
    boost::filesystem::path _path;
    int _width, _height;
    std::vector<cv::Mat> _images;
    PerPixelMap _ppm;
};
}  // namespace volcart
