#include "vector.hh"
#include<iostream>
#include<vector>
#include<chrono>
#include<random>
#include<fstream>

int main()
{
	std::srand(time(NULL));
	std::ofstream file("push_back.txt");


	file<<"Czas push_back mlab; Czas push_back std\n";


	for(int i = 100; i <= 100000; i += 100)
	{
		mlab::vector<int> vecmlab;
		std::vector<int> vecstd;

		std::cout<<"xd"<<std::endl;
		std::chrono::high_resolution_clock::time_point time1 = std::chrono::high_resolution_clock::now();

		for(int j = 0; j < i; j++)
			vecmlab.push_back(std::rand() % 100000);

		std::chrono::high_resolution_clock::time_point time2 = std::chrono::high_resolution_clock::now();

		std::chrono::duration<double, std::milli> duration = time2 - time1;

		file<<duration.count()<<", ";

		time1 = std::chrono::high_resolution_clock::now();

		for(int j = 0; j < i; j++)
			vecstd.push_back(std::rand() % 100000);

		time2 = std::chrono::high_resolution_clock::now();

		duration = time2 - time1;

		file<<duration.count()<<"\n";

		std::cout<<i<<std::endl;
	}

	file.close();

	return 0;
}
