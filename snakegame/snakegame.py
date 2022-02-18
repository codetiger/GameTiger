import time, sys
from random import randrange

class SnakeGame():
    snakeDirection = 3
    snakeBlocks = [[15, 12], [14, 12], [13, 12]]
    gameState = 0
    gameSpeed = 1
    foodPos = [0, 0]
    highScore = 0

    BOARD_LIMITS = [1, 3, 30, 23]
    DIR_INFO = [[0, -1, "UP"], [0, 1, "Down"], [-1, 0, "Left"], [1, 0, "Right"]]

    def __init__(self):
        self.highScore = 0
        self.createNewFood()
        self.lastUpdate = self.getCurrentTime()
        self.gameSpeed = int(len(self.snakeBlocks) / 10) + 1
        try:
            with open('hs.dat', 'rb') as file:
                self.highScore = int.from_bytes(file.read(), 'big')
        except Exception as e:
            self.highScore = 0

    def updateSnakePosition(self):
        newPos = [self.snakeBlocks[0][0] + self.DIR_INFO[self.snakeDirection][0], self.snakeBlocks[0][1] + self.DIR_INFO[self.snakeDirection][1]]
        if newPos[0] < self.BOARD_LIMITS[0] or newPos[0] > self.BOARD_LIMITS[2] or newPos[1] < self.BOARD_LIMITS[1] or newPos[1] >= self.BOARD_LIMITS[3] or newPos in self.snakeBlocks:
            self.gameState = 2
            return
        lastPos = self.snakeBlocks[len(self.snakeBlocks)-1]
        for i in range(len(self.snakeBlocks)-1, 0, -1):
            self.snakeBlocks[i] = self.snakeBlocks[i-1].copy()
        self.snakeBlocks[0] = newPos
        if self.checkFoodCaptured():
            self.snakeBlocks.append(lastPos)
            self.createNewFood()
            self.gameSpeed = int(len(self.snakeBlocks) / 10) + 1
            print(self.gameSpeed)
            if self.highScore < len(self.snakeBlocks)-3:
                self.highScore = len(self.snakeBlocks)-3
                with open('hs.dat', 'wb') as file:
                    file.write((self.highScore).to_bytes(4, 'big'))

    def checkFoodCaptured(self):
        for i in range(len(self.snakeBlocks)):
            if self.snakeBlocks[i][0] == self.foodPos[0] and self.snakeBlocks[i][1] == self.foodPos[1]:
                return True
        return False

    def createNewFood(self):
        self.foodPos[0] = self.BOARD_LIMITS[0] + randrange(self.BOARD_LIMITS[2]-self.BOARD_LIMITS[0])
        self.foodPos[1] = self.BOARD_LIMITS[1] + randrange(self.BOARD_LIMITS[3]-self.BOARD_LIMITS[1])

    def drawBlock(self, display, x, y):
        display.rect(x, y, 10, 10, display.DARK_GREEN)
        display.fill_rect(x+2, y+2, 6, 6, display.DARK_GREEN)

    def updateFramebuffer(self, display):
        display.fill(display.GREEN)
        display.rect(self.BOARD_LIMITS[0]*10, self.BOARD_LIMITS[1]*10, self.BOARD_LIMITS[2]*10, (self.BOARD_LIMITS[3]-self.BOARD_LIMITS[1])*10, display.DARK_GREEN)
        for pos in self.snakeBlocks:
            self.drawBlock(display, pos[0]*10, pos[1]*10)
        self.drawBlock(display, self.foodPos[0]*10, self.foodPos[1]*10)
        self.drawBlock(display, 200, 10)
        display.text(str(len(self.snakeBlocks)-3), 222, 12, display.DARK_GREEN)
        display.text("H: " + str(self.highScore), 12, 12, display.DARK_GREEN)
        if self.gameState == 2:
            display.text("Game Over", 120, 120, display.DARK_GREEN)

    def gameLoopUpdate(self, display):
        timeDiff = self.getTimeDiff(self.lastUpdate, self.getCurrentTime())
        if self.gameState == 1 and timeDiff > (350 - self.gameSpeed*25):
            self.lastUpdate = self.getCurrentTime()
            self.updateSnakePosition()
        self.updateFramebuffer(display)

    def keyPressed(self, keyIndex):
        if keyIndex >= len(self.DIR_INFO):
            return
        self.gameState = 1 if self.gameState == 0 else self.gameState
        self.snakeDirection = keyIndex

    def getCurrentTime(self):
        if sys.implementation.name == "micropython":
            return time.ticks_us()
        else:
            return time.time()

    def getTimeDiff(self, start, end):
        if sys.implementation.name == "micropython":
            return int(time.ticks_diff(end, start) / 1000)
        else:
            return int((end-start)*1000.0)
  