package main // import "github.com/simon-engledew/screenslaver/go/cmd/server"

import (
	"fmt"
	"image"
	"log"
	"net/http"
	"os"
	"strconv"
	"time"

	"github.com/simon-engledew/screenslaver/go/pkg/screenslaver"

	"github.com/go-chi/chi"
	"github.com/go-chi/chi/middleware"

	kingpin "gopkg.in/alecthomas/kingpin.v2"
)

var (
	imageArg = kingpin.Arg("IMAGE", "image to serve").Required().ExistingFile()
	portFlag = kingpin.Flag("port", "port to listen on").Default("8080").Int()
)

func main() {
	kingpin.Version("0.0.1")
	kingpin.Parse()

	fd, err := os.Open(*imageArg)
	if err != nil {
		log.Fatal(err)
	}
	defer fd.Close()

	src, _, err := image.Decode(fd)
	if err != nil {
		log.Fatal(err)
	}

	data := screenslaver.Encode(src, image.Point{600, 448})

	router := chi.NewRouter()
	router.Use(middleware.Logger)
	router.Use(middleware.Recoverer)
	router.Get("/image", func(w http.ResponseWriter, r *http.Request) {
		header := w.Header()
		header.Set("Content-Length", strconv.Itoa(len(data)))
		header.Set("Content-Type", "application/octet-stream")
		w.WriteHeader(200)
		_, err := w.Write(data)
		if err != nil {
			panic(err)
		}
	})

	server := &http.Server{
		Addr:           fmt.Sprintf(":%d", *portFlag),
		Handler:        router,
		ReadTimeout:    10 * time.Second,
		WriteTimeout:   10 * time.Second,
		MaxHeaderBytes: 1 << 20,
	}

	log.Fatal(server.ListenAndServe())
}
