# gcv2hocr
gcv2hocr converts from Google Cloud Vision OCR output to hocr to make a searchable pdf.

<!-- BEGIN-MARKDOWN-TOC -->
* [Installation](#installation)
* [Usage](#usage)
	* [How to get OCR (json) data:](#how-to-get-ocr-json-data)
* [How to make a searchable pdf:](#how-to-make-a-searchable-pdf)
* [Acknowledgments](#acknowledgments)
* [Licence](#licence)

<!-- END-MARKDOWN-TOC -->

## Installation

Compile:

```
make
```

Install to `/usr/local/bin`:

```
make install
```

Install to `$HOME/.local/bin`:

```
make install PREFIX=$HOME/.local
```

Uninstall:

```
make uninstall
```

## Usage

```sh
gcv2hocr test.jpg.json output.hocr
```

`test.jpg.json` is a output of [Google Cloud Vision OCR](https://cloud.google.com/vision/docs/).
`output.hocr` is a output of gcv2hocr.

You may specify image size in argument.

```sh
gcv2hocr test.jpg.json output.hocr 1280 960
```

First number `1280` is image height of the picture.
Second number `960` is image width of the picture.

gcv2hocr also generates intermediary files `preout1.txt` and `preout2.txt`.

You may use gcv2hocr.py instead of gcv2hocr executable.

```sh
python gcv2hocr.py test.jpg.json > output.hocr
```

### How to get OCR (json) data:

Execute gcvocr.sh.

```sh
./gcvocr.sh test.jpg "Your API KEY"
```

gcvocr.sh is slightly modified [sample.sh originally written by Kazuhiro
FURUHATA (openspc@alpha.ocn.ne.jp)](http://www.openspc2.org/reibun/CloudVision/v1/code/bash/0002/index.html).

## How to make a searchable pdf:

To create a searchable pdf, use the `hocr-pdf` script bundled with
[hocr-tools](https://github.com/tmbdev/hocr-tools).

makepdf.sh is a sample shell script to make a searchable pdf from indivisual jpeg files.

## Acknowledgments

To Kazuhiro FURUHATA, he gave me a permission redistribute and modify sample.sh.

To Philipp Zumstein, he gave me answers several questions about hocr.

To Konstantin Baierer, he wrote python port (gcv2hocr.py).

## Licence

```
Creative Commons Attribution 4.0 International.

You are free to:

    Share — copy and redistribute the material in any medium or format
    Adapt — remix, transform, and build upon the material
    for any purpose, even commercially.

    The licensor cannot revoke these freedoms as long as you follow the license terms.
```

2016.9.2 ENDO Michiaki (inugami.mamoru@gmail.com)

