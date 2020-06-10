/*
 * This file is part of the SmuView project.
 *
 * Copyright (C) 2020 Frank Stettner <frank-stettner@gmx.net>
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

#ifndef SETTINGSMANAGER_HPP
#define SETTINGSMANAGER_HPP

#include <memory>

#include <QSettings>
#include <QString>

using std::shared_ptr;

namespace sv {

class Session;

namespace channels {
class BaseChannel;
}
namespace data {
namespace properties {
class BaseProperty;
}
class BaseSignal;
}
namespace devices {
class BaseDevice;
class Configurable;
}

class SettingsManager
{

public:
	SettingsManager();

	/**
	 * Check if a setting for this device exists.
	 *
	 * @param[in] device The device.
	 *
	 * @return true if a setting exists.
	 */
	static bool has_device_settings(shared_ptr<sv::devices::BaseDevice> device);

	/**
	 * Save the configurable to the settings.
	 *
	 * @param[in] configurable The configurable to save.
	 * @param[in] settings The settings.
	 * @param[in] key_prefix The prefix of the settings keys for the configurable.
	 */
	static void save_configurable(
		const shared_ptr<sv::devices::Configurable> &configurable,
		QSettings &settings, const QString &key_prefix = "");

	/**
	 * Save the channel to the settings.
	 *
	 * @param[in] channel The channel to save.
	 * @param[in] settings The settings.
	 * @param[in] key_prefix The prefix of the settings keys for the channel.
	 */
	static void save_channel(
		const shared_ptr<sv::channels::BaseChannel> &channel,
		QSettings &settings, const QString &key_prefix = "");

	/**
	 * Save the signal to the settings.
	 *
	 * @param[in] signal The signal to save.
	 * @param[in] settings The settings.
	 * @param[in] key_prefix The prefix of the settings keys for the signal.
	 */
	static void save_signal(
		const shared_ptr<sv::data::BaseSignal> &signal,
		QSettings &settings, const QString &key_prefix = "");

	/**
	 * Save the property to the settings.
	 *
	 * @param[in] property The property to save.
	 * @param[in] settings The settings.
	 * @param[in] key_prefix The prefix of the settings keys for the property.
	 */
	static void save_property(
		const shared_ptr<sv::data::properties::BaseProperty> &property,
		QSettings &settings, const QString &key_prefix = "");

	/**
	 * Restore a device from the settings.
	 *
	 * @param[in] session The SmuView session.
	 * @param[in] settings The settings.
	 * @param[in] key_prefix The prefix of the settings keys for the device.
	 *
	 * @return The restored device.
	 */
	static shared_ptr<sv::devices::BaseDevice> restore_device(
		Session &session, QSettings &settings, QString key_prefix = "");

	/**
	 * Restore a configurable from the settings.
	 *
	 * @param[in] session The SmuView session.
	 * @param[in] settings The settings.
	 * @param[in] key_prefix The prefix of the settings keys for the configurable.
	 *
	 * @return The restored configurable.
	 */
	static shared_ptr<sv::devices::Configurable> restore_configurable(
		Session &session, QSettings &settings, QString key_prefix = "");

	/**
	 * Restore a property from the settings.
	 *
	 * @param[in] session The SmuView session.
	 * @param[in] settings The settings.
	 * @param[in] key_prefix The prefix of the settings keys for the property.
	 *
	 * @return The restored property.
	 */
	static shared_ptr<sv::data::properties::BaseProperty> restore_property(
		Session &session, QSettings &settings, const QString &key_prefix = "");

	/**
	 * Restore a channel from the settings.
	 *
	 * @param[in] session The SmuView session.
	 * @param[in] settings The settings.
	 * @param[in] key_prefix The prefix of the settings keys for the channel.
	 *
	 * @return The restored channel.
	 */
	static shared_ptr<sv::channels::BaseChannel> restore_channel(
		Session &session, QSettings &settings, const QString &key_prefix = "");

	/**
	 * Restore a signal from the settings.
	 *
	 * @param[in] session The SmuView session.
	 * @param[in] settings The settings.
	 * @param[in] key_prefix The prefix of the settings keys for the signal.
	 *
	 * @return The restored signal.
	 */
	static shared_ptr<sv::data::BaseSignal> restore_signal(
		Session &session, QSettings &settings, const QString &key_prefix = "");

};

} // namespace sv

#endif // SETTINGSMANAGER_HPP
