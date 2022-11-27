# -*- coding: utf-8 -*-

class LexicalStageError(BaseException):
    pass

class SyntaxStageError(BaseException):
    pass

class DuplicateSymbolError(BaseException):
    pass

class UndefinedSymbolError(BaseException):
    pass
