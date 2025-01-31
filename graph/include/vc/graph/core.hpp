#pragma once

/** @file */

#include <opencv2/core.hpp>
#include <smgl/Node.hpp>

#include "vc/core/filesystem.hpp"
#include "vc/core/io/MeshIO.hpp"
#include "vc/core/types/ITKMesh.hpp"
#include "vc/core/types/PerPixelMap.hpp"
#include "vc/core/types/Segmentation.hpp"
#include "vc/core/types/UVMap.hpp"
#include "vc/core/types/Volume.hpp"
#include "vc/core/types/VolumePkg.hpp"
#include "vc/core/types/VolumetricMask.hpp"

namespace volcart
{

/**
 * @brief Load a VolumePkg from disk
 *
 * @ingroup Graph
 */
class LoadVolumePkgNode : public smgl::Node
{
private:
    /** Path */
    filesystem::path path_{};
    /** Loaded VolPkg */
    VolumePkg::Pointer vpkg_{nullptr};

public:
    /** @brief VolPkg path */
    smgl::InputPort<filesystem::path> path;
    /** @brief Loaded VolPkg */
    smgl::OutputPort<VolumePkg::Pointer> volpkg;

    /** Constructor */
    LoadVolumePkgNode();

private:
    /** Smeagol custom serialization */
    auto serialize_(bool /*useCache*/, const filesystem::path& /*cacheDir*/)
        -> smgl::Metadata override;

    /** Smeagol custom deserialization */
    void deserialize_(
        const smgl::Metadata& meta,
        const filesystem::path& /*cacheDir*/) override;
};

/**
 * @brief Retrieve VolumePkg properties (e.g. name, version, etc.)
 *
 * @ingroup Graph
 */
class VolumePkgPropertiesNode : public smgl::Node
{
private:
    /** VolPkg */
    VolumePkg::Pointer vpkg_{nullptr};

public:
    /** @brief Input VolumePkg */
    smgl::InputPort<VolumePkg::Pointer> volpkg;
    /** @copydoc VolumePkg::name() */
    smgl::OutputPort<std::string> name;
    /** @copydoc VolumePkg::version() */
    smgl::OutputPort<int> version;
    /** @copydoc VolumePkg::materialThickness() */
    smgl::OutputPort<double> materialThickness;

    /** Constructor */
    VolumePkgPropertiesNode();
};

/**
 * @copydoc VolumePkg::volume(const Volume::Identifier&) const
 *
 * @ingroup Graph
 */
class VolumeSelectorNode : public smgl::Node
{
private:
    /** VolumePkg */
    VolumePkg::Pointer vpkg_{nullptr};
    /** Volume UUID */
    Volume::Identifier id_{};
    /** Volume */
    Volume::Pointer vol_{nullptr};

public:
    /** @brief Input VolumePkg */
    smgl::InputPort<VolumePkg::Pointer> volpkg;
    /** @brief Volume UUID */
    smgl::InputPort<Volume::Identifier> id;
    /** @brief Volume */
    smgl::OutputPort<Volume::Pointer> volume;

    /** Constructor */
    VolumeSelectorNode();

private:
    /** Smeagol custom serialization */
    auto serialize_(bool /*useCache*/, const filesystem::path& /*cacheDir*/)
        -> smgl::Metadata override;

