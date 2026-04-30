
#include <iostream>
#include <fstream> // Б-ка для роботи з файлами
#include <sstream> // Б-ка для розбиття завдань на окремі частинки
#include <vector>  // Б-ка для динамічних масивів
#include <string>  // Б-ка для рядкових даних
#include <algorithm>  // Б-ка для сортування завдань
#include <windows.h>
#include <iomanip>

using namespace std;

// Кольори
const string RED = "\033[31m";
const string GREEN = "\033[32m";
const string YELLOW = "\033[33m";
const string RESET = "\033[0m";

// Структура 
struct Task {
    int num = 0;       // Номер завдання (замість id)
    string name = "";  // Назва
    int hard = 0;      // Складність
    int type = 1;      // 1 - Ментальне, 2 - Фізичне
    double time = 0.0; // Час на виконання
    int hours = 0;     // Годин до дедлайну
    bool done = false; // Виконано чи ні   -    false (не виконано) або true (виконано)
    double prior = 0.0;// Пріоритет
};

void makeTestFile(const string& f_name) {
    ofstream f(f_name); // створення файлу
    if (f.is_open()) { // якщо створився записуємо початкові дані або для прикладу..
        f << "НМТ Математика: Вектори;8;1;2.5;14;0\n";
        f << "Прибирання в кімнаті;3;2;1.0;72;0\n";
        f << "ЛР з С++;7;1;1.5;5;1\n";
        f << "Тренажер Синтаксису;2;1;0.25;29;0\n";
        f.close(); // закрити після запису
    }
}
// Ф-я для зчитування завдань з файлу
vector<Task> readTasks(const string& f_name) {
    vector<Task> arr;
    ifstream f(f_name); // відкриваємо файл

    if (!f.good()) { // якщо файлу нема
        makeTestFile(f_name); // ств. його за доп. ф-ї створення
        f.clear();  // очищаємо помилки..
        f.open(f_name);// і відкриваємо цей ств. файл
    }

    string line; // для рядка файлу (1 завдання)
    int cur_num = 1; // змінна для кожного завдання
    while (getline(f, line)) {   // цикл читає завдання поки не дійде до останнього завдання
        if (line.empty()) continue;  // пропуск порожнього рядка

        stringstream ss(line); // це як для рзучності розбиття рядка на окремі частинки
        string temp; // для частинки тексту
        Task t; // ств. пустого завдання, щоб призначити йому індивідуальну змінну
        t.num = cur_num++;

        // розбиваємо дані рядка на окремі частинки (розділені => ;)
        getline(ss, t.name, ';');
        getline(ss, temp, ';'); t.hard = stoi(temp);
        getline(ss, temp, ';'); t.type = stoi(temp);
        getline(ss, temp, ';'); t.time = stod(temp);
        getline(ss, temp, ';'); t.hours = stoi(temp);
        getline(ss, temp, ';'); t.done = stoi(temp);

        t.prior = 0.0;
        arr.push_back(t); // додаємо завдання у масив
    }
    return arr;
}

// Ф-я для збереження змін у файл
void saveToFile(const string& f_name, const vector<Task>& arr) {
    ofstream f(f_name); // відкриваємо файл
    for (const auto& t : arr) { // цикл для всі типів даних завдань
        f << t.name << ";" << t.hard << ";" << t.type << ";"
            << t.time << ";" << t.hours << ";" << t.done << "\n";
    }
}


