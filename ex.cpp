#include <iostream>

int main()
{
    std::shared_ptr<int> b {new int(10)};
    std::shared_ptr<int> a = std::make_shared<int>(b);

    std::cout << b << ' ' << b.get() << ' ' << a.get() << ' ' << '\n';
}