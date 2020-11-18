#pragma execution_character_set("utf-8")
#include "DoctorScheduling.h"
#include <QSpinBox>
#include <QPushButton>
#include <QMessageBox>
#include "funcs.h"
#include <unordered_map>
#include <tuple>
#include "GA.h"
#include <QProgressBar>
#include <fstream>
#include <sstream>
#include <QFileDialog>

QTableWidget* DisplayCalendar(const int& month, const int& year)
{
    std::vector<int> month_long = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

    if (UruJudge(year))
    {
        month_long[1] = 29;
    }

    auto* table = new QTableWidget;
    table->setRowCount(month_long[month - 1]);
    table->setColumnCount(2);

    auto str = new QTableWidgetItem(QString("枠数"));
    table->setHorizontalHeaderItem(0, str);
    auto str1 = new QTableWidgetItem(QString("担当者"));
    table->setHorizontalHeaderItem(1, str1);

    table->setAutoScroll(true);

    std::vector<QSpinBox*> sps;
    for (int i = 0; i < month_long[month - 1]; i++)
    {
        //table->setColumnWidth(i, 100);
        //auto lb = new QLabel(QString::number(i + 1));
        //lb->setStyleSheet("color: red");

        //table->setCellWidget(i, 0, lb);
        auto sp = new QSpinBox();
        sp->setAlignment(Qt::AlignRight);
        sps.emplace_back(sp);

        int weekday = GetWeekDay(year, month, i + 1);
        if (weekday == 0)
        {
            auto str = new QTableWidgetItem(QString::number(i + 1) + QString(" (日)"));
            str->setForeground(Qt::red);
            table->setVerticalHeaderItem(i, str);
            sp->setValue(2);
            table->setCellWidget(i, 0, sp);
        }
        else if (weekday == 1)
        {
            auto str = new QTableWidgetItem(QString::number(i + 1) + QString(" (月)"));
            sp->setValue(1);
            table->setVerticalHeaderItem(i, str);
            table->setCellWidget(i, 0, sp);
        }
        else if (weekday == 2)
        {
            auto str = new QTableWidgetItem(QString::number(i + 1) + QString(" (火)"));
            sp->setValue(1);
            table->setVerticalHeaderItem(i, str);
            table->setCellWidget(i, 0, sp);
        }
        else if (weekday == 3)
        {
            auto str = new QTableWidgetItem(QString::number(i + 1) + QString(" (水)"));
            sp->setValue(1);
            table->setVerticalHeaderItem(i, str);
            table->setCellWidget(i, 0, sp);
        }
        else if (weekday == 4)
        {
            auto str = new QTableWidgetItem(QString::number(i + 1) + QString(" (木)"));
            sp->setValue(1);
            table->setVerticalHeaderItem(i, str);
            table->setCellWidget(i, 0, sp);
        }
        else if (weekday == 5)
        {
            auto str = new QTableWidgetItem(QString::number(i + 1) + QString(" (金)"));
            sp->setValue(1);
            table->setVerticalHeaderItem(i, str);
            table->setCellWidget(i, 0, sp);
        }
        else
        {
            auto str = new QTableWidgetItem(QString::number(i + 1) + QString(" (土)"));
            str->setForeground(Qt::red);
            sp->setValue(2);
            table->setVerticalHeaderItem(i, str);
            table->setCellWidget(i, 0, sp);
        }
    }

    return table;
}

QTableWidget* DisplayKoho()
{
    auto* table = new QTableWidget;

    table->setColumnCount(4);
    table->setRowCount(1);
    table->setColumnWidth(1, 50);
    table->setColumnWidth(3, 235);

    auto str = new QTableWidgetItem(QString("氏名"));
    table->setHorizontalHeaderItem(0, str);
    auto str1 = new QTableWidgetItem(QString("枠数"));
    table->setHorizontalHeaderItem(1, str1);
    auto str2 = new QTableWidgetItem(QString("希望日"));
    table->setHorizontalHeaderItem(2, str2);
    auto str3 = new QTableWidgetItem(QString("不可日"));
    table->setHorizontalHeaderItem(3, str3);

    // table->setAutoScroll(true);

    return table;
}

