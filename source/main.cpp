#include <iostream>
#include <ctime>
#include <chrono>
#include "input.h"
#include "MeshDataAgent.h"
#include "ObjectTime.h"
#include "DofMap.h"


void solve(std::shared_ptr<DataBase> solve_db, std::shared_ptr<MeshDataAgent> mesh);
void printLogo();
void testNodeField(std::shared_ptr<MeshDataAgent> mda);
int main(int argc, char *argv[])
{
    printLogo();
    std::cout << "Analysis Start ..." << std::endl;
    TimeMater::getInstance()->start();

    // build modal
    auto input = make_shared<Input>(argv[1]);
    auto root_db = input->d_root_db;
    auto modal_market_db = root_db->getDataBase("modal_market");
    auto global_control_db = root_db->getDataBase("global_control");
    auto mda = make_shared<MeshDataAgent>(global_control_db);    
    solve(root_db->getDataBase("solver"), mda);
    testNodeField(mda);
    TimeMater::getInstance()->getPassedCpuTimeFromStart("all solve time");
    return 0;
}
