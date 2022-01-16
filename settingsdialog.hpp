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

#ifndef SETTINGSDIALOG_HPP
#define SETTINGSDIALOG_HPP

#include <QDialogButtonBox> // Pole przycisków
#include <QPushButton> // Przycisk
#include <QDialog> // Baza dialogu

#include "workerobject.hpp" // Klasa pracownika

namespace Ui { class SettingsDialog; }

//! Klasa dialogu ustawień.
class SettingsDialog : public QDialog
{

		Q_OBJECT

	private:

		Ui::SettingsDialog *ui; //!< Interfejs.

		WorkerObject::Settings m_saved; //!< Pierwotne ustawienia.

	public:

		//! Konstruktor dialogu.
		explicit SettingsDialog(
		          const WorkerObject::Settings& values = WorkerObject::Settings(), /*!< [in] Pierwotne ustawienia. */
		          QWidget *parent = nullptr /*!< [in] Rodzic dielogu. */);

		virtual ~SettingsDialog(void) override; //!< Destruktor dialogu.

		WorkerObject::Settings getValues(void) const; //!< Pobranie aktualnych ustawień.

	public slots:

		virtual void accept(void) override; //!< Obsługa akceptacji dialogu.

		void applyValues(void); //!< Obsługa akcji "Zastosuj".
		void resetValues(void); //!< Reset do wartości pierwotnych.
		void defaultValues(void); //!< Przywrócenie wartości domyślnych.

		//! Ustawienie wybranych wartości.
		void setValues(const WorkerObject::Settings& values /*!< [in] Nowe wartości. */);

	private slots:

		void valuesUpdate(void); //!< Obsługa aktualizacji ustawień losowanej wartości.
		void delayUpdate(void); //!< Obsługa aktualizacji ustawień opóźnienia.

	signals:

		//! Zdarzenie związane z akceptacją nowych wartości ustawień.
		void valuesAccepted(const WorkerObject::Settings& values /*!< [in] Wybrane ustawienia. */);

};

#endif // SETTINGSDIALOG_HPP
