#pragma once

#include <iostream>
#include <vector>
#include "random_custom.h"
#include <functional>
#include <numeric>

#include "funcs.h"

// 1���ゲ�Ƃ�evaluate����K�v������̂ŖY�ꂸ��
class GABit1d
{
private:
	int individual_num;  // 1���゠����̌̐�
	int chrom_length;  // ���F�̂̒���
	// ������A�G���[�g�Ƃ����łȂ����𕪂��Ă��������֗�
	std::vector<std::vector<int>> individuals;  // �̌Q�A�܂�1���� 0 or 1�̃r�b�g
	std::vector<double> individuals_points;  // �]���l��vector
	
public:
	double cross_rate = 0.0;
	double mutation_rate = 0.0;
	int elite_num = 0;  // elite�̐�
	
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
	void two_point_cross(std::vector<std::vector<int>>& vec, const int& parent1, const int& parent2);  // 2�_�����B�m�����w��Bparent��index���w��

	void set_mutation_rate(const double& val)
	{
		mutation_rate = val;
	};
	void mutate(const int& index);  // �ˑR�ψ�

	void evaluate(const std::function<double(std::vector<int>)>& f)  // f�͊O����^����B�]���Ώۂ̗��^���Ă��̕]���ldouble��Ԃ��֐��Bindividuals�S�Ă���x�ɕ]������B
	{
		for (int i = 0; i < individual_num; i++)
		{
			individuals_points[i] = f(individuals[i]);
		}
	};

	std::vector<double> evaluate_test(const std::vector<std::vector<int>>& vec, const std::function<double(std::vector<int>)>& f)  // f�͊O����^����B�]���Ώۂ̗��^���Ă��̕]���ldouble��Ԃ��֐��Bindividuals�S�Ă���x�ɕ]������B
	{
		std::vector<double> test_vec;
		for (int i = 0; i < vec.size(); i++)
		{
			test_vec.emplace_back(f(vec[i]));
		}
		return test_vec;
	};

	std::vector<std::vector<int>> roulette_select(std::vector<std::vector<int>> vec_individuals, std::vector<double> vec_points, int constraint_num = 0);  // ���[���b�g�I���B������ɑ΂��čs��
	
	void process_generation(const std::function<double(std::vector<int>)>& f, const int& constraint_num);  // 1����i�߂�B�]������Ă��邱�Ƃ��O��B�Ȃ��G���[�g�͎������ĂȂ�(������ψق������K�v������̂ł߂�ǂ�����)�B�]���֐���^����

	void process_greedy_generation(const std::function<double(std::vector<int>)>& f);
	
	std::vector<double> get_evalutates()
	{
		return individuals_points;
	}

	std::vector<std::vector<int>> get_individuals()
	{
		return individuals;
	}

	std::pair<std::vector<std::vector<int>>, std::vector<double>> get_best_individuals(int n)    // �]���l�̗ǂ����̂���n�Ƃ��̃X�R�A��Ԃ�
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
	int individual_num;  // 1���゠����̌̐�
	int chrom_length;  // ���F�̂̒���
	// ������A�G���[�g�Ƃ����łȂ����𕪂��Ă��������֗�
	std::vector<std::vector<int>> individuals;  // �̌Q
	std::vector<double> individuals_points;  // �]���l��vector

public:
	double cross_rate = 0.0;
	double mutation_rate = 0.0;
	int elite_num = 0;  // elite�̐�

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
		// �^����ꂽvector(������chrom_length)�������_���ɕ��ёւ��Aindividual_num�����B
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
		// �܂�2�_�I��
		int firstp = rnd(chrom_length);
		int secondp = rnd(chrom_length);
		while (firstp == secondp)
		{
			//�@������ꏏ�Ȃ�I�ђ���
			secondp = rnd(chrom_length);
		}

		// ���Ԃɕ��ׂ�
		if (firstp > secondp)
		{
			std::swap(firstp, secondp);
		}

