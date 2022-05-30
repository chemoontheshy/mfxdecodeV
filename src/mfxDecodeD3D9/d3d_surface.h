#pragma once
#include <iostream>
#include <Windows.h>
#include <d3d9.h>

namespace vsnc
{
	namespace d3d
	{
		/// <summary>
		/// D3D9��Ⱦ��
		/// </summary>
		class D3D9Surface
		{
		public:
			/// <summary>
			/// ���캯��
			/// </summary>
			/// <param name="wnd">���ھ��</param>
			/// <param name="width">���</param>
			/// <param name="height">�߶�</param>
			D3D9Surface(HWND wnd, const std::size_t width, const std::size_t height);

			/// <summary>
			/// �ͷ���Դ���������ǰ�������
			/// </summary>
			void               Release() noexcept;

			/// <summary>
			/// x��Ⱦ
			/// </summary>
			/// <param name="dat">NV12��ʽ��ͼ���ָ��ͷ</param>
			/// <returns></returns>
			bool               Render(const D3DLOCKED_RECT& rect,const bool flag);
		private:
			/// <summary>��ʾ������</summary>
			const std::size_t  m_uWidth;

			/// <summary>��ʾ����߶�</summary>
			const std::size_t  m_uHeight;

			/// <summary>����ͬ�߳����ı���</summary>
			CRITICAL_SECTION   m_pCritical;

			/// <summary>Direct3D9</summary>
			IDirect3D9*        m_pDirect3D9;

			/// <summary>Direct3D9�豸</summary>
			IDirect3DDevice9*  m_pDirect3DDevice;

			/// <summary>Direct3D9��Ⱦ��</summary>
			IDirect3DSurface9* m_pDirect3DSurfaceRender;

			/// <summary>��ʾ�������</summary>
			RECT               m_pViewport;


		};
	}
}