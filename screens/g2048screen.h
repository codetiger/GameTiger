#include "../core/common.h"
#include "../core/screen.h"
#include "../core/image.h"
#include "../core/keyboard.h"

#define BOARDSIZE 4

class G2048Screen : public Screen
{
private:
    uint8_t board[BOARDSIZE*BOARDSIZE];
    uint8_t highestValue = 2;
    uint32_t score;
    int8_t direction;
    Image *font;

    Color colors[12] = {
        Color(180, 165, 147),
        Color(233, 221, 209),
        Color(233, 217, 187),
        Color(237, 162, 102),

        Color(241, 129, 80),
        Color(241, 101, 78),
        Color(240, 70, 45),
        Color(232, 198, 96),

        Color(231, 195, 80),
        Color(232, 190, 64),
        Color(231, 186, 49),
        Color(231, 182, 36),
    };

    void addRandomBlock();
    void updateBoard();
    void move(bool& valid_step);
    void combine(bool& valid_step);
    void printBoard();
public:
    G2048Screen(void (*returnCallBack)(int8_t menu), void (*highScoreCallBack)(uint32_t highscore), uint32_t highscore);
    ~G2048Screen();

    void update(uint16_t deltaTimeMS);
    void draw(Display *display);
    void keyPressed(uint8_t key);
    void keyReleased(uint8_t key);
    void keyDown(uint8_t key);
};
