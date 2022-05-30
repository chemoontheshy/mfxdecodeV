#include "mfx_directx.h"



namespace vsnc
{
	namespace mfx
	{
		IDirect3DDeviceManager9*          m_pDeviceManager9 =nullptr;
		IDirect3DDevice9Ex*               m_pD3DD9=nullptr;
		IDirect3D9Ex*                     m_pD3D9=nullptr;
		HANDLE                            m_pDeviceHandle =nullptr;
		IDirectXVideoAccelerationService* m_pDXVAServiceDec =nullptr;
		mfxFrameAllocResponse             m_iSaveALLocResponse = {};
		//渲染
		
		template <typename Ty>
		static void __release(Ty&& val) noexcept;
	}
}

template<typename Ty>
void vsnc::mfx::__release(Ty&& val) noexcept
{
	if (val)
	{
		val->Release();
		val = nullptr;
	}
}


mfxStatus vsnc::mfx::CreateHWDevice(mfxSession session, mfxHDL* deviceHandle, HWND window)
{
	if (!window)
	{
		window = WindowFromPoint({ 0,0 });
	}
	RECT rc;
	GetClientRect(window, &rc);
	auto hr = Direct3DCreate9Ex(D3D_SDK_VERSION, &m_pD3D9);
	throw_if_negative(hr);
	throw_if_nullptr(m_pD3D9);
	//配置参数
	D3DPRESENT_PARAMETERS D3DPP;
	memset(&D3DPP, 0, sizeof(D3DPP));
	D3DPP.Windowed                   = true;
	D3DPP.hDeviceWindow              = window;
	D3DPP.Flags                      = D3DPRESENTFLAG_VIDEO;
	D3DPP.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
	D3DPP.PresentationInterval       = D3DPRESENT_INTERVAL_IMMEDIATE;
	D3DPP.BackBufferCount            = 1;
	D3DPP.BackBufferFormat           = D3DFMT_A8R8G8B8;
	D3DPP.BackBufferWidth            = rc.right - rc.left;
	D3DPP.BackBufferHeight           = rc.bottom - rc.top;

	D3DPP.Flags                     |= D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;
	D3DPP.SwapEffect                 = D3DSWAPEFFECT_DISCARD;

	hr = m_pD3D9->CreateDeviceEx(0,
		D3DDEVTYPE_HAL,
		window,
		D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED ,
		&D3DPP,
		nullptr,
		&m_pD3DD9);
	throw_if_negative(hr);

	throw_if_negative(m_pD3DD9->ResetEx(&D3DPP, nullptr));
	throw_if_negative(m_pD3DD9->Clear(0, nullptr, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0));
	UINT resetToken = 0;
	throw_if_negative(DXVA2CreateDirect3DDeviceManager9(&resetToken, &m_pDeviceManager9));
	throw_if_negative(m_pDeviceManager9->ResetDevice(m_pD3DD9, resetToken));
	*deviceHandle = (mfxHDL)m_pDeviceManager9;
	return  MFX_ERR_NONE;
}


void vsnc::mfx::CleanupHWDevice()
{
	m_pDeviceManager9->CloseDeviceHandle(m_pDeviceHandle);
	__release(m_pDXVAServiceDec);
	__release(m_pDeviceManager9);
	__release(m_pD3DD9);
	__release(m_pD3D9);
}


