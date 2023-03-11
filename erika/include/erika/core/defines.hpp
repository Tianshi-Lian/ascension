#pragma once

// Exports
#ifdef ERIKA_EXPORT
#ifdef _MSC_VER
#define ERIKA_API __declspec(dllexport)
#else
#define ERIKA_API __attribute__((visibility("default")))
#endif
#else
// Imports
#ifdef _MSC_VER
#define ERIKA_API __declspec(dllimport)
#else
#define ERIKA_API
#endif
#endif
