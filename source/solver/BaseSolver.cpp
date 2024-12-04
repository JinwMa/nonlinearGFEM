#include "BaseSolver.h"

void BaseSolver::init(Input *pinput, Mesh *pmesh)
{
    // 功能一：初始化elementdata
    int num_element = pmesh->actual_element_count;
    d_element_data.resize(num_element);
    for (int i = 0; i < num_element; i++)
    {
        int element_id = pmesh->ElementIdList[i];
        int element_order = pmesh->ElementOrderInList[element_id];
        int element_index = element_order - 1;
        auto &element_data = d_element_data[element_index];
        // 单元层面的初始化
        element_data.is_initialized = false;
        element_data.is_alive = true;
        element_data.element_id = element_id;
        element_data.element_layer = 0;
        element_data.element_order = element_order;

        // 节点层面的初始化
        auto node_ids = pmesh->NodesOnElements[element_index];
        int num_nodes = node_ids.size();
        element_data.num_nodes = num_nodes;
        element_data.node_ids = node_ids;
        element_data.coordinates.resize(num_nodes);
        for (int inode = 0; inode < num_nodes; inode++)
        {
            int node_id = node_ids[inode];
            int node_order = pmesh->NodeOrderInList[node_id];
            int node_index = node_order - 1;
            element_data.coordinates[inode] = pmesh->NodesCoordinate[node_index];
        }
        // 单元内部负责积分点上的初始化
    }

    // 功能二： 初始化控制参数
    d_contral_param = std::make_shared<ObjectContralParam>();
    d_contral_param->load_step = 0;
    d_contral_param->iteration_step = 0;
}

void BaseSolver::setVectorToElementData(std::vector<double> &vector,
                                        Dof_Map *pdof_map,
                                        std::vector<ObjectElementData> &ElementData,
                                        std::string name)
{
    if (pdof_map == nullptr)
        toolbox::error("pdof_map is null");

    const int v_size = vector.size();
    const int num_element = ElementData.size();

    if (name == "u")
    {
        for (int i = 0; i < num_element; i++)
        {
            auto &elementdata = ElementData[i];
            int num_nodes = elementdata.node_ids.size();
            if (num_nodes <= 0)
                toolbox::error("elementdata is not initialized");
            if (elementdata.u.size() == 0)
                elementdata.u.resize(num_nodes);
            for (int inode = 0; inode < num_nodes; inode++)
            {
                int node_id = elementdata.node_ids[inode];
                auto &dofs = pdof_map->NodesDofs[node_id];
                int dof_size = dofs.size();
                if (elementdata.u[inode].size() == 0)
                    elementdata.u[inode].resize(dof_size);
                for (size_t idof = 0; idof < dof_size; idof++)
                {
                    int dof_index = pdof_map->getDofIndex(node_id, dofs[idof]);
                    elementdata.u.at(inode).at(idof) = vector.at(dof_index);
                }
            }
        }
    }
    else if (name == "du")
    {
        for (int i = 0; i < num_element; i++)
        {
            auto &elementdata = ElementData[i];
            int num_nodes = elementdata.node_ids.size();
            if (num_nodes <= 0)
                toolbox::error("elementdata is not initialized");
            if (elementdata.du.size() == 0)
                elementdata.du.resize(num_nodes);
            for (int inode = 0; inode < num_nodes; inode++)
            {
                int node_id = elementdata.node_ids[inode];
                auto &dofs = pdof_map->NodesDofs[node_id];
                int dof_size = dofs.size();
                if (elementdata.du[inode].size() == 0)
                    elementdata.du[inode].resize(dof_size);
                for (size_t idof = 0; idof < dof_size; idof++)
                {
                    int dof_index = pdof_map->getDofIndex(node_id, dofs[idof]);
                    elementdata.du.at(inode).at(idof) = vector.at(dof_index);
                }
            }
        }        
    }
    else if (name == "ddu")
    {
        for (int i = 0; i < num_element; i++)
        {
            auto &elementdata = ElementData[i];
            int num_nodes = elementdata.node_ids.size();
            if (num_nodes <= 0)
                toolbox::error("elementdata is not initialized");
            if (elementdata.ddu.size() == 0)
                elementdata.ddu.resize(num_nodes);
            for (int inode = 0; inode < num_nodes; inode++)
            {
                int node_id = elementdata.node_ids[inode];
                auto &dofs = pdof_map->NodesDofs[node_id];
                int dof_size = dofs.size();
                if (elementdata.ddu[inode].size() == 0)
                    elementdata.ddu[inode].resize(dof_size);
                for (size_t idof = 0; idof < dof_size; idof++)
                {
                    int dof_index = pdof_map->getDofIndex(node_id, dofs[idof]);
                    elementdata.ddu.at(inode).at(idof) = vector.at(dof_index);
                }
            }
        }            
    }
    else
    {
        toolbox::error(name + " is given error in BaseSolver::setVectorToElementData");
    }
}

