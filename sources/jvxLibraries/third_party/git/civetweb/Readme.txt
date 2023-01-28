Locate library civetweb. There is an additional patch required to add one include to compile properly.

How to get the library:
1) git clone https://github.com/civetweb/civetweb.git
2) cd civetweb
3) git apply -p1 ../minor-patch-due-to-cpp.patch

Checked out commit:  8cc6a8e5bf8bd6cfb7e9c92958a7af555301293f 

How I created the patch:

1) Modify code
2) diff -ur civetweb.orig/src/ civetweb/src > my-new-patch-x86.patch


