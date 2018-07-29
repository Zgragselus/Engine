#ifndef __DLL_DEF_H__
#define __DLL_DEF_H__

#ifdef CORE_DLL_EXPORTS
#define CORE_API __declspec(dllexport) 
#else
#define CORE_API __declspec(dllimport) 
#endif

#endif