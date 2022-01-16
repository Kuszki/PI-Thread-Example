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

#include "workerobject.hpp"
#include <QDebug>

WorkerObject::WorkerObject(QObject *parent)
: QObject(parent) // Konstruktor klasy bazowej
, m_mutex(QMutex::Recursive) // Konstruktor muteksa
{
	// Muteks będzie zabezpieczał metody klasy pracownika
	// przed jednoczesnym dostępem z wielu wątków. Sprawi,
	// że obiekt zyska właściwość "Thread-Safe". Rekursywny
	// muteks może być blokowany dowolną liczbę razy z
	// jednego wątku - kolejna próba blokady kończy się
	// powodzeniem, przez co można jednym wątkiem blokować
	// go w różnych miejscach krytycznych.

	// Jeśli zablokujemy muteks, należy go po opuszczeniu
	// sekcji krytycznej odblokować. W innym przypadku
	// dojdzie do blokady innego wątku, który oczekuje
	// na zdjęcie blokady. Aby wygodniej posługiwać się
	// muteksami istnieje klasa pomocnicza QMutexLocker.
	// Objekt tej klasy automatycznie blokuje mutex w
	// konstruktorze i zdejmuje blokadę w destruktorze.

	Settings defaults; // Ustawienia domyślne

	m_minSleep = defaults.delayMin;
	m_maxSleep = defaults.delayMax;

	m_minValue = defaults.valueMin;
	m_maxValue = defaults.valueMin;
}

WorkerObject::~WorkerObject(void) {}

bool WorkerObject::tryLock(void)
{
	QMutexLocker locker(&m_mutex); // Zablokuj mutex // Zablokuj mutex

	// Gdy jest aktywne zadanie - niepowodzenie
	if (m_isJobActive) return false;

	m_isTerminated = false; // Oczyść flagę zakończenia
	m_isJobActive = true; // Ustaw flagę aktywności

	return true; // Zwróć powodzenie operacji.
}

void WorkerObject::unlock(void)
{
	QMutexLocker locker(&m_mutex); // Zablokuj mutex

	m_isTerminated = false; // Oczyść flagę zakończenia
	m_isJobActive = false; // Oczyść flagę aktywności
}

WorkerObject::Settings WorkerObject::getSettings(void) const
{
	QMutexLocker locker(&m_mutex); // Zablokuj mutex

	// Zwróć aktualne wartości ustawień
	return
	{
		m_minValue,
		m_maxValue,
		m_minSleep,
		m_maxSleep
	};
}

unsigned WorkerObject::minSleep(void) const
{
	QMutexLocker locker(&m_mutex); // Zablokuj mutex

	return m_minSleep; // Zwróć wartość ustawienia
}

void WorkerObject::setMinSleep(unsigned newMinSleep)
{
	QMutexLocker locker(&m_mutex); // Zablokuj mutex

	// Gdy ustawienia są poprawne - zapisz je
	if (m_maxSleep > newMinSleep)
		m_minSleep = newMinSleep;
}

unsigned WorkerObject::maxSleep(void) const
{
	QMutexLocker locker(&m_mutex); // Zablokuj mutex

	return m_maxSleep; // Zwróć wartość ustawienia
}

int WorkerObject::getValue(void)
{
	QMutexLocker locker(&m_mutex); // Zablokuj mutex

	// Zwróć wylosowaną wartość z zadanego zakresu
	return m_gen.bounded(m_minValue, m_maxValue+1);
}

void WorkerObject::setMaxSleep(unsigned newMaxSleep)
{
	QMutexLocker locker(&m_mutex); // Zablokuj mutex

	// Gdy ustawienia są poprawne - zapisz je
	if (m_minSleep < newMaxSleep)
		m_maxSleep = newMaxSleep;
}

void WorkerObject::setSleepRange(unsigned newMinSleep,
                                 unsigned newMaxSleep)
{
	QMutexLocker locker(&m_mutex); // Zablokuj mutex

	// Gdy ustawienia są poprawne - zapisz je
	if (newMinSleep < newMaxSleep)
	{
		m_minSleep = newMinSleep;
		m_maxSleep = newMaxSleep;
	}
}

void WorkerObject::setRollRange(int newMinValue, int newMaxValue)
{
	QMutexLocker locker(&m_mutex); // Zablokuj mutex

	// Gdy ustawienia są poprawne - zapisz je
	if (newMinValue < newMaxValue)
	{
		m_minValue = newMinValue;
		m_maxValue = newMaxValue;
	}
}

void WorkerObject::setSettings(const Settings& settings)
{
	QMutexLocker locker(&m_mutex); // Zablokuj mutex

	// Zaktualizuj ustawienia, jeśli te są poprawne
	setRollRange(settings.valueMin, settings.valueMax);
	setSleepRange(settings.delayMin, settings.delayMax);
}

void WorkerObject::generateValues(unsigned count)
{
	if (!tryLock()) return; // Uzyskaj blokadę.

	// Pobierz wątek, w którym uruchomiono metodę
	QThread* currThread = QThread::currentThread();
	QRandomGenerator rand; // Generator opóźnienia

	emit progressBegin(0, count); // Informuj o początku obliczeń
	emit progressUpdate(0); // Ustaw zerowy postęp obliczeń

	// Wykonaj zadaną liczbę obliczeń
	for (int i = 1; i <= count; ++i)
	{
		// Wylosuj opóżnienie za pomocą pomocniczego generatora
		const int sleep = rand.bounded(m_minSleep, m_maxSleep);
		const int n = getValue(); // Losuj wartość

		currThread->msleep(sleep); // Opóźnij wątek

		emit progressUpdate(i); // Aktualizuj postęp
		emit valueReady(n); // Przekaż wygenerowaną wartość

		if (isTerminated()) break; // Zakończ obliczenia, jeśli potrzeba
	}

	this->unlock(); // Odblokuj możliwość wykonania obliczeń

	emit progressEnd(); // Poinformuj o końcu obliczeń
}

void WorkerObject::terminate(void)
{
	QMutexLocker locker(&m_mutex); // Zablokuj mutex

	// Ustaw flagę zakończenia, jeśli trwają obliczenia
	if (m_isJobActive) m_isTerminated = true;
}

int WorkerObject::minValue(void) const
{
	QMutexLocker locker(&m_mutex); // Zablokuj mutex

	return m_minValue; // Zwróć wartość ustawienia
}

void WorkerObject::setMinValue(int newMinValue)
{
	QMutexLocker locker(&m_mutex); // Zablokuj mutex

	// Gdy ustawienia są poprawne - zapisz je
	if (m_maxValue > newMinValue)
		m_minValue = newMinValue;
}

int WorkerObject::maxValue(void) const
{
	QMutexLocker locker(&m_mutex); // Zablokuj mutex

	return m_maxValue; // Zwróć wartość ustawienia
}

void WorkerObject::setMaxValue(int newMaxValue)
{
	QMutexLocker locker(&m_mutex); // Zablokuj mutex

	// Gdy ustawienia są poprawne - zapisz je
	if (m_minValue < newMaxValue)
		m_maxValue = newMaxValue;
}

bool WorkerObject::isTerminated(void) const
{
	QMutexLocker locker(&m_mutex); // Zablokuj mutex

	return m_isTerminated; // Zwróć wartość flagi
}

bool WorkerObject::isJobActive(void) const
{
	QMutexLocker locker(&m_mutex); // Zablokuj mutex

	return m_isJobActive; // Zwróć wartość flagi
}
