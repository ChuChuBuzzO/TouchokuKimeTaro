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
#include <QIcon>
#include <QMessageBox>

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
    	} else if (weekday == 1)
        {
            auto str = new QTableWidgetItem(QString::number(i + 1) + QString(" (月)"));
            sp->setValue(1);
            table->setVerticalHeaderItem(i, str);
            table->setCellWidget(i, 0, sp);
    	} else if (weekday == 2)
    	{
            auto str = new QTableWidgetItem(QString::number(i + 1) + QString(" (火)"));
            sp->setValue(1);
            table->setVerticalHeaderItem(i, str);
            table->setCellWidget(i, 0, sp);
    	} else if (weekday == 3)
    	{
            auto str = new QTableWidgetItem(QString::number(i + 1) + QString(" (水)"));
            sp->setValue(1);
            table->setVerticalHeaderItem(i, str);
            table->setCellWidget(i, 0, sp);
    	} else if (weekday == 4)
    	{
            auto str = new QTableWidgetItem(QString::number(i + 1) + QString(" (木)"));
            sp->setValue(1);
            table->setVerticalHeaderItem(i, str);
            table->setCellWidget(i, 0, sp);
    	} else if (weekday == 5)
    	{
            auto str = new QTableWidgetItem(QString::number(i + 1) + QString(" (金)"));
            sp->setValue(1);
            table->setVerticalHeaderItem(i, str);
            table->setCellWidget(i, 0, sp);
    	} else
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
DoctorScheduling::DoctorScheduling(QWidget *parent)
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
    connect(ui.pushButtonWakuCheck, &QPushButton::clicked, this, &DoctorScheduling::buttonWakuCheckClicked);
    connect(ui.actionlicense, SIGNAL(triggered()), this, SLOT(show_license()));
    connect(ui.actionevaluate, SIGNAL(triggered()), this, SLOT(show_evaluates_function()));
}

LicenseWindow::LicenseWindow(DoctorScheduling* parent)
{
    ui_license.setupUi(this);
    ui_license.labelLink->setTextInteractionFlags(Qt::LinksAccessibleByMouse);
    ui_license.labelLink->setOpenExternalLinks(true);
    ui_license.labelLink->setTextFormat(Qt::RichText);
	ui_license.labelLink->setText("<a href=\"https://github.com/ChuChuBuzzO/TouchokuKimeTaro\">https://github.com/ChuChuBuzzO/TouchokuKimeTaro</a>");
}

ShowEvalFunc::ShowEvalFunc(DoctorScheduling* parent)
{
    ui_evaluates.setupUi(this);
}

void DoctorScheduling::show_license()
{
    auto lic_sub = new LicenseWindow(this);
    lic_sub->setWindowTitle("ライセンスについて");
    lic_sub->exec();
}

void DoctorScheduling::show_evaluates_function()
{
    auto eval_sub = new ShowEvalFunc(this);
    eval_sub->setWindowTitle("評価関数について");
    eval_sub->exec();
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

    ui_sub.spinBoxUnchangedGen1->setValue(parent->unchangedgen1);
    ui_sub.doubleSpinBoxCrossChanged->setValue(parent->cross_changed);
    ui_sub.doubleSpinBoxMutationChanged->setValue(parent->mutate_changed);

    ui_sub.spinBoxUnchaigedGen2->setValue(parent->unchangedgen2);
    ui_sub.doubleSpinBoxMassiveMutateRate->setValue(parent->mutate_massive_rate);

    ui_sub.spinBoxChildConstraint->setValue(parent->child_constraint);
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

        unchangedgen1 = ga_sub->ui_sub.spinBoxUnchangedGen1->value();
        cross_changed = ga_sub->ui_sub.doubleSpinBoxCrossChanged->value();
        mutate_changed = ga_sub->ui_sub.doubleSpinBoxMutationChanged->value();

        unchangedgen2 = ga_sub->ui_sub.spinBoxUnchaigedGen2->value();
        mutate_massive_rate = ga_sub->ui_sub.doubleSpinBoxMassiveMutateRate->value();

        child_constraint = ga_sub->ui_sub.spinBoxChildConstraint->value();
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
        ui.textBrowser->setText("Error! File can not be opened");
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
        ofs << "{" << qobject_cast<QSpinBox*>(calendar->cellWidget(index, 0))->value() << "}";
        ofs << "," << calendar->item(index, 1)->text().trimmed().toLocal8Bit().toStdString() << std::endl;
        index++;
    }
}

