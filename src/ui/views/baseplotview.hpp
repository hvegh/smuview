/*
 * This file is part of the SmuView project.
 *
 * Copyright (C) 2017-2020 Frank Stettner <frank-stettner@gmx.net>
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

#ifndef UI_VIEWS_BASEPLOTVIEW_HPP
#define UI_VIEWS_BASEPLOTVIEW_HPP

#include <vector>

#include <QAction>
#include <QMenu>
#include <QSettings>
#include <QToolBar>
#include <QToolButton>
#include <QUuid>

#include "src/ui/views/baseview.hpp"

using std::vector;

namespace sv {

class Session;

namespace channels {
class BaseChannel;
}

namespace ui {

namespace widgets {
namespace plot {
class BaseCurveData;
class Plot;
}
}

namespace views {

enum class PlotType {
	TimePlot,
	XYPlot,
};

class BasePlotView : public BaseView
{
	Q_OBJECT

public:
	BasePlotView(Session &session, QUuid uuid = QUuid(),
		QWidget *parent = nullptr);

	void save_settings(QSettings &settings) const override;
	void restore_settings(QSettings &settings) override;

protected:
	void update_add_marker_menu();

	PlotType plot_type_;
	widgets::plot::Plot *plot_;

private:
	void setup_ui();
	void setup_toolbar();
	void connect_signals();

	QMenu *add_marker_menu_;
	QToolButton *add_marker_button_;
	QAction *const action_add_marker_;
	QAction *const action_add_diff_marker_;
	QAction *const action_zoom_best_fit_;
	QAction *const action_add_signal_;
	QAction *const action_save_;
	QAction *const action_config_plot_;
	QToolBar *toolbar_;

protected Q_SLOTS:
	virtual void on_action_add_signal_triggered() = 0;

private Q_SLOTS:
	void on_action_add_marker_triggered();
	void on_action_add_diff_marker_triggered();
	void on_action_zoom_best_fit_triggered();
	void on_action_save_triggered();
	void on_action_config_plot_triggered();

};

} // namespace views
} // namespace ui
} // namespace sv

#endif // UI_VIEWS_BASEPLOTVIEW_HPP
