#include <QCoreApplication>
#include <functional>
#include <iostream>

class Person
{
public:
    Person(std::string _name) { name = _name; }
    std::string name;
};

class Foo
{
public:
    Foo(int num) : num_(num) {}
    void print_add(int i) const { std::cout << num_ + i << '\n'; }
    void print_add2(Foo * f) const { std::cout << f->num_ << '\n'; }
    void print_add3(Person * p) const { std::cout << p->name.c_str() << '\n'; }
    int num_;
};

void print_num(int i)
{
    std::cout << i << '\n';
}

void print_str(std::string s)
{
    std::cout << s.c_str() << '\n';
}


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    // store a free function
    std::function<void(int)> f_display = print_num;
    f_display(-9);

    // store a lambda
    std::function<void()> f_display_42 = []() { print_num(42); };
    f_display_42();

    // store the result of a call to std::bind
    std::function<void()> f_display_31337 = std::bind(print_num, 31337);
    f_display_31337();

    // store a call to a member function
    std::function<void(const Foo&, int)> f_add_display = &Foo::print_add;
    Foo foo(314159);
    f_add_display(foo, 1);

    std::function<void(const Foo&, Foo *)> f_add_display2 = &Foo::print_add2;
    Foo foo2(100);
    f_add_display2(foo, &foo2);

    std::function<void(const Foo&, Person *)> f_add_display3 = &Foo::print_add3;
    Person p("John");
    f_add_display3(foo, &p);

    std::function<void(std::string)> f_str;
    f_str = print_str;
    f_str("STRING!");
    return a.exec();
}
