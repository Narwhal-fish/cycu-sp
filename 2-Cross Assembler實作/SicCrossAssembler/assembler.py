# -*- coding: utf-8 -*-

from error import LexicalStageError, SyntaxStageError

class Assembler:
    def __init__(self):
        self.__work_list = []
    
    def assemble(self, source):
        for src_line in source.split('\n'):
            self.__work_list.append({
                'source_line': src_line,
                'location': None,
                'object_code': None,
            })
            
            
            
        src_lines = source.split('\n')
        # pass 1
        for src_line in src_lines:
            try:
                
            pass
        
        # pass 2
        for src_line in src_lines:
            try:
                token_list = self.__lexical_analysis(src_line)
                order_list = self.__syntax_analysis(token_list)
            except LexicalStageError:
                pass
            except SyntaxStageError:
                pass
    
    def get_assembly_list(self):
        return self.__assembly_list
    
    def __lexical_analysis(self, source_line):
        pass
    
    def __syntax_analysis(self, token_list):
        pass
    
    def __execute(self, order_list):
        pass

if __name__ == '__main__':
    source = '\n\
COPY	START	1000\n\
FIRST	STL	RETADR\n\
CLOOP	JSUB	RDREC\n\
	LDA	LENGTH\n\
	COMP	ZERO\n\
	JEQ	ENDFIL\n\
	JSUB	WRREC\n\
	J	CLOOP\n\
ENDFIL	LDA	EOF\n\
	STA	BUFFER\n\
	LDA	THREE\n\
	STA	LENGTH\n\
	JSUB	WRREC\n\
	LDL	RETADR\n\
	RSUB\n\
EOF	BYTE	C\'EOF\'\n\
THREE	WORD	3\n\
ZERO	WORD	0\n\
RETADR	RESW	1\n\
LENGTH	RESW	1\n\
BUFFER	RESB	4096\n\
.\n\
.	SUBROUTINE TO READ RECORD INTO BUFFER\n\
.\n\
RDREC	LDX	ZERO\n\
	LDA	ZERO\n\
RLOOP	TD	INPUT\n\
	JEQ	RLOOP\n\
	RD	INPUT\n\
	COMP	ZERO\n\
	JEQ	EXIT\n\
	STCH	BUFFER,X\n\
	TIX	MAXLEN\n\
	JLT	RLOOP\n\
EXIT	STX	LENGTH\n\
	RSUB\n\
INPUT	BYTE	X\'F1\'\n\
MAXLEN	WORD	4096\n\
.\n\
.	SUBROUTINE TO WRITE RECORD FROM BUFFER\n\
.\n\
WRREC	LDX	ZERO\n\
WLOOP	TD	OUTPUT\n\
	JEQ	WLOOP\n\
	LDCH	BUFFER,X\n\
	WD	OUTPUT\n\
	TIX	LENGTH\n\
	JLT	WLOOP\n\
	RSUB\n\
OUTPUT	BYTE	X\'05\'\n\
	END	FIRST'
