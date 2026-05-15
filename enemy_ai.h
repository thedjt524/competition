#ifndef ENEMY_AI_H
#define ENEMY_AI_H

#include "game.h"
#include <vector>
#include <algorithm>
#include <cmath>

enum class AIMode {
    Chase,      // 追逐玩家
    Patrol,     // 巡逻
    Random,     // 随机移动
    Guard       // 守卫
};

class EnemyAIController {
private:
    // 为敌人添加AI模式属性
    struct EnemyAI {
        enemy* enemyPtr;
        AIMode mode;
        int patrolIndex;
        std::vector<std::pair<int, int>> patrolPoints;
    };
    
    std::vector<EnemyAI> enemyAIs;
    
public:
    // 注册敌人到AI系统
    void registerEnemy(enemy* enemyPtr, AIMode mode = AIMode::Chase) {
        if (!enemyPtr) return;

        for (auto& ai : enemyAIs) {
            if (ai.enemyPtr == enemyPtr)
                return; // 已注册
        }

        EnemyAI ai;
        ai.enemyPtr = enemyPtr;
        ai.mode = mode;
        ai.patrolIndex = 0;
        enemyAIs.push_back(ai);
    }
    
    // 移除敌人的AI
    void unregisterEnemy(enemy* enemyPtr) {
        enemyAIs.erase(
            std::remove_if(enemyAIs.begin(), enemyAIs.end(),
                [enemyPtr](const EnemyAI& ai) {
                    return ai.enemyPtr == enemyPtr;
                }),
            enemyAIs.end());
    }
    
    // 执行所有敌人的AI逻辑
    void update(gamemap& gameMap, player* playerPtr) {
        if (!playerPtr) return;
        
        for (auto& ai : enemyAIs) {
            if (!ai.enemyPtr || !ai.enemyPtr->isActive())
                continue;

            if (ai.enemyPtr->getHasActedThisTurn())
                continue;

            // 优先尝试攻击
            if (ai.enemyPtr->attack(playerPtr)) {
                continue;
            }

            int targetX = ai.enemyPtr->getX();
            int targetY = ai.enemyPtr->getY();

            switch (ai.mode) {
                case AIMode::Chase:
                    chasePlayer(ai, playerPtr, targetX, targetY);
                    break;
                case AIMode::Patrol:
                    patrol(ai, targetX, targetY);
                    break;
                case AIMode::Random:
                    randomMove(ai, targetX, targetY);
                    break;
                case AIMode::Guard:
                    // 守卫模式不移动
                    break;
            }

            // 应用移动
            if (isValidMove(ai, gameMap, playerPtr, targetX, targetY)) {
                ai.enemyPtr->setPosition(targetX, targetY);
                ai.enemyPtr->markacted();
            }
        }
    }

    void clearAllEnemies() {
        enemyAIs.clear();
    }
    
private:
    // 追逐玩家逻辑（支持移动范围）
    void chasePlayer(EnemyAI& ai, player* playerPtr, int& targetX, int& targetY) {
        int startX = ai.enemyPtr->getX();
        int startY = ai.enemyPtr->getY();

        int dx = playerPtr->getX() - startX;
        int dy = playerPtr->getY() - startY;

        int stepX = (dx > 0) ? 1 : (dx < 0 ? -1 : 0);
        int stepY = (dy > 0) ? 1 : (dy < 0 ? -1 : 0);

        int moveRange = ai.enemyPtr->getmoverange();

        targetX = startX + stepX * (std::abs(dx) < moveRange ? std::abs(dx) : moveRange);
        targetY = startY + stepY * (std::abs(dy) < moveRange ? std::abs(dy) : moveRange);
    }
    
    // 巡逻逻辑（支持移动范围）
    void patrol(EnemyAI& ai, int& targetX, int& targetY) {
        if (ai.patrolPoints.empty()) {
            int currentX = ai.enemyPtr->getX();
            int currentY = ai.enemyPtr->getY();
            ai.patrolPoints = {
                {currentX + 1, currentY},
                {currentX, currentY + 1},
                {currentX - 1, currentY},
                {currentX, currentY - 1}
            };
        }

        int destX = ai.patrolPoints[ai.patrolIndex].first;
        int destY = ai.patrolPoints[ai.patrolIndex].second;

        int moveRange = ai.enemyPtr->getmoverange();

        targetX = ai.enemyPtr->getX();
        targetY = ai.enemyPtr->getY();

        int dx = destX - targetX;
        int dy = destY - targetY;

        int stepX = (dx > 0) ? 1 : (dx < 0 ? -1 : 0);
        int stepY = (dy > 0) ? 1 : (dy < 0 ? -1 : 0);

        targetX += stepX * (std::abs(dx) < moveRange ? std::abs(dx) : moveRange);
        targetY += stepY * (std::abs(dy) < moveRange ? std::abs(dy) : moveRange);

        ai.patrolIndex = (ai.patrolIndex + 1) % ai.patrolPoints.size();
    }
    
    // 随机移动逻辑（支持移动范围）
    void randomMove(EnemyAI& ai, int& targetX, int& targetY) {
        int dir = rand() % 4;
        int dx[] = {1, -1, 0, 0};
        int dy[] = {0, 0, 1, -1};

        int moveRange = ai.enemyPtr->getmoverange();
        int steps = 1 + rand() % moveRange;

        targetX = ai.enemyPtr->getX() + dx[dir] * steps;
        targetY = ai.enemyPtr->getY() + dy[dir] * steps;
    }
    
    // 检查移动是否有效
    bool isValidMove(EnemyAI& ai, gamemap& gameMap, player* playerPtr, int x, int y) {
        // 边界检查
        if (x < 0 || x >= gameMap.getMapWidth() ||
            y < 0 || y >= gameMap.getMapLength()) {
            return false;
        }

        // 不能移动到玩家位置
        if (playerPtr &&
            x == playerPtr->getX() &&
            y == playerPtr->getY()) {
            return false;
        }

        // 不能与其他敌人重叠
        auto enemies = gameMap.getenemies();
        for (auto e : enemies) {
            if (e && e != ai.enemyPtr &&
                e->isActive() &&
                e->getX() == x &&
                e->getY() == y) {
                return false;
            }
        }

        return true;
    }
};

#endif