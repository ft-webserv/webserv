#include <iostream>
#include <map>
#include <string>

int main() {
    std::map<std::string, int> myMap;

    // 키-값 쌍 추가
    myMap["banana"] = 2;
    myMap["cherry"] = 8;
    myMap["date"] = 3;
    myMap["apple"] = 5;

    // 맵 순회하며 출력
    for (const auto& pair : myMap) {
        std::cout << pair.first << ": " << pair.second << std::endl;
    }

    return 0;
}
