#include <iostream>
#include <ctime>
#include <chrono>
#include <omp.h>
#include "input.h"
#include "MeshDataAgent.h"
#include "ObjectTime.h"
#include "DofMap.h"



void solve(std::shared_ptr<DataBase> solve_db, std::shared_ptr<MeshDataAgent> mesh);
void printLogo();
void test(std::shared_ptr<DataBase> db, std::shared_ptr<MeshDataAgent> mda);
int main(int argc, char *argv[])
{
    printLogo();
    std::cout << "Analysis Start ..." << std::endl;
    TimeMater::getInstance()->start();

    // build modal
    auto input = make_shared<Input>(argv[1]);
    auto rootDb = input->d_root_db;
    auto modalMarketDb = rootDb->getDataBase("modal_market");
    auto globalControlDb = rootDb->getDataBase("global_control");
    auto mda = make_shared<MeshDataAgent>(globalControlDb);    
    solve(rootDb->getDataBase("solver"), mda);
    test(rootDb, mda);
    TimeMater::getInstance()->getPassedCpuTimeFromStart("all solve time");





    return 0;
}
