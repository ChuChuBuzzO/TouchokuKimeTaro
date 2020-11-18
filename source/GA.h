#pragma once

#include <iostream>
#include <vector>
#include "random_custom.h"
#include <functional>
#include <numeric>

#include "funcs.h"

// 1世代ごとにevaluateする必要があるので忘れずに
class GABit1d
{
private:
	int individual_num;  // 1世代あたりの個体数
	int chrom_length;  // 染色体の長さ
	// 実装上、エリートとそうでない分を分けておく方が便利
	std::vector<std::vector<int>> individuals;  // 個体群、つまり1世代 0 or 1のビット
	std::vector<double> individuals_points;  // 評価値のvector
	
public:
	double cross_rate = 0.0;
	double mutation_rate = 0.0;
	int elite_num = 0;  // eliteの数
	
	GABit1d(int ind_n, int ind_c)
	{
		individual_num = ind_n;
		chrom_length = ind_c;

		for (int i = 0; i < individual_num; i++)
		{
			individuals_points.emplace_back(0.0);
		}
	}

	void set_individuals(std::vector<std::vector<int>> inds)
	{
		individuals = inds;
		if (individuals.size() != individual_num)
		{
			std::cout << "individuals num don't match the setting num" << std::endl;
			std::cerr << "individuals num don't match the setting num" << std::endl;
		}
	}

	void set_individuals_random()
	{
		std::vector<std::vector<int>> tmp_vector;
		for (int i = 0; i < individual_num; i++)
		{
			std::vector<int> tmp;
			for (int k = 0; k < chrom_length; k++)
			{
				tmp.emplace_back(rnd(2));
			}
			tmp_vector.emplace_back(tmp);
		}
		
		individuals = tmp_vector;
	}

	void set_elite_num(const int& n)
	{
		elite_num = n;
	}

	void set_cross_rate(const double& val)
	{
		cross_rate = val;
	};
	void two_point_cross(std::vector<std::vector<int>>& vec, const int& parent1, const int& parent2);  // 2点交差。確率を指定。parentでindexを指定

	void set_mutation_rate(const double& val)
	{
		mutation_rate = val;
	};
	void mutate(const int& index);  // 突然変異

	void evaluate(const std::function<double(std::vector<int>)>& f)  // fは外から与える。評価対象の列を与えてその評価値doubleを返す関数。individuals全てを一度に評価する。
	{
		for (int i = 0; i < individual_num; i++)
		{
			individuals_points[i] = f(individuals[i]);
		}
	};

	std::vector<double> evaluate_test(const std::vector<std::vector<int>>& vec, const std::function<double(std::vector<int>)>& f)  // fは外から与える。評価対象の列を与えてその評価値doubleを返す関数。individuals全てを一度に評価する。
	{
		std::vector<double> test_vec;
		for (int i = 0; i < vec.size(); i++)
		{
			test_vec.emplace_back(f(vec[i]));
		}
		return test_vec;
	};

	std::vector<std::vector<int>> roulette_select(std::vector<std::vector<int>> vec_individuals, std::vector<double> vec_points, int constraint_num = 0);  // ルーレット選択。部分列に対して行う
	
	void process_generation(const std::function<double(std::vector<int>)>& f, const int& constraint_num);  // 1世代進める。評価されていることが前提。なおエリートは実装してない(交差や変異を避ける必要があるのでめんどくさい)。評価関数を与える

	void process_greedy_generation(const std::function<double(std::vector<int>)>& f);
	
	std::vector<double> get_evalutates()
	{
		return individuals_points;
	}

	std::vector<std::vector<int>> get_individuals()
	{
		return individuals;
	}

	std::pair<std::vector<std::vector<int>>, std::vector<double>> get_best_individuals(int n)    // 評価値の良いものからn個とそのスコアを返す
	{
		std::vector<size_t> indices(individual_num);
		std::iota(indices.begin(), indices.end(), 0);
		std::sort(indices.begin(), indices.end(), [&](size_t i1, size_t i2) {
			return individuals_points[i1] > individuals_points[i2];
		});

		// std::cout << indices << std::endl;
		
		std::vector<std::vector<int>> ret_individual;
		std::vector<double> ret_score;
		for (int i = 0; i < n; i++)
		{
			ret_individual.emplace_back(individuals[indices[i]]);
			ret_score.emplace_back(individuals_points[indices[i]]);
		}
		std::pair<std::vector<std::vector<int>>, std::vector<double>> ret = std::make_pair(ret_individual, ret_score);

		return ret;
	};
};

class GAIntWithoutDup
{
private:
	int individual_num;  // 1世代あたりの個体数
	int chrom_length;  // 染色体の長さ
	// 実装上、エリートとそうでない分を分けておく方が便利
	std::vector<std::vector<int>> individuals;  // 個体群
	std::vector<double> individuals_points;  // 評価値のvector

public:
	double cross_rate = 0.0;
	double mutation_rate = 0.0;
	int elite_num = 0;  // eliteの数

	GAIntWithoutDup(int ind_n, int ind_c)
	{
		individual_num = ind_n;
		chrom_length = ind_c;

		for (int i = 0; i < individual_num; i++)
		{
			individuals_points.emplace_back(0.0);
		}
	}

	void set_individuals(std::vector<std::vector<int>> inds)
	{
		individuals = inds;
		if (individuals.size() != individual_num)
		{
			std::cout << "individuals num don't match the setting num" << std::endl;
			std::cerr << "individuals num don't match the setting num" << std::endl;
		}
	}

