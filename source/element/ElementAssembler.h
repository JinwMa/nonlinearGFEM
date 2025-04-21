#ifndef ELEMENTASSEMBLER_H
#define ELEMENTASSEMBLER_H

#include<Eigen/Dense>
#include <Eigen/Sparse>
#include "input.h"
#include "DofMap.h"
#include "mesh.h"
#include "ObjectControlParam.h"
#include "ObjectElementData.h"

class ElementAssembler
{
    private:
    int max_threads = 6;

    public:
    ElementAssembler(std::shared_ptr<DataBase> db,
                    std::shared_ptr<DofMap> dof_map,
                    std::shared_ptr<Mesh> mesh)
    {
        d_mesh = mesh;
        d_db = db;
        d_dof_map = dof_map;
    }
    virtual ~ElementAssembler();
    
    void init()
    {
        takeDB();
    }
    void takeDB(){};
    void assembleMaterix(const std::string type, 
                         std::vector<ElementData> elementData,
                         Eigen::SparseMatrix<double> & matrix);
    void assembleVector(const std::string type, 
                        std::vector<ElementData> elementData,
                        Eigen::VectorXd & vector);


    public:
    std::shared_ptr<DataBase> d_db;
    std::shared_ptr<DofMap> d_dof_map;
    std::shared_ptr<Mesh> d_mesh;
    std::vector<std::string> d_element_list;
 
};





#endif