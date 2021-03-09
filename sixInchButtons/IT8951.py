import ctypes

class IT8951():
    def __init__(self,libFileName=None):
        #print("test")
        if libFileName is None:
            self.screen=ctypes.CDLL('/home/pi/eReader/IT8951/IT8951.so')
        else:
            self.screen=ctypes.CDLL(libFileName)

    def init(self):#initializes the screen
        self.screen.IT8951_Init()

    def cancel(self):
        self.screen.IT8951_Cancel()

    def Dev_exit(self):#probably the same?
        self.screen.IT8951_Cancel()

    def Clear(self,color=None):
        if color is None:
            color=255-15#white
        color=ctypes.c_uint8(color)
        self.screen.IT8951DisplayClear(color);

    def sleep(self):
        self.screen.IT8951Sleep()

    def getbuffer(self,image):#PIL image input
        #let's try to get 8 bit working first
        #from PIL import Image
        #if image.mode!='P':#isn't BMP
        #    image=Image.Image.tobitmap(image)
        import numpy as np
        image=np.array(image)
        height,width=image.shape
        matrix=image.flatten(order='C')
        #matrix=np.zeros(image.shape[0]*image.shape[1])
        #for i in range(image.shape[0]):
        #    for j in range(image.shape[1]):
        #        pixel=image[i,j]
        #        #print(pixel)
        #        cPixel=pixel[2]>>3<<11#R
        #        cPixel|=pixel[1]>>2<<5#B
        #        cPixel|=pixel[0]>>3#G
        #        matrix[i*image.shape[1]+j]=cPixel
        return image,height,width

    def display(self,buffer):
        matrix,height,width=buffer
        #should these be variable?
        matrix=matrix.ctypes.data_as(ctypes.POINTER(ctypes.c_uint8))
        x=ctypes.c_uint16(0)
        y=ctypes.c_uint16(0)

        width=ctypes.c_uint16(width)
        height=ctypes.c_uint16(height)
        isSixteen=ctypes.c_uint8(0)#0 8 bit, 1 if 16
        self.screen.IT8951_Matrix_Example(x,y,width,height,matrix,isSixteen)


    def show_BMP(self,x,y,fileName):
        fileName=ctypes.c_char_p(bytes(fileName,'utf-8'))
        x=ctypes.c_uint32(x)
        y=ctypes.c_uint32(y)
        self.screen.IT8951_BMP_Example(x,y,fileName)


    def test(self,image=None):#roughly the same as main.c
        self.init()
        print("Example 1")
        self.screen.IT8951DisplayExample()
        print("Example 2")
        self.screen.IT8951DisplayExample2()
        print("IT8951_GUI_Example")
        self.screen.IT8951_GUI_Example()
        if image is None:
            print("no image provided")
        else:
            self.show_BMP(0,0,image)
        self.cancel()


if __name__=="__main__":
    screen=IT8951()
    #screen.test()#"04.bmp")
    screen.init()
    screen.Clear()
    screen.Dev_exit()
