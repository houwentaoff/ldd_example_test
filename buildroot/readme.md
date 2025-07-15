**使用buildroot**

# 目得
* 为硬件平台使用buildroot构建sdk或提供第三方包/工具，省去交叉编译

# 关注glibc版本
Toolchain  ---> C library (glibc)
package/glibc/glibc.mk -->GLIBC_VERSION = glibc-2.26-146-gd300041c533a3d837c9f37a099bcc95466860e98

## 强制替换glibc版本
* buildroot 上寻找匹配的版本
* 替换其中的glibc.mk
* 若出现no hash found,则找到该脚本去掉hash校验,继续编译
  hash检查代码在文件 ${BUILDROOT}/support/download/check-hash 中
```sh
if [ ${nb_checks} -eq 0 ]; then
    case " ${BR_NO_CHECK_HASH_FOR} " in
    *" ${base} "*)
        # File explicitly has no hash
        exit 0
        ;;
    esac
    printf "ERROR: No hash found for %s\n" "${base}" >&2
    exit 1 # 此处修改为 exit 0,可使hash检查失效
fi
```
* 切勿用高版本glibc替换低版本容易内核panic.

## 查看glibc支持的版本
strings libc.so.6 |grep GLIBC
ls -l libc.so.6

# 关注依赖的内核头文件
Toolchain  --->Custom kernel headers series (4.15.x) 
若硬件采用的内核版本号太高,buildroot里面不存在，采用新的buildroot版本,勿使用自定义新的，会导致版本超限,make失败.

# 关注硬件板子大小端，Cotex Ax
Target options  ---> Target Architecture ---> AArch64 (little endian)
Target options  ---> Target Architecture Variant (cortex-A72) ---> cortex-A72

# 使用外部编译工具链
外部编译工具链若不可重定向则buildroot不能使用,这时可不用外部工具链而选择buildroot相似的工具链.

# 其它组件
## php 
在buildroot 2019-02版本中，php版本为7.3.2，会出现只生成执行文件`php`而不生成动态库`.so`和静态库`.a`的现象
* 导致该现象的原因，可以在`Makefile`中看到并无`libs/libphp$(PHP_MAJOR_VERSION).bundle: ` 该规则并未在`all`中导致不会主动编译`.so`，且`.bundle`是动态库不应该使用`xxx-gcc`应该使用`xxx-ar cr $@ $? && cp $@ libs/libphp$(PHP_MAJOR_VERSION).so`
* 下文为`configure`生成的错误的`Makefile`
```makefile
 145 libs/libphp$(PHP_MAJOR_VERSION).bundle: $(PHP_GLOBAL_OBJS) $(PHP_SAPI_OBJS)
 146     $(CC) $(MH_BUNDLE_FLAGS) $(CFLAGS_CLEAN) $(EXTRA_CFLAGS) $(LDFLAGS) $(EXTRA_LDFLAGS) $(PHP_GLOBAL_OBJS:.lo=.o) $(PHP_SAPI_OBJS:.lo=.o) $(PHP_FRAMEWORKS) $(EXTRA_LIBS) $(ZEND_EXTRA_LIBS) -o $@ && cp $@ libs/libphp$(PHP_MAJOR_VERSION).so 
```
* 正确的应该如下
```makefile
 145 libs/libphp$(PHP_MAJOR_VERSION).bundle: $(PHP_GLOBAL_OBJS) $(PHP_SAPI_OBJS)
 146     $(AR) cr $@ $(PHP_GLOBAL_OBJS:.lo=.o) $(PHP_SAPI_OBJS:.lo=.o) $(PHP_FRAMEWORKS) && cp $@ libs/libphp$(PHP_MAJOR_VERSION).so 
```
* 手动使用`make libs/libphp7.bundle` 则会生成`*.so`.

