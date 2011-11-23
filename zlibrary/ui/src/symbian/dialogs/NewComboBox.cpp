#include "NewComboBox.h"

#include "ScrollerManager.h"


NewComboBox::NewComboBox(QWidget* parent) : QComboBox(parent) {
    //TODO check life time of view and delegate
    //ComboBoxListView* listView = new ComboBoxListView(this);
    ComboMenuDelegate* delegate = new ComboMenuDelegate(this, this);
    ScrollerManager::setScroll(this->view());
    //this->setView(listView);
    this->setItemDelegate(delegate);
}

ComboBoxListView::ComboBoxListView(QComboBox *cmb) : combo(cmb) {}

void ComboBoxListView::resizeEvent(QResizeEvent *event) {
    resizeContents(viewport()->width(), contentsSize().height());
    QListView::resizeEvent(event);
}

QStyleOptionViewItem ComboBoxListView::viewOptions() const {
    QStyleOptionViewItem option = QListView::viewOptions();
    option.showDecorationSelected = true;
    if (combo)
        option.font = combo->font();
    return option;
}

void ComboBoxListView::paintEvent(QPaintEvent *e) {
    //qDebug() << Q_FUNC_INFO;
    if (combo) {
        QStyleOptionComboBox opt;
        opt.initFrom(combo);
        opt.editable = combo->isEditable();
        if (combo->style()->styleHint(QStyle::SH_ComboBox_Popup, &opt, combo)) {
            //we paint the empty menu area to avoid having blank space that can happen when scrolling
            QStyleOptionMenuItem menuOpt;
            menuOpt.initFrom(this);
            menuOpt.palette = palette();
            menuOpt.state = QStyle::State_None;
            menuOpt.checkType = QStyleOptionMenuItem::NotCheckable;
            menuOpt.menuRect = e->rect();
            menuOpt.maxIconWidth = 0;
            menuOpt.tabWidth = 0;
            QPainter p(viewport());
            combo->style()->drawControl(QStyle::CE_MenuEmptyArea, &menuOpt, &p, this);
        }
    }
    QListView::paintEvent(e);
}

QStyleOptionMenuItem ComboMenuDelegate::getStyleOption(const QStyleOptionViewItem &option,
                                                        const QModelIndex &index) const
{
    QStyleOptionMenuItem menuOption;

    QPalette resolvedpalette = option.palette.resolve(QApplication::palette("QMenu"));
    QVariant value = index.data(Qt::ForegroundRole);
    if (qVariantCanConvert<QBrush>(value)) {
        resolvedpalette.setBrush(QPalette::WindowText, qvariant_cast<QBrush>(value));
        resolvedpalette.setBrush(QPalette::ButtonText, qvariant_cast<QBrush>(value));
        resolvedpalette.setBrush(QPalette::Text, qvariant_cast<QBrush>(value));
    }
    menuOption.palette = resolvedpalette;
    menuOption.state = QStyle::State_None;
    if (mCombo->window()->isActiveWindow())
        menuOption.state = QStyle::State_Active;
    if ((option.state & QStyle::State_Enabled) && (index.model()->flags(index) & Qt::ItemIsEnabled))
        menuOption.state |= QStyle::State_Enabled;
    else
        menuOption.palette.setCurrentColorGroup(QPalette::Disabled);
    if (option.state & QStyle::State_Selected)
        menuOption.state |= QStyle::State_Selected;
    menuOption.checkType = QStyleOptionMenuItem::NonExclusive;
    menuOption.checked = mCombo->currentIndex() == index.row();
//    if (QComboBoxDelegate::isSeparator(index))
//        menuOption.menuItemType = QStyleOptionMenuItem::Separator;
//    else
        menuOption.menuItemType = QStyleOptionMenuItem::Normal;

    QVariant variant = index.model()->data(index, Qt::DecorationRole);
    switch (variant.type()) {
    case QVariant::Icon:
        menuOption.icon = qvariant_cast<QIcon>(variant);
        break;
    case QVariant::Color: {
        static QPixmap pixmap(option.decorationSize);
        pixmap.fill(qvariant_cast<QColor>(variant));
        menuOption.icon = pixmap;
        break; }
    default:
        menuOption.icon = qvariant_cast<QPixmap>(variant);
        break;
    }
    if (qVariantCanConvert<QBrush>(index.data(Qt::BackgroundRole))) {
        menuOption.palette.setBrush(QPalette::All, QPalette::Background,
                                    qvariant_cast<QBrush>(index.data(Qt::BackgroundRole)));
    }
    menuOption.text = index.model()->data(index, Qt::DisplayRole).toString()
                           .replace(QLatin1Char('&'), QLatin1String("&&"));
    menuOption.tabWidth = 0;
    menuOption.maxIconWidth =  option.decorationSize.width() + 4;
    menuOption.menuRect = option.rect;
    menuOption.rect = option.rect;

    // Make sure fonts set on the combo box also overrides the font for the popup menu.
//    if (mCombo->testAttribute(Qt::WA_SetFont)
//            || mCombo->testAttribute(Qt::WA_MacSmallSize)
//            || mCombo->testAttribute(Qt::WA_MacMiniSize)
//            || mCombo->font() != qt_app_fonts_hash()->value("QComboBox", QFont())
//            )
//        menuOption.font = mCombo->font();
//    else
//        menuOption.font = qt_app_fonts_hash()->value("QComboMenuItem", mCombo->font());
    menuOption.font = MenuItemParameters::getFont();

    menuOption.fontMetrics = QFontMetrics(menuOption.font);

    return menuOption;
}
