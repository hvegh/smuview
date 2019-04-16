/*
 * This file is part of the SmuView project.
 *
 * Copyright (C) 2019 Frank Stettner <frank-stettner@gmx.net>
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

#include <stdexcept>

#include <QDebug>
#include <qwt_slider.h>

#include "doubleslider.hpp"
#include "src/util.hpp"
#include "src/devices/configurable.hpp"
#include "src/devices/properties/baseproperty.hpp"
#include "src/devices/properties/doubleproperty.hpp"

using std::dynamic_pointer_cast;

namespace sv {
namespace ui {
namespace datatypes {

DoubleSlider::DoubleSlider(
		shared_ptr<sv::devices::properties::BaseProperty> property,
		const bool auto_commit, const bool auto_update,
		QWidget *parent) :
	QwtSlider(parent),
	BaseWidget(property, auto_commit, auto_update)
{
	// Check property
	if (property_ != nullptr &&
			property_->data_type() != devices::DataType::Double) {

		QString msg = QString("DoubleSlider with property of type ").append(
			devices::deviceutil::format_data_type(property_->data_type()));
		throw std::runtime_error(msg.toStdString());
	}

	setup_ui();
	connect_signals();
}

void DoubleSlider::setup_ui()
{
	this->setOrientation(Qt::Horizontal);
	this->setScalePosition(QwtSlider::TrailingScale);
	this->setTrough(true);
	this->setGroove(false);
	if (property_ != nullptr && property_->is_listable()) {
		shared_ptr<devices::properties::DoubleProperty> double_prop =
			dynamic_pointer_cast<devices::properties::DoubleProperty>(property_);

		this->setLowerBound(double_prop->min());
		this->setUpperBound(double_prop->max());
		this->setTotalSteps(
			(double_prop->max() - double_prop->min()) / double_prop->step());
	}
	if (property_ == nullptr || !property_->is_setable())
		this->setDisabled(true);
	if (property_ != nullptr && property_->is_getable())
		on_value_changed(property_->value());
	else
		on_value_changed(QVariant(.0));
}

void DoubleSlider::connect_signals()
{
	// Widget -> Property
	connect_widget_2_prop_signals();

	// Property -> Widget
	if (auto_update_ && property_ != nullptr) {
		connect(property_.get(), SIGNAL(value_changed(const QVariant)),
			this, SLOT(on_value_changed(const QVariant)));
	}
}

void DoubleSlider::connect_widget_2_prop_signals()
{
	if (auto_commit_ && property_ != nullptr && property_->is_setable()) {
		connect(this, SIGNAL(valueChanged(double)),
			this, SLOT(value_changed(const double)));
	}
}

void DoubleSlider::disconnect_widget_2_prop_signals()
{
	if (auto_commit_ && property_ != nullptr && property_->is_setable()) {
		disconnect(this, SIGNAL(valueChanged(double)),
			this, SLOT(value_changed(const double)));
	}
}

QVariant DoubleSlider::variant_value() const
{
	return QVariant(this->value());
}

void DoubleSlider::value_changed(const double value)
{
	if (property_ != nullptr)
		property_->change_value(QVariant(value));
}

void DoubleSlider::on_value_changed(const QVariant qvar)
{
	// Disconnect Widget -> Property signal to prevent echoing
	disconnect_widget_2_prop_signals();

	this->setValue(qvar.toDouble());

	connect_widget_2_prop_signals();
}

} // namespace datatypes
} // namespace ui
} // namespace sv