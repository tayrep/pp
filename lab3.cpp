#include <iostream>
#include <future>
#include <thread>
#include <cstdlib>

const int ROWS{ 10 };
const int COLS{ 11 };
const int COUNT{ ROWS * COLS };
const int NTHREAD{ 4 };

int count_not_multiple(const int* arr, int size, int divisor)
{
    int result{ 0 };
    for (int i = 0; i < size; ++i)
        if (arr[i] % divisor != 0)
            ++result;
    return result;
}

int count_parallel(const int* arr, int divisor)
{
    std::future<int> ftr[NTHREAD - 1]{};
    const int chunk{ COUNT / NTHREAD };

    for (int i = 0; i < NTHREAD - 1; ++i)
        ftr[i] = std::async(std::launch::async, count_not_multiple,
                            arr + chunk * i, chunk, divisor);

    int total{ count_not_multiple(arr + chunk * (NTHREAD - 1),
                                  chunk + COUNT % NTHREAD, divisor) };

    for (int i = 0; i < NTHREAD - 1; ++i)
        total += ftr[i].get();

    return total;
}

void init(int* arr)
{
    for (int i = 0; i < COUNT; ++i)
        arr[i] = std::rand() % 201 - 100;
}

void print_matrix(const int* arr)
{
    for (int r = 0; r < ROWS; ++r)
    {
        for (int c = 0; c < COLS; ++c)
            std::cout << arr[r * COLS + c] << '\t';
        std::cout << '\n';
    }
}

int main()
{
    int arr[COUNT]{};
    init(arr);

    int divisor = 4;

    std::cout << "main thread ID: " << std::this_thread::get_id() << '\n';
    std::cout << "matrix " << ROWS << " x " << COLS << ":\n";
    print_matrix(arr);

    std::cout << "seq: "
              << count_not_multiple(arr, COUNT, divisor) << '\n';
    std::cout << "parallel: "
              << count_parallel(arr, divisor) << '\n';
}
