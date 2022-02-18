import sys

if sys.implementation.name == "micropython":
    from machine import Pin
else:
    import pygame
    from pygame.locals import (K_UP, K_DOWN, K_LEFT, K_RIGHT, K_a, K_b, KEYDOWN)
    pygame.init()

class Keyboard():
    KEY_UP = 0
    KEY_DOWN = 1
    KEY_LEFT = 2
    KEY_RIGHT = 3
    KEY_A = 4
    KEY_B = 5

    keys = {}

    def __init__(self):
        if sys.implementation.name == "micropython":
            gpioPins = [2, 0, 1, 3, 4, 5]
            for i in range(len(gpioPins)):
                self.keys[i] = {"object": Pin(gpioPins[i], Pin.IN, Pin.PULL_DOWN)}
        else:
            keyCodes = [K_UP, K_DOWN, K_LEFT, K_RIGHT, K_a, K_b]
            for i in range(len(keyCodes)):
                self.keys[i] = {"object": keyCodes[i]}

    def getKeyPressed(self):
        if sys.implementation.name == "micropython":
            for i in range(len(self.keys)):
                if self.keys[i]["object"].value() == 1:
                    return i
        else:
            for event in pygame.event.get():
                if event.type == KEYDOWN:
                    for i in range(len(self.keys)):
                        if event.key == self.keys[i]["object"]:
                            return i
        return None