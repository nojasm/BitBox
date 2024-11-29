from PIL import Image
img: Image.Image = Image.open("charset 5x9.png")
pixels = img.load()

cw = 5
ch = 9
w = img.size[0]
h = img.size[1]

nChars = int(w / cw)

# [[true, false, ...], [false, true, ...], ..., <nChars>]
charsetBools = [[] for i in range(nChars)]

for y in range(h):
    for x in range(w):
        charIndex = x // cw
        charsetBools[charIndex].append(pixels[x, y] == (255, 255, 255, 255))

code = ""
code += "vector<vector<bool>> chars = {"
for cb in charsetBools:
    code += "\n  {" + ", ".join([("true" if b else "false") for b in cb]) + "},"
code += "\n};"

with open("charset.hpp", "w+") as cf:
    cf.write(code)