package main

/***********
1. 利用framebuffer接口 截取屏幕截图
2. go调用unix系统调用
带有Raw前缀的一组操作表示直接调用syscall
而不带Raw前缀的操作则在真正调用syscall前会先调用
runtime.entersyscall，并在syscall返回后插入runtime.exitsyscall
据了解非阻塞的常用Raw.



***********/

import (
	"fmt"
	"image"
	"image/png"
	"os"
	"syscall"
	"unsafe"
	/*
	   "image/png"
	   "bytes"
	*/)

var (
	FBIOGET_VSCREENINFO = 0x4600
)

type fbBitfield struct {
	offset    uint32 /* beginning of bitfield */
	length    uint32 /* length of bitfield */
	msb_right uint32 /* != 0 : Most significant bit is */
}

type fbVarInfo struct {
	xres         uint32 /* visible resolution */
	yres         uint32
	xres_virtual uint32 /* virtual resolution */
	yres_virtual uint32
	xoffset      uint32 /* offset from virtual to visible */
	yoffset      uint32 /* resolution */

	bits_per_pixel uint32 /* guess what */
	grayscale      uint32 /* 0 = color, 1 = grayscale, */
	/* >1 = FOURCC          */
	red    fbBitfield /* bitfield in fb mem if true color, */
	green  fbBitfield /* else only length is significant */
	blue   fbBitfield
	transp fbBitfield /* transparency         */

	nonstd uint32 /* != 0 Non standard pixel format */

	activate uint32 /* see FB_ACTIVATE_* */

	height uint32 /* height of picture in mm */
	width  uint32 /* width of picture in mm */

	accel_flags uint32 /* (OBSOLETE) see fb_info.flags */

	/* Timing: All values in pixclocks, except pixclock (of course) */
	pixclock     uint32 /* pixel clock in ps (pico seconds) */
	left_margin  uint32 /* time from sync to picture */
	right_margin uint32 /* time from picture to sync */
	upper_margin uint32 /* time from sync to picture */
	lower_margin uint32
	hsync_len    uint32    /* length of horizontal sync */
	vsync_len    uint32    /* length of vertical sync */
	sync         uint32    /* see FB_SYNC_* */
	vmode        uint32    /* see FB_VMODE_* */
	rotate       uint32    /* angle we rotate counter clockwise */
	colorspace   uint32    /* colorspace for FOURCC-based modes */
	reserved     [4]uint32 /* Reserved for future compatibility */
}

func main() {
	fb, err := os.Open("/dev/fb0")
	if err != nil {
		panic("can't open file /dev/fb0")
	}
	defer fb.Close()
	fmt.Printf("file open %v \n", fb.Fd())

	var varInfo fbVarInfo
	_, _, errno := syscall.RawSyscall(syscall.SYS_IOCTL, fb.Fd(), uintptr(FBIOGET_VSCREENINFO), uintptr(unsafe.Pointer(&varInfo)))
	if errno != 0 {
		fmt.Println(errno)
		panic("can't ioctl ... ")
	}
	fmt.Printf("width = %d height = %d\n", varInfo.xres, varInfo.yres)
	fmt.Printf("xoffset = %d yoffset = %d\n", varInfo.xoffset, varInfo.yoffset)
	fmt.Printf("depth = %d\n", varInfo.bits_per_pixel)

	// func Mmap(fd int, offset int64, length int, prot int, flags int) (data []byte, err error)
	bpp := varInfo.bits_per_pixel / 8
	size := varInfo.xres * varInfo.yres * bpp
	offset := varInfo.xoffset*bpp + varInfo.xres*bpp*varInfo.yoffset
	mapsize := size + offset
	fmt.Printf("mapsize = %d\n", mapsize)
	fmt.Printf("red:offset:%d len:%d\n", varInfo.red.offset, varInfo.red.length)
	fmt.Printf("green:offset:%d len:%d\n", varInfo.green.offset, varInfo.green.length)
	fmt.Printf("blue:offset:%d len:%d\n", varInfo.blue.offset, varInfo.blue.length)
	fmt.Printf("alpha:offset:%d len:%d\n", varInfo.transp.offset, varInfo.transp.length)
	//这里不能使用syscall.MAP_SHARED,提示权限不够，也不能使用PROT_WRITE同样是权限不够，不知何故?
	data, err := syscall.Mmap(int(fb.Fd()), 0, int(mapsize), syscall.PROT_READ /*|syscall.PROT_WRITE*/, syscall.MAP_SHARED)
	if err != nil {
		fmt.Println(err)
		panic("map failed ...")
	}
	defer syscall.Munmap(data)

	// save as png image.
	//
	// func Encode(w io.Writer, m image.Image) error
	//varInfo.xres, varInfo.yres
	m := image.NewRGBA(image.Rect(0, 0, int(varInfo.xres), int(varInfo.yres)))
	//image.
	screen := data[offset:]
	fmt.Println("size screen:", len(screen))
	m.Pix = screen
	/*
		for i, _ := range m.Pix {
			m.Pix[i] = 0xff
			//data[i] = 0
		}
	*/
	/*
		func(in []byte) []byte {
			var out []byte

			for key, value := range in {
				//blue
				out[key/4] = in[key*4+0]
				//green
				out[key/4+1] = in[key*4+1]
				//red
				out[key/4+2] = in[key*4+2]
				//alpha
				out[key/4+3] = in[key*4+3]
			}

		}(screen)
	*/
	/*
		m, err := png.Decode(bytes.NewBuffer(screen))
		if err != nil {
			panic("decode fail...")
		}*/
	output, err := os.OpenFile("screenshot.png", os.O_WRONLY|os.O_CREATE, 0666)
	if err != nil {
		panic("can't open file screenshot.png")
	}
	defer output.Close()
	err = png.Encode(output, m)
	if err != nil {
		panic("encoding fail...")
	}

}