	void set_individuals_random(const std::vector<int> &vec)
	{
		// 与えられたvector(長さはchrom_length)をランダムに並び替え、individual_num個分作る。
		std::vector<std::vector<int>> tmp_vector;
		std::random_device seed_gen;
		std::mt19937 engine(seed_gen());
		for (int i = 0; i < individual_num; i++)
		{
			std::vector<int> tmp(vec.size());
			std::copy(vec.begin(), vec.end(), tmp.begin());
			for (int k = 0; k < chrom_length; k++)
			{
				std::shuffle(tmp.begin(), tmp.end(), engine);
			}
			tmp_vector.emplace_back(tmp);
		}

		individuals = tmp_vector;
	}

	void set_elite_num(const int& n)
	{
		elite_num = n;
	}

	void set_cross_rate(const double& val)
	{
		cross_rate = val;
	};
	void partial_cross(std::vector<std::vector<int>>& vec, const int& parent1, const int& parent2)
	{
		// まず2点選ぶ
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

		// 真ん中部分で部分交叉(重複排除のため)
		for (int i = firstp; i <= secondp; i++)
		{
			// 複数同じ値がある可能性もあり、右からか左から探すかを選ぶことにする。
			// 先に保存しておいて、まずその場で交換
			int tmp_1 = vec[parent1][i];
			int tmp_2 = vec[parent2][i];
			// 先に探しておかないと、交換すると同じ値が来てしまう。

			int vec_length = vec[parent1].size();
			if (rnd.randBool(0.5))
			{
				for (auto &a : vec[parent1])
				{
					if (a == tmp_2)
					{
						a = tmp_1;
						break;
					}
				}

				for (auto &a : vec[parent2])
				{
					if (a == tmp_1)
					{
						a = tmp_2;
						break;
					}
				}
			} else
			{
				for (int k = vec_length - 1; k >= 0; k--)
				{
					if (vec[parent1][k] == tmp_2)
					{
						vec[parent1][k] = tmp_1;
						break;
					}
				}

				for (int k = vec_length - 1; k >= 0; k--)
				{
					if (vec[parent2][k] == tmp_1)
					{
						vec[parent2][k] = tmp_2;
						break;
					}
				}
			}
			// その後で元を交換
			std::iter_swap(vec[parent1].begin() + i, vec[parent2].begin() + i);
			// std::iter_swap(vec[parent1].begin() + i, vec[parent2].begin() + i);
		}
	};  // 部分交差。確率を指定。parentでindexを指定

	void set_mutation_rate(const double& val)
	{
		mutation_rate = val;
	};
	void mutate(const int& index)
	{
		// すべての遺伝子につきmutation_rateに基づいて変異を加える
		for (int i = 0; i < chrom_length; i++)
		{
			if (rnd.randBool(mutation_rate))
			{
				// ランダムにもう1点選ぶ。
				int p = rnd(chrom_length);

				while (i == p)
				{
					p = rnd(chrom_length);  // 偶然同じなら選び直し
				}
				// その値と入れ替える
				int tmp = individuals[index][i];
				individuals[index][i] = individuals[index][p];
				individuals[index][p] = tmp;
			}
		}
		
	};  // 突然変異は転座で

	void evaluate(const std::function<double(std::vector<int>)>& f)  // fは外から与える。評価対象の列を与えてその評価値doubleを返す関数。individuals全てを一度に評価する。
	{
		for (int i = 0; i < individual_num; i++)
		{
			individuals_points[i] = f(individuals[i]);
		}
	};

	std::vector<double> evaluate_test(const std::vector<std::vector<int>>& vec, const std::function<double(std::vector<int>)>& f)  // fは外から与える。評価対象の列を与えてその評価値doubleを返す関数。individuals全てを一度に評価する。
	{
		std::vector<double> test_vec;
		for (int i = 0; i < vec.size(); i++)
		{
			test_vec.emplace_back(f(vec[i]));
		}
		return test_vec;
	};

	std::vector<std::vector<int>> roulette_select(std::vector<std::vector<int>> vec_individuals, std::vector<double> vec_points, int constraint_num = 0)
	{

		
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
	}// ルーレット選択。部分列に対して行う

	void process_generation(const std::function<double(std::vector<int>)>& f, const int& constraint_num)  // 1世代進める。評価されていることが前提。なおエリートは実装してない(交差や変異を避ける必要があるのでめんどくさい)。評価関数を与える
	{
		if (std::all_of(individuals_points.begin(), individuals_points.end(), [](int x) { return x == 0; }))
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
			if (std::find(high_evalpoints_ind.begin(), high_evalpoints_ind.end(), i) != high_evalpoints_ind.end())
			{
				elites.emplace_back(individuals[i]);
				elites_points.emplace_back(individuals_points[i]);
			}
			else
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
				partial_cross(selected_non_elites, 2 * i, 2 * i + 1);
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


	std::vector<double> get_evalutates()
	{
		return individuals_points;
	}

	std::vector<std::vector<int>> get_individuals()
	{
		return individuals;
	}

	std::pair<std::vector<std::vector<int>>, std::vector<double>> get_best_individuals(int n)    // 評価値の良いものからn個とそのスコアを返す
	{
		std::vector<size_t> indices(individual_num);
		std::iota(indices.begin(), indices.end(), 0);
		std::sort(indices.begin(), indices.end(), [&](size_t i1, size_t i2) {
			return individuals_points[i1] > individuals_points[i2];
			});

		// std::cout << indices << std::endl;

		std::vector<std::vector<int>> ret_individual;
		std::vector<double> ret_score;
		for (int i = 0; i < n; i++)
		{
			ret_individual.emplace_back(individuals[indices[i]]);
			ret_score.emplace_back(individuals_points[indices[i]]);
		}
		std::pair<std::vector<std::vector<int>>, std::vector<double>> ret = std::make_pair(ret_individual, ret_score);

		return ret;
	};
};