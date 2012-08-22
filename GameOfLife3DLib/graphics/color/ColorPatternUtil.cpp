#include "StdAfx.h"
#include "graphics/GraphicsCommon.h"
#include "graphics/brush/LinearGradientBrush.h"
#include "graphics/color/ColorValue.h"
#include "graphics/color/ColorUtil.h"
#include "graphics/color/ColorPatternUtil.h"
#include "graphics/color/ColorSet.h"
#include "graphics/color/SolidColor.h"
#include "graphics/color/LinearGradientColor.h"
#include "graphics/color/GradientStop.h"
#include "graphics/color/GradientStopCollection.h"

void graphics::color::ColorPatternUtil::GeneratePattern(
    IN COLOR_PATTERN colorPattern,
    IN const graphics::color::ColorValue& color,
    OUT std::vector<std::shared_ptr<graphics::brush::LinearGradientBrush>> &brushes )
{
    switch (colorPattern) {
    case COLOR_PATTERN_FLAT:
        brushes.clear();
        brushes.resize(1);

        brushes[0] = std::make_shared<graphics::brush::LinearGradientBrush>();
        brushes[0]->SetGradientDirection(graphics::GRADIENT_DIRECTION_TOP_BOTTOM);
        brushes[0]->AddGradientStop(
            D2D1::GradientStop(0.0, color.ToD2D1_COLOR_F()));
        brushes[0]->AddGradientStop(
            D2D1::GradientStop(1.0, color.ToD2D1_COLOR_F()));

        break;

    case COLOR_PATTERN_GRADIENT:
        brushes.clear();
        brushes.resize(1);

        brushes[0] = std::make_shared<graphics::brush::LinearGradientBrush>();
        brushes[0]->SetGradientDirection(graphics::GRADIENT_DIRECTION_TOP_BOTTOM);
        brushes[0]->AddGradientStop(
            D2D1::GradientStop(0.0, color.CloneAndBrighter(0.4f).ToD2D1_COLOR_F()));
        brushes[0]->AddGradientStop(
            D2D1::GradientStop(1.0, color.CloneAndDarker(0.4f).ToD2D1_COLOR_F()));

        break;

    case COLOR_PATTERN_CONVEX:
        brushes.clear();
        brushes.resize(2);

        brushes[0] = std::make_shared<graphics::brush::LinearGradientBrush>();
        brushes[0]->SetGradientDirection(graphics::GRADIENT_DIRECTION_TOPLEFT_TO_BOTTOMRIGHT);
        brushes[0]->AddGradientStop(
            D2D1::GradientStop(0.0, color.CloneAndBrighter(0.4f).ToD2D1_COLOR_F()));
        brushes[0]->AddGradientStop(
            D2D1::GradientStop(1.0, color.CloneAndDarker(0.4f).ToD2D1_COLOR_F()));

        brushes[1] = std::make_shared<graphics::brush::LinearGradientBrush>();
        brushes[1]->AddGradientStop(
            D2D1::GradientStop(0.0, color.CloneAndBrighter(0.2f).ToD2D1_COLOR_F()));
        brushes[1]->AddGradientStop(
            D2D1::GradientStop(1.0, color.CloneAndDarker(0.2f).ToD2D1_COLOR_F()));

        break;
    case COLOR_PATTERN_CONCAVE:
        brushes.clear();
        brushes.resize(2);

        brushes[0] = std::make_shared<graphics::brush::LinearGradientBrush>();
        brushes[0]->SetGradientDirection(graphics::GRADIENT_DIRECTION_TOPLEFT_TO_BOTTOMRIGHT);
        brushes[0]->AddGradientStop(
            D2D1::GradientStop(0.0, color.CloneAndDarker(0.4f).ToD2D1_COLOR_F()));
        brushes[0]->AddGradientStop(
            D2D1::GradientStop(1.0, color.CloneAndBrighter(0.4f).ToD2D1_COLOR_F()));

        brushes[1] = std::make_shared<graphics::brush::LinearGradientBrush>();
        brushes[1]->AddGradientStop(
            D2D1::GradientStop(0.0, color.CloneAndDarker(0.2f).ToD2D1_COLOR_F()));
        brushes[1]->AddGradientStop(
            D2D1::GradientStop(1.0, color.CloneAndBrighter(0.2f).ToD2D1_COLOR_F()));

        break;
    case COLOR_PATTERN_CONVEX_FRAME:
        brushes.clear();
        brushes.resize(3);

        brushes[0] = std::make_shared<graphics::brush::LinearGradientBrush>();
        brushes[0]->AddGradientStop(
            D2D1::GradientStop(0.0, color.CloneAndDarker(0.3f).ToD2D1_COLOR_F()));
        brushes[0]->AddGradientStop(
            D2D1::GradientStop(1.0, color.CloneAndDarker(0.3f).ToD2D1_COLOR_F()));

        brushes[1] = std::make_shared<graphics::brush::LinearGradientBrush>();
        brushes[1]->AddGradientStop(
            D2D1::GradientStop(0.0, color.CloneAndBrighter(0.3f).ToD2D1_COLOR_F()));
        brushes[1]->AddGradientStop(
            D2D1::GradientStop(1.0, color.CloneAndBrighter(0.3f).ToD2D1_COLOR_F()));

        brushes[2] = std::make_shared<graphics::brush::LinearGradientBrush>();
        brushes[2]->AddGradientStop(
            D2D1::GradientStop(0.0, color.CloneAndBrighter(0.2f).ToD2D1_COLOR_F()));
        brushes[2]->AddGradientStop(
            D2D1::GradientStop(1.0, color.CloneAndDarker(0.2f).ToD2D1_COLOR_F()));

        break;
    case COLOR_PATTERN_CONCAVE_FRAME:
        brushes.clear();
        brushes.resize(3);

        brushes[0] = std::make_shared<graphics::brush::LinearGradientBrush>();
        brushes[0]->AddGradientStop(
            D2D1::GradientStop(0.0, color.CloneAndBrighter(0.3f).ToD2D1_COLOR_F()));
        brushes[0]->AddGradientStop(
            D2D1::GradientStop(1.0, color.CloneAndBrighter(0.3f).ToD2D1_COLOR_F()));

        brushes[1] = std::make_shared<graphics::brush::LinearGradientBrush>();
        brushes[1]->AddGradientStop(
            D2D1::GradientStop(0.0, color.CloneAndDarker(0.3f).ToD2D1_COLOR_F()));
        brushes[1]->AddGradientStop(
            D2D1::GradientStop(1.0, color.CloneAndDarker(0.3f).ToD2D1_COLOR_F()));

        brushes[2] = std::make_shared<graphics::brush::LinearGradientBrush>();
        brushes[2]->AddGradientStop(
            D2D1::GradientStop(0.0, color.CloneAndBrighter(0.2f).ToD2D1_COLOR_F()));
        brushes[2]->AddGradientStop(
            D2D1::GradientStop(1.0, color.CloneAndDarker(0.2f).ToD2D1_COLOR_F()));

        break;

    case COLOR_PATTERN_FRAME:
        brushes.clear();
        brushes.resize(6);

        brushes[0] = std::make_shared<graphics::brush::LinearGradientBrush>();
        brushes[0]->AddGradientStop(
            D2D1::GradientStop(0.0, color.CloneAndDarker(0.3f).ToD2D1_COLOR_F()));
        brushes[0]->AddGradientStop(
            D2D1::GradientStop(1.0, color.CloneAndDarker(0.3f).ToD2D1_COLOR_F()));

        brushes[1] = std::make_shared<graphics::brush::LinearGradientBrush>();
        brushes[1]->AddGradientStop(
            D2D1::GradientStop(0.0, color.CloneAndBrighter(0.3f).ToD2D1_COLOR_F()));
        brushes[1]->AddGradientStop(
            D2D1::GradientStop(1.0, color.CloneAndBrighter(0.3f).ToD2D1_COLOR_F()));

        brushes[2] = std::make_shared<graphics::brush::LinearGradientBrush>();
        brushes[2]->AddGradientStop(
            D2D1::GradientStop(0.0, color.ToD2D1_COLOR_F()));
        brushes[2]->AddGradientStop(
            D2D1::GradientStop(1.0, color.ToD2D1_COLOR_F()));

        brushes[3] = std::make_shared<graphics::brush::LinearGradientBrush>();
        brushes[3]->AddGradientStop(
            D2D1::GradientStop(0.0, color.CloneAndBrighter(0.3f).ToD2D1_COLOR_F()));
        brushes[3]->AddGradientStop(
            D2D1::GradientStop(1.0, color.CloneAndBrighter(0.3f).ToD2D1_COLOR_F()));

        brushes[4] = std::make_shared<graphics::brush::LinearGradientBrush>();
        brushes[4]->AddGradientStop(
            D2D1::GradientStop(0.0, color.CloneAndDarker(0.3f).ToD2D1_COLOR_F()));
        brushes[4]->AddGradientStop(
            D2D1::GradientStop(1.0, color.CloneAndDarker(0.3f).ToD2D1_COLOR_F()));

        brushes[5] = std::make_shared<graphics::brush::LinearGradientBrush>();
        brushes[5]->AddGradientStop(
            D2D1::GradientStop(0.0, color.CloneAndBrighter(0.2f).ToD2D1_COLOR_F()));
        brushes[5]->AddGradientStop(
            D2D1::GradientStop(1.0, color.CloneAndDarker(0.2f).ToD2D1_COLOR_F()));

        break;
    case COLOR_PATTERN_CONVEX_METAL:
        brushes.clear();
        brushes.resize(3);

        brushes[0] = std::make_shared<graphics::brush::LinearGradientBrush>();
        brushes[0]->AddGradientStop(
            D2D1::GradientStop(0.0, color.CloneAndDarker(0.3f).ToD2D1_COLOR_F()));
        brushes[0]->AddGradientStop(
            D2D1::GradientStop(1.0, color.CloneAndDarker(0.3f).ToD2D1_COLOR_F()));

        brushes[1] = std::make_shared<graphics::brush::LinearGradientBrush>();
        brushes[1]->AddGradientStop(
            D2D1::GradientStop(0.0, color.CloneAndBrighter(0.3f).ToD2D1_COLOR_F()));
        brushes[1]->AddGradientStop(
            D2D1::GradientStop(1.0, color.CloneAndBrighter(0.3f).ToD2D1_COLOR_F()));

        brushes[2] = std::make_shared<graphics::brush::LinearGradientBrush>();
        brushes[2]->AddGradientStop(
            D2D1::GradientStop(0.0, color.CloneAndChangeBrightness(0.8f).ToD2D1_COLOR_F()));
        brushes[2]->AddGradientStop(
            D2D1::GradientStop(0.5, color.CloneAndChangeBrightness(0.55f).ToD2D1_COLOR_F()));
        brushes[2]->AddGradientStop(
            D2D1::GradientStop(0.5, color.CloneAndChangeBrightness(0.45f).ToD2D1_COLOR_F()));
        brushes[2]->AddGradientStop(
            D2D1::GradientStop(1.0, color.CloneAndChangeBrightness(0.2f).ToD2D1_COLOR_F()));

        break;

    case COLOR_PATTERN_CONCAVE_METAL:
        brushes.clear();
        brushes.resize(3);

        brushes[0] = std::make_shared<graphics::brush::LinearGradientBrush>();
        brushes[0]->AddGradientStop(
            D2D1::GradientStop(0.0, color.CloneAndBrighter(0.3f).ToD2D1_COLOR_F()));
        brushes[0]->AddGradientStop(
            D2D1::GradientStop(1.0, color.CloneAndBrighter(0.3f).ToD2D1_COLOR_F()));

        brushes[1] = std::make_shared<graphics::brush::LinearGradientBrush>();
        brushes[1]->AddGradientStop(
            D2D1::GradientStop(0.0, color.CloneAndDarker(0.3f).ToD2D1_COLOR_F()));
        brushes[1]->AddGradientStop(
            D2D1::GradientStop(1.0, color.CloneAndDarker(0.3f).ToD2D1_COLOR_F()));

        brushes[2] = std::make_shared<graphics::brush::LinearGradientBrush>();
        brushes[2]->AddGradientStop(
            D2D1::GradientStop(0.0, color.CloneAndChangeBrightness(0.8f).ToD2D1_COLOR_F()));
        brushes[2]->AddGradientStop(
            D2D1::GradientStop(0.5, color.CloneAndChangeBrightness(0.55f).ToD2D1_COLOR_F()));
        brushes[2]->AddGradientStop(
            D2D1::GradientStop(0.5, color.CloneAndChangeBrightness(0.45f).ToD2D1_COLOR_F()));
        brushes[2]->AddGradientStop(
            D2D1::GradientStop(1.0, color.CloneAndChangeBrightness(0.2f).ToD2D1_COLOR_F()));

        break;
    }
}

