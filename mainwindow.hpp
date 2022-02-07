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

#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QProgressBar> // Pasek postępu
#include <QMessageBox> // Okna komunikatów
#include <QMainWindow> // Okno główne
#include <QDateTime> // Data i czas
#include <QSettings> // Klasa ustawień
#include <QSpinBox> // Edytor liczby
#include <QThread> // Klasa wątku

#include "workerobject.hpp" // Klasa pracownika
#include "settingsdialog.hpp" // Dialog ustawień

namespace Ui { class MainWindow; }

//! Klasa głównego okna aplikacji.
class MainWindow : public QMainWindow
{

		Q_OBJECT

	private:

		Ui::MainWindow *ui; //!< Interfejs.

		WorkerObject* m_worker; //!< Obiekt pracownika.

		QSpinBox* m_spin; //!< Edytor liczby zadań.
		QProgressBar* m_progress; //!< Pasek postępu.
		QThread* m_thread; //!< Wątek pracownika.

	public:

		//! Konstruktor okna.
		explicit MainWindow(QWidget *parent = nullptr /*!< Rodzic okna. */);

		//! Destruktor okna.
		virtual ~MainWindow(void) override;

	private slots:

		//! Obsługa wygenerowanej wartości.
		void valueReady(int value /*!< [in] Liczba zadań. */);

		void runClicked(void); //!< Wybrano "Start".

		void stopClicked(void); //!< Wybrano "Stop".

		void settingsClicked(void); //!< Wybrano "Opcje".

		void aboutClicked(void); //!< Wybrano "O programie".

		void aboutqtClicked(void); //!< Wybrano "O Qt".

		void progressDone(void); //!< Zakończono obliczenia.

	signals:

		//! Zdarzenie uruchamiające obliczenia.
		void jobRequest(unsigned count /*!< [in] Liczba zadań. */);

};

#endif // MAINWINDOW_HPP
