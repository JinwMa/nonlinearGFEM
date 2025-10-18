#include <cstdio>
#include <iostream>
#include <ctime>
#include <chrono>
#include "input.h"
#include "MeshDataAgent.h"
#include "ObjectTime.h"
#include "DofMap.h"

#include "BaseFieldData.h"
#include "NodeFieldData.h"
#include "FieldManager.h"
#include "DofMap.h"
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

void testNodeField(std::shared_ptr<MeshDataAgent> mda)
{
    // auto node_field = std::make_shared<NodeFieldData<double>>("aaa");
    auto field_manager = make_shared<FieldManager> (mda);
    field_manager->createNodeField<double>("disp", 1, 0.0);
    // field_manager->printNodeFieldData<double>("disp");
    field_manager->createElementField<int>("alive", 1, 10);
    // field_manager->printElementFieldData<int>("alive");

    DofTypes::Dof_Tag a = DofTypes::Dof_Tag::MAX;
    std::cout << DofTypes::to_string(a) << " " << DofTypes::to_int(a) << std::endl;
    std::cout << mda->getNodeInternalId(1) << std::endl;

    std::shared_ptr<DofMap> dofMap = make_shared<DofMap> (mda);
    
}