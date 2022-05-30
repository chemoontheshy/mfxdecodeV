#ifndef __VSNC_VSNC_UTILS_VSNC_ERROR_H__
#define __VSNC_VSNC_UTILS_VSNC_ERROR_H__


#include <string>
#include <sstream>
#include <iostream>
#include <stdexcept>


#include <cstring>
#include <cerrno>


namespace vsnc
{

	namespace utils
	{


		/// <summary>
		/// <para>获取格式化后的错误信息</para>
		/// <para>格式化后的信息格式如右：[err]file:line: msg</para>
		/// </summary>
		/// <param name="path">错误所处文件</param>
		/// <param name="line">错误所处行数</param>
		/// <param name="msg">错误信息字符串</param>
		/// <param name="err">错误码</param>
		/// <returns>格式化后的错误信息字符串</returns>
		template <typename _Err = int>
		std::string __fmt_msg(const std::string path, const int line, const std::string msg, const _Err& err = errno);

		/// <summary>
		/// 当条件为真时抛出异常
		/// </summary>
		/// <param name="path">判断所处文件</param>
		/// <param name="line">判断所处行数</param>
		/// <param name="cond">判断条件</param>
		/// <param name="msg">错误信息</param>
		/// <param name="err">错误码</param>
		template <typename _Err = int>
		void        __throw_if(const std::string path, const int line, const bool cond, const std::string msg, const _Err& err = errno);

		/// <summary>
		/// 当值为0时抛出异常
		/// </summary>
		/// <typeparam name="_Val">值的类型</typeparam>
		/// <param name="path">判断所处文件</param>
		/// <param name="line">判断所处行数</param>
		/// <param name="val">待判断的变量</param>
		/// <param name="name">待判断的变量名</param>
		template <typename _Val>
		void        __throw_if_zero(const std::string path, const int line, _Val&& val, const std::string name);

		/// <summary>
		/// 当指针为空时抛出异常
		/// </summary>
		/// <param name="path">指针所处文件</param>
		/// <param name="line">指针所处行数</param>
		/// <param name="ptr">待判断的指针</param>
		/// <param name="name">待判断的指针名</param>
		template <typename _Ptr>
		void        __throw_if_nullptr(const std::string path, const int line, _Ptr&& ptr, const std::string name);

		/// <summary>
		/// 当错误码小于0时抛出异常
		/// </summary>
		/// <param name="path">判断所处文件</param>
		/// <param name="line">判断所处行数</param>
		/// <param name="err">错误码</param>
		/// <param name="name">待判断变量名</param>
		template <typename _Err>
		void        __throw_if_negative(const std::string path, const int line, const _Err err, const std::string name);


	}

}


/// <summary>定义以无需传入文件名、行数和其它某些信息</summary>
#define fmt_msg(msg)           vsnc::utils::__fmt_msg           (__FILE__, __LINE__, msg)
#define throw_if(cond)         vsnc::utils::__throw_if          (__FILE__, __LINE__, cond, #cond)
#define throw_if_zero(val)     vsnc::utils::__throw_if_zero     (__FILE__, __LINE__, val,  #val)
#define throw_if_nullptr(ptr)  vsnc::utils::__throw_if_nullptr  (__FILE__, __LINE__, ptr,  #ptr)
#define throw_if_negative(err) vsnc::utils::__throw_if_negative (__FILE__, __LINE__, err,  #err)


/// <summary>
/// <para>获取格式化后的错误信息</para>
/// <para>格式化后的信息格式如右：[err]file:line: msg</para>
/// </summary>
/// <param name="path">错误所处文件</param>
/// <param name="line">错误所处行数</param>
/// <param name="msg">错误信息字符串</param>
/// <param name="err">错误码</param>
/// <returns>格式化后的错误信息字符串</returns>
template<typename _Err>
inline std::string vsnc::utils::__fmt_msg(const std::string path, const int line, const std::string msg, const _Err& err)
{
	auto file_name = [](const char* const path) -> std::string {
#ifdef _WIN32
		return (strrchr((path), '\\') ? (strrchr((path), '\\') + 1) : (path));
#else
		return (strrchr((path), '/') ? (strrchr((path), '/') + 1) : (path));
#endif // _WIN32
	};

	std::ostringstream os;
	os << "[" << err << "]" << file_name(path.c_str()) << ":" << line << ": " << msg;
#ifdef DEBUG
	std::cerr << os.str() << std::endl;
#endif // DEBUG
	return os.str();
}


/// <summary>
/// 当条件为真时抛出异常
/// </summary>
/// <param name="path">判断所处文件</param>
/// <param name="line">判断所处行数</param>
/// <param name="cond">判断条件</param>
/// <param name="msg">错误信息</param>
/// <param name="err">错误码</param>
template<typename _Err>
void vsnc::utils::__throw_if(const std::string path, const int line, const bool cond, const std::string msg, const _Err& err)
{
	if (cond) {
		throw std::runtime_error(__fmt_msg(path, line, msg, err));
	}
}


/// <summary>
/// 当值为0时抛出异常
/// </summary>
/// <typeparam name="_Val">值的类型</typeparam>
/// <param name="path">判断所处文件</param>
/// <param name="line">判断所处行数</param>
/// <param name="val">待判断的变量</param>
/// <param name="name">待判断的变量名</param>
template<typename _Val>
void vsnc::utils::__throw_if_zero(const std::string path, const int line, _Val&& val, const std::string name)
{
	__throw_if(path, line, (0 == val), (name + " is zero"));
}


/// <summary>
/// 当指针为空时抛出异常
/// </summary>
/// <param name="path">指针所处文件</param>
/// <param name="line">指针所处行数</param>
/// <param name="ptr">待判断的指针</param>
/// <param name="name">待判断的指针名</param>
template<typename _Ptr>
inline void vsnc::utils::__throw_if_nullptr(const std::string path, const int line, _Ptr&& ptr, const std::string name)
{
	__throw_if(path, line, (nullptr == ptr), (name + " is null pointer"));
}


/// <summary>
/// 当错误码小于0时抛出异常
/// </summary>
/// <param name="path">判断所处文件</param>
/// <param name="line">判断所处行数</param>
/// <param name="err">错误码</param>
/// <param name="name">待判断变量名</param>
template<typename _Err>
void vsnc::utils::__throw_if_negative(const std::string path, const int line, const _Err err, const std::string name)
{
	__throw_if(path, line, (0 > err), name + " is negative", err);
}


#endif // !__VSNC_VSNC_UTILS_VSNC_ERROR_H__

