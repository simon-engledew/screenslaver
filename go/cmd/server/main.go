package main // import "github.com/simon-engledew/screenslaver/go/cmd/server"

import (
	"flag"
	"image"
	"image/color"
	"image/draw"
	"log"
	"math"
	"net"
	"os"

	"github.com/MaxHalford/halfgone"
	"github.com/nfnt/resize"
)

func resizeImage(src image.Image, dst draw.Image, fn resize.InterpolationFunction) {
	srcSize := src.Bounds().Size()
	dstSize := dst.Bounds().Size()

	if srcSize.X == dstSize.X && srcSize.Y == dstSize.Y {
		log.Println("Image is correct size")
		draw.Draw(
			dst,
			dst.Bounds(),
			src,
			image.ZP,
			draw.Over,
		)
		return
	}

	w, h := dstSize.X, dstSize.Y
	iw, ih := srcSize.X, srcSize.Y

	if iw > w {
		ih = int(math.Max(float64(srcSize.Y)*float64(w)/float64(srcSize.X), 1.0))
		iw = w
	}
	if ih > h {
		iw = int(math.Max(float64(srcSize.X)*float64(h)/float64(srcSize.Y), 1.0))
		ih = h
	}

	draw.Draw(
		dst,
		image.Rect(0, 0, iw, ih).Add(image.Pt((w-iw)/2, (h-ih)/2)),
		resize.Resize(uint(iw), uint(ih), src, fn),
		image.ZP,
		draw.Over,
	)
}

func encodeImage(src image.Image, size image.Point) []uint8 {
	dst := image.NewGray(image.Rect(0, 0, size.X, size.Y))
	draw.Draw(dst, dst.Bounds(), &image.Uniform{color.White}, image.ZP, draw.Src)

	resizeImage(src, dst, resize.Lanczos3)

	dst = halfgone.JarvisJudiceNinkeDitherer{}.Apply(dst)

	row := int(size.X / 8)

	data := make([]uint8, size.Y*row)

	for y := 0; y < size.Y; y++ {
		offset := y * row
		for x := 0; x < size.X; x++ {
			white := dst.At(x, y).(color.Gray).Y == 255

			if white {
				data[offset+(x/8)] |= 1 << uint(x%8)
			}
		}
	}

	return data
}

func handleRequest(data []uint8, conn net.Conn) {
	defer conn.Close()

	_, err := conn.Write(data)

	if err != nil {
		log.Print(err)
	}
}

func main() {
	flag.Parse()

	fd, err := os.Open(flag.Arg(0))
	if err != nil {
		panic(err)
	}
	src, _, err := image.Decode(fd)
	if err != nil {
		panic(err)
	}

	data := encodeImage(src, image.Point{600, 448})

	log.Println("Image encoded")

	listener, err := net.Listen("tcp", ":8000")
	if err != nil {
		panic(err)
	}
	defer listener.Close()

	for {
		conn, err := listener.Accept()
		if err != nil {
			panic(err)
		}

		go handleRequest(data, conn)
	}
}
