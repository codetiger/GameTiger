#include "pico/stdlib.h"
#include "core/screen.h"
#include "core/image.h"
#include "core/keyboard.h"

#define BOARDSIZE 4

class G2048Screen : public Screen
{
private:
    uint8_t board[BOARDSIZE*BOARDSIZE];
    uint8_t highestValue = 2;
    uint32_t score;
    int8_t direction;
    Image *font;

    void addRandomBlock();
    void updateBoard();
    void move(bool& valid_step);
    void combine(bool& valid_step);
    void printBoard();
public:
    G2048Screen(void (*returnCallBack)(int8_t menu), void (*highScoreCallBack)(uint32_t highscore), uint32_t highscore);
    ~G2048Screen();

    void update();
    void draw(Display *display);
    void keyPressed(uint8_t key);
    void keyReleased(uint8_t key);
    void keyDown(uint8_t key);
};
