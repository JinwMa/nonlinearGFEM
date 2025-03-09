#include <iostream>
#include <ctime>
#include <chrono>
#include "input.h"
#include "mesh.h"

void solve(std::shared_ptr<DataBase> solve_db, Mesh & mesh);
int main(int argc, char *argv[])
{
    // 读取输入文件
    Input input(argv[1]);

    // 设置根数据库
    auto root_db = input.d_root_db;

    // 模型市场数据库
    auto modal_market_db = root_db->getDataBase("modal_market");

    // 网格数据库
    auto mesh_db = modal_market_db->getDataBase("mesh");

    // 创建网格类
    Mesh mesh(mesh_db);

    solve(root_db->getDataBase("solve"), mesh);

    return 0;
}
