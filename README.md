# gcv2hocr

gcv2hocr converts from Google Cloud Vision OCR output to hocr to make a searchable pdf.


Compile:

gcc main.c -o gcv2hocr


Usage:

gcv2hocr test.jpg.json output.hocr 1280 960

test.jpg.json is a output of Google Cloud Vision OCR.

I used cloudvisreq.py obtained here:
https://gist.github.com/dannguyen/a0b69c84ebc00c54c94d

output.hocr is a output of gcv2hocr.

First number "1280" is image height of the picture.
Second number "960" is image width of the picture.

gcv2hocr also generate preout1.txt and preout2.txt.

To make a searchable pdf, I used hocr-tools obtained here.
https://github.com/tmbdev/hocr-tools

You may use hocr-pdf to make a searchable pdf.

2016.8.24 ENDO Michiaki (inugami.mamoru@gmail.com)
