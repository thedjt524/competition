#ifndef LEVEL_MANAGER_H
#define LEVEL_MANAGER_H

#include <vector>
#include <string>
#include "game.h"
#include "enemy_ai.h"
#include "level_data.h"

class LevelManager {
private:
    std::vector<LevelData> levels; // 存储所有关卡数据
    int currentLevelIndex = -1;

public:
    LevelManager() {
        initializeLevels(); // 在构造函数中定义所有关卡
    }

    // 获取当前关卡数据
    const LevelData& getCurrentLevel() const {
        return levels[currentLevelIndex];
    }

    // 加载指定关卡
    bool loadLevel(int levelNumber, gamemap& gameMap, EnemyAIController& aiController) {
        // 清除旧关卡（非常重要！）
        clearMapForNewLevel(gameMap, aiController);

        // 查找关卡
        for (size_t i = 0; i < levels.size(); ++i) {
            if (levels[i].levelNumber == levelNumber) {
                currentLevelIndex = i;
                const LevelData& level = levels[i];

                std::cout << "正在加载关卡: " << level.levelName << std::endl;

                if (gameMap.getplayer()) {
                gameMap.getplayer()->setPosition(level.playerstartx, level.playerstarty);
                }

                // 根据关卡数据生成敌人
                for (const auto& spawnData : level.enemies) {
                    enemy* newEnemy = gameMap.createenemy(
                        spawnData.id,
                        spawnData.x,
                        spawnData.y,
                        spawnData.symbol,
                        spawnData.name,
                        spawnData.life,
                        spawnData.damage,
                        spawnData.attackRange,
                        spawnData.moverange
                    );
                    
                   
                    // 注册到AI控制器
                    aiController.registerEnemy(newEnemy, spawnData.aiMode);
                }
                return true;
            }
        }
        return false; // 未找到关卡
    }

    // 加载下一关
    bool loadNextLevel(gamemap& gameMap, EnemyAIController& aiController) {
        if (currentLevelIndex + 1 < levels.size()) {
            return loadLevel(levels[currentLevelIndex + 1].levelNumber, gameMap, aiController);
        }
        return false; // 已经是最后一关
    }

    bool isGameComplete() const {
        return currentLevelIndex + 1 >= levels.size();
    }

private:
    // 定义所有关卡的内容
    void initializeLevels() {
        // --- 关卡 1 ---
        LevelData level1;
        level1.levelNumber = 1;
        level1.playerstartx = 3;
        level1.playerstarty = 0;
        level1.levelName = "新手试炼";
        level1.enemies = {
            {2, 1, 7, 'E', "近战兵", AIMode::Chase, 1, 1, 1,1},//数字分别代表敌人ID、X坐标、Y坐标、符号、名称、生命、攻击力、攻击范围,移动范围
            {3, 6, 7, 'E', "守卫者", AIMode::Chase, 2, 1, 1,1}
        };
        levels.push_back(level1);

        // --- 关卡 2 ---
        LevelData level2;
        level2.levelNumber = 2;
        level2.playerstartx = 3;
        level2.playerstarty = 6;
        level2.levelName = "深入敌后";
        level2.enemies = {
            {2, 1, 1, 'A', "潜伏者", AIMode::Chase, 1, 1, 1,3},
            {3, 7, 2, 'E', "近战兵", AIMode::Chase, 1, 1, 1,1},
            {4, 7, 5, 'E', "近战兵", AIMode::Chase, 1, 1, 1,1}
        };
        levels.push_back(level2);

        // 可以继续添加更多关卡
    }

    // 清理地图，为加载新关卡做准备
    void clearMapForNewLevel(gamemap& gameMap, EnemyAIController& aiController) {
        gameMap.resetWithoutPlayer();
        aiController.clearAllEnemies();
        std::cout<<"地图已清除，准备加载新关卡..."<<std::endl;
    }
};
#endif