# screenslaver

Prototype ESP8266+Waveshare E-ink client for a backend written in Golang:

![waffle](https://user-images.githubusercontent.com/14410/68798613-9e396a00-064e-11ea-8ec0-55877b4e2818.jpg)

Golang will resize and dither the image, then serve it via HTTP.

The e-ink client will connect to Wifi via WPS and then download and render the image.

Currently a bunch of stuff is hard-coded. If anyone stumbles across this and is interested in getting it working for themselves, pop me an issue and I will put some more time into it.
