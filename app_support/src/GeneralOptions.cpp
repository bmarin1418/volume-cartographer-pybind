#include "vc/app_support/GeneralOptions.hpp"

namespace po = boost::program_options;

auto GetGeneralOpts() -> po::options_description
{
    // clang-format off
    po::options_description opts("General Options");
    opts.add_options()
        ("help,h", "Show this message")
        ("cache-memory-limit", po::value<std::string>(), "Maximum size of the "
            "slice cache in bytes. Accepts the suffixes: (K|M|G|T)(B). "
            "Default: 50% of the total system memory.")
        ("progress", po::value<bool>()->default_value(true),
            "When enabled, show algorithm progress bars.")
        ("log-level", po::value<std::string>()->default_value("info"),
         "Options: off, critical, error, warn, info, debug");
    // clang-format on

    return opts;
}

auto GetMeshIOOpts() -> po::options_description
{
    // clang-format off
    po::options_description opts("Mesh IO Options");
    opts.add_options()
        ("texture-format", po::value<std::string>(), "File extension for the "
            "mesh texture image. Generally can be any file format supported "
            "by OpenCV.");
    // clang-format on
    return opts;
}
