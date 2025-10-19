#ifndef ELEMENTASSEMBLER_H
#define ELEMENTASSEMBLER_H

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


/*
   执行所有需要循环单元的操作，如计算刚度等
*/

class ElementProcessingStrategy {
public:
    virtual ~ElementProcessingStrategy() = default;
    virtual void processElement(std::shared_ptr<BaseElement> elementPointer,
                               int elementId, 
                               const std::vector<int>& nodes, 
                               std::vector<Dof>& dofs) = 0;
    
    // 可选：提供线程相关的上下文信息
    virtual void onThreadStart(int threadId) {}
    virtual void onThreadEnd(int threadId) {}
};

class DofCreationStrategy : public ElementProcessingStrategy {
public:
    void processElement(std::shared_ptr<BaseElement> elementPointer,
                       int elementId, 
                       const std::vector<int>& nodes, 
                       std::vector<Dof>& dofs) override {
        elementPointer->createDofsInElement(elementId, nodes, dofs);
    }
    
    void onThreadStart(int threadId) override {
        #pragma omp critical
        std::cout << "线程 " << threadId << " 开始创建自由度" << std::endl;
    }
};


// 上下文类 - 封装并行处理框架
class ParallelPartProcessor {
private:
    std::shared_ptr<DataBase> d_db_;
    std::shared_ptr<MeshDataAgent> d_mda_;
    std::unique_ptr<ElementProcessingStrategy> strategy_;
    
public:
    ParallelPartProcessor(std::shared_ptr<DataBase> db, 
                         std::shared_ptr<MeshDataAgent> mda)
        : d_db_(std::move(db)), d_mda_(std::move(mda)) {}
    
    void setStrategy(std::unique_ptr<ElementProcessingStrategy> strategy) {
        strategy_ = std::move(strategy);
    }
    
    void execute() {
        if (!strategy_) {
            throw std::runtime_error("未设置处理策略");
        }
        
        int max_threads = d_db_->getInt("max_threads");
        omp_set_num_threads(max_threads);    
        std::cout << "配置最大线程数: " << max_threads << std::endl;    
        
        for (auto partId : d_mda_->getPartIdList()) {
            processPart(partId);
        }
    }
    
private:
    void processPart(int partId) {
        std::cout << "处理 partId: " << partId << std::endl;        
        std::vector<int> elements;
        d_mda_->getElementListByPardId(partId, elements);
        std::string et = d_mda_->getElementTypeByPartId(partId);
        std::shared_ptr<BaseElement> elementPointer = createElement(et);
        std::cout << "元素数量: " << elements.size() << std::endl;        
        
        #pragma omp parallel
        {
            int threadId = omp_get_thread_num();
            strategy_->onThreadStart(threadId);
            
            #pragma omp for
            for (int i = 0; i < elements.size(); i++) {
                auto elementId = elements[i];
                std::vector<int> nodes;
                std::vector<Dof> dofs;
                d_mda_->getElementNodes(elementId, nodes);
                
                strategy_->processElement(elementPointer, elementId, nodes, dofs);
            }
            
            strategy_->onThreadEnd(threadId);
        }
        
        std::cout << "完成 partId: " << partId << std::endl << std::endl;
    }
    
    std::shared_ptr<BaseElement> createElement(const std::string& elementType) {
        if (elementType == "C3D8R") return std::make_shared<SolidHex8>();
        // 更多元素类型...
        throw std::runtime_error("未知的元素类型: " + elementType);
    }
};





class ElementAssembler
{

public:
    ElementAssembler(std::shared_ptr<DataBase> db,
                     std::shared_ptr<MeshDataAgent> mda,
                     std::shared_ptr<DofMap> dofMap) : d_db(db), d_mda(mda), d_dofMap(dofMap) {};
    ~ElementAssembler() {};

    // 基于单元创建自由度
    void createDofbyElements();



private:
    int max_threads = 6;
    std::shared_ptr<DataBase> d_db;
    std::shared_ptr<MeshDataAgent> d_mda;
    std::shared_ptr<DofMap> d_dofMap;
};

#endif