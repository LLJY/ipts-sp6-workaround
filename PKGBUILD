# SPDX-License-Identifier: GPL-2.0-only
pkgname=ipts-sp6-workaround
pkgver=0.1.4
pkgrel=2
pkgdesc='Surface Pro 6 EDS1 HID report-size correction and bounded IPTS client recovery'
url='https://github.com/LLJY/ipts-sp6-workaround'
arch=('x86_64')
license=('GPL-2.0-only')
depends=('iptsd' 'udev-hid-bpf>=2.0' 'python' 'systemd' 'util-linux' 'bash' 'grep')
makedepends=('clang' 'bpf' 'libbpf')
install=ipts-sp6-workaround.install
source=('0010-Microsoft__IPTS_SP6_ReportSize.bpf.c' 'hid_bpf.h' 'hid_bpf_helpers.h'
        'recover' 'hid-active' 'ipts-sp6-recover.service' '20-sp6-recovery.conf'
        '80-ipts-sp6-hid-bpf.rules' '51-ipts-sp6-systemd.rules' 'README.md' 'LICENSE')
sha256sums=('42c3f28f9d951c29d39558cb7912472256726f035e3020c884025ebbbac4f002'
            'b213d0e58cf7bdce529befe50c3a96fc9b5c2ce5131d91079edb4129a5f1fc1c'
            '41255c0f05db025423bd487af1f77f6544ab98aca3be827c16b7b8b0d0d7ce6f'
            '394d6696bb7dbb3e72a467be267113ca1cf2fa05adb799ca5e1f4a26fd106adf'
            '2efd96e9b495838387a0b1fe1bbd03004a7a370757f858dabdcc6d4fb1436da4'
            'd272111f4216b88553a967b5ed07ee9ddfb91dc1f8c2ac7f6112ac32c1e10910'
            'bf37c9e182ae21d00c1dc4379bf1a0c72db9cfe6ff11a680f243a6fc0975a235'
            '3a9997529e11f669d5cb8a91ee17905e82a50c4a125beb159e5a0a22c37ccb4a'
            '5835816a23f1ba7da58175c1596dd8744e85ca1f6dad8a742a2ecab4a148ccc9'
            '7f155f659e57b13d4be1b2ceca59da113215b10a337c39410c915af9e47212e2'
            'fd101dd35f436cc2d4c31cda6e7fef93f4f17ccc265123c2934e978bf5119be9')

build() {
    # CO-RE object: no module/kernel compilation, or per-kernel installation.
    # The build host must expose HID-BPF struct_ops in its kernel BTF.
    bpftool btf dump file /sys/kernel/btf/vmlinux format c > vmlinux.h
    clang -target bpf -O2 -g -D__x86_64__ -fms-extensions \
        -Wno-microsoft-anon-tag "-ffile-prefix-map=$srcdir=." -I. \
        -c 0010-Microsoft__IPTS_SP6_ReportSize.bpf.c \
        -o 0010-Microsoft__IPTS_SP6_ReportSize.bpf.o
    udev-hid-bpf inspect 0010-Microsoft__IPTS_SP6_ReportSize.bpf.o >/dev/null
}

package() {
    install -Dm644 0010-Microsoft__IPTS_SP6_ReportSize.bpf.o \
        "$pkgdir/usr/lib/ipts-sp6-workaround/0010-Microsoft__IPTS_SP6_ReportSize.bpf.o"
    install -Dm644 80-ipts-sp6-hid-bpf.rules "$pkgdir/usr/lib/udev/rules.d/80-ipts-sp6-hid-bpf.rules"
    install -Dm644 51-ipts-sp6-systemd.rules "$pkgdir/usr/lib/udev/rules.d/51-ipts-sp6-systemd.rules"
    install -Dm755 recover "$pkgdir/usr/lib/ipts-sp6-workaround/recover"
    install -Dm755 hid-active "$pkgdir/usr/lib/ipts-sp6-workaround/hid-active"
    install -Dm644 ipts-sp6-recover.service "$pkgdir/usr/lib/systemd/system/ipts-sp6-recover.service"
    install -Dm644 20-sp6-recovery.conf "$pkgdir/usr/lib/systemd/system/iptsd@.service.d/20-sp6-recovery.conf"
    install -Dm644 README.md "$pkgdir/usr/share/doc/$pkgname/README.md"
    install -Dm644 LICENSE "$pkgdir/usr/share/licenses/$pkgname/LICENSE"
}
