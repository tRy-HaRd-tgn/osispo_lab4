#include <iostream>
#include <chrono>
#include <thread>
#include <mutex>
#include <condition_variable>
/*
Mutex обеспечивает, что только один поток может получить доступ
к общим данным в определенный момент времени, в то время как остальные
потоки ожидают разблокировки мьютекса. Это гарантирует правильное обновление
и чтение общих данных и исключает возможность конфликтов и непредсказуемого поведения программы.
*/
std::mutex mtx;
std::condition_variable cv;//используется для уведомления процесса
bool finish = false;
int countX = 0; //необходимо для учета количетсва выведенных событий
int countY = 0; //необходимо для учета количетсва выведенных событий

int MAINcount = 0;//счетчик от которого будет зависеть будет ли прекрешен процесс
void processA()//активирует случаное событие из двух
{
    std::unique_lock<std::mutex> lock(mtx);
    while (!finish && MAINcount < 100)
    {
        
        int randomNum = std::rand() % 2; // генерируем случайное число 0 или 1
        if (randomNum == 0)
        {
            std::cout << "A: Event X generated" << std::endl;
            countX++;
            MAINcount++;
        }
        else
        {
            std::cout << "A: Event Y generated" << std::endl;
            countY++;
            MAINcount++;
        }
        cv.notify_one();//если потоки ожидают то разблокирует ожидающий поток
        lock.unlock();
        std::this_thread::sleep_for(std::chrono::seconds(1));//оставляем процесс ожидать
        lock.lock();
    }

    if (MAINcount >= 100)
    {
        std::cout << "A: 100 events generated. Process B should stop." << std::endl;
        system("pause");
    
    }
    finish = true;
    cv.notify_one();//если потоки ожидают то разблокирует ожидающий поток
}

void processB()//отображает буквы этих событий
{
    std::unique_lock<std::mutex> lock(mtx);
    while (!finish && MAINcount < 100)
    {
        cv.wait(lock);
        if (countX > 0)
        {
            std::cout << "B: Event X detected" << std::endl;
            countX--;   //обнуляет значение чтобы не путаться
        }

        if (countY > 0)
        {
            std::cout << "B: Event Y detected" << std::endl;
            countY--;   //обнуляет значение чтобы не путаться
        }
    }

    if (MAINcount >= 100)
    {
        std::cout << "B: 100 events detected. Process A should stop." << std::endl;//процесс получил достаточно ивентов
        system("pause");
    }
}

int main()
{
    std::srand(static_cast<unsigned int>(std::time(nullptr)));//настоящий рандом

    std::thread tA(processA);
    std::thread tB(processB);

    //многопоточность
    tA.join();
    tB.join();

    return 0;
}