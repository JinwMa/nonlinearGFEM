#ifndef GFEM_ELEMENT_STRATEGY_H
#define GFEM_ELEMENT_STRATEGY_H

#include <iostream>
#include <string>
#include <vector>
#include <Eigen/Sparse>
#include <Eigen/PardisoSupport>
#include <Eigen/Dense>
#include <ctime>
#include <fstream>
#include <omp.h>
#include <chrono>

#include "ElementInterFace.h"
#include "input.h"
#include "MeshDataAgent.h"
#include "DofMap.h"
#include "toolbox.h"

class ElementStrategy
{
public:
    virtual ~ElementStrategy() = default;
    virtual void processElement(std::shared_ptr<BaseElement> elementPointer,
                                int elementId,
                                const std::vector<int> &nodes,
                                std::vector<Dof> &dofs) = 0;

    // 可选：提供线程相关的上下文信息
    virtual void onThreadStart(int threadId) {}
    virtual void onThreadEnd(int threadId) {}
};

class DofCreationStrategy : public ElementStrategy
{
public:
    void processElement(std::shared_ptr<BaseElement> elementPointer,
                        int elementId,
                        const std::vector<int> &nodes,
                        std::vector<Dof> &dofs) override
    {
        elementPointer->createDofsInElement(elementId, nodes, dofs);
    }

    void onThreadStart(int threadId) override
    {
#pragma omp critical
        std::cout << "线程 " << threadId << " 开始创建自由度" << std::endl;
    }
};

#endif