/// constructor
DoctorScheduling::DoctorScheduling(QWidget* parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

    time_t t = std::time(nullptr);
    const tm* localTime = std::localtime(&t);
    int month = localTime->tm_mon + 1;  // 今の月
    int year = localTime->tm_year + 1900;  // 今年

    ui.spinBoxY->setValue(year);
    ui.spinBoxM->setValue(month);

    calendar = DisplayCalendar(month, year);
    ui.scrollArea->setWidget(calendar);

    tablekoho = DisplayKoho();
    ui.scrollArea_2->setWidget(tablekoho);



    // button関連付け
    connect(ui.pushButtonAdd, &QPushButton::clicked, this, &DoctorScheduling::buttonAddClicked);
    connect(ui.pushButtonDelete, &QPushButton::clicked, this, &DoctorScheduling::buttonDeleteClicked);
    connect(ui.pushButtonRun, &QPushButton::clicked, this, &DoctorScheduling::buttonRunClicked);
    connect(ui.pushButtonGASetting, &QPushButton::clicked, this, &DoctorScheduling::buttonGASettingClicked);
    connect(ui.pushButtonLoad, &QPushButton::clicked, this, &DoctorScheduling::buttonLoadClicked);
    connect(ui.pushButtonSave, &QPushButton::clicked, this, &DoctorScheduling::buttonSaveClicked);
    connect(ui.pushButtonExport, &QPushButton::clicked, this, &DoctorScheduling::buttonExportClicked);

}

void DoctorScheduling::buttonAddClicked()
{
    tablekoho->insertRow(0);
}

void DoctorScheduling::buttonDeleteClicked()
{
    int r = tablekoho->currentRow();
    tablekoho->removeRow(r);
}

GA_sub::GA_sub(DoctorScheduling* parent)
{
    ui_sub.setupUi(this);
    ui_sub.spinBoxIndNum->setValue(parent->ind_num);  // これで親にアクセスできる
    ui_sub.spinBoxGenNum->setValue(parent->gen_num);
    ui_sub.spinBoxEliteNum->setValue(parent->elite_num);
    ui_sub.spinBoxEliteNum->setMaximum(parent->ind_num - 1);

    ui_sub.doubleSpinBoxCrossRate->setValue(parent->cross_rate);
    ui_sub.doubleSpinBoxMutationRate->setValue(parent->mutation_rate);
    ui_sub.checkBoxE->setChecked(parent->end_value_flag);
    ui_sub.doubleSpinBoxEValue->setValue(parent->end_value);

}



void DoctorScheduling::buttonGASettingClicked()
{
    auto ga_sub = new GA_sub(this);
    int i = ga_sub->exec();

    if (i == QDialog::Accepted)  // okボタン
    {
        ind_num = ga_sub->ui_sub.spinBoxIndNum->value();

        gen_num = ga_sub->ui_sub.spinBoxGenNum->value();
        elite_num = ga_sub->ui_sub.spinBoxEliteNum->value();
        cross_rate = ga_sub->ui_sub.doubleSpinBoxCrossRate->value();
        mutation_rate = ga_sub->ui_sub.doubleSpinBoxMutationRate->value();
        end_value_flag = ga_sub->ui_sub.checkBoxE->checkState();
        end_value = ga_sub->ui_sub.doubleSpinBoxEValue->value();
    }
}

void DoctorScheduling::buttonLoadClicked()
{
    QString filename = QFileDialog::getOpenFileName(this, tr("open the csv file"), "c:/", tr("csv files(*.csv);;All Files(*.*)"));

    auto file_path = filename.toStdString();
    std::ifstream ifs(file_path);

    bool flag = true;
    if (!ifs)
    {
        std::cout << "Error! File can not be opened" << std::endl;
        flag = false;
    }

    if (flag)
    {
        tablekoho->setRowCount(0);
        std::string str_buf;
        while (std::getline(ifs, str_buf))
        {
            std::string str_tmp;
            std::istringstream stream(str_buf);

            tablekoho->insertRow(tablekoho->rowCount());
            // ","区切りで読み込む
            int j = 0;
            while (std::getline(stream, str_tmp, ','))
            {
                if (j > 3)
                {
                    break;
                }
                QString qs = QString::fromLocal8Bit(str_tmp.c_str());  // qstringに変換
                qs.replace(" ", ",");
                //std::cout << qs.toStdString() << std::endl;
                auto pCell = new QTableWidgetItem;
                tablekoho->setItem(tablekoho->rowCount() - 1, j, pCell);
                pCell->setText(qs);

                j++;
            }
        }
    }
}

