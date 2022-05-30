#ifndef __VSNC_UTILS_VSNC_UTILS_H__
#define __VSNC_UTILS_VSNC_UTILS_H__


#include <sstream> // std::stringstream, std::fixed
#include <iomanip> // std::setprecision
#include <chrono>
#include <thread> // std::this_thread
#include <cstring>


namespace vsnc
{

	namespace utils
	{

		/// <summary>
		/// 将变量转为布尔数值
		/// </summary>
		/// <param name="value">其它类型数值</param>
		/// <returns>布尔数值</returns>
		template <typename _Ty>
		inline bool        __b(_Ty&& value) noexcept { return static_cast<bool>(value); }

		/// <summary>
		/// 将变量转为uint8_t数值
		/// </summary>
		/// <param name="value">其它类型数值</param>
		/// <returns>uint8_t数值</returns>
		template <typename _Ty>
		inline uint8_t     __u8(_Ty&& value) noexcept { return static_cast<uint8_t>(value); }

		/// <summary>
		/// 将变量转为整形数字
		/// </summary>
		/// <param name="value">其它类型数值</param>
		/// <returns>整形数字</returns>
		template <typename _Ty>
		inline int         __n(_Ty&& value) noexcept { return static_cast<int>(value); }

		/// <summary>
		/// 将变量转为长整形数字
		/// </summary>
		/// <param name="value">其它类型数值</param>
		/// <returns>长整形数字</returns>
		template <typename _Ty>
		inline int64_t     __n64(_Ty&& value) noexcept { return static_cast<int64_t>(value); }

		/// <summary>
		/// 将变量转为std::size_t型数字
		/// </summary>
		/// <param name="value">其它类型数值</param>
		/// <returns>std::size_t型数字</returns>
		template <typename _Ty>
		inline std::size_t __sz(_Ty&& value) noexcept { return static_cast<std::size_t>(value); }

		/// <summary>
		/// 将变量转为单精度浮点型型数字
		/// </summary>
		/// <param name="value">其它类型数值</param>
		/// <returns>单精度浮点型数字</returns>
		template <typename _Ty>
		inline float       __f(_Ty&& value) noexcept { return static_cast<float>(value); }

		/// <summary>
		/// 将变量转为双精度浮点型型数字
		/// </summary>
		/// <param name="value">其它类型数值</param>
		/// <returns>双精度浮点型数字</returns>
		template <typename _Ty>
		inline double      __d(_Ty&& value) noexcept { return static_cast<double>(value); }

		/// <summary>
		/// 清零内存
		/// </summary>
		/// <typeparam name="_Ty">内存类型</typeparam>
		/// <param name="var">对应类型的变量</param>
		template <typename _Ty>
		inline void        __zero_mem(_Ty&& var) noexcept { memset(&var, 0, sizeof(_Ty)); }

		/// <summary>
		/// 将浮点数按固定精度转为字符串，不足补0
		/// </summary>
		/// <param name="value">待转化的字符串</param>
		/// <param name="precision">小数点后的位数</param>
		/// <returns>固定精度的字符串</returns>
		template <typename _Ty>
		std::string        __to_string_with_precision(_Ty&& value, const unsigned int precision = 2);

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

		/// <summary>
		/// 获取计算机的UUID
		/// </summary>
		/// <returns>计算机的UUID</returns>
		// std::string        __get_uuid() noexcept;

	}

}


/// <summary>
/// 将浮点数按固定精度转为字符串，不足补0
/// </summary>
/// <param name="value">待转化的字符串</param>
/// <param name="precision">小数点后的位数</param>
/// <returns>固定精度的字符串</returns>
template<typename _Ty>
std::string vsnc::utils::__to_string_with_precision(_Ty&& value, const unsigned int precision)
{
	std::stringstream ss;
	ss << std::fixed << std::setprecision(precision) << value;
	return ss.str();
}


#endif // !__VSNC_UTILS_VSNC_UTILS_H__

