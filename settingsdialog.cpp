/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                                         *
 *  {description}                                                          *
 *  Copyright (C) 2022  Łukasz "Kuszki" Dróżdż  lukasz.kuszki@gmail.com    *
 *                                                                         *
 *  This program is free software: you can redistribute it and/or modify   *
 *  it under the terms of the GNU General Public License as published by   *
 *  the  Free Software Foundation, either  version 3 of the  License, or   *
 *  (at your option) any later version.                                    *
 *                                                                         *
 *  This  program  is  distributed  in the hope  that it will be useful,   *
 *  but WITHOUT ANY  WARRANTY;  without  even  the  implied  warranty of   *
 *  MERCHANTABILITY  or  FITNESS  FOR  A  PARTICULAR  PURPOSE.  See  the   *
 *  GNU General Public License for more details.                           *
 *                                                                         *
 *  You should have  received a copy  of the  GNU General Public License   *
 *  along with this program. If not, see http://www.gnu.org/licenses/.     *
 *                                                                         *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "settingsdialog.hpp"
#include "ui_settingsdialog.h"

SettingsDialog::SettingsDialog(const WorkerObject::Settings& values, QWidget *parent)
: QDialog(parent) // Konstruktor klasy bazowej
, ui(new Ui::SettingsDialog) // Stworzenie obiektu interfejsu
, m_saved(values) // Zapisanie wartości pierwotnych ustawień
{
	ui->setupUi(this); // Zbuduj interfejs z pliku UI

	// Obsługa aktualizacji ustawień losowanej wartości
	connect(ui->minvalSpin, qOverload<int>(&QSpinBox::valueChanged),
	        this, &SettingsDialog::valuesUpdate);

	// Obsługa aktualizacji ustawień losowanej wartości
	connect(ui->maxvalSpin, qOverload<int>(&QSpinBox::valueChanged),
	        this, &SettingsDialog::valuesUpdate);

	// Obsługa aktualizacji ustawień opóźnienia
	connect(ui->mindelSpin, qOverload<int>(&QSpinBox::valueChanged),
	        this, &SettingsDialog::delayUpdate);

	// Obsługa aktualizacji ustawień opóźnienia
	connect(ui->maxdelSpin, qOverload<int>(&QSpinBox::valueChanged),
	        this, &SettingsDialog::delayUpdate);

	// Obsługa resetowania ustawień
	connect(ui->buttonBox->button(QDialogButtonBox::Reset), &QPushButton::clicked,
	        this, &SettingsDialog::resetValues);

	// Obsługa akcji "Zastosuj"
	connect(ui->buttonBox->button(QDialogButtonBox::Apply), &QPushButton::clicked,
	        this, &SettingsDialog::applyValues);

	// Obsługa akcji ustawień domyślnych
	connect(ui->buttonBox->button(QDialogButtonBox::RestoreDefaults), &QPushButton::clicked,
	        this, &SettingsDialog::defaultValues);

	setValues(values); // Ustawienie pierwotnych wartości ustawień
}

SettingsDialog::~SettingsDialog(void)
{
	delete ui; // Usunięcie interfejsu
}

WorkerObject::Settings SettingsDialog::getValues(void) const
{
	// Zwrócenie struktury ustawień
	return
	{
		ui->minvalSpin->value(),
		ui->maxvalSpin->value(),
		unsigned(ui->mindelSpin->value()),
		unsigned(ui->maxdelSpin->value())
	};
}

void SettingsDialog::accept(void)
{
	QDialog::accept(); // Domyślne działanie akceptacji dialogu

	emit valuesAccepted(getValues()); // Sygnał nowych ustawień
}

void SettingsDialog::applyValues(void)
{
	emit valuesAccepted(getValues()); // Sygnał nowych ustawień
}

void SettingsDialog::resetValues(void)
{
	setValues(m_saved); // Ustawienie pierwotnych ustawień
}

void SettingsDialog::defaultValues(void)
{
	setValues(WorkerObject::Settings()); // Ustawienie domyślnych ustawień
}

void SettingsDialog::setValues(const WorkerObject::Settings& values)
{
	// Jeśli ustawienia losowania są poprawne - zapisz je
	if (values.valueMin < values.valueMax)
	{
		ui->minvalSpin->setMaximum(values.valueMax-1);
		ui->maxvalSpin->setMinimum(values.valueMin+1);

		ui->minvalSpin->setValue(values.valueMin);
		ui->maxvalSpin->setValue(values.valueMax);
	}

	// Jeśli ustawienia opóźnienia są poprawne - zapisz je
	if (values.delayMin <= values.delayMax)
	{
		ui->mindelSpin->setMaximum(values.delayMax);
		ui->maxdelSpin->setMinimum(values.delayMin);

		ui->mindelSpin->setValue(values.delayMin);
		ui->maxdelSpin->setValue(values.delayMax);
	}
}

void SettingsDialog::valuesUpdate(void)
{
	// Zaktualizuj wartości brzegowe na podstawie aktualnych wartości
	ui->minvalSpin->setMaximum(ui->maxvalSpin->value()-1);
	ui->maxvalSpin->setMinimum(ui->minvalSpin->value()+1);
}

void SettingsDialog::delayUpdate(void)
{
	// Zaktualizuj wartości brzegowe na podstawie aktualnych wartości
	ui->mindelSpin->setMaximum(ui->maxdelSpin->value());
	ui->maxdelSpin->setMinimum(ui->mindelSpin->value());
}
