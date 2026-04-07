#include <cstdio>
#include <iostream>
#include <ctime>
#include <chrono>
#include <memory>

#include "input.h"
#include "MeshDataAgent.h"
#include "ObjectTime.h"
#include "DofMap.h"


#include "BaseFieldData.h"
#include "NodeFieldData.h"
#include "FieldManager.h"
#include "ElementAssembler.h"
#include "discretization/material/MaterialInitializer.h"
#include "discretization/material/LinearElastic.h"
#include "discretization/material/MaterialFactory.h"
#include "discretization/material/MaterialManager.h"
#include <Eigen/Dense>


void printLogo()
{
    printf("\n\n");
    printf("          ██████╗ ███████╗███████╗███╗   ███╗\n");
    printf("         ██╔════╝ ██╔════╝██╔════╝████╗ ████║\n");
    printf("         ██║  ███╗███████╗█████╗  ██╔████╔██║\n");
    printf("         ██║   ██║██╔════╝██╔══╝  ██║╚██╔╝██║\n");
    printf("         ╚██████╔╝██╗     ███████╗██║ ╚═╝ ██║\n");
    printf("          ╚═════╝ ╚═╝     ╚══════╝╚═╝     ╚═╝\n");
    printf("         GENERALIZED FINITE ELEMENT METHOD\n");
    printf("                   v1.0 | © 2025\n");
    printf("────────────────────────────────────────────────────────────────\n");
}

void test(std::shared_ptr<DataBase> db,
          std::shared_ptr<MeshDataAgent> mda)
{
    // Initialize materials from database
    try {
        auto modalMarketDb = db->getDataBase("modal_market");
        if (modalMarketDb) {
            MaterialInitializer::initializeFromDatabase(modalMarketDb);
        } else {
            std::cout << "警告: 未找到 modal_market 数据库" << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << "材料初始化错误: " << e.what() << std::endl;
    }

    // Test material functionality directly
    std::cout << "\n=== 材料模块测试 ===" << std::endl;
    try {
        // Test 1: Create linear elastic material directly
        std::cout << "测试1: 直接创建线弹性材料" << std::endl;
        auto steel = std::make_shared<LinearElastic>("Steel", 7850.0, 2.1e11, 0.3);
        std::cout << "  材料名称: " << steel->getName() << std::endl;
        std::cout << "  密度: " << steel->getDensity() << " kg/m³" << std::endl;
        std::cout << "  弹性模量: " << steel->getYoungsModulus() << " Pa" << std::endl;
        std::cout << "  泊松比: " << steel->getPoissonsRatio() << std::endl;
        std::cout << "  是否线性: " << (steel->isLinear() ? "是" : "否") << std::endl;

        // Test 2: Compute stress
        Eigen::VectorXd strain(6);
        strain << 0.001, 0.0, 0.0, 0.0, 0.0, 0.0; // Uniaxial strain
        Eigen::VectorXd stress = steel->computeStress(strain);
        std::cout << "\n测试2: 计算应力" << std::endl;
        std::cout << "  应变: " << strain.transpose() << std::endl;
        std::cout << "  应力: " << stress.transpose() << std::endl;

        // Test 3: Get constitutive matrix
        Eigen::MatrixXd D = steel->getDSDu();
        std::cout << "\n测试3: 本构矩阵 (6x6)" << std::endl;
        std::cout << "  前3行前3列:\n" << D.block<3,3>(0,0) << std::endl;

        // Test 4: Material factory
        std::cout << "\n测试4: 材料工厂" << std::endl;
        auto& factory = MaterialFactory::getInstance();
        // Create a simple database for material creation
        // (In real usage, database would be populated from input file)
        std::cout << "  材料工厂已初始化，支持类型: linear_elastic" << std::endl;

        // Test 5: Material manager
        std::cout << "\n测试5: 材料管理器" << std::endl;
        auto& manager = MaterialManager::getInstance();
        manager.addMaterial("test_steel", steel);
        auto retrieved = manager.getMaterial("test_steel");
        std::cout << "  材料添加和检索成功: " << retrieved->getName() << std::endl;
        std::cout << "  管理器中的材料数量: " << manager.getNumMaterials() << std::endl;

        std::cout << "=== 材料测试通过 ===" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "材料测试失败: " << e.what() << std::endl;
    }

    // auto node_field = std::make_shared<NodeFieldData<double>>("aaa");
    auto fieldManager = make_shared<FieldManager> (mda);
    fieldManager->createNodeField<double>("disp", 1, 0.0);
    // fieldManager->printNodeFieldData<double>("disp");
    fieldManager->createElementField<int>("alive", 1, 10);
    // fieldManager->printElementFieldData<int>("alive");

    DofTypes::Dof_Tag a = DofTypes::Dof_Tag::MAX;
    std::cout << DofTypes::to_string(a) << " " << DofTypes::to_int(a) << std::endl;
    std::cout << mda->getNodeInternalId(1) << std::endl;

    std::shared_ptr<DofMap> dofMap = make_shared<DofMap> (mda);

    std::shared_ptr<ElementAssembler> elementAssembler = make_shared<ElementAssembler> (db, mda, dofMap);

    elementAssembler->createDofbyElements();
    
}