void DoctorScheduling::buttonWakuCheckClicked()
{
    int days = 0;
    for (int i = 0, s = tablekoho->rowCount(); i < s; i++)
    {
        auto name_item = tablekoho->item(i, 0);
        if (name_item)  // 空の名前行は無視
        {
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
            days += day_num;
        }
    }
	
    ui.labelhopedWaku->setText(QString::number(days));


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

    ui.labelRequiredWaku->setText(QString::number(row_indices.size()));
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
		for(int i = 0, s = calendar->rowCount(); i < s; i++)
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
            } else
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

    /// 設定値の取得
    int vacant_days = ui.spinBoxVac->value();
    double penalty0 = ui.spinBoxPenalty0->value();
    double penalty1 = ui.spinBoxPenalty1->value();
    double penalty2 = ui.spinBoxPenalty2->value();

    ui.textBrowser->setText(log_string);


	if (std::accumulate(tantou_days_vec.begin(), tantou_days_vec.end(), 0) != static_cast<int>(row_indices.size()))
	{
        check_before_run = false;
        log_string += "希望枠数の合計と、総計枠数を同じ値にしてください";
	}
    	// 以下でGA実行	
    if (check_before_run)
    {
        // まず、日数 * 人数分の要素を持つvectorをひとつの個体とする。
        // 要素数は
        int individual_length = row_indices.size();

        // std::cout << individual_num << std::endl;

        auto ga = GAIntWithoutDup(ind_num, individual_length);

        ga.set_individuals_random(row_indices);
        ga.set_cross_rate(cross_rate);
        ga.set_mutation_rate(mutation_rate);
        ga.set_elite_num(elite_num);
    	
        // 評価関数(どうしてもvectorを引数で持つ関数で渡さないといけないのでやや工夫が必要になる。ラムダ式でいけるはず)
        // std::cout << row_indices << std::endl;
        auto evaluate = [&](const std::vector<int>& vec)
        {
            double penalty_value = 0.0; // penaltyの値なので最後に逆数にする必要あり。

            int index_saved = 0;
            int tantou_id = 0;
        	for (auto waku_num : tantou_days_vec)
        	{
                // 先に部分列を得る
                std::vector<int> partial_vec;
        		for (int i = 0; i < waku_num; i++)
        		{
                    partial_vec.emplace_back(vec[index_saved + i]);
        		}
                index_saved += waku_num;  // 先に足しとく
        		

        		// partial_vecをsortする(昇順)。
                std::sort(partial_vec.begin(), partial_vec.end());

                int partial_length = partial_vec.size();

        		// n日以内に2回あればpenalty
        		if (partial_length > 1)
        		{
        			for (int j = 1; j < partial_length; j++)
        			{
                        int d = partial_vec[j] - partial_vec[j - 1];
        				if (d < vacant_days)
        				{
                            penalty_value += (vacant_days - d) * penalty0;
        				}
        			}        			
        		}

        		// 希望日になければpenalty
                if (tantou_hope_vec[tantou_id].size() > 0)  // そもそも希望日なければ考えない。
                {
                    for (auto day : partial_vec)
                    {
                        if (std::find(tantou_hope_vec[tantou_id].begin(), tantou_hope_vec[tantou_id].end(), day) == tantou_hope_vec[tantou_id].end())
                        {
                            penalty_value += penalty1;
                        }
                    }
                }
        		// 不可日にあたればpenalty
                for (auto day : partial_vec)
                {
                    if (std::find(tantou_deny_vec[tantou_id].begin(), tantou_deny_vec[tantou_id].end(), day) != tantou_deny_vec[tantou_id].end())
                    {
                        penalty_value += penalty2;
                    }
                }
                // もし同じ枠に偶然同じ人があたれば絶大なpenalty(既にsortされているのでそのままuniqueで良い)
                if (std::unique(std::begin(partial_vec), std::end(partial_vec)) != std::end(partial_vec))
                {
                    penalty_value += 10000;
                }
        		
                tantou_id++;
        	}        	
            return 1 / (penalty_value + 0.000001);
        };

        double best_value = 0.0;
        int cnt_unchanged = 0;
    	for (int i = 0; i < gen_num; i++)
    	{
            //std::cout << "-------" << i << " generation -------" << std::endl;
            // std::cout << "\r----------" << i << " generation ----------";
            ui.progressBar->setValue(i * 100 / gen_num);
    		
            ga.process_generation(evaluate, child_constraint);
            
            auto res = ga.get_best_individuals(1);
            ui.textBrowser->setText("generation " + QString::number(i) + " best score: " + QString::number(res.second[0]));
          	// std::cout << "best score: " << res.second[0] << std::endl;  // 出力の長さが変わる可能性あるため' 'で埋めとかないと変な値になり得る。
            // std::cout << res.first[0] << std::endl;

    		if (best_value == res.second[0])
    		{
                cnt_unchanged++;
    		} else if (cnt_unchanged > 0 && !(best_value == res.second[0]))
    		{
                cnt_unchanged = 0;
                ga.set_mutation_rate(mutation_rate);
                ga.set_cross_rate(cross_rate);
    		}
    		
            best_value = res.second[0];

    		if (cnt_unchanged > unchangedgen2)
    		{
    			// 大変異を起こす
                ga.set_mutation_rate(0.8);
                ga.set_cross_rate(0.01);
                // std::cout << "Major Mutation occurs!! " << std::endl;
                ui.textBrowser->append(" Massive Mutation occurs!!");
    		}
    		else if (cnt_unchanged > unchangedgen1)
    		{
                double new_mutation_rate = std::min(ga.mutation_rate + (mutate_changed - ga.mutation_rate) / 10, mutate_changed);
                double new_cross_rate = std::max(ga.cross_rate + (cross_changed - ga.cross_rate) / 10, cross_changed);
                ga.set_mutation_rate(new_mutation_rate);
                ga.set_cross_rate(new_cross_rate);
                // std::cout << "m" << new_mutation_rate << std::endl;
                // std::cout << "c" << new_cross_rate << std::endl;
    		}

    		
    	}
        auto res = ga.get_best_individuals(1);
        ui.textBrowser->setText(QString("finished. best scoreは") + QString::number(res.second[0]) + QString("pt.です"));

        // std::cout << "\n" << "best individual: " << res.first[0] << std::endl;

    	// もうraw_indicesいらんので加工させてもらう。calendar上に結果を表示。
        row_indices.erase(std::unique(row_indices.begin(), row_indices.end()), row_indices.end()); // これでrow_indicesは{1, 2, 3, 4, 5, 6...}みたいになった
        std::vector<QString> name_days(row_indices.size(), "");
        int name_index = 0;
        int index = 0;
        for (auto waku_days : tantou_days_vec)
        {
        	for (int i = 0; i < waku_days; i++)
        	{
                int day = res.first[0][index + i];
                 name_days[day - 1] += tantou_name_vec[name_index];
                 name_days[day - 1] += " ";
        	}
            index += waku_days;
            name_index++;
        }

        for (auto day : row_indices)
        {
            auto pCell = new QTableWidgetItem;
            calendar->setItem(day - 1, 1, pCell);
            pCell->setText(name_days[day - 1]);
        }

    	// 最後に、どこが希望に沿えていて、どこが希望に沿えなかったかも出力。これまでと重複しているが、分かりにくいので、もう1回やっちゃう。
    	// 1人ずつ確認
    	QMessageBox msgBox(this);
        msgBox.setWindowTitle("result");
        QString result_log = "";
        int zurasi = 0;
    	for (int i = 0, s = tantou_name_vec.size(); i < s; i++)
    	{
    		for (int j = 0; j < tantou_days_vec[i]; j++)
    		{
    			// その人の入る日が希望日に入っているか。また不可日に当たっていないか。
                int d = res.first[0][zurasi + j];

    			if (!(tantou_hope_vec[i].empty()))
    			{
                    if (std::find(tantou_hope_vec[i].begin(), tantou_hope_vec[i].end(), d) == tantou_hope_vec[i].end())
                    {
                        //std::cout << tantou_name_vec[i].toLocal8Bit().toStdString() << " wasn't assigned to the desired day: " << d << std::endl;
                        //ui.textBrowser->append(tantou_name_vec[i] + " wasn't assigned to the desired day: " + QString::number(d));
                        QString tmp = tantou_name_vec[i] + " wasn't assigned to the desired day: " + QString::number(d) + "    \n";
                        result_log += tmp;
                    }
    			}

                if (std::find(tantou_deny_vec[i].begin(), tantou_deny_vec[i].end(), d) != tantou_deny_vec[i].end())
                {
                    //std::cout << tantou_name_vec[i].toLocal8Bit().toStdString() << " was assigned to the impossible day" << d << "!!!!" << std::endl;
                    //ui.textBrowser->append(tantou_name_vec[i] + " was assigned to the impossible day: " + QString::number(d) + "!!!!");
                    QString tmp = tantou_name_vec[i] + " was assigned to the impossible day: " + QString::number(d) + "!!!!    \n";
                    result_log += tmp;
                }
    		}
            zurasi += tantou_days_vec[i];
    	}
        msgBox.setText(result_log);
        msgBox.exec();
	}
}