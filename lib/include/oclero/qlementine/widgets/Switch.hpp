#pragma once

#include <oclero/qlementine/widgets/RoundedFocusFrame.hpp>

#include <QAbstractButton>
#include <QVariantAnimation>

namespace oclero::qlementine {
class QStyleOptionFocusRoundedRect;

/// A switch toggle button like on iOS and Android.
class Switch : public QAbstractButton {
  Q_OBJECT

public:
  Switch(QWidget* parent = nullptr);

  QSize sizeHint() const override;

  void initStyleOptionFocus(QStyleOptionFocusRoundedRect& opt) const;

protected:
  void paintEvent(QPaintEvent* e) override;
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
  void enterEvent(QEvent* e) override;
#else
  void enterEvent(QEnterEvent* e) override;
#endif
  void leaveEvent(QEvent* e) override;
  void changeEvent(QEvent* e) override;
  void focusInEvent(QFocusEvent* e) override;
  void focusOutEvent(QFocusEvent* e) override;
  void checkStateSet() override;

private:
  void setupAnimation();
  void startAnimation();
  QRect getSwitchRect() const;

  const QColor& getBgColor() const;
  const QColor& getBorderColor() const;
  const QColor& getFgColor() const;
  const QColor& getTextColor() const;

private:
  int _handlePadding{ 2 };
  bool _isMouseOver{ false };
  QVariantAnimation _handleAnimation;
  QVariantAnimation _bgAnimation;
  QVariantAnimation _borderAnimation;
  QVariantAnimation _fgAnimation;
  RoundedFocusFrame* _focusFrame{ nullptr };
};
} // namespace oclero::qlementine
