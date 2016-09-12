#!/bin/bash

test_sample () {
    base="$1"
    echo >&2 "Testing sample $base"
    # echo ./gcv2hocr.py "$base.jpg.json" '>' "$base.hocr"
    ./gcv2hocr.py "$base.jpg.json" > "$base.hocr"
    hocr-spec "$base.hocr"
    hocr-pdf "$(dirname "$base")" > "$base.pdf"
    # x-www-browser "$base.pdf"
}

test_sample sample/eng/test
test_sample sample/jpn/jptest2
