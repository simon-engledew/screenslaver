package screenslaver // import "github.com/simon-engledew/screenslaver/go/pkg/screenslaver"

import (
	"image"
	"image/color"
	"image/draw"

	"github.com/MaxHalford/halfgone"
	"github.com/nfnt/resize"
)

func Encode(src image.Image, size image.Point) []uint8 {
	dst := image.NewGray(image.Rect(0, 0, size.X, size.Y))
	draw.Draw(dst, dst.Bounds(), &image.Uniform{color.White}, image.ZP, draw.Src)

	resizeAndDraw(src, dst, resize.Lanczos3)

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
