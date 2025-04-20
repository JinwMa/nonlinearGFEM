#include <iostream>
#include <ctime>
#include <chrono>
#include "input.h"
#include "mesh.h"
#include "ObjectTime.h"
#include "DofMap.h"
void solve(std::shared_ptr<DataBase> solve_db, std::shared_ptr<Mesh> mesh);
int main(int argc, char *argv[])
{
    TimeMater::getInstance()->start();
    // 读取输入文件
    auto input = make_shared<Input>(argv[1]);
    // 设置根数据库
    auto root_db = input->d_root_db;
    // 模型市场数据库
    auto modal_market_db = root_db->getDataBase("modal_market");
    // 全局控制数据库
    auto global_control_db = root_db->getDataBase("global_control");
    // 创建网格类
    auto mesh = make_shared<Mesh>(global_control_db);

    auto dof_map = make_shared<DofMap>(mesh);
    // 求解
    solve(root_db->getDataBase("solver"), mesh);
    TimeMater::getInstance()->getPassedCpuTimeFromLast();
    return 0;
}