    /** Smeagol custom deserialization */
    void deserialize_(
        const smgl::Metadata& meta,
        const filesystem::path& /*cacheDir*/) override;
};

/**
 * @brief Set/Retrieve Volume properties (voxel size, bounds, etc.)
 *
 * @ingroup Graph
 */
class VolumePropertiesNode : public smgl::Node
{
private:
    /** Input volume */
    Volume::Pointer volume_{nullptr};
    /** Volume cache size (in bytes) */
    std::size_t cacheMem_{2'000'000'000};

public:
    /** @brief Input Volume */
    smgl::InputPort<Volume::Pointer> volumeIn;
    /** @copydoc Volume::setCacheMemoryInBytes */
    smgl::InputPort<std::size_t> cacheMemory;

    /** @copydoc Volume::bounds */
    smgl::OutputPort<Volume::Bounds> bounds;
    /** @copydoc Volume::voxelSize */
    smgl::OutputPort<double> voxelSize;
    /**
     * @brief Output Volume
     *
     * @warning After update, volumeIn and volumeOut are pointers to the same
     * Volume object.
     */
    smgl::OutputPort<Volume::Pointer> volumeOut;

    /** Constructor */
    VolumePropertiesNode();

private:
    /** Smeagol custom serialization */
    auto serialize_(bool /*useCache*/, const filesystem::path& /*cacheDir*/)
        -> smgl::Metadata override;

    /** Smeagol custom deserialization */
    void deserialize_(
        const smgl::Metadata& meta,
        const filesystem::path& /*cacheDir*/) override;
};

/**
 * @copydoc VolumePkg::segmentation(const Segmentation::Identifier&) const
 *
 * @ingroup Graph
 */
class SegmentationSelectorNode : public smgl::Node
{
private:
    /** VolumePkg */
    VolumePkg::Pointer vpkg_{nullptr};
    /** Segmentation UUID */
    Segmentation::Identifier id_{};
    /** Segmentation */
    Segmentation::Pointer seg_{nullptr};

public:
    /** @brief Input VolumePkg */
    smgl::InputPort<VolumePkg::Pointer> volpkg;
    /** @brief Segmentation UUID */
    smgl::InputPort<Segmentation::Identifier> id;
    /** @brief Segmentation */
    smgl::OutputPort<Segmentation::Pointer> segmentation;

    /** Constructor */
    SegmentationSelectorNode();

private:
    /** Smeagol custom serialization */
    auto serialize_(bool /*useCache*/, const filesystem::path& /*cacheDir*/)
        -> smgl::Metadata override;

    /** Smeagol custom deserialization */
    void deserialize_(
        const smgl::Metadata& meta,
        const filesystem::path& /*cacheDir*/) override;
};

/**
 * @brief Retrieve Segmentation properties (e.g. pointset)
 *
 * @ingroup Graph
 */
class SegmentationPropertiesNode : public smgl::Node
{
    /** Input segmentation */
    Segmentation::Pointer seg_{nullptr};

public:
    /** @brief Input segmentation */
    smgl::InputPort<Segmentation::Pointer> segmentation;
    /** @copydoc Segmentation::getPointSet() */
    smgl::OutputPort<Segmentation::PointSet> pointSet;

    /** Constructor */
    SegmentationPropertiesNode();
};

/**
 * @brief Retrieve Mesh properties (e.g. number of vertices, number of faces,
 * etc.)
 *
 * @ingroup Graph
 */
class MeshPropertiesNode : public smgl::Node
{
private:
    /** Input mesh */
    ITKMesh::Pointer mesh_{nullptr};

public:
    /** @brief Input mesh */
    smgl::InputPort<ITKMesh::Pointer> mesh{&mesh_};
    /** @brief Number of vertices in the mesh */
    smgl::OutputPort<std::size_t> numVertices;
    /** @brief Number of faces in the mesh */
    smgl::OutputPort<std::size_t> numFaces;

    /** Constructor */
    MeshPropertiesNode();
};

/**
 * @copybrief ReadMesh()
 *
 * @see ReadMesh()
 * @ingroup Graph
 */
class LoadMeshNode : public smgl::Node
{
private:
    /** File path */
    filesystem::path path_{};
    /** Include the loaded file in the graph cache */
    bool cacheArgs_{false};
    /** Loaded file(s) */
    MeshReaderResult loaded_{};

public:
    /** @brief Input path */
    smgl::InputPort<filesystem::path> path;
    /** @brief Include the loaded file in the graph cache */
    smgl::InputPort<bool> cacheArgs;
    /** @brief Loaded mesh */
    smgl::OutputPort<ITKMesh::Pointer> mesh;
    /** @brief Loaded UV map */
    smgl::OutputPort<UVMap::Pointer> uvMap;
    /** @brief Loaded texture image */
    smgl::OutputPort<cv::Mat> texture;

