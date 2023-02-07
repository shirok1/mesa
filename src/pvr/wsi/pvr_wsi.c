/*
 * Copyright © Imagination Technologies Ltd.
 *
 * The contents of this file are subject to the MIT license as set out below.
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
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <string.h>

#include "pvr_wsi.h"
#include "pvr_mesa_wsi_interface.h"
#include "vk_physical_device.h"
#include "wsi_common_entrypoints.h"

VkResult
pvr_mesa_wsi_init(struct pvr_mesa_wsi **pmwsi,
                  VkPhysicalDevice physicalDevice,
                  PFN_vkVoidFunction (VKAPI_PTR *pvr_vk_mesa_wsi_sym_addr)
                     (VkPhysicalDevice physicalDevice, const char *),
                  const VkAllocationCallbacks *alloc,
                  int fd,
                  bool sw)
{
   struct pvr_mesa_wsi *mwsi;
   VkResult result;

   mwsi = vk_zalloc(alloc, sizeof(*mwsi), 8,
                    VK_SYSTEM_ALLOCATION_SCOPE_INSTANCE);
   if (!mwsi)
      return VK_ERROR_OUT_OF_HOST_MEMORY;

   mwsi->symtab.pvr_vk_mesa_wsi_sym_addr = pvr_vk_mesa_wsi_sym_addr;
   mwsi->physicalDevice = physicalDevice;

   result = wsi_device_init(&mwsi->wsi,
                            physicalDevice,
                            pvr_vk_mesa_wsi_sym_addr,
                            alloc,
                            fd, NULL, sw);
   if (result != VK_SUCCESS) {
      vk_free(alloc, mwsi);
      return result;
   }

   if (!sw)
      mwsi->wsi.supports_modifiers = true;

   {
      VK_FROM_HANDLE(vk_physical_device, physical_device, (mwsi->physicalDevice));
      physical_device->wsi_device = &mwsi->wsi;
   }

   *pmwsi = mwsi;

   return VK_SUCCESS;
}

void
pvr_mesa_wsi_finish(struct pvr_mesa_wsi *mwsi,
		    const VkAllocationCallbacks *alloc)
{
   {
      VK_FROM_HANDLE(vk_physical_device, physical_device, (mwsi->physicalDevice));
      physical_device->wsi_device = NULL;
   }

   wsi_device_finish(&mwsi->wsi, alloc);

   vk_free(alloc, mwsi);
}

VkResult
pvr_mesa_wsi_common_get_images(UNUSED struct pvr_mesa_wsi *mwsi,
                               VkSwapchainKHR swapchain,
                               uint32_t *pSwapchainImageCount,
                               VkImage *pSwapchainImages)
{
   return wsi_common_get_images(swapchain,
                                pSwapchainImageCount,
                                pSwapchainImages);
}

VkResult
pvr_mesa_wsi_common_acquire_next_image2(struct pvr_mesa_wsi *mwsi,
                                        VkDevice device,
                                        const VkAcquireNextImageInfoKHR *pAcquireInfo,
                                        uint32_t *pImageIndex)
{
   return wsi_common_acquire_next_image2(&mwsi->wsi,
                                         device,
                                         pAcquireInfo,
                                         pImageIndex);
}

VkResult
pvr_mesa_wsi_common_queue_present(struct pvr_mesa_wsi *mwsi,
                                  VkDevice device,
                                  VkQueue queue,
                                  int queue_family_index,
                                  const VkPresentInfoKHR *pPresentInfo)
{
   return wsi_common_queue_present(&mwsi->wsi,
                                   device,
                                   queue,
                                   queue_family_index,
                                   pPresentInfo);
}

/*
 * The mwsi parameter is currently unused. Note that it is invalid for
 * pvr_mesa_wsi_init, which is responsible for allocating it.
 */
PUBLIC void *
pvr_mesa_wsi_sym_addr(UNUSED struct pvr_mesa_wsi *mwsi, const char *name)
{
   static const struct {
      char *name;
      void *addr;
   } lookup[] = {
      { "pvr_mesa_wsi_init",
            pvr_mesa_wsi_init },
      { "pvr_mesa_wsi_finish",
            pvr_mesa_wsi_finish },
      { "pvr_mesa_wsi_common_get_images",
            pvr_mesa_wsi_common_get_images },
      { "pvr_mesa_wsi_common_acquire_next_image2",
            pvr_mesa_wsi_common_acquire_next_image2 },
      { "pvr_mesa_wsi_common_queue_present",
            pvr_mesa_wsi_common_queue_present },
   };
   unsigned i;

   for (i = 0; i < ARRAY_SIZE(lookup); i++) {
      if (!strcmp(name, lookup[i].name))
         return lookup[i].addr;
   }

   return NULL;
}
