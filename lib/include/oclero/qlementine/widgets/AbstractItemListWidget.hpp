#pragma once

#include <oclero/qlementine/style/Theme.hpp>
#include <oclero/qlementine/style/QlementineStyleOption.hpp>

#include <QWidget>
#include <QIcon>
#include <QVariant>
#include <QString>
#include <QVector>
#include <QVariantAnimation>

namespace oclero::qlementine {
class RoundedFocusFrame;

/// A class to factorize common behavior between widgets such as tab bars.
class AbstractItemListWidget : public QWidget {
  Q_OBJECT

  Q_PROPERTY(int currentIndex READ currentIndex WRITE setCurrentIndex NOTIFY currentIndexChanged)
  Q_PROPERTY(QVariant currentData READ currentData WRITE setCurrentData NOTIFY currentDataChanged)
  Q_PROPERTY(int itemCount READ itemCount NOTIFY itemCountChanged)
  Q_PROPERTY(QSize iconSize READ iconSize WRITE setIconSize NOTIFY iconSizeChanged)
  Q_PROPERTY(bool itemsShouldExpand READ itemsShouldExpand WRITE setItemsShouldExpand NOTIFY itemsShouldExpandChanged)

public:
  explicit AbstractItemListWidget(QWidget* parent = nullptr);
  ~AbstractItemListWidget() override;

  QSize sizeHint() const override;

  int itemCount() const;
  Q_SIGNAL void itemCountChanged();

  int currentIndex() const;
  void setCurrentIndex(int index);
  Q_SIGNAL void currentIndexChanged();

  QVariant currentData() const;
  void setCurrentData(const QVariant& data);
  Q_SIGNAL void currentDataChanged();

  const QSize& iconSize() const;
  void setIconSize(const QSize& size);
  Q_SIGNAL void iconSizeChanged();

  bool itemsShouldExpand() const;
  void setItemsShouldExpand(bool expand);
  Q_SIGNAL void itemsShouldExpandChanged();

  int addItem(const QString& text, const QIcon& icon = {}, const QString& badge = {}, const QVariant& data = {});
  void removeItem(int index);
  int findItemIndex(const QVariant& data) const;

  void setItemData(int index, const QVariant& text);
  QVariant getItemData(int index) const;

  void setItemText(int index, const QString& text);
  QString getItemText(int index) const;

  void setItemIcon(int index, const QIcon& icon);
  QIcon getItemIcon(int index) const;

  void setItemBadge(int index, const QString& badge);
  QString getItemBadge(int index) const;

  void setItemEnabled(int index, bool enabled);
  bool isItemEnabled(int index) const;

  Q_SLOT void moveToNextItem();
  Q_SLOT void moveToPreviousItem();

  virtual void initStyleOptionFocus(QStyleOptionFocusRoundedRect& opt) const;

protected: // QWidget override.
  void keyPressEvent(QKeyEvent* e) override;
  void keyReleaseEvent(QKeyEvent* e) override;
  void mousePressEvent(QMouseEvent* e) override;
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
  void enterEvent(QEvent* e) override;
#else
  void enterEvent(QEnterEvent* e) override;
#endif
  void leaveEvent(QEvent* e) override;
  void mouseReleaseEvent(QMouseEvent* e) override;
  void mouseMoveEvent(QMouseEvent* e) override;
  void paintEvent(QPaintEvent* e) override;
  void resizeEvent(QResizeEvent* e) override;
  void focusInEvent(QFocusEvent* e) override;
  void showEvent(QShowEvent* e) override;
  bool event(QEvent* e) override;
  void changeEvent(QEvent* e) override;

protected: // Available to child classes.
  struct Item {
    bool enabled{ true };
    QString text;
    QIcon icon;
    QString badge;
    QVariant data;
    QRect rect;
    QSize sizeHint;
    QVariantAnimation* bgColorAnimation{ nullptr };
    QVariantAnimation* fgColorAnimation{ nullptr };
    QVariantAnimation* badgeBgAnimation{ nullptr };
    QVariantAnimation* badgeFgAnimation{ nullptr };
  };

  virtual QMargins getItemPadding() const;
  virtual int getItemMinimumHeight() const;
  virtual int getItemSpacing() const;
  virtual double getItemRadius() const;
  virtual const QColor& getItemFgColor(MouseState mouse, bool selected, const Theme& theme) const = 0;
  virtual const QColor& getItemBgColor(MouseState mouse, const Theme& theme) const = 0;
  virtual const QColor& getItemFgColor(MouseState mouse, bool selected, const QPalette& palette) const;
  virtual const QColor& getItemBgColor(MouseState mouse, const QPalette& palette) const;

  virtual QMargins getPadding() const;
  virtual int getSpacing() const;
  virtual double getRadius() const;
  virtual const QColor& getBgColor(const Theme& theme) const = 0;
  virtual const QColor& getBgColor(const QPalette& palette) const;

  virtual QMargins getBadgePadding() const;
  virtual const QColor& getItemBadgeBgColor(MouseState mouse, bool selected, const Theme& theme) const = 0;
  virtual const QColor& getItemBadgeFgColor(MouseState mouse, bool selected, const Theme& theme) const = 0;
  virtual const QColor& getItemBadgeBgColor(MouseState mouse, bool selected, const QPalette& palette) const;
  virtual const QColor& getItemBadgeFgColor(MouseState mouse, bool selected, const QPalette& palette) const;

  virtual void drawBackground(QPainter& p) const;
  virtual void drawItemBackground(QPainter& p, const Item& item) const;
  virtual void drawItemForeground(QPainter& p, const Item& item) const;
  virtual void drawCurrentItemIndicator(QPainter& p) const;
  virtual bool shouldDrawItemBgForCurrent() const;

  virtual QFont labelFont() const;

protected:
  int itemAtPos(const QPoint& pos) const;
  bool hitTestItemRect(const QPoint& pos, const QRect& itemRect, int leftPadding, int rightPadding) const;
  void updateItemsSizeHints();
  void updateItemRects();
  MouseState getItemMouseState(int index, const Item& item) const;
  std::tuple<QColor, QColor, QColor, QColor> getItemBgAndFgColor(int index, MouseState mouse) const;
  const QColor& getCurrentItemIndicatorColor() const;
  const QColor& getBgColor() const;
  QRect getCurrentItemRect() const;
  QRect getFocusedItemRect() const;
  QRect getAnimatedCurrentItemRect() const;
  void updateCurrentIndexAnimation(bool immediate = false);
  void updateItemsAnimations();
  void setFocusedIndex(int index);
  void setHoveredIndex(int index, bool updateAnims = true);
  void setPressedIndex(int index, bool updateAnims = true);
  const QFont& badgeFont() const;
  const QRect& actualRect() const;

private:
  QRect _actualRect;
  bool _itemsShouldExpand{ false };
  int _currentIndex{ -1 };
  int _focusedIndex{ -1 };
  int _hoveredIndex{ -1 };
  int _pressedIndex{ -1 };
  QSize _iconSize;
  QVector<Item> _items;
  QVariantAnimation _currentIndexAnimation;
  RoundedFocusFrame* _focusFrame{ nullptr };
  QFont _badgeFont;
  bool _firstShow{ false };
};
} // namespace oclero::qlementine
