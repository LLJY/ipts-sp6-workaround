# Surface Pro 6 IPTS multitouch workaround

An Arch Linux package for the Surface Pro 6 IPTS EDS1 touchscreen (`045E:001F`). It works with the packaged kernel and stock `iptsd`; it does not require maintaining a custom kernel.

## The issue

Two separate kernel bugs can prevent multitouch:

- **Mode-switch timeouts:** when firmware delivers `READY_FOR_DATA` and `QUIESCE_IO` together, MEI receive-work notifications can coalesce. IPTS reads only one response per callback, leaving `QUIESCE_IO` queued. The later `GET_DEVICE_INFO` notification reads that old reply, leaving its own reply queued. Both waits can return `EAGAIN` even though firmware replied on time. The failed restart disables the HID endpoint, so simply restarting `iptsd` does not recover it.
- **Rejected raw reports:** the EDS1 descriptor declares a 7,488-byte report, but the driver submits a 7,485-byte buffer. Current HID core rejects the report before it reaches `iptsd`.

The timeout mechanism was reproduced with a temporary 3 ms diagnostic delay that batches the firmware replies. The stock driver failed; a temporary receive-draining driver succeeded. The original untraced failures strongly match this mechanism. The descriptor failure was independently demonstrated.

Related reports: [iptsd #152](https://github.com/linux-surface/iptsd/issues/152), [iptsd #201](https://github.com/linux-surface/iptsd/issues/201). The [HID core safety change](https://github.com/torvalds/linux/commit/0a3fe972a7cb1404f693d6f1711f32bc1d244b1c) exposes the descriptor mismatch.

## What this package does

- Applies an exact-match HID-BPF descriptor correction: 7,482 data bytes + two scan-time bytes + one report ID = 7,485 bytes.
- On the specific mode ioctl `EAGAIN`/`ENODEV` failure, rebinds only the dedicated IPTS MEI client and restarts its `iptsd` instance. This clears host queues and recreates driver state; it is not a firmware reset. Recovery has three attempts and then tries to restore fallback single-touch mode.
- Preserves `iptsd`'s systemd device association during udev change events, including package upgrades.

Recovery checks the failed service invocation and rediscovers the hidraw node. Its health check verifies an enabled HID endpoint and corrected descriptor; physical multitouch was tested separately. There is no periodic watchdog or calibration change.

## Build and install

Requirements: `iptsd`, `udev-hid-bpf` 2.x, and a kernel with HID-BPF struct operations and BTF support. Build dependencies are `clang`, `bpf` (providing `bpftool`), and `libbpf`. Configure the [linux-surface package repository](https://github.com/linux-surface/linux-surface/wiki/Package-Repositories) if needed for `iptsd`.

```sh
git clone https://github.com/LLJY/ipts-sp6-workaround.git
cd ipts-sp6-workaround
makepkg -si
```

Reboot to activate automatically. To activate immediately, with a brief touch interruption:

```sh
sudo /usr/lib/ipts-sp6-workaround/recover --force
```

No service needs enabling: udev starts `iptsd`, and its systemd failure hook invokes recovery.

## Check or remove

```sh
systemctl status 'iptsd@*' ipts-sp6-recover.service
journalctl -b -u ipts-sp6-recover.service
sudo udev-hid-bpf list-loaded
```

To remove the workaround:

```sh
sudo pacman -R ipts-sp6-workaround
reboot
```

Rebooting also detaches any remaining pinned BPF correction. The stock `iptsd` package is retained.

## Tested scope

Tested on a Surface Pro 6 with `7.2.9-2-cachyos-surface-latest` and `iptsd 3.1.0`. Raw reports were accepted after correction, and two to four simultaneous contact slots were recorded. Two normal package boots worked immediately; the final package boot did not need recovery. Runtime rebind and upgrade tests also passed. This is not a claim of reliability across every kernel, firmware state, or Surface model.

## Kernel proposals

`kernel-patches/series` lists proposed source fixes: restore MEI's nonblocking receive API, drain IPTS's receive queue, and correct the EDS1 descriptor. The package does not apply these patches. A diagnostic receive-draining module was tested, while the exact linked nonblocking patch series was compiled as affected objects but not installed or runtime-tested. EDS2 behavior was reviewed from source, not tested on hardware.

License: GPL-2.0-only. The HID-BPF helper headers retain their upstream attribution.

---

**This was an AI agent debugged fix that worked. Use at your own risk.**
