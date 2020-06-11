import os
import sys

def run(c):
    print(c)
    if os.system(c) != 0:
        print("Error: command failed: " + c)
        sys.exit(1)

png = os.popen("ls -1 *.png").readlines()

pns = []
for p in png:
    p = p.strip()
    pn = p + ".pdf"
    pns.append(pn)
    run("convert " + p + " " + pn)

if len(pns) > 0:
    run("pdfcat " + " ".join(pns) + " out.pdf")



