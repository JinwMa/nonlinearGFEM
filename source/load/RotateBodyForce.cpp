#include "RotateBodyForce.h"
#include "elementinterface.h"


void RotateBodyForce::takeDB(Input * pinput, Mesh * pmesh, std::string name)
{
    if (name.empty()) toolbox::error("name of RotateBodyForce should not be empty");
    d_rotation_axis = pinput->getVectorDouble(name + "_rotation_axis");
    d_rotation_refer_point = pinput->getVectorDouble(name + "_rotation_refer_point");
    d_angular_velocity = pinput->getDouble(name + "_rotation_angular_velocity");
    if (!pinput->ifExist(name + "_set_id"))
    {
        d_element_ids = pmesh->ElementIdList;
    }
    else
    {
        int set_id = pinput->getInt(name + "_set_id");
        d_element_ids = pmesh->element_sets[set_id];
    }
}

void RotateBodyForce::buildLoadVector(Input * pinput,
                                      Mesh * pmesh,
                                      Dof_Map * pdofmap,
                                      std::vector<int> & dofs_index,
                                      std::vector<double> & loadvector)
{
    dofs_index.resize(pdofmap->dof_size);
    loadvector.resize(pdofmap->dof_size);
    for (int i = 0; i < pdofmap->dof_size; i++)dofs_index[i] = i;
    for (int i = 0; i < d_element_ids.size(); i++)
    {
        int element_id = d_element_ids[i];
        std::string element_set_name = pmesh->element_setname[element_id];
        d_set_elements[element_set_name].push_back(element_id);
    }
    for (auto set_elements : d_set_elements)
    {
        BaseElement * pelement;
        std::string element_set_name = set_elements.first;
        std::string element_type = pinput->getString(element_set_name + "_type");
        if (element_type == "LinearHex8")
        {
            pelement = new LinearHex8;
        }
        else if (element_type == "LinearTet4")
        {
            pelement = new LinearTet4;
        }
        else
        {
            toolbox::error("not supprot this type of element: " + element_type);
        }
        pelement->takeDB(pinput, pmesh, element_set_name);
        pelement->SetElement();
        std::vector<std::vector<double>> GaussPoint = pelement->d_GaussPoints;
        // 循环所有的单元
        for (int i = 0; i < set_elements.second.size(); i++)
        {
            int element_id = set_elements.second[i];
            int element_location = pmesh->ElementOrderInList[element_id];
            std::vector<int> node_ids_in_a_element = pmesh->NodesOnElements[element_location - 1];
            double nodes_coordinates[20][3] = {0.0, 0.0};
            // 拿到节点坐标
            for (int i = 0; i < pelement->numNodes; i++)
                for (int j = 0; j < pelement->dim; j++)
                {
                    int node_id = node_ids_in_a_element[i];
                    int node_location = pmesh->NodeOrderInList[node_id];
                    nodes_coordinates[i][j] = pmesh->NodesCoordinate[node_location - 1][j];
                }

            
            std::vector<double> jkb;
            std::vector<std::vector<double>> sf;
            std::vector<std::vector<std::vector<double>>> sf_dxyz;
            int num_GP = pelement->num_intergration_point;
            pelement->getShapeFunction(nodes_coordinates, GaussPoint, sf, sf_dxyz, jkb, num_GP);            
            // 循环高斯点
            int p_size = GaussPoint[0].size();
            for (size_t i = 0; i < GaussPoint.size(); i++)
            {                
                double x[3] = {0.0}; // 坐标
                // double y = 0.0;
                // double z = 0.0;
                for (size_t inode = 0; inode < node_ids_in_a_element.size(); inode++)
                {
                    x[0] = x[0] + nodes_coordinates[inode][0] * sf[i][inode];
                    x[1] = x[1] + nodes_coordinates[inode][1] * sf[i][inode];
                    x[2] = x[2] + nodes_coordinates[inode][2] * sf[i][inode];
                }
                double r[3] = {0.0};
                r[0] = x[0] - d_rotation_refer_point[0];
                r[1] = x[1] - d_rotation_refer_point[1];
                r[2] = x[2] - d_rotation_refer_point[2];
                double p1[3] = {0.0};
                double dir[3] = {0.0};
                for (int ii = 0; ii < 3; ii++) p1[ii] = d_rotation_refer_point[ii];
                for (int ii = 0; ii < 3; ii++) dir[ii] = d_rotation_axis[ii];

                double dis = toolbox::distance_point_to_line(p1, dir, x);
                double f = d_angular_velocity * d_angular_velocity * dis * jkb[i] * GaussPoint[i][p_size - 1]; // 积分点上的力
                // 确定积分点上力的方向
                double r1[3] = {0.0};
                r1[0] = d_rotation_axis[0];
                r1[1] = d_rotation_axis[1];
                r1[2] = d_rotation_axis[2];
                double r2[3] = {0.0};
                toolbox::a3_cross_b3(r, r1, r2); // r2 为平面的法向  
                double r3[3] = {0.0};
                toolbox::a3_cross_b3(r1, r2, r3); // r3 为积分点上力的方向
                toolbox::normalize_a3(r3);
                for (size_t inode = 0; inode < node_ids_in_a_element.size(); inode++)
                {
                    int node_id = node_ids_in_a_element[inode];
                    int node_dof_dx = pdofmap->dofmap[node_id * 6 - 6];
                    int node_dof_dy = pdofmap->dofmap[node_id * 6 - 5];
                    int node_dof_dz = pdofmap->dofmap[node_id * 6 - 4];
                    loadvector[node_dof_dx] += f * r3[0] * sf[i][inode];
                    loadvector[node_dof_dy] += f * r3[1] * sf[i][inode]; 
                    loadvector[node_dof_dz] += f * r3[2] * sf[i][inode]; 
                }
            }
        }
        delete pelement;
    }
}

void RotateBodyForce::buildLoadStiffnessMatrix(Input * pinput,
                                               Mesh * pmesh,
                                               Dof_Map * pdofmap,
                                               std::vector<int> & dofs_index,
                                               std::vector<double> & loadvector)
{

}