    /** Constructor */
    LoadMeshNode();

private:
    /** Smeagol custom serialization */
    auto serialize_(bool useCache, const filesystem::path& cacheDir)
        -> smgl::Metadata override;

    /** Smeagol custom deserialization */
    void deserialize_(
        const smgl::Metadata& meta,
        const filesystem::path& /*cacheDir*/) override;
};

/**
 * @copybrief WriteMesh()
 *
 * @see WriteMesh()
 * @ingroup Graph
 */
class WriteMeshNode : public smgl::Node
{
private:
    /** File path */
    filesystem::path path_{};
    /** Mesh */
    ITKMesh::Pointer mesh_{nullptr};
    /** UV map */
    UVMap::Pointer uv_{};
    /** Texture image */
    cv::Mat texture_{};
    /** Include the saved file in the graph cache */
    bool cacheArgs_{false};

public:
    /** @brief Output file */
    smgl::InputPort<filesystem::path> path;
    /** @brief Mesh */
    smgl::InputPort<ITKMesh::Pointer> mesh;
    /** @brief UV map */
    smgl::InputPort<UVMap::Pointer> uvMap;
    /** @brief Texture image */
    smgl::InputPort<cv::Mat> texture;
    /** @brief Include the saved file in the graph cache */
    smgl::InputPort<bool> cacheArgs;

    /** Constructor */
    WriteMeshNode();

private:
    /** Smeagol custom serialization */
    auto serialize_(bool useCache, const filesystem::path& cacheDir)
        -> smgl::Metadata override;

    /** Smeagol custom deserialization */
    void deserialize_(
        const smgl::Metadata& meta,
        const filesystem::path& /*cacheDir*/) override;
};

/**
 * @copybrief UVMap::Rotate()
 *
 * The original UVMap is not modified by this operation. Unlike the static
 * function, this node accepts input angles in degrees.
 *
 * @see UVMap::Rotate()
 * @ingroup Graph
 */
class RotateUVMapNode : public smgl::Node
{
private:
    /** Input UVMap */
    UVMap::Pointer uvMapIn_;
    /** Rotation angle (degrees) */
    double theta_{0};
    /** Output UVMap */
    UVMap::Pointer uvMapOut_;

public:
    /** @brief Input UVMap */
    smgl::InputPort<UVMap::Pointer> uvMapIn;
    /** @brief Rotation angle, in degrees */
    smgl::InputPort<double> theta;
    /** @brief Output UVMap */
    smgl::OutputPort<UVMap::Pointer> uvMapOut;

    /** Constructor */
    RotateUVMapNode();

private:
    /** Smeagol custom serialization */
    auto serialize_(bool useCache, const filesystem::path& cacheDir)
        -> smgl::Metadata override;

    /** Smeagol custom deserialization */
    void deserialize_(
        const smgl::Metadata& meta, const filesystem::path& cacheDir) override;
};

/**
 * @copybrief UVMap::Flip()
 *
 * The input UVMap is not modified by this operation.
 *
 * @ingroup Graph
 */
class FlipUVMapNode : public smgl::Node
{
private:
    /** Input UVMap */
    UVMap::Pointer uvMapIn_;
    /** Flip axis */
    UVMap::FlipAxis axis_{UVMap::FlipAxis::Vertical};
    /** Output UVMap */
    UVMap::Pointer uvMapOut_;

public:
    /** @copydoc UVMap::FlipAxis */
    using FlipAxis = UVMap::FlipAxis;
    /** @brief Input UVMap */
    smgl::InputPort<UVMap::Pointer> uvMapIn;
    /** @brief Flip axis */
    smgl::InputPort<FlipAxis> flipAxis;
    /** @brief Output UVMap */
    smgl::OutputPort<UVMap::Pointer> uvMapOut;

