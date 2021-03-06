#pragma once

#include "Path.h"
#include <vector>

namespace kmd {
	namespace Files {
		bool exists(Path path);
		void createDirectories(Path path);
		void removeDirectory(Path path);
		bool isDirectory(Path path);
		void copy(Path from, Path to, bool replace = false);
		std::vector<Path> newDirectoryStream(Path path);
	}
}
