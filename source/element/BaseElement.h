#ifndef BASEELEMENT1_H
#define BASEELEMENT1_H
class BaseElement
{
    public:
    virtual ~BaseElement() = 0; //定义纯虚析构函数
    virtual void ComputeStiffness();
};

#endif