#include"ConstraintManager.h"
// #include"SPC.h"



using namespace std;

void ConstraintManager::takeDB(Input & input, Mesh & mesh)
{
    //首先检查是否存在boundary_conditions
    vector<ConstraintEquation> equations;
    auto it = input.db.find("boundary_conditions");
    if (it == input.db.end())
    {
        std::cout << "the key boundary_condition is not found" << std::endl;
        exit(1);
    }
    // 若存在，则一一读取
    auto constraints = it->second;
    for (size_t i = 0; i < constraints.size(); i++)
    {
        auto it = input.db.find(constraints[i]+"_type");
        if (it == input.db.end())
        {
            std::cout << "the key " << constraints[i]+"_type" << " is not found" << std::endl;
            exit(1);
        }
        if (it->second.size() != 1) 
        {
            cout << "the format of " << it->first << " is wrong" << endl;
            exit(1);
        }
        std::cout << it->second[0] << std::endl;
        if (it->second[0] == "SPC")
        {
            // auto constrain = new SPC(it->first);
            // constrain->takeDB(input, mesh);
            // constrain->buildDofMap();
            // equations = constrain->buildEquations();
            // delete constrain;
            // FinalConstraintEquations.push_back(equations);
        }
        else
        {
            cout << "not support this type : " << it->second[0] << endl;
            exit(1);
        }
        std::cout << "111111" << std::endl;
        
    }
}