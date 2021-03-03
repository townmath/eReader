#libraries
import os
#import pdf2image
from getkey import getkey, keys
Test=False#True#
if not Test:
    from waveshare_epd import epd4in2
    directory="/boot/books/"
    #buttons
    import buttonshim
    from evdev import uinput, UInput, ecodes as e
else:
    directory="."
import time
from PIL import Image,ImageDraw,ImageFont
import pickle
#pdf
import PyPDF2
import textwrap
import slate3k
import tempfile
#epub
import ebooklib
from ebooklib import epub
import html2text
import io



#constants
WHITE = (0xFF, 0xFF, 0xFF)
BLACK = (0x00, 0x00, 0x00)
TEXT_COLOR = BLACK
BACKGROUND_COLOR = WHITE
FONTSIZE = 23
DLINE=15
WLINE=23

if not Test:#pimoroni shim buttons
    KEYCODES = [e.KEY_UP, e.KEY_DOWN, e.KEY_LEFT, e.KEY_RIGHT, e.KEY_ENTER]
    BUTTONS = [buttonshim.BUTTON_E, buttonshim.BUTTON_D, buttonshim.BUTTON_C, buttonshim.BUTTON_B, buttonshim.BUTTON_A]

    ui = UInput({e.EV_KEY: KEYCODES}, name="Button-SHIM", bustype=e.BUS_USB)

    @buttonshim.on_press(BUTTONS)
    def button_p_handler(button, pressed):
        keycode = KEYCODES[button]
        print("Press: {}".format(keycode))
        ui.write(e.EV_KEY, keycode, 1)
        ui.syn()


    @buttonshim.on_release(BUTTONS)
    def button_r_handler(button, pressed):
        keycode = KEYCODES[button]
        print("Release: {}".format(keycode))
        ui.write(e.EV_KEY, keycode, 0)
        ui.syn()

def get_name_list():
   nameList=[]
   for root, dirs, files in os.walk(directory, topdown=False):
      for name in files:
         if (root==directory and
             (name.endswith(".pdf") or name.endswith(".epub")) and
             not name.startswith('._')):
            #print(name)
            nameList.append(os.path.join(root, name))
   return nameList

if Test:
    font = ImageFont.truetype('/Library/Fonts/DejaVuSans.ttf',FONTSIZE)
    width,height=400,300
else:
    font = ImageFont.truetype("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", FONTSIZE)
    epd = epd4in2.EPD()
    epd.init()
    epd.Clear()
    width,height=epd.width,epd.height

def convert_pdf_to_string(file):#_path):
    doc=slate3k.PDF(file)#open(file_path,'rb'))
    #print(doc)
    #test=slate3k.utils.normalise_whitespace(''.join(doc[0]).replace('\n', ' '))
    return doc.text().replace('- ','')

