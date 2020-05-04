/*
 * This file is part of the SmuView project.
 *
 * Copyright (C) 2018-2020 Frank Stettner <frank-stettner@gmx.net>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef UI_TABS_WELCOMETAB_HPP
#define UI_TABS_WELCOMETAB_HPP

#include <QString>
#include <QWidget>

#include "src/ui/tabs/basetab.hpp"

namespace sv {

class Session;

namespace ui {
namespace tabs {

class WelcomeTab : public BaseTab
{
	Q_OBJECT

private:

public:
	WelcomeTab(Session &session, QWidget *parent = nullptr);

	string tab_id() override;
	QString tab_title() override;
	/** The WelcomeTab can always be closed */
	bool request_close() override;

	// TODO: scope
	void save_settings() const override;
	void restore_settings() override;

private:
	void setup_ui();

};

} // namespace tabs
} // namespace ui
} // namespace sv

#endif // UI_TABS_WELCOMETAB_HPP
