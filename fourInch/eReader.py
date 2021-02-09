#libraries
import os
#import pdf2image
from getkey import getkey, keys
Test=True#False
if not Test:
    from waveshare_epd import epd4in2
    directory="/boot/books/"
else:
    directory="."
import time
from PIL import Image,ImageDraw,ImageFont
import PyPDF2
import textwrap
import slate3k
import tempfile
#constants
WHITE = (0xFF, 0xFF, 0xFF)
BLACK = (0x00, 0x00, 0x00)
TEXT_COLOR = BLACK
BACKGROUND_COLOR = WHITE
FONTSIZE = 23
DLINE=15
WLINE=23

def get_name_list():
   nameList=[]
   for root, dirs, files in os.walk(directory, topdown=False):
      for name in files:
         if root==directory and name.endswith(".pdf"):
            #print(name)
            nameList.append(os.path.join(root, name))
   return nameList

if Test:
    font = ImageFont.truetype('/Library/Fonts/Arial Black.ttf',FONTSIZE)
    width,height=400,300
else:
    font = ImageFont.truetype("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", FONTSIZE)
    epd = epd4in2.EPD()
    epd.init()
    epd.Clear()
    width,height=epd.width,epd.height

def convert_pdf_to_string(file):#_path):
    doc=slate3k.PDF(file)#open(file_path,'rb'))
    #test=slate3k.utils.normalise_whitespace(''.join(doc[0]).replace('\n', ' '))
    return doc.text().replace('- ','')

def extract_text(pdfPage):
    pdf_writer = PyPDF2.PdfFileWriter()
    pdf_writer.addPage(pdfPage)
    tempFile=tempfile.TemporaryFile()#open("temp.pdf",'wb')
    pdf_writer.write(tempFile)
    #tempFile.close()
    tempFile.seek(os.SEEK_SET)
    return convert_pdf_to_string(tempFile)

def selection(nameList):
    selected=0
    done=False
    while not done:
        outNames=''
        for i,name in enumerate(nameList):
            if selected==i:
                outNames+='> '
            else:
                outNames+='  '
            outNames+=name[len(directory)+1:len(directory)+1+20]+"\n"
        image = Image.new("RGB", (height, width))
        draw = ImageDraw.Draw(image)
        draw.rectangle((0, 0, height, width),fill=BACKGROUND_COLOR)
        draw.text((0, 0), outNames, font=font, fill=TEXT_COLOR)
        if Test:
            image.show()#save("test.png")#for testing
        else:
            epd.display(epd.getbuffer(image))#for epaper
            #epd.sleep()#sleep every time?
            #time.sleep(2)#or this kind of sleep? and?
        key = getkey()
        if key == keys.DOWN:
            if selected>=len(nameList):
                selected=0
            else:
                selected+=1
        elif key == keys.UP:
            if selected<=0:
                selected=len(nameList)-1
            else:
                selected-=1
        else:
            done=True
    #print(outName)
    return nameList[selected]

def read_book(name):
    pdf_file = open(name, 'rb')
    read_pdf = PyPDF2.PdfFileReader(pdf_file)
    page=0
    oldPageLen=0
    oldPage=-1
    line=0
    dLine=DLINE
    zoom=1
    quitIt=False
    while not quitIt:
        if page!=oldPage:
            pdfPage = read_pdf.getPage(page)
            text=extract_text(pdfPage)
            text=text.replace('\n',' ')
            oldLen=len(text)+1
            while oldLen!=len(text):
                oldLen=len(text)
                text=text.replace('  ',' ')
            text=text.replace('- ','')
            oldPage=page
        if len(text)<=1:#skip empty pages
            page+=1
            continue
        image = Image.new("RGB", (height, width))
        draw = ImageDraw.Draw(image)
        draw.rectangle(
            (0, 0, height, width),
            fill=BACKGROUND_COLOR
        )
        x,y=0,0
        #print(text)
        #help from https://stackoverflow.com/questions/8257147/wrap-text-in-pil
        textList=textwrap.wrap(text, width=WLINE)
        #text="\n".join(textList)
        #print(font.getsize("\n".join(textList[line:line+dLine])))
        draw.text((x, y), "\n".join(textList[line:line+dLine]), font=font, fill=TEXT_COLOR)
        if Test:
            image.show()#save("test.png")#for testing
        else:
            epd.init()
            epd.display(epd.getbuffer(image))#for epaper
            epd.sleep()#sleep every time?
            #time.sleep(2)#or this kind of sleep? and?
        key = getkey()
        if key == keys.RIGHT:
            line+=dLine
            if line>=len(textList):
                oldPageLen=len(textList)
                line=0
                page+=1
        elif key == keys.LEFT:
            line-=dLine
            if line<0:
                if page>0:
                    page-=1
                line=oldPageLen-dLine
          #else go back to menu, or do nothing?
        #elif key==keys.UP:
        #    zoom+=.1
        #elif key==keys.DOWN:
        #    zoom-=.1
        else:
            quitIt=True
       #elif key == 'a':
       #elif key == 'Y':

if __name__=="__main__":
    nameList=get_name_list()
    #if len(nameList)==0:#TODO
    if len(nameList)>1:#choose pdf to read
        book=selection(nameList)
        read_book(book)
    else:
        read_book(nameList[0])

    if not Test:
        epd.init()
        epd.Clear()
        epd.sleep()
        epd.Dev_exit()
        print("system will shutdown in 5 seconds")
        time.sleep(5)
        os.system("sudo shutdown -h now")
