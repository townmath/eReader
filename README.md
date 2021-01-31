# eReader
This is my attempts at making an inexpensive, open source, eReader.  Apart from the high-minded ideals of an open source reader, I also made this because the fact that I still print stuff out on physical paper makes me sad.  

If you use my (or their) IT8951 code you need to compile it as a shared library like so:    
`gcc -shared -Wall -Wl,-soname,IT8951 IT8951.c miniGUI.c main.c AsciiLib.c bmp.c -o IT8951.so -lbcm2835`.    
Thanks to https://stackoverflow.com/questions/5081875/ctypes-beginner for the format.     