    /** Constructor */
    FlipUVMapNode();

private:
    /** Smeagol custom serialization */
    auto serialize_(bool useCache, const filesystem::path& cacheDir)
        -> smgl::Metadata override;

    /** Smeagol custom deserialization */
    void deserialize_(
        const smgl::Metadata& meta, const filesystem::path& cacheDir) override;
};

/**
 * @copybrief UVMap::Plot()
 *
 * @see UVMap::Plot()
 * @ingroup Graph
 */
class PlotUVMapNode : public smgl::Node
{
private:
    /** Input UVMap */
    UVMap::Pointer uvMap_;
    /** Input mesh */
    ITKMesh::Pointer uvMesh_;
    /** Output image */
    cv::Mat plot_;

public:
    /** @brief Input UVMap */
    smgl::InputPort<UVMap::Pointer> uvMap;
    /** @brief Input mesh */
    smgl::InputPort<ITKMesh::Pointer> uvMesh;
    /** @brief Plot image */
    smgl::OutputPort<cv::Mat> plot;

    /** Constructor */
    PlotUVMapNode();

private:
    /** Smeagol custom serialization */
    auto serialize_(bool useCache, const filesystem::path& cacheDir)
        -> smgl::Metadata override;

    /** Smeagol custom deserialization */
    void deserialize_(
        const smgl::Metadata& meta, const filesystem::path& cacheDir) override;
};

/**
 * @copybrief ReadImage()
 *
 * @see ReadImage()
 * @ingroup Graph
 */
class LoadImageNode : public smgl::Node
{
private:
    /** File path */
    filesystem::path path_{};
    /** Include the loaded file in the graph cache */
    bool cacheArgs_{false};
    /** Loaded image */
    cv::Mat image_{};

public:
    /** @brief Input path */
    smgl::InputPort<filesystem::path> path;
    /** @brief Include the loaded file in the graph cache */
    smgl::InputPort<bool> cacheArgs;
    /** @brief Loaded image */
    smgl::OutputPort<cv::Mat> image;

    /** Constructor */
    LoadImageNode();

private:
    /** Smeagol custom serialization */
    auto serialize_(bool useCache, const filesystem::path& cacheDir)
        -> smgl::Metadata override;

    /** Smeagol custom deserialization */
    void deserialize_(
        const smgl::Metadata& meta,
        const filesystem::path& /*cacheDir*/) override;
};

/**
 * @copybrief WriteImage()
 *
 * @see WriteImage()
 * @ingroup Graph
 */
class WriteImageNode : public smgl::Node
{
private:
    /** File path */
    filesystem::path path_{};
    /** Image */
    cv::Mat image_{};
    /** Include the saved file in the graph cache */
    bool cacheArgs_{false};

public:
    /** @brief Output file */
    smgl::InputPort<filesystem::path> path;
    /** @brief Output image */
    smgl::InputPort<cv::Mat> image;
    /** @brief Include the saved file in the graph cache */
    smgl::InputPort<bool> cacheArgs;

    /** Constructor */
    WriteImageNode();

private:
    /** Smeagol custom serialization */
    auto serialize_(bool useCache, const filesystem::path& cacheDir)
        -> smgl::Metadata override;

