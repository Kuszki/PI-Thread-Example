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

#ifndef WORKEROBJECT_HPP
#define WORKEROBJECT_HPP

#include <QRandomGenerator> // Liczby pseudolosowe
#include <QObject> // Baza obiektu
#include <QThread> // Klasa wątku
#include <QMutex> // Klasa mutexa (semafor binarny)

//! Klasa robotnika.
class WorkerObject : public QObject
{

		Q_OBJECT

	//! Struktura ustawień.
	public: struct Settings
	{
		int valueMin = 1; //!< Minimalna losowana wartość.
		int valueMax = 6; //!< Maksymalna losowana wartość.

		unsigned delayMin = 500; //!< Minimalne opóźnienie.
		unsigned delayMax = 1500; //!< Maksymalne opóźnienie.
	};

	protected:

		mutable QMutex m_mutex; //!< Muteks do synchronizacji.

		QRandomGenerator m_gen; //!< Generator liczb pseudolosowych.

		unsigned m_minSleep; //!< Minimalna losowana wartość.
		unsigned m_maxSleep; //!< Maksymalna losowana wartość.

		int m_minValue; //!< Minimalne opóźnienie.
		int m_maxValue; //!< Maksymalne opóźnienie.

		bool m_isTerminated = false; //!< Flaga zakończenia obliczeń.
		bool m_isJobActive = false; //!< Flaga obliczeń w toku.

		bool tryLock(void); //!< Próba rozpoczęcia obliczeń.
		void unlock(void); //!< Zakończenie obliczeń.

	public:

		//! Konstruktor pracownika.
		explicit WorkerObject(QObject *parent = nullptr /*!< [in] Rodzic pracownika. */);

		virtual ~WorkerObject(void) override; //!< Destruktor pracownika.

		Settings getSettings(void) const; //!< Pobranie aktualnych ustawień.

		unsigned minSleep(void) const; //!< Pobranie aktualnej minimalnej wartości opóźnienia.
		unsigned maxSleep(void) const; //!< Pobranie aktualnej maksymalnej wartości opóźnienia.

		int minValue(void) const; //!< Pobranie aktualnej minimalnej losowanej wartości.
		int maxValue(void) const; //!< Pobranie aktualnej maksymalnej losowanej wartości.

		bool isTerminated(void) const; //!< Pobranie flagi zakończenia obliczeń.
		bool isJobActive(void) const; //!< Pobranie flagi trwania obliczeń.

		int getValue(void); //!< Pobranie pojedynczej losowanej wartości.

	public slots:

		//! Ustalenie nowej wartości minimalnej losowania.
		void setMinSleep(unsigned newMinSleep /*!< [in] Nowa wartość. */);

		//! Ustalenie nowej wartości maksymalnej losowania.
		void setMaxSleep(unsigned newMaxSleep /*!< [in] Nowa wartość. */);

		//! Ustalenie nowej wartości minimalnej opóźnienia.
		void setMinValue(int newMinValue /*!< [in] Nowa wartość. */);

		//! Ustalenie nowej wartości maksymalnej opóźnienia.
		void setMaxValue(int newMaxValue /*!< [in] Nowa wartość. */);

		//! Ustalenie przedziału losowanych wartości.
		void setSleepRange(unsigned newMinSleep, /*!< [in] Nowa wartość minimalna. */
		                   unsigned newMaxSleep /*!< [in] Nowa wartość maksymalna. */);

		//! Ustalenie przedziału opóźnienia.
		void setRollRange(int newMinValue, /*!< [in] Nowa wartość minimalna. */
		                  int newMaxValue /*!< [in] Nowa wartość maksymalna. */);

		//! Ustawienie nowych wartości ustawień.
		void setSettings(const WorkerObject::Settings& settings /*!< [in] Nowe ustawienia. */);

		//! Zlecenie wykonania obliczeń.
		void generateValues(unsigned count /*!< [in] Liczba zadań. */);

		void terminate(void); //!< Zakończenie obliczeń.

	signals:

		//! Informacja o wygenerowaniu wartości.
		void valueReady(int value /*!< [in] Wartość. */);

		//! Informacja o rozpoczęciu obliczeń.
		void progressBegin(int start, /*!< [in] Wartość początkowa postępu. */
		                   int stop /*!< [in] Wartość końcowa postępu. */);

		//! Informacja o postępie obliczeń.
		void progressUpdate(int step /*!< [in] Wartość postępu. */);

		void progressEnd(void); //!< Informacja o zakończeniu obliczeń.

};

#endif // WORKEROBJECT_HPP
