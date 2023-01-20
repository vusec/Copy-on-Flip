# Copy-on-Flip: Hardening ECC Memory Against Rowhammer Attacks

## Published Work

Paper: [Copy-on-Flip: Hardening ECC Memory Against Rowhammer Attacks.](https://download.vusec.net/papers/cof_ndss23.pdf) __Andrea Di Dio__, Koen Koning, Herbert Bos, Cristiano Giuffrida. In _NDSS_, February 2023.

## Building CoF

The CoF kernel can be built and installed like any other version of the Linux kernel.

__N.B.__ The current code has been written to be optimized and functional on the experimental setup outlined in the paper (Section VII). In order to adapt it to different bare metal systems you will have to set the relevant config flags and adapt the code accordingly.
