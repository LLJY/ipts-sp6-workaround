/* SPDX-License-Identifier: GPL-2.0-only */
/* Minimal udev-hid-bpf metadata ABI, derived from its GPL-2.0 helpers.
 * Copyright (c) 2022 Benjamin Tissoires
 */
#ifndef IPTS_SP6_BPF_HELPERS_H
#define IPTS_SP6_BPF_HELPERS_H
#include "vmlinux.h"
#define bpf_stream_vprintk __ipts_unused_stream_vprintk
#include <bpf/bpf_helpers.h>
#undef bpf_stream_vprintk
#include <linux/errno.h>

extern __u8 *hid_bpf_get_data(struct hid_bpf_ctx *ctx, unsigned int offset,
                            const size_t __sz) __ksym;

#define HID_GROUP_GENERIC 0x0001
#define HID_DEVICE(b, g, vendor, product) struct { \
    __uint(name, 0); __uint(bus, (b)); __uint(group, (g)); \
    __uint(vid, (vendor)); __uint(pid, (product)); \
} ipts_sp6_match
#define HID_BPF_CONFIG(device) union { device; } _device_ids SEC(".hid_bpf_config")
#endif
