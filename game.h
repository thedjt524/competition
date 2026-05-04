#ifndef GAME_H
#define GAME_H

#include <iostream>
#include<vector>
#include<string>
#include<memory>

enum class entitytype{
    player,
    enemy
};

class entity{
    protected:
    int id;
    int x;
    int y;
    char symbol;
    std::string name;
    entitytype type;
    bool active;

    public:
    // 构造函数
    entity(int id, int x, int y, char symbol, const std::string& name, entitytype type)
        : id(id), x(x), y(y), symbol(symbol), name(name), type(type), active(true) {}
    
    // 虚析构函数
    virtual ~entity() = default;
    
    // 获取方法
    int getId() const { return id; }
    int getX() const { return x; }
    int getY() const { return y; }
    char getSymbol() const { return symbol; }
    std::string getName() const { return name; }
    entitytype getType() const { return type; }
    bool isActive() const { return active; }
    
    // 设置方法
    void setPosition(int newX, int newY) { 
        x = newX; 
        y = newY; 
    }
    void setSymbol(char newSymbol){
        symbol=newSymbol;
    }
    void setName(const std::string& newName){
        name=newName;
    }
    void setActive(bool newActive){
        active=newActive;
    }

    virtual void update(){};//每帧更新一次

    virtual bool move(int targetx, int targety) {
        //移动到新位置
       x=targetx;
       y=targety;
       return true;
   }

  std::string getPositionString() const {
        return "(" + std::to_string(x) + ", " + std::to_string(y) + ")";
    }
};

class player:public entity{
    private:
    int life;
    int score;
    public:
    player(int id, int x, int y, char symbol='@', const std::string& name="Player")
        : entity(id, x, y, symbol, name, entitytype::player),life(5),score(0) {}
    int getlife() const { return life; }
    int getscore() const { return score; }

    void takedamage(int damage){
        life-=damage;
    }

    void addscore(int points){
        score+=points;
    }

    void update() override{
        //更新玩家状态
        if(life<=0){
            setActive(false);
        }
    }
};

class enemy:public entity{
    private:
    int damageattack;
    int life;
    public:
    enemy(int id, int x, int y, char symbol='E', const std::string& name="Enemy")
        : entity(id, x, y, symbol, name, entitytype::enemy),damageattack(1),life(1) {}
    int getdamageattack() const { return damageattack; }
    int getlife() const { return life; }

    void update() override{
        //更新敌人状态
        if(life<=0){
            setActive(false);
        }
    }
};


   



class gamemap{
    private:
    static const int map_length=8;//地图长度为8
    static const int map_width=8;//地图宽度为8
    char grid[map_length][map_width];//地图数组

    bool isvalidposition(int y,int x) const{
        return (y>=0&&y<map_length&&x>=0&&x<map_width);
    }
//实体管理
    std::vector<std::unique_ptr<entity>> entities;
    player* player_ptr;//玩家指针

    public:
    gamemap(){
        fillall('#');
    }
    int getMapLength(){
        return map_length;
    }
    int getMapWidth(){
        return map_width;
    }
    void fillall(char c){
        for(int i=0;i<map_length;i++){
            for(int j=0;j<map_width;j++){
                grid[i][j]=c;
            }
        }
    }
    void clearmap(char c='#')
    {
        fillall(c);
    }

    player* createplayer(int id,int x,int y,char symbol='@', const std::string& name="Player"){
        auto newplayer=std::make_unique<player>(id,x,y,symbol,name);
        player_ptr=newplayer.get();
        entities.push_back(std::move(newplayer));
        updategridfromentities();
        return player_ptr;
    }

    enemy* createenemy(int id,int x,int y,char symbol='E', const std::string& name="Enemy"){
        auto newenemy=std::make_unique<enemy>(id,x,y,symbol,name);
        enemy* ptr = newenemy.get();
        entities.push_back(std::move(newenemy));
        updategridfromentities();
        return ptr;
    }

    void updategridfromentities(){
        fillall('#');//先重置地图

        for(auto& entity:entities){
            if(entity->isActive()){
                int x=entity->getX();
                int y_display = entity->getY();
                int y_internal = map_length - 1 - y_display;
                if(x >= 0 && x < map_width && y_display >= 0 && y_display < map_length){
                    grid[y_internal][x]=entity->getSymbol();
                }
            }
        }
    }
    
    player* getplayer()const{
        return player_ptr;
    }

    std::vector<enemy*> getenemies() const {
        std::vector<enemy*> result;
        for (const auto& entity : entities) {
            if(entity->getType()==entitytype::enemy){
                result.push_back(static_cast<enemy*>(entity.get()));
            }
            
        }
        return result;
    }

