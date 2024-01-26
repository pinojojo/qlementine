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

#include <oclero/qlementine/style/Theme.hpp>
#include <oclero/qlementine/utils/PrimitiveUtils.hpp>
#include <oclero/qlementine/utils/FontUtils.hpp>
#include <oclero/qlementine/utils/ColorUtils.hpp>

#include <QGuiApplication>
#include <QJsonObject>
#include <QFile>
#include <QScreen>
#include <QColor>

#include <optional>

namespace oclero::qlementine {
namespace {
#define TRY_GET_COLOR_ATTRIBUTE(JSON_OBJ, NAME) tryGetColor(JSON_OBJ, QStringLiteral(#NAME), NAME)
#define TRY_GET_INT_ATTRIBUTE(JSON_OBJ, NAME) tryGetInt(JSON_OBJ, QStringLiteral(#NAME), NAME)
#define TRY_GET_DOUBLE_ATTRIBUTE(JSON_OBJ, NAME) tryGetDouble(JSON_OBJ, QStringLiteral(#NAME), NAME)

#define SET_COLOR(JSON_OBJ, NAME) setColor(JSON_OBJ, QStringLiteral(#NAME), NAME)
#define SET_INT(JSON_OBJ, NAME) setInt(JSON_OBJ, QStringLiteral(#NAME), NAME)
#define SET_DOUBLE(JSON_OBJ, NAME) setDouble(JSON_OBJ, QStringLiteral(#NAME), NAME)

std::optional<QColor> tryGetColorRecursive(QJsonObject const& jsonObj, QString const& key, int maxRecursiveCalls = 1) {
  if (jsonObj.contains(key)) {
    const auto variant = jsonObj.value(key).toVariant();

    // Try parse a QColor.
    const auto color = tryGetColorFromVariant(variant);
    if (color.has_value()) {
      return color.value();
    }

    // Check if the value is a reference to another value.
    if (variant.userType() == QMetaType::QString && maxRecursiveCalls > 0) {
      const auto variantString = variant.toString();
      if (variantString != key) {
        return tryGetColorRecursive(jsonObj, variantString, --maxRecursiveCalls);
      }
    }
  }

  return {};
}

void tryGetColor(QJsonObject const& jsonObj, QString const& key, QColor& target) {
  if (const auto opt = tryGetColorRecursive(jsonObj, key)) {
    target = opt.value();
  }
}

QString tryGetString(QJsonObject const& jsonObj, QString const& key, QString const& defaultValue) {
  if (jsonObj.contains(key)) {
    const auto variant = jsonObj.value(key).toVariant();
    if (variant.isValid() && variant.canConvert<QString>()) {
      return variant.toString();
    }
  }
  return defaultValue;
}

std::optional<int> tryGetIntRecursive(QJsonObject const& jsonObj, QString const& key, int maxRecursiveCalls = 1) {
  if (jsonObj.contains(key)) {
    const auto variant = jsonObj.value(key).toVariant();

    // Check if the value is a reference to another value.
    if (variant.userType() == QMetaType::QString && maxRecursiveCalls > 0) {
      const auto variantString = variant.toString();
      if (variantString != key) {
        return tryGetIntRecursive(jsonObj, variantString, --maxRecursiveCalls);
      }
    }

    // Try parse an int.
    if (variant.isValid() && variant.canConvert<int>()) {
      return variant.toInt();
    }
  }
  return {};
}

void tryGetInt(QJsonObject const& jsonObj, QString const& key, int& target) {
  if (const auto opt = tryGetIntRecursive(jsonObj, key)) {
    target = opt.value();
  }
}

std::optional<double> tryGetDoubleRecursive(QJsonObject const& jsonObj, QString const& key, int maxRecursiveCalls = 1) {
  if (jsonObj.contains(key)) {
    const auto variant = jsonObj.value(key).toVariant();

    // Check if the value is a reference to another value.
    if (variant.userType() == QMetaType::QString && maxRecursiveCalls > 0) {
      const auto variantString = variant.toString();
      if (variantString != key) {
        return tryGetDoubleRecursive(jsonObj, variantString, --maxRecursiveCalls);
      }
    }

    // Try parse an int.
    if (variant.isValid() && variant.canConvert<double>()) {
      return variant.toDouble();
    }
  }
  return {};
}

void tryGetDouble(QJsonObject const& jsonObj, QString const& key, double& target) {
  if (const auto opt = tryGetDoubleRecursive(jsonObj, key)) {
    target = opt.value();
  }
}

QJsonDocument readJsonDoc(QString const& jsonPath) {
  QFile jsonFile(jsonPath);
  if (jsonFile.open(QIODevice::ReadOnly)) {
    const auto fileContents = jsonFile.readAll();
    QJsonParseError jsonParseError{};
    auto jsonDoc = QJsonDocument::fromJson(fileContents, &jsonParseError);
    if (jsonParseError.error == QJsonParseError::ParseError::NoError && !jsonDoc.isEmpty()) {
      return jsonDoc;
    }
  }

  return {};
}

void setColor(QJsonObject& jsonObj, const QString& key, const QColor& value) {
  jsonObj.insert(key, toHexRGBA(value));
}

void setInt(QJsonObject& jsonObj, const QString& key, int value) {
  jsonObj.insert(key, value);
}

void setDouble(QJsonObject& jsonObj, const QString& key, double value) {
  jsonObj.insert(key, value);
}
} // namespace

Theme::Theme() {
  initializeFonts();
  initializePalette();
}

Theme::Theme(QJsonDocument const& jsonDoc) {
  initializeFromJson(jsonDoc);
  initializePalette();
}

Theme::Theme(QString const& jsonPath)
  : Theme(readJsonDoc(jsonPath)) {}

void Theme::initializeFonts() {
  // Fonts.
  const auto defaultFont = QFont(QStringLiteral("Microsoft YaHei"));
  const auto fixedFont = QFont(QStringLiteral("Roboto Mono"));
  const auto dpi = QGuiApplication::primaryScreen()->logicalDotsPerInch();
  fontRegular = defaultFont;
  fontRegular.setWeight(QFont::Weight::Normal);
  fontRegular.setPointSizeF(pixelSizeToPointSize(fontSize, dpi));

  fontBold = defaultFont;
  fontBold.setWeight(QFont::Weight::Bold);
  fontBold.setPointSizeF(pixelSizeToPointSize(fontSize, dpi));

  fontH1 = defaultFont;
  fontH1.setWeight(QFont::Weight::Bold);
  fontH1.setPointSizeF(pixelSizeToPointSize(fontSizeH1, dpi));

  fontH2 = defaultFont;
  fontH2.setWeight(QFont::Weight::Bold);
  fontH2.setPointSizeF(pixelSizeToPointSize(fontSizeH2, dpi));

  fontH3 = defaultFont;
  fontH3.setWeight(QFont::Weight::Bold);
  fontH3.setPointSizeF(pixelSizeToPointSize(fontSizeH3, dpi));

  fontH4 = defaultFont;
  fontH4.setWeight(QFont::Weight::Bold);
  fontH4.setPointSizeF(pixelSizeToPointSize(fontSizeH4, dpi));

  fontH5 = defaultFont;
  fontH5.setWeight(QFont::Weight::Bold);
  fontH5.setPointSizeF(pixelSizeToPointSize(fontSizeH5, dpi));

  fontCaption = defaultFont;
  fontCaption.setWeight(QFont::Weight::Normal);
  fontCaption.setPointSizeF(pixelSizeToPointSize(fontSizeS1, dpi));

  fontMonospace = fixedFont;
  fontMonospace.setWeight(QFont::Weight::Normal);
  fontMonospace.setPointSizeF(pixelSizeToPointSize(fontSizeMonospace, dpi));
}

void Theme::initializePalette() {
  // Shades.
  palette.setColor(QPalette::ColorGroup::All, QPalette::ColorRole::Window, backgroundColorMain2);
  palette.setColor(QPalette::ColorGroup::All, QPalette::ColorRole::Dark, backgroundColorMain3);
  palette.setColor(QPalette::ColorGroup::All, QPalette::ColorRole::Mid, backgroundColorMain3);
  palette.setColor(QPalette::ColorGroup::All, QPalette::ColorRole::Midlight, backgroundColorMain2);
  palette.setColor(QPalette::ColorGroup::All, QPalette::ColorRole::Light, backgroundColorMain2);

  // ItemViews.
  // Compute color without alpha, to avoid color blending when the QTreeView is animating.
  const auto itemViewBase = backgroundColorMain1;
  const auto itemViewAlternate =
    getColorSourceOver(itemViewBase, colorWithAlpha(neutralColorDisabled, neutralColorDisabled.alpha() / 2));
  const auto itemViewDisabled = getColorSourceOver(backgroundColorMain1, neutralColorDisabled);
  palette.setColor(QPalette::ColorGroup::All, QPalette::ColorRole::Base, itemViewBase);
  palette.setColor(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Base, itemViewDisabled);
  palette.setColor(QPalette::ColorGroup::All, QPalette::ColorRole::AlternateBase, itemViewAlternate);
  palette.setColor(QPalette::ColorGroup::Disabled, QPalette::ColorRole::AlternateBase, itemViewDisabled);
  palette.setColor(QPalette::ColorGroup::All, QPalette::ColorRole::NoRole, backgroundColorMainTransparent);
  palette.setColor(QPalette::ColorGroup::Disabled, QPalette::ColorRole::NoRole, backgroundColorMainTransparent);

  // Tooltips.
  palette.setColor(QPalette::ColorGroup::All, QPalette::ColorRole::ToolTipBase, secondaryColor);
  palette.setColor(QPalette::ColorGroup::All, QPalette::ColorRole::ToolTipText, secondaryColorForeground);

  // Highlight.
  palette.setColor(QPalette::ColorGroup::All, QPalette::ColorRole::Highlight, primaryColor);
  palette.setColor(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Highlight, primaryColorDisabled);
  palette.setColor(QPalette::ColorGroup::All, QPalette::ColorRole::HighlightedText, primaryColorForeground);
  palette.setColor(QPalette::ColorGroup::Disabled, QPalette::ColorRole::HighlightedText, primaryColorDisabled);

  // Text.
  palette.setColor(QPalette::ColorGroup::All, QPalette::ColorRole::Text, secondaryColor);
  palette.setColor(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Text, secondaryColorDisabled);
  palette.setColor(QPalette::ColorGroup::All, QPalette::ColorRole::WindowText, secondaryColor);
  palette.setColor(QPalette::ColorGroup::Disabled, QPalette::ColorRole::WindowText, secondaryColorDisabled);
  palette.setColor(QPalette::ColorGroup::All, QPalette::ColorRole::PlaceholderText, secondaryColorDisabled);
  palette.setColor(QPalette::ColorGroup::Disabled, QPalette::ColorRole::PlaceholderText, neutralColorDisabled);
  palette.setColor(QPalette::ColorGroup::All, QPalette::ColorRole::Link, primaryColor);
  palette.setColor(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Link, secondaryColorDisabled);
  palette.setColor(QPalette::ColorGroup::All, QPalette::ColorRole::LinkVisited, primaryColor);
  palette.setColor(QPalette::ColorGroup::Disabled, QPalette::ColorRole::LinkVisited, secondaryColorDisabled);
  palette.setColor(QPalette::ColorGroup::All, QPalette::ColorRole::BrightText, secondaryAlternativeColor);
  palette.setColor(QPalette::ColorGroup::Disabled, QPalette::ColorRole::BrightText, secondaryAlternativeColorDisabled);

  // Buttons.
  palette.setColor(QPalette::ColorGroup::All, QPalette::ColorRole::ButtonText, secondaryColorForeground);
  palette.setColor(QPalette::ColorGroup::Disabled, QPalette::ColorRole::ButtonText, secondaryColorForegroundDisabled);
  palette.setColor(QPalette::ColorGroup::All, QPalette::ColorRole::Button, neutralColor);
  palette.setColor(QPalette::ColorGroup::Normal, QPalette::ColorRole::Button, neutralColor);
  palette.setColor(QPalette::ColorGroup::Current, QPalette::ColorRole::Button, neutralColorHovered);
  palette.setColor(QPalette::ColorGroup::Active, QPalette::ColorRole::Button, neutralColorPressed);
  palette.setColor(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Button, neutralColorDisabled);

  //palette.setColor(QPalette::ColorGroup::, QPalette::ColorRole::Background, backgroundColor);
}

void Theme::initializeFromJson(QJsonDocument const& jsonDoc) {
  if (jsonDoc.isObject()) {
    const auto jsonObj = jsonDoc.object();
    if (!jsonObj.isEmpty()) {
      // Parse metadata.
      auto const metaObj = jsonObj.value(QStringLiteral("meta")).toObject();
      meta = ThemeMeta{
        tryGetString(metaObj, QStringLiteral("name"), {}),
        tryGetString(metaObj, QStringLiteral("version"), {}),
        tryGetString(metaObj, QStringLiteral("author"), {}),
      };

      // Parse all values.
      TRY_GET_COLOR_ATTRIBUTE(jsonObj, backgroundColorMain1);
      TRY_GET_COLOR_ATTRIBUTE(jsonObj, backgroundColorMain2);
      TRY_GET_COLOR_ATTRIBUTE(jsonObj, backgroundColorMain3);
      TRY_GET_COLOR_ATTRIBUTE(jsonObj, backgroundColorMain4);
      backgroundColorMainTransparent = colorWithAlpha(backgroundColorMain1, 0);

      TRY_GET_COLOR_ATTRIBUTE(jsonObj, neutralColorDisabled);
      TRY_GET_COLOR_ATTRIBUTE(jsonObj, neutralColor);
      TRY_GET_COLOR_ATTRIBUTE(jsonObj, neutralColorHovered);
      TRY_GET_COLOR_ATTRIBUTE(jsonObj, neutralColorPressed);
      neutralColorTransparent = colorWithAlpha(neutralColorDisabled, 0);

      TRY_GET_COLOR_ATTRIBUTE(jsonObj, focusColor);

      TRY_GET_COLOR_ATTRIBUTE(jsonObj, primaryColor);
      TRY_GET_COLOR_ATTRIBUTE(jsonObj, primaryColorHovered);
      TRY_GET_COLOR_ATTRIBUTE(jsonObj, primaryColorPressed);
      TRY_GET_COLOR_ATTRIBUTE(jsonObj, primaryColorDisabled);
      primaryColorTransparent = colorWithAlpha(primaryColor, 0);

      TRY_GET_COLOR_ATTRIBUTE(jsonObj, primaryColorForeground);
      TRY_GET_COLOR_ATTRIBUTE(jsonObj, primaryColorForegroundHovered);
      TRY_GET_COLOR_ATTRIBUTE(jsonObj, primaryColorForegroundPressed);
      TRY_GET_COLOR_ATTRIBUTE(jsonObj, primaryColorForegroundDisabled);
      primaryColorForegroundTransparent = colorWithAlpha(primaryColorForeground, 0);

      TRY_GET_COLOR_ATTRIBUTE(jsonObj, primaryAlternativeColor);
      TRY_GET_COLOR_ATTRIBUTE(jsonObj, primaryAlternativeColorHovered);
      TRY_GET_COLOR_ATTRIBUTE(jsonObj, primaryAlternativeColorPressed);
      TRY_GET_COLOR_ATTRIBUTE(jsonObj, primaryAlternativeColorDisabled);
      primaryAlternativeColorTransparent = colorWithAlpha(primaryAlternativeColor, 0);

      TRY_GET_COLOR_ATTRIBUTE(jsonObj, secondaryColor);
      TRY_GET_COLOR_ATTRIBUTE(jsonObj, secondaryColorHovered);
      TRY_GET_COLOR_ATTRIBUTE(jsonObj, secondaryColorPressed);
      TRY_GET_COLOR_ATTRIBUTE(jsonObj, secondaryColorDisabled);
      secondaryColorTransparent = colorWithAlpha(secondaryColor, 0);

      TRY_GET_COLOR_ATTRIBUTE(jsonObj, secondaryAlternativeColor);
      TRY_GET_COLOR_ATTRIBUTE(jsonObj, secondaryAlternativeColorHovered);
      TRY_GET_COLOR_ATTRIBUTE(jsonObj, secondaryAlternativeColorPressed);
      TRY_GET_COLOR_ATTRIBUTE(jsonObj, secondaryAlternativeColorDisabled);
      secondaryAlternativeColorTransparent = colorWithAlpha(secondaryAlternativeColor, 0);

      TRY_GET_COLOR_ATTRIBUTE(jsonObj, secondaryColorForeground);
      TRY_GET_COLOR_ATTRIBUTE(jsonObj, secondaryColorForegroundHovered);
      TRY_GET_COLOR_ATTRIBUTE(jsonObj, secondaryColorForegroundPressed);
      TRY_GET_COLOR_ATTRIBUTE(jsonObj, secondaryColorForegroundDisabled);
      secondaryColorForegroundTransparent = colorWithAlpha(secondaryColorForeground, 0);

      TRY_GET_COLOR_ATTRIBUTE(jsonObj, semiTransparentColor1);
      TRY_GET_COLOR_ATTRIBUTE(jsonObj, semiTransparentColor2);
      TRY_GET_COLOR_ATTRIBUTE(jsonObj, semiTransparentColor3);
      TRY_GET_COLOR_ATTRIBUTE(jsonObj, semiTransparentColor4);
      semiTransparentColorTransparent = colorWithAlpha(semiTransparentColor1, 0);

      TRY_GET_COLOR_ATTRIBUTE(jsonObj, statusColorSuccess);
      TRY_GET_COLOR_ATTRIBUTE(jsonObj, statusColorSuccessHovered);
      TRY_GET_COLOR_ATTRIBUTE(jsonObj, statusColorSuccessPressed);
      TRY_GET_COLOR_ATTRIBUTE(jsonObj, statusColorSuccessDisabled);
      TRY_GET_COLOR_ATTRIBUTE(jsonObj, statusColorInfo);
      TRY_GET_COLOR_ATTRIBUTE(jsonObj, statusColorInfoHovered);
      TRY_GET_COLOR_ATTRIBUTE(jsonObj, statusColorInfoPressed);
      TRY_GET_COLOR_ATTRIBUTE(jsonObj, statusColorInfoDisabled);
      TRY_GET_COLOR_ATTRIBUTE(jsonObj, statusColorWarning);
      TRY_GET_COLOR_ATTRIBUTE(jsonObj, statusColorWarningHovered);
      TRY_GET_COLOR_ATTRIBUTE(jsonObj, statusColorWarningPressed);
      TRY_GET_COLOR_ATTRIBUTE(jsonObj, statusColorWarningDisabled);
      TRY_GET_COLOR_ATTRIBUTE(jsonObj, statusColorError);
      TRY_GET_COLOR_ATTRIBUTE(jsonObj, statusColorErrorHovered);
      TRY_GET_COLOR_ATTRIBUTE(jsonObj, statusColorErrorPressed);
      TRY_GET_COLOR_ATTRIBUTE(jsonObj, statusColorErrorDisabled);
      TRY_GET_COLOR_ATTRIBUTE(jsonObj, statusColorForeground);
      TRY_GET_COLOR_ATTRIBUTE(jsonObj, statusColorForegroundHovered);
      TRY_GET_COLOR_ATTRIBUTE(jsonObj, statusColorForegroundPressed);
      TRY_GET_COLOR_ATTRIBUTE(jsonObj, statusColorForegroundDisabled);

      TRY_GET_COLOR_ATTRIBUTE(jsonObj, borderColor);
      TRY_GET_COLOR_ATTRIBUTE(jsonObj, borderColorHovered);
      TRY_GET_COLOR_ATTRIBUTE(jsonObj, borderColorPressed);
      TRY_GET_COLOR_ATTRIBUTE(jsonObj, borderColorDisabled);
      borderColorTransparent = colorWithAlpha(borderColor, 0);

      TRY_GET_COLOR_ATTRIBUTE(jsonObj, shadowColor1);
      TRY_GET_COLOR_ATTRIBUTE(jsonObj, shadowColor2);
      TRY_GET_COLOR_ATTRIBUTE(jsonObj, shadowColor3);
      shadowColorTransparent = colorWithAlpha(shadowColor1, 0);

      TRY_GET_INT_ATTRIBUTE(jsonObj, fontSize);
      TRY_GET_INT_ATTRIBUTE(jsonObj, fontSizeH1);
      TRY_GET_INT_ATTRIBUTE(jsonObj, fontSizeH2);
      TRY_GET_INT_ATTRIBUTE(jsonObj, fontSizeH3);
      TRY_GET_INT_ATTRIBUTE(jsonObj, fontSizeH4);
      TRY_GET_INT_ATTRIBUTE(jsonObj, fontSizeH5);
      TRY_GET_INT_ATTRIBUTE(jsonObj, fontSizeS1);
      TRY_GET_INT_ATTRIBUTE(jsonObj, animationDuration);
      TRY_GET_INT_ATTRIBUTE(jsonObj, focusAnimationDuration);
      TRY_GET_INT_ATTRIBUTE(jsonObj, sliderAnimationDuration);
      TRY_GET_DOUBLE_ATTRIBUTE(jsonObj, borderRadius);
      TRY_GET_DOUBLE_ATTRIBUTE(jsonObj, checkBoxBorderRadius);
      TRY_GET_DOUBLE_ATTRIBUTE(jsonObj, menuItemBorderRadius);
      TRY_GET_DOUBLE_ATTRIBUTE(jsonObj, menuBarItemBorderRadius);
      TRY_GET_INT_ATTRIBUTE(jsonObj, borderWidth);
      TRY_GET_INT_ATTRIBUTE(jsonObj, controlHeightLarge);
      TRY_GET_INT_ATTRIBUTE(jsonObj, controlHeightMedium);
      TRY_GET_INT_ATTRIBUTE(jsonObj, controlHeightSmall);
      TRY_GET_INT_ATTRIBUTE(jsonObj, controlDefaultWidth);
      TRY_GET_INT_ATTRIBUTE(jsonObj, dialMarkLength);
      TRY_GET_INT_ATTRIBUTE(jsonObj, dialMarkThickness);
      TRY_GET_INT_ATTRIBUTE(jsonObj, dialTickLength);
      TRY_GET_INT_ATTRIBUTE(jsonObj, dialTickSpacing);
      TRY_GET_INT_ATTRIBUTE(jsonObj, dialGrooveThickness);
      TRY_GET_INT_ATTRIBUTE(jsonObj, focusBorderWidth);

      auto iconExtent = 16;
      TRY_GET_INT_ATTRIBUTE(jsonObj, iconExtent);
      iconSize = QSize{ iconExtent, iconExtent };
      iconSizeMedium = iconSize * 1.5;
      iconSizeLarge = iconSize * 2;
      iconSizeExtraSmall = iconSize * 0.75;

      TRY_GET_INT_ATTRIBUTE(jsonObj, sliderTickSize);
      TRY_GET_INT_ATTRIBUTE(jsonObj, sliderTickSpacing);
      TRY_GET_INT_ATTRIBUTE(jsonObj, sliderTickThickness);
      TRY_GET_INT_ATTRIBUTE(jsonObj, sliderGrooveHeight);
      TRY_GET_INT_ATTRIBUTE(jsonObj, progressBarGrooveHeight);
      TRY_GET_INT_ATTRIBUTE(jsonObj, spacing);
      TRY_GET_INT_ATTRIBUTE(jsonObj, scrollBarThicknessFull);
      TRY_GET_INT_ATTRIBUTE(jsonObj, scrollBarThicknessSmall);
      TRY_GET_INT_ATTRIBUTE(jsonObj, scrollBarMargin);
      TRY_GET_INT_ATTRIBUTE(jsonObj, tabBarPaddingTop);
      TRY_GET_INT_ATTRIBUTE(jsonObj, tabBarTabMaxWidth);
      TRY_GET_INT_ATTRIBUTE(jsonObj, tabBarTabMinWidth);

      tabBarTabMaxWidth = std::max(0, tabBarTabMaxWidth);
      tabBarTabMinWidth = std::max(0, tabBarTabMinWidth);
      if (tabBarTabMinWidth > tabBarTabMaxWidth) {
        std::swap(tabBarTabMinWidth, tabBarTabMaxWidth);
      }

      // Fonts.
      initializeFonts();
    }
  }
}

QJsonDocument Theme::toJson() const {
  QJsonObject jsonObj;

  // Metadata.
  QJsonObject metadataObj;
  metadataObj.insert("name", meta.name);
  metadataObj.insert("version", meta.version);
  metadataObj.insert("author", meta.author);
  jsonObj.insert("meta", metadataObj);

  // Colors.
  SET_COLOR(jsonObj, backgroundColorMain1);
  SET_COLOR(jsonObj, backgroundColorMain2);
  SET_COLOR(jsonObj, backgroundColorMain3);
  SET_COLOR(jsonObj, backgroundColorMain4);

  SET_COLOR(jsonObj, neutralColor);
  SET_COLOR(jsonObj, neutralColorHovered);
  SET_COLOR(jsonObj, neutralColorPressed);
  SET_COLOR(jsonObj, neutralColorDisabled);

  SET_COLOR(jsonObj, focusColor);

  SET_COLOR(jsonObj, primaryColor);
  SET_COLOR(jsonObj, primaryColorHovered);
  SET_COLOR(jsonObj, primaryColorPressed);
  SET_COLOR(jsonObj, primaryColorDisabled);

  SET_COLOR(jsonObj, primaryColorForeground);
  SET_COLOR(jsonObj, primaryColorForegroundHovered);
  SET_COLOR(jsonObj, primaryColorForegroundPressed);
  SET_COLOR(jsonObj, primaryColorForegroundDisabled);

  SET_COLOR(jsonObj, primaryAlternativeColor);
  SET_COLOR(jsonObj, primaryAlternativeColorHovered);
  SET_COLOR(jsonObj, primaryAlternativeColorPressed);
  SET_COLOR(jsonObj, primaryAlternativeColorDisabled);

  SET_COLOR(jsonObj, secondaryColor);
  SET_COLOR(jsonObj, secondaryColorHovered);
  SET_COLOR(jsonObj, secondaryColorPressed);
  SET_COLOR(jsonObj, secondaryColorDisabled);

  SET_COLOR(jsonObj, secondaryColorForeground);
  SET_COLOR(jsonObj, secondaryColorForegroundHovered);
  SET_COLOR(jsonObj, secondaryColorForegroundPressed);
  SET_COLOR(jsonObj, secondaryColorForegroundDisabled);

  SET_COLOR(jsonObj, secondaryAlternativeColor);
  SET_COLOR(jsonObj, secondaryAlternativeColorHovered);
  SET_COLOR(jsonObj, secondaryAlternativeColorPressed);
  SET_COLOR(jsonObj, secondaryAlternativeColorDisabled);

  SET_COLOR(jsonObj, statusColorSuccess);
  SET_COLOR(jsonObj, statusColorSuccessHovered);
  SET_COLOR(jsonObj, statusColorSuccessPressed);
  SET_COLOR(jsonObj, statusColorSuccessDisabled);

  SET_COLOR(jsonObj, statusColorInfo);
  SET_COLOR(jsonObj, statusColorInfoHovered);
  SET_COLOR(jsonObj, statusColorInfoPressed);
  SET_COLOR(jsonObj, statusColorInfoDisabled);

  SET_COLOR(jsonObj, statusColorWarning);
  SET_COLOR(jsonObj, statusColorWarningHovered);
  SET_COLOR(jsonObj, statusColorWarningPressed);
  SET_COLOR(jsonObj, statusColorWarningDisabled);

  SET_COLOR(jsonObj, statusColorError);
  SET_COLOR(jsonObj, statusColorErrorHovered);
  SET_COLOR(jsonObj, statusColorErrorPressed);
  SET_COLOR(jsonObj, statusColorErrorDisabled);

  SET_COLOR(jsonObj, shadowColor1);
  SET_COLOR(jsonObj, shadowColor2);
  SET_COLOR(jsonObj, shadowColor3);

  SET_COLOR(jsonObj, borderColor);
  SET_COLOR(jsonObj, borderColorHovered);
  SET_COLOR(jsonObj, borderColorPressed);
  SET_COLOR(jsonObj, borderColorDisabled);

  SET_COLOR(jsonObj, semiTransparentColor1);
  SET_COLOR(jsonObj, semiTransparentColor2);
  SET_COLOR(jsonObj, semiTransparentColor3);
  SET_COLOR(jsonObj, semiTransparentColor4);

  SET_INT(jsonObj, fontSize);
  SET_INT(jsonObj, fontSizeMonospace);
  SET_INT(jsonObj, fontSizeH1);
  SET_INT(jsonObj, fontSizeH2);
  SET_INT(jsonObj, fontSizeH3);
  SET_INT(jsonObj, fontSizeH4);
  SET_INT(jsonObj, fontSizeH5);
  SET_INT(jsonObj, fontSizeS1);
  SET_INT(jsonObj, animationDuration);
  SET_INT(jsonObj, focusAnimationDuration);
  SET_INT(jsonObj, sliderAnimationDuration);
  SET_DOUBLE(jsonObj, borderRadius);
  SET_DOUBLE(jsonObj, checkBoxBorderRadius);
  SET_DOUBLE(jsonObj, menuBarItemBorderRadius);
  SET_INT(jsonObj, borderWidth);
  SET_INT(jsonObj, controlHeightLarge);
  SET_INT(jsonObj, controlHeightMedium);
  SET_INT(jsonObj, controlHeightSmall);
  SET_INT(jsonObj, controlDefaultWidth);
  SET_INT(jsonObj, dialMarkLength);
  SET_INT(jsonObj, dialMarkThickness);
  SET_INT(jsonObj, dialTickLength);
  SET_INT(jsonObj, dialTickSpacing);
  SET_INT(jsonObj, dialGrooveThickness);
  SET_INT(jsonObj, focusBorderWidth);
  setInt(jsonObj, "iconSize", iconSize.width());
  setInt(jsonObj, "iconSizeMedium", iconSizeMedium.width());
  setInt(jsonObj, "iconSizeLarge", iconSizeLarge.width());
  setInt(jsonObj, "iconSizeExtraSmall", iconSizeExtraSmall.width());
  SET_INT(jsonObj, sliderTickSize);
  SET_INT(jsonObj, sliderTickSpacing);
  SET_INT(jsonObj, sliderTickThickness);
  SET_INT(jsonObj, sliderGrooveHeight);
  SET_INT(jsonObj, progressBarGrooveHeight);
  SET_INT(jsonObj, spacing);
  SET_INT(jsonObj, scrollBarThicknessFull);
  SET_INT(jsonObj, scrollBarThicknessSmall);
  SET_INT(jsonObj, scrollBarMargin);
  SET_INT(jsonObj, tabBarPaddingTop);
  SET_INT(jsonObj, tabBarTabMaxWidth);
  SET_INT(jsonObj, tabBarTabMinWidth);

  QJsonDocument jsonDoc;
  jsonDoc.setObject(jsonObj);
  return jsonDoc;
}

bool Theme::operator==(const Theme& other) const {
  // All generated values are not used in this equality test, on purpose.
  // TODO Remove this and instead use spaceship operator when C++20 is available on macOS/XCode.
  // clang-format off
  return meta == other.meta
    && backgroundColorMain1 == other.backgroundColorMain1
    && backgroundColorMain2 == other.backgroundColorMain2
    && backgroundColorMain3 == other.backgroundColorMain3
    && backgroundColorMain4 == other.backgroundColorMain4

    && neutralColorDisabled == other.neutralColorDisabled
    && neutralColor == other.neutralColor
    && neutralColorHovered == other.neutralColorHovered
    && neutralColorPressed == other.neutralColorPressed

    && focusColor == other.focusColor

    && primaryColor == other.primaryColor
    && primaryColorHovered == other.primaryColorHovered
    && primaryColorPressed == other.primaryColorPressed
    && primaryColorDisabled == other.primaryColorDisabled

    && primaryColorForeground == other.primaryColorForeground
    && primaryColorForegroundHovered == other.primaryColorForegroundHovered
    && primaryColorForegroundPressed == other.primaryColorForegroundPressed
    && primaryColorForegroundDisabled == other.primaryColorForegroundDisabled

    && primaryAlternativeColor == other.primaryAlternativeColor
    && primaryAlternativeColorHovered == other.primaryAlternativeColorHovered
    && primaryAlternativeColorPressed == other.primaryAlternativeColorPressed
    && primaryAlternativeColorDisabled == other.primaryAlternativeColorDisabled

    && secondaryColor == other.secondaryColor
    && secondaryColorHovered == other.secondaryColorHovered
    && secondaryColorPressed == other.secondaryColorPressed
    && secondaryColorDisabled == other.secondaryColorDisabled

    && secondaryAlternativeColor == other.secondaryAlternativeColor
    && secondaryAlternativeColorHovered == other.secondaryAlternativeColorHovered
    && secondaryAlternativeColorPressed == other.secondaryAlternativeColorPressed
    && secondaryAlternativeColorDisabled == other.secondaryAlternativeColorDisabled

    && secondaryColorForeground == other.secondaryColorForeground
    && secondaryColorForegroundHovered == other.secondaryColorForegroundHovered
    && secondaryColorForegroundPressed == other.secondaryColorForegroundPressed
    && secondaryColorForegroundDisabled == other.secondaryColorForegroundDisabled

    && statusColorSuccess == other.statusColorSuccess
    && statusColorSuccessHovered == other.statusColorSuccessHovered
    && statusColorSuccessPressed == other.statusColorSuccessPressed
    && statusColorSuccessDisabled == other.statusColorSuccessDisabled
    && statusColorInfo == other.statusColorInfo
    && statusColorInfoHovered == other.statusColorInfoHovered
    && statusColorInfoPressed == other.statusColorInfoPressed
    && statusColorInfoDisabled == other.statusColorInfoDisabled
    && statusColorWarning == other.statusColorWarning
    && statusColorWarningHovered == other.statusColorWarningHovered
    && statusColorWarningPressed == other.statusColorWarningPressed
    && statusColorWarningDisabled == other.statusColorWarningDisabled
    && statusColorError == other.statusColorError
    && statusColorErrorHovered == other.statusColorErrorHovered
    && statusColorErrorPressed == other.statusColorErrorPressed
    && statusColorErrorDisabled == other.statusColorErrorDisabled
    && statusColorForeground == other.statusColorForeground
    && statusColorForegroundHovered == other.statusColorForegroundHovered
    && statusColorForegroundPressed == other.statusColorForegroundPressed
    && statusColorForegroundDisabled == other.statusColorForegroundDisabled

    && shadowColor1 == other.shadowColor1
    && shadowColor2 == other.shadowColor2
    && shadowColor3 == other.shadowColor3

    && borderColor == other.borderColor
    && borderColorHovered == other.borderColorHovered
    && borderColorPressed == other.borderColorPressed
    && borderColorDisabled == other.borderColorDisabled

    && animationDuration == other.animationDuration
    && focusAnimationDuration == other.focusAnimationDuration
    && sliderAnimationDuration == other.sliderAnimationDuration
    && borderRadius == other.borderRadius
    && checkBoxBorderRadius == other.checkBoxBorderRadius
    && menuItemBorderRadius == other.menuItemBorderRadius
    && menuBarItemBorderRadius == other.menuBarItemBorderRadius
    && borderWidth == other.borderWidth
    && controlHeightLarge == other.controlHeightLarge
    && controlHeightMedium == other.controlHeightMedium
    && controlHeightSmall == other.controlHeightSmall
    && controlDefaultWidth == other.controlDefaultWidth
    && dialMarkLength == other.dialMarkLength
    && dialMarkThickness == other.dialMarkThickness
    && dialTickLength == other.dialTickLength
    && dialTickSpacing == other.dialTickSpacing
    && dialGrooveThickness == other.dialGrooveThickness
    && focusBorderWidth == other.focusBorderWidth
    && iconSize == other.iconSize
    && iconSizeMedium == other.iconSizeMedium
    && iconSizeLarge == other.iconSizeLarge
    && iconSizeExtraSmall == other.iconSizeExtraSmall
    && sliderTickSize == other.sliderTickSize
    && sliderTickThickness == other.sliderTickThickness
    && sliderGrooveHeight == other.sliderGrooveHeight
    && progressBarGrooveHeight == other.progressBarGrooveHeight
    && spacing == other.spacing
    && scrollBarThicknessFull == other.scrollBarThicknessFull
    && scrollBarThicknessSmall == other.scrollBarThicknessSmall
    && scrollBarMargin == other.scrollBarMargin
    && tabBarPaddingTop == other.tabBarPaddingTop
    ;
  // clang-format on
}

bool Theme::operator!=(const Theme& other) const {
  return !(*this == other);
}

} // namespace oclero::qlementine
