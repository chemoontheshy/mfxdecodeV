#include "mfx_decode.h"
#include "d3d_surface.h"
#include "../common/get_socket_data.h"
#include <fstream>
#include "../common/vsnc_common_utils.h"
#include <list>


LRESULT CALLBACK WindowProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	switch (Msg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	default:
		break;
	}
	return DefWindowProcW(hWnd, Msg, wParam, lParam);
}

int main()
{
	std::string filename = "../../3rdparty/video/1280x720_30fps_60s.hevc";
	// �Զ����Ƹ�ʽ��
	std::ifstream file(filename, std::ios::in | std::ios::binary);
	if (!file.is_open())
	{
		std::cout << "can't open the file:" + filename << std::endl;
		exit(-1);
	}
	// ��ָ���ƶ����ļ������
	file.seekg(0, file.end);

	// ��ȡ�ļ��ĳ���
	size_t len = file.tellg();
	std::vector<uint8_t> lstFileBuffer;
	lstFileBuffer.reserve(len);
	uint8_t* data = nullptr;
	data = lstFileBuffer.data();
	std::cout << "total:" << len << std::endl;

	// ��ָ���ƶ����ļ��ʼ
	file.seekg(0, std::ios::beg);

	//���ļ�д����������
	file.read(reinterpret_cast<char*>(data), len);

	file.close();

	std::list<vsnc::stucture::Packet> lstPacket;
	size_t num = 0;
	while (len > 0)
	{
		vsnc::stucture::Packet packet;

		auto tempLen = VSNC_MIN(1000, len);
		packet.Data = data + num * 1000;
		packet.Length = tempLen;
		lstPacket.push_back(packet);
		num++;
		len -= tempLen;
	}

	std::cout << "size: " << lstPacket.size() << std::endl;
	static constexpr int WND_WID = 1280;
	static constexpr int WND_HGT = 720;

	WNDCLASSEX wc;
	ZeroMemory(&wc, sizeof(wc));

	wc.cbSize = sizeof(wc);
	wc.lpfnWndProc = (WNDPROC)WindowProc;
	wc.lpszClassName = L"D3D";
	wc.style = CS_HREDRAW | CS_VREDRAW;

	//ע��
	RegisterClassEx(&wc);

	HINSTANCE hInstance = GetModuleHandle(nullptr);
	HWND wnd = CreateWindow(L"D3D", L"Video", WS_EX_OVERLAPPEDWINDOW, 0, 0, WND_WID, WND_HGT,
		nullptr, nullptr, hInstance, nullptr);
	throw_if_nullptr(wnd);
	// ��ʼ����Ⱦ
	auto render = vsnc::d3d::D3D9Surface(wnd, WND_WID, WND_HGT);
	ShowWindow(wnd, SW_SHOWDEFAULT);
	UpdateWindow(wnd);

	//Render
	MSG msg;
	ZeroMemory(&msg, sizeof(msg));

	//decode
	vsnc::mfx::MFXDecoder mfxDecoder(false);
	vsnc::stucture::Packet srcPacket;
	D3DLOCKED_RECT rect;
	while (msg.message != WM_QUIT) {
		//PeekMessage, not GetMessage
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else {
			while (!mfxDecoder.GetPacket(rect))
			{
				if (!lstPacket.empty())
				{
					srcPacket = lstPacket.front();
					lstPacket.pop_front();
					mfxDecoder.FillMFXBitStream(srcPacket);
				}
				else
				{
					exit(-1);
				}
			}
			Sleep(20);
			if (!render.Render(rect, false))
			{
				break;
			}
		}
	}
	//�ͷ���Դ
	render.Release();
	std::cout << "endl" << std::endl;
	UnregisterClass(L"D3D", hInstance);
	return 0;
}