void DoctorScheduling::buttonSaveClicked()
{
    QString filename = QFileDialog::getSaveFileName(this, tr("select the directory"), "c:/", tr("csv files(*.csv)"));

    std::ofstream ofs(filename.toStdString());

    int index = 0;
    while (true)
    {
        if (index >= tablekoho->rowCount())
        {
            break;
        }
        auto a = tablekoho->item(index, 0);

        if (a)
        {
            ofs << tablekoho->item(index, 0)->text().trimmed().toLocal8Bit().toStdString() << ",";
            ofs << tablekoho->item(index, 1)->text().trimmed().toLocal8Bit().toStdString() << ",";
            ofs << tablekoho->item(index, 2)->text().trimmed().replace(",", " ").toLocal8Bit().toStdString() << ",";
            ofs << tablekoho->item(index, 3)->text().trimmed().replace(",", " ").toLocal8Bit().toStdString() << std::endl;
        }
        index++;
    }
}

void DoctorScheduling::buttonExportClicked()
{
    QString filename = QFileDialog::getSaveFileName(this, tr("select the directory"), "c:/", tr("csv files(*.csv)"));

    std::ofstream ofs(filename.toStdString());

    int index = 0;
    while (true)
    {
        if (index >= calendar->rowCount())
        {
            break;
        }

        ofs << calendar->verticalHeaderItem(index)->text().toLocal8Bit().toStdString() << " ";
        ofs << "{" << qobject_cast<QSpinBox*>(calendar->cellWidget(index, 0))->value() << "}: ";
        ofs << calendar->item(index, 1)->text().trimmed().toLocal8Bit().toStdString() << std::endl;
        index++;
    }
}



