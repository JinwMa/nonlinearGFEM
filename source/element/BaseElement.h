#ifndef BASEELEMENT1_H
#define BASEELEMENT1_H
class BaseElement
{
    private:
    int num_edofs;
    int num_nodes;
    public:
    int integration_order = 2;
    virtual ~BaseElement() = 0; //定义纯虚析构函数
    virtual void ComputeStiffness();
    virtual void ComputeInternalForce();
    virtual void SetGaussIntegration() ;
    virtual void getShapeFunction() ;
    
};

#endif