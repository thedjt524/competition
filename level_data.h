#ifndef LEVEL_DATA_H
#define LEVEL_DATA_H

#include <string>
#include "enemy_ai.h" // 为了使用 AIMode

// 描述单个敌人的生成参数
struct EnemySpawnData {
    int id;                     
    int x;                      
    int y;                      
    char symbol;                
    std::string name;           
    AIMode aiMode;              
    int life;                   
    int damage;                 
    int attackRange; 
    int moverange;
};

// 描述单个关卡的数据
struct LevelData {
    int levelNumber;                        // 关卡编号
    std::string levelName;                 // 关卡名称
    std::vector<EnemySpawnData> enemies;   // 本关卡的敌人列表
    int playerstartx;                      // 玩家初始位置x坐标
    int playerstarty;                      // 玩家初始位置y坐标
};

#endif