void DoctorScheduling::buttonRunClicked()
{
    bool check_before_run = true;
    QString log_string = "";
    /// row_indices-> calendar上の日vector
    /// required_people_num-> 上記に対応した必要人数
    /// tantou_name_vec-> 担当者名のvector
    /// tantou_day_vec-> 日数のvector
    /// tantou_hope_vec-> 希望日のvector
    /// tantou_deny_vec-> 不可日のvector
    // 選択範囲があれば、その中だけで実行
    // 選択row indexを、分かりにくいのでvectorに
    QModelIndexList selectedList = calendar->selectionModel()->selectedRows();
    //for (int i = 0; i < selectedList.count(); i++)
    //    QMessageBox::information(this, "", QString::number(selectedList.at(i).row()));
    std::vector<int> row_indices;  // indicesだが、何日に対応させる。{1, 2, 3, 3, 4, 4, 5, 6, 7}みたいになる。(必要人数分重なる)
    //std::vector<int> required_people_num;  // 必要人数
    for (auto a : selectedList)
    {
        // std::cout << a.row() << std::endl;
        auto b = calendar->verticalHeaderItem(a.row())->text();
        QStringList l = b.split(QLatin1Char(' '));
        // fprintf(stdout, qPrintable(l[0]));

        for (int i = 0, s = qobject_cast<QSpinBox*>(calendar->cellWidget(a.row(), 0))->value(); i < s; i++)
        {
            row_indices.emplace_back(l[0].toInt());
        }
        //required_people_num.emplace_back(qobject_cast<QSpinBox*>(calendar->cellWidget(a.row(), 0))->value());

    }
    //for(int i = 0; i < row_indices.size(); i++)
    //{
 //       std::cout << row_indices[i] << std::endl;
    //}

    if (row_indices.empty())
    {
        for (int i = 0, s = calendar->rowCount(); i < s; i++)
        {
            auto b = calendar->verticalHeaderItem(i)->text().split(QLatin1Char(' '))[0].toInt();
            for (int j = 0, s = qobject_cast<QSpinBox*>(calendar->cellWidget(i, 0))->value(); j < s; j++)
            {

                row_indices.emplace_back(b);
            }

            // required_people_num.emplace_back(qobject_cast<QSpinBox*>(calendar->cellWidget(i, 0))->value());
        }
    }

    //std::cout << row_indices << std::endl;
    //std::cout << required_people_num << std::endl;
    //
    /// 次に、tantouを取得していく。
    std::vector<QString> tantou_name_vec;
    std::vector<int> tantou_days_vec;
    std::vector<std::vector<int>> tantou_hope_vec;
    std::vector<std::vector<int>> tantou_deny_vec;
    for (int i = 0, s = tablekoho->rowCount(); i < s; i++)
    {
        auto name_item = tablekoho->item(i, 0);
        if (name_item)  // 空の名前行は無視
        {
            auto name = name_item->text();

            auto str_num_item = tablekoho->item(i, 1);
            QString str_num;
            if (str_num_item)
            {
                str_num = str_num_item->text();
            }
            else
            {
                str_num = QString("0");
            }
            bool ok1;
            auto day_num = str_num.toInt(&ok1, 10);
            // fprintf(stdout, qPrintable(str_num));
            if (!ok1)
            {
                check_before_run = false;
                log_string += "担当者枠数に無効な数字が含まれています(半角英数で入力を)\n";
            }


            auto hoped_days_str_item = tablekoho->item(i, 2);
            QStringList hoped_days_str;
            if (hoped_days_str_item)
            {
                hoped_days_str = hoped_days_str_item->text().split(QLatin1Char(','));
            }

            std::vector<int> hoped_days;



            for (auto a : hoped_days_str)
            {
                if (!(a == ""))  // 一回セル触ると空文字列が入ってしまう
                {
                    bool ok2;
                    int tmp = a.trimmed().toInt(&ok2, 10);
                    if (!ok2)
                    {
                        check_before_run = false;
                        log_string += "希望日に無効な数字が含まれています(','区切り、半角英数で入力を)\n";
                    }
                    else
                    {
                        hoped_days.emplace_back(tmp);
                    }
                }
            }



            auto denied_days_str_item = tablekoho->item(i, 3);
            QStringList denied_days_str;
            if (denied_days_str_item)
            {
                denied_days_str = denied_days_str_item->text().split(QLatin1Char(','));
            }

            std::vector<int> denied_days;


            for (auto a : denied_days_str)
            {
                if (!(a == ""))
                {
                    bool ok3;
                    int tmp = a.trimmed().toInt(&ok3, 10);
                    if (!ok3)
                    {
                        check_before_run = false;
                        log_string += "不可日に無効な数字が含まれています(','区切り、半角英数で入力を)\n";
                    }
                    else
                    {
                        denied_days.emplace_back(tmp);
                    }
                }

            }



            //koho_map[name] = std::tuple<int, std::vector<int>, std::vector<int>>(day_num, hoped_days, denied_days);
            tantou_name_vec.emplace_back(name);
            tantou_days_vec.emplace_back(day_num);
            tantou_hope_vec.emplace_back(hoped_days);
            tantou_deny_vec.emplace_back(denied_days);
        }
    }
    // 後で必要なことに気づいた
    if (tantou_name_vec.empty())
    {
        check_before_run = false;
        log_string += "誰も入力がありません\n";
    }

    //for (auto a : koho_map)
    //{
 //       fprintf(stdout, qPrintable(a.first));
 //       std::cout << "\n" << std::get<0>(a.second) << std::endl;
 //       std::cout << "\n" << std::get<1>(a.second) << std::endl;
 //       std::cout << "\n" << std::get<2>(a.second) << std::endl;
    //}

    /// 設定値の取得
    int vacant_days = ui.spinBoxVac->value();
    double penalty0 = ui.spinBoxPenalty0->value();
    double penalty1 = ui.spinBoxPenalty1->value();
    double penalty2 = ui.spinBoxPenalty2->value();
    double penalty3 = ui.spinBoxPenalty3->value();
    double penalty4 = ui.spinBoxPenalty4->value();

    ui.textBrowser->setText(log_string);

    // 以下でGA実行
    if (check_before_run)
    {
        // まず、日数 * 人数分の要素を持つvectorをひとつの個体とする。
        // 要素数は
        int individual_length = row_indices.size() * tantou_name_vec.size();

        // std::cout << individual_num << std::endl;

        auto ga = GABit1d(ind_num, individual_length);

        ga.set_individuals_random();
        ga.set_cross_rate(cross_rate);
        ga.set_mutation_rate(mutation_rate);
        ga.set_elite_num(elite_num);

        // 評価関数(どうしてもvectorを引数で持つ関数で渡さないといけないのでやや工夫が必要になる。ラムダ式でいけるはず)
        int days = row_indices.size();
        int people_num = tantou_name_vec.size();
        // std::cout << row_indices << std::endl;
        auto evaluate = [&](const std::vector<int>& vec)
        {
            double penalty_value = 0.0; // penaltyの値なので最後に逆数にする必要あり。

            std::vector<int> waku(days, 0);
            for (int i = 0; i < people_num; i++)
            {
                // 1人ずつ見ていく
                int min_ind = i * days;
                // 1のindicesを先に調べる。
                std::vector<int> one_indices;
                for (int k = 0; k < days; k++)
                {
                    if (vec[min_ind + k] == 1)  // 1だったら
                    {
                        one_indices.emplace_back(k);  // これで1のindicesに変換できた
                        waku[k] += 1;
                    }
                }
                // vacant_days以内に1が2つあればpenalty。つまり1が2つ以上あるとき、その差がvacant_days以内であればpenalty
                if (one_indices.size() >= 2)
                {
                    // 日数空かなかった場合のペナルティ(これだけ計算分けないといけない)
                    for (int j = 1, s = one_indices.size(); j < s; j++)
                    {
                        int diff = row_indices[one_indices[j]] - row_indices[one_indices[j - 1]];
                        // std::cout << diff << std::endl;
                        if (diff < vacant_days)
                        {
                            penalty_value += penalty0 * (vacant_days - diff);
                        }
                    }
                }
                // 希望日に入れなかった場合のペナルティ
                if (!(tantou_hope_vec[i].empty()))  // 希望日なければそもそも加算しない。ある場合だけ加算。
                {
                    for (auto a : one_indices)
                    {
                        if (!(std::find(tantou_hope_vec[i].begin(), tantou_hope_vec[i].end(), row_indices[a]) != tantou_hope_vec[i].end()))
                        {
                            // 希望日に入れない
                            penalty_value += penalty1;
                        }
                    }
                }

                // 不可日にあたった場合のペナルティ
                for (auto a : one_indices)
                {
                    if (std::find(tantou_deny_vec[i].begin(), tantou_deny_vec[i].end(), row_indices[a]) != tantou_deny_vec[i].end()) {
                        penalty_value += penalty2;
                    }
                }

                // 枠数を超えた場合のペナルティ
                int waku_num = one_indices.size();  // 実際の枠
                int tmp_num = tantou_days_vec[i];  // 担当の希望枠数
                if (waku_num > tmp_num)
                {
                    penalty_value += penalty3 * (waku_num - tmp_num);  // 1日超えるごとに掛け算
                }
                else if (waku_num < tmp_num)
                {
                    penalty_value += penalty4 * (tmp_num - waku_num);  // 1日満たないごとに掛け算
                }
            }
            // 日数分前部うまっていないとまずい
            //std::cout << waku << std::endl;
            // std::cout << waku << std::endl;
            for (auto a : waku)
            {
                if (!(a == 1))
                {
                    penalty_value += std::abs(a - 1) * 10000;
                }
            }
            // std::cout << penalty_value << std::endl;


            return 1 / (penalty_value + 0.000001);
        };

        double best_value = 0.0;
        int cnt_unchanged = 0;
        for (int i = 0; i < gen_num; i++)
        {
            //std::cout << "-------" << i << " generation -------" << std::endl;
            // std::cout << "\r----------" << i << " generation ----------";
            ui.progressBar->setValue(i * 100 / gen_num);

            ga.process_generation(evaluate, 5);

            auto res = ga.get_best_individuals(1);
            std::cout << "best one" << std::endl;
            std::cout << res.second[0] << std::endl;
            // std::cout << res.first[0] << std::endl;


            if (best_value == res.second[0])
            {
                cnt_unchanged++;
            }
            else if (cnt_unchanged > 0 && !(best_value == res.second[0]))
            {
                cnt_unchanged = 0;
                ga.set_mutation_rate(mutation_rate);
                ga.set_cross_rate(cross_rate);
            }

            best_value = res.second[0];

            if (cnt_unchanged > 3)
            {
                double new_mutation_rate = std::min(ga.mutation_rate + 0.01, 0.1);
                double new_cross_rate = std::max(ga.cross_rate - 0.1, 0.4);
                ga.set_mutation_rate(new_mutation_rate);
                ga.set_cross_rate(new_cross_rate);
                std::cout << "m" << new_mutation_rate << std::endl;
                std::cout << "c" << new_cross_rate << std::endl;
            }



            //std::cout << res.second[0] << std::endl;

      //      int tmp = res.first[0].size();
            //for (int i = 0; i < tmp/4; i++)
            //{
      //          std::cout << res.first[0][i];
            //}
      //      std::cout << "\n";

      //      for (int i = tmp/4; i < tmp/2; i++)
      //      {
      //          std::cout << res.first[0][i];
      //      }
      //      std::cout << "\n";


      //      for (int i = tmp/2; i < tmp*3 /4; i++)
      //      {
      //          std::cout << res.first[0][i];
      //      }
      //      std::cout << "\n";


      //      for (int i = tmp * 3 / 4; i < tmp; i++)
      //      {
      //          std::cout << res.first[0][i];
      //      }
      //      std::cout << "\n";

        }
        auto res = ga.get_best_individuals(1);
        ui.textBrowser->setText(QString("finished. best scoreは") + QString::number(res.second[0]) + QString("pt.です"));

        std::cout << res.first[0] << std::endl;

        std::cout << "evals: " << evaluate(res.first[0]) << "pts" << std::endl;



        //   /// <summary>
        //   ///
        //   /// </summary>
        //   int tmp = res.first[0].size();
        //   for (int i = 0; i < tmp/4; i++)
        //   {
        //       std::cout << res.first[0][i];
        //   }
        //   std::cout << "\n";

        //   for (int i = tmp/4; i < tmp/2; i++)
        //   {
        //       std::cout << res.first[0][i];
        //   }
        //   std::cout << "\n";


        //   for (int i = tmp/2; i < tmp*3 /4; i++)
        //   {
        //       std::cout << res.first[0][i];
        //   }
        //   std::cout << "\n";


        //   for (int i = tmp * 3 / 4; i < tmp; i++)
        //   {
        //       std::cout << res.first[0][i];
        //   }
        //   std::cout << "\n";
           /////


        std::vector<QString> res_name_vec;
        for (int i = 0; i < days; i++)
        {
            QString name_string = "";
            for (int k = 0; k < people_num; k++)
            {
                if (res.first[0][k * days + i] == 1)
                {
                    name_string += QString(tantou_name_vec[k]) + QString(" / ");
                }
            }

            if (name_string.endsWith(" / "))
            {
                name_string.remove(name_string.size() - 3, 3);
            }
            res_name_vec.emplace_back(name_string);
        }

        int cnt = 0;
        for (int i = 0, s = calendar->rowCount(); i < s; i++)
        {
            auto tmp_val = qobject_cast<QSpinBox*>(calendar->cellWidget(i, 0))->value();

            QString tmp_name = "";

            for (int k = 0; k < tmp_val; k++)
            {
                if (k > 0)
                {
                    tmp_name += QString(", ");
                    //calendar->item(i, 1)->text() += QString(" / ");
                }

                if (res_name_vec[cnt] == "")
                {
                    tmp_name += "Unknown";
                    // calendar->item(i, 1)->text() += "Unknown";
                }
                else
                {
                    tmp_name += res_name_vec[cnt];
                    //calendar->item(i, 1)->text() += res_name_vec[cnt];

                }
                //calendar->item(i, 1)->setText(tmp_name);
                auto pCell = new QTableWidgetItem;
                calendar->setItem(i, 1, pCell);
                pCell->setText(tmp_name);

                cnt++;
            }

        }

    }
}