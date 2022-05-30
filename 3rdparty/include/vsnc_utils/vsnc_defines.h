#ifndef __VSNC_VSNC_UTILS_VSNC_DEFINES_H__
#define __VSNC_VSNC_UTILS_VSNC_DEFINES_H__


#ifdef _WIN32
#ifdef _WINDLL
#define __VSNC_PORT __declspec(dllexport)
#else
#define __VSNC_PORT __declspec(dllimport)
#endif // WIN32_DLL
#else
#define __VSNC_PORT
#endif // _WIN32


// 仅在测试时引出
#if __VSNC_TEST
#define __VSNC_TEST_PORT __VSNC_PORT
#else
#define __VSNC_TEST_PORT
#endif // __VSNC_TEST


#define __VSNC_IN     // 输入参数
#define __VSNC_OUT    // 输出参数
#define __VSNC_IN_OUT // 输入输出参数


#endif // !__VSNC_VSNC_UTILS_VSNC_DEFINES_H__

