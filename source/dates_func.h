#pragma once

// ���t�v�Z�p�B�o�ߔN�ɂ��N���⍷�A�j�����v�Z����B
// https://ufcpp.net/study/algorithm/o_days.html�Q��(�O���S���I��̌o�ߓ���) ..�܂��ǂ�����ł��ǂ��̂���..


inline int GetDays(int y, int m, int d)
{
    if (m <= 2)
    {
        m += 12;
        y -= 1;
    }
    int dy = 365 * (y - 1);
    int c = y / 100;
    int dl = (y >> 2) - c + (c >> 2);
    int dm = (m * 979 - 1033) >> 5;
    return dy + dl + dm + d - 1;
}

// �j���v�Z(�c�F���[�̌���)
// 0: ���j�� 1: ���j�� ...
inline int GetWeekDay(int y, int m, int d)
{
    if (m < 2) {
        m += 12;
        y -= 1;
    }
    return (y + y / 4 - y / 100 + y / 400 + (13 * m + 8) / 5 + d) % 7;
}

// ���邤�N����
inline bool UruJudge(int y)
{
    if (y % 400 == 0 || (y % 4 == 0 && y % 100 != 0)) {
    	return true;
    }
    else {
        return false;
    }
}
