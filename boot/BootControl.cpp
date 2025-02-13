/*
 * Copyright (C) 2020 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#define LOG_TAG "android.hardware.boot@1.2-impl-intel"

#include <memory>

#include <log/log.h>
#include <hardware/boot_control.h>
#include <hardware/hardware.h>
#include "BootControl.h"

namespace android {
namespace hardware {
namespace boot {
namespace V1_2 {
namespace implementation {

using ::android::hardware::boot::V1_0::CommandResult;
using ::android::hardware::boot::V1_1::MergeStatus;

BootControl::BootControl(boot_control_module_t *module) : mModule(module) {}

// Methods from ::android::hardware::boot::V1_0::IBootControl.
Return<uint32_t> BootControl::getNumberSlots() {
    return mModule->getNumberSlots(mModule);
}

Return<uint32_t> BootControl::getCurrentSlot() {
    return mModule->getCurrentSlot(mModule);
}

Return<void> BootControl::markBootSuccessful(markBootSuccessful_cb _hidl_cb) {
    int ret = mModule->markBootSuccessful(mModule);
    struct CommandResult cr;
    cr.success = (ret == 0);
    cr.errMsg = strerror(-ret);
    _hidl_cb(cr);
    return Void();
}

Return<void> BootControl::setActiveBootSlot(uint32_t slot, setActiveBootSlot_cb _hidl_cb) {
    int ret = mModule->setActiveBootSlot(mModule, slot);
    struct CommandResult cr;
    cr.success = (ret == 0);
    cr.errMsg = strerror(-ret);
    _hidl_cb(cr);
    return Void();
}

Return<void> BootControl::setSlotAsUnbootable(uint32_t slot, setSlotAsUnbootable_cb _hidl_cb) {
    int ret = mModule->setSlotAsUnbootable(mModule, slot);
    struct CommandResult cr;
    cr.success = (ret == 0);
    cr.errMsg = strerror(-ret);
    _hidl_cb(cr);
    return Void();
}

Return<BoolResult> BootControl::isSlotBootable(uint32_t slot) {
    int32_t ret = mModule->isSlotBootable(mModule, slot);
    if (ret < 0) {
        return BoolResult::INVALID_SLOT;
    }
    return ret ? BoolResult::TRUE : BoolResult::FALSE;
}

Return<BoolResult> BootControl::isSlotMarkedSuccessful(uint32_t slot) {
    int32_t ret = mModule->isSlotMarkedSuccessful(mModule, slot);
    if (ret < 0) {
        return BoolResult::INVALID_SLOT;
    }
    return ret ? BoolResult::TRUE : BoolResult::FALSE;
}

Return<void> BootControl::getSuffix(uint32_t slot, getSuffix_cb _hidl_cb) {
    hidl_string ans;
    const char *suffix = mModule->getSuffix(mModule, slot);
    if (suffix) {
        ans = suffix;
    }
    _hidl_cb(ans);
    return Void();
}

// Methods from ::android::hardware::boot::V1_1::IBootControl.
// Snapshot merge status is only used by virtual a/b update.
Return<bool> BootControl::setSnapshotMergeStatus(MergeStatus status) {
    return true;
}

Return<MergeStatus> BootControl::getSnapshotMergeStatus() {
    return MergeStatus::NONE;
}


// Methods from ::android::hardware::boot::V1_2::IBootControl.
Return<uint32_t> BootControl::getActiveBootSlot() {
    auto get_active_slot = mModule->getActiveBootSlot;
    if (!get_active_slot) {
        ALOGE("Failed to find the implementation of getActiveBootSlot in boot"
              " control HAL.");
        return 0;
    }
    return get_active_slot(mModule);

}

IBootControl* HIDL_FETCH_IBootControl(const char* /* hal */) {
    int ret = 0;
    boot_control_module_t *module = NULL;
    hw_module_t **hwm = reinterpret_cast<hw_module_t **>(&module);
    ret = hw_get_module(BOOT_CONTROL_HARDWARE_MODULE_ID, const_cast<const hw_module_t **>(hwm));
    if (ret) {
        ALOGE("hw_get_module %s failed: %d", BOOT_CONTROL_HARDWARE_MODULE_ID, ret);
        return nullptr;
    }
    module->init(module);

    auto hal = new BootControl(module);
    return hal;

}

}  // namespace implementation
}  // namespace V1_2
}  // namespace boot
}  // namespace hardware
}  // namespace android
