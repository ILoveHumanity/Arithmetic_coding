// Arithmetic_coding.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
#include <iostream>
#include <Windows.h>
#include <fstream>
#include <vector>
#include <string>

bool check_fequal(std::string a, std::string b) { // проверка на совпадение файлов
    std::ifstream fin_a(a);       // открываем файл на чтение
    std::ifstream fin_b(b);       // открываем файл на чтение
    char c_a, c_b;
    while (fin_a.get(c_a)) {
        if (fin_b.get(c_b)) {
            if (c_a != c_b) {
                fin_a.close();
                fin_b.close();
                std::cout << "Файлы не одинаковые"; // соответствующие символы не совпадают
                return false;
            }
        }
        else {
            fin_a.close();
            fin_b.close();
            std::cout << "Файлы не одинаковые"; // второй содержит меньше символов
            return false;
        }
    }
    if (fin_b.get(c_b)) {
        fin_a.close();
        fin_b.close();
        std::cout << "Файлы не одинаковые"; // первый содержит меньше символов
        return false;
    }
    fin_a.close();
    fin_b.close();
    std::cout << "Файлы одинаковые"; // все совпало
    return true;
}

int main()
{
    SetConsoleCP(1251); // устанавливаем кодировку для ввода\вывода на консоль
    SetConsoleOutputCP(1251);
    int flag;
    std::cout << "Если вы хотите зашифровать файл введите 1 если расшифровать 0: ";
    std::cin >> flag;
    if (flag) {
        
    }
    else {

    }
}