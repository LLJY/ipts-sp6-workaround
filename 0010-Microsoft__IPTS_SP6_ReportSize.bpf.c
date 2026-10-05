// SPDX-License-Identifier: GPL-2.0-only
/* Correct the EDS1 synthetic raw report descriptor to match its 7485-byte buffer. */
#include "vmlinux.h"
#include "hid_bpf.h"
#include "hid_bpf_helpers.h"
#include <bpf/bpf_tracing.h>

HID_BPF_CONFIG(HID_DEVICE(0x0000, HID_GROUP_GENERIC, 0x045E, 0x001F));

SEC(HID_BPF_RDESC_FIXUP)
int BPF_PROG(ipts_sp6_report_size, struct hid_bpf_ctx *hctx)
{
    __u8 *d;
    if (hctx->size != 96)
        return 0;
    d = hid_bpf_get_data(hctx, 0, 96);
    if (!d)
        return 0;
    /* Match the raw report ID, 16-bit scan time, and the offending 8-bit count. */
    if (d[57] != 0x05 || d[58] != 0x0d || d[63] != 0x85 || d[64] != 0x41 ||
        d[67] != 0x95 || d[68] != 0x01 || d[69] != 0x75 || d[70] != 0x10 ||
        d[75] != 0x75 || d[76] != 0x08 || d[77] != 0x96 ||
        d[78] != 0x3d || d[79] != 0x1d || d[80] != 0x81 || d[81] != 0x03)
        return 0;
    /* 7482 data bytes + 2 scan-time bytes + 1 report ID = 7485. */
    d[78] = 0x3a;
    return 0;
}

HID_BPF_OPS(ipts_sp6_size_ops) = {
    .hid_rdesc_fixup = (void *)ipts_sp6_report_size,
};

SEC("syscall")
int probe(struct hid_bpf_probe_args *ctx)
{
    ctx->retval = -EINVAL;
    if (ctx->rdesc_size == 96 && ctx->rdesc[63] == 0x85 && ctx->rdesc[64] == 0x41 &&
        ctx->rdesc[77] == 0x96 && ctx->rdesc[78] == 0x3d && ctx->rdesc[79] == 0x1d)
        ctx->retval = 0;
    return 0;
}
char _license[] SEC("license") = "GPL";
