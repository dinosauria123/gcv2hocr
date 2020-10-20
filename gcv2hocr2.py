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

    height = None
    width = None

    templates = {
        'ocr_page': Template("""<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="$lang" lang="$lang">
  <head>
    <title>HOCR File</title>
    <meta http-equiv="Content-Type" content="text/html;charset=utf-8" />
    <meta name='ocr-system' content='gcv2hocr.py' />
    <meta name='ocr-langs' content='$lang' />
    <meta name='ocr-number-of-pages' content='1' />
    <meta name='ocr-capabilities' content='ocr_page ocr_carea ocr_par ocr_line ocrx_word ocrp_lang'/>
  </head>
  <body>
    <div class='ocr_page' lang='$lang' title='image "$title";bbox 0 0 $page_width $page_height'>
        <div class='ocr_carea' lang='$lang' title='bbox $x0 $y0 $x1 $y1'>$content</div>
    </div>
  </body>
</html>
    """),
        'ocr_line': Template("""
            <span class='ocr_line' id='$htmlid' title='bbox $x0 $y0 $x1 $y1; baseline $baseline'>$content
            </span>"""),
        'ocrx_word': Template("""
                <span class='ocrx_word' id='$htmlid' title='bbox $x0 $y0 $x1 $y1'>$content</span>"""),
        'ocr_carea':Template("""
                <div class='ocr_carea' id='$htmlid' title='bbox $x0 $y0 $x1 $y1'>$content</div>"""),
        'ocr_par': Template("""
                <p class='ocr_par' dir='ltr' id='$htmlid' title='bbox $x0 $y0 $x1 $y1'>$content</p>""")

    }

    def __init__(self,
                 htmlid=None,
                 ocr_class=None,
                 lang='unknown',
                 baseline="0 0",
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
        self.page_height = GCVAnnotation.height if GCVAnnotation.height else page_height
        self.page_width = GCVAnnotation.width if GCVAnnotation.width else page_width
        self.lang = lang
        self.ocr_class = ocr_class
        try:
            self.x0 = int(float(self.page_width*(box[0]['x'] if 'x' in box[0] and box[0]['x'] > 0 else 0)))
            self.y0 = int(float(self.page_height*(box[0]['y'] if 'y' in box[0] and box[0]['y'] > 0 else 0)))
            self.x1 = int(float(self.page_width*(box[2]['x'] if 'x' in box[2] and box[2]['x'] > 0 else 0)))
            self.y1 = int(float(self.page_height*(box[2]['y'] if 'y' in box[2] and box[2]['y'] > 0 else 0)))
        except ValueError as e:
            output = 'Input JSON does not have proper boundingBox values. ' \
                      'This page of the PDF either must not have been ' \
                      'parsed correctly by GCV or the JSON is somehow corrupt.'
            print(e, "\n", output)

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

def fromResponse(resp, file_name, baseline_tolerance=2, **kwargs):
    # last_baseline = -100
    page = None
    curline = None
    if isinstance(resp, bool) and not resp:
        box = [{"x": 0, "y": 0}, {"x": 0, "y": 0}, {"x": 0, "y": 0}, {"x": 0, "y": 0}]
        page = GCVAnnotation(
            ocr_class='ocr_page',
            htmlid='page_0',
            box=box,
            title = file_name
        )
    else:
        for page_id, page_json in enumerate(resp['responses'][0]['fullTextAnnotation']['pages']):
          box = [{"x": 0, "y": 0}, {"x": 0, "y": 0}, {"x": 0, "y": 0}, {"x": 0, "y": 0}]
          GCVAnnotation.height = page_json.get('width')
          GCVAnnotation.width = page_json.get('height')
          page = GCVAnnotation(
                    ocr_class='ocr_page',
                    htmlid='page'+ str(page_id),
                    box=box,
                    title=file_name
                    )
          for block_id, block_json in enumerate(page_json['blocks']):
            box = block_json['boundingBox']['normalizedVertices']
            block = GCVAnnotation(ocr_class='ocr_carea',htmlid="block_%d" % block_id, box=box)
            page.content.append(block)

            line_id = 0
            for paragraph_id, paragraph_json in enumerate(block_json['paragraphs']):
                box = paragraph_json['boundingBox']['normalizedVertices']
                par = GCVAnnotation(ocr_class='ocr_par',htmlid="par_"+ str(block_id) + "_" + str(paragraph_id), box=box)
                block.content.append(par)
                curline = GCVAnnotation(ocr_class='ocr_line', htmlid="line_"+ str(block_id) + "_" + str(paragraph_id)+"_"+ str(line_id), box=box)
                par.content.append(curline)
                for word_id, word_json in enumerate(paragraph_json['words']):
                    box = word_json['boundingBox']['normalizedVertices']
                    word_text = ''.join([
                        symbol['text'] for symbol in word_json['symbols']
                    ])
                    word = GCVAnnotation(ocr_class='ocrx_word', htmlid="word_"+ str(block_id) + "_" + str(paragraph_id)+"_"+ str(word_id), content=word_text, box=box)

                    curline.content.append(word)
                    for symbol_id, symbol_json in enumerate(word_json['symbols']):
                         if 'property' in symbol_json and 'detectedBreak' in symbol_json['property'] and 'type' in symbol_json['property']['detectedBreak']:
                             if symbol_json['property']['detectedBreak']['type'] == "LINE_BREAK" or symbol_json['property']['detectedBreak']['type'] == "EOL_SURE_SPACE":
                                 line_id = line_id + 1
                                 curline = GCVAnnotation(ocr_class='ocr_line', htmlid="line_"+ str(block_id) + "_" + str(paragraph_id)+"_"+ str(line_id), box=box)
                                 par.content.append(curline)
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

    instream = sys.stdin if (args.gcv_file == '-') else open(args.gcv_file, 'r', encoding='utf-8' )
    resp = json.load(instream)
    page = fromResponse(resp, str(args.gcv_file.rsplit('.',1)[0]), **args.__dict__)

    if args.savefile:
        with (open(args.savefile, 'w', encoding="utf-8")) as outfile:
            outfile.write(page.render().encode('utf-8') if str == bytes else page.render())
            outfile.close()
    else:
        if str == bytes:
            print(page.render().encode('utf-8'))
        else:
            print(page.render())
