// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and / or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The below copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//
// Vulkan Cookbook
// ISBN: 9781786468154
// � Packt Publishing Limited
//
// Author:   Pawel Lapinski
// LinkedIn: https://www.linkedin.com/in/pawel-lapinski-84522329



#ifndef VULKAN_PRESENTATION
#define VULKAN_PRESENTATION

#include "Common.h"
#include "VulkanLoader.h"
#include "VulkanQueue.h"

namespace LavaCake {
	namespace Core {
		struct PresentInfo {
			VkSwapchainKHR  Swapchain;
			uint32_t        ImageIndex;
		};

		bool CreatePresentationSurface(VkInstance         instance,
			WindowParameters   window_parameters,
			VkSurfaceKHR     & presentation_surface);

		bool SelectQueueFamilyThatSupportsPresentationToGivenSurface(VkPhysicalDevice   physical_device,
			VkSurfaceKHR       presentation_surface,
			uint32_t         & queue_family_index);

		bool SelectDesiredPresentationMode(VkPhysicalDevice   physical_device,
			VkSurfaceKHR       presentation_surface,
			VkPresentModeKHR   desired_present_mode,
			VkPresentModeKHR & present_mode);

		bool GetCapabilitiesOfPresentationSurface(VkPhysicalDevice           physical_device,
			VkSurfaceKHR               presentation_surface,
			VkSurfaceCapabilitiesKHR & surface_capabilities);

		bool PresentImage(VkQueue                  queue,
			std::vector<VkSemaphore> rendering_semaphores,
			std::vector<PresentInfo> images_to_present);

		void DestroyPresentationSurface(VkInstance     instance,
			VkSurfaceKHR & presentation_surface);

	}
} // namespace LavaCake

#endif // CREATING_A_VULKAN_INSTANCE_WITH_WSI_EXTENSIONS_ENABLED