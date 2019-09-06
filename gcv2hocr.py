#!/usr/bin/env python3

import sys
import json
import argparse
from string import Template

try:
    from html import escape  # python 3.x
except ImportError:
    from cgi import escape  # python 2.x

class GCVAnnotation:

    templates = {
        'ocr_page': Template("""<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="$lang" lang="$lang">
  <head>
    <title>$title</title>
    <meta http-equiv="Content-Type" content="text/html;charset=utf-8" />
    <meta name='ocr-system' content='gcv2hocr.py' />
    <meta name='ocr-langs' content='$lang' />
    <meta name='ocr-number-of-pages' content='1' />
    <meta name='ocr-capabilities' content='ocr_page ocr_carea ocr_line ocrx_word ocrp_lang'/>
  </head>
  <body>
    <div class='ocr_page' lang='$lang' title='bbox 0 0 $page_width $page_height'>
        <div class='ocr_carea' lang='$lang' title='bbox $x0 $y0 $x1 $y1'>$content</div>
    </div>
  </body>
</html>
    """),
        'ocr_line': Template("""
            <span class='ocr_line' id='$htmlid' title='bbox $x0 $y0 $x1 $y1; baseline $baseline'>$content
            </span>"""),
        'ocrx_word': Template("""
                <span class='ocrx_word' id='$htmlid' title='bbox $x0 $y0 $x1 $y1'>$content</span>""")
    }

    def __init__(self,
                 htmlid=None,
                 ocr_class=None,
                 lang='unknown',
                 baseline="0 -5",
                 page_height=None,
                 page_width=None,
                 content=None,
                 box=None,
                 title='',
                 savefile=False):
        if content==None:
            self.content = []
        else:
            self.content = content
        self.title = title
        self.htmlid = htmlid
        self.baseline = baseline
        self.page_height = page_height
        self.page_width = page_width
        self.lang = lang
        self.ocr_class = ocr_class
        self.x0 = box[0]['x'] if 'x' in box[0] and box[0]['x'] > 0 else 0
        self.y0 = box[0]['y'] if 'y' in box[0] and box[0]['y'] > 0 else 0
        self.x1 = box[2]['x'] if 'x' in box[2] and box[2]['x'] > 0 else 0
        self.y1 = box[2]['y'] if 'y' in box[2] and box[2]['y'] > 0 else 0

    def maximize_bbox(self):
        self.x0 = min([w.x0 for w in self.content])
        self.y0 = min([w.y0 for w in self.content])
        self.x1 = max([w.x1 for w in self.content])
        self.y1 = max([w.y1 for w in self.content])

    def __repr__(self):
        return "<%s [%s %s %s %s]>%s</%s>" % (self.ocr_class, self.x0, self.y0,
                                              self.x1, self.y1, self.content,
                                              self.ocr_class)
    def render(self):
        if type(self.content) == type([]):
            content = "".join(map(lambda x: x.render(), self.content))
        else:
            content = escape(self.content)
        return self.__class__.templates[self.ocr_class].substitute(self.__dict__, content=content)

def fromResponse(resp, baseline_tolerance=2, **kwargs):
    last_baseline = -100
    page = None
    curline = None
    if isinstance(resp, bool) and not resp:
        box = [{"x": 0, "y": 0}, {"x": 0, "y": 0}, {"x": 0, "y": 0}, {"x": 0, "y": 0}]
        page = GCVAnnotation(
            ocr_class='ocr_page',
            htmlid='page_0',
            box=box,
            **kwargs
        )
    else:
        for anno_idx, anno_json in enumerate(resp['textAnnotations']):
            box = anno_json['boundingPoly']['vertices']
            if anno_idx == 0:
                page = GCVAnnotation(
                    ocr_class='ocr_page',
                    htmlid='page_0',
                    box=box,
                    **kwargs
                    )
                continue

            if not 'description' in anno_json:
                continue

            word = GCVAnnotation(ocr_class='ocrx_word', content=anno_json['description'], box=box)
            #if word.y1-abs(last_baseline) > baseline_tolerance:
            curline = GCVAnnotation(
                    ocr_class='ocr_line',
                    htmlid="line_%d" % (len(page.content)),
                    box=box)
            page.content.append(curline)
            last_baseline = word.y1
            word.htmlid="word_%d_%d" % (len(page.content) - 1, len(curline.content))
            curline.content.append(word)
        for line in page.content:
            line.maximize_bbox()
        page.maximize_bbox()
    if not page.page_width: page.page_width = page.x1
    if not page.page_height: page.page_height = page.y1
    return page

if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('gcv_file', help='GCV JSON file, "-" for STDIN')
    parser.add_argument(
        "--baseline",
        "-B",
        help="Baseline offset",
        metavar="pn pn-1 ...",
        default="0 0")
    parser.add_argument(
        "--baseline-tolerance",
        "-T",
        help="Y Tolerance to recognize same line. Default: 2",
        metavar="INT",
        type=int,
        default=2)
    parser.add_argument(
        "--title",
        "-t",
        help="Document title")
    parser.add_argument(
        "--lang",
        "-L",
        default='unknown',
        help="Language")
    parser.add_argument(
        "--page-width",
        "-W",
        help="Image width. Automatically detected unless specified")
    parser.add_argument(
        "--page-height",
        "-H",
        help="Image height. Automatically detected unless specified")
    parser.add_argument(
        "--savefile",
        help="Save to this file instead of outputting to stdout"
    )
    args = parser.parse_args()

    instream = sys.stdin if args.gcv_file is '-' else open(args.gcv_file, 'r', encoding='utf-8' )
    resp = json.load(instream)
    resp = resp['responses'][0] if 'responses' in resp and len(resp['responses']) >= 0 and "textAnnotations" in resp['responses'][0] else False
    del(args.gcv_file)
    page = fromResponse(resp, **args.__dict__)

    if args.savefile:
        with (open(args.savefile, 'w', encoding="utf-8")) as outfile:
            outfile.write(page.render().encode('utf-8') if str == bytes else page.render())
            outfile.close()
    else:
        if str == bytes:
            print(page.render().encode('utf-8'))
        else:
            print(page.render())
