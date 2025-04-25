#include "ElementInterface.h"


std::shared_ptr<BaseElement> newElement(const std::string & type)
{
    if (type == "LinearHex8")
    {
        return make_shared<LinearHex8>();
    }
    else 
    {
        toolbox::error("not supported element type");
    }
    return nullptr;
}



void getGroup(std::shared_ptr<DataBase> db,
              std::shared_ptr<Mesh> mesh,
              std::vector<int> & igroup)
{
    auto sub_sub_db = db->getDataBase("elementIds");
    std::string group_type = sub_sub_db->getString("type");
    if (group_type == "range")
    {
        int start = sub_sub_db->getInt("start");
        int end = sub_sub_db->getInt("end");
        int inc = 1;
        if (sub_sub_db->ifExist("inc"))
            inc = sub_sub_db->getInt("inc");
        for (int i = start; i <= end; i = i + inc)
        {
            int element_id = mesh->getElementInternalId(i);
            igroup.push_back(element_id);
        }
    }
    else
    {
        toolbox::error("not support this type of elementIds");
    }
}