#include "game.h"
#include "enemy_ai.h"
#include<Windows.h>
#include <iostream>
#include<ctime>
#include<cstdlib>
#include<cmath>
#include "linear_attack.h"
#include "level_manager.h" 
#include "rogue_system.h"
#pragma comment(lib, "winmm.lib")



int main(){
    SetConsoleOutputCP(CP_UTF8);
    srand(time(nullptr));
    gamemap game_map;
    EnemyAIController aicontroller;
    LevelManager levelManager;
    RogueSystem rogueSystem;

    int currentLevel = 1;

    player*player_ptr=game_map.createplayer(1, 3, 0, 'P', "玩家");

    
    while (true) {
        // 加载当前关卡
        if (!levelManager.loadLevel(currentLevel, game_map, aicontroller)) {
            std::cout << "无法加载关卡 " << currentLevel << std::endl;
            break;
        }

        player_ptr = game_map.getplayer();
        if (!player_ptr) {
            std::cout << "玩家创建失败！\n";
            break;
        }

        player_ptr->resetacted();

        bool levelRunning = true;

       
        while (levelRunning) {
            system("cls");

            std::cout << "===== 第 " << currentLevel << " 关 =====\n";
            game_map.printWithPlayerPosition();
            game_map.printEntitiesInfo();
            game_map.printEnemiesPositions();

            std::cout << "玩家生命：" << player_ptr->getlife() << std::endl;

            
            player_ptr->resetacted();

            std::cout << "玩家回合！\n";
            std::cout << "请选择操作：\n";
            std::cout << "1 - 移动\n";
            std::cout << "2 - 直线攻击\n";

            bool actionCompleted = false;

            while (!actionCompleted) {
                int choice = 0;
                std::cin >> choice;
                std::cin.ignore(10000, '\n');

                if (choice == 1) {
                    //  先显示移动范围
                    game_map.calculateMoveRangeHighlight(player_ptr);

                    //  清屏并显示带高亮的地图
                    system("cls");
                    std::cout << "===== 第 " << currentLevel << " 关 =====\n";
                    game_map.printWithPlayerPosition();
                    game_map.printEntitiesInfo();
                    game_map.printEnemiesPositions();

                    std::cout << "玩家生命：" << player_ptr->getlife() << std::endl;

                    //  再让玩家输入坐标
                    int tx, ty;
                    std::cout << "输入目标坐标(x y)：";
                    std::cin >> tx >> ty;
                    std::cin.ignore(10000, '\n');

                    //  执行移动
                    if (game_map.moveplayerto(tx, ty)) {
                        player_ptr->markacted();
                        actionCompleted = true;
                    } else {
                        std::cout << "移动失败，请重新选择操作。\n";
                    }

                    game_map.clearHighlight();
                }

                else if (choice == 2) {
                    game_map.calculateAttackRangeHighlight(player_ptr);
                    system("cls");
                    game_map.printWithPlayerPosition();
                    game_map.printEntitiesInfo();
                    game_map.printEnemiesPositions();

                    std::cout << "玩家生命：" << player_ptr->getlife() << std::endl;
                    std::cout << "红色 + 为可攻击区域\n";

                    int tx, ty;
                    std::cout << "输入攻击目标坐标(x y)：";
                    std::cin >> tx >> ty;
                    std::cin.ignore(10000, '\n');

                    if (game_map.playerLinearAttackAt(tx, ty)) {
                        actionCompleted = true;
                    } else {
                        std::cout << "攻击失败，请重新选择操作。\n";
                    }

                    game_map.clearHighlight();
                }

                // 清除高亮
                game_map.clearHighlight();
            }

            // 玩家死亡
            if (player_ptr->getlife() <= 0) {
                std::cout << "玩家死亡！游戏结束！\n";
                return 0;
            }

            
            std::cout << "敌方回合！\n";
            aicontroller.update(game_map, player_ptr);
            game_map.updateAllEntities();

            
            auto enemies = game_map.getenemies();
            if (enemies.empty()) {
                std::cout << "第 " << currentLevel << " 关通关！\n";

                if (levelManager.isGameComplete()) {
                    std::cout << "🎉 恭喜你，通关了整个游戏！\n";
                    return 0;
                }

                // 玩家选择天赋
                rogueSystem.chooseTalent(player_ptr);

                currentLevel++;
                std::cout << "按回车进入下一关...\n";
                std::cin.get();
                break; // 跳出当前关卡循环，进入下一关
            }

            std::cout << "\n按回车继续...\n";
            std::cin.get();
        }
    }

    return 0;
}