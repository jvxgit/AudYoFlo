1) curl -kLSs https://www.qcustomplot.com/release/2.1.1/QCustomPlot.tar.gz -o qcustomplot.tar.gz
2) tar -xvzf qcustomplot.tar.gz
3) cd qcustomplot
4) patch -p1 < ../qcustomplot-2-1-0-patch-01082023.patch

https://www.shellhacks.com/create-patch-diff-command-linux/

Patch created:
diff -u orig/qcustomplot.cpp qcustomplot/qcustomplot.cpp > my-new-patch.patch

Patch applied:
patch -p1 < ../my-new-patch.patch


