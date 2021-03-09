import os,pdf2image,pickle
from getkey import getkey, keys
Test=False#
if not Test:
    import IT8951
    directory="/boot/books/"
    #buttons
    import buttonshim
    from evdev import uinput, UInput, ecodes as e
else:
    directory="."
import time
from PIL import Image,ImageDraw,ImageFont
height,width=800,600
font=ImageFont.truetype("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",24)

if not Test:#pimoroni shim buttons
    KEYCODES = [e.KEY_LEFT, e.KEY_UP, e.KEY_ENTER, e.KEY_DOWN, e.KEY_RIGHT]
    BUTTONS = [buttonshim.BUTTON_A, buttonshim.BUTTON_B, buttonshim.BUTTON_C, buttonshim.BUTTON_D, buttonshim.BUTTON_E]

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

def selection(nameList):
    selected=0
    done=False
    while not done:
        outNames=50*' '+'\n'
        for i,name in enumerate(nameList):
            if selected==i:
                outNames+='> '
            else:
                outNames+='  '
            outNames+=name+"\n"
        image = Image.new("P", (width, height),255)
        draw = ImageDraw.Draw(image)
        #draw.rectangle((0, 0, width, height),fill=255)
        draw.text((0,0), outNames, font=font, fill=0,align='center')
        if Test:
            image.show()#save("test.png")#for testing
        else:
            image=image.rotate(90,expand=1)
            epd.display(epd.getbuffer(image))#for epaper
            #epd.sleep()#sleep every time?
            #time.sleep(2)#or this kind of sleep? and?
        if not Test:
            buttonshim.set_pixel(0x00,0x00,0x00)
        key = getkey()
        if key in [keys.DOWN,keys.RIGHT]:
            if not Test:
                buttonshim.set_pixel(0x00, 0xff, 0x00)
            if selected>=len(nameList):
                selected=0
            else:
                selected+=1
        elif key in [keys.UP,keys.LEFT]:
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

#https://stackoverflow.com/questions/46149003/pil-zoom-into-image-at-a-particular-point
def zoom_it(img, zoom):
    w, h = img.size
    x,y=w//2,h//2
    zoom2 = zoom * 2
    img = img.crop((x - w / zoom2, y - h / zoom2,
                    x + w / zoom2, y + h / zoom2))
    return img.resize((w, h), Image.LANCZOS)

def get_name_list():
   nameList=[]
   for root, dirs, files in os.walk(directory, topdown=False):
      for name in files:
         if root==directory and name.lower().endswith(".pdf") and not name.startswith("."):
            #TODO add output and selection
            print(name)
            nameList.append(name)
            #print(name)#os.path.join(root, name))
   return nameList

if __name__=="__main__":
    if not Test:
        epd = IT8951.IT8951()
        epd.init()
        epd.Clear()

    nameList=get_name_list()
    #if len(nameList)==0:#TODO
    if len(nameList)>1:#choose pdf to read
        name=selection(nameList)
    elif len(nameList)==1:
        name=nameList[0]
    else:
        print("no books")
    try:
        pageFile=open(os.path.join(directory,"pageFile.bin"),'rb')
        pages=pickle.load(pageFile)
        page,zoom=pages.get(name,(1,1))
        pageFile.close()
    except:
        pages=dict()
        page,zoom=1,1
    asleep=False
    quitIt=False
    while not quitIt:
        images = pdf2image.convert_from_path(os.path.join(directory,name),first_page=page,last_page=page,#)#
                                            #use_cropbox=False)#,single_file=True)
                                            grayscale=True)
        image=zoom_it(images[0],zoom)
        image=image.rotate(90, Image.NEAREST, expand = 1)#to match the screen
        image=image.resize((800,600),Image.LANCZOS)#if it is a size problem
        #image=image.convert('P')#in case it needs to be P
        if Test:
            #img_to_numpy_test(image)
            image.show()#save("test.png")#for testing
            print(image)
            print(image.mode)
            #Himage = Image.open('4in2.bmp')
            #print(Himage)
            #print(Himage.mode)
        elif not asleep:
            epd.display(epd.getbuffer(image))#for epaper
            #epd.sleep()#sleep every time?
            #time.sleep(2)
        if not Test:
            buttonshim.set_pixel(0x00,0x00,0x00)
        key = getkey()
        if key == keys.RIGHT:
            if not Test:
                buttonshim.set_pixel(0x94, 0x00, 0xd3)
            page+=1
        elif key == keys.LEFT:
            if not Test:
                buttonshim.set_pixel(0x00, 0x00, 0xff)#blue
            if page>0:
                page-=1
          #else go back to menu, or do nothing?
        elif key==keys.UP:
            if not Test:
                buttonshim.set_pixel(0xff, 0xff, 0x00)
            zoom-=.1
        elif key==keys.DOWN:
            if not Test:
                buttonshim.set_pixel(0x00, 0xff, 0x00)
            zoom+=.1
        else:
            if not Test:
                buttonshim.set_pixel(0x99, 0x00, 0x00)#red
            if asleep:
                quitIt=True
            elif not Test:
                asleep=True
                epd.sleep()
            else:
                quitIt=True
        if asleep and key in [keys.RIGHT,keys.LEFT,keys.DOWN,keys.UP]:
            asleep=False
            epd.init()
       #elif key == 'a':
       #elif key == 'Y':
    if not Test:
        pageFile=open(os.path.join(directory,"pageFile.bin"),'wb')
        pages[name]=(page,zoom)
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
