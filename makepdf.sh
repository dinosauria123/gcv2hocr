#!/bin/sh

a=1
while [ $a -le 32 ]
do
    if [ $a -le 9 ] ; then
	   number="00"$a
	fi
	
    if [ $a -ge 10 -a $a -le 99 ] ; then
	   number="0"$a
	fi
	
    if [ $a -ge 100 ] ; then
	   number=$a
	fi
	
    echo "Google OCR page$number.jpg"
    sh ./gcvocr.sh page$number.jpg [Your API Key]

    echo "Convert page$number.hocr"
    ./gcv2hocr page$number.jpg.json page$number.hocr

    a=`expr $a + 1`
done

echo "Generating out.pdf"
python hocr-pdf --savefile out0.pdf ./

echo "Reducing pdf size"
gs -sDEVICE=pdfwrite -dCompatibilityLevel=1.4 -dPDFSETTINGS=/default -dNOPAUSE -dQUIET -dBATCH -dAutoRotatePages=/None -sOutputFile=out.pdf out0.pdf
