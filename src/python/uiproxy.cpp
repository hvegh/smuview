/*
 * This file is part of the SmuView project.
 *
 * Copyright (C) 2019-2020 Frank Stettner <frank-stettner@gmx.net>
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

#include <memory>
#include <string>
#include <pybind11/pybind11.h>

#include <QDebug>
#include <QDockWidget>
#include <QEventLoop>
#include <QMetaObject>
#include <QTimer>
#include <QVariant>

#include "uiproxy.hpp"
#include "src/mainwindow.hpp"
#include "src/session.hpp"
#include "src/channels/basechannel.hpp"
#include "src/data/analogtimesignal.hpp"
#include "src/devices/basedevice.hpp"
#include "src/devices/configurable.hpp"
#include "src/python/uihelper.hpp"
#include "src/ui/tabs/basetab.hpp"
#include "src/ui/views/baseview.hpp"
#include "src/ui/views/plotview.hpp"
#include "src/ui/views/valuepanelview.hpp"

using std::shared_ptr;
using std::string;

namespace py = pybind11;

namespace sv {
namespace python {

UiProxy::UiProxy(Session &session, shared_ptr<UiHelper> ui_helper) :
	session_(session),
	ui_helper_(ui_helper),
	event_loop_(),
	timer_()
{
	if (session_.main_window()) {
		connect(this, &UiProxy::add_device_tab,
			session_.main_window(), &MainWindow::add_device_tab);
	}

	connect(this, &UiProxy::add_data_view,
		ui_helper_.get(), &UiHelper::add_data_view);

	connect(this, &UiProxy::add_control_view,
		ui_helper_.get(), &UiHelper::add_control_view);

	connect(this, QOverload<std::string, Qt::DockWidgetArea, shared_ptr<sv::channels::BaseChannel>>::of(&UiProxy::add_plot_view),
		ui_helper_.get(), QOverload<std::string, Qt::DockWidgetArea, shared_ptr<sv::channels::BaseChannel>>::of(&UiHelper::add_plot_view));
	connect(this, QOverload<std::string, Qt::DockWidgetArea, shared_ptr<sv::data::AnalogTimeSignal>>::of(&UiProxy::add_plot_view),
		ui_helper_.get(), QOverload<std::string, Qt::DockWidgetArea, shared_ptr<sv::data::AnalogTimeSignal>>::of(&UiHelper::add_plot_view));
	connect(this, QOverload<std::string, Qt::DockWidgetArea, shared_ptr<sv::data::AnalogTimeSignal>, shared_ptr<sv::data::AnalogTimeSignal>>::of(&UiProxy::add_plot_view),
		ui_helper_.get(), QOverload<std::string, Qt::DockWidgetArea, shared_ptr<sv::data::AnalogTimeSignal>, shared_ptr<sv::data::AnalogTimeSignal>>::of(&UiHelper::add_plot_view));

	connect(this, &UiProxy::add_power_panel_view,
		ui_helper_.get(), &UiHelper::add_power_panel_view);

	connect(this, QOverload<std::string, Qt::DockWidgetArea, shared_ptr<sv::channels::BaseChannel>>::of(&UiProxy::add_value_panel_view),
		ui_helper_.get(), QOverload<std::string, Qt::DockWidgetArea, shared_ptr<sv::channels::BaseChannel>>::of(&UiHelper::add_value_panel_view));
	connect(this, QOverload<std::string, Qt::DockWidgetArea, shared_ptr<sv::data::AnalogTimeSignal>>::of(&UiProxy::add_value_panel_view),
		ui_helper_.get(), QOverload<std::string, Qt::DockWidgetArea, shared_ptr<sv::data::AnalogTimeSignal>>::of(&UiHelper::add_value_panel_view));

	connect(this, &UiProxy::add_signal_to_data_view,
		ui_helper_.get(), &UiHelper::add_signal_to_data_view);

	connect(this, &UiProxy::add_signal_to_plot_view,
		ui_helper_.get(), &UiHelper::add_signal_to_plot_view);
	connect(this, &UiProxy::add_signals_to_xy_plot_view,
		ui_helper_.get(), &UiHelper::add_signals_to_xy_plot_view);

	connect(this, &UiProxy::show_message_box,
		ui_helper_.get(), &UiHelper::show_message_box);
	connect(this, &UiProxy::show_string_input_dialog,
		ui_helper_.get(), &UiHelper::show_string_input_dialog);
	connect(this, &UiProxy::show_double_input_dialog,
		ui_helper_.get(), &UiHelper::show_double_input_dialog);
	connect(this, &UiProxy::show_int_input_dialog,
		ui_helper_.get(), &UiHelper::show_int_input_dialog);
}


void UiProxy::ui_add_device_tab(shared_ptr<devices::BaseDevice> device)
{
	Q_EMIT add_device_tab(device);
}

string UiProxy::ui_add_data_view(string device_id, Qt::DockWidgetArea area,
	shared_ptr<data::AnalogTimeSignal> signal)
{
	string id;
	init_wait_for_view_added(id);
	Q_EMIT add_data_view(device_id, area, signal);
	event_loop_.exec();
	finish_wait_for_signal();

	return id;
}

string UiProxy::ui_add_control_view(string device_id, Qt::DockWidgetArea area,
	shared_ptr<devices::Configurable> configurable)
{
	string id;
	init_wait_for_view_added(id);
	Q_EMIT add_control_view(device_id, area, configurable);
	event_loop_.exec();
	finish_wait_for_signal();

	return id;
}

string UiProxy::ui_add_plot_view(string device_id, Qt::DockWidgetArea area,
	shared_ptr<channels::BaseChannel> channel)
{
	string id;
	init_wait_for_view_added(id);
	Q_EMIT add_plot_view(device_id, area, channel);
	event_loop_.exec();
	finish_wait_for_signal();

	return id;
}

string UiProxy::ui_add_plot_view(string device_id, Qt::DockWidgetArea area,
	shared_ptr<data::AnalogTimeSignal> signal)
{
	string id;
	init_wait_for_view_added(id);
	Q_EMIT add_plot_view(device_id, area, signal);
	event_loop_.exec();
	finish_wait_for_signal();

	return id;
}

string UiProxy::ui_add_plot_view(string device_id, Qt::DockWidgetArea area,
	shared_ptr<data::AnalogTimeSignal> x_signal,
	shared_ptr<data::AnalogTimeSignal> y_signal)
{
	string id;
	init_wait_for_view_added(id);
	Q_EMIT add_plot_view(device_id, area, x_signal, y_signal);
	event_loop_.exec();
	finish_wait_for_signal();

	return id;
}

string UiProxy::ui_add_power_panel_view(string device_id, Qt::DockWidgetArea area,
	shared_ptr<data::AnalogTimeSignal> voltage_signal,
	shared_ptr<data::AnalogTimeSignal> current_signal)
{
	string id;
	init_wait_for_view_added(id);
	Q_EMIT add_power_panel_view(device_id, area, voltage_signal, current_signal);
	event_loop_.exec();
	finish_wait_for_signal();

	return id;
}

string UiProxy::ui_add_value_panel_view(string device_id, Qt::DockWidgetArea area,
	shared_ptr<channels::BaseChannel> channel)
{
	string id;
	init_wait_for_view_added(id);
	Q_EMIT add_value_panel_view(device_id, area, channel);
	event_loop_.exec();
	finish_wait_for_signal();

	return id;
}

string UiProxy::ui_add_value_panel_view(string device_id, Qt::DockWidgetArea area,
	shared_ptr<data::AnalogTimeSignal> signal)
{
	string id;
	init_wait_for_view_added(id);
	Q_EMIT add_value_panel_view(device_id, area, signal);
	event_loop_.exec();
	finish_wait_for_signal();

	return id;
}

void UiProxy::ui_add_signal_to_data_view(string device_id, string view_id,
	shared_ptr<data::AnalogTimeSignal> signal)
{
	Q_EMIT add_signal_to_data_view(device_id, view_id, signal);
}

void UiProxy::ui_add_signal_to_plot_view(string device_id, string view_id,
	shared_ptr<data::AnalogTimeSignal> signal)
{
	Q_EMIT add_signal_to_plot_view(device_id, view_id, signal);
}

void UiProxy::ui_add_signals_to_xy_plot_view(string device_id, string view_id,
	shared_ptr<data::AnalogTimeSignal> x_signal,
	shared_ptr<data::AnalogTimeSignal> y_signal)
{
	Q_EMIT add_signals_to_xy_plot_view(device_id, view_id, x_signal, y_signal);
}


bool UiProxy::ui_show_message_box(const std::string &title,
	const std::string &text)
{
	bool ok;
	init_wait_for_message_box(ok);
	Q_EMIT show_message_box(title, text);
	event_loop_.exec();
	finish_wait_for_signal();

	return ok;
}

py::object UiProxy::ui_show_string_input_dialog(const string &title,
	const string &label, const string &value)
{
	bool ok;
	QVariant qvar;
	init_wait_for_input_dialog(ok, qvar);
	Q_EMIT show_string_input_dialog(title, label, value);
	event_loop_.exec();
	finish_wait_for_signal();

	if (ok)
		return py::cast(qvar.toString().toStdString());
	else
		return py::cast<py::none>(Py_None);
}

py::object UiProxy::ui_show_double_input_dialog(const string &title,
	const string &label, double value, int decimals, double step,
	double min, double max)
{
	bool ok;
	QVariant qvar;
	init_wait_for_input_dialog(ok, qvar);
	Q_EMIT show_double_input_dialog(
		title, label, value, decimals, step, min, max);
	event_loop_.exec();
	finish_wait_for_signal();

	if (ok)
		return py::cast(qvar.toDouble());
	else
		return py::cast<py::none>(Py_None);
}

py::object UiProxy::ui_show_int_input_dialog(const string &title,
	const string &label, int value, int step, int min, int max)
{
	bool ok;
	QVariant qvar;
	init_wait_for_input_dialog(ok, qvar);
	Q_EMIT show_int_input_dialog(title, label, value, step, min, max);
	event_loop_.exec();
	finish_wait_for_signal();

	if (ok)
		return py::cast(qvar.toInt());
	else
		return py::cast<py::none>(Py_None);
}

void UiProxy::init_wait_for_view_added(string &id, int timeout)
{
	event_loop_finished_conn_ =
		connect(ui_helper_.get(), &UiHelper::view_added,
			[this, &id](std::string view_id) {
				id = view_id;
				event_loop_.quit();
			});

	if (timeout > 0) {
		timer_.setSingleShot(true);
		timer_conn_ = connect(&timer_, &QTimer::timeout,
			&event_loop_, &QEventLoop::quit);
		timer_.start(timeout);
	}
}

void UiProxy::init_wait_for_message_box(bool &ok, int timeout)
{
	event_loop_finished_conn_ =
		connect(ui_helper_.get(), &UiHelper::message_box_finished,
			[this, &ok]() {
				ok = true;
				event_loop_.quit();
			});
	event_loop_canceled_conn_ =
		connect(ui_helper_.get(), &UiHelper::message_box_canceled,
			[this, &ok]() {
				ok = false;;
				event_loop_.quit();
			});

	if (timeout > 0) {
		timer_.setSingleShot(true);
		timer_conn_ = connect(&timer_, &QTimer::timeout,
			&event_loop_, &QEventLoop::quit);
		timer_.start(timeout);
	}
}

void UiProxy::init_wait_for_input_dialog(bool &ok, QVariant &qvar, int timeout)
{
	event_loop_finished_conn_ =
		connect(ui_helper_.get(), &UiHelper::input_dialog_finished,
			[this, &ok, &qvar](const QVariant &qvar_input) {
				ok = true;
				qvar = qvar_input;
				event_loop_.quit();
			});
	event_loop_canceled_conn_ =
		connect(ui_helper_.get(), &UiHelper::input_dialog_canceled,
			[this, &ok]() {
				ok = false;;
				event_loop_.quit();
			});

	if (timeout > 0) {
		timer_.setSingleShot(true);
		timer_conn_ = connect(&timer_, &QTimer::timeout,
			&event_loop_, &QEventLoop::quit);
		timer_.start(timeout);
	}
}

void UiProxy::finish_wait_for_signal()
{
	if (event_loop_finished_conn_)
		disconnect(event_loop_finished_conn_);
	if (event_loop_canceled_conn_)
		disconnect(event_loop_canceled_conn_);
	if (timer_conn_)
		disconnect(timer_conn_);
}

} // namespace python
} // namespace sv
