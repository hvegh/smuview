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

#include <string>

#include <QAction>
#include <QDebug>
#include <QDir>
#include <QFileDialog>
#include <QMessageBox>
#include <QSettings>
#include <QString>
#include <QTextOption>
#include <QTextStream>
#include <QToolBar>
#include <QVBoxLayout>

#include <QCodeEditor>
#include <QPythonCompleter>
#include <QPythonHighlighter>

#include "smuscriptview.hpp"
#include "src/session.hpp"
#include "src/python/smuscriptrunner.hpp"
#include "src/ui/views/baseview.hpp"

using std::string;

namespace sv {
namespace ui {
namespace views {

SmuScriptView::SmuScriptView(Session &session, string script_file_name,
		QWidget *parent) :
	BaseView(session, parent),
	script_file_name_(script_file_name),
	action_open_(new QAction(this)),
	action_save_(new QAction(this)),
	action_save_as_(new QAction(this)),
	action_run_(new QAction(this)),
	text_changed_(false),
	started_from_here_(false)
{
	id_ = "smuscript:" + std::to_string(BaseView::id_counter++);;

	setup_ui();
	setup_toolbar();
	connect_signals();
}

QString SmuScriptView::title() const
{
	if (script_file_name_.length() <= 0)
		return tr("Untitled");

	std::size_t found = script_file_name_.find_last_of("/\\");
	return QString::fromStdString(script_file_name_.substr(found+1));
}

bool SmuScriptView::ask_to_save(const QString &title)
{
	if (!text_changed_)
		return true;

	QMessageBox::StandardButton reply = QMessageBox::warning(this, title,
		tr("The file \"%1\" has unsaved changes. Would you like to save them?").
			arg(QString::fromStdString(script_file_name_)),
		QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);

	if (reply == QMessageBox::Yes) {
		if (!this->save(QString::fromStdString(script_file_name_)))
			return false;
	}
	else if (reply == QMessageBox::Cancel)
		return false;

	return true;
}

void SmuScriptView::setup_ui()
{
	QVBoxLayout *layout = new QVBoxLayout();

	editor_ = new QCodeEditor();
	//editor->setSyntaxStyle();
	editor_->setCompleter(new QPythonCompleter);
	editor_->setHighlighter(new QPythonHighlighter);
	editor_->setAutoIndentation(true);
	editor_->setAutoParentheses(true);
	editor_->setWordWrapMode(QTextOption::WordWrap);
	if (!script_file_name_.empty()) {
		QFile script_file(script_file_name_.c_str());
		if (script_file.open(QFile::ReadOnly | QFile::Text))
			editor_->setPlainText(script_file.readAll());
	}
	layout->addWidget(editor_);

	this->central_widget_->setLayout(layout);
}

void SmuScriptView::setup_toolbar()
{
	action_open_->setText(tr("&Open"));
	action_open_->setIconText(tr("Open"));
	action_open_->setIcon(
		QIcon::fromTheme("document-open",
		QIcon(":/icons/document-open.png")));
	action_open_->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_O));
	connect(action_open_, SIGNAL(triggered(bool)),
		this, SLOT(on_action_open_triggered()));

	action_save_->setText(tr("&Save"));
	action_save_->setIconText(tr("Save"));
	action_save_->setIcon(
		QIcon::fromTheme("document-save",
		QIcon(":/icons/document-save.png")));
	action_save_->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_S));
	connect(action_save_, SIGNAL(triggered(bool)),
		this, SLOT(on_action_save_triggered()));

	action_save_as_->setText(tr("Save &As"));
	action_save_as_->setIconText(tr("Save As"));
	action_save_as_->setIcon(
		QIcon::fromTheme("document-save-as",
		QIcon(":/icons/document-save-as.png")));
	action_save_as_->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_A));
	connect(action_save_as_, SIGNAL(triggered(bool)),
		this, SLOT(on_action_save_as_triggered()));

	action_run_->setText(tr("Run"));
	action_run_->setIconText(tr("Run"));
	action_run_->setIcon(
		QIcon::fromTheme("media-playback-start",
		QIcon(":/icons/media-playback-start.png")));
	action_run_->setCheckable(true);
	action_run_->setChecked(false);
	connect(action_run_, SIGNAL(triggered(bool)),
		this, SLOT(on_action_run_triggered()));
	if (session_.smu_script_runner()->is_running())
		action_run_->setDisabled(true);

	toolbar_ = new QToolBar("SmuScript Toolbar");
	toolbar_->addAction(action_open_);
	toolbar_->addAction(action_save_);
	toolbar_->addAction(action_save_as_);
	toolbar_->addSeparator();
	toolbar_->addAction(action_run_);
	this->addToolBar(Qt::TopToolBarArea, toolbar_);
}

