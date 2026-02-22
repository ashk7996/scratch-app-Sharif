#include "spiritFunctions.h"
#include "../Common/common.h"
#include <cmath>

void execute(BlockType type) {
    // if (type != START && executeIndex == 0) {
    //     scratchApp.isExecuting = false;
    //     scratchApp.executeIndex = 0;
    //     return;
    // }

    // cout << executeIndex << endl;

    switch (type) {
        default: {
            break;
        }

        case MOTION_SETX: {
            setSpiritX(150);
            break;
        }

        case MOTION_SETY: {
            setSpiritY(150);
            break;
        }
    }
}

// <<--------------------------------------------- Motion Commands --------------------------------------------->>
void setSpiritX(int x) {
    scratchApp.spirit.spiritX = x;
}

void setSpiritY(int y) {
    scratchApp.spirit.spiritY = y;
}

void ValidatePosition() {
    if (scratchApp.spirit.spiritX >= scratchApp.DW)
        scratchApp.spirit.spiritX = scratchApp.DW - 1;
    else if (scratchApp.spirit.spiritX <= 0)
        scratchApp.spirit.spiritX = 0;
    if (scratchApp.spirit.spiritY >= scratchApp.DH)
        scratchApp.spirit.spiritY = scratchApp.DH - 1;
    else if (scratchApp.spirit.spiritY <= 0)
        scratchApp.spirit.spiritY = 0;
}

void ValidateDirection() {
    if (scratchApp.spirit.direction >= 360) {
        int q = scratchApp.spirit.direction % 360;
        scratchApp.spirit.direction -= q * 360;
    } else if (scratchApp.spirit.direction <= -360) {
        int q = scratchApp.spirit.direction % 360;
        scratchApp.spirit.direction -= q * 360;
    }
}

void MoveNSteps(int n) {
    double degree = scratchApp.spirit.direction * M_PI / 180;
    double dx = n * cos(degree);
    double dy = -n * sin(degree);
    scratchApp.spirit.spiritX += dx;
    scratchApp.spirit.spiritY += dy;
    ValidatePosition();
}

void TurnNDegrees(int n) {
    scratchApp.spirit.direction += n;
    ValidateDirection();
}

void TurnRight() {
    scratchApp.spirit.direction = 90;
}

void TurnLeft() {
    scratchApp.spirit.direction = -90;
}

void GoToXY(int x, int y) {
    scratchApp.spirit.spiritX = x;
    scratchApp.spirit.spiritY = y;
    ValidatePosition();
}

void PointInDirection(int direction) {
    scratchApp.spirit.direction = direction;
    ValidateDirection();
}

void ChangeXBy(int x) {
    scratchApp.spirit.spiritX += x;
    ValidatePosition();
}

void ChangeYBy(int y) {
    scratchApp.spirit.spiritY += y;
    ValidatePosition();
}

void IfOnEdge() {
    if (scratchApp.spirit.spiritX >= scratchApp.DW || scratchApp.spirit.spiritX <= 0 || scratchApp.spirit.spiritY >=
        scratchApp.DH || scratchApp.spirit.spiritY <= 0)
        scratchApp.spirit.direction += 180;
    ValidateDirection();
}

void goToRandomPosition() {
    int x = rand() % (scratchApp.DW + 1);
    int y = rand() % (scratchApp.DH + 1);
    setSpiritX(x);
    setSpiritY(y);
}

// <<--------------------------------------------- Looks Commands --------------------------------------------->>

// void nextCostume(){
//
//     if(scratchApp.spirit.currentCostume == scratchApp.spirit.costumeCount - 1)
//         scratchApp.spirit.currentCostume = 0;
//     else
//         scratchApp.spirit.currentCostume++;
// }
//
// void changeCostume(int n){
//
//     if(n<=0 || n > scratchApp.spirit.costumeCount )
//         return;
//     else
//         scratchApp.spirit.currentCostume = n - 1;
// }
//
// void ValidateSize(){
//
//     int minSize = 5;
//     int maxSize = 500;
//     if(scratchApp.spirit.size <= minSize)
//         scratchApp.spirit.size = minSize;
//     else if(scratchApp.spirit.size >= maxSize)
//         scratchApp.spirit.size = maxSize;
// }
//
// void changeSizeBy(int n){
//
//     scratchApp.spirit.size += n;
//     ValidateSize();
// }
//
// void setSize(int n){
//
//     scratchApp.spirit.size = n;
//     ValidateSize();
// }
//
// void show(){
//
//     scratchApp.spirit.isShow = true;
// }
// void hide(){
//
//     scratchApp.spirit.isShow = false;
// }
//
// void spiritCostume(){
//
//     std::cout << scratchApp.spirit.currentCostume + 1;
// }
//
// void spiritSize(){
//
//     std::cout << scratchApp.spirit.size;
// }
