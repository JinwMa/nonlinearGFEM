#ifndef GFEM_OPENMP_STRATEGY_H
#define GFEM_OPENMP_STRATEGY_H

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
#include "ElementStrategy.h"
#include "../material/MaterialManager.h"


class OmpStrategy
{
    public:
    OmpStrategy()
    {
        d_max_threads = omp_get_max_threads();
    }
    virtual ~OmpStrategy() = default;
    int d_max_threads;
};

// 上下文类 - 封装并行处理框架
class ParallelPartProcessor : public OmpStrategy
{
private:
    std::shared_ptr<DataBase> d_db_;
    std::shared_ptr<MeshDataAgent> d_mda_;
    std::unique_ptr<ElementStrategy> strategy_;

public:
    ParallelPartProcessor(std::shared_ptr<DataBase> db,
                          std::shared_ptr<MeshDataAgent> mda)
        : d_db_(std::move(db)), d_mda_(std::move(mda)) {}

    void setStrategy(std::unique_ptr<ElementStrategy> strategy)
    {
        strategy_ = std::move(strategy);
    }

    void execute()
    {
        if (!strategy_)
        {
            throw std::runtime_error("未设置处理策略");
        }

        int max_threads = std::min(d_db_->getInt("max_threads"), d_max_threads);
        omp_set_num_threads(max_threads);
        std::cout << "配置最大线程数: " << max_threads << std::endl;

        for (auto partId : d_mda_->getPartIdList())
        {
            processPart(partId);
        }
    }

private:
    void processPart(int partId)
    {
        std::cout << "处理 partId: " << partId << std::endl;
        std::vector<int> elements;
        d_mda_->getElementListByPartId(partId, elements);
        std::string et = d_mda_->getElementTypeByPartId(partId);
        std::shared_ptr<BaseElement> elementPointer = createElement(et);

        // Try to set material for this part
        try {
            // Get material ID from database (assuming format: partX_material_id or material_id)
            std::string materialKey = "part" + std::to_string(partId) + "_material_id";
            std::string materialId;
            if (d_db_->ifExist(materialKey)) {
                materialId = d_db_->getString(materialKey);
            } else if (d_db_->ifExist("material_id")) {
                materialId = d_db_->getString("material_id");
            } else {
                materialId = "default_material";
            }

            // Get material from manager and set to element
            auto& materialManager = MaterialManager::getInstance();
            if (materialManager.hasMaterial(materialId)) {
                auto material = materialManager.getMaterial(materialId);
                elementPointer->setMaterial(material);
                std::cout << "设置材料: " << materialId << " 给 partId: " << partId << std::endl;
            } else {
                std::cout << "警告: 材料 " << materialId << " 未找到，使用默认材料" << std::endl;
                // Optionally create a default linear elastic material
                // auto defaultMaterial = std::make_shared<LinearElastic>("default", 0.0, 1.0e7, 0.3);
                // elementPointer->setMaterial(defaultMaterial);
            }
        } catch (const std::exception& e) {
            std::cerr << "设置材料时出错: " << e.what() << std::endl;
        }

        std::cout << "元素数量: " << elements.size() << std::endl;

#pragma omp parallel
        {
            int threadId = omp_get_thread_num();
            strategy_->onThreadStart(threadId);

#pragma omp for
            for (int i = 0; i < elements.size(); i++)
            {
                auto elementId = elements[i];
                std::vector<int> nodes;
                std::vector<Dof> dofs;
                d_mda_->getElementNodes(elementId, nodes);

                strategy_->processElement(elementPointer, elementId, nodes, dofs);
            }

            strategy_->onThreadEnd(threadId);
        }

        std::cout << "完成 partId: " << partId << std::endl
                  << std::endl;
    }

    std::shared_ptr<BaseElement> createElement(const std::string &elementType)
    {
        if (elementType == "C3D8R")
            return std::make_shared<SolidHex8>();
        // 更多元素类型...
        throw std::runtime_error("未知的元素类型: " + elementType);
    }
};

#endif