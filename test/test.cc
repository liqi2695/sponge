#include <iostream>
#include <string>
#include <map>

using namespace std;



int main() {
    std::string str{"2sdfsfs"};
    std::string str1 = str.substr(2);
    std::map<int, int> mp{
        {1,2},
        {2,3},
        {3,4}
    };
    auto it = mp.lower_bound(2);
    cout << it->first << endl;
    cout << str1 << endl;
    return 0;
}