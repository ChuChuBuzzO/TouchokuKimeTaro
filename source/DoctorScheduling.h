#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_DoctorScheduling.h"
#include "ui_GA_sub.h"
#include "ui_LicenseWindow.h"
#include "ui_ShowEvalFunc.h"
#include <iomanip>
#include <iostream>
#include <vector>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTableWidget>
#include "dates_func.h"
#include <QDialog>
#include "GA.h"


class DoctorScheduling : public QMainWindow
{
    Q_OBJECT

public:
    explicit DoctorScheduling(QWidget *parent = Q_NULLPTR);

    int ind_num = 2000;
    int gen_num = 2000;
    int elite_num = 1;
    double cross_rate = 0.92;
    double mutation_rate = 0.05;
    int unchangedgen1 = 3;
    double cross_changed = 0.5;
    double mutate_changed = 0.05;
	
    int unchangedgen2 = 200;
    double mutate_massive_rate = 0.8;
    int child_constraint = 2;

private:

    Ui::DoctorSchedulingClass ui;
	
    QTableWidget* calendar;
    QTableWidget* tablekoho;

    double evaluate(const std::vector<int>& vec);

    void buttonAddClicked();
    void buttonDeleteClicked();
    void buttonRunClicked();
    void buttonGASettingClicked();
    void buttonLoadClicked();
    void buttonSaveClicked();
    void buttonExportClicked();
    void buttonWakuCheckClicked();

private slots:
    void show_license();
    void show_evaluates_function();
};

class GA_sub : public QDialog
{
    Q_OBJECT

public:
    explicit GA_sub(DoctorScheduling* parent = Q_NULLPTR);

    Ui::DialogGA ui_sub;

};

class LicenseWindow : public QDialog
{
    Q_OBJECT

public:
    explicit LicenseWindow(DoctorScheduling* parent = Q_NULLPTR);

    Ui::DialogLW ui_license;

};

class ShowEvalFunc : public QDialog
{
    Q_OBJECT

public:
    explicit ShowEvalFunc(DoctorScheduling* parent = Q_NULLPTR);

    Ui::DialogSE ui_evaluates;

};