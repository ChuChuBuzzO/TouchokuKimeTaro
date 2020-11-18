#include "GA.h"


void GABit1d::two_point_cross(std::vector<std::vector<int>>& vec, const int& parent1, const int& parent2)  // この中にはmutate_rateの要素が入ってないことに注意
{
	// 切り分けるポイント選ぶ。0→0と1の間。
	int firstp = rnd(chrom_length);
	int secondp = rnd(chrom_length);

	while (firstp == secondp)
	{
		//　万が一一緒なら選び直す
		secondp = rnd(chrom_length);
	}

	// 順番に並べる
	if (firstp > secondp)
	{
		std::swap(firstp, secondp);
	}

	// 真ん中部分を交換
	for (int i = firstp; i <= secondp; i++)
	{
		std::iter_swap(vec[parent1].begin() + i, vec[parent2].begin() + i);
	}
}

void GABit1d::mutate(const int& index)  // indexは染色体番号
{
	// すべての遺伝子につきmutation_rateに基づいて変異を加える
	for (int i = 0; i < chrom_length; i++)
	{
		if (rnd.randBool(mutation_rate))
		{
			individuals[index][i] = 1 - individuals[index][i];
		}
	}
}


std::vector<std::vector<int>> GABit1d::roulette_select(std::vector<std::vector<int>> vec_individuals, std::vector<double> vec_points, int constraint_num)
{
	// どうせ適応度高いの生き残ってくる確率高いので、elite選択なしで
	std::vector<std::vector<int>> next_generation_partial;  // 次の世代になるはずのvector

	double sum = std::accumulate(vec_points.begin(), vec_points.end(), 0.0);  // 評価値の合計

	//蓄積値でルーレット
	int ind_num = vec_individuals.size();

	if (constraint_num == 0)
	{
		constraint_num = ind_num;
	}

	std::random_device rd;
	std::mt19937 mt(rd());

	// 制限付きversionの実装
	std::vector<int> constraints(ind_num, 0);

	for (int i = 0; i < ind_num; i++)
	{
		//std::cout << vec_individuals.size() << std::endl;
		std::uniform_real_distribution<double> score(0.0, sum);
		double sc = score(mt);
		double cn = 0;

		for (int j = 0, s = vec_points.size(); j < s; j++)
		{
			cn += vec_points[j];
			if (cn >= sc)
			{
				next_generation_partial.emplace_back(vec_individuals[j]);
				constraints[j] += 1;
				if (constraints[j] > constraint_num)
				{
					sum -= vec_points[j];
					vec_individuals.erase(vec_individuals.begin() + j);
					vec_points.erase(vec_points.begin() + j);
					constraints.erase(constraints.begin() + j);
				}
				break;
			}
		}
	}
	
	// 世代を更新
	return next_generation_partial;
}

void GABit1d::process_generation(const std::function<double(std::vector<int>)>& f, const int& constraint_num)
{
	if (std::all_of(individuals_points.begin(), individuals_points.end(), [](int x) { return x == 0;}))
	{
		evaluate(f);
	}
	
	std::vector<std::vector<int>> next_generation;

	// n個のエリートをまず分離→エリートは交叉も突然変異の対象にもならない
	std::vector<std::vector<int>> elites;
	std::vector<double> elites_points;
	
	std::vector<std::vector<int>> non_elites;
	std::vector<double> non_elites_points;

	// 評価値を高い方から順にn個のindexを調べる(argsortで)。
	std::vector<int> high_evalpoints_ind;
	std::vector<size_t> indices(individual_num);
	std::iota(indices.begin(), indices.end(), 0);
	std::sort(indices.begin(), indices.end(), [&](size_t i1, size_t i2) 
	{
		return individuals_points[i1] > individuals_points[i2];
	});
	for (int i = 0; i < elite_num; i++)
	{
		high_evalpoints_ind.emplace_back(indices[i]);
	}
	std::vector<size_t>().swap(indices);

	for (int i = 0; i < individual_num; i++)
	{
		if(std::find(high_evalpoints_ind.begin(), high_evalpoints_ind.end(), i) != high_evalpoints_ind.end())
		{
			elites.emplace_back(individuals[i]);
			elites_points.emplace_back(individuals_points[i]);
		} else
		{
			non_elites.emplace_back(individuals[i]);
			non_elites_points.emplace_back(individuals_points[i]);
		}
	}

	// roulette選択
	auto selected_non_elites = roulette_select(non_elites, non_elites_points, constraint_num);

	// 若い番号から2個ずつ選んで交差させる
	for (int i = 0, s = selected_non_elites.size() / 2; i < s; i++)
	{
		if (rnd.randBool(cross_rate))
		{
			two_point_cross(selected_non_elites, 2 * i, 2 * i + 1);
		}
	}
	// 突然変異させる
	for (int i = 0, s = selected_non_elites.size(); i < s; i++)
	{
		mutate(i);
	}

	// eliteとnon_eliteをくっつける。そして評価する。
	elites.insert(elites.end(), selected_non_elites.begin(), selected_non_elites.end());
	individuals = elites;
	
	evaluate(f);	
};

void GABit1d::process_greedy_generation(const std::function<double(std::vector<int>)>& f)
{
	if (std::all_of(individuals_points.begin(), individuals_points.end(), [](int x) { return x == 0; }))
	{
		evaluate(f);
	}
	
}
