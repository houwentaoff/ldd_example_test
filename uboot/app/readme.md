# uboot启动裸机程序

## 如何测试

1. `make`
3. `uboot: loady txt-sections `
2. `uboot: go main.addr (Entry point address,asm中的main函数地址)`

## 如何编写ld文件
`ld --verbose`会打印出默认的lds; lds已经打包进`ld` 程序里面去了
lds文件中能看到txt段并不死放在开头，提前即可.

## 如何将elf转换成bin文件
* bin文件应该全为data，即`file hello.bin` 结果应为data,如果不为data说明含有文件头信息被`file`命令解析出来了
* bin应该比elf小很多，如果不是，原因都由`ld`链接文件导致,请仔细检查,如txt段前有头文件信息或`--gap-fill=0xff`

## 如何显示main地址
* 1
``` hello.ld
ENTRY(main) #显示如下的 entry point address

```
```bash
`readelf -h hello.elf`
ELF Header:
  Magic:   7f 45 4c 46 02 01 01 00 00 00 00 00 00 00 00 00
  Class:                             ELF64
  Data:                              2's complement, little endian
  Version:                           1 (current)
  OS/ABI:                            UNIX - System V
  ABI Version:                       0
  Type:                              EXEC (Executable file)
  Machine:                           AArch64
  Version:                           0x1
  Entry point address:               0x800560
  Start of program headers:          64 (bytes into file)
  Start of section headers:          4777984 (bytes into file)
  Flags:                             0x0
  Size of this header:               64 (bytes)
  Size of program headers:           56 (bytes)
  Number of program headers:         5
  Size of section headers:           64 (bytes)
  Number of section headers:         37
  Section header string table index: 34
```
* 2
  从反汇编asm文件中可以找到main的地址

### 如何运行
`go 0x800560` 直接从main启动，由于没有从start.s的_start开始执行汇编所以glibc并没有初始化，构造函数并不能执行，malloc也不能被执行
printf建议使用uboot自身的地址强制赋值使用即可.另外还需注意字符串后面必须加'\0'，都斯因为没有初始化，不过初始化的全局变量还是能正
常使用
