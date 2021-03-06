#include "Path.h"
#include "StringHelper.h"
#include <sstream>

using namespace kmd;

namespace {
	std::string concat(std::string a, std::string b) {
		while (a[a.size() - 1] == '/' || a[a.size() - 1] == '\\') a = a.substr(0, a.size() - 1);
		while (b[b.size() - 1] == '/' || b[b.size() - 1] == '\\') b = b.substr(0, b.size() - 1);
		return a + '/' + b;
	}
}

bool Path::startsWith(Path other) {
	std::string me = path;
	std::string he = other.path;
	if (he == ".") return true;
	if (me[0] == '.' && me[1] == '/') me = me.substr(2);
	if (he[0] == '.' && he[1] == '/') he = he.substr(2);
	for (unsigned i = 0; i < he.size(); ++i) {
		if (me[i] != he[i]) return false;
	}
	return true;
}

Path Path::relativize(Path other) {
	std::string me = path;
	std::string he = other.path;
	if (me == ".") return other;
	if (me[0] == '.' && me[1] == '/') me = me.substr(2);
	if (he[0] == '.' && he[1] == '/') he = he.substr(2);
	Path p;
	p.path = he.substr(me.size());
	while (p.path.size() > 0 && (p.path[0] == '/' || p.path[0] == '\\')) p.path = p.path.substr(1);
	return p;
}

Path Path::resolve(std::string subpath) {
	Path sub = Paths::get(subpath);
	if (sub.isAbsolute()) return sub;
	Path path;
	if (subpath == ".") path.path = this->path;
	else path.path = concat(this->path, subpath);
	return path;
}

Path Path::resolve(Path subpath) {
	if (subpath.isAbsolute()) return subpath;
	Path path;
	if (subpath.path == ".") path.path = this->path;
	else path.path = concat(this->path, subpath.path);
	return path;
}

Path Path::parent() {
	if (this->path == ".") return toAbsolutePath().parent();
	else {
		for (int i = static_cast<int>(path.size() - 1); i >= 0; --i) {
			if (path[i] == '/' || path[i] == '\\') {
				return Paths::get(path.substr(0, i));
			}
		}
	}
	return *this;
}

std::string Path::getFileName() {
	for (int i = static_cast<int>(path.size() - 1); i >= 0; --i) {
		if (path[i] == '/' || path[i] == '\\') {
			return path.substr(i + 1);
		}
	}
	return path;
}

std::string Path::toString() {
	if (path[0] == '.' && path[1] == '/') return path.substr(2);
	else return path;
}

bool Path::isAbsolute() {
	return (path.size() > 0 && path[0] == '/') || (path.size() > 1 && path[1] == ':');
}

namespace {
	std::string fix(std::string path) {
		path = replace(path, '\\', '/');
		path = replace(path, "//", "/");
		return path;
	}
}

Path Paths::get(std::string a) {
	Path path;
	a = fix(a);
	if (a[a.size() - 1] == '/' || a[a.size() - 1] == '\\') path.path = a.substr(0, a.size() - 1);
	else path.path = a;
	return path;
}

Path Paths::get(std::string a, std::string b) {
	Path path;
	a = fix(a);
	b = fix(b);
	if (startsWith(b, "./")) b = b.substr(2);
	if (a == ".") path.path = b;
	else path.path = concat(a, b);
	return path;
}

Path Paths::get(std::string a, std::string b, std::string c) {
	Path path;
	a = fix(a);
	b = fix(b);
	c = fix(c);
	if (startsWith(b, "./")) b = b.substr(2);
	if (startsWith(c, "./")) c = c.substr(2);
	if (a == ".") path.path = concat(b, c);
	else path.path = concat(concat(a, b), c);
	return path;
}

Path Paths::get(std::string a, std::string b, std::string c, std::string d) {
	Path path;
	a = fix(a);
	b = fix(b);
	c = fix(c);
	d = fix(d);
	if (startsWith(b, "./")) b = b.substr(2);
	if (startsWith(c, "./")) c = c.substr(2);
	if (startsWith(d, "./")) d = d.substr(2);
	if (a == ".") path.path = concat(concat(b, c), d);
	else path.path = concat(concat(concat(a, b), c), d);
	return path;
}

Path Paths::get(std::string a, std::string b, std::string c, std::string d, std::string e) {
	Path path;
	a = fix(a);
	b = fix(b);
	c = fix(c);
	d = fix(d);
	e = fix(e);
	if (startsWith(b, "./")) b = b.substr(2);
	if (startsWith(c, "./")) c = c.substr(2);
	if (startsWith(d, "./")) d = d.substr(2);
	if (startsWith(e, "./")) e = e.substr(2);
	if (a == ".") path.path = concat(concat(concat(b, c), d), e);
	else path.path = concat(concat(concat(concat(a, b), c), d), e);
	return path;
}

#ifdef SYS_WINDOWS
#include <Windows.h>
#endif

Path Path::toAbsolutePath() {
#ifdef SYS_WINDOWS
	if (path[1] == ':') return *this;
	char buffer[1001];
	GetCurrentDirectoryA(1000, buffer);
	if (this->path == ".") return Paths::get(buffer);
	else return Paths::get(replace(buffer, '\\', '/'), path);
#endif
#ifdef SYS_LINUX
	char buffer[1001];
	realpath(path.c_str(), buffer);
	return Paths::get(buffer);
#endif
#ifdef SYS_OSX
	char buffer[1001];
	realpath(path.c_str(), buffer);
	return Paths::get(buffer);
#endif
}

#ifdef SYS_OSX
#include <mach-o/dyld.h>
#endif

#ifdef SYS_LINUX
#include <unistd.h>
#endif

Path Paths::executableDir() {
#ifdef SYS_WINDOWS
	char path[MAX_PATH];
	HMODULE module = GetModuleHandle(nullptr);
	GetModuleFileNameA(module, path, sizeof(path));
	std::string s(path);
	s = s.substr(0, lastIndexOf(s, '\\'));
	if (endsWith(s, "build\\Debug") || endsWith(s, "build\\Release")) {
		for (int i = 0; i < 2; ++i) s = s.substr(0, lastIndexOf(s, '\\'));
	}
	return Paths::get(s);
#endif
#ifdef SYS_OSX
	unsigned pathsize = 1001;
	char path[pathsize];
	_NSGetExecutablePath(path, &pathsize);
	std::string s(path);
	s = s.substr(0, lastIndexOf(s, '/'));
	return Paths::get(s);
#endif
#ifdef SYS_LINUX
	int pathsize = 1001;
	char path[pathsize];
	for (int i = 0; i < 1001; ++i) path[i] = 0;
	readlink("/proc/self/exe", path, pathsize);
	std::string s(path);
	//s = "/home/robert/Projekte/BlocksFromHeaven-Kore/Kore/Tools/kake/kake";
	printf("exe path: %s", s.c_str());
	s = s.substr(0, lastIndexOf(s, '/'));
	return Paths::get(s);
#endif
}