void graphics::color::ColorPatternUtil::GeneratePattern(
    IN COLOR_PATTERN colorPattern,
    IN const graphics::color::ColorValue& color,
    OUT std::shared_ptr<graphics::color::ColorSet> colorSet)
{
    colorSet->Clear();

    switch (colorPattern) {
    case COLOR_PATTERN_FLAT: {
        colorSet->AddColor(std::make_shared<graphics::color::SolidColor>(color));
    }
    break;

    case COLOR_PATTERN_GRADIENT: {
        auto linearGradientColor =
            std::make_shared<graphics::color::LinearGradientColor>();
        linearGradientColor->SetGradientDirection(graphics::GRADIENT_DIRECTION_TOP_BOTTOM);
        graphics::color::GradientStopCollection collection;
        collection.AddGradientStop(graphics::color::GradientStop(0.0f, color.CloneAndBrighter(0.4f)));
        collection.AddGradientStop(graphics::color::GradientStop(1.0f, color.CloneAndDarker(0.4f)));
        linearGradientColor->SetColor(collection);
        colorSet->AddColor(linearGradientColor);
    }
    break;

    case COLOR_PATTERN_CONVEX: {
        auto linearGradientColor1 =
            std::make_shared<graphics::color::LinearGradientColor>();
        linearGradientColor1->SetGradientDirection(graphics::GRADIENT_DIRECTION_TOP_BOTTOM);
        graphics::color::GradientStopCollection collection1;
        collection1.AddGradientStop(graphics::color::GradientStop(0.0f, color.CloneAndBrighter(0.4f)));
        collection1.AddGradientStop(graphics::color::GradientStop(1.0f, color.CloneAndDarker(0.4f)));
        linearGradientColor1->SetColor(collection1);
        colorSet->AddColor(linearGradientColor1);

        auto linearGradientColor2 =
            std::make_shared<graphics::color::LinearGradientColor>();
        linearGradientColor2->SetGradientDirection(graphics::GRADIENT_DIRECTION_TOP_BOTTOM);
        graphics::color::GradientStopCollection collection2;
        collection2.AddGradientStop(graphics::color::GradientStop(0.0f, color.CloneAndBrighter(0.2f)));
        collection2.AddGradientStop(graphics::color::GradientStop(1.0f, color.CloneAndDarker(0.2f)));
        linearGradientColor2->SetColor(collection2);
        colorSet->AddColor(linearGradientColor2);
    }
    break;

    case COLOR_PATTERN_CONCAVE: {
        auto linearGradientColor1 =
            std::make_shared<graphics::color::LinearGradientColor>();
        linearGradientColor1->SetGradientDirection(graphics::GRADIENT_DIRECTION_TOP_BOTTOM);
        graphics::color::GradientStopCollection collection1;
        collection1.AddGradientStop(graphics::color::GradientStop(0.0f, color.CloneAndDarker(0.4f)));
        collection1.AddGradientStop(graphics::color::GradientStop(1.0f, color.CloneAndBrighter(0.4f)));
        linearGradientColor1->SetColor(collection1);
        colorSet->AddColor(linearGradientColor1);

        auto linearGradientColor2 =
            std::make_shared<graphics::color::LinearGradientColor>();
        linearGradientColor2->SetGradientDirection(graphics::GRADIENT_DIRECTION_TOP_BOTTOM);
        graphics::color::GradientStopCollection collection2;
        collection2.AddGradientStop(graphics::color::GradientStop(0.0f, color.CloneAndDarker(0.2f)));
        collection2.AddGradientStop(graphics::color::GradientStop(1.0f, color.CloneAndBrighter(0.2f)));
        linearGradientColor2->SetColor(collection2);
        colorSet->AddColor(linearGradientColor2);
    }
    break;

    case COLOR_PATTERN_CONVEX_FRAME: {
        auto solidColor1 =
            std::make_shared<graphics::color::SolidColor>(color.CloneAndDarker(0.3f));
        colorSet->AddColor(solidColor1);

        auto solidColor2 =
            std::make_shared<graphics::color::SolidColor>(color.CloneAndBrighter(0.3f));
        colorSet->AddColor(solidColor2);

        auto linearGradientColor1 =
            std::make_shared<graphics::color::LinearGradientColor>();
        linearGradientColor1->SetGradientDirection(graphics::GRADIENT_DIRECTION_TOP_BOTTOM);
        graphics::color::GradientStopCollection collection;
        collection.AddGradientStop(graphics::color::GradientStop(0.0f, color.CloneAndBrighter(0.2f)));
        collection.AddGradientStop(graphics::color::GradientStop(1.0f, color.CloneAndDarker(0.2f)));
        linearGradientColor1->SetColor(collection);
        colorSet->AddColor(linearGradientColor1);
    }
    break;
    case COLOR_PATTERN_CONCAVE_FRAME: {
        auto solidColor1 =
            std::make_shared<graphics::color::SolidColor>(color.CloneAndBrighter(0.3f));
        colorSet->AddColor(solidColor1);

        auto solidColor2 =
            std::make_shared<graphics::color::SolidColor>(color.CloneAndDarker(0.3f));
        colorSet->AddColor(solidColor2);

        auto linearGradientColor1 =
            std::make_shared<graphics::color::LinearGradientColor>();
        linearGradientColor1->SetGradientDirection(graphics::GRADIENT_DIRECTION_TOP_BOTTOM);
        graphics::color::GradientStopCollection collection;
        collection.AddGradientStop(graphics::color::GradientStop(0.0f, color.CloneAndBrighter(0.2f)));
        collection.AddGradientStop(graphics::color::GradientStop(1.0f, color.CloneAndDarker(0.2f)));
        linearGradientColor1->SetColor(collection);
        colorSet->AddColor(linearGradientColor1);
    }
    break;
    case COLOR_PATTERN_FRAME: {
        auto solidColor1 =
            std::make_shared<graphics::color::SolidColor>(color.CloneAndDarker(0.3f));
        colorSet->AddColor(solidColor1);

        auto solidColor2 =
            std::make_shared<graphics::color::SolidColor>(color.CloneAndBrighter(0.3f));
        colorSet->AddColor(solidColor2);

        auto solidColor3 =
            std::make_shared<graphics::color::SolidColor>(color);
        colorSet->AddColor(solidColor3);

        auto solidColor4 =
            std::make_shared<graphics::color::SolidColor>(color.CloneAndBrighter(0.3f));
        colorSet->AddColor(solidColor4);

        auto solidColor5 =
            std::make_shared<graphics::color::SolidColor>(color.CloneAndDarker(0.3f));
        colorSet->AddColor(solidColor5);

        auto linearGradientColor1 =
            std::make_shared<graphics::color::LinearGradientColor>();
        linearGradientColor1->SetGradientDirection(graphics::GRADIENT_DIRECTION_TOP_BOTTOM);
        graphics::color::GradientStopCollection collection;
        collection.AddGradientStop(graphics::color::GradientStop(0.0f, color.CloneAndBrighter(0.2f)));
        collection.AddGradientStop(graphics::color::GradientStop(1.0f, color.CloneAndDarker(0.2f)));
        linearGradientColor1->SetColor(collection);
        colorSet->AddColor(linearGradientColor1);
    }
    break;
    case COLOR_PATTERN_CONVEX_METAL: {
        auto solidColor1 =
            std::make_shared<graphics::color::SolidColor>(color.CloneAndDarker(0.3f));
        colorSet->AddColor(solidColor1);

        auto solidColor2 =
            std::make_shared<graphics::color::SolidColor>(color.CloneAndBrighter(0.3f));
        colorSet->AddColor(solidColor2);

        auto linearGradientColor1 =
            std::make_shared<graphics::color::LinearGradientColor>();
        linearGradientColor1->SetGradientDirection(graphics::GRADIENT_DIRECTION_TOP_BOTTOM);
        graphics::color::GradientStopCollection collection;
        collection.AddGradientStop(graphics::color::GradientStop(0.0f, color.CloneAndChangeBrightness(0.8f)));
        collection.AddGradientStop(graphics::color::GradientStop(0.5f, color.CloneAndChangeBrightness(0.55f)));
        collection.AddGradientStop(graphics::color::GradientStop(0.5f, color.CloneAndChangeBrightness(0.45f)));
        collection.AddGradientStop(graphics::color::GradientStop(1.0f, color.CloneAndChangeBrightness(0.2f)));
        linearGradientColor1->SetColor(collection);
        colorSet->AddColor(linearGradientColor1);
    }
    break;
    case COLOR_PATTERN_CONCAVE_METAL: {
        auto solidColor1 =
            std::make_shared<graphics::color::SolidColor>(color.CloneAndBrighter(0.3f));
        colorSet->AddColor(solidColor1);

        auto solidColor2 =
            std::make_shared<graphics::color::SolidColor>(color.CloneAndDarker(0.3f));
        colorSet->AddColor(solidColor2);

        auto linearGradientColor1 =
            std::make_shared<graphics::color::LinearGradientColor>();
        linearGradientColor1->SetGradientDirection(graphics::GRADIENT_DIRECTION_TOP_BOTTOM);
        graphics::color::GradientStopCollection collection;
        collection.AddGradientStop(graphics::color::GradientStop(0.0f, color.CloneAndChangeBrightness(0.8f)));
        collection.AddGradientStop(graphics::color::GradientStop(0.5f, color.CloneAndChangeBrightness(0.55f)));
        collection.AddGradientStop(graphics::color::GradientStop(0.5f, color.CloneAndChangeBrightness(0.45f)));
        collection.AddGradientStop(graphics::color::GradientStop(1.0f, color.CloneAndChangeBrightness(0.2f)));
        linearGradientColor1->SetColor(collection);
        colorSet->AddColor(linearGradientColor1);
    }
    break;
    }
}
