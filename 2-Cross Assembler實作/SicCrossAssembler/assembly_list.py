# -*- coding: utf-8 -*-

class AssemblyList:
    def __init__(self):
        self.__line_title_str = 'Line'
        self.__location_title_str = 'Location'
        self.__object_code_title_str = 'Object code'
        self.__source_code_title_str = 'Source statement'
        
        self.__line_width = len(self.__line_title_str)
        self.__location_width = len(self.__location_title_str)
        self.__object_code_width = len(self.__object_code_title_str)
        self.__source_code_width = len(self.__source_code_title_str)
        
        self.__location = []
        self.__object_code = []
        self.__source_code = []
    
    def add(self, location, object_code, source_code):
        self.__location.append(location)
        self.__object_code.append(object_code)
        self.__source_code.append(source_code)
        
        self.__line_width = max(self.__line_width, len(str(len(self.__location))))
        self.__location_width = max(self.__location_width, len(location))
        self.__object_code_width = max(self.__object_code_width, len(object_code))
        self.__source_code_width = max(self.__source_code_width, len(source_code))
    
    def __get_title_str(self):
        line_str = 'Line'
        location_str = 'Location'
        object_code_str = 'Object code'
        source_code_str = 'Source statement'
        return f'{self.__line_title_str:<{self.__line_width}}    ' +\
               f'{self.__location_title_str:<{self.__location_width}}    ' +\
               f'{self.__object_code_title_str:<{self.__object_code_width}}    ' +\
               f'{self.__source_code_title_str:<{self.__source_code_width}}' + '\n'
    
    def __get_content_str(self):
        content_str = ''
        line_number = 1
        for loc, obj, src in zip(self.__location, self.__object_code, self.__source_code):
            content_str += f'{line_number:<{self.__line_width}}    ' +\
                           f'{loc:<{self.__location_width}}    ' +\
                           f'{obj:<{self.__object_code_width}}    ' +\
                           f'{src:<{self.__source_code_width}}' + '\n'
            line_number += 1
        return content_str
    
    def print_list(self):
        print(self.__get_title_str(), end='')
        print()
        print(self.__get_content_str(), end='')
    
    def save_list(self, save_path):
        fp = open(save_path, 'w')
        fp.write(self.__get_title_str())
        fp.write('\n')
        fp.write(self.__get_content_str())
        fp.close()

if __name__ == '__main__':
    assembly_list = AssemblyList()
    assembly_list.add('1000', '', 'COPY    START   1000')
    assembly_list.add('1000', '141033', 'FIRST   STL     RETADR    .this is comment 1')
    assembly_list.add('1003', '482039', 'CLOOP   JSUB    RDREC     .this is comment 2')
    assembly_list.add('1006', '001036', '        LDA     LENGTH    .this is comment 3')
    assembly_list.add('1009', '281030', '        COMP    ZERO      .a == zero')
    assembly_list.add('100C', '301015', "        JEQ     ENDFIL    .this is comment 4")
    assembly_list.add('1008', '454F46454F46454F46', "EOF     BYTE    C'EOFEOFEOF'")
    assembly_list.print_list()
    assembly_list.save_list('example.txt')
