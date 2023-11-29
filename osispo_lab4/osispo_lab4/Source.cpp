#include <iostream>
#include <chrono>
#include <thread>
#include <mutex>
#include <condition_variable>
/*
Mutex ������������, ��� ������ ���� ����� ����� �������� ������
� ����� ������ � ������������ ������ �������, � �� ����� ��� ���������
������ ������� ������������� ��������. ��� ����������� ���������� ����������
� ������ ����� ������ � ��������� ����������� ���������� � ���������������� ��������� ���������.
*/
std::mutex mtx;
std::condition_variable cv;//������������ ��� ����������� ��������
bool finish = false;
int countX = 0; //���������� ��� ����� ���������� ���������� �������
int countY = 0; //���������� ��� ����� ���������� ���������� �������

int MAINcount = 0;//������� �� �������� ����� �������� ����� �� ��������� �������
void processA()//���������� �������� ������� �� ����
{
    std::unique_lock<std::mutex> lock(mtx);
    while (!finish && MAINcount < 100)
    {
        
        int randomNum = std::rand() % 2; // ���������� ��������� ����� 0 ��� 1
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
        cv.notify_one();//���� ������ ������� �� ������������ ��������� �����
        lock.unlock();
        std::this_thread::sleep_for(std::chrono::seconds(1));//��������� ������� �������
        lock.lock();
    }

    if (MAINcount >= 100)
    {
        std::cout << "A: 100 events generated. Process B should stop." << std::endl;
        system("pause");
    
    }
    finish = true;
    cv.notify_one();//���� ������ ������� �� ������������ ��������� �����
}

void processB()//���������� ����� ���� �������
{
    std::unique_lock<std::mutex> lock(mtx);
    while (!finish && MAINcount < 100)
    {
        cv.wait(lock);
        if (countX > 0)
        {
            std::cout << "B: Event X detected" << std::endl;
            countX--;   //�������� �������� ����� �� ��������
        }

        if (countY > 0)
        {
            std::cout << "B: Event Y detected" << std::endl;
            countY--;   //�������� �������� ����� �� ��������
        }
    }

    if (MAINcount >= 100)
    {
        std::cout << "B: 100 events detected. Process A should stop." << std::endl;//������� ������� ���������� �������
        system("pause");
    }
}

int main()
{
    std::srand(static_cast<unsigned int>(std::time(nullptr)));//��������� ������

    std::thread tA(processA);
    std::thread tB(processB);

    //���������������
    tA.join();
    tB.join();

    return 0;
}