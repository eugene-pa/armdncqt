#include <QCoreApplication>

#include <QVector>
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    using namespace std;

    // вектор строк и разные методы обхода
    string	s1 = "1234",
            s2 = "5678",
            s3 = "90!!";
    vector<string> v;
    v.push_back(s1);
    v.push_back(s2);
    v.push_back(s3);

    vector<string>::iterator iter = v.begin();

    for (; iter!=v.end(); ++iter)		// префиксный инкремент предпочтительнее с точки зрения быстродействия, но эквивалентен по смыслу
    {
        cout << *iter << "\r\n";
    }
    cout << "\r\n";

    for (unsigned int i=0; i<v.size(); i++)
    {
        cout << v.at(i) << "\r\n";
    }
    cout << "\r\n";

    // auto заменяет тии string, при этом, в отдичие от итератора, получаем значение, а не указатель!
    for (auto value : v)
    {
        cout << value << "\r\n";
    }
    cout << "\r\n";


    // словарь, способы обхода и поиска значений по ключу
    unordered_map <int, string> map;
    map[1] = s1;
    map[2] = s2;
    map[3] = s3;

    // обход с итератором
    // ключевое слово auto заменяет тип "unordered_map <int, string>::iterator"
    for (auto it = map.begin(); it != map.end(); ++it)
        cout << it->first << " = " << it->second << "\r\n";
    cout << "\r\n";

    // компактная форма обхода возвращает не указатель на пару PAIR, а значение
    for (auto val : map)
        cout << val.first << " - " << val.second << "\r\n";
    cout << "\r\n";

    if (map.count(3))						// проверка наличия элемента
        cout <<  map[3] << "\r\n";
    if (map.count(2))
        cout <<  map[2] << "\r\n";
    if (map.count(1))
        cout <<  map[1] << "\r\n";

    if (map.count(0))
        cout <<  map[0] << "\r\n";


    return a.exec();
}
