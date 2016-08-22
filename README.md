# gcv2hocr

gcv2hocr converts from Google Cloud Vision OCR output to hocr to make a searchable pdf.


Compile:

gcc main.c -o gcv2hocr

You must specify image size in main.c before compile.
Default : imgheight=1169 and imgwidth=826.


Usage:

gcv2hocr test.jpg.json output.hocr

test.jpg.json is a output of Google Cloud Vision OCR.

I used cloudvisreq.py obtained here:
https://gist.github.com/dannguyen/a0b69c84ebc00c54c94d

output.hocr is a output of gcv2hocr.

gcv2hocr also generate preout1.txt and preout2.txt.

To make a searchable pdf, I used hocr-tools obtained here.
https://github.com/tmbdev/hocr-tools

You may use hocr-pdf to make a searchable pdf.

2016.8.22 ENDO Michiaki (inugami.mamoru@gmail.com)
