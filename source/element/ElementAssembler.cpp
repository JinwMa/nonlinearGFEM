#include "ElementAssembler.h"

void ElementAssembler::takeDB()
{
    // step 1 : get element list
    d_element_list = d_db->getVectorString("refer_element_list");
    auto db = d_db->d_root_db->getDataBase("modal_market");
    for (auto it : d_element_list)
    {
        auto sub_db = db->getDataBase(it);
        // step 2 : get element pointers
        std::string element_type = sub_db->getString("type");
        auto element = newElement(element_type);
        element->takeDB(sub_db);
        element->init();
        d_element_pointers.push_back(element);
        // step 3 : build element groups    
        std::vector<int> igroup;
        getGroup(sub_db, d_mesh, igroup);
        d_element_groups.push_back(igroup);
    }
}



// 开辟连续空间：不要并行
void ElementAssembler::allocateElementData(std::vector<ElementData> & elementDatas)
{
    elementDatas.resize(d_mesh->d_actual_element_count);
   int numType = d_element_list.size();
   for (int i = 0; i < numType; i++)
   {
       auto elementPointer = d_element_pointers[i];
       auto elementGroup = d_element_groups[i];    
// #pragma omp parallel for
       for (auto iElement : elementGroup)
       {
        elementPointer->allocateElementData(elementDatas[iElement]);
       }
   }
}

void ElementAssembler::preBuildNodesDofs()
{
    int numType = d_element_list.size();
    for (int i = 0; i < numType; i++)
    {
        auto elementPointer = d_element_pointers[i];
        auto elementGroup = d_element_groups[i];
        for (auto iElement : elementGroup)
        {
            auto tags = elementPointer->getDofLab();            
            auto Ints = d_dof_map->transTagsToInts(tags);
            auto nodeIds = d_mesh->d_element_connectivity[iElement];
            for (auto i : nodeIds)
            {
                int nodeInternalId = d_mesh->getNodeInternalId(i);
                d_dof_map->d_nodes_dofs[nodeInternalId].insert(Ints.begin(), Ints.end());
            }
        }
    }
}