//                               ГОЛОВНА ФУНКЦІЯ
int main() {
    SetConsoleOutputCP(1251);

    string file_name = "tasks.txt"; // наш файл для роботи з завданнями
    vector<Task> my_tasks = readTasks(file_name);  // передаємо завдання у змінну my_tasks

    int en_mind = 100;  // ментальне самопочуття
    int en_body = 100;  // фізичне самопочуття
    int menu_choice; // змінна вибору користувача

    do {  //                        МЕНЮ
        cout << "\n=========================================================\n";
        cout << "                 АДАПТИВНИЙ ПЛАНУВАЛЬНИК\n";
        cout << "=========================================================\n";
        cout << "[1] Показати завдання та аналіз на завтра\n";
        cout << "[2] Додати нове завдання\n";
        cout << "[3] Відмітити завдання як виконане\n";
        cout << "[0] Зберегти та вийти\n";
        cout << "=========================================================\n";
        cout << "Ваш вибір: ";
        cin >> menu_choice;

        switch (menu_choice) {
        case 1: {
            cout << "\n[ВВІД] Ваша ментальна енергія (0-100): ";
            cin >> en_mind;
            cout << "[ВВІД] Ваша фізична енергія (0-100): ";
            cin >> en_body;
            cout << "\nАналізую ситуацію...\n";
            Sleep(800);  // це як штучна пауза

            // Розрахунок пріоритетів
            for (auto& t : my_tasks) {  // цикл по завданнях
                if (t.done) {  // якщо виконане..
                    t.prior = -1.0; // буде найнижчий пріоритет
                    continue; // перехід до наступного завдання
                }
                t.prior = (t.hard * 100.0) / (t.hours > 0 ? t.hours : 1); // формула пріоритету: складність * 100 / дедлайн..
                
                // Якщо енергії менше, ніж вимагає завдання — пріоритет падає
                if (t.type == 1 && en_mind < t.hard * 10) t.prior *= 0.3;
                if (t.type == 2 && en_body < t.hard * 10) t.prior *= 0.3;

                // підвищення пріоритету, якщо дедлайн <= 24 години
                if (t.hours <= 24) t.prior *= 5.0;
            }

            // сортування за пріоритетом
            sort(my_tasks.begin(), my_tasks.end(), [](const Task& a, const Task& b) {
                return a.prior > b.prior;
                });

            cout << "\n---------------------- ВАШІ ЗАВДАННЯ --------------------\n\n";
            double work_today = 0.0; // години роботи на завтра
            int left_tasks = 0; // кількість завдань, що лишилися

            for (const auto& t : my_tasks) {
                cout << "[" << t.num << "] "; // номер завдання
                if (t.done) { // якщо зроблене, то фарбуються у зелений
                    cout << GREEN << "[V] ЗРОБЛЕНО: " << t.name << RESET << " (Час: " << t.time << " год)\n\n";
                }
                else {
                    left_tasks++; // збільшення невиконаних..
                    if (t.hours <= 48) { // якщо дедлайн <= 48, то переносимо час на завтра
                        work_today += t.time;
                    }
                    else { // якщо ще достатньо часу, то час / дні
                        work_today += (t.time / (t.hours / 24.0));
                    }

                    cout << t.name << "\n";
                    cout << "    Складність: ";
                    if (t.hard >= 7) cout << RED;          // розклала рівень складності по кольорам
                    else if (t.hard >= 4) cout << YELLOW;
                    else cout << GREEN;
                    cout << t.hard << "/10" << RESET; // після виводу колір скидається

                    cout << " | Сили: " << (t.type == 1 ? "Ментальні" : "Фізичні") << " | Час: " << t.time << " год\n";

                    // повідомлення попереджень про дедлайн..
                    if (t.hours <= 24) {
                        cout << "    [!] " << RED << "ГОРИТЬ! ДЕДЛАЙН: " << t.hours << " годин!" << RESET << "\n";
                    }
                    else if (t.hours <= 72) {
                        cout << "    [~] " << YELLOW << "Скоро здавати: " << t.hours << " год" << RESET << "\n";
                    }
                    else {
                        cout << "    [ ] Часу ще багато: " << (t.hours / 24) << " дн.\n";
                    }
                    cout << "\n";
                }
            }

            const double MAX_H = 8.0;  // загальна норма годин для роботи
            int load_perc = (work_today / MAX_H) * 100.0;  // розрахунок навантаження

            cout << "------------- АНАЛІЗ НАВАНТАЖЕННЯ НА ЗАВТРА -------------\n";
            cout << "Залишилось зробити: " << left_tasks << " завдань\n";
            cout << "Орієнтовний час роботи: ~" << fixed << setprecision(1) << work_today << " год\n\n";


            cout << "Шкала втоми (Норма " << MAX_H << " год):\n[";
            int blocks = min(20, (load_perc * 20) / 100); // розрахунок відсотка навантаження
            for (int i = 0; i < 20; i++) {
                if (i < blocks) {  // розклад за кольорами
                    if (load_perc > 100) cout << RED << "#" << RESET;
                    else if (load_perc > 70) cout << YELLOW << "#" << RESET;
                    else cout << GREEN << "#" << RESET;
                }
                else cout << "-";  // для порожнього місця
            }
            cout << "] " << load_perc << "%\n\n";


            // поради / висновки
            if (load_perc > 100) {
                cout << RED << "[УВАГА] ПЕРЕВАНТАЖЕННЯ\n" << RESET << "Ти не встигнеш все. Роби головне!\n";
            }
            else if (load_perc > 70) {
                cout << YELLOW << "[!] ЩІЛЬНИЙ ГРАФІК\n" << RESET << "Буде важко, але реально.\n";
            }
            else {
                cout << GREEN << "[OK] ВСЕ СУПЕР\n" << RESET << "Можна і попрацювати, і відпочити.\n";
            }
            break;
        }
        case 2: {
            Task new_t; // порожнє нове завдання
            // Знаходимо максимальний номер, щоб нове завдання мало наступний
            int max_num = 0; // максимальний номер для нового завдання
            for (auto& t : my_tasks) {
                if (t.num > max_num) max_num = t.num; // Змінено з id
            }
            new_t.num = max_num + 1; // ось і новий номер..

            cout << "\n---------- ДОДАВАННЯ ЗАВДАННЯ ----------\n";
            cout << "Назва: ";
            cin.ignore(); // Очищаємо буфер перед вводом рядка
            getline(cin, new_t.name); // зчитування рядка
             
            // окремі дані
            cout << "Складність (1-10): ";
            cin >> new_t.hard;

            cout << "Тип (1 - Розумове, 2 - Фізичне): ";
            cin >> new_t.type;

            cout << "Скільки годин займе (напр. 1.5): ";
            cin >> new_t.time;

            cout << "Через скільки годин дедлайн: ";
            cin >> new_t.hours;

            new_t.done = false; // це нове завдання ще точно не виконане..
            new_t.prior = 0.0; // пріоритету ще немає

            my_tasks.push_back(new_t); // додавання завдання в кінець списку
            saveToFile(file_name, my_tasks); // збереження у файлі
            cout << GREEN << "\nЗавдання успішно додано!\n" << RESET;
            break;
        }
        case 3: { // позначити як зроблене
            cout << "\nВведіть номер завдання [...], яке виконали: ";
            int task_num; // номер завдання
            cin >> task_num; 
            bool is_found = false; // чи є такий номер

            for (auto& t : my_tasks) { // пошук потрібного завдання
                if (t.num == task_num) { 
                    t.done = true; // виконано
                    is_found = true; // зайдено
                    cout << GREEN << "\nМолодець! Завдання '" << t.name << "' виконано!\n" << RESET;
                    break;
                }
            }
            if (!is_found) cout << RED << "Немає такого номера.\n" << RESET;
            else saveToFile(file_name, my_tasks);
            break;
        }
        case 0: // вихід з програми
            cout << "\nЗберігаю дані... До побачення!\n";
            saveToFile(file_name, my_tasks);
            break;
        default: // якщо введено зовсін не ту цифру...
            cout << RED << "\nНевідома команда. Спробуй ще раз.\n" << RESET;
            break;
        }
    } while (menu_choice != 0); // повтор циклу, поки не будо введено 0

    return 0;
}