# GUI界面
* 使用目录中的.guiconfig 文件是可以进行图形显示的，挂载/移植上buildroot中的rootfs后，首先需要使用buildroot中的./target/usr/bin/modetest 进行条纹测试，用来测试drm 是否正常，正常后进进行下列步骤
* modetest可以使用后，就可以使用libdrm app进行显示(test_drm/test_drm.c)也可以用ffmpeg中的ffplay进行显示图像，若用ffplay显示需要将ffplay中的代码进行细微修改，补丁如下，rgb屏为800x480大小
```ffplay.diff
--- ffmpeg-4.2.4.back/fftools/ffplay.c  2020-07-09 17:17:46.000000000 +0800
+++ ffmpeg-4.2.4/fftools/ffplay.c  2022-08-13 19:24:18.548355221 +0800
@@ -310,8 +310,8 @@
 static AVInputFormat *file_iformat;
 static const char *input_filename;
 static const char *window_title;
-static int default_width  = 640;
-static int default_height = 480;
+static int default_width  = 800;//640;
+static int default_height = 480;//480;
 static int screen_width  = 0;
 static int screen_height = 0;
 static int screen_left = SDL_WINDOWPOS_CENTERED;
@@ -844,6 +844,8 @@
         int pitch;
         if (*texture)
             SDL_DestroyTexture(*texture);
+        //new_format = SDL_PIXELFORMAT_RGBA8888;
+        printf("=>%s %d new_format[0x%x]\n", __func__, __LINE__, new_format);
         if (!(*texture = SDL_CreateTexture(renderer, new_format, SDL_TEXTUREACCESS_STREAMING, new_width, new_height)))
             return -1;
         if (SDL_SetTextureBlendMode(*texture, blendmode) < 0)
@@ -909,6 +911,8 @@
     Uint32 sdl_pix_fmt;
     SDL_BlendMode sdl_blendmode;
     get_sdl_pix_fmt_and_blendmode(frame->format, &sdl_pix_fmt, &sdl_blendmode);
+    printf("=>%s line[%d]frame->format[0x%x] sdl_pix_fmt[0x%x]\n",
+                __func__, __LINE__, frame->format, sdl_pix_fmt);
     if (realloc_texture(tex, sdl_pix_fmt == SDL_PIXELFORMAT_UNKNOWN ? SDL_PIXELFORMAT_ARGB8888 : sdl_pix_fmt, frame->width, frame->height, sdl_blendmode, 0) < 0)
         return -1;
     switch (sdl_pix_fmt) {
@@ -3734,6 +3738,9 @@
             flags |= SDL_WINDOW_BORDERLESS;
         else
             flags |= SDL_WINDOW_RESIZABLE;
+        printf("tom =>%s line[%d]default_width[%d]default_height[%d]flags[%d] \n",
+                    __func__, __LINE__, default_width, default_height, flags);
+        flags = SDL_WINDOW_FULLSCREEN;
         window = SDL_CreateWindow(program_name, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, default_width, default_height, flags);
         SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
         if (window) {
```
* 命令：`./ffplay vd80-17224-1c_9-1-21_7_22pm.mp4` ffplay播放mp4文件

# 非GUI界面
/proc/cmdline : console=ttyS0,9600n8 earlycon=tomuart,mmio32,0x45000000 mem=4084M root=/dev/ram0 rw init=/linuxrc
`tomuart`和`drivers/tty/serial/8250/8250_early.c`中的`OF_EARLYCON_DECLARE(tomuart, "snps,dw-apb-uart", early_searial8250_setup)`对应，这里只使用到了device->con->write 可以屏蔽掉device->con->read实现
dts中uart0打开即可`&uart0{status = "okay";};` 
dts中的bootargs只有在uboot未传递参数的情况下才会起作用，uboot中的参数设定如下和cmdline一致
`bootargs=console=ttyS0,9600n8 earlycon=tomuart,mmio32,0x45000000 mem=4084M root=/dev/ram0 rw init=/linuxrc`
在rootfs中的`dev`目录需执行如下命令`sudo mknod console c  5 1`
这时你就能将rootfs跑起来，这里面需要的buildroot.its文件见同目录





