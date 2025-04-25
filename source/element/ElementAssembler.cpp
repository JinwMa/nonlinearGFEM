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
        d_element_pointers.push_back(element);
        // step 3 : build element groups    
        std::vector<int> igroup;
        getGroup(sub_db, d_mesh, igroup);
        d_element_groups.push_back(igroup);
    }
}


void ElementAssembler::allocateElementData(std::vector<ElementData> & elementDatas)
{
}