def get_image_from_bytes(byteString,image):
    stream=io.BytesIO(byteString)
    newImage= Image.open(stream)
    #image=image.convert("P")
    #image=image.rotate(90, Image.NEAREST, expand = 1)#to match the screen
    h,w=newImage.size
    mult=width/w if w>h else height/h
    newWidth=int(w*mult)
    newHeight=int(h*mult)
    newImage=newImage.resize((newHeight,newWidth),Image.LANCZOS)#if it is a size problem
    image.paste(newImage,((height-newHeight)//2,(width-newWidth)//2))
    print(w,width,newWidth,h,height,newHeight,image)
    return image

def extract_image(imageText,text,book,image):
    print(imageText)
    def get_href_index(text,index):
        hrefStart,hrefEnd=None,None
        for i in range(index,len(text)):
            if text[i]=='(':
                hrefStart=i+1
            elif text[i]==')' and hrefStart is not None:
                hrefEnd=i
                break
        return hrefStart,hrefEnd
    imageText=imageText.replace('\n','')
    index=imageText.find('![')
    hrefStart,hrefEnd=get_href_index(imageText,index)
    print(hrefStart,hrefEnd)
    if hrefEnd is None:#image is across two text blocks
        start=text.find(imageText[index:])
        hrefStart,hrefEnd=get_href_index(text,start)
        if hrefEnd is None:#still across two text blocks, bummer
            return None,text[:start]
        href=text[hrefStart:hrefEnd]
        print(href)
        imgObj=book.get_item_with_href(href)
        image=get_image_from_bytes(imgObj.content,image)
        return image,text[:start]+text[hrefEnd+1:]
    else:
        start=text.find(imageText[index:hrefEnd])
        href=imageText[hrefStart:hrefEnd]
        print(start,hrefEnd,href)
        imgObj=book.get_item_with_href(href)
        image=get_image_from_bytes(imgObj.content,image)
        return image,text[:start]+text[start+(hrefEnd-index)+1:]

def extract_text_pdf(pdfPage):
    # xObject = pdfPage['/Resources'].get('/XObject',None)#.getObject()
    # print(pdfPage.__dict__)
    # print(xObject)
    # image=None
    # if xObject is not None:
    #     for obj in xObject:
    #         if xObject[obj]['/Subtype'] == '/Image':
    #             size = (xObject[obj]['/Width'], xObject[obj]['/Height'])
    #             data = xObject[obj].__dict__['_data']#.getData()
    #             print(xObject[obj]['/Filter'])
    #             image = Image.new("RGB", (height, width))
    #             image=get_image_from_bytes(data,image)
    #             # if xObject[obj]['/ColorSpace'] == '/DeviceRGB':
    #             #     mode = "RGB"
    #             # else:
    #             #     mode = "P"
    #             # if xObject[obj]['/Filter'][0] == '/FlateDecode':
    #             #     img = Image.frombytes(mode, size, data)
    #             #     img.save(obj[1:] + ".png")
    #             # elif xObject[obj]['/Filter'][0] == '/DCTDecode':
    #             #     img = open(obj[1:] + ".jpg", "wb")
    #             #     img.write(data)
    #             #     img.close()
    #             # elif xObject[obj]['/Filter'][0] == '/JPXDecode':
    #             #     img = open(obj[1:] + ".jp2", "wb")
    #             #     img.write(data)
    #             #     img.close()
    # import pdftotext
    # text=pdfPage.extractText()
    # print(text)
    # return text,image
    #too fancy??
    pdf_writer = PyPDF2.PdfFileWriter()
    pdf_writer.addPage(pdfPage)
    tempFile=tempfile.TemporaryFile()#open("temp.pdf",'wb')
    pdf_writer.write(tempFile)
    #tempFile.close()
    tempFile.seek(os.SEEK_SET)
    return convert_pdf_to_string(tempFile)

def extract_text_epub(chapter):
    html = html2text.HTML2Text()
    text=html.handle(chapter.content.decode('utf-8'))#
    return text

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
            outNames+=name[len(directory):len(directory)+20]+"\n"
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
        if not Test:
            buttonshim.set_pixel(0x00,0x00,0x00)
        key = getkey()
        if key == keys.DOWN or key == keys.RIGHT:
            if not Test:
                buttonshim.set_pixel(0x00, 0xff, 0x00)
            if selected>=len(nameList):
                selected=0
            else:
                selected+=1
        elif key == keys.UP or key == keys.LEFT:
            if not Test:
                buttonshim.set_pixel(0xff, 0xff, 0x00)
            if selected<=0:
                selected=len(nameList)-1
            else:
                selected-=1
        else:
            done=True
            if not Test:
                buttonshim.set_pixel(0x99, 0x00, 0x00)#red
    #print(outName)
    return nameList[selected]

def read_book(name):
    if name.endswith('pdf'):
        pdf_file = open(name, 'rb')
        read_pdf = PyPDF2.PdfFileReader(pdf_file)
    elif name.endswith('epub'):
        book = epub.read_epub(name)
        items=[]
        for item in book.get_items_of_type(ebooklib.ITEM_DOCUMENT):
            items.append(item.get_id())
    try:
        pageFile=open(os.path.join(directory,"pageFile.bin"),'rb')
        pages=pickle.load(pageFile)
        page,line=pages.get(name,(0,0))
        pageFile.close()
    except:
        pages=dict()
        page,line=0,0
    oldPageLen=0
    oldPage=-1
    dLine=DLINE
    quitIt=False
    isImage=False
    while not quitIt:
        if page!=oldPage:
            if name.endswith('pdf'):
                #print(page)
                pdfPage = read_pdf.getPage(page)
                text=extract_text_pdf(pdfPage)
                #print(text)
            elif name.endswith('epub'):
                if page>=len(items):
                    print("end of book")
                    page-=1
                chapter=book.get_item_with_id(items[page])
                text=extract_text_epub(chapter)
                #print(text)
            text=text.replace('\n',' ')
            oldLen=len(text)+1
            while oldLen!=len(text):
                oldLen=len(text)
                text=text.replace('  ',' ')
            text=text.replace('- ','')
            oldPage=page
            isImage=False
        if len(text)<=1:#skip empty pages
            page+=1
            continue
        #print(len(text),page,line,chapter)
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
        imageText="\n".join(textList[line:line+dLine])
        if '![' in imageText:#checks for image on page
            #print(imageText)
            image,text=extract_image(imageText,text,book,image)
            isImage=True
            #print(text)
            #print(len(imageText))
        else:
            draw.text((x, y), imageText, font=font, fill=TEXT_COLOR)
            isImage=False
        if Test:
            image.show()#save("test.png")#for testing
        else:
            epd.init()
            epd.display(epd.getbuffer(image))#for epaper
            epd.sleep()#sleep every time?
            #time.sleep(2)#or this kind of sleep? and?
        if not Test:
            buttonshim.set_pixel(0x00,0x00,0x00)#turn off while waiting
        key = getkey()
        if key == keys.RIGHT or key==keys.DOWN:
            if not Test:
                buttonshim.set_pixel(0x94, 0x00, 0xd3)
            if not isImage:#if there was an image keep line the same
                line+=dLine
                if line>=len(textList):
                    oldPageLen=len(textList)
                    line=0
                    page+=1
        elif key == keys.LEFT or key==keys.UP:
            if not Test:
                buttonshim.set_pixel(0x00, 0x00, 0xff)#blue
            isImage=False
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
            if not Test:
                buttonshim.set_pixel(0xff, 0x00, 0x00)#red
            quitIt=True
       #elif key == 'a':
       #elif key == 'Y':
    pages[name]=(page,line)
    return pages

if __name__=="__main__":
    nameList=get_name_list()
    #if len(nameList)==0:#TODO
    if len(nameList)>1:#choose pdf to read
        book=selection(nameList)
        pages=read_book(book)
    else:
        pages=read_book(nameList[0])

    if not Test:
        pageFile=open(os.path.join(directory,"pageFile.bin"),'wb')
        #pages[name]=(page,line)
        pickle.dump(pages,pageFile)
        pageFile.close()
        epd.init()
        epd.Clear()
        epd.sleep()
        epd.Dev_exit()
        print("system will shutdown in 5 seconds")
        for i in range(5):
            print(i)
            buttonshim.set_pixel(0xff, 0x00, 0x00)
            time.sleep(.5)
            buttonshim.set_pixel(0x00, 0x00, 0x00)
            time.sleep(.5)
        os.system("sudo shutdown -h now")
