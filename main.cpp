#include <iostream>
#include <chrono>
#include <random>
#include <string>
#include <stdexcept>

#include "AVLTree.hpp"
#include "base_structures/dynamic_array/DynamicArray.hpp"

void benchmark() {
    std::cout << "\n=== Бенчмарк (время в миллисекундах) ===\n";
    std::cout << "N\tInsert\tSearch\tErase\n";

    int sizes[] = {10000, 100000, 1000000};
    int num_sizes = 3;

    for (int s = 0; s < num_sizes; ++s) {
        int N = sizes[s];
        AVLTree<int> tree;
        
        DynamicArray<int> data(N); 
        
        std::mt19937 rng(42);
        std::uniform_int_distribution<int> dist(0, N * 100);
        
        for (int i = 0; i < N; ++i) {
            data.set(i, dist(rng));
        }

        auto start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < N; ++i) tree.insert(data.get(i));
        auto end = std::chrono::high_resolution_clock::now();
        auto insert_time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

        start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < N; ++i) tree.contains(data.get(i));
        end = std::chrono::high_resolution_clock::now();
        auto search_time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

        start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < N; ++i) {
            try { tree.erase(data.get(i)); } catch(...) {}
        }
        end = std::chrono::high_resolution_clock::now();
        auto erase_time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

        std::cout << N << "\t" << insert_time << "\t" << search_time << "\t" << erase_time << "\n";
    }
}

AVLTree<int>::TraverseOrder choose_order() {
    int choice;
    std::cout << "Выберите обход:\n";
    std::cout << "1. KLP (Корень-Лево-Право)\n";
    std::cout << "2. KPL (Корень-Право-Лево)\n";
    std::cout << "3. LKP (Лево-Корень-Право) - по возрастанию\n";
    std::cout << "4. LPK (Лево-Право-Корень)\n";
    std::cout << "5. PKL (Право-Корень-Лево) - по убыванию\n";
    std::cout << "6. PLK (Право-Лево-Корень)\n";
    std::cout << "Ваш выбор (1-6): ";
    std::cin >> choice;

    switch (choice) {
        case 1: return AVLTree<int>::TraverseOrder::KLP;
        case 2: return AVLTree<int>::TraverseOrder::KPL;
        case 3: return AVLTree<int>::TraverseOrder::LKP;
        case 4: return AVLTree<int>::TraverseOrder::LPK;
        case 5: return AVLTree<int>::TraverseOrder::PKL;
        case 6: return AVLTree<int>::TraverseOrder::PLK;
        default: 
            std::cout << "Неверный выбор, используется KLP.\n";
            return AVLTree<int>::TraverseOrder::KLP;
    }
}

void print_menu() {
    std::cout << "\n--- AVL Tree Menu ---\n";
    std::cout << "1. Вставить число (вручную)\n";
    std::cout << "2. Удалить число\n";
    std::cout << "3. Найти число\n";
    std::cout << "4. Вывести дерево в строку\n";
    std::cout << "5. Показать размер\n";
    std::cout << "6. Сгенерировать дерево из N элементов\n";
    std::cout << "7. Извлечь поддерево\n";
    std::cout << "8. Бенчмарк (10^4, 10^5, 10^6)\n";
    std::cout << "9. Загрузить дерево из строки\n";
    std::cout << "0. Выход\n";
    std::cout << "Выбор: ";
}

int main() {
    AVLTree<int> tree;
    int choice;

    do {
        print_menu();
        if (!(std::cin >> choice)) break;

        if (choice == 1) {
            int val;
            std::cout << "Введите число: ";
            std::cin >> val;
            if (tree.insert(val)) std::cout << "Добавлено.\n";
            else std::cout << "Уже есть.\n";
        }
        else if (choice == 2) {
            int val;
            std::cout << "Введите число: ";
            std::cin >> val;
            try {
                tree.erase(val);
                std::cout << "Удалено.\n";
            } catch (const std::exception& e) {
                std::cout << "Ошибка: " << e.what() << "\n";
            }
        }
        else if (choice == 3) {
            int val;
            std::cout << "Введите число: ";
            std::cin >> val;
            std::cout << (tree.contains(val) ? "Найдено" : "Не найдено") << "\n";
        }
        else if (choice == 4) {
            auto order = choose_order();
            std::cout << "Дерево (строка): " << tree.serialize(order) << "\n";
            std::cout << "(Скопируйте её для пункта 9, если хотите загрузить обратно)\n";
        }
        else if (choice == 5) {
            std::cout << "Размер: " << tree.size() << "\n";
        }
        else if (choice == 6) {
            int N;
            std::cout << "Сколько элементов сгенерировать? ";
            std::cin >> N;
            
            DynamicArray<int> data(N); 
            
            std::mt19937 rng(42);
            std::uniform_int_distribution<int> dist(0, N * 10);
            
            for (int i = 0; i < N; ++i) {
                data.set(i, dist(rng));
            }
            for (int i = 0; i < N; ++i) {
                tree.insert(data.get(i));
            }
            std::cout << "Добавлено " << N << " элементов.\n";
        }
        else if (choice == 7) {
            int val;
            std::cout << "Введите корень поддерева: ";
            std::cin >> val;
            AVLTree<int> sub = tree.get_subtree(val);
            std::cout << "Поддерево: " << sub.serialize() << "\n";
        }
        else if (choice == 8) {
            benchmark();
        }
        else if (choice == 9) {
            std::string str;
            std::cout << "Введите строку (например, {5}({3}()[])[{7}()[]]): ";
            std::cin.ignore(); 
            std::getline(std::cin, str);
            try {
                tree.deserialize(str);
                std::cout << "Дерево успешно загружено!\n";
                std::cout << "Новый размер: " << tree.size() << "\n";
            } catch (const std::exception& e) {
                std::cout << "Ошибка десериализации: " << e.what() << "\n";
            }
        }
        else if (choice == 0) {
            std::cout << "Выход.\n";
        }
        else {
            std::cout << "Неверный выбор.\n";
        }

    } while (choice != 0);

    return 0;
}