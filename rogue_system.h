#ifndef ROGUE_SYSTEM_H
#define ROGUE_SYSTEM_H

#include <vector>
#include <string>
#include <iostream>
#include <unordered_map>
#include <stdexcept>
#include <algorithm>
#include "rogue_talent.h"
#include "game.h"

class RogueSystem {
public:
    RogueSystem() {
        initTalents();
    }

    /**
     * 在关卡通关后调用
     * 从当前可解锁的天赋中选择一个
     */
    const Talent& chooseTalent(player* playerPtr) {
        auto available = getAvailableTalents();

        if (available.empty()) {
            throw std::runtime_error("没有可选择的天赋");
        }

        std::cout << "\n🎯 选择天赋：\n";
        for (size_t i = 0; i < available.size(); ++i) {
            std::cout << i + 1 << ". " << available[i]->name
                      << " - " << available[i]->description << "\n";
        }

        int choice = 0;
        while (true) {
            std::cin >> choice;
            if (choice >= 1 && choice <= static_cast<int>(available.size())) {
                break;
            }
            std::cout << "无效选择，请重新输入：";
        }

        Talent& selected = *const_cast<Talent*>(available[choice - 1]);

        // 应用天赋加成
        playerPtr->setlife(playerPtr->getlife() + selected.lifeBonus);
        playerPtr->setAttackPower(playerPtr->getAttackPower() + selected.attackBonus);
        playerPtr->setAttackRange(playerPtr->getAttackRange() + selected.attackRangeBonus);
        playerPtr->setmoverange(playerPtr->getmoverange() + selected.moveRangeBonus);

        // 标记为已解锁
        selected.isUnlocked = true;

        std::cout << "✅ 已获得天赋：【" << selected.name << "】\n";
        return selected;
    }

private:
    std::unordered_map<std::string, Talent> talents;

   
    void initTalents() {
        talents["strong_body"] = {
            "strong_body",
            "强壮体魄",
            "生命值+1",
            1, 0, 0, 0,
            {},
            UnlockRule::All,
            false
        };

        talents["shoter_instinct"] = {
            "shoter_instinct",
            "射手直觉",
            "射程+1",
            0, 0, 1, 0,
            {},
            UnlockRule::All,
            false
        };

        talents["sharp_blade"] = {
            "sharp_blade",
            "锋利之刃",
            "攻击力+1",
            0, 1, 0, 0,
            {},
            UnlockRule::All,
            false
        };

        talents["giant_power"] = {
            "giant_power",
            "巨人之力",
            "生命值+2",
            2, 0, 0, 0,
            {"strong_body"},
            UnlockRule::All,
            false
        };

        

        talents["critical_hit"] = {
            "critical_hit",
            "致命一击",
            "攻击力+2",
            0, 2, 0, 0,
            {"sharp_blade", "shoter_instinct"},
            UnlockRule::All,
            false
        };

        talents["berserker_rage"] = {
            "berserker_rage",
            "狂暴怒吼",
            "攻击力+3",
            0, 3, 0, 0,
            {"giant_power", "critical_hit"},
            UnlockRule::All, // ✅ OR 规则
            false
        };
    }

    /**
     * 获取当前可解锁的天赋
     * 根据每个天赋配置的 UnlockRule 判断
     */
    std::vector<const Talent*> getAvailableTalents() const {
        std::vector<const Talent*> result;

        for (const auto& pair : talents) {
            const Talent& t = pair.second;

            if (t.isUnlocked) {
                continue;
            }

            bool canUnlock = false;

            if (t.prerequisites.empty()) {
                canUnlock = true;
            }
            else if (t.rule == UnlockRule::All) {
                // AND 逻辑：所有前置都必须解锁
                canUnlock = true;
                for (const auto& preId : t.prerequisites) {
                    auto it = talents.find(preId);
                    if (it == talents.end() || !it->second.isUnlocked) {
                        canUnlock = false;
                        break;
                    }
                }
            }
            else if (t.rule == UnlockRule::Any) {
                // OR 逻辑：任意一个前置解锁即可
                for (const auto& preId : t.prerequisites) {
                    auto it = talents.find(preId);
                    if (it != talents.end() && it->second.isUnlocked) {
                        canUnlock = true;
                        break;
                    }
                }
            }

            if (canUnlock) {
                result.push_back(&t);
            }
        }

        return result;
    }
};

#endif