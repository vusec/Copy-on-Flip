# Copy-on-Flip: Hardening ECC Memory Against Rowhammer Attacks

## Published Work

Paper: [Copy-on-Flip: Hardening ECC Memory Against Rowhammer Attacks.](https://download.vusec.net/papers/cof_ndss23.pdf) __Andrea Di Dio__, Koen Koning, Herbert Bos, Cristiano Giuffrida. In _NDSS_, February 2023.

## Building CoF

The CoF kernel can be built and installed like any other version of the Linux kernel.

__N.B.__ The current code has been written to be optimized and functional on the experimental setup outlined in the paper (Section VII). In order to adapt it to different bare metal systems you will have to set the relevant config flags and adapt the code accordingly (e.g., manually redirect slabs using the `SLAB_COF` and `___GFP_COF`).

## CoF Patch

To quickly view the changes we made to the vanilla 5.4.1 kernel, you can look at the `cof.patch` file. This can be applied as follows:

1. Get the v5.4.1 vanilla kernel version:
   ```bash
   git clone git://git.kernel.org/pub/scm/linux/kernel/git/stable/linux-stable.git
   git checkout v5.4.1
   ```
2. Apply the patch:
   ```bash
   git apply cof.patch
   ```
3. Copy over the kernel config:
   ```bash
   cp cof_kernel/.config linux-stable/
   ```
4. Build and install the kernel:
   ```bash
   make -j`nproc`
   make modules_install
   sudo make install
   ```
