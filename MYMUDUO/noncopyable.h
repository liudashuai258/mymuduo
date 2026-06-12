/*
    noncopyable被继承以后，派生类对象可以正常构造和析构，但是不能被复制和赋值。
    防止重复释放资源导致内存泄漏或者double free错误。
    原理是对复制构造函数和赋值运算符进行删除（delete），使得编译器在尝试使用这些函数时产生编译错误，从而禁止了对象的复制和赋值操作。
*/
#pragma once
class noncopyable
{
    public:
    //禁止复制
    noncopyable(const noncopyable&)=delete;
    //禁止赋值
    noncopyable& operator=(const noncopyable&)=delete;
    protected://可以被继承
    //允许子类构造
    noncopyable()=default;
    //允许子类析构
    ~noncopyable()=default;
};