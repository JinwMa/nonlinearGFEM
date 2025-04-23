#include "ElementAssembler.h"

void ElementAssembler::takeDB()
{
    // step 1
    // ################################
    // build d_element_list
    // ################################
    d_element_list = d_db->getVectorString("refer_element_list");
    auto db = d_db->d_root_db->getDataBase("modal_market");
    for (auto it : d_element_list)
    {
        auto sub_db = db->getDataBase(it);
        // step 2
        // #############################
        // build d_element_pointers
        // #############################
        std::string element_type = sub_db->getString("type");
        auto element = make_shared<BaseElement>();
        if (element_type == "LinearHex8")
        {
            element = make_shared<LinearHex8>();
        }
        else
        {
            toolbox::error("not support this type of element");
        }
        element->takeDB(sub_db);
        d_element_pointers.push_back(element);
        // step 3
        // #########################
        // build d_element_groups
        // #########################
        auto sub_sub_db = sub_db->getDataBase("elementIds");
        std::string group_type = sub_sub_db->getString("type");
        std::vector<int> igroup;
        if (group_type == "range")
        {
            int start = sub_sub_db->getInt("start");
            int end = sub_sub_db->getInt("end");
            int inc = 1;
            if(sub_sub_db->ifExist("inc")) inc = sub_sub_db->getInt("inc");
            for (int i = start; i <= end; i = i + inc)
            {
                int element_id = d_mesh->getElementInternalId(i);
                igroup.push_back(element_id);
            }
        }
        else
        {
            toolbox::error("not support this type of elementIds");
        }
        d_element_groups.push_back(igroup);
    }
}


void ElementAssembler::allocateElementData(std::vector<ElementData> & elementDatas)
{
    int numElement = d_mesh->d_actual_element_count;
    elementDatas.resize(numElement);

    // 初始化单元基本数据
    for (int i = 0; i < numElement; i++)
    {
        auto & elementdata = elementDatas[i];
        elementdata.element_internal_id = i;
        auto elementConnect = d_mesh->d_element_connectivity[i];
        int numNodes = elementConnect.size();
        elementdata.node_internal_ids.resize(numNodes);
        elementdata.nodes_coords.resize(numNodes);
        for (int inode = 0; inode < numNodes; inode++)
        {
            int nodeInternalId = d_mesh->getNodeInternalId(elementConnect[inode]);
            elementdata.node_internal_ids[inode] = nodeInternalId;
            elementdata.nodes_coords[inode] = d_mesh->d_nodes_coordinate[nodeInternalId];
        }
    }

    // debug
    // for (auto & it : elementDatas)
    // {
        // it.debugPrint();
    // }
    // u以下的数据都在单元上开放

}





