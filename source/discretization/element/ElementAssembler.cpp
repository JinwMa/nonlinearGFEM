#include "ElementAssembler.h"
#include <omp.h>
#include <thread>

void ElementAssembler::createDofbyElements()
{
    // 设置线程数
    max_threads = d_db->getInt("max_threads");
    omp_set_num_threads(max_threads);    
    std::cout << "配置最大线程数: " << max_threads << std::endl;    
    // 循环part
    for (auto partId : d_mda->getPartIdList())
    {
        std::cout << "处理 partId: " << partId << std::endl;        
        std::vector<int> elements;
        d_mda->getElementListByPardId(partId, elements);
        std::string et = d_mda->getElementTypeByPartId(partId);
        std::shared_ptr<BaseElement> elementPointer;
        std::cout << et << std::endl;
        if (et == "C3D8R") elementPointer = make_shared<SolidHex8>();        
        std::cout << "元素数量: " << elements.size() << std::endl;        
        // 使用并行for循环
        #pragma omp parallel for
        for (int i = 0; i < elements.size(); i++)
        {
            auto elementId = elements[i];
            std::vector<int> nodes;
            std::vector<Dof> dofs;
            d_mda->getElementNodes(elementId, nodes);
            int omp_id = omp_get_thread_num();
            int actual_threads = omp_get_num_threads();
            // #pragma omp critical
            // {
            //     std::cout << "元素 " << elementId << " 由线程 " << omp_id << " 处理" << std::endl;
            // }
            elementPointer->createDofsInElement(elementId, nodes, dofs);           
        }
        
        std::cout << "完成 partId: " << partId << std::endl << std::endl;
    }
}