.PHONY: all
all: bin/encode bin/server

.SECONDEXPANSION:
$(wildcard go/cmd/*/main.go): $$(dir $$@)/go.mod $$(dir $$@)/go.sum $(wildcard go/pkg/screenslaver/*)
	touch $@

bin/encode: go/cmd/encode/main.go
	(cd $(dir $<); go build -o $(abspath $@) -ldflags='-s -w' $(notdir $<))

bin/server: go/cmd/server/main.go
	(cd $(dir $<); go build -o $(abspath $@) -ldflags='-s -w' $(notdir $<))

.PHONY: static
static: $(wildcard static/*)
	(cd static; surge . screenslaver.surge.sh)
