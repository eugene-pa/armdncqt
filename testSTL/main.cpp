#include <QCoreApplication>

#include <QVector>
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>

#include <sstream>
#include <iomanip>
//#include <ctime>
//#include <cstdlib>
//#include <locale>



int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    using namespace std;

    string	s1 = "one",
            s2 = "two",
            s3 = "three";

    // vector ================================================================================================================================================
    // вектор строк и разные методы обхода
    vector<string> v;
    v.push_back(s1);
    v.push_back(s2);
    v.push_back(s3);

#if _MSC_VER >= 1800
    // 1. Обход с использованием средств С++ 11
    // auto заменяет тии string, при этом, в отличие от итератора, получаем значение, а не указатель!
    for (auto value : v)
    {
        cout << value << "\r\n";
    }
    cout << "\r\n";
#endif

    // 2. обход с использованием итератора
    vector<string>::iterator iter = v.begin();
    for (; iter!=v.end(); ++iter)		// префиксный инкремент предпочтительнее с точки зрения быстродействия, но эквивалентен по смыслу
    {
        cout << *iter << "\r\n";
    }
    cout << "\r\n";

    // 3. обход с использованием прямой индексации
    for (unsigned int i=0; i<v.size(); i++)
    {
        cout << v.at(i) << "\r\n";
    }
    cout << "\r\n";


    // unordered_map ============================================================================================================================================
    // несортированный ассоциативный контейнер; оптимален с точки зрени быстродействия
    unordered_map <int, string> map;
    map[1] = s1;
    map[2] = s2;
    map[3] = s3;

    // 1. обход с использованием с++11; возвращает не указатель на пару PAIR, а значение
    // оптимизированный обход выполняется с использованием константной ссылки,
    // если использовать обычный синтаксис:  for (auto val : map)  - создается лишняя локальная копия
    for (const auto& val : map)
        cout << val.first << " - " << val.second << "\r\n";
    cout << "\r\n";

#if _MSC_VER >= 1800
    // 2. обход и модификация с использованием с++11;
    for (auto& val : map)
        val.second += " modified";
    cout << "\r\n";

    // 3. обход с итератором
    // ключевое слово auto заменяет тип "unordered_map <int, string>::iterator"
    for (auto it = map.begin(); it != map.end(); ++it)
        cout << it->first << " = " << it->second << "\r\n";
    cout << "\r\n";
#endif

    // проверка наличия элемента по ключу - вызовом функции count - подсчет числа записей с данным ключем (ключ уникальный, поэтому count()==1 или 0)
    if (map.count(3))
        cout <<  map[3] << "\r\n";
    if (map.count(2))
        cout <<  map[2] << "\r\n";
    if (map.count(1))
        cout <<  map[1] << "\r\n";

    if (map.count(0))
        cout <<  map[0] << "\r\n";


    // работа со временем ============================================================================================================================================
    std::tm t;
    std::istringstream ss("2011.02.18 23:12:34");

    ss >> std::get_time(&t, "%Y.%m.%d %H:%M:%S");
    if (ss.fail()) {
        std::cout << "Parse failed\n";
    }
    else {
        std::cout << std::put_time(&t, "%c") << '\n';
    }


    return a.exec();
}
