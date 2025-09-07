
#include <QtQml/qqmlregistration.h>

#include <QAbstractListModel>
#include <QObject>
#include <amarula/dbus/connman/gconnman.hpp>




using Service = Amarula::DBus::G::Connman::Service;
using ServiceList = Amarula::DBus::G::Connman::Manager::ProxyList<Service>;

namespace Esterv::Network {

class WifiBox : public QObject {

    Q_OBJECT
    Q_PROPERTY(QString name READ name NOTIFY nameChanged)
    Q_PROPERTY(QString path READ path CONSTANT)
    Q_PROPERTY(uint strength READ strength NOTIFY strengthChanged)
    Q_PROPERTY(State state READ state NOTIFY stateChanged)

    QML_UNCREATABLE("")
    QML_ELEMENT


   public:
    using ServProperties = Amarula::DBus::G::Connman::ServProperties;
    using State = ServProperties::State;
    Q_ENUM(State)
    WifiBox(std::weak_ptr<Service> service, QObject *parent)
        : QObject(parent), service_{std::move(service)},
          path_{QString::fromStdString(service_.lock()->objPath())} {
        if (auto service = service_.lock()) {
            auto update_properties = [this](const auto &props) {
                const auto new_strength = props.getStrength();
                if (new_strength != cached_strength_) {
                    cached_strength_ = new_strength;

                    QMetaObject::invokeMethod(
                        this, [this]() { Q_EMIT strengthChanged(); },
                        Qt::QueuedConnection);
                }

                const auto new_state = props.getState();
                if (new_state != cached_state_) {
                    cached_state_ = new_state;
                    QMetaObject::invokeMethod(
                        this, [this]() { Q_EMIT stateChanged(); },
                        Qt::QueuedConnection);
                }

            };
            service->onPropertyChanged(update_properties);
            update_properties(service->properties());
        }
    };
    [[nodiscard]] auto name() const -> QString {
        if (auto service = service_.lock()) {
            return QString::fromStdString(service->properties().getName());
        }
        return "";
    }
    [[nodiscard]] auto path() const -> QString {
        return path_;
    }
    [[nodiscard]] auto strength() const { return cached_strength_; }
    [[nodiscard]] auto state() const { return cached_state_; }

    Q_INVOKABLE void connect() {
        if (auto service = service_.lock()) {
            service->connect([](bool sucesss){
                qDebug()<<"connect "<<sucesss;
            });
        }
    }

    Q_INVOKABLE void disconnect() {
        if (auto service = service_.lock()) {
            service->disconnect([](bool sucesss){
                qDebug()<<"disconnect "<<sucesss;
            });
        }
    }

   Q_SIGNALS:
    void nameChanged();
    void strengthChanged();
    void stateChanged();

   private:
    std::weak_ptr<Service> service_;
    const QString path_;
    uint cached_strength_{0};
    State cached_state_{State::Idle};

    friend class WifiModel;
};

class WifiModel : public QAbstractListModel {
    Q_OBJECT
    Q_PROPERTY(int count READ count NOTIFY countChanged)
    QML_UNCREATABLE("")
    QML_ELEMENT

   public:
    enum ModelRoles : uint16_t { NameRole = Qt::UserRole + 1, StrengthRole, StateRole ,ObjectRole};
    [[nodiscard]] auto count() const -> int;
    explicit WifiModel(QObject *parent = nullptr);
    void updateWifis(const ServiceList &services);

    // NOLINTNEXTLINE(modernize-use-trailing-return-type)
    Q_INVOKABLE bool setProperty(int index, QString role, QVariant value);

    // NOLINTNEXTLINE(modernize-use-trailing-return-type)
    [[nodiscard]] int rowCount(const QModelIndex &pindex) const override;
    // NOLINTNEXTLINE(modernize-use-trailing-return-type)
    [[nodiscard]] QVariant data(const QModelIndex &index,
                                int role) const override;
    // NOLINTNEXTLINE(modernize-use-trailing-return-type)
    [[nodiscard]] QHash<int, QByteArray> roleNames() const override;

   Q_SIGNALS:
    void countChanged(int count);

   private:
    int count_;
    QList<WifiBox *> wifis_;

    void remove_row_at(int row);
    void move_row(int old_index, int new_index);
    void insert_row_at(int index, WifiBox *box);
};

class Manager : public QObject {
    Q_OBJECT
    Q_PROPERTY(WifiModel *wifis READ wifis CONSTANT)

    QML_ELEMENT
    QML_SINGLETON

   public:
    explicit Manager(QObject *parent = nullptr);
    [[nodiscard]] auto wifis() const { return wifis_; }

    Q_INVOKABLE void scan() const;

    Q_INVOKABLE void finishConnect(const QString& passphrase = QString());

   Q_SIGNALS:
    /*
     * A wifi with path and name is asking for passphrase
     */
    void requestInputPassphrase(QString path, QString name);
    /*
     * A wifi with path stoped requesting input
     */
    void finishRequestInput(QString path);

   private:
    using Connman = Amarula::DBus::G::Connman::Connman;
    Connman connman_;
    WifiModel *wifis_;
    std::condition_variable request_input_cv_;
    std::mutex request_input_mx_;
    bool finish_connecting_{false};
    std::string passphrase_;
};

}  // namespace Esterv::Network
