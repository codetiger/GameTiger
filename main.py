import sys, time
from snakegame.snakegame import SnakeGame
from components.battery import Battery
from components.keyboard import Keyboard

if sys.implementation.name == "micropython":
    from components.wsdisplay import LCD_display
else:
    from components.pydisplay import LCD_display

def drawBatteryStatus(display, battery):
    display.rect(275, 8, 30, 14, display.BLACK)
    display.fill_rect(277, 10, int(26 * battery.level() / 100), 10, display.BLACK)
    display.fill_rect(305, 12, 3, 6, display.BLACK)
    if battery.charging():
        sig = ">"
        if time.time() % 3 == 1:
            sig = ">>"
        elif time.time() % 3 == 2:
            sig = ">>>"
        display.text(sig, 278, 11, display.GREEN)

if __name__=='__main__':
    battery = Battery()
    print("Battery status: %d %d" % (battery.charging(), battery.level()))

    keyboard = Keyboard()

    display = LCD_display()
    display.fill(0x0)
    display.setBrightness(0)
    display.show()
    display.fill(0xffffff)
    display.drawImage("logo.565", 92, 56, 128, 128)
    display.setBrightness(50)
    display.show()

    while(keyboard.getKeyPressed() == None):
        pass

    snakeGame = SnakeGame()

    while(1):
        key = keyboard.getKeyPressed()
        if key != None:
            print("Key Pressed: %d" %(key))
            snakeGame.keyPressed(key)
        
        snakeGame.gameLoopUpdate(display)
        drawBatteryStatus(display, battery)
        display.show()
