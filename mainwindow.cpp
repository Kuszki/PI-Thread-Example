/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                                         *
 *  {description}                                                          *
 *  Copyright (C) 2020  Łukasz "Kuszki" Dróżdż  lukasz.kuszki@gmail.com    *
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

#include "mainwindow.hpp"
#include "qaction.h"
#include "qmessagebox.h"
#include "qtextbrowser.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
: QMainWindow(parent) // Konstruktor klasy bazowej
, ui(new Ui::MainWindow) // Stworzenie obiektu interfejsu
{
	m_worker = new WorkerObject(); // Utwórz pracownika
	m_thread = new QThread(this); // Utwórz obiekt wątku

	m_progress = new QProgressBar(this); // Utwórz pasek postępu
	m_spin = new QSpinBox(this); // Utwórz pole edycji liczby zadań

	m_worker->moveToThread(m_thread); // Przenieś pracownika do jego wątku
	m_thread->start(); // Uruchom wątek pracownika (wraz z petlą zdarzeniową)

	m_progress->hide(); // Ukryj pasek postępu

	m_spin->setRange(1, 100); // Ustaw zakres pole edycji
	m_spin->setValue(10); // Ustaw bieżącą wartość
	m_spin->setPrefix(tr("Job count: ")); // Ustaw tekst przed wartością
	m_spin->setLayoutDirection(Qt::LeftToRight); // Wymuś kierunek wyrównania

	ui->setupUi(this); // Zbuduj interfejs z pliku UI

	ui->spinBar->addWidget(m_spin); // Dodaj widget do paska menu

	ui->statusbar->addPermanentWidget(m_progress); // Dodaj widget do paska statusu

	ui->actionStop->setEnabled(false); // Wyłącz akcję "Stop"

	QSettings settings("PI", "Thread"); // Otwarcie pliku z ustawieniami
	WorkerObject::Settings prefs; // Struktura opcji (z wartościami domyślnymi)

	settings.beginGroup("worker"); // Otwórz grupę i wczytaj ustawienia pracownika
	prefs.valueMin = settings.value("valmin", prefs.valueMin).toInt();
	prefs.valueMax = settings.value("valmax", prefs.valueMax).toInt();
	prefs.delayMin = settings.value("delmin", prefs.delayMin).toUInt();
	prefs.delayMax = settings.value("delmax", prefs.delayMax).toUInt();
	settings.endGroup(); // Zamknij grupę ustawień

	m_worker->setSettings(prefs); // Zapisz ustawienia w pracowniku

	settings.beginGroup("window"); // Otwórz grupę i wczytaj ustawienia okna
	restoreState(settings.value("state", saveState()).toByteArray());
	restoreGeometry(settings.value("geometry", saveGeometry()).toByteArray());
	settings.endGroup(); // Zamknij grupę ustawień

	// Uruchamianie obliczeń
	connect(this, &MainWindow::jobRequest,
	        m_worker, &WorkerObject::generateValues);

	// Obsługa wygenerowanej wartości
	connect(m_worker, &WorkerObject::valueReady,
	        this, &MainWindow::valueReady);

	// Obsługa końca obliczeń
	connect(m_worker, &WorkerObject::progressEnd,
	        this, &MainWindow::progressDone);

	// Ustawienie paska postępu
	connect(m_worker, &WorkerObject::progressBegin,
	        m_progress, &QProgressBar::setRange);

	// Wyświetlenie paska postępu
	connect(m_worker, &WorkerObject::progressBegin,
	        m_progress, &QProgressBar::show);

	// Wyświetlanie wartości postępu
	connect(m_worker, &WorkerObject::progressUpdate,
	        m_progress, &QProgressBar::setValue);

	// Ukrywanie paska postępu
	connect(m_worker, &WorkerObject::progressEnd,
	        m_progress, &QProgressBar::hide);

	// Wyświetlanie "O programie"
	connect(ui->actionAbout, &QAction::triggered,
	        this, &MainWindow::aboutClicked);

	// Wyświetlanie "O Qt"
	connect(ui->actionAboutQt, &QAction::triggered,
	        this, &MainWindow::aboutqtClicked);

	// Uruchamianie obliczeń
	connect(ui->actionRun, &QAction::triggered,
	        this, &MainWindow::runClicked);

	// Zatrzymywanie obliczeń
	connect(ui->actionStop, &QAction::triggered,
	        this, &MainWindow::stopClicked);

	// Otwieranie ustawień
	connect(ui->actionSettings, &QAction::triggered,
	        this, &MainWindow::settingsClicked);

	// Czyszczenie okna komunikatów
	connect(ui->actionClear, &QAction::triggered,
	        ui->textBrowser, &QTextBrowser::clear);
}

