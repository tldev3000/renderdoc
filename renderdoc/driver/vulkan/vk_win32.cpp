/******************************************************************************
 * The MIT License (MIT)
 * 
 * Copyright (c) 2015 Baldur Karlsson
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 ******************************************************************************/

#include "vk_replay.h"
#include "vk_core.h"

static int dllLocator=0;

void VulkanReplay::OutputWindow::SetWindowHandle(void *wn)
{
	wnd = (HWND)wn;
}

void VulkanReplay::OutputWindow::InitSurfaceDescription(VkSurfaceDescriptionWindowKHR &surfDesc)
{
	GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
		(const char *)&dllLocator, (HMODULE *)&surfDesc.pPlatformHandle);
	
	surfDesc.pPlatformWindow = wnd;
	surfDesc.platform = VK_PLATFORM_WIN32_KHR;
}

void VulkanReplay::GetOutputWindowDimensions(uint64_t id, int32_t &w, int32_t &h)
{
	if(id == 0 || m_OutputWindows.find(id) == m_OutputWindows.end())
		return;
	
	OutputWindow &outw = m_OutputWindows[id];
	
	RECT rect = {0};
	GetClientRect(outw.wnd, &rect);
	w = rect.right-rect.left;
	h = rect.bottom-rect.top;
}

bool VulkanReplay::IsOutputWindowVisible(uint64_t id)
{
	if(id == 0 || m_OutputWindows.find(id) == m_OutputWindows.end())
		return false;

	return (IsWindowVisible(m_OutputWindows[id].wnd) == TRUE);
}

RENDERDOC_WindowHandle WrappedVulkan::GetHandleForSurface(const VkSurfaceDescriptionKHR* surf)
{
	RDCASSERT(surf);
	VkSurfaceDescriptionWindowKHR *winDesc = (VkSurfaceDescriptionWindowKHR *)surf;

	RDCASSERT(winDesc->platform == VK_PLATFORM_WIN32_KHR);

	return winDesc->pPlatformWindow;
}

bool LoadVulkanLibrary()
{
	return Process::LoadModule("vulkan-0.dll");
}