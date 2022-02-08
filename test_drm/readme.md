## 显示绿色 1920*1080 图片
gcc  -g testdrm.c -I /usr/include/drm/  -Wl,-rpath=/home/tom/work/libdrm-2.4.109/builddir   -ldrm
./a.out

## 显示条纹图片
modetest   -M vc4 -D 0 -a -s 32@74:1920x1080  -P 173@74:1920x1080 -Ftiles