mfxStatus vsnc::mfx::mfx_alloc(mfxHDL pthis, mfxFrameAllocRequest* request, mfxFrameAllocResponse* response)
{
	auto __mfx_alloc = [=](mfxFrameAllocRequest* request, mfxFrameAllocResponse* response)->mfxStatus
	{
		// 视频解码器渲染目标
		DWORD DxvaType = DXVA2_VideoDecoderRenderTarget;
		HRESULT hr = S_OK;

		D3DFORMAT format = (D3DFORMAT)MAKEFOURCC('N', 'V', '1', '2');
		mfxMemId* mids = new mfxMemId[request->NumFrameSuggested];
		if (!mids) return MFX_ERR_MEMORY_ALLOC;
		if (!m_pDeviceHandle)
		{
			throw_if_negative(m_pDeviceManager9->OpenDeviceHandle(&m_pDeviceHandle));
		}

		IDirectXVideoAccelerationService* pDXVAServiceTmp = nullptr;
		hr = m_pDeviceManager9->GetVideoService(m_pDeviceHandle, IID_IDirectXVideoDecoderService, (void**)&m_pDXVAServiceDec);
		pDXVAServiceTmp = m_pDXVAServiceDec;
		if (FAILED(hr)) return MFX_ERR_MEMORY_ALLOC;

		hr = pDXVAServiceTmp->CreateSurface(request->Info.Width,
			request->Info.Height,
			request->NumFrameSuggested - 1,
			format,
			D3DPOOL_DEFAULT,
			0,
			DxvaType,
			(IDirect3DSurface9**)mids,
			nullptr);
		if (FAILED(hr)) return MFX_ERR_MEMORY_ALLOC;
		response->mids = mids;
		response->NumFrameActual = request->NumFrameSuggested;

		return MFX_ERR_NONE;
	};

	pthis;
	mfxStatus sts = MFX_ERR_NONE;
	if (request->NumFrameSuggested <= m_iSaveALLocResponse.NumFrameActual &&
		MFX_MEMTYPE_EXTERNAL_FRAME & request->Type &&
		MFX_MEMTYPE_FROM_DECODE & request->Type &&
		m_iSaveALLocResponse.NumFrameActual != 0)
	{
		*response = m_iSaveALLocResponse;
	}
	else {
		__mfx_alloc(request, response);
		m_iSaveALLocResponse = *response;
	}
	return sts;
}

mfxStatus vsnc::mfx::mfx_lock(mfxHDL pthis, mfxMemId mid, mfxFrameData* ptr)
{
	auto render = reinterpret_cast<IDirect3DSurface9*>(mid);
	if (!render) return MFX_ERR_INVALID_HANDLE;
	if (!ptr) return MFX_ERR_LOCK_MEMORY;

	D3DSURFACE_DESC desc;
	HRESULT hr = render->GetDesc(&desc);
	if (FAILED(hr)) return MFX_ERR_LOCK_MEMORY;

	D3DLOCKED_RECT locked;
	hr = render->LockRect(&locked, 0, D3DLOCK_NOSYSLOCK);
	if (FAILED(hr)) return MFX_ERR_LOCK_MEMORY;

	ptr->Pitch = (mfxU16)locked.Pitch;
	ptr->Y = (mfxU8*)locked.pBits;
	ptr->U = (mfxU8*)locked.pBits + desc.Height * locked.Pitch;
	ptr->V = ptr->U + 1;

	return MFX_ERR_NONE;
}

mfxStatus vsnc::mfx::mfx_unlock(mfxHDL pthis, mfxMemId mid, mfxFrameData* ptr)
{
	pthis; // To suppress warning for this unused parameter

	IDirect3DSurface9* pSurface = (IDirect3DSurface9*)mid;

	if (pSurface == 0) return MFX_ERR_INVALID_HANDLE;

	pSurface->UnlockRect();

	if (NULL != ptr)
	{
		ptr->Pitch = 0;
		ptr->R = 0;
		ptr->G = 0;
		ptr->B = 0;
		ptr->A = 0;
	}

	return MFX_ERR_NONE;
}

mfxStatus vsnc::mfx::mfx_gethdl(mfxHDL pthis, mfxMemId mid, mfxHDL* handle)
{
	pthis; // To suppress warning for this unused parameter

	if (handle == 0) return MFX_ERR_INVALID_HANDLE;

	*handle = mid;
	return MFX_ERR_NONE;
}

mfxStatus vsnc::mfx::mfx_free(mfxHDL pthis, mfxFrameAllocResponse* response)
{
	pthis; // To suppress warning for this unused parameter

	if (!response) return MFX_ERR_NULL_PTR;

	if (response->mids)
	{
		for (mfxU32 i = 0; i < response->NumFrameActual; i++)
		{
			if (response->mids[i])
			{
				IDirect3DSurface9* handle = (IDirect3DSurface9*)response->mids[i];
				handle->Release();
			}
		}
	}

	delete[] response->mids;
	response->mids = 0;

	return MFX_ERR_NONE;
}