    std::vector<enemy*> getEntitiesAt(int x, int y_display) const {
        std::vector<enemy*> result;
        for (const auto& entity : entities) {
            if (entity->isActive() && entity->getX() == x && entity->getY() == y_display) {
                if(entity->getType()==entitytype::enemy){
                    result.push_back(static_cast<enemy*>(entity.get()));
                }
            }
        }
        return result;
    }

    bool hasenemyat(int x, int y) const{
        auto enemies=getEntitiesAt(x,y);
        for(auto e:enemies){
            if(e->isActive()){
                return true;
            }
        }
        return false;
    }

    bool moveplayerto(int targetx, int targety_display){
        if(!player_ptr||!player_ptr->isActive()){
            return false;
        }
        int oldx=player_ptr->getX();
        int oldy=player_ptr->getY();
        if(targetx < 0 || targetx >= map_width || targety_display < 0 || targety_display >= map_length){
            return false;
        }
        if(targetx==oldx&&targety_display==oldy){
            return false;
        }
        if(hasenemyat(targetx,targety_display)){

            return false;
        }

        player_ptr->setPosition(targetx,targety_display);
        updategridfromentities();
        std::cout<<"玩家移动到"<<targetx<<","<<targety_display<<std::endl;
        return true;

       }

    bool moveplayerbyinput(){
       if(!player_ptr||!player_ptr->isActive()){
        std::cout<<"玩家不存在"<<std::endl;
        return false;
    }
    int targetx,targety;
    std::cout<<"请输入目标坐标(x,y):"<<std::endl;
    std::cin>>targetx>>targety;
    if(!moveplayerto(targetx,targety)){
        std::cout<<"移动失败"<<std::endl;
        return false;
    }
    std::cin.ignore(10000,'\n');
    return moveplayerto(targetx,targety);
    }


       void print()const{
        std::cout<<"+";
        for(int i=0;i<map_width;i++){
            std::cout<<"--";
        }
        std::cout<<"-+\n";
        for(int y=0;y<map_length;y++){
            std::cout<<" " << (map_length - 1 - y) << " | ";
            for(int x=0;x<map_width;x++){
                std::cout<<grid[y][x]<<" ";
            }
            std::cout<<"|\n";
        }
        std::cout<<"   +";
        for(int i=0;i<map_width;i++){
            std::cout<<"--";
        }
        std::cout<<"-+\n";

        std::cout << "    ";
        for (int x = 0; x < map_width; x++) {
            std::cout << " " << x;
        }
        std::cout << "\n\n";
    }

    void printWithPlayerPosition() const {
        if (!player_ptr) return;
        
        print();
        
        // 标记玩家位置
        std::cout << "\n玩家位置: " << player_ptr->getPositionString() << "\n";
        
        // 显示可移动范围提示
        std::cout << "地图坐标范围: x: 0-" << (map_width-1) << ", y: 0-" << (map_length-1) << "\n";
    }

    void printEntitiesInfo() const {
        std::cout << "\n=== 实体信息 ===\n";
        for (const auto& entity : entities) {
            if (entity->isActive()) {
                std::cout << "ID: " << entity->getId() 
                          << " 名称: " << entity->getName()
                          << " 位置: " << entity->getPositionString()
                          << " 符号: " << entity->getSymbol() << "\n";
                
                if (auto playerPtr = dynamic_cast<player*>(entity.get())) {
                    std::cout << "  类型: 玩家 生命: " << playerPtr->getlife() 
                              << " 分数: " << playerPtr->getscore() << "\n";
                } else if (auto enemyPtr = dynamic_cast<enemy*>(entity.get())) {
                    std::cout << "  类型: 敌人 攻击力: " << enemyPtr->getdamageattack() << "\n";
                }
            }
        }
        std::cout << "================\n";
    }

    void printEnemiesPositions() const {
        std::cout << "\n=== 敌人位置信息 ===\n";
        int enemyCount = 0;
        for (const auto& entity : entities) {
            if (entity->isActive() && entity->getType() == entitytype::enemy) {
                std::cout << "敌人" << ++enemyCount << ": " << entity->getName()
                          << " 位置: " << entity->getPositionString() << "\n";
            }
        }
        if (enemyCount == 0) {
            std::cout << "当前没有敌人\n";
        }
        std::cout << "==================\n";
    }

     void updateAllEntities() {
        for (auto& entity : entities) {
            if (entity->isActive()) {
                entity->update();
            }
        }
        updategridfromentities(); 
    }

    

};
    #endif