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
        std::string file_link; 
        std::cout << "Пожалуйста введите путь до файла: ";
        std::cin >> file_link;
        std::vector<int> IndexForSymbol(256); // таблица под значения кодов для символов, изначально используется для подсчета встреченых символов для экономии памяти
        std::ifstream fin(file_link);       // открываем файл на чтение, считаем встреченые символы, закрываем
        char c;
        while (fin.get(c)) {
            IndexForSymbol[(byte)c] += 1;
        }
        fin.close();

        std::vector<unsigned int> frequency; // массивы под количество встреченых символов и их значение (содержат только встереченные, порядок элементов совпадает, отсортированы по убыванию количества встреченых раз) 
        std::vector<char> signs;

        for (int i = 0; i < (int)IndexForSymbol.size(); ++i) { // цикл по всем возможным символам
            if (IndexForSymbol[i]) { // если символ встречался в тексте
                frequency.push_back(0); // добовляем под него место в массивах
                signs.push_back(0);
                int j = (int)frequency.size() - 1; // вставляем его на свое место с сохранением убывания вероятности, и соответствия между двумя массивами  
                while (j > 0 && IndexForSymbol[i] > frequency[j - 1]) {
                    frequency[j] = frequency[j - 1];
                    signs[j] = signs[j - 1];
                    --j;
                }
                frequency[j] = IndexForSymbol[i];
                signs[j] = i;
            }
        }
        std::vector<unsigned int> b( (int)signs.size()+1 );
        b[0] = 0;
        for (long long i = 0, buf = 0; i < (int)signs.size(); ++i) { // цикл по встреченым символам
            IndexForSymbol[(byte)signs[i]] = i+1;
            buf += frequency[i];
            b[i+1] = buf;
        }

        std::string encrypted_file_link = file_link; // создаем путь к файлу с зашифрованным текстом
        encrypted_file_link.replace((int)encrypted_file_link.size() - 4, (int)encrypted_file_link.size(), "_encrypted_ar.bin");
        std::ofstream fout(encrypted_file_link, std::ios_base::binary); // открываем файл с зашифрованным текстом на запись
        fin.open(file_link);       // открываем файл с открытым текстом на чтение
        int size_signs = signs.size();
        // Записываем таблицу со значениями и кодми в файл
        fout.write((char*)&size_signs, sizeof(int));
        for (int i = 0; i < size_signs; i++) {
            fout.write((char*)&(signs[i]), sizeof(char));
            fout.write((char*)&(b[i+1]), sizeof(unsigned int));
        }
        // Записываем текст в файл
        unsigned int l = 0, h = 65535, l_prev, h_prev, delitel = b[(int)b.size() - 1];
        unsigned int First_qtr = (h + l) / 4; // 16384
        unsigned int Half = First_qtr * 2; // 32768
        unsigned int Third_qtr = First_qtr * 3;// 49152
        unsigned int bits_to_follow = 0; // Сколько битов сбрасывать
        char byte_c = 0; unsigned int byte_size = 0;
        while (fin.get(c)) { //
            int j = IndexForSymbol[c];
            l_prev = l;
            h_prev = h;
            l = l_prev + b[j-1] * (h_prev - l_prev + 1) / delitel;
            h = l_prev + b[j] * (h_prev - l_prev + 1) / delitel - 1;
            for (;;) {
                if (h < Half) {
                    for (;bits_to_follow > 0; bits_to_follow--) {
                        byte_c = byte_c << 1;
                        byte_size++;
                        if (byte_size == 8) {
                            fout.write((char*)&byte_c, 1);
                            byte_c = 0;
                            byte_size = 0;
                        }
                    }
                }
                else if (l >= Half) {
                    for (; bits_to_follow > 0; bits_to_follow--) {
                        byte_c = byte_c << 1 | 1;
                        byte_size++;
                        if (byte_size == 8) {
                            fout.write((char*)&byte_c, 1);
                            byte_c = 0;
                            byte_size = 0;
                        }
                    }
                    l -= Half;
                    h -= Half;
                }
                else if (l >= First_qtr && h < Third_qtr) {
                    bits_to_follow++;
                    l -= First_qtr;
                    h -= First_qtr;
                }
                else break;
                l += l;
                h += h + 1;
            }
        }
        if (byte_size) { // обработка последнего кода
            byte_c = byte_c << (8 - byte_size);
            fout.write((char*)&byte_c, 1);
        }
        fin.close();
        fout.close(); // закрываем файлы
    }

    else {
        std::string encrypted_file_link;
        std::cout << "Пожалуйста введите путь до файла: ";
        std::cin >> encrypted_file_link;
        std::ifstream fin(encrypted_file_link, std::ios_base::binary); // открываем файл с зашифрованным текстом на чтение
        
        int size_signs;
        fin.read((char*)&size_signs, sizeof(int));
        std::vector<char> signs(size_signs);
        std::vector<unsigned int> b(size_signs + 1);
        b[0] = 0;
        for (int i = 0; i < size_signs; i++) {
            fin.read((char*)&(signs[i]), sizeof(char));
            fin.read((char*)&(b[i + 1]), sizeof(unsigned int));
        }


    }
}