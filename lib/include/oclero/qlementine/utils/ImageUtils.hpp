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

#include <oclero/qlementine/utils/RadiusesF.hpp>

#include <QPixmap>
#include <QString>
#include <QColor>

#include <iomanip>
#include <sstream>
#include <type_traits>

namespace oclero::qlementine {
/// Converts an integer to its std::string hex form.
template<typename T, typename std::enable_if<std::is_integral<T>::value, T>::type* = nullptr>
std::string to_hex(T const i, bool const prefix = false) {
  std::stringstream stream;
  if (prefix) {
    stream << "0x";
  }
  stream << std::setfill('0') << std::setw(sizeof(T) * 2) << std::hex << i;
  return stream.str();
}

/// Convets an integer to its QString hex form.
template<typename T, typename std::enable_if<std::is_integral<T>::value, T>::type* = nullptr>
QString toHex(T const i, bool const prefix = false) {
  return QString::fromStdString(to_hex(i, prefix));
}

/// Basically colorize the QPixmap and returns a QImage.
QImage colorizeImage(QPixmap const& input, QColor const& color);

/// Basically colorize the QPixmap.
QPixmap colorizePixmap(QPixmap const& input, QColor const& color);

/// Tints the QPixmap, preserving contrast between shades.
QPixmap tintPixmap(QPixmap const& input, QColor const& color);

/// Looks for a colorized version of the input pixmap in the cache.
/// If not existing, generates it and adds it to the cache.
/// Else, just returns the existing pixmap.
/// If any error, returns the input pixmap.
QPixmap getColorizedPixmap(QPixmap const& input, QColor const& color);

/// Looks for a tinted version of the input pixmap in the cache.
/// If not existing, generates it and adds it to the cache.
/// Else, just returns the existing pixmap.
/// If any error, returns the input pixmap.
QPixmap getTintedPixmap(QPixmap const& input, QColor const& color);

/// Gets the key to use in the QPixmapCache.
QString getColorizedPixmapKey(QPixmap const& pixmap, QColor const& color);

/// Gets the key to use in the QPixmapCache.
QString getTintedPixmapKey(QPixmap const& pixmap, QColor const& color);

/// Type of effect applied to colorize the image.
enum class ColorizeMode {
  // Replaces all {R,G,B} values with another, thus loosing luminance, but preserve alpha.
  // Useful for flat symbolic icons.
  Colorize,
  // Applies an {R,G,B} tint, but keeps luminance and alpha.
  // Useful for complex icons with multiple colors and shades.
  Tint,
};

/// Gets the pixmap in the cache, or creates it if not yet there.
QPixmap getCachedPixmap(QPixmap const& input, QColor const& color, ColorizeMode mode);

/// Makes an icon from the file located at the path in parameter. Fixes the standard Qt behavior.
QIcon makeIconFromSvg(const QString& svgPath, const QSize& size);

/// Makes a QPixmap from the file located at the path in parameter at the desired size.
QPixmap makePixmapFromSvg(const QString& svgPath, const QSize& size);

/// Makes a QPixmap from the file located at the path in parameter at the desired size.
QPixmap makePixmapFromSvg(const QString& backgroundSvgPath, const QColor& backgroundSvgColor,
  const QString& foregroundSvgPath, const QColor& foregroundSvgColor, const QSize& size);

/// Makes a QPixmap with rounded corners.
QPixmap makeRoundedPixmap(QPixmap const& input, double radius);

/// Makes a QPixmap with rounded corners.
QPixmap makeRoundedPixmap(QPixmap const& input, const RadiusesF& radiuses);

/// Makes a QPixmap with rounded corners.
QPixmap makeRoundedPixmap(QPixmap const& input, double topLeft, double topRight, double bottomRight, double bottomLeft);

/// Makes a pixmap that fits the requested size.
QPixmap makeFitPixmap(QPixmap const& input, const QSize& size);

/// Gets the aspect ratio (width by height) of the image, without loading it into memory.
double getImageAspectRatio(QString const& path);

/// Gets a version of the pixmap with padding around.
QImage getExtendedImage(QPixmap const& input, int padding);

/// Gets a version of the image with padding around.
QImage getExtendedImage(QImage const& input, int padding);

/// Gets a blurred version of the input pixmap
QPixmap getBlurredPixmap(QPixmap const& input, double blurRadius, bool const extend);

/// Gets a drop shadow for the input pixmap (i.e. a blurred colorized version).
QPixmap getDropShadowPixmap(QPixmap const& input, double blurRadius, QColor const& color = Qt::black);

/// Gets a drop shadow for a QRect.
QPixmap getDropShadowPixmap(QSize const& size, double borderRadius, double blurRadius, QColor const& color = Qt::black);
} // namespace oclero::qlementine
