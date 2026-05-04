#include "game.h"
#include<Windows.h>
#include <iostream>


int main(){
     SetConsoleOutputCP(CP_UTF8);
     gamemap gamemap;

     player*player=gamemap.createplayer(1,3,4,'@',"Player1");
     gamemap.createenemy(2,2,2,'E',"Enemy1");
     gamemap.createenemy(3,3,3,'E',"Enemy2");

     std::cout<<"初始状态："<<std::endl;
     gamemap.printWithPlayerPosition();
     gamemap.printEntitiesInfo();

     std::cout<<"输入坐标移动玩家："<<std::endl;
     gamemap.moveplayerbyinput();
     gamemap.printWithPlayerPosition();
     gamemap.printEntitiesInfo();

     gamemap.printEnemiesPositions();



     

    return 0;
}