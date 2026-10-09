Locate library qwt version 6.2 in the qwt subfolder.
There is an additional patch required to add one include to compile properly.

How to get the library:
1) git clone -b qwt-6.2 https://github.com/mbdevpl/qwt.git
2) cd qwt
3) git apply -p0 ../patch-compile-windows-16012023.patch

Checked out commit: 7a8f33f470b2752d6eefaaeb833350544b70e131

How I created the patch:

1) Modify code
2)  diff -ur qwt.orig/src/ qwt/src > patch-compile-windows-16012023.patch

How to apply the patch:
1) git apply -p1 <path>/<to>/patch-compile-windows-16012023.patch

