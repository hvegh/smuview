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

#include <QWidget>

#include "usertab.hpp"
#include "src/session.hpp"
#include "src/devices/userdevice.hpp"
#include "src/ui/tabs/devicetab.hpp"

namespace sv {
namespace ui {
namespace tabs {

UserTab::UserTab(Session &session,
		shared_ptr<sv::devices::UserDevice> device, QWidget *parent) :
	DeviceTab(session, device, parent)
{
}

void UserTab::restore_settings()
{
}

void UserTab::save_settings() const
{
}

} // namespace tabs
} // namespace ui
} // namespace sv
