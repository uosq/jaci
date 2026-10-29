//
// Created by tevin on 29/06/2026.
//

#include "../../thirdparty/d3d9/d3d9.h"
#include "../../thirdparty/d3d9/windows_base.h"

#include "../features/gui/gui.hpp"

LPDIRECT3DDEVICE9 d3d_device = nullptr;

void* original_D3D9SwapChainEx_Present = nullptr;
void* original_D3D9DeviceEx_Reset = nullptr;

// 2 vfunctions after D3D9DeviceEx::Reset
// you find vfunction18 (Present) which calls PresentEx
// but PresentEx and Present are generic as shit so can't reliably sig them
// so use the D3D9SwapChainEx::Present instead
// https://github.com/doitsujin/dxvk/blob/64542839fedb9684a5636660d1d138121ee17eec/src/d3d9/d3d9_device.cpp#L4248
HRESULT D3D9SwapChainEx_Present(IDirect3DSwapChain9Ex* swapchain, const RECT* pSourceRect,
	const RECT* pDestRect, HWND hDestWindowOverride, const RGNDATA* pDirtyRegion, DWORD dwFlags)
{
	if (!d3d_device)
	{
		(void)swapchain->GetDevice(&d3d_device);
		// undo the AddRef from GetDevice
		// https://github.com/doitsujin/dxvk/blob/a9ab43b31d6ee5054a69d31080d43f867bed1d15/src/util/com/com_pointer.h#L17
		// https://github.com/doitsujin/dxvk/blob/a9ab43b31d6ee5054a69d31080d43f867bed1d15/src/d3d9/d3d9_device_child.h#L56
		if (d3d_device) d3d_device->Release();
	}

	if (d3d_device)
	{
		CGui::Initialize();
		CGui::Render();
	}

	using OriginalFn = HRESULT(*)(IDirect3DSwapChain9Ex* swapchain, const RECT* pSourceRect,
	const RECT* pDestRect, HWND hDestWindowOverride, const RGNDATA* pDirtyRegion, DWORD dwFlags);
	return ((OriginalFn)original_D3D9SwapChainEx_Present)(swapchain, pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion, dwFlags);
}

// xref: Device reset failed because device still has alive losable resources: Device not reset. Remaining resources:
HRESULT D3D9DeviceEx_Reset(LPDIRECT3DDEVICE9 device, D3DPRESENT_PARAMETERS* pPresentationParamters)
{
	using OriginalFn = HRESULT(*)(LPDIRECT3DDEVICE9 device, D3DPRESENT_PARAMETERS* pPresentationParamters);
	const auto ret = ((OriginalFn)original_D3D9DeviceEx_Reset)(device, pPresentationParamters);

	CGui::Reset();
	d3d_device = device;

	return ret;
}