void BaseSolver::linear_solver(const Eigen::SparseMatrix<double> &K,
                               Eigen::VectorXd &P,
                               const Eigen::SparseMatrix<double> &C,
                               Eigen::VectorXd &G,
                               Eigen::VectorXd &x,
                               const std::string type)
{
    x.setZero();
    auto start = std::chrono::high_resolution_clock::now();
    int K_row = K.rows();
    int K_col = K.cols();
    int C_row = C.rows();
    int C_col = C.cols();
    // 判断K是否为方阵
    if (K_row != K_col)
        toolbox::error("K size is wrong in linear solver");
    // 判断C和K是否列数相同
    if (K_col != C_col)
        toolbox::error("C size is wrong in linear solver");
    // 检查P和G的正确性
    if (P.size() != K_row)
        toolbox::error("P size is wrong in linear solver");
    if (G.size() != C_row)
        toolbox::error("G size is wrong in linear solver");

    Eigen::SparseMatrix<double> K_AL(K_row + C_row, K_row + C_row);
    K_AL.setZero();

    // 将K塞到增广矩阵中
    for (int ii = 0; ii < K.outerSize(); ++ii)
    {
        for (Eigen::SparseMatrix<double>::InnerIterator it(K, ii); it; ++it)
        {
            K_AL.coeffRef(it.row(), it.col()) = it.value();
        }
    }
    // 将C塞到增广矩阵中
    for (int ii = 0; ii < C.outerSize(); ++ii)
    {
        for (Eigen::SparseMatrix<double>::InnerIterator it(C, ii); it; ++it)
        {
            K_AL.coeffRef(it.row() + K_row, it.col()) = it.value();
        }
    }
    // 将C的转置塞到增广矩阵中
    Eigen::SparseMatrix<double> CT = C.transpose();
    for (int ii = 0; ii < CT.outerSize(); ++ii)
    {
        for (Eigen::SparseMatrix<double>::InnerIterator it(CT, ii); it; ++it)
        {
            K_AL.coeffRef(it.row(), it.col() + K_col) = it.value();
        }
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> duration = end - start;
    if(std::getenv("CHECKSOLVER") != nullptr)std::cout << "assembleK_AL: " << duration.count() << " ms" << std::endl;

    // 设置PG向量
    Eigen::VectorXd PG(P.size() + G.size());
    PG.setZero();

    PG.head(P.size()) = Eigen::Map<Eigen::VectorXd>(P.data(), P.size());
    // 将 B 的元素拷贝到 result 的后半部分
    PG.tail(G.size()) = Eigen::Map<Eigen::VectorXd>(G.data(), G.size());

    // 求解 K_AL * x = PG
    Eigen::PardisoLU<Eigen::SparseMatrix<double>> solver;

    solver.compute(K_AL);

    if (solver.info() != Eigen::Success)
        toolbox::error("分解失败");
    x = solver.solve(PG);
    if (solver.info() != Eigen::Success)
        toolbox::error("求解失败");

    if (std::getenv("CHECKSOLVER") != nullptr)
    {
        std::cout << "checking the error of linear_equation_solver:" << std::endl;
        Eigen::VectorXd test = K_AL * x;
        std::vector<double> a(PG.size());
        std::vector<double> b(test.size());
        for (int i = 0; i < PG.size(); i++)
            a[i] = PG[i];
        for (int i = 0; i < test.size(); i++)
            b[i] = test[i];
        toolbox::checkvector(a, b);
    }
}

void BaseSolver::linear_solver2(const Eigen::SparseMatrix<double> &K,
                                Eigen::VectorXd &P,
                                const Eigen::SparseMatrix<double> &C,
                                Eigen::VectorXd &G,
                                Eigen::VectorXd &x,
                                const std::string type)
{
    int K_row = K.rows();
    int K_col = K.cols();
    int C_row = C.rows();
    int C_col = C.cols();
    // 判断K是否为方阵
    if (K_row != K_col)
        toolbox::error("K size is wrong in linear solver");
    // 判断C和K是否列数相同
    if (K_col != C_col)
        toolbox::error("C size is wrong in linear solver");
    // 检查P和G的正确性
    if (P.size() != K_row)
        toolbox::error("P size is wrong in linear solver");
    if (G.size() != C_row)
        toolbox::error("G size is wrong in linear solver");

    Eigen::SparseMatrix<double> CT = C.transpose();
    Eigen::SparseMatrix<double> CTC;
    sparseMatrixMultiply(CT, C, CTC);
    CTC = CTC * 1.E11;
    Eigen::SparseMatrix<double> K_AL = K + CTC;

    // 设置PG向量
    Eigen::VectorXd PG = CT * G * 1.E11 + P;
    // 求解 K_AL * x = PG
    // Eigen::PardisoLU<Eigen::SparseMatrix<double>> solver;
    Eigen::PardisoLDLT<Eigen::SparseMatrix<double>> solver; // 23.6s
    // Eigen::ConjugateGradient<Eigen::SparseMatrix<double>> solver;   //121s

    solver.compute(K_AL);

    if (solver.info() != Eigen::Success)
        toolbox::error("分解失败");
    x = solver.solve(PG);
    if (solver.info() != Eigen::Success)
        toolbox::error("求解失败");

    if (std::getenv("CHECKSOLVER") != nullptr)
    {
        std::cout << "checking the error of linear_equation_solver:" << std::endl;
        Eigen::VectorXd test = K_AL * x;
        std::vector<double> a(PG.size());
        std::vector<double> b(test.size());
        for (int i = 0; i < PG.size(); i++)
            a[i] = PG[i];
        for (int i = 0; i < test.size(); i++)
            b[i] = test[i];
        toolbox::checkvector(a, b);
    }
}

void BaseSolver::sparseMatrixMultiply(const Eigen::SparseMatrix<double> &A, const Eigen::SparseMatrix<double> &B, Eigen::SparseMatrix<double> &C)
{
    assert(A.cols() == B.rows() && "Matrix dimensions must agree.");
    C.resize(A.rows(), B.cols());

    Eigen::SparseMatrix<double> B_transposed = B.transpose();
    C.reserve(Eigen::VectorXi::Constant(A.rows(), num_nonzero)); // 假设每行最多有n个非零项,可以根据实际情况调整

    for (int i = 0; i < A.outerSize(); ++i)
    {
        for (Eigen::SparseMatrix<double>::InnerIterator itA(A, i); itA; ++itA)
        {
            // itA.row() 是行号, itA.col() 是列号, itA.value() 是值
            int rowA = itA.row();
            int colA = itA.col();
            double valueA = itA.value();

            // 遍历 B 的第 colA 行(即 B 转置后的第 colA 列)
            for (Eigen::SparseMatrix<double>::InnerIterator itB(B_transposed, colA); itB; ++itB)
            {
                int colB = itB.row(); // B 转置后的列号(实际是 B 的列号)
                double valueB = itB.value();
                // 将结果加入到三元组列表中
                C.coeffRef(rowA, colB) += valueA * valueB;
            }
        }
    }
}
