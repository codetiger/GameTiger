import time, os, sys
import pygame

class LCD_display():
    screen = None
    GREEN = (30, 255, 60)
    DARK_GREEN = (30, 120, 60)
    BLACK = (0, 0, 0)
    WHITE = (255, 255, 255)

    def __init__(self):
        self.screen = pygame.display.set_mode([320, 240])
        self.myfont = pygame.font.SysFont('Arial', 8)
        self.width = 320
        self.height = 240

    def show(self):
        pygame.display.flip()

    def fill(self, color):
        self.screen.fill(color)

    def rect(self, x, y, width, height, color):
        pygame.draw.rect(self.screen, color, (x, y, width, height), 1)

    def fill_rect(self, x, y, width, height, color):
        pygame.draw.rect(self.screen, color, (x, y, width, height), 0)

    def text(self, str, x, y, color):
        textsurface = self.myfont.render(str, False, color)
        self.screen.blit(textsurface, (x,y))

    def pixel(self, x, y, color):
        self.screen.set_at([x, y], color)

    def scroll(self, xinc, yinc):
        self.screen.scroll(xinc, yinc)

    def setBrightness(self, brightness):
        pass

    def drawImage(self, filename, x, y, width, height):
        with open(filename, "rb") as f:
            for i in range(height):
                for j in range(width):
                    pixelData = int.from_bytes(f.read(2), 'big') & 0xFFFF
                    pixelData = ((pixelData & 0xFF) << 8) | ((pixelData >> 8) & 0xFF)
                    red = (pixelData & 0xF800) >> 11
                    red = (red << 3) | (red >> 2)
                    green = (pixelData & 0x07e0) >> 5
                    green = (green << 2) | (green >> 1)
                    blue = pixelData & 0x001f
                    blue = (blue << 3) | (blue >> 2)
                    self.pixel(x + j, y + i, (red, green, blue))
