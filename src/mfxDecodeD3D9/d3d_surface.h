#pragma once
#include <iostream>
#include <Windows.h>
#include <d3d9.h>

namespace vsnc
{
	namespace d3d
	{
		/// <summary>
		/// D3D9渲染器
		/// </summary>
		class D3D9Surface
		{
		public:
			/// <summary>
			/// 构造函数
			/// </summary>
			/// <param name="wnd">窗口句柄</param>
			/// <param name="width">宽度</param>
			/// <param name="height">高度</param>
			D3D9Surface(HWND wnd, const std::size_t width, const std::size_t height);

			/// <summary>
			/// 释放资源，程序结束前必须调用
			/// </summary>
			void               Release() noexcept;

			/// <summary>
			/// x渲染
			/// </summary>
			/// <param name="dat">NV12格式的图像的指针头</param>
			/// <returns></returns>
			bool               Render(const D3DLOCKED_RECT& rect,const bool flag);
		private:
			/// <summary>显示区域宽度</summary>
			const std::size_t  m_uWidth;

			/// <summary>显示区域高度</summary>
			const std::size_t  m_uHeight;

			/// <summary>作用同线程锁的变量</summary>
			CRITICAL_SECTION   m_pCritical;

			/// <summary>Direct3D9</summary>
			IDirect3D9*        m_pDirect3D9;

			/// <summary>Direct3D9设备</summary>
			IDirect3DDevice9*  m_pDirect3DDevice;

			/// <summary>Direct3D9渲染器</summary>
			IDirect3DSurface9* m_pDirect3DSurfaceRender;

			/// <summary>显示区域矩形</summary>
			RECT               m_pViewport;


		};
	}
}