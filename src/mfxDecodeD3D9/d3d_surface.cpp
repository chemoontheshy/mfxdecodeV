#include "d3d_surface.h"
#include "../common/vsnc_common_utils.h"

vsnc::d3d::D3D9Surface::D3D9Surface(HWND wnd, const std::size_t width, const std::size_t height) :
	m_uWidth(width),
	m_uHeight(height)
{
	// 获取的显示器的范围
	GetClientRect(wnd, &m_pViewport);
	// 初始化句柄
	InitializeCriticalSection(&m_pCritical);
	//释放
	Release();
	//初始化
	m_pDirect3D9 = Direct3DCreate9(D3D_SDK_VERSION);
	if (!m_pDirect3D9)
	{
		exit(-1);
	}

	D3DPRESENT_PARAMETERS d3dpp;
	memset(&d3dpp, 0, sizeof(d3dpp));
	d3dpp.hDeviceWindow = wnd;
	d3dpp.BackBufferCount = 1;
	d3dpp.Windowed = TRUE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8;
	//立马present,不设置会阻塞，频率和屏幕刷新率一致
	d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
	throw_if_negative(m_pDirect3D9->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, wnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &d3dpp, &m_pDirect3DDevice));
	throw_if_negative(m_pDirect3DDevice->CreateOffscreenPlainSurface(
		static_cast<UINT>(m_uWidth), static_cast<UINT>(m_uHeight), (D3DFORMAT)MAKEFOURCC('N', 'V', '1', '2'),
		D3DPOOL_DEFAULT, &m_pDirect3DSurfaceRender, nullptr));
}

void vsnc::d3d::D3D9Surface::Release() noexcept
{
	EnterCriticalSection(&m_pCritical);
	if (m_pDirect3D9) m_pDirect3D9 = nullptr;
	if (m_pDirect3DDevice) m_pDirect3DDevice = nullptr;
	if (m_pDirect3DSurfaceRender) m_pDirect3DSurfaceRender = nullptr;
	LeaveCriticalSection(&m_pCritical);
}

bool vsnc::d3d::D3D9Surface::Render(const D3DLOCKED_RECT& rect, const bool flag)
{
	D3DLOCKED_RECT d3d_rect;
	if (!m_pDirect3DSurfaceRender)
	{
		return false;
	}

	if (FAILED(m_pDirect3DSurfaceRender->LockRect(&d3d_rect, nullptr, D3DLOCK_DONOTWAIT))) return false;
	auto dst = reinterpret_cast<BYTE*>(d3d_rect.pBits);
	auto ptr = reinterpret_cast<BYTE*>(rect.pBits);
	if (flag)
	{
		auto ptr = reinterpret_cast<BYTE*>(rect.pBits);
		//需要一行行复制

		//复制Y
		for (size_t i = 0; i < m_uHeight; i++)
		{
			memcpy(dst, ptr, d3d_rect.Pitch);
			dst += d3d_rect.Pitch;
			ptr += rect.Pitch;
		}
		//复制UV
		for (size_t i = 0; i < m_uHeight / 2; i++)
		{
			memcpy(dst, ptr, d3d_rect.Pitch);
			dst += d3d_rect.Pitch;
			ptr += rect.Pitch;
		}
	}
	else
	{
		memcpy(dst, rect.pBits, m_uHeight * rect.Pitch * 3 / 2);
	}
	if (FAILED(m_pDirect3DSurfaceRender->UnlockRect())) return false;
	m_pDirect3DDevice->Clear(0, nullptr, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);
	m_pDirect3DDevice->BeginScene();
	IDirect3DSurface9* pBackBuffer = nullptr;
	m_pDirect3DDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer);
	m_pDirect3DDevice->StretchRect(m_pDirect3DSurfaceRender, nullptr, pBackBuffer, &m_pViewport, D3DTEXF_LINEAR);
	m_pDirect3DDevice->EndScene();
	m_pDirect3DDevice->Present(nullptr, nullptr, nullptr, nullptr);
	pBackBuffer->Release();
	return true;
}
