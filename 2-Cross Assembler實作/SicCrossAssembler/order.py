# -*- coding: utf-8 -*-

from enum import Enum
from dataclasses import dataclass

class OrderType(Enum):
    DEFINE = 'define'  # define variable
    INSTRUCTION = 'instruction'
    RESERVE = 'reserve'
    CONSTANT = 'constant'
    START = 'start'
    END = 'end'

@dataclass
class Order:
    type: OrderType
    operand: str
    index: bool
