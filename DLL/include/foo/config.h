#pragma once

//#include <limits>
#include <cstdint>

#if defined(linux) || defined(__linux)
#	define FRAME_LINUX
#	error Not yet.
#elif defined(_WIN32) || defined(__WIN32__)
#	define FRAME_WINDOWS
#else
#	error This operating system is not supported.
#endif


#if !defined(DIRSEP)
#	if defined(FRAME_WINDOWS)
#		define DIRSEP	'\\'
#	else
#		define DIRSEP	'/'
#	endif
#endif

// If we're not building libs, we're included in the project source tree so force static.
#if !FOO_BUILD_LIBS && !defined(FOO_STATIC)
#	define FOO_STATIC
#endif

#if !defined(FOO_STATIC)
#	if defined(FRAME_WINDOWS)
#		define FOO_API_EXPORT __declspec(dllexport)
#		define FOO_API_IMPORT __declspec(dllimport)
#	else
#		if (__GNUC__ >= 4)
#			define FOO_API_EXPORT __attribute__ ((__visibility__ ("default")))
#			define FOO_API_IMPORT __attribute__ ((__visibility__ ("default")))
#			define FOO_API_LOCAL  __attribute__ ((__visibility__ ("hidden")))
#		endif
#	endif
#else
#	undef FOO_STATIC
#endif

#if !defined(FOO_API_EXPORT)
#	define FOO_API_EXPORT
#endif

#if !defined(FOO_API_IMPORT)
#	define FOO_API_IMPORT
#endif

#if !defined(FOO_API_LOCAL)
#	define FOO_API_LOCAL
#endif

// FOO_API
#if defined(FOO_BUILD_LIBS)
#	define FOO_API FOO_API_EXPORT
#	undef FOO_BUILD_LIBS
#else
#	define FOO_API FOO_API_IMPORT
#endif


#ifdef _MSC_VER
#	pragma warning(disable : 4251)
#endif

namespace foo {

	/// @brief Directory separator.
	/// We should be using std::filesystem anyway so this is only for
	/// backwards compatability.
	static constexpr char dirsep = DIRSEP;

}
