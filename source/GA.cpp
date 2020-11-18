#include "GA.h"


void GABit1d::two_point_cross(std::vector<std::vector<int>>& vec, const int& parent1, const int& parent2)  // ���̒��ɂ�mutate_rate�̗v�f�������ĂȂ����Ƃɒ���
{
	// �؂蕪����|�C���g�I�ԁB0��0��1�̊ԁB
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

	// �^�񒆕���������
	for (int i = firstp; i <= secondp; i++)
	{
		std::iter_swap(vec[parent1].begin() + i, vec[parent2].begin() + i);
	}
}

void GABit1d::mutate(const int& index)  // index�͐��F�̔ԍ�
{
	// ���ׂĂ̈�`�q�ɂ�mutation_rate�Ɋ�Â��ĕψق�������
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
	// �ǂ����K���x�����̐����c���Ă���m�������̂ŁAelite�I���Ȃ���
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
}

void GABit1d::process_generation(const std::function<double(std::vector<int>)>& f, const int& constraint_num)
{
	if (std::all_of(individuals_points.begin(), individuals_points.end(), [](int x) { return x == 0;}))
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

	// roulette�I��
	auto selected_non_elites = roulette_select(non_elites, non_elites_points, constraint_num);

	// �Ⴂ�ԍ�����2���I��Ō���������
	for (int i = 0, s = selected_non_elites.size() / 2; i < s; i++)
	{
		if (rnd.randBool(cross_rate))
		{
			two_point_cross(selected_non_elites, 2 * i, 2 * i + 1);
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

void GABit1d::process_greedy_generation(const std::function<double(std::vector<int>)>& f)
{
	if (std::all_of(individuals_points.begin(), individuals_points.end(), [](int x) { return x == 0; }))
	{
		evaluate(f);
	}
	
}
