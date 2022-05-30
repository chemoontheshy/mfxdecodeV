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

#define __VSNC_IN     // �������
#define __VSNC_OUT    // �������
#define __VSNC_IN_OUT // �����������


#endif // !__VSNC_VSNC_UTILS_VSNC_DEFINES_H__


namespace vsnc
{
	/// <summary>
	/// ���ýṹ��
	/// </summary>
	namespace stucture
	{
		/// <summary>
		/// ���ݰ�
		/// </summary>
		struct Packet
		{
			/// <summary>ָ��ͷ </summary>
			uint8_t* Data;
			/// <summary>���� </summary>
			size_t   Length;
		};
	}

	namespace utils
	{
		template <typename _Ty>
		inline void __zero_mem(_Ty&& var) noexcept { memset(&var, 0, sizeof(_Ty)); }

		/// <summary>
		/// ��ȡ�Ժ���Ϊ��λ��UTCʱ��
		/// </summary>
		/// <returns>�Ժ���Ϊ��λ��UTCʱ��</returns>
		inline int64_t     __utc() noexcept { return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count(); }

		/// <summary>
		/// ��ǰ�߳�����Ϊ��λ��ʱ
		/// </summary>
		/// <param name="val">�ӳٵ�����</param>
		inline void        __sleep_seconds(const int val) noexcept { std::this_thread::sleep_for(std::chrono::seconds(val)); }

		/// <summary>
		/// ��ǰ�߳��Ժ���Ϊ��λ��ʱ
		/// </summary>
		/// <param name="val">�ӳٵĺ�����</param>
		inline void        __sleep_milliseconds(const int val) noexcept { std::this_thread::sleep_for(std::chrono::milliseconds(val)); }
	}

	namespace error
	{
		/// <summary>
		/// ��ȡ��ʽ����Ĵ�����Ϣ
		/// </summary>
		/// <typeparam name="_Err">������</typeparam>
		/// <param name="path">���������ļ�</param>
		/// <param name="line">������������</param>
		/// <param name="msg">������Ϣ�ַ���</param>
		/// <param name="err">������</param>
		/// <returns>��ʽ����Ĵ�����Ϣ�ַ���</returns>
		template<typename _Err = int>
		std::string __fmt_msg(const std::string path, const int line, const std::string msg, const _Err& err = errno);

		/// <summary>
		/// ������Ϊ��ʱ�׳��쳣
		/// </summary>
		/// <param name="path">�ж������ļ�</param>
		/// <param name="line">�ж���������</param>
		/// <param name="cond">�ж�����</param>
		/// <param name="msg">������Ϣ</param>
		/// <param name="err">������</param>
		template <typename _Err =int>
		void __throw_if(const std::string path, const int line, const bool cond, const std::string msg, const _Err& err = errno);

		/// <summary>
		/// ��ֵΪ0ʱ�׳��쳣
		/// </summary>
		/// <typeparam name="_Val">ֵ������</typeparam>
		/// <param name="path">�ж������ļ�</param>
		/// <param name="line">�ж���������</param>
		/// <param name="val">���жϵı���</param>
		/// <param name="name">���жϵı�����</param>
		template <typename _Val>
		void __throw_if_zero(const std::string path, const int line, _Val& val, const std::string name);

		/// <summary>
		/// ��ָ��Ϊ��ʱ�׳��쳣
		/// </summary>
		/// <param name="path">ָ�������ļ�</param>
		/// <param name="line">ָ����������</param>
		/// <param name="ptr">���жϵ�ָ��</param>
		/// <param name="name">���жϵ�ָ����</param>
		template <typename _Ptr>
		void        __throw_if_nullptr(const std::string path, const int line, _Ptr&& ptr, const std::string name);

		/// <summary>
		/// ��������С��0ʱ�׳��쳣
		/// </summary>
		/// <param name="path">�ж������ļ�</param>
		/// <param name="line">�ж���������</param>
		/// <param name="err">������</param>
		/// <param name="name">���жϱ�����</param>
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