MainWindow::~MainWindow(void)
{
	QSettings settings("PI", "Thread"); // Otwarcie pliku z ustawieniami
	WorkerObject::Settings prefs = m_worker->getSettings(); // Pobranie ustawień

	settings.beginGroup("worker"); // Zapis ustawień pracownika
	settings.setValue("valmin", prefs.valueMin);
	settings.setValue("valmax", prefs.valueMax);
	settings.setValue("delmin", prefs.delayMin);
	settings.setValue("delmax", prefs.delayMax);
	settings.endGroup(); // Zamknięcie grupy

	settings.beginGroup("window"); // Zapis stanu i rozmiaru okna
	settings.setValue("state", saveState());
	settings.setValue("geometry", saveGeometry());
	settings.endGroup(); // Zamknięcie grupy

	m_worker->terminate(); // Zakończenie obliczeń (jeśli trwają)
	m_worker->deleteLater(); // Usunięcie pracownika

	m_thread->exit(); // Wysłanie informacji o zakończeniu wątku
	m_thread->wait(); // Oczekiwanie aż wątek się skończy

	delete ui; // Usunięcie interfejsu
}

void MainWindow::runClicked(void)
{
	const QString msg = tr("%1 -> Sending job request")
	                    .arg(QTime::currentTime().toString());

	ui->textBrowser->append(msg); // Wyświetlenie wiadomości

	// Ustawienie stanu dostępnych akcji
	ui->actionRun->setEnabled(false); // Blokada
	ui->actionStop->setEnabled(true); // Dostępna
	ui->actionSettings->setEnabled(false); // Blokada

	emit jobRequest(m_spin->value()); // Uruchomienie obliczeń
}

void MainWindow::stopClicked(void)
{
	const QString msg = tr("%1 -> Sending terminate request")
	                    .arg(QTime::currentTime().toString());

	ui->textBrowser->append(msg); // Wyświetlenie wiadomości

	m_worker->terminate(); // Zakończ obliczenia
}

void MainWindow::settingsClicked(void)
{
	// Stwórz dialog z obecnymi ustawieniamu
	SettingsDialog* dialog = new SettingsDialog(
	                              m_worker->getSettings(),
	                              this);

	// Usuń dialog, gdy ten się zakończy
	connect(dialog, &SettingsDialog::finished,
	        dialog, &SettingsDialog::deleteLater);

	// Zaktualizuj zapisane ustawienia
	connect(dialog, &SettingsDialog::valuesAccepted,
	        m_worker, &WorkerObject::setSettings);

	dialog->open(); // Pokaż dialog (modalnie)
}

void MainWindow::aboutClicked(void)
{
	QMessageBox::about(this, tr("About program"),
	                   tr("PI-Thread-Example program. See project repository at "
	                      "<a href=https://github.com/Kuszki/PI-Thread-Example>GitHub</a>."));
}

void MainWindow::aboutqtClicked(void)
{
	QMessageBox::aboutQt(this);
}

void MainWindow::valueReady(int value)
{
	const QString msg = tr("%1 -> Rolled value %2")
	                    .arg(QTime::currentTime().toString())
	                    .arg(value);

	ui->textBrowser->append(msg); // Wyświetlenie wiadomości
}

void MainWindow::progressDone(void)
{
	const QString msg = tr("%1 -> All jobs done")
	                    .arg(QTime::currentTime().toString());

	ui->textBrowser->append(msg); // Wyświetlenie wiadomości

	// Ustawienie stanu dostępnych akcji
	ui->actionRun->setEnabled(true); // Dostępna
	ui->actionStop->setEnabled(false); // Blokada
	ui->actionSettings->setEnabled(true); // Dostępna
}

