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
