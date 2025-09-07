#include <algorithm>
#include <esterv/utils/network_manger.hpp>

namespace Esterv::Network {

using ServType = Amarula::DBus::G::Connman::ServProperties::Type;
using Amarula::DBus::G::Connman::TechProperties;

constexpr int RI_WAIT_TIMEOUT_SECONDS = 30;

Manager::Manager(QObject *parent)
    : QObject(parent), wifis_{new WifiModel(this)} {
    const auto manager = connman_.manager();
    manager->registerAgent(manager->internalAgentPath());
    manager->onServicesChanged([&](const auto &services) {
        QMetaObject::invokeMethod(
            this, [services, this]() { wifis_->updateWifis(services); },
            Qt::QueuedConnection);
    });

    manager->onRequestInputPassphrase([&](const auto &service) -> auto {
        const auto name = service->properties().getName();
        const auto obj_path = service->objPath();
        QMetaObject::invokeMethod(
            this,
            [name, obj_path, this]() {
                Q_EMIT requestInputPassphrase(QString::fromStdString(obj_path),
                                              QString::fromStdString(name));
            },
            Qt::QueuedConnection);
        std::unique_lock<std::mutex> lock(request_input_mx_);
        if (!request_input_cv_.wait_for(
                lock, std::chrono::seconds(RI_WAIT_TIMEOUT_SECONDS),
                [&] { return finish_connecting_; })) {
            service->disconnect();
        }

        QMetaObject::invokeMethod(
            this,
            [this, name]() {
                Q_EMIT finishRequestInput(QString::fromStdString(name));
            },
            Qt::QueuedConnection);

        return std::pair<bool, std::string>{true, passphrase_};
    });
}

WifiModel::WifiModel(QObject *parent) : QAbstractListModel(parent) {}

void WifiModel::remove_row_at(int row) {
    if (row < 0 || row >= wifis_.size()) {
        return;
    }

    beginRemoveRows(QModelIndex(), row, row);
    WifiBox *box = wifis_.takeAt(row);
    delete box;
    endRemoveRows();
}

void WifiModel::move_row(int old_index, int new_index) {
    if (old_index < 0 || old_index >= wifis_.size()) {
        return;
    }
    if (new_index < 0 || new_index >= wifis_.size()) {
        return;
    }
    if (old_index == new_index) {
        return;
    }

    beginMoveRows(QModelIndex(), old_index, old_index, QModelIndex(),
                  (old_index < new_index ? new_index + 1 : new_index));

    wifis_.move(old_index, new_index);

    endMoveRows();
}

void WifiModel::insert_row_at(int index, WifiBox *box) {
    if (box == nullptr) {
        return;
    }

    index = std::max(index, 0);
    index = static_cast<int>(std::min<qsizetype>(index, wifis_.size()));

    beginInsertRows(QModelIndex(), index, index);
    wifis_.insert(index, box);
    endInsertRows();
}

void WifiModel::updateWifis(const ServiceList &services) {
    int new_service_index = 0;
    int old_service_index = 0;
    for (const auto &service : services) {
        const auto props = service->properties();

        if (props.getType() == ServType::Wifi && !props.getName().empty()) {
            bool is_new_service = true;
            for (const auto &old_service : std::as_const(wifis_)) {
                auto old_serv_ptr = old_service->service_.lock();
                if (!old_serv_ptr) {
                    remove_row_at(old_service_index);
                } else {
                    if (old_serv_ptr->objPath() == service->objPath()) {
                        move_row(old_service_index, new_service_index);
                        is_new_service = false;
                    }
                    ++old_service_index;
                }
            }

            if (is_new_service) {
                auto *const box = new WifiBox(service, this);
                connect(box, &WifiBox::stateChanged, this, [=] {
                    Q_EMIT dataChanged(index(new_service_index),
                                       index(new_service_index),
                                       QList<int>{ModelRoles::StateRole});
                });
                connect(box, &WifiBox::strengthChanged, this, [=] {
                    Q_EMIT dataChanged(index(new_service_index),
                                       index(new_service_index),
                                       QList<int>{ModelRoles::StrengthRole});
                });
                connect(box, &WifiBox::nameChanged, this, [=] {
                    Q_EMIT dataChanged(index(new_service_index),
                                       index(new_service_index),
                                       QList<int>{ModelRoles::NameRole});
                });
                insert_row_at(new_service_index, box);
            }

            ++new_service_index;
        }
    }
}

auto WifiModel::rowCount(const QModelIndex &pindex) const -> int {
    Q_UNUSED(pindex)
    return static_cast<int>(wifis_.size());
}

auto WifiModel::roleNames() const -> QHash<int, QByteArray> {
    return {{NameRole, "name"},
            {StrengthRole, "strength"},
            {StateRole, "state"},
            {ObjectRole, "object"}};
}

auto WifiModel::count() const -> int { return static_cast<int>(wifis_.size()); }

auto WifiModel::data(const QModelIndex &index, int role) const -> QVariant {
    if (!index.isValid() || index.row() >= wifis_.size()) {
        return {};
    }

    const auto &wifi = wifis_.at(index.row());
    switch (role) {
        case NameRole:
            return wifi->name();
        case StrengthRole:
            return wifi->strength();
        case StateRole:
            return static_cast<int>(wifi->state());
        case ObjectRole:
            return QVariant::fromValue(wifi);
        default:
            return {};
    }
    return {};
}

void Manager::scan() const {
    const auto manager = connman_.manager();
    const auto techs = manager->technologies();
    for (const auto &tech : techs) {
        const auto props = tech->properties();
        const auto type = props.getType();
        if (type == TechProperties::Type::Wifi) {
            tech->scan();
        }
    }
}

void Manager::finishConnect(const QString &passphrase) {
    passphrase_ = passphrase.toStdString();
    {
        const std::unique_lock<std::mutex> lock(request_input_mx_);
        finish_connecting_ = true;
    }
    request_input_cv_.notify_one();
}

}  // namespace Esterv::Network
