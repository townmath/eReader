#!/usr/bin/python3

import binascii
import struct
import textwrap

class LineInfo:
    def __init__(self, offset, length):
        self.offset = offset
        self.length = length

class Book:
    def __init__(self, title = "", author = "", text = ""):
        self.title = title
        self.author = author
        self.text = text
    
    def generate_obk(self):
        data = bytearray()
        line_lut = list()
        magic = bytearray("OPENBOOK", encoding='ascii')
        title = bytearray(self.title, encoding='ascii')
        author = bytearray(self.author, encoding='ascii')
        text = bytearray()
        line_lut.append(LineInfo(0, len(title))) # first line contains title
        line_lut.append(LineInfo(line_lut[-1].offset + line_lut[-1].length, len(author))) # second line contains author
        paragraphs = self.text.split("\n")
        for i in range(0, len(paragraphs)):
            if paragraphs[i] == "":
                paragraphs[i] = "|" # this is just to trick the word wrapper into keeping our blank lines
        for paragraph in paragraphs:
            lines = textwrap.wrap(paragraph, 26)
            for line in lines:
                line_bytes = bytearray(line, encoding='ascii', errors='ignore')
                length = len(line_bytes)
                if line_bytes == bytearray("|", encoding='ascii'):
                    text += bytearray(" ", encoding='ascii') # undo our trick from earlier
                else:
                    text += line_bytes
                line_lut.append(LineInfo(line_lut[-1].offset + line_lut[-1].length, length))
        line_lut[-1].length |= 0x8000 # end of text marker
        start_of_data = len(magic) + len(line_lut) * 12
        data += magic
        for entry in line_lut:
            data += struct.pack('<Q', entry.offset + start_of_data)
            data += struct.pack('<L', entry.length)
        data += title
        data += author
        data += text
        return data
        
if __name__=="__main__":
    print("Minimum Viable Book Binder\n▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔")
    title = input("Title for the book: ")
    author = input("Author of the book: ")
    filename = input("Input text file (without extension): ")
    text = None
    with open(filename + ".txt", 'r', encoding='utf-8') as infile:
        text = infile.read()
    book = Book(title, author, text)
    output = book.generate_obk()
    print("Generated Open Book data of length", len(output))
    outfile = open(filename + ".obk", 'wb')
    outfile.write(output)
    outfile.close()
