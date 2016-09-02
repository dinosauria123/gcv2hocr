# gcv2hocr

gcv2hocr converts from Google Cloud Vision OCR output to hocr to make a searchable pdf.


Compile:

gcc main.c -o gcv2hocr


Usage:

gcv2hocr test.jpg.json output.hocr 1280 960

test.jpg.json is a output of Google Cloud Vision OCR.
output.hocr is a output of gcv2hocr.

First number "1280" is image height of the picture.
Second number "960" is image width of the picture.

gcv2hocr also generate preout1.txt and preout2.txt.


How to get OCR (json) data:

Execute gcvocr.sh.

./gcvocr.sh test.jpg "Your API KEY"

gcvocr.sh is slightly modified sample.sh originally written by Kazuhiro FURUHATA (openspc@alpha.ocn.ne.jp).

http://www.openspc2.org/reibun/CloudVision/v1/code/bash/0002/index.html
 

How to make a searchable pdf:

To make a searchable pdf, I used hocr-tools obtained here.
https://github.com/tmbdev/hocr-tools

You may use hocr-pdf to make a searchable pdf.


Acknowledgment:

To Kazuhiro FURUHATA, he gave me a permission redistribute and modify sample.sh.
To Philipp Zumstein, he gave me answers several questions about hocr.


Licence:

Creative Commons Attribution 4.0 International.

You are free to:

    Share — copy and redistribute the material in any medium or format
    Adapt — remix, transform, and build upon the material
    for any purpose, even commercially.

    The licensor cannot revoke these freedoms as long as you follow the license terms.



2016.9.2 ENDO Michiaki (inugami.mamoru@gmail.com)