void SmuScriptView::connect_signals()
{
	connect(editor_, &QCodeEditor::textChanged,
		this, &SmuScriptView::on_text_changed);

	connect(session_.smu_script_runner().get(), &python::SmuScriptRunner::script_started,
		this, &SmuScriptView::on_script_started);
	connect(session_.smu_script_runner().get(), &python::SmuScriptRunner::script_finished,
		this, &SmuScriptView::on_script_finished);
}

void SmuScriptView::save_settings(QSettings &settings) const
{
	(void)settings;
}

void SmuScriptView::restore_settings(QSettings &settings)
{
	(void)settings;
}

bool SmuScriptView::save(QString file_name)
{
	if (file_name.length() <= 0) {
		file_name = QFileDialog::getSaveFileName(this,
			tr("Save SmuScript-File"),
			QDir::homePath(), tr("Python Files (*.py)"));
		if (file_name.length() <= 0)
			return false;
	}

	QFile file(file_name);
	if (file.open(QFile::WriteOnly | QFile::Text | QFile::Truncate)) {
		QTextStream stream(&file);
		stream << editor_->toPlainText() << flush;
		file.close();

		text_changed_ = false;
		Q_EMIT file_save_state_changed(false);
	}
	else {
		QMessageBox::critical(this, tr("File error"),
			tr("Could not save to file \"%1\".").arg(file_name),
			QMessageBox::Ok);
		return false;
	}

	// Check if filename has changed
	if (script_file_name_ != file_name.toStdString()) {
		script_file_name_ = file_name.toStdString();
		Q_EMIT file_name_changed(file_name);
	}

	return true;
}

void SmuScriptView::run_script()
{
	if (action_run_->isChecked()) {
		// Already running!
		return;
	}

	action_run_->activate(QAction::Trigger);
}

void SmuScriptView::stop_script()
{
	if (!action_run_->isChecked()) {
		// Not running!
		return;
	}

	action_run_->activate(QAction::Trigger);
}

void SmuScriptView::on_action_open_triggered()
{
	if (!ask_to_save(tr("Open new script file")))
		return;

	QString file_name = QFileDialog::getOpenFileName(this,
		tr("Open SmuScript-File"), QDir::homePath(), tr("Python Files (*.py)"));
	if (file_name.length() <= 0)
		return;

	QFile file(file_name);
	if (file.open(QFile::ReadOnly | QFile::Text)) {
		editor_->setPlainText(file.readAll());
		file.close();

		text_changed_ = false;
		Q_EMIT file_save_state_changed(false);
	}

	// Check if filename has changed
	if (script_file_name_ != file_name.toStdString()) {
		script_file_name_ = file_name.toStdString();
		Q_EMIT file_name_changed(file_name);
	}
}

void SmuScriptView::on_action_save_triggered()
{
	this->save(QString::fromStdString(script_file_name_));
}

void SmuScriptView::on_action_save_as_triggered()
{
	this->save("");
}

void SmuScriptView::on_text_changed()
{
	text_changed_ = true;
	Q_EMIT file_save_state_changed(true);
}

void SmuScriptView::on_action_run_triggered()
{
	if (action_run_->isChecked()) {
		if (!ask_to_save(tr("File changed")))
			return;

		action_run_->setText(tr("Stop"));
		action_run_->setIconText(tr("Stop"));
		action_run_->setIcon(
			QIcon::fromTheme("media-playback-stop",
			QIcon(":/icons/media-playback-stop.png")));

		started_from_here_ = true;
		session_.smu_script_runner()->run(script_file_name_);
	}
	else {
		action_run_->setText(tr("Run"));
		action_run_->setIconText(tr("Run"));
		action_run_->setIcon(
			QIcon::fromTheme("media-playback-start",
			QIcon(":/icons/media-playback-start.png")));

		started_from_here_ = false;
		session_.smu_script_runner()->stop();
	}
}

void SmuScriptView::on_script_started()
{
	if (started_from_here_)
		Q_EMIT script_started();
	else
		action_run_->setDisabled(true);
}

void SmuScriptView::on_script_finished()
{
	if (started_from_here_) {
		action_run_->setText(tr("Run"));
		action_run_->setIconText(tr("Run"));
		action_run_->setIcon(
			QIcon::fromTheme("media-playback-start",
			QIcon(":/icons/media-playback-start.png")));
		action_run_->setChecked(false);
		started_from_here_ = false;

		Q_EMIT script_finished();
	}
	else
		action_run_->setDisabled(false);
}

} // namespace views
} // namespace ui
} // namespace sv