    /** Smeagol custom deserialization */
    void deserialize_(
        const smgl::Metadata& meta,
        const filesystem::path& /*cacheDir*/) override;
};

/**
 * @copybrief PerPixelMap::ReadPPM()
 *
 * @see PerPixelMap::ReadPPM()
 * @ingroup Graph
 */
class LoadPPMNode : public smgl::Node
{
private:
    /** File path */
    filesystem::path path_{};
    /** Include the loaded file in the graph cache */
    bool cacheArgs_{false};
    /** Loaded PPM */
    PerPixelMap::Pointer ppm_{};

public:
    /** @brief Input path */
    smgl::InputPort<filesystem::path> path;
    /** @brief Include the loaded file in the graph cache */
    smgl::InputPort<bool> cacheArgs;
    /** @brief Loaded PerPixelMap */
    smgl::OutputPort<PerPixelMap::Pointer> ppm;

    /** Constructor */
    LoadPPMNode();

private:
    /** Smeagol custom serialization */
    auto serialize_(bool useCache, const filesystem::path& cacheDir)
        -> smgl::Metadata override;

    /** Smeagol custom deserialization */
    void deserialize_(
        const smgl::Metadata& meta,
        const filesystem::path& /*cacheDir*/) override;
};

/**
 * @copybrief PerPixelMap::WritePPM()
 *
 * @see PerPixelMap::WritePPM()
 * @ingroup Graph
 */
class WritePPMNode : public smgl::Node
{
private:
    /** File path */
    filesystem::path path_{};
    /** PPM */
    PerPixelMap::Pointer ppm_{};
    /** Include the saved file in the graph cache */
    bool cacheArgs_{false};

public:
    /** @brief Output file */
    smgl::InputPort<filesystem::path> path;
    /** @brief Output PerPixelMap */
    smgl::InputPort<PerPixelMap::Pointer> ppm;
    /** @brief Include the saved file in the graph cache */
    smgl::InputPort<bool> cacheArgs;

    /** Constructor */
    WritePPMNode();

private:
    /** Smeagol custom serialization */
    auto serialize_(bool useCache, const filesystem::path& cacheDir)
        -> smgl::Metadata override;

    /** Smeagol custom deserialization */
    void deserialize_(
        const smgl::Metadata& meta,
        const filesystem::path& /*cacheDir*/) override;
};

/** @brief Retrieve PerPixelMap properties (e.g. mask, cell map, etc.)
 *
 * @ingroup Graph
 */
class PPMPropertiesNode : public smgl::Node
{
private:
    /** Input PPM */
    PerPixelMap::Pointer ppm_;

public:
    /** @brief Input PerPixelMap */
    smgl::InputPort<PerPixelMap::Pointer> ppm;
    /**
     * @copybrief PerPixelMap::mask()
     *
     * @copydetails PerPixelMap::setMask(const cv::Mat&)
     */
    smgl::OutputPort<cv::Mat> mask;
    /** @copydoc PerPixelMap::cellMap() */
    smgl::OutputPort<cv::Mat> cellMap;

    /** Constructor */
    PPMPropertiesNode();
};

/**
 * @brief Load a VolumetricMask from a .vcps file
 *
 * Given file must be of type=int, dim=3.
 *
 * @ingroup Graph
 */
class LoadVolumetricMaskNode : public smgl::Node
{
private:
    /** Input file */
    filesystem::path path_{};
    /** Include the loaded file in the graph cache */
    bool cacheArgs_{false};
    /** Loaded VolumetricMask */
    VolumetricMask::Pointer mask_{};

public:
    /** @brief Input path */
    smgl::InputPort<filesystem::path> path;
    /** @brief Include the loaded file in the graph cache */
    smgl::InputPort<bool> cacheArgs;
    /** @brief Loaded VolumetricMask */
    smgl::OutputPort<VolumetricMask::Pointer> volumetricMask;

    /** Constructor */
    LoadVolumetricMaskNode();

private:
    /** Smeagol custom serialization */
    auto serialize_(bool useCache, const filesystem::path& cacheDir)
        -> smgl::Metadata override;

    /** Smeagol custom deserialization */
    void deserialize_(
        const smgl::Metadata& meta,
        const filesystem::path& /*cacheDir*/) override;
};

}  // namespace volcart
