#pragma once

#include <d3d11.h>
#include <d3d12.h>
#include <Windows.h>
#include <cstdint>
#include <Psapi.h>
#include <map>

#include "Renderer.h"
#include "IRenderCallback.h"

/*
* Here we have typedefs of the functions we want to hook.
* We have these so we can call these functions through pointers to specific memory addresses.
*
* Setting the proper calling convention is important (__stdcall).
* It makes it so we can write/read function parameters into/from memory in the correct way.
* 64-bit functions actually use the __fastcall calling convention, but the compiler changes
* __stdcall to __fastcall automatically for 64-bit compilation.
*
* A great resource on calling conventions: https://www.codeproject.com/Articles/1388/Calling-Conventions-Demystified
*/
typedef HRESULT (__stdcall* Present)(IDXGISwapChain* This, UINT SyncInterval, UINT Flags);
typedef HRESULT (__stdcall* ResizeBuffers)(IDXGISwapChain* This, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags);
typedef void (__stdcall* ExecuteCommandLists)(ID3D12CommandQueue* This, UINT NumCommandLists, const ID3D12CommandList** ppCommandLists);

class DirectXHook
{
public:
	DirectXHook();
	void Hook();
	void DrawExamples(bool doDraw);
	void SetRenderCallback(IRenderCallback* object);

private:
	static std::string m_printPrefix;
	static Renderer m_renderer;
	IDXGISwapChain* m_dummySwapChain = nullptr;
	ID3D12CommandQueue* m_dummyCommandQueue = nullptr;
	static uintptr_t m_presentTrampoline;
	static uintptr_t m_resizeBuffersTrampoline;
	static uintptr_t m_executeCommandListsTrampoline;
	static uintptr_t m_originalPresentAddress;
	static uintptr_t m_originalResizeBuffersAddress;
	static uintptr_t m_originalExecuteCommandListsAddress;
	static std::vector<std::vector<unsigned char>> m_functionHeaders;
	static bool m_firstPresent;
	static bool m_firstResizeBuffers;
	static bool m_firstExecuteCommandLists;

	bool IsDirectX12Loaded();
	uintptr_t CreateBufferedTrampoline(void* destination);
	IDXGISwapChain* CreateDummySwapChain();
	ID3D12CommandQueue* CreateDummyCommandQueue();
	std::pair<uintptr_t, uintptr_t> HookSwapChainVmt(IDXGISwapChain* dummySwapChain, uintptr_t newPresentAddress, uintptr_t newResizeBuffersAddress);
	uintptr_t HookCommandQueueVmt(ID3D12CommandQueue* dummyCommandQueue, uintptr_t newExecuteCommandListsAddress);
	static void SetFunctionHeaders();
	static void RemoveDoubleHooks(uintptr_t trampolineAddress, uintptr_t originalFunctionAddress, std::vector<unsigned char> originalBytes);
	static uintptr_t FindTrampolineDestination(uintptr_t trampJmp);
	static HRESULT __stdcall OnPresent(IDXGISwapChain* pThis, UINT syncInterval, UINT flags);
	static HRESULT __stdcall OnResizeBuffers(IDXGISwapChain* pThis, UINT bufferCount, UINT width, UINT height, DXGI_FORMAT newFormat, UINT swapChainFlags);
	static void __stdcall OnExecuteCommandLists(ID3D12CommandQueue* pThis, UINT numCommandLists, const ID3D12CommandList** ppCommandLists);
};