		// �^�񒆕����ŕ�������(�d���r���̂���)
		for (int i = firstp; i <= secondp; i++)
		{
			// ���������l������\��������A�E���炩������T������I�Ԃ��Ƃɂ���B
			// ��ɕۑ����Ă����āA�܂����̏�Ō���
			int tmp_1 = vec[parent1][i];
			int tmp_2 = vec[parent2][i];
			// ��ɒT���Ă����Ȃ��ƁA��������Ɠ����l�����Ă��܂��B

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
			// ���̌�Ō�������
			std::iter_swap(vec[parent1].begin() + i, vec[parent2].begin() + i);
			// std::iter_swap(vec[parent1].begin() + i, vec[parent2].begin() + i);
		}
	};  // ���������B�m�����w��Bparent��index���w��

	void set_mutation_rate(const double& val)
	{
		mutation_rate = val;
	};
	void mutate(const int& index)
	{
		// ���ׂĂ̈�`�q�ɂ�mutation_rate�Ɋ�Â��ĕψق�������
		for (int i = 0; i < chrom_length; i++)
		{
			if (rnd.randBool(mutation_rate))
			{
				// �����_���ɂ���1�_�I�ԁB
				int p = rnd(chrom_length);

				while (i == p)
				{
					p = rnd(chrom_length);  // ���R�����Ȃ�I�ђ���
				}
				// ���̒l�Ɠ���ւ���
				int tmp = individuals[index][i];
				individuals[index][i] = individuals[index][p];
				individuals[index][p] = tmp;
			}
		}
		
	};  // �ˑR�ψق͓]����

	void evaluate(const std::function<double(std::vector<int>)>& f)  // f�͊O����^����B�]���Ώۂ̗��^���Ă��̕]���ldouble��Ԃ��֐��Bindividuals�S�Ă���x�ɕ]������B
	{
		for (int i = 0; i < individual_num; i++)
		{
			individuals_points[i] = f(individuals[i]);
		}
	};

	std::vector<double> evaluate_test(const std::vector<std::vector<int>>& vec, const std::function<double(std::vector<int>)>& f)  // f�͊O����^����B�]���Ώۂ̗��^���Ă��̕]���ldouble��Ԃ��֐��Bindividuals�S�Ă���x�ɕ]������B
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

		
		std::vector<std::vector<int>> next_generation_partial;  // ���̐���ɂȂ�͂���vector

		double sum = std::accumulate(vec_points.begin(), vec_points.end(), 0.0);  // �]���l�̍��v

		//�~�ϒl�Ń��[���b�g
		int ind_num = vec_individuals.size();

		if (constraint_num == 0)
		{
			constraint_num = ind_num;
		}

		std::random_device rd;
		std::mt19937 mt(rd());

		// �����t��version�̎���		
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

		// ������X�V
		return next_generation_partial;
	}// ���[���b�g�I���B������ɑ΂��čs��

	void process_generation(const std::function<double(std::vector<int>)>& f, const int& constraint_num)  // 1����i�߂�B�]������Ă��邱�Ƃ��O��B�Ȃ��G���[�g�͎������ĂȂ�(������ψق������K�v������̂ł߂�ǂ�����)�B�]���֐���^����
	{
		if (std::all_of(individuals_points.begin(), individuals_points.end(), [](int x) { return x == 0; }))
		{
			evaluate(f);
		}

		std::vector<std::vector<int>> next_generation;

		// n�̃G���[�g���܂��������G���[�g�͌������ˑR�ψق̑Ώۂɂ��Ȃ�Ȃ�
		std::vector<std::vector<int>> elites;
		std::vector<double> elites_points;

		std::vector<std::vector<int>> non_elites;
		std::vector<double> non_elites_points;

		// �]���l�����������珇��n��index�𒲂ׂ�(argsort��)�B
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

		// roulette�I��
		auto selected_non_elites = roulette_select(non_elites, non_elites_points, constraint_num);

		// �Ⴂ�ԍ�����2���I��Ō���������
		for (int i = 0, s = selected_non_elites.size() / 2; i < s; i++)
		{
			if (rnd.randBool(cross_rate))
			{
				partial_cross(selected_non_elites, 2 * i, 2 * i + 1);
			}
		}
		// �ˑR�ψق�����
		for (int i = 0, s = selected_non_elites.size(); i < s; i++)
		{
			mutate(i);
		}

		// elite��non_elite����������B�����ĕ]������B
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

	std::pair<std::vector<std::vector<int>>, std::vector<double>> get_best_individuals(int n)    // �]���l�̗ǂ����̂���n�Ƃ��̃X�R�A��Ԃ�
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