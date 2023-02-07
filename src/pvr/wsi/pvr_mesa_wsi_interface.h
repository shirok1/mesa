/*************************************************************************/ /*!
@File
@Title          PVR interface to the Vulkan WSI layer in Mesa
@Copyright      Copyright (c) Imagination Technologies Ltd. All Rights Reserved
@License        MIT

The contents of this file are subject to the MIT license as set out below.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/ /**************************************************************************/

#ifndef PVR_MESA_WSI_INTERFACE_H
#define PVR_MESA_WSI_INTERFACE_H

#include <stdbool.h>
#include <stdint.h>

#include <vulkan/vulkan.h>

/*
 * The pvr_mesa_wsi structure holds the Mesa WSI state, and is opaque to
 * the PowerVK DDK.
 */
struct pvr_mesa_wsi;

/*
 * Functions defined in Mesa for use by the PowerVR DDK.
 * All functions have a "pvr_mesa_wsi" prefix.
 */

void *
pvr_mesa_wsi_sym_addr(struct pvr_mesa_wsi *mwsi,
                      const char *name);

VkResult
pvr_mesa_wsi_init(struct pvr_mesa_wsi **mwsi,
                  VkPhysicalDevice physicalDevice,
                  PFN_vkVoidFunction (VKAPI_PTR *pvr_vk_mesa_wsi_sym_addr)
                     (VkPhysicalDevice physicalDevice, const char *),
                  const VkAllocationCallbacks *alloc,
                  int fd,
                  bool sw);

void
pvr_mesa_wsi_finish(struct pvr_mesa_wsi *mwsi,
                    const VkAllocationCallbacks *alloc);

VkResult
pvr_mesa_wsi_common_get_images(struct pvr_mesa_wsi *mwsi,
                               VkSwapchainKHR swapchain,
                               uint32_t *pSwapchainImageCount,
                               VkImage *pSwapchainImages);

VkResult
pvr_mesa_wsi_common_acquire_next_image2(struct pvr_mesa_wsi *mwsi,
                                        VkDevice device,
                                        const VkAcquireNextImageInfoKHR *pAcquireInfo,
                                        uint32_t *pImageIndex);

VkResult
pvr_mesa_wsi_common_queue_present(struct pvr_mesa_wsi *mwsi,
                                  VkDevice device,
                                  VkQueue queue,
                                  int queue_family_index,
                                  const VkPresentInfoKHR *pPresentInfo);

/*
 * Functions defined in the PowerVR DDK for use by Mesa.
 * All functions have a "pvr_vk_mesa_wsi" prefix.
 */
VKAPI_ATTR PFN_vkVoidFunction VKAPI_CALL
pvr_vk_mesa_wsi_sym_addr(VkPhysicalDevice physicalDevice,
                         const char *name);

#endif /* PVR_MESA_WSI_INTERFACE_H */
