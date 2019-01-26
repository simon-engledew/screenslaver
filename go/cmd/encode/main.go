package main // import "github.com/simon-engledew/screenslaver/go/cmd/encode"

import (
	"image"
	"io/ioutil"
	"log"
	"os"

	"github.com/simon-engledew/screenslaver/go/pkg/screenslaver"

	kingpin "gopkg.in/alecthomas/kingpin.v2"
)

var (
	sourceArg = kingpin.Arg("SOURCE", "image to convert").Required().ExistingFile()
	targetArg = kingpin.Arg("TARGET", "output file").Required().String()
)

func main() {
	kingpin.Version("0.0.1")
	kingpin.Parse()

	fd, err := os.Open(*sourceArg)
	if err != nil {
		log.Fatal(err)
	}
	defer fd.Close()

	src, _, err := image.Decode(fd)
	if err != nil {
		log.Fatal(err)
	}

	data := screenslaver.Encode(src, image.Point{600, 448})

	err = ioutil.WriteFile(*targetArg, data, 0644)
	if err != nil {
		log.Fatal(err)
	}
}
