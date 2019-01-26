package screenslaver

import (
	"image"
	"image/draw"
	"math"

	"github.com/nfnt/resize"
)

func resizeAndDraw(src image.Image, dst draw.Image, fn resize.InterpolationFunction) {
	srcSize := src.Bounds().Size()
	dstSize := dst.Bounds().Size()

	if srcSize.X == dstSize.X && srcSize.Y == dstSize.Y {
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
