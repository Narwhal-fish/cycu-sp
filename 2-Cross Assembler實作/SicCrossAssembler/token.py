# -*- coding: utf-8 -*-

from enum import Enum
from dataclasses import dataclass

class TokenType(Enum):
    INSTRUCTION = "instruction"
    PSEUDO = 'pseudo'
    REGISTER = 'register'
    DELIMITER = 'delimiter'
    DECIMAL = 'decimal'
    HEXADECIMAL = 'hexadecimal'
    STRING = "string"
    LITERAL = 'literal'

@dataclass
class Token:
    type: TokenType
    content: str
