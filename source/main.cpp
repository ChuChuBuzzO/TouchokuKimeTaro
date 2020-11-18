#include "DoctorScheduling.h"
#include "GA.h"
#include <QtWidgets/QApplication>
#include "funcs.h"

double evaluate_generation(const std::vector<int>& vec)
{
	return std::accumulate(vec.begin(), vec.end(), 0.0);
}



int main(int argc, char *argv[])
{
    // GAをonemax問題で実験 →　オッケー
 //   auto ga = GABit1d(300, 50);
	//
	//// 100世代回してみる
 //   int gens_num = 200;
	//double cross_rate = 0.92;
	//double mutation_rate = 0.05;
	//ga.set_individuals_random();
	//ga.set_cross_rate(cross_rate);
	//ga.set_mutation_rate(mutation_rate);
	//ga.set_elite_num(3);
	//
	//for (int i = 0; i < gens_num; i++)
	//{
	//	std::cout << "-------" << i << " generation -------" << std::endl;
	//	ga.process_generation(evaluate_generation);

	//	// 評価値の最も良いものを表示
	//	auto res = ga.get_best_individuals(2);
	//	std::cout << "best one" << std::endl;
	//	std::cout << res.first[0] << std::endl;
	//	std::cout << res.second[0] << std::endl;
	//}
	//
	//int d1 = GetDays(2020, 10, 30);
	//int d2 = GetDays(2020, 11, 3);

	//std::cout << d2 - d1 << std::endl;


    QApplication a(argc, argv);
    DoctorScheduling w;
	QIcon icon("./touchoku.ico");
	w.setWindowIcon(icon);	
    w.show();
    return a.exec();
}

