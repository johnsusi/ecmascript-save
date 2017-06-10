#ifndef ECMASCRIPT_FILESYSTEM_H
#define ECMASCRIPT_FILESYSTEM_H

#include "config.h"

#if defined(HAVE_STD_FILESYSTEM)

#include <filesystem>
namespace fs = std::filesystem;

#elif defined(HAVE_STD_EXPERIMENTAL_FILESYSTEM)

#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;

#elif defined(HAVE_BOOST_FILESYSTEM)

#include <boost/filesystem.hpp>
namespace fs = boost::filesystem;

#endif

#endif