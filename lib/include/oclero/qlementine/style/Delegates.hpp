// MIT License
//
// Copyright (c) 2023 Olivier Clero
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#pragma once

#include <oclero/qlementine/style/QlementineStyle.hpp>

#include <QItemDelegate>
#include <QPointer>

namespace oclero::qlementine {
class ComboBoxDelegate : public QItemDelegate {
public:
  ComboBoxDelegate(QWidget* widget, QlementineStyle& style);

protected:
  void paint(QPainter* p, const QStyleOptionViewItem& opt, const QModelIndex& idx) const override;
  QSize sizeHint(const QStyleOptionViewItem& opt, const QModelIndex& idx) const override;

private:
  const QWidget* _widget{ nullptr };
  const QPointer<QlementineStyle> _qlementineStyle;
};
} // namespace oclero::qlementine
