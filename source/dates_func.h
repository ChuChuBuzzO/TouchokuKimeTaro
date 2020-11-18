#pragma once

// 日付計算用。経過年により年月や差、曜日を計算する。
// https://ufcpp.net/study/algorithm/o_days.html参照(グレゴリオ暦の経過日数) ..まあどこが基準でも良いのだが..


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

// 曜日計算(ツェラーの公式)
// 0: 日曜日 1: 月曜日 ...
inline int GetWeekDay(int y, int m, int d)
{
    if (m < 2) {
        m += 12;
        y -= 1;
    }
    return (y + y / 4 - y / 100 + y / 400 + (13 * m + 8) / 5 + d) % 7;
}

// うるう年判定
inline bool UruJudge(int y)
{
    if (y % 400 == 0 || (y % 4 == 0 && y % 100 != 0)) {
    	return true;
    }
    else {
        return false;
    }
}
