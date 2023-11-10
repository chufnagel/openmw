
#include "stringsetting.hpp"

#include <QLineEdit>
#include <QMutexLocker>

#include <components/settings/settings.hpp>

#include <apps/opencs/model/prefs/setting.hpp>

#include "category.hpp"
#include "state.hpp"

CSMPrefs::StringSetting::StringSetting(
    Category* parent, QMutex* mutex, const std::string& key, const QString& label, std::string_view default_)
    : Setting(parent, mutex, key, label)
    , mDefault(default_)
    , mWidget(nullptr)
{
}

CSMPrefs::StringSetting& CSMPrefs::StringSetting::setTooltip(const std::string& tooltip)
{
    mTooltip = tooltip;
    return *this;
}

CSMPrefs::SettingWidgets CSMPrefs::StringSetting::makeWidgets(QWidget* parent)
{
    mWidget = new QLineEdit(QString::fromUtf8(mDefault.c_str()), parent);

    if (!mTooltip.empty())
    {
        QString tooltip = QString::fromUtf8(mTooltip.c_str());
        mWidget->setToolTip(tooltip);
    }

    connect(mWidget, &QLineEdit::textChanged, this, &StringSetting::textChanged);

    return SettingWidgets{ .mLabel = nullptr, .mInput = mWidget, .mLayout = nullptr };
}

void CSMPrefs::StringSetting::updateWidget()
{
    if (mWidget)
    {
        mWidget->setText(QString::fromStdString(Settings::Manager::getString(getKey(), getParent()->getKey())));
    }
}

void CSMPrefs::StringSetting::textChanged(const QString& text)
{
    {
        QMutexLocker lock(getMutex());
        Settings::Manager::setString(getKey(), getParent()->getKey(), text.toStdString());
    }

    getParent()->getState()->update(*this);
}
