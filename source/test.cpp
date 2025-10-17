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
    auto node_field = std::make_shared<NodeFieldData<double>>("aaa");
}