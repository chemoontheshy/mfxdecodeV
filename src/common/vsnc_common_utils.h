#ifndef __VSNC_COMMON_UTILS_H_
#define __VSNC_COMMON_UTILS_H_

#include <chrono>
#include <thread> // std::this_thread
#include <iostream>

#ifdef _WIN32
#ifdef _WINDLL
#define __VSNC_PORT __declspec(dllexport)
#else
#define __VSNC_PORT __declspec(dllimport)
#endif // WIN32_DLL

#define __VSNC_IN     // 输入参数
#define __VSNC_OUT    // 输出参数
#define __VSNC_IN_OUT // 输入输出参数


#endif // !__VSNC_VSNC_UTILS_VSNC_DEFINES_H__


namespace vsnc
{
	/// <summary>
	/// 常用结构体
	/// </summary>
	namespace stucture
	{
		/// <summary>
		/// 数据包
		/// </summary>
		struct Packet
		{
			/// <summary>指针头 </summary>
			uint8_t* Data;
			/// <summary>长度 </summary>
			size_t   Length;
		};
	}

	namespace utils
	{
		template <typename _Ty>
		inline void __zero_mem(_Ty&& var) noexcept { memset(&var, 0, sizeof(_Ty)); }

		/// <summary>
		/// 获取以毫秒为单位的UTC时间
		/// </summary>
		/// <returns>以毫秒为单位的UTC时间</returns>
		inline int64_t     __utc() noexcept { return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count(); }

		/// <summary>
		/// 当前线程以秒为单位延时
		/// </summary>
		/// <param name="val">延迟的秒数</param>
		inline void        __sleep_seconds(const int val) noexcept { std::this_thread::sleep_for(std::chrono::seconds(val)); }

		/// <summary>
		/// 当前线程以毫秒为单位延时
		/// </summary>
		/// <param name="val">延迟的毫秒数</param>
		inline void        __sleep_milliseconds(const int val) noexcept { std::this_thread::sleep_for(std::chrono::milliseconds(val)); }
	}

	namespace error
	{
		/// <summary>
		/// 获取格式化后的错误信息
		/// </summary>
		/// <typeparam name="_Err">错误码</typeparam>
		/// <param name="path">错误所处文件</param>
		/// <param name="line">错误所处行数</param>
		/// <param name="msg">错误信息字符串</param>
		/// <param name="err">错误码</param>
		/// <returns>格式化后的错误信息字符串</returns>
		template<typename _Err = int>
		std::string __fmt_msg(const std::string path, const int line, const std::string msg, const _Err& err = errno);

		/// <summary>
		/// 当条件为真时抛出异常
		/// </summary>
		/// <param name="path">判断所处文件</param>
		/// <param name="line">判断所处行数</param>
		/// <param name="cond">判断条件</param>
		/// <param name="msg">错误信息</param>
		/// <param name="err">错误码</param>
		template <typename _Err =int>
		void __throw_if(const std::string path, const int line, const bool cond, const std::string msg, const _Err& err = errno);

		/// <summary>
		/// 当值为0时抛出异常
		/// </summary>
		/// <typeparam name="_Val">值的类型</typeparam>
		/// <param name="path">判断所处文件</param>
		/// <param name="line">判断所处行数</param>
		/// <param name="val">待判断的变量</param>
		/// <param name="name">待判断的变量名</param>
		template <typename _Val>
		void __throw_if_zero(const std::string path, const int line, _Val& val, const std::string name);

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

#define VSNC_MAX(a,b)          ((a)>(b)?(a):(b))
#define VSNC_MIN(a,b)          ((a)<(b)?(a):(b))

#define fmt_msg(msg)           vsnc::error::__fmt_msg           (__FILE__, __LINE__, msg)
#define throw_if(cond)         vsnc::error::__throw_if          (__FILE__, __LINE__, cond, #cond)
#define throw_if_zero(val)     vsnc::error::__throw_if_zero     (__FILE__, __LINE__, val,  #val)
#define throw_if_nullptr(ptr)  vsnc::error::__throw_if_nullptr  (__FILE__, __LINE__, ptr,  #ptr)
#define throw_if_negative(err) vsnc::error::__throw_if_negative (__FILE__, __LINE__, err,  #err)

template<typename _Err>
std::string vsnc::error::__fmt_msg(const std::string path, const int line, const std::string msg, const _Err& err)
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
#ifdef _DEBUG
	std::cerr << os.str() << std::endl;
#endif // DEBUG
	return os.str();
}

template<typename _Err>
void vsnc::error::__throw_if(const std::string path, const int line, const bool cond, const std::string msg, const _Err& err)
{

	if (cond) throw std::runtime_error(__fmt_msg(path, line, msg, err));
}

template<typename _Val>
void vsnc::error::__throw_if_zero(const std::string path, const int line, _Val& val, const std::string name)
{
	__throw_if(path, line, (0 == val), (name + "is zero"));
}

template<typename _Ptr>
void vsnc::error::__throw_if_nullptr(const std::string path, const int line, _Ptr&& ptr, const std::string name)
{
	__throw_if(path, line, (nullptr == ptr), (name + " is null pointer"));
}

template<typename _Err>
void vsnc::error::__throw_if_negative(const std::string path, const int line, const _Err err, const std::string name)
{
	__throw_if(path, line, (0 > err), name + " is negative", err);
}

#endif // !__VSNC_COMMON_UTILS_H_
