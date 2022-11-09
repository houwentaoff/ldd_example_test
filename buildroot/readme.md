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
* modetest可以使用后，就可以使用libdrm app进行显示(test_drm/test_drm.c)也可以用ffmpeg中的ffplay进行显示图像，若用ffplay显示需要将ffplay中的代码进行细微修改，补丁如下
```ffplay.diff

```
* 命令：`./ffplay vd80-17224-1c_9-1-21_7_22pm.mp4` ffplay播放mp4文件




