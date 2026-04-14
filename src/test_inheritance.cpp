#include <iostream>
// 关键：设置控制台UTF-8编码，彻底解决中文乱码
#ifdef _WIN32
#include <windows.h>
#endif

// 基类Base1
class Base1 {
protected:
    int a;
public:
    Base1(int a_) : a(a_) {
        std::cout << "构造 Base1(a=" << a << ")" << std::endl;
    }
    ~Base1() {
        std::cout << "析构 Base1" << std::endl;
    }
};

// 基类Base2
class Base2 {
protected:
    int b;
public:
    Base2(int b_) : b(b_) {
        std::cout << "构造 Base2(b=" << b << ")" << std::endl;
    }
    ~Base2() {
        std::cout << "析构 Base2" << std::endl;
    }
};

// 派生类Derived（多重继承Base1和Base2）
class Derived : public Base1, public Base2 {
private:
    int c;
public:
    Derived(int a_, int b_, int c_) : Base1(a_), Base2(b_), c(c_) {
        std::cout << "构造 Derived(c=" << c << ")" << std::endl;
    }
    ~Derived() {
        std::cout << "析构 Derived" << std::endl;
    }
};

int main() {
    // 关键：Windows下设置控制台UTF-8编码，解决中文乱码
    #ifdef _WIN32
    SetConsoleOutputCP(65001);
    #endif

    std::cout << "创建Derived对象(10, 20, 30):" << std::endl;
    Derived d(10, 20, 30);
    std::cout << "对象创建完成，程序即将退出:" << std::endl;
    
    // 暂停窗口，避免一闪而过
    std::cout << "\n按任意键退出程序..." << std::endl;
    system("pause");
    return 0;
}