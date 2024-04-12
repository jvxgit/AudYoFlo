.function volume_orc_double_ip
.dest 8 d1 double
.doubleparam 8 p1

muld d1, d1, p1

.function volume_orc_float_ip
.dest 4 d1 float
.floatparam 4 p1

mulf d1, d1, p1

.function volume_orc_double
.dest 8 d1 double
.source 8 p1 double
.doubleparam 8 p2

muld d1, p1, p2

.function volume_orc_float
.dest 4 d1 float
.source 4 p1 float
.floatparam 4 p2

mulf d1, p1, p2
