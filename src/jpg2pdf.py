import os
import sys

def run(c):
    print(c)
    if os.system(c) != 0:
        print("Error: command failed: " + c)
        sys.exit(1)

png = os.popen("ls -1 *.jpg").readlines()

pns = []
for p in png:
    p = p.strip()
    pn = p + ".pdf"
    pns.append(pn)
    # run("convert -gravity center -resize 2550x3300 -density 300 -units pixelsperinch -background Blue -extent 2550x3300 " + p + " " + pn)
    run(" ".join(["convert", p, "-resize 1240x1753 -extent 1240x1753 -gravity center -units PixelsPerInch -density 300x300", pn]))
if len(pns) > 0:
    run("pdfcat " + " ".join(pns) + " out.pdf")
'''
ImageMagick has some security policies disabling some rights for security reasons. See why at the bottom of this answer.
You will have to edit a config file to re-enble the action you need.

Open /etc/ImageMagick-6/policy.xml with your favorite text editor, find the line <policy domain="coder" rights="none" pattern="PDF" /> and replace "none" by "read|write"

# Open the file 
sudo nano /etc/ImageMagick-6/policy.xml

# find and edit the line
<policy domain="coder" rights="none" pattern="PDF" />
# to :
<policy domain="coder" rights="read|write" pattern="PDF" />
'''
